/*================================================================================================

              MAC TRAFFIC VOLUME MEASUREMENTS

GENERAL DESCRIPTION

This module contains functions to

- Process config requests from RRC (setup, modify, delete, delete_all).
- Handle the 10ms timer for traffic volume reporting.
- Handle periodic and event triggered mode.
- Compute  raw rlc payload, average rlc payload, variance rlc payload.
- Compute transport channel traffic volume.
- Block DTCH transmissions on RACH


Copyright (c) 2001,2002 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2003-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

================================================================================================*/

/*================================================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mac/vcs/mactraffic.c_v   1.4   18 Jul 2002 14:00:18   skrishna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/src/mactraffic.c#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when      who   what, where, why
--------  ---   ----------------------------------------------------------------------------------
05/10/16   as   Remove excessive F3s
07/26/14   rkmk    Reduce image size by using right macro depending upon number of parameters
05/03/14   geg  Use safer versions of memcpy() and memmove()
04/11/14   scv  ULC: Fix for HSRACH TVM reporting, and HSRACH large ping crash
04/04/14   scv  Code changes for UL Compression feature
12/09/13   as   Added code change not to disable RACH if there is any pending data in DL 
                that needs to be received which in turn would not trigger EDRX full sleep.
11/27/13   scv  Code changes for UL Compression feature
11/20/13   kv   Moved mask for MSG_4to9 from MSG_SSID_DIAG to MSG_SSID_WCDMA_MAC
11/12/13   kv   Removed deprecated header file, tmc.h 
07/22/13   kc   New MRAB API - change to save CS call where PS RAB release is colliding with RLC RESET.
05/28/13   kc   Change for MRM-ID extension for Rel-9 
04/29/13   kv   Moved F3s under debug flag & added QXDM support for tvm_debug
02/21/13   kc   Fix to add RB ID only if TVM reporting quantities are configured
01/30/13   scv  Fixed KW Errors
01/23/13   kc   Fix to handle meas objs in any order when HSRACH is configured
01/17/13   as   Removed unwanted F3's.
01/08/13   kc   API to provide validity of measurement ID
01/04/13   kc   Removing duplicate/redundant code
12/03/12   mrg  Backing out TVM recent changes for CR-393288 and CR421950.
11/26/12   mrg  Trigger 4A/4B reports(Don't skip) eventhough no IE given for BO quantity reporting
11/7/12    ts   Fixed KW errors
10/11/12   mrg  TVM optimization for MRAB
09/07/12   mrg  TVM Events not triggered after Prohibit Timer expriy and 4a/4b condition satisfied
09/05/12   kc   Post a MR to RRC only if there is a valid RB for which results have to be reported for Periodic case 
08/13/12   mrg  HS-RACH TVM issues (Multiple TVMs sent , 4A/4B back to backcontinous reported)
                 1)Multiple unnecessary Events 4A-4B triggered are avoidede now
                 2)When Prohibit timer is running Timer_To_Trigger is not running – So it will delay TVM event triggering by PTT+TTT -fixed
                 3)During DCH<->RACH/FACH transition TRCH are added with our checking for TRCH type 
07/24/12   geg  Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
03/28/12   zr   Adding feature definitions for Dime
06/19/12   mrg  HS-RACH & MAC-I Feature code integrated
02/28/12   grk  Restricting R99 ISR lock to BO function call only during TVMM calculations.
01/02/12   grk  Feature cleanup
10/06/11   grk  Fixed KW errors
10/06/11   grk  Fixed KW errors
08/09/11   kc   Added check for TVM configuration before accessing TVM pointers
08/26/11   grk  Fix for KW errors
02/28/11   sv   Support for transition from TMC heap to Modem Heap in WCDMA.
11/10/10   mrg  Lint error fixed
10/21/10   mrg  Updated logics to handle Mode switch (Periodic <-> Event) for MAC_TRAFFIC_MEAS_MODIFY request
10/01/10   mrg  TVM event and periodic measurements handled with different timers
10/11/09   mrg  Fix to accept MCM if received while on cell DCH state and
                 validity is set to UE_State  = all-But-Cell-DCH.
10/07/09   mrg  Fixed Klocwork warnings
07/06/09   av   R99 & L2 Mutex cleanup
05/15/09  grk   Added changes for R99 mutex.
05/07/09  ssg   Updated Copyright Information
04/29/09   av   Fixed compiler warnings in L2 
01/11/08  mrg   Added mac_compute_traffic_volume_above_threshold(), compute_obj_list_raw_payload()
                and proc_event_trigger_criteria_cu() functions for FEATURE_TVM_IND_IN_CU.
                For Overwriting RACH TVM paramaeters with TVM MCM update request,
                modifying IF-condition in check_n_copy_modify_event_trigger_params()
01/30/08  mrg   To Properly updating TVM info Reporting Criteria for RACH,modifying 
                IF-condition in check_n_copy_modify_event_trigger_params()          
01/02/08   sk   Fixed compiler warnings generated due to the new compiler.
09/05/07   sk   Used the rlc_ul_enh_buffer_status fn under FEATURE_RLC_ENH_BUFFER_STATUS
08/05/07   sk   Added headers to the retransmission pdus.
07/23/07   sk   Added changes to include the rlc headers as part of the buffer status for TVM reporting
02/04/07   sk   Klocwork fix
08/14/06   sk   Added HSUPA support
03/12/06   sk   Removed evt_first_meas flag. Not being used anywhere.
01/20/06   sk   COpied the report mode in check_n_modify_modify_params()
07/21/05   gsc  Fixed lint error(s)
05/20/05   sk   Fixed compilation error in code added in prev rev (#41).
05/18/05   sk   In check_meas_report_qty() function, accept the configuration for periodic
                mode even when nothing is being asked to report. This was needed for passing
                a RF GCF testcase.
05/05/05   sk   In prepare_n_send_traffic_volume_measurement_report, include the RB's only
                if RLC payload, average or variance has been asked to report. Otherwise juts
                send the measurement report.
04/28/05   sk   Set block_transmission to FALSE to mac_ul_traffic_volume_init,
                mac_ul_delete_traffic_volume_meas, mac_ul_delete_all_traffic_volume_meas.
03/09/05   sk   Fixed all lint errors.
02/09/05   sk   Fix for critical lint errors.
12/08/04   sk   Fixed compiler warnings.
09/20/04   sk   While sending the report do not include the RB's whose mode is Transparent.
09/13/04   sk   For the tx interruption timer, if tx timer was running previously because of
                some earlier event 4a and presently if there is another event 4a generated, restart
                the tx interrupt timer with the new value only if the newer event's tx interrupt value
                is smaller than the remaining time for the earlier tx interrupt timer value otherwise
                let the earlier timer run to completion.
08/24/04   sk   proc_event_trigger_criteria: Let the pending time after trigger continue to run during
                the time when txInterruptionAfetrTrigger timer is running also.
                check_n_copy_modify_event_trigger params, take care of the case when RRC configures
                on modify command adds a new event to an exisiting transport channel which
                was originally in setup.
08/18/04   sk   Reset the timer counters on a state transition (DCH->FACH or vice versa). This will
                be indicated to MAC by RRC using the RESUME and SAVE command. And consider beginning
                a new measurement after a state transition. Hence need not have to cross the threshold
                at that instant and if they stay below threshold (for 4b) and above threshold for 4a
                an event can be triggered.
07/23/04   sk   Added evt_not_first_meas to be associated with each transport channel for which measurements
                are configured.Fixes CR 34598
07/23/04   sk   Added a new function mac_ul_update_trch_info(). This will update the transport channel
                list whenever TVM is confgiured and when measurements needs to be done for all uplink
                transport channels that are active.
06/18/04   sk   In mac_ul_traffic_volume_init, define the Traffic volume timer. Setting the timer
                will be done when ther is atleast one measurement to make.
04/30/04   sk   The timer is started when there is atleast one event for which the validity is the
                same as the  state that UE is currently in. Added a boolean mac_tvm_timer_started to
                keep track of the timer started status. The num_measurements field is also apporpriately
                adjusted when a new measurement is added or deleted.
04/19/04   sk   In populate_rach_trch_id() function, returned the boolean status.
03/16/04   sk   Added meas_start_incl to take care of the validity sent on meas crl messages. Added
                checks on the loop conditions as part of defensive coding.
03/16/04   sk   In prepare_n_send_measurement_reports() : removed check for the count of number of
                RB's. This will allow MAC to send blank reports to RRC.
02/13/04   sk   Added prepare_addtl_meas_incl to ensure that measurement reports are not sent in
                duplicate when tvm is appended as an additional measurement to intra-freq or other
                measurements.
02/11/04   sk   Copied the number of events parameter when MAC recieves a modify command
02/04/03   sk   Corrected the handling of MCM with modify. Fixes CR: 38817
09/26/03   vk   Defined function mac_reset_periodic_event_trigger_counters(..) that clears timers
                on state transition
08/28/03   sk   proc_event_trigger_criteria() : Set the evt_first_meas boolean to false.
07/24/03   sk   In ppopulate_rach_trch_id(), populate rach id for event transport channels
                should be done only for event triggered mode.
06/26/03   sk   In proc_event_trigger_criteria, set evt_first_meas to FALSE after transmission of
                the first event report. In populate_rach_id() : set evet_first_meas to TRUE to indicate
                that this is the first measurement for the RACH transport channel.
05/29/04  sk    mac_get_additional_traffic_vol_msmt_results() function added to supportadditional
                measurements.
03/29/04  sk    In mac_ul_handle_traffic_volume_tmr():   Set the 10ms timer tick again for
                INVALID_UL_CCTRCH too.
02/15/03  sk    Initiliazed the rlc_payload, variance_payload, average_payload at the beginning of
                the function in prepare_n_send_traffic_volume_report().
02/13/03  sk    1. prepare_n_send_traffic_volume_report : Set the average, variance, raw to 0xFFFFFFFF
                when the measurement report quantity is set to FALSE.
                2. check_n_copy_event_trigger_criteria : Copied the number of events separately for
                each all the transport channels specified in the event transport channel list.
                This will take care of the case when both DCH and RACH are specified.
01/08/03  sk    mac_ul_setup_traffic_volume_meas():
                Set suspend_meas to FALSE when setting up traffic volume measurements.
                mac_ul_resume_traffic_volume_meas():
                Set the traffic volume timer ticking again as we resume measurements.
11/15/02  sk    Removed function check_meas_obj_list. not MAC will be able to handle both DCH and RACH
                when specidifed in the object list.Also sup[port for traffic volume report log packet.
11/13/02  sk    Moved populating the meas object list to check_modify_params() in the case
                when MAC gets a modify command from RRC.
10/07/02  sk    check_meas_obj_list(): Can handle measurement requests for different types of
                transport channels (RACH & DCH). Store only the transport channel related
                information for the current cctrch type.
10/10/02  sk    Initialized the variables when we sav ethe measurement configuration for a id.
10/04/02  sk    Handled the case when MAC can receive more than one setup command for the same
                measurement id.
09/25/02  sk    In the case of event trigger criteria, trigger the report when the current buffer
                occupancy is greater than threshold immediately after configuring. & also after the
                pending time after trigger expires.Took care of some border cases when the report
                interval is less than the measurement interval. Added code for handling RESUME & SAVE
                command
08/16/02  sk    check_n_copy_event_trigger_params(): In the case of RACH populate the transport
                channel id.
07/18/02  sk    maci_post_traffic_volume_meas_report(): Added FEATURE_TRAFFIC_VOLUME_MEAS.
07/16/02  sk    maci_post_cfg_cnf_msg(): Added  FEATURE_TRAFFIC_VOLUME_MEAS. Note: This will eb
                removed once RRC removes this feature definition.
06/08/02  sk    Added proc_block_dtch_tx_on_rach() to handle block command. Removed
                FEATURE_TRAFFIC_VOLUME_MEAS
05/20/02  sk    Added functionality to distinguish a config complete message(Measurement Control
                Message) from one which is incomplete(recvd from SIB 11/12).
                prepare_n_send_traffic_meas(): copied the event results in the case of
                event triggered criteria to meas_ind which is sent RRC.
                proc_event_trigger_criteria: Took care of additional cases when the
                event can't be triggered setting the send_status = FALSE;
05/01/02  sk    Created the file. This file contains functions implementing the traffic volume
                measurements. This feature is defined under #ifdef FEATURE_TRAFFIC_VOLUME_MEAS

================================================================================================*/
/*================================================================================================
                     INCLUDE FILES
================================================================================================*/
#include "wcdma_variation.h"
#include  "customer.h"
#include  "rex.h"
#include  "msg.h"
#include  "task.h"
#include  "macrrcif_v.h"
#include  "macinternal.h"
#include  "l2ultask.h"
#include  "mactraffic.h"
#include  "rrccmd_v.h"
#include  "macul.h"
#include  <string.h>
#include "macrlcif.h"

#include "l1mathutil.h"

#include "macullog.h"

#include "maculsubtask.h"

#ifdef FEATURE_WCDMA_HS_RACH
#include "maculhs.h"
#include "macrach.h"
#endif


/*================================================================================================
                         D A T A    D E C L A R A T I O N S
================================================================================================*/

#ifdef FEATURE_WCDMA_MRAB_TVM_MAC_OPT
#error code not present
#endif /* FEATURE_WCDMA_MRAB_TVM_MAC_OPT */


/*The number of traffic volume measurements so far */
static uint16  num_event_measurements=0;
static uint16  num_periodic_measurements = 0;


static boolean mac_tvm_event_timer_started = FALSE;
static boolean mac_tvm_periodic_timer_started = FALSE;


/*This is the index at which the raw_rlc_payload information for the rb's */
/* in the current timeout would be placed */
static uint16  cur_index=0;

/* Traffic volume measurement Config table */
maci_ul_traffic_cfg_index_type   ul_traffic_info_table[UE_NUM_TRAFFIC_MEAS];

/* Traffic volume timer*/
rex_timer_type                   mac_ul_tvm_event_tmr_blk;
rex_timer_type                   mac_ul_tvm_periodic_tmr_blk;

uint16                           tvm_debug = 0;


/* UL Buffer status obtained from RLC */
rlc_ul_buf_status_type           traffic_ul_buf_status;

/* This array contains the raw payload information for all the radio bearers*/
uint32                           rb_raw_payload[MAX_RB][TRAFFIC_MAX_SAMPLES];

/* TRUE indicates that the Tx interruption timer is active currently. FALSE*/
/* indicates that the DTCH transmissions on RACH are not blocked now.*/
boolean                          block_transmission=FALSE;

/* Measurement id which caused the blocking of DTCH transmissions on RACH*/
uint16                           tx_interrupt_tmr;

/* Counter to keep track of the Tx interruption timer */
uint16                           tx_interrupt_cnt;

mac_ul_traffic_trch_info_type    evt_trch_info[UE_MAX_TRCH];

/* This will indicate if the circular buffer has rolled over */
boolean                          rolled_over = FALSE;

/* This is the first measurement */
//boolean                          evt_first_meas;

/* This structure would contain the results of theresults for a particular
measurement identity at a time */
mac_ul_traffic_meas_ind_type    meas_ind;

boolean prepare_addtl_meas_incl = FALSE;

/*---------------------------------------------------------------------------
  Contains the list of logical channels for which MAC needs the buffer status
---------------------------------------------------------------------------*/
extern rlc_ul_lc_info_type              mac_ul_lc_info;

// This indicates the minimum transport block size for
// each transport channel. THis will be used for computing
// the exact buffer status for UM for traffic volume
// measurements. In case of UM, the PDU sizes can vary. We
// will pick up the minimum size corresponding to the
// transport channel to which the LC is mapped to and use the
// minimum TB size for computation of headers.
// For AM PDU size is fixed and is configured by RRC to RLC
// Hence this field will NOT be used for AM. TVM is not done
// for TM channels.
extern uint32  trch_min_tb_size[UE_MAX_TRCH];

/* Uplink RLC buffer status buffer */
//extern rlc_ul_buf_status_type    ul_buf_status;


/*===============================================================================

FUNCTION MAC_UL_TRAFFIC_VOLUME_INIT

DESCRIPTION Initialise the measurement id config table, intermediate measurements
table, buffer status and the raw payload information.
====================================================================================*/

void mac_ul_traffic_volume_init (void)
{
  uint16 i;

  for (i=0;i<UE_NUM_TRAFFIC_MEAS; i++)
  {
    /*-------------------------------------------------------------------------------
    Initialize the uplink traffic volume table. this table stores the config
    information and the intermediate measurement results. "valid" flag is set
    to FALSE to indicate that there are no valid information available now.
    -------------------------------------------------------------------------------*/
    ul_traffic_info_table[i].valid=FALSE;
    ul_traffic_info_table[i].traffic_cfg_ptr = NULL;
  }

  /*---------------------------------------------------------------------------------
  Initalize the traffic_ul_buf_status structure. Every 10ms the RLC buffer status is
  obtained in traffic_ul_buf_status.
  ---------------------------------------------------------------------------------*/
  memset(&traffic_ul_buf_status,0, sizeof(rlc_ul_buf_status_type));

  tx_interrupt_tmr = 0;
  tx_interrupt_cnt = 0;
  num_event_measurements = 0;
  num_periodic_measurements = 0;
  //evt_first_meas = TRUE;
  mac_tvm_event_timer_started = FALSE;
  mac_tvm_periodic_timer_started = FALSE;
  block_transmission = FALSE;

  /*---------------------------------------------------------------------------------
  Initalize the radio bearer raw payload information.
  ---------------------------------------------------------------------------------*/
  for (i=0;i<MAX_RB;i++)
  {
    /*------------------------------------------------------------------------------
    rb_raw_payload is an array of 26 elements(maximum measurement interval is 260ms)
    and is maintained for every radio bearer (1..32).
    ------------------------------------------------------------------------------*/
    memset(rb_raw_payload[i],0,TRAFFIC_MAX_SAMPLES*sizeof(uint32));
  }

  // Define the traffic volume timer.
  rex_def_timer(&mac_ul_tvm_event_tmr_blk,    wcdma_l2_ul_get_tcb(), MAC_UL_TVM_EVENT_TMR_EXP_SIG);
  rex_def_timer(&mac_ul_tvm_periodic_tmr_blk, wcdma_l2_ul_get_tcb(), MAC_UL_TVM_PERIODIC_TMR_EXP_SIG);

}/* mac_ul_traffic_volume_init */


/*===================================================================================
 FUNCTION NAME  mac_reset_periodic_event_timer_counters

 PURPOSE    This function clears the periodic timer, event triggered related timers
 when a state transition occurs.
===================================================================================*/
void mac_reset_periodic_event_trigger_counters (void)
{
  maci_ul_traffic_cfg_index_type     *int_cfg_param_ptr;
  maci_ul_traffic_cfg_param          *traffic_cfg_ptr;
  uint8                              i, j, k;

  /* Indicate that this is the first measurement for RACH transport channel */
  //evt_first_meas = TRUE;

  /* If number of measurements is 0 then return */
  if ((num_event_measurements == 0) && (num_periodic_measurements == 0))
  {
    return;
  }
  // Loop through all possible Measurement Id's
  for (i=1;i<=MAC_UL_MAX_MEAS_ID; i++)
  {
    // Get the information pertaining to this measurement id.
    int_cfg_param_ptr = GET_UL_TRAFFIC_CFG_PARAM_PTR(i);

    // If this id has valid parameters in it, then
    if (int_cfg_param_ptr->valid)
    {
      // If non null
      if (int_cfg_param_ptr->traffic_cfg_ptr != NULL)
      {
        traffic_cfg_ptr = int_cfg_param_ptr->traffic_cfg_ptr;

        if (traffic_cfg_ptr->num_trch > UE_MAX_TRCH)
        {
          MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "NumTrch %d > %d",
                                                      traffic_cfg_ptr->num_trch,
                                                      UE_MAX_TRCH);
        }
        else
        {
          // If the report mode is event triggerred then populate rach transport
          // id
          if (traffic_cfg_ptr->report_mode == EVENT_TRIGGERED_MODE)
          {
            // Loop through the transport channels specified in the event criteria
            // information.
            if (traffic_cfg_ptr->event_trch_incl)
            {

              for (j=0;j<traffic_cfg_ptr->event_num_trch
                  && traffic_cfg_ptr->event_num_trch < UE_MAX_TRCH;j++)
              {
                if (traffic_cfg_ptr->num_events[j] > MAX_NUM_EVENTS)
                {
                  MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Number of events %d configured for TrchId %d",
                                                             traffic_cfg_ptr->event_num_trch,
                                                             traffic_cfg_ptr->event_trch_info[j].trch_id);
                }
                else
                {
                  for (k=0; k < traffic_cfg_ptr->num_events[j] && k < MAX_NUM_EVENTS; k++)
                  {
                    traffic_cfg_ptr->trch_event_param[j][k].prohibit_tmr_cnt= 0;
                    traffic_cfg_ptr->trch_event_param[j][k].trigger_tmr_cnt = 0;
                    traffic_cfg_ptr->trch_event_param[j][k].evt_not_first_meas = FALSE;
                    traffic_cfg_ptr->trch_event_param[j][k].first_event_trch_id = 0xFF;
                  }
                }

              } // end for

            } // end if
          } // End of if of report mode
          else if (traffic_cfg_ptr->report_mode == PERIODIC_MODE)
          {
            traffic_cfg_ptr->periodic_tmr_cnt = 0;
          }
          else
          {
            // Do Nothing
          }
        } // End of else
      }//else
    }// cfg ptr not null
  } // Loop through all possible measurement identities.
}



/*===================================================================================
 FUNCTION NAME  maci_post_cfg_cnf_msg()

 PURPOSE    This function posts a config confirmation message to RRC
===================================================================================*/
void maci_post_cfg_cnf_msg
(
/* Acknowledgement number*/
uint16   config_ack_num,

/* Measurement identity for which ack is sent to RRC*/
uint16  meas_id,

/* Indicates if the configuration was succesful or not*/
boolean status
)
{
  rrc_cmd_type   *rrc_cmd_ptr;

  /* Failed to allocate memory for RRC command */
  if ((rrc_cmd_ptr = rrc_get_mac_cmd_buf()) == NULL)
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR , "Could not get RRC cmd buffer");
  }
  /* Send RRC_CMAC_MEAS_CTRL_CNF message to RRC */
  else
  {

    /*Note : This feature definition would be removed only when RRC also removes
    this FEATURE definition */
    /* Fill in command header */
    rrc_cmd_ptr->cmd_hdr.cmd_id = RRC_CMAC_MEAS_CTRL_CNF;

    /* Fill in command data */
    rrc_cmd_ptr->cmd.mac_meas_ctrl_cnf.config_ack_num = config_ack_num;
    rrc_cmd_ptr->cmd.mac_meas_ctrl_cnf.meas_id = meas_id;
    rrc_cmd_ptr->cmd.mac_meas_ctrl_cnf.status = status;

    /* Send command to RRC */
    rrc_put_mac_cmd(rrc_cmd_ptr);

  }

} /*maci_post_cfg_cnf_msg */

/*===================================================================================
 FUNCTION NAME  maci_post_traffic_volume_meas_report

 PURPOSE    This function posts the result of traffic volume measurements for a
            measurement identity.
===================================================================================*/
void maci_post_traffic_volume_meas_report
(
/* Contains the measurement results for a particular measurement id*/
mac_ul_traffic_meas_ind_type  *meas_ind_ptr
)
{
  rrc_cmd_type   *rrc_cmd_ptr;

  /* Failed to allocate memory for RRC command */
  if ((rrc_cmd_ptr = rrc_get_mac_cmd_buf()) == NULL)
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR , "Could not get RRC cmd buffer");
  }
  /* Send RRC_CMAC_UL_TRAFFIC_IND message to RRC */
  else
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "Sending RRC_CMAC_UL_TRAFFIC_IND to RRC");

    /* Fill in command header */
    rrc_cmd_ptr->cmd_hdr.cmd_id = RRC_CMAC_UL_TRAFFIC_IND;
    /* Fill in command data */
    WCDMA_MEMCPY(&rrc_cmd_ptr->cmd.cmac_ul_traffic_ind,
                 sizeof(mac_ul_traffic_meas_ind_type),
                 meas_ind_ptr,
                 sizeof(mac_ul_traffic_meas_ind_type));

    /* Send command to RRC */
    rrc_put_mac_cmd(rrc_cmd_ptr);
  }
} /*maci_post_cfg_cnf_msg */

/*==================================================================================
 FUNCTION NAME  check_meas_report_qty

 PURPOSE    This function checks if measurement interval is specified in the
            case when we are asked to report the average or variance.

==================================================================================*/
boolean check_meas_report_qty
(
/* Configuration parameters recv'd from RRC */
mac_ul_traffic_meas_config_type *oc_traffic_cfg_ptr
)
{
  boolean status_ind=TRUE;

  /* If the measurement report is included */
  if (oc_traffic_cfg_ptr->meas_report_incl)
  {
    /* If MAC has to report either the average or variance*/
    if (oc_traffic_cfg_ptr->meas_report_qty.avg_payload ||
        oc_traffic_cfg_ptr->meas_report_qty.var_payload)
    {
      /* If the measurement quantity information is not included*/
      if (!oc_traffic_cfg_ptr->meas_qty_incl)
      {
        status_ind=FALSE;
        MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Measurement interval not specified");
      }
      else
      {
        /* If the measurement interval is 0, then it is invalid*/
        if (oc_traffic_cfg_ptr->meas_qty.meas_interval/10 ==0)
        {
          status_ind=FALSE;
          MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Measurement Interval not valid");
        }
      }
    }
    /*------------------------------------------------------------------------
      If every value in measurement report quantity is set to FALSE
      then it is an error since we don't have any quantity to report
    ------------------------------------------------------------------------*/
    else if (!oc_traffic_cfg_ptr->meas_report_qty.rlc_payload)
    {
      /*--------------------------------------------------------------------
      In the case of event triggered mode or periodic mode, UE can be asked to
      report only the event results (as 4A, 4B ) and NW might not ask UE to report
      raw, average, or variance. Its allowed as per spec.
      ---------------------------------------------------------------------*/
      status_ind=TRUE;

      MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Report quantity not specified");
    }
  }

  return status_ind;
}
/*==================================================================================
 FUNCTION NAME  check_report_mode

 PURPOSE    This function return TRUE if the report mode is periodic or event
            triggered else returns FALSE

==================================================================================*/
boolean check_report_mode
(
/* report mode */
mac_ul_traffic_report_mode_e_type report_mode
)
{
  boolean status=TRUE;
  /*------------------------------------------------------------------------------
  if the report mode is neither periodic nor event triggered then it is an error.
  ------------------------------------------------------------------------------*/
  if ((report_mode != PERIODIC_MODE)&&(report_mode!=EVENT_TRIGGERED_MODE ))
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Report mode invalid");
    status= FALSE;
  }
  return status;
}
/*==================================================================================
 FUNCTION NAME  populate_object_list

 PURPOSE    This function finds all the uplink currently active transport channels
            and populates the transport channel information array. It returns the number
            of transport channels.
==================================================================================*/
uint16 populate_object_list
(
/* Array of transport channel type and id information*/
mac_ul_traffic_trch_info_type     trch_info[]

)
{
  uint16                  i,j = 0;     /* loop index */
  tr_ch_id_type           trch_id; /*transport channel id */
  trch_id_to_idx_map_type *id_to_idx_ptr; /*Transport channel id to index pointer*/

  
#ifdef FEATURE_WCDMA_HS_RACH
  if(mac_hs_rach_info.hs_rach_active == TRUE)
  {
    /* When HS-RACH is active, single EDCH TrCh is present at MAC */
    /* TrCh ID is not applicable/relevant for EDCH type TrCh - Confirmed with RRC folks */
    trch_info[j].trch_id = 0;
    trch_info[j].trch_type = EDCH;
    j++;
  }
  else
#endif  
  {
    /* Go through all the possible transport channels id */
    for (i=0;i<MAX_TRCH;i++)
    {
      /* The transport channel id ranges from 1 to 32 */
      trch_id = i+MAC_UL_MIN_TRCH_ID;
   
      /* Get the index where transport channel information for this id is stored*/
      id_to_idx_ptr = GET_UL_TRCH_ID_TO_IDX_PTR(trch_id);
   
      /* if the corresponding index has valid parameters */
      if (id_to_idx_ptr->valid)
      {
        if(j < UE_MAX_TRCH)
        {
          /* Store the transport channel id and the type in the appropriate place */
          trch_info[j].trch_id = trch_id;
          trch_info[j].trch_type = GET_UL_CCTRCH_TYPE;   
          /* Increment the count of valid transport channels */
          j++;
        }
        else
        {
          MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC_TVM_ERR: trch_info array out of bounds - j %d",
                                                      j);
          break;
        }
      } /*if transport channel is valid*/
      
    }/*for the number of transport channels */
  }

  /* j keeps a count of the valid transport channels */
  return j;
}

/*==================================================================================
 FUNCTION NAME  populate_rach_trch_id

 PURPOSE: UTRAN can configure traffic volume measurements by specifying both the
 DCH and RACH objects in the measurement object list. In the case when the current ul
 cctrch type is DCH and they have configured measurements for RACH, the trch_id will
 not be available for RACH. Hence this function is called when UE transitions to
 cell_fach state and a RACH id is required to do the measurements.
==================================================================================*/
boolean populate_rach_trch_id(void)
{
  maci_ul_traffic_cfg_index_type     *int_cfg_param_ptr;
  maci_ul_traffic_cfg_param          *traffic_cfg_ptr;
  uint8                              i, j;
  boolean                            status = TRUE;

  /* Indicate that this is the first measurement for RACH transport channel */
  //evt_first_meas = TRUE;
  block_transmission = FALSE;

  /* If number of measurements is 0 then return */
  if ((num_event_measurements == 0) && (num_periodic_measurements == 0))
  {
    return status;
  }

  // Loop through all possible Measurement Id's
  for (i=1;i<=MAC_UL_MAX_MEAS_ID; i++)
  {
    // Get the information pertaining to this measurement id.
    int_cfg_param_ptr = GET_UL_TRAFFIC_CFG_PARAM_PTR(i);

    // If this id has valid parameters in it, then
    if (int_cfg_param_ptr->valid)
    {
      // If non null
      if (int_cfg_param_ptr->traffic_cfg_ptr != NULL)
      {
        traffic_cfg_ptr = int_cfg_param_ptr->traffic_cfg_ptr;

        if (traffic_cfg_ptr->num_trch > UE_MAX_TRCH)
        {
          MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "NumTrch %d > %d",
                                                      traffic_cfg_ptr->num_trch,
                                                      UE_MAX_TRCH);
        }
        else
        {
          // Loop through the number of transport channels
          for (j=0; j<traffic_cfg_ptr->num_trch ; j++)
          {
            // populate the transport channel id for rach
            if (traffic_cfg_ptr->trch_info[j].trch_type == RACH)
            {
              traffic_cfg_ptr->trch_info[j].trch_id = int_ul_trch_info[0].trch_id;
              // There can be only one RACH, hence break after finding the first one.
              break;
            }
          }

          // If the report mode is event triggerred then populate rach transport
          // id
          if (traffic_cfg_ptr->report_mode == EVENT_TRIGGERED_MODE)
          {
            // Loop through the transport channels specified in the event criteria
            // information.
            if (traffic_cfg_ptr->event_num_trch > UE_MAX_TRCH)
            {
              MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Event NumTrch %d > %d",
                                                          traffic_cfg_ptr->event_num_trch,
                                                          UE_MAX_TRCH);

            }
            else
            {
              for (j=0;j<traffic_cfg_ptr->event_num_trch;j++)
              {
                if (traffic_cfg_ptr->event_trch_info[j].trch_type == RACH)
                {
                  traffic_cfg_ptr->event_trch_info[j].trch_id = int_ul_trch_info[0].trch_id;
                  // There can be only one RACH, hence break after finding the first one.
                  break;
                }

              } // end for
            } // else
          } // end if

        }//else

      }// cfg ptr not null
    } // if the measurement id exists.
  }// Loop through all possible measurement identities.

  return status;

}// populate_rach_trch_id()


/*==================================================================================
 FUNCTION NAME  check_n_copy_event_trigger_params

 PURPOSE    This function copies the event trigger parameters for the respective
            transport channels after performing the check to see if the transport
            channels specified are valid. It copies the parameters only if they
            are valid. If the transport channel list is not specified then there
            is only one transport channel specific event criteria specified and
            it is applicable to all the  transport channels as indicated by the
            measurement object list.
==================================================================================*/

boolean check_n_copy_event_trigger_params
(
/* Pointer to Config parameters internally maintained by MAC*/
maci_ul_traffic_cfg_param             *int_tmp_cfg_ptr,

/* Pointer to Config parameters sent by RRC to MAC */
mac_ul_traffic_event_trigger_cfg_type *oc_event_cfg_ptr

)
{
  uint16                                    i,j;
  uint16                                    ntrch=1;
  mac_ul_traffic_trch_event_trigger_cfg     *oc_trch_event_param_ptr;
  boolean                                   event_trch_incl;
  maci_ul_traffic_trch_event_param          *int_trch_param_ptr;
  mac_ul_traffic_trch_event_trigger_param   *oc_trch_cfg_param_ptr;
  boolean                                   status=TRUE;
  ul_trch_info_type                         *trch_ptr;

  /*-------------------------------------------------------------------------------
   True indicates, the event parameters are specified for all transport channels
   as indicated. False implies that only one set of event parameters are specified
   and this is applied to all the uplink transport channels currently active.
  --------------------------------------------------------------------------------*/
  event_trch_incl = oc_event_cfg_ptr->trch_info_incl;

  if (event_trch_incl)
  {
    /*-----------------------------------------------------------------------------
    Copy the number of transport channels for which the event criteria are specified.
    -----------------------------------------------------------------------------*/
    ntrch = oc_event_cfg_ptr->num_trch;
  }
  /*-------------------------------------------------------------------------------
  MAC currently  supports only UE_MAX_TRCH number of transport channels.
  -------------------------------------------------------------------------------*/
  if (ntrch>UE_MAX_TRCH)
  {
    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Currently support %d trch's",
                                                UE_MAX_TRCH);
    status=FALSE;
  }
  else
  {
    /*-------------------------------------------------------------------------------
    The following piece of code checks if the transport channel for which the event
    criteria are specified are valid. Even if one transport channel id or type is
    invalid it breaks out setting status to FALSE.
    --------------------------------------------------------------------------------*/
    /* Go through the transport channels specified */
    for (i=0;i<ntrch;i++)
    {
      oc_trch_event_param_ptr=&oc_event_cfg_ptr->trch_event_cfg[i];

      /* The number of events should be >= 1 */
      if ((oc_trch_event_param_ptr->num_events==0) ||
          (oc_trch_event_param_ptr->num_events > MAX_NUM_EVENTS))
      {
        /*---------------------------------------------------------------------------
         If the number of events is 0 then set the status flag to FALSE
        ---------------------------------------------------------------------------*/
        status=FALSE;
        MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Number of events can't be 0");
        break;
      }

    } /* for number of transport channels */

    /*-------------------------------------------------------------------------------
     We copy the transport channel id & type to local memory only if all the checks
     have been done If the transport channel id's and types are valid
    -------------------------------------------------------------------------------*/
    if (status)
    {
      int_tmp_cfg_ptr->event_trch_incl = event_trch_incl;

      if (event_trch_incl)
      {
        /*---------------------------------------------------------------------------
        Copy the number of transport channels information to the local
        table internally maintained by MAC
       ---------------------------------------------------------------------------*/
        int_tmp_cfg_ptr->event_num_trch = oc_event_cfg_ptr->num_trch;

      }
      else
      {
        /*---------------------------------------------------------------------------
        When the set of transport channels on which an event trigger is not defined
        explicitly, the UE shall apply it on the transport channels specified by the
        measurement object list.
        ---------------------------------------------------------------------------*/

      }

      for (i=0;i<ntrch;i++)
      {
        oc_trch_event_param_ptr=&oc_event_cfg_ptr->trch_event_cfg[i];
        /*---------------------------------------------------------------------------
        Copy the number of events for each of these transport channels
        ---------------------------------------------------------------------------*/
        int_tmp_cfg_ptr->num_events[i] = oc_trch_event_param_ptr->num_events;

        if (event_trch_incl)
        {
          /*------------------------------------------------------------------------
          Copy the transport channel id and the type to the table internally
          maintained by MAC
          -------------------------------------------------------------------------*/
          if (oc_trch_event_param_ptr->trch_info.trch_type==RACH)
          {
            /* Get the transport channel infor pointer */
            trch_ptr = GET_UL_TRCH_INFO_PTR(0);

            /* Store the trch_id along with the config information*/
            int_tmp_cfg_ptr->event_trch_info[i].trch_id = trch_ptr->trch_id;
          }
          else
          {
            int_tmp_cfg_ptr->event_trch_info[i].trch_id=
            oc_trch_event_param_ptr->trch_info.trch_id;
          }

          int_tmp_cfg_ptr->event_trch_info[i].trch_type=
          oc_trch_event_param_ptr->trch_info.trch_type;
        }
        else
        {
          /*---------------------------------------------------------------------------
          When the set of transport channels on which an event trigger is not defined
          explicitly, the UE shall apply it on the transport channels specified by the
          measurement object list.
          ---------------------------------------------------------------------------*/
        }

        int_trch_param_ptr = int_tmp_cfg_ptr->trch_event_param[i];
        oc_trch_cfg_param_ptr = oc_trch_event_param_ptr->event_param;

        /* Copy the event specific parameters */
        for (j=0;j<int_tmp_cfg_ptr->num_events[i];j++)
        {
          /* copy the number of events information */
          int_trch_param_ptr[j].event_id = oc_trch_cfg_param_ptr[j].event_id;

          /* Event is 4A */
          if (int_trch_param_ptr[j].event_id==EVENT_4A)
          {
            /*---------------------------------------------------------------------
             If the event_id is event_4A then initialize the previous buffer
             occupancy to the highest value possible
            ----------------------------------------------------------------------*/
            int_trch_param_ptr[j].prev_bo = 0xFFFF;
            int_trch_param_ptr[j].evt_not_first_meas = FALSE;
            int_trch_param_ptr[j].first_event_trch_id = 0xFF;
          }
          /* if the event is 4B */
          else if (int_trch_param_ptr[j].event_id==EVENT_4B)
          {
            /*---------------------------------------------------------------------
             if the event is 4B then initialze the previous buffer occupancy
             to the lowest value ie 0
            ----------------------------------------------------------------------*/
            int_trch_param_ptr[j].prev_bo = 0;
            int_trch_param_ptr[j].evt_not_first_meas = FALSE;
            int_trch_param_ptr[j].first_event_trch_id = 0xFF;
          }
          else
          {
            MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Event ID not specified for trch %d",
                                                        i);
          }
          /*----------------------------------------------------------------------
          Copy all the timer values froom the config information rec'd from RRC
          ----------------------------------------------------------------------*/
          int_trch_param_ptr[j].trigger_prohibit_tmr =
          oc_trch_cfg_param_ptr[j].trigger_prohibit_tmr;
          int_trch_param_ptr[j].tx_interrupt_tmr =
          oc_trch_cfg_param_ptr[j].tx_interrupt_tmr;
          int_trch_param_ptr[j].time_to_trigger =
          oc_trch_cfg_param_ptr[j].time_to_trigger;

          /*----------------------------------------------------------------------
          Copy the report threshold.
          ----------------------------------------------------------------------*/
          int_trch_param_ptr[j].report_threshold =
          oc_trch_cfg_param_ptr[j].report_threshold;

          /*----------------------------------------------------------------------
            Initialize all the counters. All the timers for traffic volume are
            implemented in the form of counters. There is one base timer which
            expires every 10ms.
          ----------------------------------------------------------------------*/
          int_trch_param_ptr[j].trigger_tmr_cnt=0;
          int_trch_param_ptr[j].prohibit_tmr_cnt=0;

        } /* for number of events*/

      } /*for number of transport channels*/

    }/* if status */

  } /* if ntrch <= UE_MAX_TRCH */

  return status;

}

/*==================================================================================
 FUNCTION NAME  check_n_copy_modify_event_trigger_params

 PURPOSE    This function copies the event trigger parameters based on the
            modified parameters received from UTRAN for the respective
            transport channels after performing the check to see if the transport
            channels specified are valid. It copies the parameters only if they
            are valid. If the transport channel list is not specified then there
            is only one transport channel specific event criteria specified and
            it is applicable to all the  transport channels as indicated by the
            measurement object list.
==================================================================================*/

boolean check_n_copy_modify_event_trigger_params
(
/* Pointer to Config parameters internally maintained by MAC*/
maci_ul_traffic_cfg_param             *int_tmp_cfg_ptr,

/* Pointer to Config parameters sent by RRC to MAC */
mac_ul_traffic_event_trigger_cfg_type *oc_event_cfg_ptr
)
{
  uint16                                    i,j,k,l;
  uint16                                    ntrch=1;

  mac_ul_traffic_trch_event_trigger_cfg     *oc_trch_event_param_ptr;
  boolean                                   event_trch_incl, found, found_trch;
  //maci_ul_traffic_trch_event_param          *int_trch_param_ptr;
  //mac_ul_traffic_trch_event_trigger_param   *oc_trch_cfg_param_ptr;
  boolean                                   status=TRUE;
  ul_trch_info_type                         *trch_ptr;

  /*-------------------------------------------------------------------------------
   True indicates, the event parameters are specified for all transport channels
   as indicated. False implies that only one set of event parameters are specified
   and this is applied to all the uplink transport channels currently active.
  --------------------------------------------------------------------------------*/
  event_trch_incl = oc_event_cfg_ptr->trch_info_incl;

  if (event_trch_incl)
  {
    /*-----------------------------------------------------------------------------
    Copy the number of transport channels for which the event criteria are specified.
    -----------------------------------------------------------------------------*/
    ntrch = oc_event_cfg_ptr->num_trch;
  }
  else
  {
    status = FALSE;
  }

  /*-------------------------------------------------------------------------------
  MAC currently  supports only UE_MAX_TRCH number of transport channels.
  -------------------------------------------------------------------------------*/
  if (ntrch>UE_MAX_TRCH)
  {
    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Currently support %d trch's",
                                                UE_MAX_TRCH);
    status=FALSE;
  }
  else
  {
    /*-------------------------------------------------------------------------------
    The following piece of code checks if the transport channel for which the event
    criteria are specified are valid. Even if one transport channel id or type is
    invalid it breaks out setting status to FALSE.
    --------------------------------------------------------------------------------*/
    /* Go through the transport channels specified */
    for (i=0;i<ntrch;i++)
    {
      oc_trch_event_param_ptr=&oc_event_cfg_ptr->trch_event_cfg[i];

      /* The number of events should be >= 1 */
      if ((oc_trch_event_param_ptr->num_events==0) ||
          (oc_trch_event_param_ptr->num_events > MAX_NUM_EVENTS))
      {
        /*---------------------------------------------------------------------------
         If the number of events is 0 then set the status flag to FALSE
        ---------------------------------------------------------------------------*/
        status=FALSE;
        MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Number of events can't be 0");
        break;
      }
    } /* for number of transport channels */

    /*-------------------------------------------------------------------------------
    We copy the transport channel id & type to local memory only if all the checks
    have been done If the transport channel id's and types are valid
    -------------------------------------------------------------------------------*/
    if (status)
    {
      // Go through the transport channels mentioned by RRC in modify command
      for (i=0;i<ntrch;i++)
      {
        // Get the pointer to the transport channel config information
        oc_trch_event_param_ptr=&oc_event_cfg_ptr->trch_event_cfg[i];

        // Initialise the boolean to FALSE. This indicates that we have not yet found
        // a transport channel that matches the one given in setup command.
        found_trch = FALSE;

        // Go through the transport channels that are already present in the MAC
        // database.
        for (k=0;k<int_tmp_cfg_ptr->event_num_trch;k++)
        {
          // Check if the transport channel id and transport channel type matches.
          if (((oc_trch_event_param_ptr->trch_info.trch_type == RACH ) && 
               (int_tmp_cfg_ptr->event_trch_info[k].trch_type == oc_trch_event_param_ptr->trch_info.trch_type)) ||
              ((int_tmp_cfg_ptr->event_trch_info[k].trch_id == oc_trch_event_param_ptr->trch_info.trch_id) &&
               (int_tmp_cfg_ptr->event_trch_info[k].trch_type == oc_trch_event_param_ptr->trch_info.trch_type)))
          {

            MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "TVM:Replacing with modify MCM events TR_CH Id = %d,cctrch_type = %d ",
                                                       int_tmp_cfg_ptr->event_trch_info[k].trch_id,
                                                       int_tmp_cfg_ptr->event_trch_info[k].trch_type);

            // Go through the events that are being modified fiven to MAC by RRC
            for (j=0;j<oc_trch_event_param_ptr->num_events;j++)
            {
              // Initialise the boolean found to FALSE to indicate that we have  not
              // yet found a event matching the one already in the database for thie
              // transport channel id.
              found = FALSE;

              // Go through the events that are already in the MAC database for
              // this transport channel
              for (l=0;l<int_tmp_cfg_ptr->num_events[k];l++)
              {
                // If the event id matches.
                if (int_tmp_cfg_ptr->trch_event_param[k][l].event_id ==
                    oc_trch_event_param_ptr->event_param[j].event_id )
                {
                  // Then some event specific parameters for this event are being
                  // modified.
                  MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "TVM:Modify num_events %d,evt %d, trch_id %d",
                                                             oc_trch_event_param_ptr->num_events,
                                                             l,
                                                             int_tmp_cfg_ptr->event_trch_info[k].trch_id);

                  /*----------------------------------------------------------------------
                  Copy all the timer values froom the config information rec'd from RRC
                  ----------------------------------------------------------------------*/
                  if (oc_trch_event_param_ptr->event_param[j].trigger_prohibit_tmr)
                  {
                    int_tmp_cfg_ptr->trch_event_param[k][l].trigger_prohibit_tmr =
                    oc_trch_event_param_ptr->event_param[j].trigger_prohibit_tmr;
                  }

                  if (oc_trch_event_param_ptr->event_param[j].tx_interrupt_tmr)
                  {
                    int_tmp_cfg_ptr->trch_event_param[k][l].tx_interrupt_tmr =
                    oc_trch_event_param_ptr->event_param[j].tx_interrupt_tmr;
                  }

                  if (oc_trch_event_param_ptr->event_param[j].time_to_trigger)
                  {
                    int_tmp_cfg_ptr->trch_event_param[k][l].time_to_trigger =
                    oc_trch_event_param_ptr->event_param[j].time_to_trigger;
                  }

                  /*----------------------------------------------------------------------
                  Copy the report threshold.
                  ----------------------------------------------------------------------*/
                  if (oc_trch_event_param_ptr->event_param[j].report_threshold)
                  {
                    int_tmp_cfg_ptr->trch_event_param[k][l].report_threshold =
                    oc_trch_event_param_ptr->event_param[j].report_threshold;
                  }

                  /*----------------------------------------------------------------------
                    Initialize all the counters. All the timers for traffic volume are
                    implemented in the form of counters. There is one base timer which
                    expires every 10ms.
                  ----------------------------------------------------------------------*/
                  int_tmp_cfg_ptr->trch_event_param[k][l].trigger_tmr_cnt=0;
                  int_tmp_cfg_ptr->trch_event_param[k][l].prohibit_tmr_cnt=0;
                  int_tmp_cfg_ptr->trch_event_param[k][l].evt_not_first_meas=FALSE;
                  int_tmp_cfg_ptr->trch_event_param[k][l].first_event_trch_id = 0xFF;
                  found = TRUE;
                  break;
                }
              } /* for number of events actually modified */

              // Add this new event that was configured.
              if (found == FALSE)
              {
                 int_tmp_cfg_ptr->trch_event_param[k][l].event_id =
                  oc_trch_event_param_ptr->event_param[j].event_id;
                 int_tmp_cfg_ptr->trch_event_param[k][l].report_threshold =
                    oc_trch_event_param_ptr->event_param[j].report_threshold;
                 int_tmp_cfg_ptr->trch_event_param[k][l].time_to_trigger =
                    oc_trch_event_param_ptr->event_param[j].time_to_trigger;
                 int_tmp_cfg_ptr->trch_event_param[k][l].trigger_prohibit_tmr =
                    oc_trch_event_param_ptr->event_param[j].trigger_prohibit_tmr;
                 int_tmp_cfg_ptr->trch_event_param[k][l].trigger_tmr_cnt=0;
                 int_tmp_cfg_ptr->trch_event_param[k][l].prohibit_tmr_cnt=0;
                 int_tmp_cfg_ptr->trch_event_param[k][l].evt_not_first_meas=FALSE;
                 int_tmp_cfg_ptr->trch_event_param[k][l].first_event_trch_id = 0xFF;
                 int_tmp_cfg_ptr->num_events[k]++;
              }

            } /* Iterate through J */

            found_trch = TRUE;
            break;
          }
        }

        if (found_trch == FALSE)
        {
          int_tmp_cfg_ptr->event_num_trch++;

          /*------------------------------------------------------------------------
          Copy the transport channel id and the type to the table internally
          maintained by MAC
          -------------------------------------------------------------------------*/
          if (oc_trch_event_param_ptr->trch_info.trch_type==RACH)
          {
            /* Get the transport channel infor pointer */
            trch_ptr = GET_UL_TRCH_INFO_PTR(0);

            /* Store the trch_id along with the config information*/
            int_tmp_cfg_ptr->event_trch_info[k].trch_id = trch_ptr->trch_id;
          }
          else
          {
            int_tmp_cfg_ptr->event_trch_info[k].trch_id =
              oc_trch_event_param_ptr->trch_info.trch_id;
          }

          int_tmp_cfg_ptr->event_trch_info[k].trch_type =
               oc_trch_event_param_ptr->trch_info.trch_type;
          int_tmp_cfg_ptr->num_events[k] = oc_trch_event_param_ptr->num_events;

          //int_trch_param_ptr = int_tmp_cfg_ptr->trch_event_param[i];
          //oc_trch_cfg_param_ptr = oc_trch_event_param_ptr->event_param;

          /* Copy the event specific parameters */
          for (j=0;j<oc_trch_event_param_ptr->num_events;j++)
          {
            MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "TVM:Appending Modify num_events %d",
                                                       oc_trch_event_param_ptr->num_events);

            int_tmp_cfg_ptr->trch_event_param[k][j].event_id =
            oc_trch_event_param_ptr->event_param[j].event_id;
            /*----------------------------------------------------------------------
            Copy all the timer values froom the config information rec'd from RRC
            ----------------------------------------------------------------------*/
            int_tmp_cfg_ptr->trch_event_param[k][j].trigger_prohibit_tmr =
            oc_trch_event_param_ptr->event_param[j].trigger_prohibit_tmr;

            int_tmp_cfg_ptr->trch_event_param[k][j].tx_interrupt_tmr =
            oc_trch_event_param_ptr->event_param[j].tx_interrupt_tmr;

            int_tmp_cfg_ptr->trch_event_param[k][j].time_to_trigger =
            oc_trch_event_param_ptr->event_param[j].time_to_trigger;

            /*----------------------------------------------------------------------
            Copy the report threshold.
            ----------------------------------------------------------------------*/
            int_tmp_cfg_ptr->trch_event_param[k][j].report_threshold =
            oc_trch_event_param_ptr->event_param[j].report_threshold;

            /*----------------------------------------------------------------------
              Initialize all the counters. All the timers for traffic volume are
              implemented in the form of counters. There is one base timer which
              expires every 10ms.
            ----------------------------------------------------------------------*/
            int_tmp_cfg_ptr->trch_event_param[k][j].trigger_tmr_cnt=0;
            int_tmp_cfg_ptr->trch_event_param[k][j].prohibit_tmr_cnt=0;
            int_tmp_cfg_ptr->trch_event_param[k][j].evt_not_first_meas=FALSE;
            int_tmp_cfg_ptr->trch_event_param[k][j].first_event_trch_id = 0xFF;
          } /* for number of events*/
        }
      } /*for number of transport channels*/
    }/* if status */
  } /* if ntrch <= UE_MAX_TRCH */
  return status;
}

/*==================================================================================
 FUNCTION NAME  check_n_copy_periodic_params

 PURPOSE  This function copies the periodic parameters only if the transport channels
          specified are valid. If the transport channel list is not specified then
          it populates the object list too.
==================================================================================*/
boolean check_n_copy_periodic_params
(
/* Pointer to the config information internally stored by MAC */
maci_ul_traffic_cfg_param                 *int_tmp_cfg_ptr,

/* Pointer to the config information received from RRC */
mac_ul_traffic_meas_config_type           *oc_traffic_cfg_ptr

)
{
  mac_ul_traffic_periodic_report_cfg_type   *oc_periodic_cfg_ptr;
  boolean   status_ind=TRUE;

  oc_periodic_cfg_ptr = &oc_traffic_cfg_ptr->meas_report_criteria.periodic_cfg;

  if (oc_periodic_cfg_ptr->report_interval/10 < 2)
  {
    /* The minimum valid report interval is 20ms */
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Report interval not valid");
    status_ind = FALSE;
  }

  if (status_ind)
  {
    /*------------------------------------------------------------------------------
     meas_obj_list_incl=TRUE indicates that measurement is to be done for the
     transport channels specified in the object list FALSE implies apply
     measurement to all active uplink transport channels
    -------------------------------------------------------------------------------*/
    int_tmp_cfg_ptr->meas_obj_list_incl = oc_traffic_cfg_ptr->meas_obj_incl;

    /*-------------------------------------------------------------------------------
    True indicates that there is no limit on the number of reports that we send RRC
    False indicates, send only the number of reports as specified by utran
    --------------------------------------------------------------------------------*/
    int_tmp_cfg_ptr->report_infinity = oc_periodic_cfg_ptr->report_infinity;

    /*-------------------------------------------------------------------------------
       Copy the periodic config information in the local memory.These include the
        number of times the report has to be sent and the report interval.
    -------------------------------------------------------------------------------*/
    int_tmp_cfg_ptr->num_reports     = oc_periodic_cfg_ptr->num_reports;
    int_tmp_cfg_ptr->report_interval = oc_periodic_cfg_ptr->report_interval;

    /*------------------------------------------------------------------------------
    Initialise the periodic timer count to 0. All timers in MAC are implemented in
    the form of counters.
    -------------------------------------------------------------------------------*/
    int_tmp_cfg_ptr->periodic_tmr_cnt = 0;

    /*------------------------------------------------------------------------------
          Initialise the number of reports sent so far to 0.
    -------------------------------------------------------------------------------*/
    int_tmp_cfg_ptr->nreports_sent = 0;

  }

  return status_ind;

}

/*==================================================================================
 FUNCTION NAME  mac_ul_setup_traffic_volume_meas

 PURPOSE    This function populates the traffic volume measurement table and starts
            traffic volume measurements for the corresponding id.
            This function returns a boolean. True indicates the setup was sucessful.
            It goes through all the parameters specified by RRC and checks to see
            if they are valid. If it is valid then it copies them to the local
            memory.
==================================================================================*/

boolean mac_ul_setup_traffic_volume_meas
(
/* Pointer to traffic volume config information */
mac_ul_traffic_meas_config_type *oc_traffic_cfg_ptr
)
{
  uint16                                    meas_id;
  uint8                                     i;
  maci_ul_traffic_cfg_index_type            *int_cfg_param_ptr;
  maci_ul_traffic_cfg_param                 *int_tmp_cfg_ptr;
  boolean                                   status_ind=TRUE;
  mac_ul_traffic_report_mode_e_type         report_mode;
  mac_ul_traffic_meas_report_qty_type       *report_qty;

  meas_id = oc_traffic_cfg_ptr->meas_id;

  /*--------------------------------------------------------------------------------
  Measurement identity not valid. meas_id can range from 1-16
  --------------------------------------------------------------------------------*/
  if (TRAFFIC_MEAS_ID_IS_INVALID(meas_id))
  {
    status_ind=FALSE;
    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Invalid measurement ID %d",
                                                meas_id);
  }
  else if ( (oc_traffic_cfg_ptr->meas_start_incl == TRUE) && 
            (GET_UL_CCTRCH_TYPE==INVALID_UL_CCTRCH)
        #ifdef FEATURE_WCDMA_HS_RACH
             && (mac_hs_rach_info.hs_rach_active == FALSE)
        #endif
           )
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MAC UL transport channels not configured");
    status_ind=FALSE;
  }
  else
  {
    /*-----------------------------------------------------------------------------
    Get the appropriate entry for this meas_id from the config table internally
    maintained by MAC.
    ------------------------------------------------------------------------------*/
    int_cfg_param_ptr = GET_UL_TRAFFIC_CFG_PARAM_PTR(meas_id);

    /*-----------------------------------------------------------------------------
    If the traffic_cfg_ptr is not NULL then we have already received a setup cmd
    for this measurement identity. "valid=TRUE" flag indicates if the correspoding
    structures have valid parametrs in them.
    -----------------------------------------------------------------------------*/
    if (int_cfg_param_ptr->traffic_cfg_ptr != NULL)
    {
      MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Another setup command recvd for meas_id %d",
                                                 meas_id);
      int_tmp_cfg_ptr = int_cfg_param_ptr->traffic_cfg_ptr;
      
      if ((int_tmp_cfg_ptr->report_mode == EVENT_TRIGGERED_MODE) && (--num_event_measurements == 0))
      {
        (void)rex_clr_timer(&mac_ul_tvm_event_tmr_blk);
         mac_tvm_event_timer_started = FALSE;
         MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Num_event_meas = %d",
                                                    num_event_measurements);
      }
      else if ((int_tmp_cfg_ptr->report_mode == PERIODIC_MODE) && (--num_periodic_measurements == 0))
      {
        (void)rex_clr_timer(&mac_ul_tvm_periodic_tmr_blk);
        mac_tvm_periodic_timer_started = FALSE;
        MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "num_periodic_measurements = %d",
                                                   num_periodic_measurements);
      }
      
      memset(int_tmp_cfg_ptr,0x00, sizeof(maci_ul_traffic_cfg_param));
    }
    else
    {
      /*---------------------------------------------------------------------------
      Allocate memory from tmc_heap.
      ----------------------------------------------------------------------------*/
#ifdef FEATURE_WCDMA_USE_MODEM_HEAP
      int_tmp_cfg_ptr = modem_mem_alloc(sizeof(maci_ul_traffic_cfg_param), MODEM_MEM_CLIENT_WCDMA_RLC_CRIT);
#else
      int_tmp_cfg_ptr = mem_malloc(&tmc_heap,sizeof(maci_ul_traffic_cfg_param));
#endif
      if (int_tmp_cfg_ptr==NULL)
      {
        status_ind = FALSE;
        MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Failed to allocate memory");
      }
      else
      {
        memset(int_tmp_cfg_ptr,0x00, sizeof(maci_ul_traffic_cfg_param));
      }
    }
    if (status_ind)
    {
      if (oc_traffic_cfg_ptr->meas_obj_incl)
      {

        int_tmp_cfg_ptr->num_trch = oc_traffic_cfg_ptr->meas_obj_list.num_trch; /*lint !e613*/
        /*--------------------------------------------------------------------------
           If the transport channels exist then copy them to the local memory
          --------------------------------------------------------------------------*/
        WCDMA_MEMCPY(int_tmp_cfg_ptr->trch_info,
                     UE_MAX_TRCH * sizeof(mac_ul_traffic_trch_info_type),
                     oc_traffic_cfg_ptr->meas_obj_list.trch_info,
                     UE_MAX_TRCH * sizeof(mac_ul_traffic_trch_info_type));

        for (i=0;i<int_tmp_cfg_ptr->num_trch && i < UE_MAX_TRCH;i++) /*lint !e613*/
        {
          if ((int_tmp_cfg_ptr->trch_info[i].trch_type==RACH)&&
              ((GET_UL_CCTRCH_TYPE==RACH) || (oc_traffic_cfg_ptr->meas_start_incl == FALSE)))
          {
            int_tmp_cfg_ptr->trch_info[i].trch_id = int_ul_trch_info[0].trch_id; /*lint !e613*/
          }
        }
      }/*meas_obj_list_incl */ /*lint !e613 */
      else
      {
        /*----------------------------------------------------------------------------
        Populate the transport channel id and the type information from all the
        uplink transport channels currently configured in MAC.
        ----------------------------------------------------------------------------*/
        int_tmp_cfg_ptr->num_trch= populate_object_list(int_tmp_cfg_ptr->trch_info); /*lint !e613*/
      }

      /*-------------------------------------------------------------------------
       Checks if the measurement interval is specified when we are
       asked to report the average or variance of rlc bufferpayload
      --------------------------------------------------------------------------*/
      /* If all the results of the checks are valid */
      if (status_ind && check_meas_report_qty(oc_traffic_cfg_ptr))
      {
        /*-----------------------------------------------------------------------
        Copy the report quantity(indicates if we have to report average, raw,
        variance buffer payload. Copy the measuremen to calculate the average or
        variance
        -----------------------------------------------------------------------*/
        report_qty = &oc_traffic_cfg_ptr->meas_report_qty;
        int_tmp_cfg_ptr->avg_payload = report_qty->avg_payload; /*lint !e613*/
        int_tmp_cfg_ptr->rlc_payload = report_qty->rlc_payload; /*lint !e613*/
        int_tmp_cfg_ptr->var_payload = report_qty->var_payload; /*lint !e613*/

        /*---------------------------------------------------------------------
        Number of samples is given by dividing the measurement interval by 10.
        We take measurement every 10ms.
        ---------------------------------------------------------------------*/
        int_tmp_cfg_ptr->nsamples=(uint16)oc_traffic_cfg_ptr->meas_qty.meas_interval/10; /*lint !e613*/

        /*---------------------------------------------------------------------
        Copy the report mode. this indicates if we have to report periodic or
        event triggered.
        ---------------------------------------------------------------------*/
        report_mode = oc_traffic_cfg_ptr->meas_report_mode.report_mode;

        /*---------------------------------------------------------------------
        Copy the relevant mode related parameters to the local memory.
        ---------------------------------------------------------------------*/
        if (report_mode==PERIODIC_MODE)
        {
          /*---------------------------------------------------------------------
          If the mode is periodic then check the periodic mode related parameters
          and then copy them to the local memory.
          ---------------------------------------------------------------------*/
          status_ind= check_n_copy_periodic_params(int_tmp_cfg_ptr,oc_traffic_cfg_ptr);
        }
        else if (report_mode==EVENT_TRIGGERED_MODE)
        {
          /*---------------------------------------------------------------------
          If the mode is event triggered then check the mode related parameters
          and then copy them to the local memory.
          ---------------------------------------------------------------------*/
          status_ind = check_n_copy_event_trigger_params(int_tmp_cfg_ptr,
                       &oc_traffic_cfg_ptr->meas_report_criteria.event_cfg);
        }
      }
      /*------------------------------------------------------------------------
       All the necessary config validations are done above. Now if the status
       indication is FALSE then free the memory. Otherwise store the pointer in
       the config index table internally by MAC
      -------------------------------------------------------------------------*/
      if (!status_ind)
      {
        /*Free the memory */
#ifdef FEATURE_WCDMA_USE_MODEM_HEAP
        modem_mem_free(int_tmp_cfg_ptr, MODEM_MEM_CLIENT_WCDMA_RLC_CRIT);
#else
        mem_free(&(tmc_heap),int_tmp_cfg_ptr);
#endif
        MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Setup configuration error");
      }
      else
      {
        /*----------------------------------------------------------------------
         Copy the tranfer(UM or AM) & report mode (periodic or event triggered)
        ----------------------------------------------------------------------*/
        int_tmp_cfg_ptr->report_mode=
        oc_traffic_cfg_ptr->meas_report_mode.report_mode; /*lint !e613*/
        int_tmp_cfg_ptr->transfer_mode=
        oc_traffic_cfg_ptr->meas_report_mode.transfer_mode;/*lint !e613*/

        /*----------------------------------------------------------------------
         Initialize the first_meas flag to TRUE indicating that this is the
         first measurement taken for this measurement identity. This boolean
         is used when we have the measurement interval greater than the
         report interval and we have to compute average or variance.
        ----------------------------------------------------------------------*/
        int_tmp_cfg_ptr->first_meas = TRUE; /*lint !e613*/

        /*---------------------------------------------------------------------
          Assign the appropriate pointers and set the valid flag to TRUE
          indicating that all the parameters are valid.
        ----------------------------------------------------------------------*/
        int_cfg_param_ptr->traffic_cfg_ptr = int_tmp_cfg_ptr;
        int_cfg_param_ptr->valid = TRUE;

        int_tmp_cfg_ptr->meas_obj_list_incl = oc_traffic_cfg_ptr->meas_obj_incl; /*lint !e613*/

        /*---------------------------------------------------------------------
        The 10ms traffic volume timer is started when there is atleast one
        measurement to make. This timer is cleared when there are no
        measurements to make.
        ----------------------------------------------------------------------*/
        /*-----------------------------------------------------------------
        start the 10ms timer for the traffic volume measurements if there is
        atleast  one traffic volume measurement to make.
        ---------------------------------------------------------------------*/

        if (int_cfg_param_ptr->traffic_cfg_ptr->report_mode == PERIODIC_MODE)
        {
          num_periodic_measurements++;
        }
        else if (int_cfg_param_ptr->traffic_cfg_ptr->report_mode == EVENT_TRIGGERED_MODE)
        {
          num_event_measurements++;
        }

        if (oc_traffic_cfg_ptr->meas_start_incl)
        {
          int_cfg_param_ptr->suspend_meas = FALSE;
          
          if ((num_event_measurements > 0) && (!mac_tvm_event_timer_started))
          {
            (void)rex_set_timer(&mac_ul_tvm_event_tmr_blk,(uint32)MAC_UL_TRAFFIC_VOLUME_TMR_BASE);
            mac_tvm_event_timer_started = TRUE;
            MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Starting mac_ul_tvm_event_tmr_blk timer %d ms",
                                                       MAC_UL_TRAFFIC_VOLUME_TMR_BASE);
          }
          if ((num_periodic_measurements > 0) && (!mac_tvm_periodic_timer_started))
          {
            (void)rex_set_timer(&mac_ul_tvm_periodic_tmr_blk,
                   (uint32)(MAC_UL_TRAFFIC_VOLUME_TMR_BASE * MAC_UL_PERIODIC_TVM_TIMER_SCALE));
            mac_tvm_periodic_timer_started = TRUE;
            MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Starting mac_ul_tvm_periodic_tmr_blk timer %d ms",
                                                       (MAC_UL_TRAFFIC_VOLUME_TMR_BASE*MAC_UL_PERIODIC_TVM_TIMER_SCALE));
          }
          
          MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Starting measurements num_event_meas %d, num_periodic_meas %d",
                                                     num_event_measurements,
                                                     num_periodic_measurements);
        }
        else
        {
          int_cfg_param_ptr->suspend_meas = TRUE;
          mac_tvm_event_timer_started = FALSE;
          mac_tvm_periodic_timer_started = FALSE;
          MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Setup recvd but suspended for now ");
        }

      }/*status_ind==TRUE */

    }/*int_tmp_cfg_ptr !=NULL */

  }/* Measurement identity is valid */

  return status_ind;
}

/*========================================================================================
 FUNCTION NAME  compute_obj_list_raw_payload()

 PURPOSE        This function calculates the raw rlc payload for all the radio bearers.

=========================================================================================*/
void compute_obj_list_raw_payload(void  )
{
  uint16                      i,j;
  ul_trch_info_type           *trch_ptr;
  ul_lc_info_type             *lcinfo_ptr;
  rlc_lc_id_type              rlc_id;
  tr_ch_id_type               trch_id;
  rlc_ul_lc_buf_status_type   *lc_buf_ptr;
  trch_id_to_idx_map_type     *id_to_idx_ptr;
  uint32                      pdu_size, total_data_bits, uncomp_data;
  uint16                      npdus = 0;
  uint16                      hdr_len;
#ifdef FEATURE_WCDMA_HS_RACH
  uint8                       rlc_hdr_len;
  rlc_ul_lc_info_type         *lc_info_ptr;
  mac_hs_ul_logical_ch_mac_d_flow_info_type *hs_lc_macd_flow_map;
#endif
  

  /* Clear the previous buffer status */
  memset(&traffic_ul_buf_status,0, sizeof(rlc_ul_buf_status_type));

#ifdef FEATURE_WCDMA_HS_RACH

  /* HS-RACH active - Only single EDCH channel can be setup */
  if(mac_hs_rach_info.hs_rach_active == TRUE)
  {
    lc_info_ptr = get_mac_hs_ul_lc_info_ptr();
    hs_lc_macd_flow_map = get_mac_hs_logch_macd_flow_mapping_ptr();

    if(lc_info_ptr->nchan > 0)
    {
      //Mutex Protection required ??
      rlc_ul_enh_buffer_status(lc_info_ptr, &traffic_ul_buf_status, TVM_QUERY, NULL);
    } 

    /*------------------------------------------------------------------------------
        When HS Rach is active, a single EDCH channel is configured 
        Compute payload for all the logChs mapped on to that EDCH channel
    ------------------------------------------------------------------------------*/
    for(i=0; i<lc_info_ptr->nchan; i++)
    {
      rlc_id = lc_info_ptr->rlc_id[i];
      /*-------------------------------------------------------------------------
         Get the buffer status for this logical channel
        --------------------------------------------------------------------------*/
      lc_buf_ptr = &traffic_ul_buf_status.chan_info[rlc_id];

      if(hs_lc_macd_flow_map[rlc_id].logical_ch_enabled == FALSE)
      {
        MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC_ERR: Logical Channel with RLC ID %d not enabled",
                                                    rlc_id);
        continue;
      }

      switch (lc_buf_ptr->type)
      {
        /*-----------------------------------------------------------------------
        If the pdu type is either unacknowledged or acknowledged mode
        ------------------------------------------------------------------------*/
        case FLEX_PDUS:

          if (hs_lc_macd_flow_map[rlc_id].rlc_mode == UE_MODE_UNACKNOWLEDGED)
          {
            pdu_size = hs_lc_macd_flow_map[rlc_id].ul_flex_min_pdu_size;
            hdr_len = 1; // number of bytes of header
          }
          else
          {
            pdu_size = lc_buf_ptr->pdu_size;
            hdr_len = 2; // number of bytes of header
          }
          rlc_hdr_len = hs_lc_macd_flow_map[rlc_id].rlc_hdr_length;
          lc_buf_ptr->nbytes = (lc_buf_ptr->new_data_bytes+lc_buf_ptr->retx_data_bytes+lc_buf_ptr->ctrl_data_bytes);
          
          #ifdef FEATURE_WCDMA_UL_COMPR
          lc_buf_ptr->nbytes += lc_buf_ptr->new_data_uncomp_bytes;
          #endif

  
          if ((lc_buf_ptr->new_data_bytes) && (pdu_size > 0))
          {
            total_data_bits = ((lc_buf_ptr->new_data_bytes + lc_buf_ptr->retx_data_bytes) << 3);
            #ifdef FEATURE_WCDMA_UL_COMPR
            total_data_bits += (lc_buf_ptr->new_data_uncomp_bytes << 3);
            #endif
            
            npdus = math_ceil(total_data_bits, (pdu_size - rlc_hdr_len));
            lc_buf_ptr->nbytes += (npdus * hdr_len);
            
            if(tvm_debug == 1)
            {
              MSG_9(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH,
                "HS_RACH_TVM: Rlc_id %d, new_data %d bytes, retx_data %d bytes,ctrl_data %d bytes, npdus %d, pdu_size %d, hdr_len %d bytes",
                rlc_id, lc_buf_ptr->new_data_bytes, lc_buf_ptr->retx_data_bytes,
                lc_buf_ptr->ctrl_data_bytes,npdus, pdu_size, hdr_len, 0, 0);
            }
            
          }
          
          rb_raw_payload[hs_lc_macd_flow_map[rlc_id].rb_id][cur_index] = lc_buf_ptr->nbytes;
          
          if (lc_buf_ptr->nbytes > 0)
          {
            MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "HS_RACH_TVM: rlc_id %d, buf_status %d",
                                                       rlc_id,
                                                       lc_buf_ptr->nbytes);
          }
          break;
          
        default:
        MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW, "Traffic volume not computed for TM pdus");
        break;
      } /* switch pdu type */
    }
  }
 else
#endif
 {
  if (mac_ul_lc_info.nchan > 0 )
  {
    REX_ISR_LOCK(&r99_ul_isr_l2_datapath_mutex);

    rlc_ul_enh_buffer_status(&mac_ul_lc_info,&traffic_ul_buf_status, TVM_QUERY, NULL);

    REX_ISR_UNLOCK(&r99_ul_isr_l2_datapath_mutex);

  }

  for (i = 0 ; i<MAX_TRCH; i++)
  {
    uncomp_data = 0;
    trch_id = i + MAC_UL_MIN_TRCH_ID ;

    /*------------------------------------------------------------------------------
      Get the transport channel information for this transport channel index given by
      trch_id
    ------------------------------------------------------------------------------*/
    id_to_idx_ptr = GET_UL_TRCH_ID_TO_IDX_PTR(trch_id);

    /*------------------------------------------------------------------------------
    If the transport channel is valid.
    ------------------------------------------------------------------------------*/
    if (id_to_idx_ptr->valid)
    {
      /*----------------------------------------------------------------------------
      Get the transport channel information pertaining to this tarnsport channel id.
      "idx" gives the index into the table where the information is stored.
      ----------------------------------------------------------------------------*/
      trch_ptr = GET_UL_TRCH_INFO_PTR(id_to_idx_ptr->idx);

      /*----------------------------------------------------------------------------
      Go through the logical channels which are mapped onto this transport
      channels
      ----------------------------------------------------------------------------*/
      for (j=0; j<trch_ptr->nlchan &&
          j < UE_MAX_UL_LOGICAL_CHANNEL ;j++)
      {
        /*-------------------------------------------------------------------------
        Get the rlc id's which are mapped to this transport channel.
        --------------------------------------------------------------------------*/
        rlc_id = trch_ptr->rlc_id[j];

        /*-------------------------------------------------------------------------
          Get the logical channel information for this RLC ID given by rlc_id.
        --------------------------------------------------------------------------*/
        lcinfo_ptr = GET_UL_LC_INFO_PTR(rlc_id);

        /*-------------------------------------------------------------------------
        Get the buffer status for this logical channel
        --------------------------------------------------------------------------*/
        lc_buf_ptr = &traffic_ul_buf_status.chan_info[rlc_id];

        /*-------------------------------------------------------------------------
          if the Logical channel is enabled
        --------------------------------------------------------------------------*/
        if (lcinfo_ptr->enable)
        {
          switch (lc_buf_ptr->type)
          {
          /*-----------------------------------------------------------------------
          If the pdu type is either unacknowledged or acknowledged mode
          ------------------------------------------------------------------------*/
          case FLEX_PDUS:
            /*---------------------------------------------------------------------
            "idx" gives the correct location at which the raw rlc payload
            data has to be stored. we maintain an array of 26 elements.This
            array is used as a circular array.
            ----------------------------------------------------------------------*/

            if (lcinfo_ptr->rlc_mode == UE_MODE_UNACKNOWLEDGED)
            {
              pdu_size = trch_min_tb_size[id_to_idx_ptr->idx];
              hdr_len = 1; // number of bytes of header
            }
            else
            {
              pdu_size = lc_buf_ptr->pdu_size;
              hdr_len = 2; // number of bytes of header
            }

            lc_buf_ptr->nbytes = (lc_buf_ptr->new_data_bytes+lc_buf_ptr->retx_data_bytes+lc_buf_ptr->ctrl_data_bytes);

            if ((lc_buf_ptr->nbytes) && (pdu_size > 0))
            {
              total_data_bits = ((lc_buf_ptr->new_data_bytes + lc_buf_ptr->retx_data_bytes) << 3);
              #ifdef FEATURE_WCDMA_UL_COMPR
              total_data_bits += (lc_buf_ptr->new_data_uncomp_bytes << 3);
              uncomp_data = lc_buf_ptr->new_data_uncomp_bytes;
              #endif
              
              npdus = math_ceil(total_data_bits, (pdu_size-lcinfo_ptr->rlc_hdr_length));

              lc_buf_ptr->nbytes += (npdus * hdr_len);

              MSG_9(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH,
              "Rlc_id %d, new_data %d bytes, retx_data %d bytes,ctrl_data %d bytes, npdus %d, pdu_size %d, hdr_len %d bytes, uncomp_data %d",
              rlc_id, lc_buf_ptr->new_data_bytes, lc_buf_ptr->retx_data_bytes,
              lc_buf_ptr->ctrl_data_bytes,npdus, pdu_size, hdr_len, uncomp_data, 0);
            }

            rb_raw_payload[lcinfo_ptr->rb_id][cur_index] = lc_buf_ptr->nbytes;

            if (lc_buf_ptr->nbytes > 0)
            {
              MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "rlc_id %d, buf_status %d",
                                                         rlc_id,
                                                         lc_buf_ptr->nbytes);
            }

            break;

          default:
            MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW, "Traffic volume not computed for TM pdus");
            break;

          } /* switch pdu type */

        } /* if logical channel enabled */

      }/* for number of logical channels */

    }/* IF the transport channel is valid */

  } /* for number of transport channels*/
}
  cur_index++;
  if (cur_index >= TRAFFIC_MAX_SAMPLES)
  {
    rolled_over = TRUE;
  }
  cur_index = cur_index%TRAFFIC_MAX_SAMPLES;
}


#ifdef FEATURE_WCDMA_HS_RACH


/*========================================================================================
 FUNCTION NAME  find_hsrach_trch_index()

 PURPOSE If the set of transport channels on which event trigger is defined is
         provided explicitly, this function would return index of TrCh type EDCH, if present.
         Else, this function would return index of TrCh type EDCH from Object list, if present.

 RETURN VALUE   The index of the EDCH Trch type form the TVM config
=========================================================================================*/
uint8   find_hsrach_trch_index(maci_ul_traffic_cfg_param   *traffic_param_ptr)
{
  uint8   loop_index, trch_index;
  
  trch_index = 0xFF;
  
  if (traffic_param_ptr->event_trch_incl)
  {
    for (loop_index = 0 ; loop_index < traffic_param_ptr->event_num_trch; loop_index++)
    {
      if(traffic_param_ptr->event_trch_info[loop_index].trch_type == EDCH)
      {
        trch_index = loop_index;
        break;
      }
    }
  }
  else
  {
   for (loop_index = 0 ; loop_index < traffic_param_ptr->num_trch; loop_index++)
   {
     if(traffic_param_ptr->trch_info[loop_index].trch_type == EDCH)
     {
       trch_index = loop_index;
       break;
     }
   }
  }
  
  if(trch_index == 0xFF)
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "TVM_ERR: No EDCH Trch idex found in TVM conifg params" );
  }
  else
  {
    if(tvm_debug == 1)
    {
      MSG_9(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH,
          "HS_RACH_TVM: trch_index %d, event_n_trch %d, first_meas %d,n_events %d,num_trch %d ",
           trch_index, traffic_param_ptr->event_num_trch, traffic_param_ptr->first_meas, traffic_param_ptr->num_events,
           traffic_param_ptr->num_trch, 0, 0, 0, 0);
    }
  }
  return trch_index;
}

/*========================================================================================
 FUNCTION NAME  populate_log_chan_info()

 PURPOSE        This function populates rlc_ids of logChs mapped on to HS-RACH TrCh

 RETURN VALUE   Total number of logical channels mapped on all configured mac-d flows configured
=========================================================================================*/
uint8 populate_hs_rach_log_chan_info
(
  mac_hs_rach_config_s_type *oc_hs_rachptr, 
  rlc_lc_id_type hs_rach_rlc_id[]
)
{
  uint16                         j = 0, k = 0, l = 0;
  uint8                          nchan = 0;
  mac_e_config_s_type            *hs_rach_config_ptr;
  mac_e_mac_d_flow_info_s_type   *e_mac_d_flow_ptr;

  if(oc_hs_rachptr == NULL)
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC_TVM_ERR: oc_hs_rachptr is NULL");
  }
  else
  {
    hs_rach_config_ptr = &(oc_hs_rachptr->hs_rach_config);
    if(hs_rach_config_ptr != NULL)
    {
      /* For each mac-d flow configured */
      for(j=0; j<hs_rach_config_ptr->num_e_mac_d_flow; j++)
      {
        e_mac_d_flow_ptr = &(hs_rach_config_ptr->e_mac_d_flow_info[j]);
        if(e_mac_d_flow_ptr != NULL)
        {
          /* compute number of logical channels mapped on this mac-d flow*/
          nchan += e_mac_d_flow_ptr->num_logch;

          /* For each logical channel on a mac-d flow */
          for(k=0; k<e_mac_d_flow_ptr->num_logch; k++)
          {
            /* save rlc_id */
            hs_rach_rlc_id[l++] =  e_mac_d_flow_ptr->e_ul_logch_info[k].rlc_id;
          }
        }
        else
        {
          MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC_TVM_ERR: e-mac-d flow pointer NULL %d",
                                                      j);
        }
      }
    }
    else
    {
      MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC_TVM_ERR: hs-rach config pointer NULL ");
    }
  }
  return nchan;
}
#endif
/*=============================================================================
 FUNCTION NAME  compute_obj_list_raw_payload_cu()

 PURPOSE        Calculates the raw rlc payload for all the radio bearers.
=============================================================================*/
void compute_obj_list_raw_payload_cu
(
#ifdef FEATURE_WCDMA_HS_RACH
    mac_hs_rach_config_s_type     *oc_hs_rachptr,
#endif
mac_ul_rach_cctrch_config_type    *oc_rachptr,

rlc_ul_buf_status_type            *int_traffic_ul_buf_status_ptr
)
{
  uint16                       i = 0, j = 0;
  rlc_lc_id_type               rlc_id;
  rlc_ul_lc_buf_status_type    *lc_buf_ptr;
  trch_id_to_idx_map_type      *id_to_idx_ptr;
  mac_ul_ded_logch_config_type *oc_dlcptr;
  rlc_ul_lc_info_type           int_mac_ul_lc_info;
  uint32                      pdu_size;
  uint16                      npdus;
  uint16                      hdr_len_bytes;
#ifdef FEATURE_WCDMA_HS_RACH
  uint16                         k = 0, l = 0;
  uint32                         rlc_pdu_size = 0;
  mac_e_mac_d_flow_info_s_type   *e_mac_d_flow_ptr;
  uecomdef_logch_mode_e_type      rlc_mode = UE_MODE_MAX_NUM;
  boolean                         hs_rach_active = FALSE, found = FALSE;

  if(oc_hs_rachptr != NULL)
  {
    if(mac_hs_rach_info.hs_rach_active == TRUE)
    {
      hs_rach_active = TRUE;
    }
  }
#endif

  /* Clear the previous buffer status */
  memset(int_traffic_ul_buf_status_ptr, 0, sizeof(rlc_ul_buf_status_type));
  
  int_mac_ul_lc_info.nchan = 0;
  
#ifdef FEATURE_WCDMA_HS_RACH
  if(hs_rach_active == FALSE)
#endif
  {
    /* udpate the internal structure with CCCH info, if present */
    if (oc_rachptr->ccch_enable)
    {
      int_mac_ul_lc_info.rlc_id[i] = oc_rachptr->ccch_id;
      int_mac_ul_lc_info.nchan += 1;
      i++;
    }
  }
  
#ifdef FEATURE_WCDMA_HS_RACH
  if(hs_rach_active == TRUE) /* populate LogCh info for HSRACH */ 
  {
    int_mac_ul_lc_info.nchan = populate_hs_rach_log_chan_info(oc_hs_rachptr, int_mac_ul_lc_info.rlc_id);	  
  }
  else 
#endif
  {  
    while ((j < oc_rachptr->ndlchan) && (j < MAX_DED_LOGCHAN_PER_TRCH) && ((i+j) < UE_MAX_UL_LOGICAL_CHANNEL))
    {
      int_mac_ul_lc_info.rlc_id[i+j] = oc_rachptr->dlchan_info[j].rlc_id;
      j++;
    }
    int_mac_ul_lc_info.nchan += oc_rachptr->ndlchan;
  }

  if (int_mac_ul_lc_info.nchan > 0 )
  {
    REX_ISR_LOCK(&r99_ul_isr_l2_datapath_mutex);

    rlc_ul_enh_buffer_status(&int_mac_ul_lc_info, int_traffic_ul_buf_status_ptr, TVM_QUERY, NULL);

    REX_ISR_UNLOCK(&r99_ul_isr_l2_datapath_mutex);

  }

#ifdef FEATURE_WCDMA_HS_RACH
  if(hs_rach_active == TRUE)
  {
    /* For each LogChan mapped on EDCH */
    for (j = 0; ((j < int_mac_ul_lc_info.nchan) && (j < UE_MAX_UL_LOGICAL_CHANNEL)); j++)
    {
      rlc_id = int_mac_ul_lc_info.rlc_id[j]; 

      /* Get the buffer status for current logical channel */
      lc_buf_ptr = &(int_traffic_ul_buf_status_ptr->chan_info[rlc_id]);
  
      /* Get rlc_mode and pdu_size for a given rlc_id */
      for(k=0; k<oc_hs_rachptr->hs_rach_config.num_e_mac_d_flow; k++)
      {
        found = FALSE;
        e_mac_d_flow_ptr = &(oc_hs_rachptr->hs_rach_config.e_mac_d_flow_info[k]);
        for(l=0; l<e_mac_d_flow_ptr->num_logch; l++)
        {
          if( rlc_id == e_mac_d_flow_ptr->e_ul_logch_info[l].rlc_id )
          {
            rlc_mode = e_mac_d_flow_ptr->e_ul_logch_info[l].rlc_mode;
            rlc_pdu_size = e_mac_d_flow_ptr->e_ul_logch_info[l].ul_flex_min_pdu_size;
            found = TRUE;
            break;
          }
        }
        if(found == TRUE)
          break;
      }

      switch(lc_buf_ptr->type)
      {
        /* If the pdu type is either unacknowledged or acknowledged mode */
        case FLEX_PDUS:

        if (rlc_mode == UE_MODE_UNACKNOWLEDGED)
        {
          pdu_size = rlc_pdu_size;
          hdr_len_bytes = 1; // number of bytes of header
        }
        else
        {
          pdu_size = lc_buf_ptr->pdu_size;
          hdr_len_bytes = 2; // number of bytes of header
        }

        lc_buf_ptr->nbytes = (uint32) (lc_buf_ptr->new_data_bytes + 
                                       lc_buf_ptr->retx_data_bytes + 
                                       lc_buf_ptr->ctrl_data_bytes);

        if ((lc_buf_ptr->new_data_bytes) && (pdu_size > 0))
        {
          npdus = math_ceil(((lc_buf_ptr->new_data_bytes + lc_buf_ptr->retx_data_bytes) << 3), 
                            (pdu_size - (hdr_len_bytes << 3)));
          npdus += math_ceil((lc_buf_ptr->ctrl_data_bytes << 3), 
                             (pdu_size - (hdr_len_bytes << 3)));
          
#ifdef FEATURE_WCDMA_UL_COMPR
          npdus += math_ceil((lc_buf_ptr->new_data_uncomp_bytes << 3), 
                             (pdu_size - (hdr_len_bytes << 3)));
#endif

          lc_buf_ptr->nbytes += (npdus * hdr_len_bytes);

          /*        Need to commented out For TASK LOCK           */
           MSG_9(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH,
          "MAC_TVM: HS_RACH_CU - Rlc_id %d, new_data %d bytes, retx_data %d bytes, ctrl_data %d bytes, npdus %d, pdu_size %d, hdr_len_bytes %d bytes",
          rlc_id, lc_buf_ptr->new_data_bytes, lc_buf_ptr->retx_data_bytes,
          lc_buf_ptr->ctrl_data_bytes, npdus, pdu_size, hdr_len_bytes, 0, 0);
          
        }

        MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MAC_TVM: rlc_id %d, buf_status %d",
                                                   rlc_id, lc_buf_ptr->nbytes);
        break;
        
        default:
        MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW, "Traffic volume not computed for TM pdus");
        break;
      }
      
    }
  }
  else  
#endif
  {
  /*----------------------------------------------------------------------------
    Get the transport channel information for this transport channel index given by
    trch_id
  ------------------------------------------------------------------------------*/
  id_to_idx_ptr = GET_UL_TRCH_ID_TO_IDX_PTR(oc_rachptr->trch_id);
  
  if (id_to_idx_ptr->valid)
  {
    for (j = 0; ((j < oc_rachptr->ndlchan) && (i < MAX_DED_LOGCHAN_PER_TRCH)); j++)
    {
      rlc_id = oc_rachptr->dlchan_info[j].rlc_id;
      
      /* Get the buffer status for this logical channel */
      lc_buf_ptr = &(int_traffic_ul_buf_status_ptr->chan_info[rlc_id]);

      /* Pointer to ordered config parameters of DCCH/DTCH */
      oc_dlcptr = &(oc_rachptr->dlchan_info[i]);
      
      switch (lc_buf_ptr->type)
      {
        /* If the pdu type is either unacknowledged or acknowledged mode */
        case FLEX_PDUS:

        if (oc_dlcptr->rlc_mode == UE_MODE_UNACKNOWLEDGED)
        {
          pdu_size = trch_min_tb_size[id_to_idx_ptr->idx];
          hdr_len_bytes = 1; // number of bytes of header
        }
        else
        {
          pdu_size = lc_buf_ptr->pdu_size;
          hdr_len_bytes = 2; // number of bytes of header
        }

        lc_buf_ptr->nbytes = (uint32) (lc_buf_ptr->new_data_bytes + 
                                       lc_buf_ptr->retx_data_bytes + 
                                       lc_buf_ptr->ctrl_data_bytes);

        if ((lc_buf_ptr->new_data_bytes) && (pdu_size > 0))
        {
          npdus = math_ceil(((lc_buf_ptr->new_data_bytes + lc_buf_ptr->retx_data_bytes) << 3), 
                            (pdu_size - (hdr_len_bytes << 3)));
          npdus += math_ceil((lc_buf_ptr->ctrl_data_bytes << 3), 
                             (pdu_size - (hdr_len_bytes << 3)));
          
#ifdef FEATURE_WCDMA_UL_COMPR
          npdus += math_ceil((lc_buf_ptr->new_data_uncomp_bytes << 3), 
                             (pdu_size - (hdr_len_bytes << 3)));
#endif
          lc_buf_ptr->nbytes += (npdus * hdr_len_bytes);

          /*        Need to commented out For TASK LOCK           */
           MSG_9(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH,
          "Rlc_id %d, new_data %d bytes, retx_data %d bytes, ctrl_data %d bytes, npdus %d, pdu_size %d, hdr_len_bytes %d bytes",
          rlc_id, lc_buf_ptr->new_data_bytes, lc_buf_ptr->retx_data_bytes,
          lc_buf_ptr->ctrl_data_bytes, npdus, pdu_size, hdr_len_bytes, 0, 0);
          
        }
        if (lc_buf_ptr->nbytes > 0)
        {
          MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "TVM:rlc_id %d, buf_status %d",
                                                     rlc_id, lc_buf_ptr->nbytes);
        }
        break;
        
        default:
        MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW, "Traffic volume not computed for TM pdus");
        break;
      } /* switch pdu type */
    } /* for number of logical channels */
  } /* IF the transport channel is valid */
  else
  {
    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC_ERR: Invalid RACH TrCh Id %d in MAC data base",
                                                oc_rachptr->trch_id);
  }
}
}

/*==================================================================
 FUNCTION NAME proc_event_trigger_criteria_cu

 PURPOSE  For all the TrCh which are configured for meas Id 4A,
          TrCh BO is calculated and checked against the threshold
          True - BO > Threshold, FALSE - BO < Threshold
===================================================================*/
boolean proc_event_trigger_criteria_cu
(
/* Measurement identity */
uint16                          meas_id,

#ifdef FEATURE_WCDMA_HS_RACH
/* pointer to HS-Rach config */
mac_hs_rach_config_s_type         *oc_hs_rachptr,
#endif

/* pointer to Rach config */
mac_ul_rach_cctrch_config_type          *oc_rachptr,

/*Internal pointer to  UL Buffer status obtained from RLC */
rlc_ul_buf_status_type                           *int_traffic_ul_buf_status_ptr
)
{
  maci_ul_traffic_trch_event_param         *event_cfg_ptr;
  uint32                                   trch_bo = 0; /* Buffer occupancy */
  uint16                                   i, j,nlchan;
  maci_ul_traffic_cfg_param                *traffic_param_ptr = NULL;
  maci_ul_traffic_cfg_index_type           *traffic_cfg_idx_ptr;
  mac_ul_traffic_event_id_e_type           event_id;
  tr_ch_id_type                            trch_id;
  boolean                                  trch_bo_crossed_threshold = FALSE;
  mac_ul_cctrch_e_type                     trch_type;
  uint16                                   ntrch = 0;
  trch_id_to_idx_map_type                  *id_to_idx_ptr;
  uint8                                    nevents=0;
  rlc_lc_id_type                           rlc_id;
  rlc_ul_lc_buf_status_type                *lc_buf_ptr;
  uint16                                    ndlchn = 0;

#ifdef FEATURE_WCDMA_HS_RACH     
  rlc_lc_id_type                           hs_rach_rlc_id[MAX_DED_LOGCHAN_PER_TRCH];
  boolean                                  hs_rach_active = FALSE;

  if(oc_hs_rachptr != NULL)
  {
    if(mac_hs_rach_info.hs_rach_active == TRUE)
    {
      hs_rach_active = TRUE;
    }
  }
#endif
  
  /*-----------------------------------------------------------------------------------
  Get the traffic config pointer and the intermediate measurement results corresponding
  to this measurement identity.
  ------------------------------------------------------------------------------------*/
  if (TRAFFIC_MEAS_ID_IS_INVALID(meas_id))
  {
    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Invalid measurement ID %d",
                                                meas_id);
  }
  else
  {
    traffic_cfg_idx_ptr = GET_UL_TRAFFIC_CFG_PARAM_PTR(meas_id);
    traffic_param_ptr = traffic_cfg_idx_ptr->traffic_cfg_ptr;
  
    if ((traffic_cfg_idx_ptr->valid == FALSE) || (traffic_param_ptr == NULL))
    {
      MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Entry with invalid parameters in TVM info table");
      return FALSE;
    }  
    
    /* If the event transport channel list is included */
    if (traffic_param_ptr->event_trch_incl)
    {
      ntrch = traffic_param_ptr->event_num_trch;
    }
    else
    {
      /*--------------------------------------------------------------------------------
      If the event transport channel list is not included then apply the criteria to
      the transport channles listed in the measurement object list.
      --------------------------------------------------------------------------------*/
      ntrch = traffic_param_ptr->num_trch;
    }
  }
  
  /* go through the list of transport channels */
  for (i = 0; ((i < ntrch) && (i < UE_MAX_TRCH) && (traffic_param_ptr!=NULL)); i++)
  {
    /* Initialise */
    trch_bo = 0;
    trch_bo_crossed_threshold = FALSE;
    
    /* if the transport channel list was specified by RRC */
    if (traffic_param_ptr->event_trch_incl)
    {
      /* TRUE implies the event criteria was specified for each TrCh in the list */
      event_cfg_ptr = traffic_param_ptr->trch_event_param[i];

      /* Get the transport channel id and type*/
      trch_id = traffic_param_ptr->event_trch_info[i].trch_id;
      trch_type = traffic_param_ptr->event_trch_info[i].trch_type;
      nevents = (uint8)traffic_param_ptr->num_events[i];
    }
    else
    {
      /* FALSE implies only one event criteria is specified and it is applicable 
       * to all TrChs specified in the object list 
       */
      event_cfg_ptr = traffic_param_ptr->trch_event_param[0];
      nevents = (uint8)traffic_param_ptr->num_events[0];

      /*-------------------------------------------------------------------------------
      When the set of Transport channels on which the event trigger is defined is not
      provided explicitly, the UE shall apply it on the Transport channels specified
      by the measurement object.
      -------------------------------------------------------------------------------*/
      /* Get the transport channel id */
      trch_id = traffic_param_ptr->trch_info[i].trch_id;
      trch_type = traffic_param_ptr->trch_info[i].trch_type;
    }

    ndlchn = 0;
#ifdef FEATURE_WCDMA_HS_RACH
    if(hs_rach_active == TRUE)
    {
      /* If HS-RACH is active, measureemnts are valid only for EDCH TrCh types */
      if(trch_type == EDCH)
      {
        /* If HS-RACH is configured, compute no of dedicated logCh mapped on it 
           Also get RLC IDs of logChs mapped on HS-RACH */
        ndlchn = populate_hs_rach_log_chan_info(oc_hs_rachptr, hs_rach_rlc_id);
        if(tvm_debug == 1)
        {
          MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MAC_TVM: LCs mapped on EDCH %d, trch idx %d",
                                                     ndlchn, i);
        }
      }
      else
      { /* do nothing - ndlchn is zero */
        MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW, "MAC_TVM: HS_RACH - Invalid TrCh type - %d, index - %d",
                                                  trch_type, i);
      }
    }
    else
#endif
    {
      /* Check the TVM only for RACH */
      if ((trch_type == GET_UL_CCTRCH_TYPE) && (trch_type == RACH) 
           && (!TRCH_ID_TO_IDX_IS_INVALID(trch_id)))
      {
        id_to_idx_ptr = GET_UL_TRCH_ID_TO_IDX_PTR(trch_id);
        if (id_to_idx_ptr->valid)
        {
          ndlchn = oc_rachptr->ndlchan;
        }
      }
    }

    for (nlchan = 0; ((nlchan < ndlchn) && (nlchan < MAX_DED_LOGCHAN_PER_TRCH)); 
      nlchan++)
   {
  #ifdef FEATURE_WCDMA_HS_RACH
     if(hs_rach_active == TRUE)
     {
       rlc_id = hs_rach_rlc_id[nlchan];
     }
     else
  #endif
     {
       rlc_id = oc_rachptr->dlchan_info[nlchan].rlc_id;
     }
     lc_buf_ptr = &(int_traffic_ul_buf_status_ptr->chan_info[rlc_id]);

     /* PDU type is either acknowledged mode or unacnowledged mode */
     if (lc_buf_ptr->type == FLEX_PDUS)
     {
       trch_bo += lc_buf_ptr->nbytes;
     }  /* if UM or AM PDU */
   } /* for number of logical channels */

   /* for number of events corresponding to the transport channel.could be 1 or 2 */
   for (j = 0; (j < nevents) && (j < MAX_NUM_EVENTS); j++)
   {
     event_id = event_cfg_ptr[j].event_id;
     if (event_id == EVENT_4A)
     {
       /*----------------------------------------------------------------------------
          If the event_id is 4A and if the transport channel BO is greater than the 
          threshold then trch_bo_crossed_threshold is set to TRUE.
          Return, if any one TrCh crosses the threshold
          ----------------------------------------------------------------------------*/
       trch_bo_crossed_threshold = ((trch_bo > event_cfg_ptr[j].report_threshold) ? TRUE: FALSE);

       MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MAC_TVM: TrCh_BO %d, report_threshold %d, Trch_bo_crossed_threshold %d",
                                                  trch_bo,
                                                  event_cfg_ptr[j].report_threshold,
                                                  trch_bo_crossed_threshold);
        
       if (trch_bo_crossed_threshold)
       {
         return trch_bo_crossed_threshold;
       }                 
     } /* if event is E4A */
     
   } /* for event list */
   
  }/* for number of transport channels */
  
  return trch_bo_crossed_threshold;
} /* proc_event_trigger_criteria_cu */

/*================================================================
 FUNCTION NAME  mac_compute_traffic_volume_above_threshold_cu

 PURPOSE  This function will be called by RRC when it needs to set the IE 
          "Traffic volume indicator", 
          if the IE "Cell update cause" is set to "uplink data transmission" 
          and Traffic volume event identity = "4a".
          TRUE - If BO > Threshold
          FALSE - If BO < Threshold
===================================================================*/
boolean mac_compute_traffic_volume_above_threshold_cu
(
/* measurement id */
uint16                             meas_id,
/* pointer to Rach config */
mac_ul_rach_cctrch_config_type    *oc_rachptr
#ifdef FEATURE_WCDMA_HS_RACH
/* pointer to HS-Rach config */
, mac_hs_rach_config_s_type         *oc_hs_rachptr
#endif
)
{
  boolean                    ret_value = FALSE;
  rlc_ul_buf_status_type     int_traffic_ul_buf_status; 
  
  /* In case of TVM */
  if (meas_id == 4)
  {
    if ((GET_UL_CCTRCH_TYPE == INVALID_UL_CCTRCH)
#ifdef FEATURE_WCDMA_HS_RACH
         &&(mac_hs_rach_info.hs_rach_active == FALSE)
#endif
       )
    {
      MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC UL transport channels not configured");
      return ret_value;
    }     
    /* Compute the raw payload for all the configured RBs */
    compute_obj_list_raw_payload_cu
    (
  #ifdef FEATURE_WCDMA_HS_RACH
        oc_hs_rachptr, 
  #endif
        oc_rachptr, 
        &int_traffic_ul_buf_status
    );
    
    /* Check whether the BO of any TrCh is greater than Threshold */
    ret_value = proc_event_trigger_criteria_cu
                (
                  meas_id, 
              #ifdef FEATURE_WCDMA_HS_RACH  
                  oc_hs_rachptr, 
              #endif
                  oc_rachptr, 
                  &int_traffic_ul_buf_status
                );
  }
  
  return (ret_value);
}


/*========================================================================================
 FUNCTION NAME compute_trch_traffic_volume

 PURPOSE    This function computes the Trasnport channel traffic volume which is sum of
            buffer occupancies of RBs multiplexed onto the transport channel. The function
            returns the number of bytes so computed for the transport channel.
            Returns Buffer occupancy of all rbs multiplexed onto the same transport
            channel
=========================================================================================*/

uint32 compute_trch_traffic_volume
(
/* Compute transport channel traffic volume for this transport channel id */
tr_ch_id_type     trch_id
)
{
  ul_trch_info_type                   *ul_trch_info_ptr;
  uint16                              nlchan;
  rlc_lc_id_type                      rlc_id;
  rlc_ul_lc_buf_status_type           *lc_buf_ptr;
  ul_lc_info_type                     *lc_info_ptr;
  trch_id_to_idx_map_type             *id_to_idx_ptr;
  uint32                              bo=0;

  /*------------------------------------------------------------------------------
  Get the index into the table where the transport channel information for this
  trch_id is stored.
  ------------------------------------------------------------------------------*/
  if (!TRCH_ID_TO_IDX_IS_INVALID(trch_id))
  {
    id_to_idx_ptr = GET_UL_TRCH_ID_TO_IDX_PTR(trch_id);
    
    if (!id_to_idx_ptr->valid)
    {
      MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW, "Transport channel invalid, trch_id %d",
                                                trch_id);
    }
    else
    {
      /*----------------------------------------------------------------------------
      Get the transport channel information corresponding to this transport channel
      id.
      ----------------------------------------------------------------------------*/
      ul_trch_info_ptr = GET_UL_TRCH_INFO_PTR (id_to_idx_ptr->idx);
  
      /*----------------------------------------------------------------------------
      Go through all the logical channels that are mapped to this transport channel
      ----------------------------------------------------------------------------*/
      for (nlchan=0; nlchan < ul_trch_info_ptr->nlchan &&
          nlchan < UE_MAX_UL_LOGICAL_CHANNEL; nlchan++)
      {
        /* Get the logical channel id */
        rlc_id = ul_trch_info_ptr->rlc_id[nlchan];
  
        /* Get the logical channel information */
        lc_info_ptr = GET_UL_LC_INFO_PTR(rlc_id);
  
        /* Logical channel enabled */
        if (lc_info_ptr->enable)
        {
          lc_buf_ptr = &traffic_ul_buf_status.chan_info[rlc_id];
  
          /* PDU type is either acknowledged mode or unacnowledged mode */
          if (lc_buf_ptr->type == FLEX_PDUS)
          {
            bo += lc_buf_ptr->nbytes;
  
          }  /* if UM or AM PDU */
  
        }  /*logical channel enabled */
  
      } /* for number of logical channels */
  
    }/*if transport channel valid */
  }
  return bo;
}

/*========================================================================================
 FUNCTION NAME  prepare_n_send_traffic_meas_report()

 PURPOSE        This function prepares the measurement report for the measurement id
                specified. This function calculates the average, variance or raw payload
                as specified in the report quantity.

=========================================================================================*/

void prepare_n_send_traffic_meas_report
(
/* Measurement identity */
uint16                          meas_id
)
{
  uint16                          i,j,rb_idx,nsamples;
  uint16                          fr_idx, cnt,idx;
  uint32                          avg;
  uint32                          sum,square_sum,var,raw_payload;
  uint16                          ntrch = 0;
  trch_id_to_idx_map_type         *id_to_idx_ptr;
  maci_ul_traffic_cfg_param       *int_traffic_param_ptr;
  ul_trch_info_type               *trch_info_ptr = NULL;
  rlc_lc_id_type                  rlc_id;
  ul_lc_info_type                 *lcinfo_ptr = NULL;
  rb_id_type                      rb_id;
  uint16                          from_index,meas_index;
  tr_ch_id_type                   trch_id;
  maci_ul_traffic_cfg_index_type  *traffic_cfg_idx_ptr;
  uint8                            nlchan;
  boolean                          lc_enabled = FALSE;
  uecomdef_logch_mode_e_type       rlc_mode;
#ifdef FEATURE_WCDMA_HS_RACH
  rlc_ul_lc_info_type         *lc_info_prt = NULL;
  mac_hs_ul_logical_ch_mac_d_flow_info_type *hs_lc_macd_flow_map = NULL;
#endif

  memset (&meas_ind,0,sizeof(mac_ul_traffic_meas_ind_type));
  
  if (TRAFFIC_MEAS_ID_IS_INVALID(meas_id))
  {
    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Invalid measurement ID %d",
                                                meas_id);
  }
  else
  {
    /*--------------------------------------------------------------------------------
    Get the traffic config pointer and the intermediate measurement results
    corresponding to this measurement identity.
    --------------------------------------------------------------------------------*/
    traffic_cfg_idx_ptr = GET_UL_TRAFFIC_CFG_PARAM_PTR(meas_id);
    int_traffic_param_ptr   = traffic_cfg_idx_ptr->traffic_cfg_ptr;
  
    meas_ind.meas_id = meas_id;
    /*--------------------------------------------------------------------------------
    Copy the report mode. This can be either periodic or event triggered.
    --------------------------------------------------------------------------------*/
    meas_ind.report_mode = int_traffic_param_ptr->report_mode;
  
    /*--------------------------------------------------------------------------------
    Copy the report transfer mode. This can be either unacknowledged or acknowledged
    --------------------------------------------------------------------------------*/
    meas_ind.report_transfer_mode = int_traffic_param_ptr->transfer_mode;
  
    /*--------------------------------------------------------------------------------
    Initialize number of rb's to 0.
    --------------------------------------------------------------------------------*/
    meas_ind.num_rbs = 0;
  
    /*--------------------------------------------------------------------------------
    Initialize num_reports_ind to FALSE. This is set to true when we have reached
    the maximum number of reports that is required to send.
    --------------------------------------------------------------------------------*/
    meas_ind.num_reports_ind = FALSE;
  
    /*--------------------------------------------------------------------------------
    If the object list is included then send report for only those transport channels
    which are specified in the object list.
    --------------------------------------------------------------------------------*/
    ntrch = int_traffic_param_ptr->num_trch;
  
    /*-----------------------------------------------------------------------
    If the mode is periodic and the measurement interval (given by nsamples*10)
    is greater than the report interval, then the first time we send a meas
    report is treated as a special case.
    -----------------------------------------------------------------------*/
    /*--------------------------------------------------------------------
    This will take care of the special case when the measurement
    interval is greater than the report interval and this is the first
    time it is happening since start of the measurements for this ID.
    --------------------------------------------------------------------*/
    /*---------------------------------------------------------------------
    If an event was triggered and we were asked to report the average or
    variance based on nsamples and if we don't have nsamples information
    (this can happen when an event is triggered in the first 20 to 30 ms
    since we start doing measurements) then we compute average or variance
    based on the number of samples we currently have.
    ----------------------------------------------------------------------*/
    
    if (int_traffic_param_ptr->first_meas)
    {
      if (rolled_over || (int_traffic_param_ptr->current_idx>=
                          int_traffic_param_ptr->nsamples))
      {
        int_traffic_param_ptr->first_meas = FALSE;
        nsamples = int_traffic_param_ptr->nsamples;
      }
      else
      {
        nsamples = int_traffic_param_ptr->current_idx;
      }
    }
    else
    {
      /*----------------------------------------------------------------------
      Get the number of samples that needs to be taken for computing the
      average or variance
      -----------------------------------------------------------------------*/
      nsamples = int_traffic_param_ptr->nsamples ;
    }

    for (i=0; (i < ntrch) && (i < UE_MAX_TRCH)  ; i++)
    {
      nlchan = 0;
 #ifdef FEATURE_WCDMA_HS_RACH
      if(mac_hs_rach_info.hs_rach_active == TRUE)
      {
        if(int_traffic_param_ptr->trch_info[i].trch_type == EDCH)
        {
          lc_info_prt = get_mac_hs_ul_lc_info_ptr();
          nlchan = lc_info_prt->nchan;
        }
        else
        {  /* do nothing - nlchan is zero */
        }
      }
      else
 #endif
      {
        trch_id = int_traffic_param_ptr->trch_info[i].trch_id;
        /*----------------------------------------------------------------------------
        Get the index into the transport channel info table for this transport channel
        identity.
        -----------------------------------------------------------------------------*/
        if (!TRCH_ID_TO_IDX_IS_INVALID(trch_id))
        {
          /*---------------------------------------------------------------------------
          Get the transport channel information pertaining to this transport channel id.
          ----------------------------------------------------------------------------*/
          id_to_idx_ptr = GET_UL_TRCH_ID_TO_IDX_PTR(trch_id);
          if ((int_traffic_param_ptr->trch_info[i].trch_type==GET_UL_CCTRCH_TYPE)
              &&(id_to_idx_ptr->valid))
          {
            trch_info_ptr = GET_UL_TRCH_INFO_PTR(id_to_idx_ptr->idx);
            nlchan = trch_info_ptr->nlchan;
          }
          else
          {  /* do nothing - nlchan is zero */
          }
        }
        else
        {  /* do nothing - nlchan is zero */
        }
      }
      
      /*---------------------------------------------------------------------------
      Go through the list of logical channels which are mapped onto this transport
      channel.
      ----------------------------------------------------------------------------*/
      for (j=0;j< nlchan && j < UE_MAX_UL_LOGICAL_CHANNEL;j++)
      {
        /* Intialize them to 0 */
        sum=avg=square_sum = var= raw_payload=0;
        
        /*-------------------------------------------------------------------------
        Get the logical channel information for this rlc_id.
        --------------------------------------------------------------------------*/
#ifdef FEATURE_WCDMA_HS_RACH
        if(mac_hs_rach_info.hs_rach_active == TRUE)
        {
          rlc_id = lc_info_prt->rlc_id[j];
          hs_lc_macd_flow_map = get_mac_hs_logch_macd_flow_mapping_ptr();
          lc_enabled = hs_lc_macd_flow_map[rlc_id].logical_ch_enabled;
        }
        else
#endif
        {
          rlc_id = trch_info_ptr->rlc_id[j];
          lcinfo_ptr = GET_UL_LC_INFO_PTR(rlc_id);
          lc_enabled = lcinfo_ptr->enable;
        }
        
        /*-------------------------------------------------------------------------
        If the logical channel is enabled.
        --------------------------------------------------------------------------*/
        if (lc_enabled)
        {
          /*-----------------------------------------------------------------------
          Get the radio bearer id to which this logical channel is mapped.
          If this a TM radio bearer then do not attach this RB in the measurement
            report list
          ------------------------------------------------------------------------*/
#ifdef FEATURE_WCDMA_HS_RACH
          if(mac_hs_rach_info.hs_rach_active == TRUE)
          {
            rb_id = hs_lc_macd_flow_map[rlc_id].rb_id;
            rlc_mode = hs_lc_macd_flow_map[rlc_id].rlc_mode;
          }
          else
#endif
          {
            rb_id = lcinfo_ptr->rb_id;
            rlc_mode = lcinfo_ptr->rlc_mode;
          }

          if (rb_id == 0) continue;

          if (rlc_mode == UE_MODE_TRANSPARENT)
          {
            continue;
          }
    
          /*-----------------------------------------------------------------------
          Get the radio bearer index where we need to store the results of the
          computations.
          ------------------------------------------------------------------------*/
          rb_idx = meas_ind.num_rbs;
          
          /*-----------------------------------------------------------------------
          Store the radio bearer identity.
          ------------------------------------------------------------------------*/
          if (int_traffic_param_ptr->rlc_payload || int_traffic_param_ptr->avg_payload
              || int_traffic_param_ptr->var_payload)
          {
            if (rb_idx < MAX_RB)
            {
              meas_ind.rb_meas[rb_idx].rb_id = rb_id;
            }
            else
            {
              MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "num of RBs %d exceeded max rb_meas bound",
                                                          meas_ind.num_rbs);
            }
          
            /*--------------------------------------------------------------------------
            Increment the number of radio bearers.
            ---------------------------------------------------------------------------*/
            meas_ind.num_rbs++;
          }
    
          meas_index = int_traffic_param_ptr->current_idx;
          
          /*-----------------------------------------------------------------------
          Initialize raw, average, variance to highest possible value. This
          variables are populated depending on which quantity is required.
          ------------------------------------------------------------------------*/
          if (rb_idx < MAX_RB)
          {
            meas_ind.rb_meas[rb_idx].raw_rlc_payload = 0xFFFFFFFF;
            meas_ind.rb_meas[rb_idx].variance_rlc_payload = 0xFFFFFFFF;
            meas_ind.rb_meas[rb_idx].average_rlc_payload  = 0xFFFFFFFF;
          }
          else
          {
            MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC_ERR: num of RBs %d exceeded max rb_meas bound, rb_idx %d",
                                                        meas_ind.num_rbs,
                                                        rb_idx);
          }
          /*-----------------------------------------------------------------------
          If the raw rlc payload information is requested
          ------------------------------------------------------------------------*/
          if (int_traffic_param_ptr->rlc_payload)
          {
            /*---------------------------------------------------------------------
            Obtain the information stored in the last entry in the array. This
            entry is given subtracting 1 from the cur_index (
            cur_index - is the location where a new payload entry has to be stored).
            ----------------------------------------------------------------------*/
            idx = TRAFFIC_IDX_DIFF(meas_index,1);
            if ((idx < TRAFFIC_MAX_SAMPLES) && (rb_id < MAX_RB) && (rb_idx < MAX_RB))
            {
              meas_ind.rb_meas[rb_idx].raw_rlc_payload = rb_raw_payload[rb_id][idx];
              MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW, "RB_ID RAW_PAYLOAD %d %d",
                                                        rb_id,
                                                        rb_raw_payload[rb_id][idx]);
            }
          }

          /*-----------------------------------------------------------------------
          If the average or variance rlc payload information is requested
          ------------------------------------------------------------------------*/
          if (int_traffic_param_ptr->avg_payload || int_traffic_param_ptr->var_payload)
          {
            /*----------------------------------------------------------------------
              obtain the "from_index" from where we need to start calculations
              for obtaining the average or variance.
            -----------------------------------------------------------------------*/
            from_index = TRAFFIC_IDX_DIFF(meas_index, nsamples);

            /*----------------------------------------------------------------------
              Compute average.
            -----------------------------------------------------------------------*/
            if (nsamples==0)
            {
              MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW, "Interval is zero");
            }
            else
            {
              /*-------------------------------------------------------------------
              rb_raw_payload is used as a circular array containing
              TRAFFIC_MAX_SAMPLES. from_index gives the index from where we should
              start doing the calculations. interval gives
              me the number of elements needed to do the average.
              --------------------------------------------------------------------*/
              for (cnt=0, fr_idx=from_index ; ((cnt<nsamples) && (rb_id < MAX_RB));
                    cnt++,fr_idx++)
              {
                fr_idx = fr_idx%TRAFFIC_MAX_SAMPLES;

                raw_payload = rb_raw_payload[rb_id][fr_idx];
                sum += raw_payload;
                MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW, "RAW PAYLOAD %d RB_ID %d",
                                                          raw_payload, rb_id);
                if (int_traffic_param_ptr->var_payload)
                {
                  square_sum += raw_payload*raw_payload;
                }
              }
              /*-----------------------------------------------------------------------
              Calculate the average rlc payload. Average is given by dividing the
              sum_rlc_payload by the number of samples. Number of samples is calculate
              by dividing the nsamples
              -----------------------------------------------------------------------*/
              avg = sum/nsamples;

              /*----------------------------------------------------------------------
              Compute variance.
              -----------------------------------------------------------------------*/
              if ((int_traffic_param_ptr->var_payload) && (rb_idx < MAX_RB))
              {
                var = (square_sum/nsamples) - (avg*avg);
                meas_ind.rb_meas[rb_idx].variance_rlc_payload = var;
                MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW, "RB_ID VAR_PAYLOAD %d %d",
                                                          rb_id, var);
              }
              /*----------------------------------------------------------------------
              Compute average.
              -----------------------------------------------------------------------*/
              if ((int_traffic_param_ptr->avg_payload) && (rb_idx < MAX_RB))
              {
                meas_ind.rb_meas[rb_idx].average_rlc_payload = avg;
                MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW, "RB_ID AVG_PAYLOAD %d %d",
                                                          rb_id, avg);
              }
            }/* nsamples>0*/

          }/*if we are asked to report either avergae or variance */

        }/*logical channel enabled */
        
      }/* for number of logical channels */
      
    }/* for number of transport channels */
  
    if (prepare_addtl_meas_incl == TRUE)
    {
      prepare_addtl_meas_incl = FALSE;
      return;
    }
    if (int_traffic_param_ptr->report_mode==PERIODIC_MODE)
    {
      /*-------------------------------------------------------------------------------
      If there is atleast one radio bearer for which results have to be reported then
      report. Else don't send the report.
      -------------------------------------------------------------------------------*/
      /*if (meas_ind.num_rbs>0) */
      {
        /*----------------------------------------------------------------------------
        If we have sent the number of reports that RRC has requested us to send then
        indicate to RRC that this will be the last report that we would eb sending
        for this measurement id in the case of periodic mode.
        -----------------------------------------------------------------------------*/
        int_traffic_param_ptr->nreports_sent++;
  
        if ((!int_traffic_param_ptr->report_infinity) &&
            (int_traffic_param_ptr->nreports_sent == int_traffic_param_ptr->num_reports))
        {
          meas_ind.num_reports_ind = TRUE;
        }
        /*----------------------------------------------------------------------------
        Post the measurement results to RRC.
        -----------------------------------------------------------------------------*/
        maci_post_traffic_volume_meas_report(&meas_ind);
        mac_ul_send_tvm_report_packet(&meas_ind, int_traffic_param_ptr->nreports_sent);
      }
    }
    else if (int_traffic_param_ptr->report_mode == EVENT_TRIGGERED_MODE) /*event_triggered_mode */
    {
      /*if (meas_ind.num_rbs>0)*/
      {
        WCDMA_MEMCPY(&meas_ind.event_result, 
                     sizeof(mac_ul_traffic_event_result),
                     &int_traffic_param_ptr->event_result,
                     sizeof(mac_ul_traffic_event_result));
        /*-----------------------------------------------------------------------------
        Post the measurement results to RRC.
        ------------------------------------------------------------------------------*/
        maci_post_traffic_volume_meas_report(&meas_ind);
  
        mac_ul_send_tvm_report_packet(&meas_ind, 0);
  
      }
    }
  
    /*--------------------------------------------------------------------------------
     Clear any old measurements associated with this measurement id.
    ---------------------------------------------------------------------------------*/
    memset (&int_traffic_param_ptr->event_result,0,sizeof(mac_ul_traffic_event_result));
  }
  
}

/*-------------------------------------------------------------------
FUNCTION mac_get_additional_traffic_vol_msmt_results

DESCRIPTION
This function would be called by RRC when it needs to append a TVM report
to one of intrafrequency or interfrequency measurement reports.

DEPENDENCIES
  None

RETURN VALUE
addtl_meas_ind contains the measurement results. Usually the RLC payload
information is sent back for all the RB's mapped onto a particular
transport channel.

SIDE EFFECTS
  None
--------------------------------------------------------------------*/
boolean
mac_get_additional_traffic_vol_msmt_results
(
uint16 meas_id,
mac_ul_traffic_meas_ind_type *addtl_meas_ind
)
{
  maci_ul_traffic_cfg_index_type    *int_cfg_param_ptr;
  boolean                           status=FALSE;

  if (TRAFFIC_MEAS_ID_IS_INVALID(meas_id))
  {
    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Invalid measurement ID %d",
                                                meas_id);
  }
  else
  {
    int_cfg_param_ptr = GET_UL_TRAFFIC_CFG_PARAM_PTR(meas_id);
  
    if (int_cfg_param_ptr->valid)
    {
      status = TRUE;
      prepare_addtl_meas_incl = TRUE;
      prepare_n_send_traffic_meas_report(meas_id);

      WCDMA_MEMCPY(addtl_meas_ind, 
                   sizeof(mac_ul_traffic_meas_ind_type),
                   &meas_ind, 
                   sizeof(mac_ul_traffic_meas_ind_type));
    }
  }
  
  return status;
}


void proc_event_trigger_criteria_enhanced
(
  uint16                                   meas_id,
  tr_ch_id_type                            trch_id,
  mac_ul_cctrch_e_type                     trch_type,
  uint8                                    nevents,
  uint32                                   trch_bo,
  maci_ul_traffic_trch_event_param         *event_cfg_ptr,
  maci_ul_traffic_trch_event_param         *cur_evt_cfg_ptr
)
{
  uint16                                   j;
  maci_ul_traffic_cfg_param                *traffic_param_ptr;
  maci_ul_traffic_cfg_index_type           *traffic_cfg_idx_ptr;
  boolean                                  send_status, prohibit_status;
  mac_ul_traffic_event_id_e_type           event_id;
  boolean                                  trch_bo_crossed_threshold=FALSE;
  boolean                                  prev_bo_crossed_threshold=FALSE;
  boolean                                  crossed_thrshld_aftr_prv_evnt_trigrd = FALSE;
  boolean                                 mrab_send_status;
  
  mrab_send_status = TRUE;
  
  if (TRAFFIC_MEAS_ID_IS_INVALID(meas_id))
  {
    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Invalid measurement ID %d",
                                                meas_id);
    return;
  }
  
  traffic_cfg_idx_ptr = GET_UL_TRAFFIC_CFG_PARAM_PTR(meas_id);
  traffic_param_ptr = traffic_cfg_idx_ptr->traffic_cfg_ptr;
  
  /* for the number of events corresponding to the transport channel. could be 1 or 2 */
  for (j=0;j< nevents && j < MAX_NUM_EVENTS;j++)
  {
    /*  Initialize send_status to FALSE at the start of processing of each event  */
    send_status = FALSE;
    /*             Initialize prohobit_status to TRUE at the start of processing of each event  */
    prohibit_status=TRUE;
    /*  Get the event id  */
    event_id = event_cfg_ptr[j].event_id;
    
    if (event_cfg_ptr[j].prohibit_tmr_cnt)
    {
      ++cur_evt_cfg_ptr[j].prohibit_tmr_cnt;
    }
    
    #ifdef FEATURE_WCDMA_MRAB_TVM_MAC_OPT
    #error code not present
#endif /* FEATURE_WCDMA_MRAB_TVM_MAC_OPT */
    {
      /*------------------------------------------------------------------------------
       If the trigger prohibit timer is active then check if prohibit_tmr_cnt has
        expired.
        ------------------------------------------------------------------------------*/
      if (event_cfg_ptr[j].prohibit_tmr_cnt)
      {
        /* Check if the timer has expired */
        if ((cur_evt_cfg_ptr[j].prohibit_tmr_cnt>0) &&((cur_evt_cfg_ptr[j].prohibit_tmr_cnt*10)>
                            event_cfg_ptr[j].trigger_prohibit_tmr))
        {
          /*-------------------------------------------------------------------------
            If the prohibit timer has expired then we can send a report if other
            conditions relating to buffer occupancy and trigger timer are met.
            --------------------------------------------------------------------------*/
          cur_evt_cfg_ptr[j].prohibit_tmr_cnt=0;
          MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "PendingTmr exp'd evtid %d trid %d",
                                                     event_id,
                                                     (uint32)trch_id);
          prohibit_status = FALSE;
        }
      }
      else
      {
        prohibit_status = FALSE;
      }
    }
    if(tvm_debug == 1)
    {
      MSG_9(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH,
        "HS_RACH_TVM: j %d, event_id %d, prohibit_tmr_cnt %d, trigger_prohibit_tmr %d, send_status %d, prohibit_status %d, not_1st_meas %d, event_id %d ",
         j, event_id, cur_evt_cfg_ptr[j].prohibit_tmr_cnt, event_cfg_ptr[j].trigger_prohibit_tmr,
         send_status,prohibit_status,  cur_evt_cfg_ptr[j].evt_not_first_meas, event_id, 0);
    }
    
    /*----------------------------------------------------------------------------
      If the event_id is 4A and if the transport channel buffer occupancy is
      greater than the threshold. Likewise if the event_id is 4B and if
      the buffer occupancy is lesser than the threshold then
      trch_bo_crossed_threshold is set to TRUE;
     ----------------------------------------------------------------------------*/
    trch_bo_crossed_threshold = (((event_id==EVENT_4A)&& (trch_bo > event_cfg_ptr[j].report_threshold)) ||
                                 ((event_id==EVENT_4B)&& (trch_bo<event_cfg_ptr[j].report_threshold)));
    
    /*----------------------------------------------------------------------------
    If the event_id is 4A and if the previous transport channel buffer occupancy is
    lesser than the threshold. Likewise if the event_id is 4B and if the previous
    buffer occupancy is greater  than the threshold then
    prev_bo_crossed_threshold is set to TRUE; An event is triggered only if both
    trch_bo_crossed_threshold and prev_bo_crossed_threshold is set to true.
    ----------------------------------------------------------------------------*/
    prev_bo_crossed_threshold = (((event_id==EVENT_4A) && (cur_evt_cfg_ptr[j].prev_bo <= event_cfg_ptr[j].report_threshold)) ||
                                  ((event_id==EVENT_4B)&&( cur_evt_cfg_ptr[j].prev_bo >= event_cfg_ptr[j].report_threshold )));
    
    /*The event could be triggered again only if TCTV becomes (4A-smaller - TCTV < Reporting threshold, 4B-Greater) than the Reporting threshold 
     * and later (4A- Greater::TCTV > Reporting threshold : 4B- Smaller::TCTV < Reporting) verified again    */
     
    crossed_thrshld_aftr_prv_evnt_trigrd = (((event_id==EVENT_4A)&& (trch_bo < event_cfg_ptr[j].report_threshold)) ||
                                            ((event_id==EVENT_4B)&& (trch_bo > event_cfg_ptr[j].report_threshold )));
    
    /* Make sure the threashold crosssed - Next Event can be triggered */
    if((crossed_thrshld_aftr_prv_evnt_trigrd == TRUE) || (!prohibit_status))
    {
      cur_evt_cfg_ptr[j].evt_not_first_meas = FALSE;
      if(tvm_debug == 1)
      {
        MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MAC_TVM: Next measurement can be triggered- Event id %d , Bo %d, prohibit_status %d",
                                                   event_id,
                                                   trch_bo,
                                                   prohibit_status);
      }
    }
    
    
    // If the immediate Event triggering is not possible don't go for further computations
    send_status = ( (TRUE == mrab_send_status) && 
                    ((!cur_evt_cfg_ptr[j].evt_not_first_meas) || 
                     (cur_evt_cfg_ptr[j].first_event_trch_id != trch_id)));
    
    if(cur_evt_cfg_ptr[j].first_event_trch_id != trch_id)
    {
      if(tvm_debug == 1)
      {
        MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Other Trch event triggered trch_id %d, prev_trch_id %d ",
                                                   trch_id,
                                                   cur_evt_cfg_ptr[j].first_event_trch_id);
      }
    }
    
    if (send_status)
    {
      /* transport channel buffer occupancy has crossed threshold value */
      if (trch_bo_crossed_threshold)
      {
         /*---------------------------------------------------------------------------
         If the pending time after trigger timer expires and if the current transport
         channel traffic volume is greater/event_4A or lesser/event_4B threshold then
         it is time to send a report. or if the previous buffer occupancy has crossed
         threshold value then send_status is set to true depending on the value of the
         time to trigger.
         ----------------------------------------------------------------------------*/
         if (prev_bo_crossed_threshold)
         {
           if(tvm_debug == 1)
          {
            MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "TVM_DBG: TrCh BO crossed threshold %d, prev_bo_crossed %d, trigger %d",
                                                       trch_bo_crossed_threshold,
                                                       prev_bo_crossed_threshold,
                                                       crossed_thrshld_aftr_prv_evnt_trigrd);
          }
           /*-------------------------------------------------------------------------
           If the time_to_trigger value is sepcified and > 0 then start the trigger
           timer count.
           -------------------------------------------------------------------------*/
           if (event_cfg_ptr[j].time_to_trigger > 0)
           {
                  /* Increment the trigger_tmr_cnt */
             ++cur_evt_cfg_ptr[j].trigger_tmr_cnt;
             
             MSG_9(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH,
               "TimeToTrigger started evtid %d trid %d, Prev_bo = %d, cur_bo=%d, Trch_id = %d",
               event_id,(uint32)trch_id,cur_evt_cfg_ptr[j].prev_bo,
               trch_bo,trch_id, 0,
               0, 0, 0);
                       
             send_status=FALSE;
           }
           /* if the time_to_trigger is absent*/
           else if (event_cfg_ptr[j].time_to_trigger==0)
           {
             /* All the conditions are met. We can send a report now*/
             send_status=TRUE;
             cur_evt_cfg_ptr[j].evt_not_first_meas = TRUE;
             //cur_evt_cfg_ptr[j].first_event_trch_id = trch_id;
             MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Prev_bo = %d, cur_bo=%d, trch_id = %d",
                                                        cur_evt_cfg_ptr[j].prev_bo,
                                                        trch_bo,
                                                        trch_id);
           }
         }
         
         /*---------------------------------------------------------------------------
         If the current transport channel buffer occupancy has crossed threshold value
         then check if the time_to_trigger value is > 0. If so then check if the
         trigger_timer_count has expired. If the timer has expired then it is time to
         send a report to RRC.
         ----------------------------------------------------------------------------*/
          else if ((cur_evt_cfg_ptr[j].trigger_tmr_cnt > 0) &&
                   ((++cur_evt_cfg_ptr[j].trigger_tmr_cnt * 10)>event_cfg_ptr[j].time_to_trigger))
          {
            /* it is time to send a report to RRC */
            send_status=TRUE;
            cur_evt_cfg_ptr[j].evt_not_first_meas = TRUE;
            //cur_evt_cfg_ptr[j].first_event_trch_id = trch_id;
            MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "TimeToTrigger exp'd evtid %d trid %d, probhibit_status %d",
                                                       event_id,
                                                       (uint32)trch_id,
                                                       prohibit_status);
            
            /*Initialise the trigger timer count */
            cur_evt_cfg_ptr[j].trigger_tmr_cnt=0;
          }
          /*--------------------------------------------------------------------------
          If the pending time after trigger just expired and if the buffer occupancy
          is greater than the threshold, then trigger a report. Likewise if the
          buffer occupancy is above threshold immediately after we configure then
          trigger a report.
          ----------------------------------------------------------------------------*/
          else if (cur_evt_cfg_ptr[j].trigger_tmr_cnt == 0)
          {
            if (event_cfg_ptr[j].time_to_trigger)
            {
              cur_evt_cfg_ptr[j].trigger_tmr_cnt++;
              MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Evt_First_Meas/PendingTmrExpd: TTT started, %d, %d, %d",
                                                         event_cfg_ptr[j].time_to_trigger,
                                                         cur_evt_cfg_ptr[j].trigger_tmr_cnt,
                                                         trch_id);
              send_status = FALSE;
            }
            else
            {
              MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Evt_First_Meas : trch_bo %d, trch_id %d, prohibit_status %d",
                                                         trch_bo,
                                                         trch_id,
                                                         prohibit_status);
              send_status = TRUE;
              cur_evt_cfg_ptr[j].evt_not_first_meas = TRUE;
              //cur_evt_cfg_ptr[j].first_event_trch_id = trch_id;
            }
          }
          else
          {
            send_status = FALSE;
          }
          
        } /* trch_bo_crossed_threshold*/
        else if (cur_evt_cfg_ptr[j].trigger_tmr_cnt > 0)
        {
          /*  If the transport channel buffer occupancy has not crossed threshold then
           *check if the time to trigger count is running. If so then initialize to zero  */
          
          cur_evt_cfg_ptr[j].trigger_tmr_cnt=0;
          MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "TimeToTrigger stopped evtid %d trchid %d, prohibit_status %d",
                                                     event_id,
                                                     (uint32)trch_id,
                                                     prohibit_status);
          /* Don't send */
          send_status = FALSE;
        }
        else
        {
          send_status = FALSE;
        }
        
    } /*send_status*/
    
    if(tvm_debug == 1)
    {
        MSG_9(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH,
          "trch_bo_cros_thres %d, prev_bo_cros_thresh %d,threshold %d, prev_bo %d, ttt %d, trig_tmr_cnt %d, prohibit_status %d, first_meas %d,f %d ",
           trch_bo_crossed_threshold, prev_bo_crossed_threshold, event_cfg_ptr[j].report_threshold, cur_evt_cfg_ptr[j].prev_bo,
           event_cfg_ptr[j].time_to_trigger,cur_evt_cfg_ptr[j].trigger_tmr_cnt, prohibit_status, cur_evt_cfg_ptr[j].evt_not_first_meas, crossed_thrshld_aftr_prv_evnt_trigrd);
    }
    
    //If Event Prohibit TImer Running dont trigger Event 
    send_status = (send_status & (!prohibit_status));
    
    /*----------------------------------------------------------------------------------
     Store the current buffer occupancy in previous_bo.
     ----------------------------------------------------------------------------------*/
    cur_evt_cfg_ptr[j].prev_bo = trch_bo;
    
    if(tvm_debug == 1)
    {
      MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Send Status %d, prev_bo - %d assigned",
                                                 send_status,
                                                 cur_evt_cfg_ptr[j].prev_bo);
    }
    
    /*------------------------------------------------------------------------------
    If the interrupt timer is running check to see if it has expired. We can resume
    transmission once the interrupt timer has expired.
    -------------------------------------------------------------------------------*/
    if (trch_type == RACH)
    {
      /*--------------------------------------------------------------------------------
      If the transmission is blocked then check to see if the Tx interrupt timer has
      expired. Resume transmissions when the timer has expired.
      ---------------------------------------------------------------------------------*/
      if (block_transmission)
      {
          /*-------------------------------------------------------------------------
          If another event 4a was triggered at the time of expiry of the tx interrupt
          after trigger then MAC is expecting another block rach req from RRC.
          --------------------------------------------------------------------------*/
        if(send_status && (event_id == EVENT_4A))
        {
            MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Event 4A reported, expecting RRC to send block rach req");
          /* Store this timer value which would be used to resume transmissions */
          /* when MAC receives a BLOCK RACH Tx command from RRC */
          // The timer was started because of an earlier event and has a value which
          // is greater than the new tx interrupt timer value then reset the counters
          // otherwise let the earlier timer continue to completion
          if ((tx_interrupt_tmr-(tx_interrupt_cnt*10)) > event_cfg_ptr[j].tx_interrupt_tmr)
          {
            tx_interrupt_tmr = event_cfg_ptr[j].tx_interrupt_tmr;
            tx_interrupt_cnt = 0;
            MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Resetting the timer tx_intp_tmr %d, new_tx_tmr %d, tx_cnt %d",
                                                       tx_interrupt_tmr,
                                                       event_cfg_ptr[j].tx_interrupt_tmr,
                                                       tx_interrupt_cnt);
          }
          else
          {
            // Let the ealier timer continue to run.
            MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Continuing the tx interrupt timer tx_intp_tmr %d, new_tx_tmr %d, tx_cnt %d",
                                                       tx_interrupt_tmr,
                                                       event_cfg_ptr[j].tx_interrupt_tmr,
                                                       tx_interrupt_cnt);
          }
        }
      }
      // If the transmission was not blocked earlier then
      // wait for RRC to send the block command
      else if (send_status && (event_id == EVENT_4A))
      {
        tx_interrupt_tmr = event_cfg_ptr[j].tx_interrupt_tmr;
        tx_interrupt_cnt = 0;
        MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "e4a reported, expecting RRC to send block rach req %d",
                                                   tx_interrupt_tmr);
      }
    }
    else
    {
      block_transmission = FALSE;
      tx_interrupt_tmr = tx_interrupt_cnt = 0;
    }
    
    /*----------------------------------------------------------------------------------
    If the event is triggered then store the value of the event ID which
    caused the trigger. Store the uplink channel type which caused the
    event trigger.
    ----------------------------------------------------------------------------------*/
    if (send_status)
    {
      traffic_param_ptr->current_idx = cur_index;

      traffic_param_ptr->event_result.event_id = event_cfg_ptr[j].event_id;

      /* Store the transport channel id which caused the trigger */
      traffic_param_ptr->event_result.trch_info.trch_id = trch_id;

      /* Store the transport channel type */
      traffic_param_ptr->event_result.trch_info.trch_type = trch_type;

      MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Trigger MR meas_id %d, event_id %d, trch_type %d",
                                                 meas_id,
                                                 event_cfg_ptr[j].event_id,
                                                 trch_type);
      /* send the measurement report immediately */
      prepare_n_send_traffic_meas_report(meas_id);

      /*--------------------------------------------------------------------------------
        If the trigger_prohibit_timer is specified then start the prohibit_timer_count
        since we are sending a report to RRC now. Trigger prohibit timer will prohibit
        the sending of any further reports  to RRC until the timer has expired.
      ---------------------------------------------------------------------------------*/
      if (event_cfg_ptr[j].trigger_prohibit_tmr)
      {
        /* Start the prohibit timer count */
        ++cur_evt_cfg_ptr[j].prohibit_tmr_cnt;

        /* Clear off the previous buffer occupancy */
        cur_evt_cfg_ptr[j].prev_bo=0;

        MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Pending timer started for event %d trchid=%d",
                                                   event_id, (uint32)trch_id);
      }

      // Set the first_meas to false.
      //evt_first_meas=FALSE;
      cur_evt_cfg_ptr[j].evt_not_first_meas = TRUE;
      cur_evt_cfg_ptr[j].first_event_trch_id = trch_id;
    }/* send_status*/
  }
}

/*====================================================================================
 FUNCTION NAME proc_event_trigger_criteria

 PURPOSE  For each transport channel listed in the report criteria for this
          measurement identity this function checks , if the transport channel traffic
          volume is greater than or lesser than the threshold value corresponding to
          the transport channel, based on the event (4A, 4B). It then sets a trigger
          and generates a report to be sent to RRC.

=======================================================================================*/

void proc_event_trigger_criteria
(
/* Measurement identity */
uint16                          meas_id
)
{
  maci_ul_traffic_trch_event_param         *event_cfg_ptr, *cur_evt_cfg_ptr;
  uint32                                   trch_bo=0; /* Buffer occupancy */
  uint16                                   i;
  maci_ul_traffic_cfg_param                *traffic_param_ptr;
  maci_ul_traffic_cfg_index_type           *traffic_cfg_idx_ptr;
  tr_ch_id_type                            trch_id;
  mac_ul_cctrch_e_type                     trch_type;
  uint16                                   ntrch;
  trch_id_to_idx_map_type                  *id_to_idx_ptr;
  uint8                                    nevents=0, cur_evt_cfg_index;
#ifdef FEATURE_WCDMA_HS_RACH
  rlc_lc_id_type                           rlc_id;
  rlc_ul_lc_buf_status_type                *lc_buf_ptr;
  rlc_ul_lc_info_type         *lc_info_ptr;
  uint8                       hsrach_trch_index;
#endif
  
  cur_evt_cfg_index = trch_bo = 0;
  #ifdef FEATURE_WCDMA_HS_RACH
  hsrach_trch_index = 0xFF;
  #endif
  
  /*-----------------------------------------------------------------------------------
  Get the traffic config pointer and the intermediate measurement results corresponding
  to this measurement identity.
  ------------------------------------------------------------------------------------*/
  if (TRAFFIC_MEAS_ID_IS_INVALID(meas_id))
  {
    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Invalid measurement ID %d",
                                                meas_id);
  }
  else
  {
    traffic_cfg_idx_ptr = GET_UL_TRAFFIC_CFG_PARAM_PTR(meas_id);
    traffic_param_ptr = traffic_cfg_idx_ptr->traffic_cfg_ptr;
    
    /* If the event transport channel list is included */
    if (traffic_param_ptr->event_trch_incl)
    {
      ntrch = traffic_param_ptr->event_num_trch;
    }
    else
    {
      /*--------------------------------------------------------------------------------
      If the event transport channel list is not included then apply the criteria to
      the transport channles listed in the measurement object list.
      --------------------------------------------------------------------------------*/
      ntrch = traffic_param_ptr->num_trch;
    }
    
    /* HS-RACH case only the first EDCH object is the valid one 
     * Even NW ocnfigures multiple EDCH objects
     */
    #ifdef FEATURE_WCDMA_HS_RACH
    if(mac_hs_rach_info.hs_rach_active == TRUE)
    {
        hsrach_trch_index = find_hsrach_trch_index(traffic_param_ptr);
        ntrch = 1;
    }
    #endif /* FEATURE_WCDMA_HS_RACH */
    
    /* go through the list of transport channels */
    for (i=0 ; i < ntrch && i < UE_MAX_TRCH ; i++)
    {
      #ifdef FEATURE_WCDMA_HS_RACH
      if(mac_hs_rach_info.hs_rach_active == TRUE)
      {
        if(hsrach_trch_index != 0xFF)
        {
          cur_evt_cfg_index = hsrach_trch_index;
        }
        else
        {
          MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "TVM_ERR: No TrCh Type of EDCH, hsrach_trch_index %d is invalid index ",
                                                      hsrach_trch_index);
          break;
          
        }
      }
      else
      #endif /* FEATURE_WCDMA_HS_RACH */
      {
        cur_evt_cfg_index = i;
      }
      
      if (cur_evt_cfg_index < UE_MAX_TRCH)
      {
        /* if the transport channel list was specified by RRC */
        if (traffic_param_ptr->event_trch_incl)
        {
          /*-------------------------------------------------------------------------------
          TRUE implies the event criteria was specified for each transport channel
          in the list.
          -------------------------------------------------------------------------------*/
          cur_evt_cfg_ptr = event_cfg_ptr = traffic_param_ptr->trch_event_param[cur_evt_cfg_index];
      
          /* Get the transport channel id and type*/
          trch_id = traffic_param_ptr->event_trch_info[cur_evt_cfg_index].trch_id;
          trch_type = traffic_param_ptr->event_trch_info[cur_evt_cfg_index].trch_type;
          nevents = (uint8)traffic_param_ptr->num_events[cur_evt_cfg_index];
        }
        else
        {
          /*-------------------------------------------------------------------------------
          FALSE implies only one event criteria is specified and it is applicable to all
          transport channels specified in the object list.
          -------------------------------------------------------------------------------*/
          event_cfg_ptr = traffic_param_ptr->trch_event_param[0];
          nevents = (uint8)traffic_param_ptr->num_events[0];
      
          cur_evt_cfg_ptr = traffic_param_ptr->trch_event_param[cur_evt_cfg_index];
    
          /*-------------------------------------------------------------------------------
          When the set of Transport channels on which the event trigger is defined is not
          provided explicitly, the UE shall apply it on the Transport channels specified
          by the measurement object.
          -------------------------------------------------------------------------------*/
          /* Get the transport channel id */
          trch_id = traffic_param_ptr->trch_info[cur_evt_cfg_index].trch_id;
          trch_type = traffic_param_ptr->trch_info[cur_evt_cfg_index].trch_type;
        }
      }
      else
      {
        MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Transport Channel Index: %d exceeds maximum number of channels: %d",
                                                    cur_evt_cfg_index,
                                                    UE_MAX_TRCH);
	return;
      }
	  
      /* Compute the Buffer status for each TRCH and check the events report
       * on each Trch with present and previous TTIs BO 
       */
     
      #ifdef FEATURE_WCDMA_HS_RACH
      if(mac_hs_rach_info.hs_rach_active == TRUE)
      {
         lc_info_ptr = get_mac_hs_ul_lc_info_ptr();
         
         /* Compute  BO on the EDCH channel */
         for(i=0; i<lc_info_ptr->nchan; i++)
         {
           rlc_id = lc_info_ptr->rlc_id[i];
           lc_buf_ptr = &traffic_ul_buf_status.chan_info[rlc_id];
           
           /* PDU type is either acknowledged mode or unacnowledged mode */
           if (lc_buf_ptr->type == FLEX_PDUS)
           {
             trch_bo += lc_buf_ptr->nbytes;
           } /* if UM or AM PDU */
         }
         
         if (trch_bo > 0)
         {
           MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MAC_TVM: Trch BO on EDCH %d",
                                                      trch_bo);
         }
         
         proc_event_trigger_criteria_enhanced(meas_id, trch_id, trch_type, nevents, trch_bo, event_cfg_ptr, cur_evt_cfg_ptr);
      }
      else
      #endif /* FEATURE_WCDMA_HS_RACH */
      {
        /* Initialise */
        trch_bo=0;
        
        if (!TRCH_ID_TO_IDX_IS_INVALID(trch_id))
        {
          id_to_idx_ptr = GET_UL_TRCH_ID_TO_IDX_PTR(trch_id);
          
          if ((trch_type==GET_UL_CCTRCH_TYPE)&&(id_to_idx_ptr->valid))
          {
            /*---------------------------------------------------------------------------------
             Compute the transport channel traffic volume.
            ---------------------------------------------------------------------------------*/
            trch_bo = compute_trch_traffic_volume(trch_id);
            
            if (trch_bo > 0)
            {
              MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Trch Id %d, Trch BO %d",
                                                         trch_id, trch_bo);
            }
            proc_event_trigger_criteria_enhanced(meas_id, trch_id, trch_type, nevents, trch_bo, event_cfg_ptr, cur_evt_cfg_ptr);
          }
        }
      } /* If HS RACH is active */
    } /* For all transport channels */
  }
} /* proc_event_trigger_criteria */

/*=======================================================================================
 FUNCTION NAME proc_periodic_mode_criteria

 PURPOSE  This function checks if it is time to send a report based on comparing
          periodic timer count and the report interval. Also it checks if there
          are any limits on the number of reports that MAC can send to RRC.

========================================================================================*/
void proc_periodic_mode_criteria
(
/* Measurement identity */
uint16                          meas_id
)
{

  maci_ul_traffic_cfg_param       *traffic_param_ptr;
  maci_ul_traffic_cfg_index_type  *traffic_cfg_idx_ptr;

  /*-------------------------------------------------------------------------
  Get the traffic config pointer and the intermediate measurement results
  corresponding to this measurement identity.
  -------------------------------------------------------------------------*/
  if (TRAFFIC_MEAS_ID_IS_INVALID(meas_id))
  {
    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Invalid measurement ID %d",
                                                meas_id);
  }
  else
  {
    traffic_cfg_idx_ptr =GET_UL_TRAFFIC_CFG_PARAM_PTR(meas_id);
    traffic_param_ptr   =traffic_cfg_idx_ptr->traffic_cfg_ptr;
    traffic_param_ptr->periodic_tmr_cnt++;
  
    /*-----------------------------------------------------------------------
        Check if this is the time to report. This is done with counters
        (periodic_tmr_cnt). If this counter is not equal to the report_interval
        then wait for the next 10ms, else it is time to send the report to RRC.
    -----------------------------------------------------------------------*/
    MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Timer_count %d, report_interval %d, meas_id %d",
             (traffic_param_ptr->periodic_tmr_cnt *MAC_UL_PERIODIC_TVM_TIMER_SCALE), 
                                               traffic_param_ptr->report_interval,
                                               meas_id);
    
    #ifdef FEATURE_WCDMA_MRAB_TVM_MAC_OPT
    #error code not present
#endif /* FEATURE_WCDMA_MRAB_TVM_MAC_OPT */
    {
      if ((traffic_param_ptr->periodic_tmr_cnt * 
         (10 * MAC_UL_PERIODIC_TVM_TIMER_SCALE))>=traffic_param_ptr->report_interval)
      {
        /* Intialize the timer count to 0 */
        traffic_param_ptr->periodic_tmr_cnt = 0;
    
        /*------------------------------------------------------------------------------
               If there is no limit on the number of reports that we need to send to rrc or if
               there is a limit& we have not exceeded it, then it is time to send a report to
               RRC. nreports_sent is incremented just at the time of sending a report. Its
               done after checking if there is atleast one radio bearer for which the results
               have to be reported.
        ------------------------------------------------------------------------------*/
        if ((traffic_param_ptr->report_infinity) ||
            (traffic_param_ptr->nreports_sent < traffic_param_ptr->num_reports))
        {
          traffic_param_ptr->current_idx = cur_index;
    
          /*---------------------------------------------------------------------------
                  Time to send a report to RRC for this measurement identity. Prepare to
                  send the report.
          ----------------------------------------------------------------------------*/
          prepare_n_send_traffic_meas_report(meas_id);
    
        }/* if report_infinity  */
        else
        {
          /* Delete this measurement identity since we have sent the maximum number of*/
          /* reports that needs to be sent*/
          (void)mac_ul_delete_traffic_volume_meas(meas_id);
          MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Sent %d reports. Deleting %d",
                                                     traffic_param_ptr->num_reports,
                                                     meas_id);
        }
      }/*if time to send report */
    }
  }
}/* proc_periodic_mode_criteria*/



/*===============================================================================
 FUNCTION NAME  mac_ul_handle_tvm_event_tmr

 PURPOSE    This function is called when MAC traffic volume timer signal
            expires every 10ms. This is used as a base timer and all the other
            timers like trigger_timer..etc are done using counters.
            This function checks for each event mode measurement identity , if the
            timer counters have expired and posts the measurement report to RRC.
================================================================================*/


void mac_ul_handle_tvm_event_tmr (void)
{
  uint16                            meas_id;
  maci_ul_traffic_cfg_index_type    *traffic_cfg_idx_ptr;
  mac_ul_traffic_report_mode_e_type report_mode;

  /*---------------------------------------------------------------------------------
  If the current uplink transport channel type is invalid then its an error since
  there are no transport channels configured in MAC so far. Hence can't make any
  measurements.
  ---------------------------------------------------------------------------------*/
  if (GET_UL_CCTRCH_TYPE==INVALID_UL_CCTRCH
#ifdef FEATURE_WCDMA_HS_RACH
       && mac_hs_rach_info.hs_rach_active == FALSE
#endif
     )
  {
    /* Set the 10ms timer tick again */
    (void)rex_set_timer(&mac_ul_tvm_event_tmr_blk,(uint32)MAC_UL_TRAFFIC_VOLUME_TMR_BASE);
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW, "MAC UL transport channels not configured yet");
  }
  else if (num_event_measurements==0)
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW, "No measurements to make");
  }
  else
  {
    /*------------------------------------------------------------------------------
    If the interrupt timer is running check to see if it has expired. We can resume
    transmission once the interrupt timer has expired.
    -------------------------------------------------------------------------------*/
    if (GET_UL_CCTRCH_TYPE == RACH)
    {
      /*--------------------------------------------------------------------------------
      If the transmission is blocked then check to see if the Tx interrupt timer has
      expired. Resume transmissions when the timer has expired.
      ---------------------------------------------------------------------------------*/
      if (block_transmission)
      {
        // Increment the timer
        if ((tx_interrupt_tmr > 0 )&& ((++tx_interrupt_cnt*10) >= tx_interrupt_tmr))
        {
          block_transmission = FALSE;
          tx_interrupt_tmr = tx_interrupt_cnt = 0;
          MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "TxInterruptTmr expired");
        }
      }
    }

    /* Set the 10ms timer tick again */
    (void)rex_set_timer(&mac_ul_tvm_event_tmr_blk,(uint32)MAC_UL_TRAFFIC_VOLUME_TMR_BASE);
    if(tvm_debug == 1)
    {
      MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "TVM_DBG: Resetting TVM event timer");
    }

    /*-----------------------------------------------------------------------------
    Compute the raw payload and store the information in the array for each
    radio bearer.
    -----------------------------------------------------------------------------*/
    compute_obj_list_raw_payload();

    for (meas_id = MAC_UL_MIN_MEAS_ID; meas_id <= MAC_UL_MAX_MEAS_ID; meas_id++)
    {
      /*--------------------------------------------------------------------------
        Get the config, pointer corresponding to this measurement
        identity.
      --------------------------------------------------------------------------*/
      traffic_cfg_idx_ptr = GET_UL_TRAFFIC_CFG_PARAM_PTR(meas_id);

      /*--------------------------------------------------------------------------
      If it contains valid parameters and the pointer is not NULL then start
      measurements for this measurement identity.Check to see if we need to
      temporarily suspend measurements for this identity. suspend_meas=TRUE indicates
      that we need to save this configuration information and resume measurements
      only after the resume command is recvd for this identity.
      --------------------------------------------------------------------------*/
      if ((traffic_cfg_idx_ptr->valid)&&(!traffic_cfg_idx_ptr->suspend_meas)&&
          (traffic_cfg_idx_ptr->traffic_cfg_ptr!=NULL))
      {
        report_mode = traffic_cfg_idx_ptr->traffic_cfg_ptr->report_mode;

        /*-----------------------------------------------------------------------
          Based on the mode, check to see if it is time to send a report.
        -----------------------------------------------------------------------*/
        if (report_mode==EVENT_TRIGGERED_MODE)
        {
          proc_event_trigger_criteria(meas_id);
        }

      }/* if traffic_cfg_ptr is valid */
      else if((tvm_debug == 1)&&(meas_id == 4))
      {
        MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "TVM_DBG: Failed to check event trigger criteria - %d %d %d",
                                                   traffic_cfg_idx_ptr->valid,
                                                   traffic_cfg_idx_ptr->suspend_meas,
                   (traffic_cfg_idx_ptr->traffic_cfg_ptr!=NULL));
      }

    }/* for */
  } /* num_measurements>0) */
}


/*===============================================================================
 FUNCTION NAME  mac_ul_handle_tvm_periodic_tmr

 PURPOSE    This function is called when MAC traffic volume timer signal
            expires every (10ms* MAC_UL_PERIODIC_TVM_TIMER_SCALE). This is used as a 
            base timer and report_interval is computed using counters.
            This function checks for each periodic mode measurement identity if the 
            timer counters have expired posts the measurement report to RRC.
================================================================================*/
void mac_ul_handle_tvm_periodic_tmr (void)
{
  uint16                            meas_id;
  maci_ul_traffic_cfg_index_type    *traffic_cfg_idx_ptr;
  mac_ul_traffic_report_mode_e_type report_mode;

  /*---------------------------------------------------------------------------------
  If the current uplink transport channel type is invalid and hs rach is inactive (no EDCH),
  then its an error since there are no transport channels configured in MAC so far. 
  Hence can't make any measurements.
  ---------------------------------------------------------------------------------*/
  if (GET_UL_CCTRCH_TYPE==INVALID_UL_CCTRCH 
#ifdef FEATURE_WCDMA_HS_RACH
       && mac_hs_rach_info.hs_rach_active == FALSE
#endif
     )
  {
    /* Set the (10ms* MAC_UL_PERIODIC_TVM_TIMER_SCALE) timer tick again */
    (void)rex_set_timer(&mac_ul_tvm_periodic_tmr_blk,
                (uint32)(MAC_UL_TRAFFIC_VOLUME_TMR_BASE * MAC_UL_PERIODIC_TVM_TIMER_SCALE));
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "TVM_PERIODIC: MAC UL transport channels not configured yet");
  }
  else if (num_periodic_measurements == 0)
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "TVM_PERIODIC:No periodic measurements to make");
  }
  else
  {

    /* Set the (10ms* MAC_UL_PERIODIC_TVM_TIMER_SCALE) timer tick again */
    (void)rex_set_timer(&mac_ul_tvm_periodic_tmr_blk,
                (uint32)(MAC_UL_TRAFFIC_VOLUME_TMR_BASE * MAC_UL_PERIODIC_TVM_TIMER_SCALE));

    /*-----------------------------------------------------------------------------
        Compute the raw payload and store the information in the array for each RB
       -----------------------------------------------------------------------------*/
    compute_obj_list_raw_payload(); 
                        //    IS IT really needed ??
        

    for (meas_id = MAC_UL_MIN_MEAS_ID; meas_id <= MAC_UL_MAX_MEAS_ID; meas_id++)
    {
//        Get the config, pointer corresponding to this meas id
      traffic_cfg_idx_ptr = GET_UL_TRAFFIC_CFG_PARAM_PTR(meas_id);

//      If it contains valid parameters and if its not temporarily suspend meas id
      if ((traffic_cfg_idx_ptr->valid)&&(!traffic_cfg_idx_ptr->suspend_meas)&&
          (traffic_cfg_idx_ptr->traffic_cfg_ptr!=NULL))
      {
        report_mode = traffic_cfg_idx_ptr->traffic_cfg_ptr->report_mode;
        if (report_mode==PERIODIC_MODE)
        {
          proc_periodic_mode_criteria(meas_id);
        }
      }/* if traffic_cfg_ptr is valid */
    }/* for */
  } /* num_measurements>0) */
}



/*======================================================================================
 FUNCTION NAME  mac_ul_delete_traffic_volume_meas

 PURPOSE    This function deletes the traffic volume measurement associated with the
            measurement identity specified.
            This function returns a boolean. true indicates that the delete operation
            was sucessful.
======================================================================================*/

boolean mac_ul_delete_traffic_volume_meas
(
/* Delete the traffic volume measurements associated with this measurement identity */
uint16 meas_id
)
{
  boolean delete_status_ind=TRUE;
  uint16  i;
  maci_ul_traffic_cfg_index_type  *traffic_cfg_idx_ptr;

  /*----------------------------------------------------------------*/
  /* Measurement identity not valid                                 */
  /*----------------------------------------------------------------*/
  if (TRAFFIC_MEAS_ID_IS_INVALID(meas_id))
  {
    delete_status_ind=FALSE;
    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Invalid measurement ID %d",
                                                meas_id);
  }
  else
  {
    traffic_cfg_idx_ptr = GET_UL_TRAFFIC_CFG_PARAM_PTR(meas_id);
    /*-----------------------------------------------------------------
      If the valid flag is set to FALSE, then its an error since we
      are asked to delete a measurement ID hic doesnt exist.
      If the traffic_cfg_ptr is NULL, then its an error since we
      are asked to delete a measurement ID hic doesnt exist.
    ------------------------------------------------------------------*/
    if ((!traffic_cfg_idx_ptr->valid)||
        (traffic_cfg_idx_ptr->traffic_cfg_ptr==NULL))
    {
      delete_status_ind=FALSE;

      if (traffic_cfg_idx_ptr->traffic_cfg_ptr!=NULL)
      {
        /* Release memory */
#ifdef FEATURE_WCDMA_USE_MODEM_HEAP
        modem_mem_free(traffic_cfg_idx_ptr->traffic_cfg_ptr, MODEM_MEM_CLIENT_WCDMA_RLC_CRIT);
#else
        mem_free(&(tmc_heap), traffic_cfg_idx_ptr->traffic_cfg_ptr);
#endif
      }

      traffic_cfg_idx_ptr->traffic_cfg_ptr=NULL;
      MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Measurment ID doesn't exist %d %d",
                                                  !traffic_cfg_idx_ptr->valid,
                                                  traffic_cfg_idx_ptr->traffic_cfg_ptr);
    }
    else
    {

      /*-----------------------------------------------------------------------------------
      Update the number of pending measurements.
      -----------------------------------------------------------------------------------*/
      if (traffic_cfg_idx_ptr->traffic_cfg_ptr->report_mode == EVENT_TRIGGERED_MODE) 
      {
        num_event_measurements--;
        
        if (num_event_measurements == 0)
        {
          /*--------------------------------------------------------------------------------
                   Stop the timer as there are no traffic volume measurements to make.
                   ---------------------------------------------------------------------------------*/
          (void)rex_clr_timer(&mac_ul_tvm_event_tmr_blk);
          mac_tvm_event_timer_started = FALSE;
          /* Reset all the block_transmission flag */
          block_transmission = FALSE;
        }
      }
      else if (traffic_cfg_idx_ptr->traffic_cfg_ptr->report_mode == PERIODIC_MODE) 
      {
        num_periodic_measurements--;
        
        if (num_periodic_measurements == 0)
        {
          /*--------------------------------------------------------------------------------
                      Stop the timer as there are no traffic volume measurements to make.
                    ---------------------------------------------------------------------------------*/
         (void)rex_clr_timer(&mac_ul_tvm_periodic_tmr_blk);
         mac_tvm_periodic_timer_started = FALSE;
        }
      }
      
      MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Deleted meas_id %d, num_event_meas %d, num_periodic %d",
                                                 meas_id,
                                                 num_event_measurements,
                                                 num_periodic_measurements);

      /*------------------------------------------------------------
        Release the memory.
      -------------------------------------------------------------*/
#ifdef FEATURE_WCDMA_USE_MODEM_HEAP
      modem_mem_free(traffic_cfg_idx_ptr->traffic_cfg_ptr, MODEM_MEM_CLIENT_WCDMA_RLC_CRIT);
#else
      mem_free(&(tmc_heap), traffic_cfg_idx_ptr->traffic_cfg_ptr);
#endif
      traffic_cfg_idx_ptr->valid=FALSE;
      traffic_cfg_idx_ptr->traffic_cfg_ptr = NULL;



      /*---------------------------------------------------------------------------------
      Initalize the traffic_ul_buf_status structure. Every 10ms the RLC buffer status is
      obtained in traffic_ul_buf_status.
      ---------------------------------------------------------------------------------*/
      memset(&traffic_ul_buf_status,0, sizeof(rlc_ul_buf_status_type));

      /* evt_first_meas is set to true whenever MAC is configured for measurements
      newly */
      //evt_first_meas = TRUE;
      cur_index=0;
      rolled_over=FALSE;

      for (i=0;i<MAX_RB;i++)
      {
        memset(rb_raw_payload[i],0,TRAFFIC_MAX_SAMPLES*sizeof(uint32));
      }

    }/* measurement identity is valid */
  }
  return delete_status_ind;
}

/*========================================================================================
 FUNCTION NAME  mac_ul_delete_all_traffic_volume_meas

 PURPOSE    This function deletes all the traffic volume measurements that have been done
            so far. This function returns a boolean. True indicates that the delete_all
            function was sucessful.
========================================================================================*/

boolean mac_ul_delete_all_traffic_volume_meas (void)
{
  uint16 i;
  boolean delete_status_ind=TRUE ;

  /* Reset this flag since all measurements are cleared up */
  block_transmission = FALSE;

  if ((num_event_measurements == 0) && (num_periodic_measurements == 0))
  {
    delete_status_ind = FALSE;
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_MED, "There are no measurements to delete");
  }
  else
  {
    /*-----------------------------------------------------------------------------
     Stop the timer as there are no traffic volume measurements to make.
    -----------------------------------------------------------------------------*/
    (void)rex_clr_timer(&mac_ul_tvm_event_tmr_blk);
    mac_tvm_event_timer_started = FALSE;

    (void)rex_clr_timer(&mac_ul_tvm_periodic_tmr_blk);
    mac_tvm_periodic_timer_started= FALSE;

    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Cleared Traffic Volume Timers");

    rolled_over=FALSE;

    /* evt_first_meas is set to true whenever MAC is configured for measurements
    newly */
    //evt_first_meas = TRUE;
  }
  /*-----------------------------------------------------------------------------
    Initialise the UL traffic measurement table.
  -----------------------------------------------------------------------------*/
  memset(&traffic_ul_buf_status,0, sizeof(rlc_ul_buf_status_type));

  /*-----------------------------------------------------------------------------
    Initialise the UL traffic volume config table
  -----------------------------------------------------------------------------*/
  for (i=0;i<UE_NUM_TRAFFIC_MEAS;i++)
  {
    if (ul_traffic_info_table[i].traffic_cfg_ptr!=NULL)
    {
#ifdef FEATURE_WCDMA_USE_MODEM_HEAP
      modem_mem_free(ul_traffic_info_table[i].traffic_cfg_ptr, MODEM_MEM_CLIENT_WCDMA_RLC_CRIT);
#else
      mem_free((&tmc_heap),ul_traffic_info_table[i].traffic_cfg_ptr);
#endif
      ul_traffic_info_table[i].traffic_cfg_ptr=NULL;
    }
    ul_traffic_info_table[i].valid=FALSE;
  }

  for (i=0;i<MAX_RB;i++)
  {
    memset(rb_raw_payload[i],0,TRAFFIC_MAX_SAMPLES*sizeof(uint32));
  }

  cur_index=0;
  tx_interrupt_tmr=0;
  tx_interrupt_cnt=0;
  /*-----------------------------------------------------------------------------
  Update the number of traffic volume measurements,number of pending measurements
  -----------------------------------------------------------------------------*/
  num_event_measurements = 0;
  num_periodic_measurements = 0;

  return delete_status_ind;

}
/*========================================================================================
 FUNCTION NAME  check_modify_params

 PURPOSE  This function checks the config information sent by RRC in the case of
          MODIFY cmd and sees if the parameters are valid. Returns TRUE if all the
          parameters which are modified are valid.
========================================================================================*/
boolean check_modify_params
(
/* Pointer to config information sent by RRC */
mac_ul_traffic_meas_config_type *oc_traffic_cfg_ptr,

/* POinter to config information internally maintained by MAC */
maci_ul_traffic_cfg_param       *int_tmp_cfg_ptr
)
{
  boolean status_ind = TRUE;
  mac_ul_traffic_report_mode_e_type report_mode;

  report_mode = oc_traffic_cfg_ptr->meas_report_mode.report_mode;

  if (oc_traffic_cfg_ptr->meas_report_incl)
  {
    /*---------------------------------------------------------------------
    If MAC is asked to report either the average or variance of rlc paylaod.
    ---------------------------------------------------------------------*/
    if ((oc_traffic_cfg_ptr->meas_report_qty.avg_payload) ||
        (oc_traffic_cfg_ptr->meas_report_qty.var_payload))
    {
      /*------------------------------------------------------------------
      if the measurement quantity is included and if the interval is 0
      then its an error.
      ------------------------------------------------------------------*/
      if ((oc_traffic_cfg_ptr->meas_qty_incl)&&
          (oc_traffic_cfg_ptr->meas_qty.meas_interval/10==0))
      {
        status_ind=FALSE;
        MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Invalid measurement interval %d",
                                                    oc_traffic_cfg_ptr->meas_qty.meas_interval);
      }
      /*------------------------------------------------------------------
      if the measurement quantity is not included and if the previous
      config, the measurement interval is zero then its an error.
      ------------------------------------------------------------------*/
      else if ((!oc_traffic_cfg_ptr->meas_qty_incl)
               &&(int_tmp_cfg_ptr->nsamples==0))
      {
        status_ind=FALSE;
        MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Invalid measurement interval %d",
                                                    oc_traffic_cfg_ptr->meas_qty.meas_interval);
      }

    }

  }
  else if (int_tmp_cfg_ptr->avg_payload || int_tmp_cfg_ptr->var_payload)
  {
    if ((oc_traffic_cfg_ptr->meas_qty_incl) &&
        (oc_traffic_cfg_ptr->meas_qty.meas_interval/10==0))
    {
      status_ind=FALSE;
      MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Invalid measurement interval");

    }
  }

  if (status_ind && oc_traffic_cfg_ptr->meas_criteria_incl)
  {
    int_tmp_cfg_ptr->report_mode = report_mode;

    if (report_mode==EVENT_TRIGGERED_MODE)
    {
      status_ind= check_n_copy_modify_event_trigger_params(int_tmp_cfg_ptr,
                  &oc_traffic_cfg_ptr->meas_report_criteria.event_cfg);
    }
    else if (report_mode == PERIODIC_MODE)
    {
      status_ind=
      check_n_copy_periodic_params(int_tmp_cfg_ptr,oc_traffic_cfg_ptr);
    }
    else
    { /* No Reporting */
      int_tmp_cfg_ptr->report_mode = report_mode;
    }
  }
  return status_ind;
}

/*========================================================================================
 FUNCTION NAME  mac_ul_modify_traffic_volume_meas

 PURPOSE    This function modifies the traffic volume measurement for this measurement
            identity.This function returns a boolean. True indicates that the modify
            operation was sucessful. The modify_status_ind is set to true after checking
            if the transport channels for which measurements have been requested for
            exists.
========================================================================================*/
boolean mac_ul_modify_traffic_volume_meas
(
/* Pointer to traffic volume config information */
mac_ul_traffic_meas_config_type *oc_traffic_cfg_ptr
)
{
  uint16                                    meas_id;
  uint8                                     i;
  maci_ul_traffic_cfg_param                 *int_tmp_cfg_ptr = NULL;
  maci_ul_traffic_cfg_index_type            *traffic_cfg_idx_ptr = NULL;
  boolean                                   modify_status_ind=TRUE;
  boolean                                   periodic_to_event_mode_change = FALSE;
  boolean                                   event_to_periodic_mode_change = FALSE;
  

  meas_id = oc_traffic_cfg_ptr->meas_id;
  /*-----------------------------------------------------------------------------------
  If the measurement identity is invalid.
  ------------------------------------------------------------------------------------*/
  if (TRAFFIC_MEAS_ID_IS_INVALID(meas_id))
  {
    modify_status_ind=FALSE;
    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Measurement ID out of range %d",
                                                meas_id);
  }
  else if ((GET_UL_CCTRCH_TYPE==INVALID_UL_CCTRCH)
#ifdef FEATURE_WCDMA_HS_RACH
            && (mac_hs_rach_info.hs_rach_active == FALSE)
#endif
          )
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC UL transport channels not configured");
    modify_status_ind=FALSE;
  }
  else
  {
    /*--------------------------------------------------------------------------------
    Get the internal config pointer for this measurement identity.
    ---------------------------------------------------------------------------------*/
    traffic_cfg_idx_ptr = GET_UL_TRAFFIC_CFG_PARAM_PTR(meas_id);
    int_tmp_cfg_ptr  =    traffic_cfg_idx_ptr->traffic_cfg_ptr;
  }

  if(int_tmp_cfg_ptr != NULL)
  {
    if ((int_tmp_cfg_ptr->report_mode == PERIODIC_MODE) && 
        (oc_traffic_cfg_ptr->meas_report_mode.report_mode == EVENT_TRIGGERED_MODE))
    {
      periodic_to_event_mode_change = TRUE;
      MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "TVM_MODIFY : periodic_to_event_mode_change %d",
                                                 periodic_to_event_mode_change);
    }
    else if((int_tmp_cfg_ptr->report_mode == EVENT_TRIGGERED_MODE) &&
            (oc_traffic_cfg_ptr->meas_report_mode.report_mode == PERIODIC_MODE))
    {
      event_to_periodic_mode_change = TRUE;
      MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "TVM_MODIFY : event_to_periodic_mode_change %d",
                                                 event_to_periodic_mode_change);
    }
  }

  /* if modify_status_ind=TRUE*/
  if (modify_status_ind)
  {
    /* if the config pointer is NULL or invalid */
    if (((traffic_cfg_idx_ptr != NULL) && (!traffic_cfg_idx_ptr->valid)) ||
        (int_tmp_cfg_ptr==NULL)) /*lint !e644*/
    {
      modify_status_ind=FALSE;

      if (int_tmp_cfg_ptr!=NULL)
      {
#ifdef FEATURE_WCDMA_USE_MODEM_HEAP
        modem_mem_free(int_tmp_cfg_ptr, MODEM_MEM_CLIENT_WCDMA_RLC_CRIT);
#else
        mem_free(&(tmc_heap), int_tmp_cfg_ptr);
#endif
      }

      MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Measurement ID does not exist");
    }
    /* Check for the validity of the modify parameters that RRC has specified */
    else if (check_modify_params(oc_traffic_cfg_ptr,int_tmp_cfg_ptr))
    {
      /*----------------------------------------------------------------------------------
       Clear any exisiting measurement results done for this measurements identity.
      ----------------------------------------------------------------------------------*/
      memset (&int_tmp_cfg_ptr->event_result,0,sizeof(mac_ul_traffic_event_result));


      /*---------------------------------------------------------------------------------
             If There is any mode change take care of Timers and measurement count 
        ----------------------------------------------------------------------------------*/
      if (periodic_to_event_mode_change)
      {
        num_periodic_measurements--;
        num_event_measurements++;
      }
      else if (event_to_periodic_mode_change)
      {
        num_event_measurements--;
        num_periodic_measurements++;
      }
      
      if (num_event_measurements == 0)
      {
        (void)rex_clr_timer(&mac_ul_tvm_event_tmr_blk);
         mac_tvm_event_timer_started = FALSE;
         MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "TVM_MODIFY : Num_event_meas = %d",
                                                    num_event_measurements);
      }
      else if ((num_event_measurements > 0) && (!mac_tvm_event_timer_started))
      {
        (void)rex_set_timer(&mac_ul_tvm_event_tmr_blk,(uint32)MAC_UL_TRAFFIC_VOLUME_TMR_BASE);
        mac_tvm_event_timer_started = TRUE;
        MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "TVM_MODIFY : Starting mac_ul_tvm_event_tmr_blk timer %d ms",
                                                   MAC_UL_TRAFFIC_VOLUME_TMR_BASE);
      }

      if (num_periodic_measurements == 0)
      {
        (void)rex_clr_timer(&mac_ul_tvm_periodic_tmr_blk);
        mac_tvm_periodic_timer_started = FALSE;
        MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "TVM_MODIFY : num_periodic_measurements = %d",
                                                   num_periodic_measurements);
      }
      else if ((num_periodic_measurements > 0) && (!mac_tvm_periodic_timer_started))
      {
        (void)rex_set_timer(&mac_ul_tvm_periodic_tmr_blk,
               (uint32)(MAC_UL_TRAFFIC_VOLUME_TMR_BASE * MAC_UL_PERIODIC_TVM_TIMER_SCALE));
        mac_tvm_periodic_timer_started = TRUE;
        MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "TVM_MODIFY : Starting mac_ul_tvm_periodic_tmr_blk timer %d ms",
                                                   (MAC_UL_TRAFFIC_VOLUME_TMR_BASE*MAC_UL_PERIODIC_TVM_TIMER_SCALE));
      }
      
      MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "TVM_MODIFY : num_event_meas %d, num_periodic_meas %d",
                                                 num_event_measurements,
                                                 num_periodic_measurements);

      /*---------------------------------------------------------------------------------
        If measurement object list is included then copy the object list.
      ----------------------------------------------------------------------------------*/
      if (oc_traffic_cfg_ptr->meas_obj_incl)
      {
        int_tmp_cfg_ptr->meas_obj_list_incl = oc_traffic_cfg_ptr->meas_obj_incl;
        int_tmp_cfg_ptr->num_trch = oc_traffic_cfg_ptr->meas_obj_list.num_trch;
        WCDMA_MEMCPY(int_tmp_cfg_ptr->trch_info, 
                     UE_MAX_TRCH * sizeof(mac_ul_traffic_trch_info_type),
                     oc_traffic_cfg_ptr->meas_obj_list.trch_info,
                     UE_MAX_TRCH * sizeof(mac_ul_traffic_trch_info_type));

        for (i=0;i<int_tmp_cfg_ptr->num_trch;i++)
        {
          if ((int_tmp_cfg_ptr->trch_info[i].trch_type==RACH) &&
              (GET_UL_CCTRCH_TYPE==RACH))
          {
            int_tmp_cfg_ptr->trch_info[i].trch_id = int_ul_trch_info[0].trch_id;
          }
        }
      }
      /*---------------------------------------------------------------------------------
        Copy the new report quantity .
      ----------------------------------------------------------------------------------*/
      if (oc_traffic_cfg_ptr->meas_report_incl)
      {
        int_tmp_cfg_ptr->avg_payload = oc_traffic_cfg_ptr->meas_report_qty.avg_payload;
        int_tmp_cfg_ptr->rlc_payload = oc_traffic_cfg_ptr->meas_report_qty.rlc_payload;
        int_tmp_cfg_ptr->var_payload = oc_traffic_cfg_ptr->meas_report_qty.var_payload;
      }

      /*---------------------------------------------------------------------------------
      If the measurement quantity is included copy the measurement interval. This is the
      interval used to calculate average or variance when the measurement report quantity
      is specified as average or vaiance.
      ---------------------------------------------------------------------------------*/
      if (oc_traffic_cfg_ptr->meas_qty_incl)
      {
        int_tmp_cfg_ptr->nsamples = (uint8)oc_traffic_cfg_ptr->meas_qty.meas_interval/10;
      }

    }/* All parameters are valid */

  } /* modify_status_ind=TRUE */

  return modify_status_ind;
}
/*=====================================================================================
 FUNCTION NAME  mac_ul_block_rach_tx

 PURPOSE    This function blocks DTCH transmissions on RACH when the UE is in CELL_FACH
            state. It resumes transmissions when the IE Tx interruption after trigger
            elapses.
=====================================================================================*/
void mac_ul_block_rach_tx (boolean block_rach)
{
  /*-----------------------------------------------------------------------------------
  If the cctrch type is invalid then its an error.
  ------------------------------------------------------------------------------------*/
  if (GET_UL_CCTRCH_TYPE != RACH)
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Invalid BLOCK command recvd");
    block_transmission = FALSE;
    tx_interrupt_tmr = tx_interrupt_cnt = 0;
  }
  else
  { /* CCTRCH_TYPE = RACH */
    /* block transmission */
    block_transmission = block_rach;
    //tx_interrupt_cnt = 0; // Reset the timer count for tx interruption after trigger.

    /* The timer would be started when we get the next 10ms timer expiry signal*/
  }/* cctrch_type = RACH*/
}
/*====================================================================================
 FUNCTION NAME  proc_block_dtch_tx_on_rach()

 PURPOSE  If the IE is in cell_fach state then it can prohibit dtch transmissions on
        RACH. block_transmission is set to TRUE when we receive a BLOCK cmd from RRC
        The transmission is resumed when the Tx interruption timer expires
=====================================================================================*/

void proc_block_dtch_tx_on_rach(rlc_ul_buf_status_type *ul_buf_status_ptr)
{

  uint16                      i;
  ul_lc_info_type             *int_lc_ptr;        /* Pointer to Logical channel info */
  rlc_ul_lc_buf_status_type   *lc_buf_status_ptr; /* Pointer to buffer status info */

  /* Eliminate TFCs based on logical channel buffer occupancy */
  for (i=0 ; i < UE_MAX_UL_LOGICAL_CHANNEL; i++)
  {
    int_lc_ptr = GET_UL_LC_INFO_PTR((rlc_lc_id_type)(i + MAC_UL_MIN_RLC_ID));

    /* Get Logical Channel Buffer Status information */
    lc_buf_status_ptr = &ul_buf_status_ptr->chan_info[i];

    /* Logical channel is enabled */
    if (int_lc_ptr->enable == TRUE)
    {
      /* Compute Transport Channel Buffer Occupancy based on Buffer Status Type */
      switch (lc_buf_status_ptr->type)
      {

      /* Flexible PDUs type */
      case FLEX_PDUS:
        if (int_lc_ptr->lc_type==UE_LOGCHAN_DTCH)
        {
          /* Set the nbytes to 0 */
          lc_buf_status_ptr->nbytes = 0;
        }
        break;

      default:
        break;
      }
    }
  }
}

/*====================================================================================
 FUNCTION NAME  mac_ul_save_traffic_volume_meas()

 PURPOSE  Svae the configuration information for this measurement identity and
 temporarily suspend measurements until a resume command is received for this
 meas_id.
=====================================================================================*/
boolean mac_ul_save_traffic_volume_meas(uint16 meas_id)
{
  maci_ul_traffic_cfg_index_type            *traffic_cfg_idx_ptr;
  boolean                                                                     rv=FALSE;
  uint8                                                                         i;
  /*--------------------------------------------------------------------------
 Get the config, pointer corresponding to this measurement
 identity.
  --------------------------------------------------------------------------*/

  if (TRAFFIC_MEAS_ID_IS_INVALID(meas_id))
  {
    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Invalid measurement ID %d",
                                                meas_id);
  }
  else
  {
    traffic_cfg_idx_ptr = GET_UL_TRAFFIC_CFG_PARAM_PTR(meas_id);
  
    if (traffic_cfg_idx_ptr->valid)
    {
      traffic_cfg_idx_ptr->suspend_meas=TRUE;
      traffic_cfg_idx_ptr->traffic_cfg_ptr->first_meas = TRUE;
  
      /* Reset the counter */
      mac_reset_periodic_event_trigger_counters();
  
      tx_interrupt_tmr = 0;
      tx_interrupt_cnt = 0;
      //evt_first_meas = TRUE;
  
      /*---------------------------------------------------------------------------------
      Initalize the radio bearer raw payload information.
      ---------------------------------------------------------------------------------*/
      for (i=0;i<MAX_RB;i++)
      {
        /*------------------------------------------------------------------------------
        rb_raw_payload is an array of 26 elements(maximum measurement interval is 260ms)
        and is maintained for every radio bearer (1..32).
        ------------------------------------------------------------------------------*/
        memset(rb_raw_payload[i],0,TRAFFIC_MAX_SAMPLES*sizeof(uint32));
      }
  
      rv=TRUE;
    }
    else
    {
      MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Save: Meas_Id %d not present",
                                                 meas_id);
    }
  }
  
  return rv;
}

/*====================================================================================
 FUNCTION NAME  mac_ul_resume_traffic_volume_meas()

 PURPOSE  Resume measurementsfor this  identity and
=====================================================================================*/

boolean mac_ul_resume_traffic_volume_meas(uint16 meas_id)
{
  maci_ul_traffic_cfg_index_type            *traffic_cfg_idx_ptr;
  boolean                                   status_ind=TRUE;
  maci_ul_traffic_cfg_param                 *traffic_cfg_ptr;


  /*--------------------------------------------------------------------------
 Get the config, pointer corresponding to this measurement
 identity.
  --------------------------------------------------------------------------*/
  if (TRAFFIC_MEAS_ID_IS_INVALID(meas_id))
  {
    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Invalid measurement ID %d",
                                                meas_id);
  }
  else
  {
    traffic_cfg_idx_ptr = GET_UL_TRAFFIC_CFG_PARAM_PTR(meas_id);
  
    if (traffic_cfg_idx_ptr->valid)
    {
      traffic_cfg_idx_ptr->suspend_meas=FALSE;
  
      traffic_cfg_ptr = traffic_cfg_idx_ptr->traffic_cfg_ptr;
  
  
      if (traffic_cfg_ptr->meas_obj_list_incl)
      {
      }
      else
      {
        /*----------------------------------------------------------------------------
        Populate the transport channel id and the type information from all the
        uplink transport channels currently configured in MAC.
        ----------------------------------------------------------------------------*/
        traffic_cfg_ptr->num_trch= populate_object_list(traffic_cfg_ptr->trch_info);
      }
      
      if (traffic_cfg_ptr->report_mode == EVENT_TRIGGERED_MODE)
      {
        if (!mac_tvm_event_timer_started)
        {
          // Start the traffic volume timer ticking again.
          (void)rex_set_timer(&mac_ul_tvm_event_tmr_blk,(uint32)MAC_UL_TRAFFIC_VOLUME_TMR_BASE);
          MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Starting event triggering TVM timer meas_id = %d, num_meas = %d",
                                                     meas_id,
                                                     num_event_measurements);
        }
        else
        {
          MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "TVM event triggering timer already running, meas_id = %d, num_meas = %d",
                                                     meas_id,
                                                     num_event_measurements);
        }
      }
      else if(traffic_cfg_ptr->report_mode == PERIODIC_MODE)
      {
        if (!mac_tvm_periodic_timer_started)
        {
          // Start the traffic volume timer ticking again.
            (void)rex_set_timer(&mac_ul_tvm_periodic_tmr_blk,(uint32)(MAC_UL_TRAFFIC_VOLUME_TMR_BASE * MAC_UL_PERIODIC_TVM_TIMER_SCALE));
          MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Starting TVM periodic timer meas_id = %d, num_meas = %d",
                                                     meas_id,
                                                     num_periodic_measurements);
        }
        else
        {
          MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "TVM periodic timer already running, meas_id = %d, num_meas = %d",
                                                     meas_id,
                                                     num_periodic_measurements);
        }
      }
    }// valid
    else
    {
      MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Resume: Meas_Id %d not present",
                                                 meas_id);
    }
  }
  
  return status_ind;

}


/*====================================================================================
 FUNCTION NAME  config_ul_mac_traffic_volume

 PURPOSE    This function process UL MAC traffic volume Command. It sends a config
            confirmation message to RRC in the case that RRC has requested for it.
            Then it processes the commands based on the command type : setup, modify,
            delete, delete_all.
=====================================================================================*/

void config_ul_mac_traffic_volume
(
/* Pointer to traffic volume config information */
mac_ul_traffic_meas_config_type *oc_traffic_cfg_ptr
)
{

  boolean config_sucess = FALSE;

  /* If oc_traffic_cfg_ptr is NULL print an error message */
  if (oc_traffic_cfg_ptr== NULL)
  {
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Null config pointer");
  }
  else /* oc_traffic_cfg_ptr is not NULL */
  {

    MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Recvd %d cmd for meas_id %d",
                                               oc_traffic_cfg_ptr->meas_cmd,
                                               oc_traffic_cfg_ptr->meas_id);
    
    switch (oc_traffic_cfg_ptr->meas_cmd)
    {
    /*------------------------------------------------------------------------------
       Start traffic volume measurement for this measurement id
    ------------------------------------------------------------------------------*/
    case MAC_TRAFFIC_MEAS_START:
      config_sucess = mac_ul_setup_traffic_volume_meas(oc_traffic_cfg_ptr);
      break;

      /*------------------------------------------------------------------------------
           Modify traffic volume measurement for this measurement id
      ------------------------------------------------------------------------------*/
    case MAC_TRAFFIC_MEAS_MODIFY:
      config_sucess = mac_ul_modify_traffic_volume_meas(oc_traffic_cfg_ptr);
      break;

      /*------------------------------------------------------------------------------
         Delete traffic volume measurement for this measurement id
      ------------------------------------------------------------------------------*/
    case MAC_TRAFFIC_MEAS_DELETE:
      config_sucess = mac_ul_delete_traffic_volume_meas(oc_traffic_cfg_ptr->meas_id);
      break;

      /*------------------------------------------------------------------------------
         Delete all the traffic volume measurements.
      ------------------------------------------------------------------------------*/
    case MAC_TRAFFIC_MEAS_DELETE_ALL:
      config_sucess = mac_ul_delete_all_traffic_volume_meas();
      break;

    case MAC_TRAFFIC_MEAS_RESUME:
      config_sucess = mac_ul_resume_traffic_volume_meas(oc_traffic_cfg_ptr->meas_id);
      break;

    case MAC_TRAFFIC_MEAS_SAVE:
      config_sucess = mac_ul_save_traffic_volume_meas(oc_traffic_cfg_ptr->meas_id);
      break;

    default:
      MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "Unknown command");
      break;
    }
    /*------------------------------------------------------------------------------
      If this is set to true then send a message to RRC confirming the receipt of
      the config information
    ------------------------------------------------------------------------------*/
    if (oc_traffic_cfg_ptr->config_conf_ind)
    {
      /*---------------------------------------------------------------------------
       Posts the message in the uplink RRC task queue.
      ---------------------------------------------------------------------------*/
      maci_post_cfg_cnf_msg(oc_traffic_cfg_ptr->config_ack_num,
                            oc_traffic_cfg_ptr->meas_id, config_sucess);
    }

    /*free memory : cfg info got from RRC */
#ifdef FEATURE_WCDMA_USE_MODEM_HEAP
    modem_mem_free(oc_traffic_cfg_ptr, MODEM_MEM_CLIENT_WCDMA_RLC_CRIT);
#else
    mem_free(&(tmc_heap),oc_traffic_cfg_ptr);
#endif
  }
}

/*==================================================================================
 FUNCTION NAME  mac_update_object_list

 PURPOSE    This function finds all the uplink currently active transport channels
            and populates the transport channel information array. It returns the number
            of transport channels.
==================================================================================*/
uint16 mac_update_object_list
(
  uint16                            ntrch,
  /* Array of transport channel type and id information*/
  mac_ul_traffic_trch_info_type     trch_info[]

)
{
  uint16                  i,j;     /* loop index */
  tr_ch_id_type           trch_id; /*transport channel id */
  trch_id_to_idx_map_type *id_to_idx_ptr; /*Transport channel id to index pointer*/
  boolean                 found = FALSE;

  /* Go through all the possible transport channels id */
  for (i=0;i<MAX_TRCH;i++)
  {
    /* The transport channel id ranges from 1 to 32 */
    trch_id = i+MAC_UL_MIN_TRCH_ID;

    /* Get the index where transport channel information for this id is stored*/
    id_to_idx_ptr = GET_UL_TRCH_ID_TO_IDX_PTR(trch_id);

    /* if the corresponding index has valid parameters */
    if (id_to_idx_ptr->valid)
    {
      found = FALSE;

      for (j=0;j<ntrch;j++)
      {
        if ((trch_id == trch_info[j].trch_id) &&
            (GET_UL_CCTRCH_TYPE == trch_info[j].trch_type))
        {
          found = TRUE;
          break;
        }
      }

      if(found == FALSE)
      {
        /* Store the transport channel id and the type in the appropriate place */
        trch_info[ntrch].trch_id = trch_id;
        trch_info[ntrch].trch_type = GET_UL_CCTRCH_TYPE;
        ntrch++;
        MSG_3(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "Added trch_id = %d, trch_type = %d, ntrch = %d",
                                                   trch_id,
                                                   GET_UL_CCTRCH_TYPE,
                                                   ntrch);
      }

    } /*if transport channel is valid*/

  }/*for the number of transport channels */

  return ntrch;
}

/*==================================================================================
 FUNCTION NAME  populate_object_list

 PURPOSE    This function finds all the uplink currently active transport channels
            and populates the transport channel information array. It returns the number
            of transport channels.
==================================================================================*/

void mac_ul_update_trch_info(void)
{
  uint16                            meas_id;
  maci_ul_traffic_cfg_index_type    *traffic_cfg_idx_ptr;
  mac_ul_traffic_report_mode_e_type report_mode;
  maci_ul_traffic_cfg_param         *traffic_cfg_ptr;
  boolean                           trch_to_be_added;
  uint32                            i;

  for (meas_id = MAC_UL_MIN_MEAS_ID; meas_id <= MAC_UL_MAX_MEAS_ID; meas_id++)
  {
    /*--------------------------------------------------------------------------
      Get the config, pointer corresponding to this measurement
      identity.
    --------------------------------------------------------------------------*/
    traffic_cfg_idx_ptr = GET_UL_TRAFFIC_CFG_PARAM_PTR(meas_id);

    /*--------------------------------------------------------------------------
     If it contains valid parameters and the pointer is not NULL then start
     measurements for this measurement identity.Check to see if we need to
     temporarily suspend measurements for this identity. suspend_meas=TRUE indicates
     that we need to save this configuration information and resume measurements
     only after the resume command is recvd for this identity.
    --------------------------------------------------------------------------*/
    if ((traffic_cfg_idx_ptr->valid)&&(!traffic_cfg_idx_ptr->suspend_meas)&&
        (traffic_cfg_idx_ptr->traffic_cfg_ptr!=NULL))
    {
      traffic_cfg_ptr = traffic_cfg_idx_ptr->traffic_cfg_ptr;

      report_mode = traffic_cfg_ptr->report_mode;

      /*if (traffic_cfg_ptr->meas_obj_list_incl == FALSE)
      {
        traffic_cfg_ptr->num_trch =
        populate_object_list(traffic_cfg_ptr->trch_info);
      }*/

      /*-----------------------------------------------------------------------
        Based on the mode, check to see if it is time to send a report.
      -----------------------------------------------------------------------*/
      if (report_mode==EVENT_TRIGGERED_MODE)
      {
        if (traffic_cfg_ptr->event_trch_incl == FALSE)
        {
          trch_to_be_added = TRUE;
          /*-----------------------------------------------------------------------
            Update TVM TrCh Info only if new TrCh of same type is configured
          -----------------------------------------------------------------------*/
          for (i = 0; i < traffic_cfg_ptr->num_trch; i++)
          {
            if(GET_UL_CCTRCH_TYPE != traffic_cfg_ptr->trch_info[i].trch_type)
            {
              trch_to_be_added = FALSE;
              break;
            }
          }
 
          if(trch_to_be_added == TRUE)
          {
            traffic_cfg_ptr->num_trch =
              mac_update_object_list(
                traffic_cfg_ptr->num_trch,
                traffic_cfg_ptr->trch_info
              );
          }
        }
      }

    }/* if traffic_cfg_ptr is valid */

  }/* for */

}

/*==================================================================================
 FUNCTION NAME  MAC_UL_IS_MEAS_VALID

 PURPOSE    This function would return TRUE if meas_id is valid & is active per MAC TVM database.
==================================================================================*/
boolean mac_ul_is_meas_valid(uint16 meas_id) 
{
  boolean ret_val = TRUE;
  maci_ul_traffic_cfg_index_type *traffic_cfg_idx_ptr = NULL;
  
  if (TRAFFIC_MEAS_ID_IS_INVALID(meas_id))
  {
    MSG_1(MSG_SSID_WCDMA_MAC, MSG_LEGACY_ERROR, "MAC_ERR: Invalid measurement ID %d",
                                                meas_id);
    ret_val = FALSE;
  }
  else
  {
    traffic_cfg_idx_ptr = GET_UL_TRAFFIC_CFG_PARAM_PTR(meas_id);
    if((traffic_cfg_idx_ptr->valid == FALSE) || 
       (traffic_cfg_idx_ptr->traffic_cfg_ptr == NULL) || 
       (traffic_cfg_idx_ptr->suspend_meas == TRUE))
    {
      ret_val = FALSE;
    }
  }
  MSG_2(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH, "MAC_TVM: Meas ID %d validity %d (0-Invalid,1-Valid)",
                                             meas_id, ret_val);
  return ret_val;
}
