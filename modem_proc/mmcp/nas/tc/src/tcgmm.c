/*===========================================================================
                       T C  G M M  S O U R C E   F I L E 

DESCRIPTION

   This file contains the function definitions between the TC and GMM entities.
   
EXTERNALIZED FUNCTIONS   

  tc_process_gmm_cmd()  
    This function processes the command received from GMM entity.

  send_gmm_tcpdu()     
    This function sends the command to GMM task.

  tc_process_tc_pdu()   
    This function processes a received TC-PDU sent by GMM.

  tc_activate_rb_test_mode() 
    This function initiates RB_TEST_MODE activation

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2001- 2011 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/ctest/vcs/tcgmm.c_v   1.8   26 Jun 2002 18:49:02   madiraju  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/tc/src/tcgmm.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
 
03/28/12   us       Compilation warnings fixed 
08/03/11    am      Putting check for number of Uplink LC IDs
05/02/08    rk      Notify RRC (NORMAL Mode) on OPEN UE TEST LOOP
04/04/08    ab      Fixed klocwork defects
05/23/05    rk      suppressed lint errors
12/03/04   mks     Reset flag tc_test_loop_closed when SS entity deactivates 
                   the radio bearer test mode
10/20/04   mks     Notify RABM every time the UE goes to loopback mode.  
10/13/04   mks     Per 5.3.2.3 of TS34.109, send CLOSE_UE_TEST_LOOP_COMPLETE
                   if  the  test  loop was already closed prior to receiving 
                   CLOSE_UE_TEST_LOOP message from SS.Now, handling loopback
                   setup with multi-rab configuration.
08/31/04   mks     Added support for handling multiple radio bearers during
                   loopback call setup
11/27/02   mks     Added tc_handle_rrc_lb_rej() to handle test loopback reject
                   message from RRC and transition UE to normal mode
10/24/02   mks     Updated tc_close_ue_test_loop() to pass the test loop mode
                   as well as dummy signalling flag to CM 
06/26/02   mks     Added tc_test_loop_closed flag to maintain whether test loop
                   is closed on the UE end - this flag is set to TRUE when UE 
                   sends UE_TEST_LOOP_COMPLETE message and set to FALSE when UE
                   opens the test loop. Added is_tc_test_loop_closed (). 
06/19/02   mks     Changed the log level of dummy sig enabled message, fixed
                   the issue with setting the dummy signaling bit 
05/03/02   mks     Updated tc_close_ue_test_loop () to pass the dummy signaling 
                   flag to RRC
03/31/02   mks     Support for test loop mode 2 & dummy DCCH, notify RABM only
                   in PS mode 
02/24/02   mks     Notify RABM about mode change, route messages to MM/GMM,
                   set RAB id and UL RLC SDU size based on the values in
                   close_ue_test_loop message
02/03/02   mks     Updated to notify CM & RRC about test mode
09/24/01   ks      Created file.

===========================================================================*/

/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
// NPR
#include "mmcp_variation.h"
#include "customer.h"

#ifdef FEATURE_TC

#define FEATURE_DSM_WM_CB

#include "tcparse.h"
#include "ms.h"
#include "tctask.h"
#include "tctask_v.h"
#include "mmtask.h"
#include "mmtask_int.h"
#include "mm.h"
#include "memory.h"
#include "string.h"
#include "rrcdata.h"
#include "wcdmamvsif.h"
#include "tc_loopback.h"
#include "rlcrrcif.h"

#ifdef FEATURE_MBMS
#error code not present
#endif
#ifdef FEATURE_TDSCDMA
#include "tdsrrcdata.h"
#include "tdsl2external.h"
#endif /* FEATURE_TDSCDMA */
#include <stringl/stringl.h>
#ifdef FEATURE_SEGMENT_LOADING
#include "mcfg_seg_load.h"
#include "ITDSCDMA.h"
#include "IWCDMA.h"
#include "IWCDMA_ext.h"
#endif
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
LOCAL boolean ps_mode = FALSE;
LOCAL boolean tc_test_loop_closed = FALSE;

/* ========================================================================
** EXTERNAL FUNCTION DEFINITIONS.
** ====================================================================== */

/* -----------------------------------------------------------------------
** FUNCTION     : TC_CM_NOTIFY_TEST_SETUP_IN_PROGRESS()

** DESCRIPTION  : This function notifies CM about UE test mode setup
** ----------------------------------------------------------------------- */
extern void tc_cm_notify_test_setup_in_progress(void);

/* -----------------------------------------------------------------------
** FUNCTION     : TC_CM_NOTIFY_LB_MODE()

** DESCRIPTION  : This function notifies CM about UE mode change to TEST 
** ----------------------------------------------------------------------- */
extern void tc_cm_notify_lb_mode(byte, boolean);

/* -----------------------------------------------------------------------
** FUNCTION     : TC_CM_NOTIFY_NORMAL_MODE()

** DESCRIPTION  : This function notifies CM about UE mode change to NORMAL
** ----------------------------------------------------------------------- */
extern void tc_cm_notify_normal_mode(void);

/* -----------------------------------------------------------------------
** FUNCTION     : TC_RRC_NOTIFY_TEST_MODE()

** DESCRIPTION  : This function notifies RRC about UE mode change to TEST 
** ----------------------------------------------------------------------- */
extern boolean tc_rrc_notify_test_mode(void);

/* -----------------------------------------------------------------------
** FUNCTION     : TC_RRC_NOTIFY_NORMAL_MODE()

** DESCRIPTION  : This function notifies RRC about UE mode change to NORMAL
** ----------------------------------------------------------------------- */
extern boolean tc_rrc_notify_normal_mode(void);

/* -----------------------------------------------------------------------
** FUNCTION     : TC_RRC_NOTIFY_LB_MODE()

** DESCRIPTION  : This function notifies RRC about UE mode change to NORMAL
** ----------------------------------------------------------------------- */
extern boolean tc_rrc_notify_lb_mode(byte, boolean);

/* -----------------------------------------------------------------------
** FUNCTION     : TC_NOTIFY_RABM(ue_mode_type_T mode)

** DESCRIPTION  : This function notifies RABM about UE mode change to TEST 
** ----------------------------------------------------------------------- */
extern void tc_notify_rabm(ue_mode_type_T mode);

/* -----------------------------------------------------------------------
** FUNCTION     : LB_CLEANUP(void)

** DESCRIPTION  : This function performs cleanup for TC entity
** ----------------------------------------------------------------------- */
extern void lb_cleanup(void);

extern void lb_rab_cleanup (byte rab_id);
#ifdef FEATURE_WCDMA
#ifndef FEATURE_SEGMENT_LOADING
extern void rlc_ul_deregister_srvc_handler 
(
  rlc_ul_deregister_srvc_type *de_reg_ptr /* Ptr to WM registration info   */
);

extern void rlc_dl_deregister_srvc_handler
(
  rlc_dl_deregister_srvc_type *de_reg_ptr      /* Ptr to WM registration Info   */
);
#endif
#endif
extern void sm_perform_local_deactivation( byte cause );

#ifdef FEATURE_MBMS
#error code not present
#endif

// Length the TC reply message to SS
#define TC_REPLY_LENGTH 2

#ifdef FEATURE_MBMS
#error code not present
#endif
// Dummy DCCH enable flag
boolean dummy_dcch_enabled;

#ifdef FEATURE_MBMS
#error code not present
#endif
/* ========================================================================
** LOCAL FUNCTION PROTOTYPES.
** These functions are available only in the scope of this file.
** ====================================================================== */

/* -----------------------------------------------------------------------
** FUNCTION     : TC_PROCESS_GMM_ERROR_IND()

** DESCRIPTION  : This function processes Error Indications from MM/GMM.
** ----------------------------------------------------------------------- */
LOCAL void tc_process_mmgmm_error_ind(tc_cmd_type*);

/* -----------------------------------------------------------------------
** FUNCTION     : TC_PROCESS_TC_PDU()

** DESCRIPTION  : This function processes a received TC-PDU from GMM.
** ----------------------------------------------------------------------- */
LOCAL void tc_process_tc_pdu(byte* tc_data, byte tc_length);

/* -----------------------------------------------------------------------
** FUNCTION     : TC_PACK_LAYER3_MESG()

** DESCRIPTION  : This function packs the L3 TC message.
** ----------------------------------------------------------------------- */
LOCAL void tc_pack_layer3_mesg(byte* ext_buf, byte msg_type);

/* -----------------------------------------------------------------------
** FUNCTION     : TC_CLOSE_UE_TEST_LOOP()

** DESCRIPTION  : This function packs the L3 TC message.
** ----------------------------------------------------------------------- */
LOCAL void tc_close_ue_test_loop(byte* recd_msg);

/* -----------------------------------------------------------------------
** FUNCTION     : TC_OPEN_UE_TEST_LOOP()

** DESCRIPTION  : This function packs the L3 TC message.
** ----------------------------------------------------------------------- */
LOCAL void tc_open_ue_test_loop(void);

/* -----------------------------------------------------------------------
** FUNCTION     : TC_DEACTIVATE_RB_TEST_MODE()

** DESCRIPTION  : This function deactivates RB_TEST_MODE
** ----------------------------------------------------------------------- */
LOCAL void tc_deactivate_rb_test_mode(void);

#ifdef FEATURE_MBMS
#error code not present
#endif
/* -----------------------------------------------------------------------
** FUNCTION     : LB_SET_UL_SDU_SIZE_TO_ZERO

** DESCRIPTION  : This function sets the UpLink SDU sizes to zerp
** ----------------------------------------------------------------------- */
static void lb_set_ul_sdu_size_to_zero(void) ;

/* -----------------------------------------------------------------------
** FUNCTION     : IS_TC_TEST_LOOP_CLOSED()

** DESCRIPTION  : This function returns TRUE if UE test loop is closed
** ----------------------------------------------------------------------- */
boolean is_tc_test_loop_closed (void);

/* -----------------------------------------------------------------------
** FUNCTION     : RESET_TC_LOOP()

** DESCRIPTION  : This function resets the TC loop
** ----------------------------------------------------------------------- */
void reset_tc_loop (void);


/*===========================================================================
** FUNCTION DEFINITIONS 
** =========================================================================*/

/* -----------------------------------------------------------------------
** FUNCTION     : TC_ACTIVATE_RB_TEST_MODE()

** DESCRIPTION  : This function activates RB_TEST_MODE
** ----------------------------------------------------------------------- */
void tc_activate_rb_test_mode(void);

/* -----------------------------------------------------------------------
** FUNCTION     : TC_CLOSE_UE_TEST_LOOP_COMPLETE()

** DESCRIPTION  : This function packs the CLOSE_UE_TEST_LOOP_COMPLETE message 
** ----------------------------------------------------------------------- */
void tc_close_ue_test_loop_complete(void);

/*===========================================================================
FUNCTION TC_PROCESS_GMM_CMD 

DESCRIPTION
  This function processes a command received from the GMM. 
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tc_process_gmm_cmd(tc_cmd_type* cmd_ptr)
{

  switch (cmd_ptr->header.cmd_id)
  {
    
    case GMMTC_ERROR_IND :
	  tc_process_mmgmm_error_ind(cmd_ptr);
      break;

    case GMMTC_UNITDATA_IND:
	  ps_mode = TRUE;
      tc_process_tc_pdu(cmd_ptr->cmd.gmm_unitdata_ind.data, 
                        cmd_ptr->cmd.gmm_unitdata_ind.message_len);
      break;

    default:
      ERR (" UNKNOWN cmd from GMM : %d", cmd_ptr->header.cmd_id,0,0);
      break;
  };
}

/*===========================================================================
FUNCTION TC_PROCESS_MM_CMD 

DESCRIPTION
  This function processes a command received from the MM. 
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tc_process_mm_cmd(tc_cmd_type* cmd_ptr)
{

  // Process the Command sent by GMM task 
  MSG_LOW_DS_0(TC_SUB,"MM Sent a Command, process it");

  switch (cmd_ptr->header.cmd_id)
  {    
    case MMTC_ERROR_IND:
      MSG_MED_DS_1 (TC_SUB,"MM sent ERROR Indication :%d :",
                cmd_ptr->cmd.mm_error_ind.cause);
      tc_process_mmgmm_error_ind(cmd_ptr);
      break;

    case MMTC_UNITDATA_IND:
      MSG_MED_DS_0(TC_SUB,"MM sent TC-PDU, signaling");
      ps_mode = FALSE;
      tc_process_tc_pdu(cmd_ptr->cmd.mm_unitdata_ind.data, 
                        cmd_ptr->cmd.mm_unitdata_ind.message_len);
      break;

    case MMTC_RAB_SETUP_IND:
    case RABMTC_RAB_SETUP_IND:
      MSG_MED_DS_0(TC_SUB,"MM sent TC-PDU, signaling");
      tc_process_rab_setup(cmd_ptr->cmd.mm_rab_est_ind.rab_id);
      break;

    case MMTC_RAB_RELEASE_IND:
    case RABMTC_RAB_REL_IND:
      MSG_MED_DS_0(TC_SUB,"MM sent TC-PDU, signaling");
      tc_process_rab_release(cmd_ptr->cmd.mm_rab_rel_ind.rab_id);
      break;

    default:
      ERR (" UNKNOWN cmd from MM : %d", cmd_ptr->header.cmd_id,0,0);
      break;
  };
}

/*===========================================================================
FUNCTION SEND_GMM_TCPDU 

DESCRIPTION
  This function sends the packed TC-PDU to the GMM to be sent to the Network. 

DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void send_gmm_tcpdu(byte mesg_type)
{
  mm_cmd_type * gmm_cmd;
  byte l3_buf[MAX_OCTETS_L3_MESSAGE]; /* Buffer to store the packed L3 msg*/
  byte tc_reply_length;

  // Send the pending message to GMM.
  tc_pack_layer3_mesg (l3_buf, mesg_type);
  tc_reply_length= TC_REPLY_LENGTH;

#ifdef FEATURE_MBMS
  #error code not present
#endif

  gmm_cmd = mm_get_cmd_buf();

  if (gmm_cmd != NULL)
  {
    // Route the request to GMM in PS mode
    if (ps_mode)
    {
	   gmm_cmd->cmd.gmmtc_unitdata_req.header.message_set     = MS_GMM_TC;
       gmm_cmd->cmd.gmmtc_unitdata_req.header.message_id      = GMMTC_UNITDATA_REQ;
    }
    // Route the request to MM in CS mode
    else
    {
	   gmm_cmd->cmd.gmmtc_unitdata_req.header.message_set     = MS_MM_TC;
       gmm_cmd->cmd.gmmtc_unitdata_req.header.message_id      = MMTC_UNITDATA_REQ;
    }
    MSG_HIGH_DS_1(TC_SUB, "Sending TC-PDU with MSG_SET : %d where MS_GMM_TC/MS_MM_TC(104/105)", gmm_cmd->cmd.gmmtc_unitdata_req.header.message_set );
    gmm_cmd->cmd.gmmtc_unitdata_req.header.message_len_lsb = TC_REPLY_LENGTH;
    gmm_cmd->cmd.gmmtc_unitdata_req.header.message_len_msb = 0;

#ifdef FEATURE_MBMS
    #error code not present
#endif

    memscpy(gmm_cmd->cmd.gmmtc_unitdata_req.data, MAX_OCTETS_L3_MESSAGE, l3_buf, tc_reply_length);   
    mm_put_cmd (gmm_cmd);
  }
  else
  {
    ERR( "Unable to get a GMM command buffer",0,0,0);
  } 
}

/*===========================================================================
FUNCTION TC_PROCESS_GMM_ERROR_IND 

DESCRIPTION
  This function processes the error indication GMM. 
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tc_process_mmgmm_error_ind(
  tc_cmd_type *error_ind /*lint -esym(715, error_ind)*/)
{
  // Process the Error Indication report received from GMM.
  
  /* ---------------------------------------------------------------------
  **  Check the current RB state - Notify CM to move to NORMAL mode if the
  **  RB state is RB_TEST_MODE or RB_TEST_MODE_PENDING_CM_CONFIRM.
  ** --------------------------------------------------------------------*/
  if (rb_test_mode == RB_TEST_MODE || 
      rb_test_mode == RB_TEST_MODE_PENDING_CM_CONFIRM) 
  {
    MSG_HIGH_DS_0(TC_SUB,"Notify CM : UE Normal mode");
    rb_test_mode = RB_UNKNOWN_MODE;
    tc_current_state = TC_UNKNOWN_MODE;
    if (tc_rat_rab_established_on == SYS_RAT_UMTS_RADIO_ACCESS)
    {
#ifdef FEATURE_WCDMA
      lb_deregister_wm_rlc ();
#endif
    }
#ifdef FEATURE_TDSCDMA
    else if (tc_rat_rab_established_on == SYS_RAT_TDS_RADIO_ACCESS)
    {
      lb_deregister_wm_tdsrlc();
    }
#endif /* FEATURE_TDSCDMA */
    lb_cleanup();
    tc_cm_notify_normal_mode(); 
  }
}

/*===========================================================================
FUNCTION TC_PROCESS_TC_PDU 

DESCRIPTION
  This function processes TC-PDU sent by GMM. 
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tc_process_tc_pdu(byte* tc_data, byte tc_length)
{
  byte ext_buf[MAX_OCTETS_L3_MESSAGE];
  byte recd_msg;
  boolean valid_message;

  valid_message = TRUE;
  
  tc_length = (byte) MIN(tc_length, MAX_OCTETS_L3_MESSAGE);
  memset (ext_buf, 0, MAX_OCTETS_L3_MESSAGE);
  memscpy(ext_buf, MAX_OCTETS_L3_MESSAGE, tc_data, tc_length);

  
  // Validate the Protocol descriminator
  if (ext_buf[0]!= TC_PD || tc_length < 2)
  {
    ERR( "Ignore L3 Mesg, Wrong PD for TC: %d or Length: %d", ext_buf[0], tc_length, 0);
    return;
  }

  recd_msg = ext_buf[1];

  // Check the length of the received message
  if ((recd_msg == CLOSE_UE_TEST_LOOP) &&  /*lint !e641 */
      (tc_length < TC_CLOSE_UE_TEST_LOOP_LEN))
  {
	valid_message = FALSE;
  }

  if ((recd_msg != CLOSE_UE_TEST_LOOP) &&  /*lint !e641 */
      (tc_length < TC_OPEN_UE_TEST_LOOP_LEN))
  {
	valid_message = FALSE;
  }

  if (valid_message == FALSE)
  {
    ERR( "Mandatory IE missing in TC msg, Discarding the Invalid TC PDU, recd_msg :0x%X", recd_msg, 0, 0);
    return;
  }
  
  MSG_HIGH_DS_2(TC_SUB," Processing TC-PDU, MSG :0x%X, Current Mode :0x%X ", recd_msg, rb_test_mode );  
  // Processed the received TC message based on the message type.
  switch(recd_msg)
  {
    case CLOSE_UE_TEST_LOOP:
	  tc_close_ue_test_loop(ext_buf);
    break;

    case OPEN_UE_TEST_LOOP:
	  tc_open_ue_test_loop();
    break;

    case ACTIVATE_RB_TEST_MODE:
	  tc_activate_rb_test_mode();
    break;

    case DEACTIVATE_RB_TEST_MODE:
	  tc_deactivate_rb_test_mode();
    break;

#ifdef FEATURE_MBMS
    #error code not present
#endif

    default:
      ERR("Received Un-identified TC message",0,0,0);
    break;
  }
}

/*===========================================================================
FUNCTION TC_CLOSE_UE_TEST_LOOP 

DESCRIPTION
  This function processes CLOSE_UE_TEST_LOOP TC-PDU sent by GMM. 
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tc_close_ue_test_loop(byte* recd_msg)
{
  uint32 no_of_lb_entities = 0;
  uint32 index = 0;
#ifdef FEATURE_SEGMENT_LOADING
  interface_t *ptr_W = NULL;
  interface_t *ptr_T = NULL;
#endif
  dummy_dcch_enabled = FALSE;
#ifdef FEATURE_SEGMENT_LOADING
  ptr_W = get_wcdma_interface();
  ptr_T = get_tdscdma_interface();
#endif  

  if (rb_test_mode != RB_TEST_MODE )
  {
    ERR( "UE not in RB TEST Mode, Ignore",0,0,0);
  }
  else if (is_tc_test_loop_closed())
  {
    ERR( "Received CLOSE_UE_TEST_LOOP when the loop is already closed", 0, 0, 0);
    /* --------------------------------------------------------------------------------
     Per 5.3.2.3 of TS34.109, If the test loop is already closed when it receives 
     CLOSE_UE_TEST_LOOP from SS, the UE shall still respond as if the loop had been 
     open, i.e. the CLOSE UE TEST LOOP COMPLETE message shall be sent.
     ---------------------------------------------------------------------------------- */
    tc_close_ue_test_loop_complete(); 
  }
  else
  {
    uint32 rb_index[MAX_RAB_TO_SETUP];
    word rab_index;
    byte lc_index;
#ifdef FEATURE_MBMS
    #error code not present
#endif

    MSG_HIGH_DS_1(TC_SUB, "UE entering Loopback Mode :%d", ((recd_msg[2] & 0x03)+1) );
    // Get the UE test loopback mode fields from the message.     
    if((recd_msg[2] & 0x03) == 0)
    {
      // Received Loopback Mode 1 setup message
	  tc_current_state = TC_LOOPBACK_MODE1;
    }
    else if((recd_msg[2] & 0x03) == 1)
    {
      // Received Loopback Mode 2 setup message
	  tc_current_state = TC_LOOPBACK_MODE2;
    }
#ifdef FEATURE_MBMS
    #error code not present
#endif

    if ((recd_msg[2] & 0x04) != 0)
    {
      // Received Dummy DCCH request
      MSG_HIGH_DS_0(TC_SUB, "UE enabling Dummy signalling on DCCH");
      dummy_dcch_enabled = TRUE;
    }

    if (tc_current_state == TC_LOOPBACK_MODE1)
    {
      no_of_lb_entities = recd_msg[3] / 3;
      MSG_HIGH_DS_1(TC_SUB, "No. of LB entities = %d", no_of_lb_entities);

      if (no_of_lb_entities > MAX_RB_PER_RAB)
      {
        no_of_lb_entities = MAX_RB_PER_RAB;
      }
    }

    for (index=0; index<MAX_RAB_TO_SETUP; index++)
    {
      rb_index[index] = 0;
    }
    
    if (tc_current_state == TC_LOOPBACK_MODE1)
    {
      for (index=0; index<no_of_lb_entities; index++)
      {
        byte msg_buf[2];
        rrc_rab_id_type rab_id = RRC_INVALID_RAB_ID;
        uint32 offset          = (index+1) * 3;
        rb_id_type rb_id       = recd_msg[offset+3];
        if (tc_rat_rab_established_on == SYS_RAT_UMTS_RADIO_ACCESS)
        {
#ifdef FEATURE_WCDMA
#ifdef FEATURE_SEGMENT_LOADING
          rab_id = IWCDMA_rrc_find_rab_for_rb_in_est_rabs (ptr_W, rb_id);
#else
          rab_id = rrc_find_rab_for_rb_in_est_rabs (rb_id);
#endif
#endif
        }
#ifdef FEATURE_TDSCDMA
        else if (tc_rat_rab_established_on == SYS_RAT_TDS_RADIO_ACCESS)
        {
#ifdef FEATURE_SEGMENT_LOADING
          rab_id = ITDSCDMA_tdsrrc_find_rab_for_rb_in_est_rabs (ptr_T, rb_id);
#else
          rab_id = tdsrrc_find_rab_for_rb_in_est_rabs (rb_id);
#endif
        }
#endif /* FEATURE_TDSCDMA */
        
        if ( rab_id == RRC_INVALID_RAB_ID || lb_rab_list[rab_id] == NULL)
        {
          ERR ("Invalid RB ID = %d, RAB ID = %d received in CLOSE_UE_TEST_LOOP from SS", rb_id, rab_id, 0);
        }
        else
        {
          msg_buf[0] = recd_msg[offset+2];
          msg_buf[1] = recd_msg[offset+1];
          
          lb_rab_list[rab_id]->sdu_type = RLC_SDU_TYPE; /*lint !e641 */

          memscpy(&lb_rab_list[rab_id]->ul_rlc_sdu_size[rb_index[rab_id]], sizeof(word), msg_buf, 2);
          MSG_HIGH_DS_3(TC_SUB,"TC: rab id = %d, rb id = %d, rlc_ul_sdu_size = %d", 
                   rab_id, rb_id, lb_rab_list[rab_id]->ul_rlc_sdu_size[rb_index[rab_id]]);
          if (lb_rab_list[rab_id]->ul_rlc_sdu_size[rb_index[rab_id]] == 0)
          {
            lb_rab_list[rab_id]->ul_rlc_sdu_size_valid[rb_index[rab_id]] = FALSE;
          }
          rb_index[rab_id]++;
        }
      }     
      for (rab_index=0; rab_index<MAX_RAB_TO_SETUP ; rab_index++)
      {
        MSG_HIGH_DS_2(TC_SUB,"TC: RAB Index = %d, Address = %d", rab_index, lb_rab_list[rab_index]);
        if (lb_rab_list[rab_index] != NULL)
        {
          MSG_HIGH_DS_1(TC_SUB,"TC: RAB estb = %d", lb_rab_list[rab_index]->rab_established);
        }       
        if (lb_rab_list[rab_index] != NULL  && lb_rab_list[rab_index]->rab_established )
        {
          byte num_ul_lc = 0;

          if (tc_rat_rab_established_on == SYS_RAT_UMTS_RADIO_ACCESS)
          {
            num_ul_lc = lb_rab_list[rab_index]->lc_info_ptr.num_ul_lc_ids_for_rab;
          }
#ifdef FEATURE_TDSCDMA
          else if (tc_rat_rab_established_on == SYS_RAT_TDS_RADIO_ACCESS)
          {
            num_ul_lc = lb_rab_list[rab_index]->tdslc_info_ptr.num_ul_lc_ids_for_rab;
          }
#endif /* FEATURE_TDSCDMA */
          MSG_HIGH_DS_1(TC_SUB,"TC: num_ul_lc = %d", num_ul_lc);

          if(lb_rab_list[rab_index]->lc_info_ptr.num_ul_lc_ids_for_rab > RRC_MAX_LC_ID_PER_RAB )
          {
            // if RRC_MAX_LC_ID_PER_RAB is invalid, skipiing copying in lb_rab_list[rab_index]
            num_ul_lc = 0;
            ERR( "Invalid num_ul_lc_ids_for_rab %d", lb_rab_list[rab_index]->lc_info_ptr.num_ul_lc_ids_for_rab,0,0);
          }
          if (num_ul_lc > 0)
          {
            for (lc_index = 0 ; lc_index < num_ul_lc ; lc_index++)
            {
              MSG_HIGH_DS_3(TC_SUB,"TC: lc_index = %d, valid: %d, size: %d", lc_index , lb_rab_list[rab_index]->ul_rlc_sdu_size_valid[lc_index], lb_rab_list[rab_index]->ul_rlc_sdu_size[lc_index] );
              if (lb_rab_list[rab_index]->ul_rlc_sdu_size_valid[lc_index] == TRUE)
              {
                lb_rab_list[rab_index]->lb_rlc_dl_wm[lc_index].dont_exceed_cnt        = MAX_LB_SDU_SIZE * 20;
                /*lint -e{64} Reviewed with the process group and recommended for suppression */
                lb_rab_list[rab_index]->lb_rlc_dl_wm[lc_index].each_enqueue_func_ptr  = lb_rx_data_from_rlc;
                MSG_HIGH_DS_2(TC_SUB,"TC: Changing Dont Exceed Cnt for RAB = %d, LC index = %d", rab_index, lc_index);
                MSG_HIGH_DS_2(TC_SUB,"UL DNE = %d, DL DNE = %d", lb_rab_list[rab_index]->lb_rlc_ul_wm[lc_index].dont_exceed_cnt, 
                  lb_rab_list[rab_index]->lb_rlc_dl_wm[lc_index].dont_exceed_cnt);
              }
            }
          }
        }
      }
    }
    
    else if (tc_current_state == TC_LOOPBACK_MODE2) 
      {
        for (rab_index=0; rab_index<MAX_RAB_TO_SETUP; rab_index++)
        {
          if (lb_rab_list[rab_index] != NULL && lb_rab_list[rab_index]->rab_established)
          {
            for (lc_index=0; lc_index<UE_MAX_DL_LOGICAL_CHANNEL; lc_index++)
            {
              lb_rab_list[rab_index]->lb_rlc_dl_wm[lc_index].dont_exceed_cnt        = MAX_LB_SDU_SIZE * 20;
              /*lint -e{64} Reviewed with the process group and recommended for suppression */
              lb_rab_list[rab_index]->lb_rlc_dl_wm[lc_index].each_enqueue_func_ptr  = lb_rx_data_from_rlc;
              MSG_HIGH_DS_2(TC_SUB,"TC: Changing Dont Exceed Cnt for RAB = %d, LC index = %d", rab_index, lc_index);
            }
          }
        }
      }
#ifdef FEATURE_MBMS
    #error code not present
#endif
    
    // Notify CM about loopback mode 
    MSG_HIGH_DS_0(TC_SUB,"Notify CM: UE loopback mode");
    tc_cm_notify_lb_mode(tc_current_state, dummy_dcch_enabled); /*lint !e641 */

    // Notify RRC about loopback mode 
    MSG_HIGH_DS_0(TC_SUB,"Notify RRC: UE loopback mode");
    (void) tc_rrc_notify_lb_mode(tc_current_state, dummy_dcch_enabled); /*lint !e641 */

  }
}

/*===========================================================================
FUNCTION TC_CLOSE_UE_TEST_LOOP_COMPLETE 

DESCRIPTION
  This function sends CLOSE_UE_TEST_LOOP_COMPLETE PDU to MM/GMM. 
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tc_close_ue_test_loop_complete()
{
  send_gmm_tcpdu(CLOSE_UE_TEST_LOOP_COMPLETE); /*lint !e641*/
  tc_test_loop_closed = TRUE;
}
/*===========================================================================
FUNCTION TC_OPEN_UE_TEST_LOOP 

DESCRIPTION
  This function processes OPEN_UE_TEST_LOOP TC-PDU sent by GMM. 
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tc_open_ue_test_loop()
{  
  if (tc_current_state != TC_UNKNOWN_MODE)
  {
    // Set the TC current mode to default mode and de-register RLC queues.
    MSG_HIGH_DS_0(TC_SUB,"Changing Test mode to INACTIVE");
    tc_current_state = TC_UNKNOWN_MODE;
    tc_test_loop_closed = FALSE;

    //lb_deregister_wm_rlc ();
    lb_set_ul_sdu_size_to_zero();
#ifdef FEATURE_MBMS
    #error code not present
#endif

	(void) tc_rrc_notify_normal_mode(); 
	(void) tc_rrc_notify_test_mode();  

    // Send the OPEN_UE_TEST_LOOP_COMPLETE message to Network.
    send_gmm_tcpdu(OPEN_UE_TEST_LOOP_COMPLETE); /*lint !e641 */
  }
  else
  {
    ERR("Received Open test loop in wrong state",0,0,0);
  }
}

#ifdef FEATURE_WCDMA

/*===========================================================================
FUNCTION LB_DEREGISTER_WM_RLC 

DESCRIPTION
  This function processes DEREGISTERS the WM with RLC for all the RABs that are established. 
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void lb_deregister_wm_rlc(void)
{  
  byte rab_index;
  word lc_index;
#ifdef FEATURE_SEGMENT_LOADING
  interface_t *ptr_W = NULL;

  ptr_W = get_wcdma_interface();

  if (ptr_W == NULL)
  {     
      ERR_FATAL("ERROR: Wrong segment", 0,0,0 );
  }
#endif

  for (rab_index=0; rab_index<MAX_RAB_TO_SETUP; rab_index++)
  {
    if (lb_rab_list[rab_index] != NULL )
    {
      int num_ul_lc, num_dl_lc;

      /* Uplink parameters to be De-Registered with RLC */
      rlc_ul_deregister_srvc_type lb_rlc_ul_dereg_params;
      /* Downlink parameters to be De-Registered with RLC */
      rlc_dl_deregister_srvc_type lb_rlc_dl_dereg_params;

      num_ul_lc = lb_rab_list[rab_index]->lc_info_ptr.num_ul_lc_ids_for_rab;
      num_dl_lc = lb_rab_list[rab_index]->lc_info_ptr.num_dl_lc_ids_for_rab;
      lb_rlc_ul_dereg_params.nchan = 0;
      lb_rlc_dl_dereg_params.nchan = 0;

      lb_rlc_ul_dereg_params.nchan = (uint8) num_ul_lc;
      for (lc_index=0 ; lc_index<num_ul_lc ; lc_index++ )
      {
          lb_rlc_ul_dereg_params.rlc_id[lc_index] = lb_rab_list[rab_index]->lc_info_ptr.ul_lc_id[ lc_index ];
          lb_rab_list[rab_index]->ul_rlc_sdu_size[lc_index] = 0; 
          lb_rab_list[rab_index]->ul_rlc_sdu_size_valid[lc_index] = TRUE; 
      }

      lb_rlc_dl_dereg_params.nchan = (uint8) num_dl_lc;
      for (lc_index=0 ; lc_index<num_dl_lc ; lc_index++ )
      {
          lb_rlc_dl_dereg_params.rlc_id[lc_index] = lb_rab_list[rab_index]->lc_info_ptr.dl_lc_id[ lc_index ];
      }

      if (lb_rlc_ul_dereg_params.nchan > 0)
      {
        MSG_HIGH_DS_2(TC_SUB,"De-Registering RAB = %d, UL :: nchan = %d", rab_index, lb_rlc_ul_dereg_params.nchan);
        for (lc_index=0 ; lc_index<num_ul_lc ; lc_index++ )
        {
          MSG_HIGH_DS_1(TC_SUB,"De-Registering UL ::  ul_lc_id = %d", lb_rlc_ul_dereg_params.rlc_id[lc_index]);
        }
#ifdef FEATURE_SEGMENT_LOADING
        IWCDMA_rlc_ul_deregister_srvc_handler (ptr_W, &lb_rlc_ul_dereg_params) ;
#else
        rlc_ul_deregister_srvc_handler (&lb_rlc_ul_dereg_params) ;
#endif
      }

      if ( lb_rlc_dl_dereg_params.nchan > 0)
      {
        MSG_HIGH_DS_2(TC_SUB,"De-Registering RAB = %d, DL :: nchan = %d", rab_index, lb_rlc_dl_dereg_params.nchan);
        for (lc_index=0 ; lc_index<num_dl_lc ; lc_index++ )
        {
          MSG_HIGH_DS_1(TC_SUB,"De-Registering DL ::  dl_lc_id = %d", lb_rlc_dl_dereg_params.rlc_id[lc_index]);
        }
#ifdef FEATURE_SEGMENT_LOADING
        IWCDMA_rlc_dl_deregister_srvc_handler (ptr_W, &lb_rlc_dl_dereg_params) ;
#else
        rlc_dl_deregister_srvc_handler (&lb_rlc_dl_dereg_params) ;
#endif
      }
    }
  }
}

#endif

#ifdef FEATURE_TDSCDMA
/*===========================================================================
FUNCTION LB_DEREGISTER_WM_TDSRLC 

DESCRIPTION
  This function processes DEREGISTERS the WM with TD-SCDMA RLC
  for all the RABs that are established. 
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void lb_deregister_wm_tdsrlc( void )
{  
  byte rab_index;
  word lc_index;
#ifdef FEATURE_SEGMENT_LOADING
  interface_t *ptr_T = NULL;

  ptr_T = get_tdscdma_interface();

  if (ptr_T == NULL)
  {     
      MSG_ERROR_DS_0(TC_SUB,"Segment Loading Error");
      return;
  }
#endif
  for (rab_index=0; rab_index<MAX_RAB_TO_SETUP; rab_index++)
  {
    if (lb_rab_list[rab_index] != NULL )
    {
      int num_ul_lc, num_dl_lc;

      tdsl2_ul_service_register_type lb_tdsrlc_ul_dereg_params;
      /* Uplink parameters to be registered with RLC */
      tdsl2_dl_service_register_type lb_tdsrlc_dl_dereg_params;
      /* Downlink parameters to be registered with RLC */

      num_ul_lc = lb_rab_list[rab_index]->tdslc_info_ptr.num_ul_lc_ids_for_rab;
      num_dl_lc = lb_rab_list[rab_index]->tdslc_info_ptr.num_dl_lc_ids_for_rab;

      lb_tdsrlc_ul_dereg_params.nchan = (uint8) num_ul_lc;
      lb_tdsrlc_ul_dereg_params.nchan = MIN(lb_tdsrlc_ul_dereg_params.nchan, RRC_MAX_LC_ID_PER_RAB);
      for (lc_index=0 ; lc_index<num_ul_lc ; lc_index++ )
      {
          lb_tdsrlc_ul_dereg_params.rlc_id[lc_index] =
            lb_rab_list[rab_index]->tdslc_info_ptr.ul_lc_id[ lc_index ];
          lb_rab_list[rab_index]->ul_rlc_sdu_size[lc_index] = 0; 
          lb_rab_list[rab_index]->ul_rlc_sdu_size_valid[lc_index] = TRUE; 
      }

      lb_tdsrlc_dl_dereg_params.nchan = (uint8) num_dl_lc;
      lb_tdsrlc_dl_dereg_params.nchan = MIN(lb_tdsrlc_dl_dereg_params.nchan, RRC_MAX_LC_ID_PER_RAB);
      for (lc_index=0 ; lc_index<num_dl_lc ; lc_index++ )
      {
          lb_tdsrlc_dl_dereg_params.rlc_id[lc_index] =
            lb_rab_list[rab_index]->tdslc_info_ptr.dl_lc_id[ lc_index ];
      }

      if (lb_tdsrlc_ul_dereg_params.nchan > 0)
      {
        MSG_HIGH_DS_2(TC_SUB,"De-Registering RAB = %d, UL :: nchan = %d", rab_index, lb_tdsrlc_ul_dereg_params.nchan);
        lb_tdsrlc_ul_dereg_params.service = FALSE;  /* FALSE = De-register */
        for (lc_index=0 ; lc_index<num_ul_lc ; lc_index++ )
        {
          MSG_HIGH_DS_1(TC_SUB,"De-Registering UL ::  ul_lc_id = %d",
                    lb_tdsrlc_ul_dereg_params.rlc_id[lc_index]);
        }
#ifdef FEATURE_SEGMENT_LOADING
        (void) ITDSCDMA_tdsl2_ul_service_register(ptr_T, &lb_tdsrlc_ul_dereg_params );
#else
        (void) tdsl2_ul_service_register( &lb_tdsrlc_ul_dereg_params );
#endif
      }

      if (lb_tdsrlc_dl_dereg_params.nchan > 0)
      {
        MSG_HIGH_DS_2(TC_SUB,"De-Registering RAB = %d, DL :: nchan = %d",
                  rab_index, lb_tdsrlc_dl_dereg_params.nchan);
        lb_tdsrlc_dl_dereg_params.service = FALSE;  /* FALSE = De-register */
        for (lc_index=0 ; lc_index<num_dl_lc ; lc_index++ )
        {
          MSG_HIGH_DS_1(TC_SUB,"De-Registering DL ::  dl_lc_id = %d",
                    lb_tdsrlc_dl_dereg_params.rlc_id[lc_index]);
        }
#ifdef FEATURE_SEGMENT_LOADING
        (void) ITDSCDMA_tdsl2_dl_service_register(ptr_T,  &lb_tdsrlc_dl_dereg_params );
#else
        (void) tdsl2_dl_service_register( &lb_tdsrlc_dl_dereg_params );
#endif
      }
    }
  }
}
#endif /* FEATURE_TDSCDMA */

#ifdef FEATURE_WCDMA

/*===========================================================================
FUNCTION LB_DEREGISTER_RAB_WM_RLC 

DESCRIPTION
  This function processes DEREGISTERS the WM with RLC for the Specified RABs that is established. 
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void lb_deregister_rab_wm_rlc(byte rab_index)
  {  
  word lc_index;
  int num_ul_lc, num_dl_lc;
  
  /* Uplink parameters to be De-Registered with RLC */
  rlc_ul_deregister_srvc_type lb_rlc_ul_dereg_params;
  /* Downlink parameters to be De-Registered with RLC */
  rlc_dl_deregister_srvc_type lb_rlc_dl_dereg_params;
#ifdef FEATURE_SEGMENT_LOADING
  interface_t* ptr_W = NULL;

  ptr_W = get_wcdma_interface();
  if (ptr_W == NULL)
  {
     ERR_FATAL("Segment Loading Error\n", 0,0,0);
  }  
#endif
  num_ul_lc = lb_rab_list[rab_index]->lc_info_ptr.num_ul_lc_ids_for_rab;
  num_dl_lc = lb_rab_list[rab_index]->lc_info_ptr.num_dl_lc_ids_for_rab;
  lb_rlc_ul_dereg_params.nchan = 0;
  lb_rlc_dl_dereg_params.nchan = 0;
  
  lb_rlc_ul_dereg_params.nchan = (uint8) num_ul_lc;
  for (lc_index=0 ; lc_index<num_ul_lc ; lc_index++ )
  {
      lb_rlc_ul_dereg_params.rlc_id[lc_index] = lb_rab_list[rab_index]->lc_info_ptr.ul_lc_id[ lc_index ];
      lb_rab_list[rab_index]->ul_rlc_sdu_size[lc_index] = 0; 
      lb_rab_list[rab_index]->ul_rlc_sdu_size_valid[lc_index] = TRUE; 
  }
  
  lb_rlc_dl_dereg_params.nchan = (uint8) num_dl_lc;
  for (lc_index=0 ; lc_index<num_dl_lc ; lc_index++ )
  {
      lb_rlc_dl_dereg_params.rlc_id[lc_index] = lb_rab_list[rab_index]->lc_info_ptr.dl_lc_id[ lc_index ];
  }
  
  if (lb_rlc_ul_dereg_params.nchan > 0)
  {
    MSG_HIGH_DS_2(TC_SUB,"De-Registering RAB = %d, UL :: nchan = %d", rab_index, lb_rlc_ul_dereg_params.nchan);
    for (lc_index=0 ; lc_index<num_ul_lc ; lc_index++ )
    {
      MSG_HIGH_DS_1(TC_SUB,"De-Registering UL ::  ul_lc_id = %d", lb_rlc_ul_dereg_params.rlc_id[lc_index]);
    }
#ifdef FEATURE_SEGMENT_LOADING
    IWCDMA_rlc_ul_deregister_srvc_handler (ptr_W, &lb_rlc_ul_dereg_params) ;
#else
    rlc_ul_deregister_srvc_handler (&lb_rlc_ul_dereg_params) ;
#endif
  }
  
  if ( lb_rlc_dl_dereg_params.nchan > 0)
  {
    MSG_HIGH_DS_2(TC_SUB,"De-Registering RAB = %d, DL :: nchan = %d", rab_index, lb_rlc_dl_dereg_params.nchan);
    for (lc_index=0 ; lc_index<num_dl_lc ; lc_index++ )
    {
      MSG_HIGH_DS_1(TC_SUB,"De-Registering DL ::  dl_lc_id = %d", lb_rlc_dl_dereg_params.rlc_id[lc_index]);
    }
#ifdef FEATURE_SEGMENT_LOADING
    IWCDMA_rlc_dl_deregister_srvc_handler (ptr_W, &lb_rlc_dl_dereg_params) ;
#else
    rlc_dl_deregister_srvc_handler (&lb_rlc_dl_dereg_params) ;
#endif
  }
}

#endif

#ifdef FEATURE_TDSCDMA
/*===========================================================================
FUNCTION LB_DEREGISTER_RAB_WM_TDSRLC 

DESCRIPTION
  This function processes DEREGISTERS the WM with TD-SCDMA RLC
  for the Specified RABs that is established. 
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void lb_deregister_rab_wm_tdsrlc(byte rab_index)
  {  
  word lc_index;
  int num_ul_lc, num_dl_lc;
#ifdef FEATURE_SEGMENT_LOADING
  interface_t* ptr_T = NULL;
#endif
  tdsl2_ul_service_register_type lb_tdsrlc_ul_dereg_params;
  /* Uplink parameters to be registered with RLC */
  tdsl2_dl_service_register_type lb_tdsrlc_dl_dereg_params;
  /* Downlink parameters to be registered with RLC */
#ifdef FEATURE_SEGMENT_LOADING
  ptr_T = get_tdscdma_interface();

  if (ptr_T == NULL)
  {
     ERR_FATAL("Segment Loading error \n", 0,0,0);
  }
#endif
  num_ul_lc = lb_rab_list[rab_index]->tdslc_info_ptr.num_ul_lc_ids_for_rab;
  num_dl_lc = lb_rab_list[rab_index]->tdslc_info_ptr.num_dl_lc_ids_for_rab;
  
  lb_tdsrlc_ul_dereg_params.nchan = (uint8) num_ul_lc;
  lb_tdsrlc_ul_dereg_params.nchan = MIN(lb_tdsrlc_ul_dereg_params.nchan, RRC_MAX_LC_ID_PER_RAB);  
  for (lc_index=0 ; lc_index<num_ul_lc ; lc_index++ )
  {
      lb_tdsrlc_ul_dereg_params.rlc_id[lc_index] =
        lb_rab_list[rab_index]->tdslc_info_ptr.ul_lc_id[ lc_index ];
      lb_rab_list[rab_index]->ul_rlc_sdu_size[lc_index] = 0; 
      lb_rab_list[rab_index]->ul_rlc_sdu_size_valid[lc_index] = TRUE; 
  }
  
  lb_tdsrlc_dl_dereg_params.nchan = (uint8) num_dl_lc;
  lb_tdsrlc_dl_dereg_params.nchan = MIN(lb_tdsrlc_dl_dereg_params.nchan, RRC_MAX_LC_ID_PER_RAB);  
  for (lc_index=0 ; lc_index<num_dl_lc ; lc_index++ )
  {
      lb_tdsrlc_dl_dereg_params.rlc_id[lc_index] =
        lb_rab_list[rab_index]->tdslc_info_ptr.dl_lc_id[ lc_index ];
  }
  
  if (lb_tdsrlc_ul_dereg_params.nchan > 0)
  {
    MSG_HIGH_DS_2(TC_SUB,"De-Registering RAB = %d, UL :: nchan = %d", rab_index, lb_tdsrlc_ul_dereg_params.nchan);
    lb_tdsrlc_ul_dereg_params.service = FALSE;  /* FALSE = De-register */
    for (lc_index=0 ; lc_index<num_ul_lc ; lc_index++ )
    {
      MSG_HIGH_DS_1(TC_SUB,"De-Registering UL ::  ul_lc_id = %d",
                lb_tdsrlc_ul_dereg_params.rlc_id[lc_index]);
    }
#ifdef FEATURE_SEGMENT_LOADING
    (void) ITDSCDMA_tdsl2_ul_service_register(ptr_T,  &lb_tdsrlc_ul_dereg_params );
#else
    (void) tdsl2_ul_service_register( &lb_tdsrlc_ul_dereg_params );
#endif
  }
  
  if (lb_tdsrlc_dl_dereg_params.nchan > 0)
  {
    MSG_HIGH_DS_2(TC_SUB,"De-Registering RAB = %d, DL :: nchan = %d",
              rab_index, lb_tdsrlc_dl_dereg_params.nchan);
    lb_tdsrlc_dl_dereg_params.service = FALSE;  /* FALSE = De-register */
    for (lc_index=0 ; lc_index<num_dl_lc ; lc_index++ )
    {
      MSG_HIGH_DS_1(TC_SUB, "De-Registering DL ::  dl_lc_id = %d",
                lb_tdsrlc_dl_dereg_params.rlc_id[lc_index]);
    }
#ifdef FEATURE_SEGMENT_LOADING
    (void) ITDSCDMA_tdsl2_dl_service_register( ptr_T, &lb_tdsrlc_dl_dereg_params );
#else
    (void) tdsl2_dl_service_register( &lb_tdsrlc_dl_dereg_params );
#endif
  }
}
#endif /* FEATURE_TDSCDMA */

/*===========================================================================
FUNCTION     : LB_SET_UL_SDU_SIZE_TO_ZERO

DESCRIPTION  : This function sets the UpLink SDU sizes to zero

DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/ 
void lb_set_ul_sdu_size_to_zero(void)
{  
  byte rab_index;
  word lc_index;

  for (rab_index=0; rab_index<MAX_RAB_TO_SETUP; rab_index++)
  {
    if (lb_rab_list[rab_index] != NULL && lb_rab_list[rab_index]->rab_established == TRUE )
    {
      for (lc_index=0; lc_index<UE_MAX_UL_LOGICAL_CHANNEL; lc_index++)
      {
        lb_rab_list[rab_index]->ul_rlc_sdu_size[lc_index] = 0; 
        lb_rab_list[rab_index]->ul_rlc_sdu_size_valid[lc_index] = TRUE; 
      }
      for (lc_index=0; lc_index<UE_MAX_DL_LOGICAL_CHANNEL; lc_index++)
      {
        lb_rab_list[rab_index]->lb_rlc_dl_wm[lc_index].dont_exceed_cnt = 1;
        lb_rab_list[rab_index]->lb_rlc_dl_wm[lc_index].each_enqueue_func_ptr = NULL;
      }
    }
  }
}
/*===========================================================================
FUNCTION TC_ACTIVATE_RB_TEST_MODE 

DESCRIPTION
  This function puts the UE in RB_TEST_MODE 
  
DEPENDENCIES
  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/ 
void tc_activate_rb_test_mode()
{
  /* ---------------------------------------------------------------------
  ** Process the RB activate test mode message from Network 
  ** --------------------------------------------------------------------*/
  switch(rb_test_mode)
  {
     case RB_UNKNOWN_MODE:    
        /* -------------------------------------------------------
        ** If current RB state is RB_UNKNOWN MODE :
        **    (1) set RB state to RB_TEST_MODE_PENDING_CM_CONFIRM 
        **    (2) Notify CM about change of mode to test
        ** -------------------------------------------------------*/
        rb_test_mode = RB_TEST_MODE_PENDING_CM_CONFIRM;
		
        tc_cm_notify_test_setup_in_progress(); 
        break;

     case RB_TEST_MODE_PENDING_CM_CONFIRM:
        /* -------------------------------------------------------
        ** If current RB state is RB_TEST_MODE_PENDING_CM_CONFIRM :
        **    (1) Notify RRC about change of mode to test 
        **    (2) set RB state to RB_TEST_MODE
        **    (3) send ACTIVATE_RB_TEST_MODE_COMPLETE to network
        ** -------------------------------------------------------*/
	   (void) tc_rrc_notify_test_mode();  

        // Notify RABM about test loop in PS mode
		tc_notify_rabm(UE_LB_MODE1);

        rb_test_mode = RB_TEST_MODE; 
        send_gmm_tcpdu(ACTIVATE_RB_TEST_MODE_COMPLETE); /*lint !e641 */
        break;

     case RB_TEST_MODE:
        MSG_HIGH_DS_0(TC_SUB,"UE is already in RB_TEST_MODE");
        break;

     default:
        ERR("Unknown RB_TEST_MODE",0,0,0);
        break;
  }  
}

/*===========================================================================
FUNCTION TC_DEACTIVATE_RB_TEST_MODE 

DESCRIPTION
  This function processes DEACTIVATE_RB_TEST_MODE TC-PDU sent by GMM. 
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tc_deactivate_rb_test_mode()
{
  /* ---------------------------------------------------------------------
  ** Process the RB deactivate test mode message from Network 
  **
  ** Step 1: If the current RB state is RB_TEST_MODE, notify RRC & RABM to 
  **         move to NORMAL mode 
  ** --------------------------------------------------------------------*/
  if (rb_test_mode == RB_TEST_MODE) 
  {
    MSG_HIGH_DS_0(TC_SUB,"Notify RRC : UE Normal mode");
    (void) tc_rrc_notify_normal_mode(); 
  }

  // Notify RABM about normal mode
  MSG_HIGH_DS_0(TC_SUB,"Notify RABM : UE Normal mode");
  tc_notify_rabm(UE_NORMAL_MODE); 

  /* ---------------------------------------------------------------------
  ** Step 2: Inform CM about NORMAL mode if the current RB state is
  **         RB_TEST_MODE or RB_TEST_MODE_PENDING_CM_CONFIRM
  ** ---------------------------------------------------------------------*/
  if (rb_test_mode == RB_TEST_MODE || 
      rb_test_mode == RB_TEST_MODE_PENDING_CM_CONFIRM) 
  {
    rb_test_mode = RB_UNKNOWN_MODE;
	tc_cm_notify_normal_mode(); 
  }
    
  /* ---------------------------------------------------------------------
  ** Step 3: Send DEACTIVATE_RB_TEST_MODE_COMPLETE_MSG to the network.
  **         Send TC PDU to GMM/MM
  ** --------------------------------------------------------------------*/
  tc_current_state = TC_UNKNOWN_MODE;
  tc_test_loop_closed = FALSE;
  if (tc_rat_rab_established_on == SYS_RAT_UMTS_RADIO_ACCESS)
  {
#ifdef FEATURE_WCDMA
    lb_deregister_wm_rlc ();
#endif
  }
#ifdef FEATURE_TDSCDMA
  else if (tc_rat_rab_established_on == SYS_RAT_TDS_RADIO_ACCESS)
  {
    lb_deregister_wm_tdsrlc();
  }
#endif /* FEATURE_TDSCDMA */
  lb_cleanup();

  sm_perform_local_deactivation((byte)PPD_UNKNOWN_REASON);

  send_gmm_tcpdu(DEACTIVATE_RB_TEST_MODE_COMPLETE);    /*lint !e641 */
}
#ifdef FEATURE_MBMS
#error code not present
#endif

/*===========================================================================
FUNCTION PACK_LAYER3_MESG 

DESCRIPTION
  This function packs a Layer3 message (TC-TC peer) to be sent to GMM. 
  
DEPENDENCIES
  It is assumed that all the mandatory IE fields are given by upper layers.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tc_pack_layer3_mesg
(
  byte* ext_buf,          /* Buffer contains the packed message */  
  byte msg_type /* TC message being packed */
)
{
  ext_buf[0] = TC_PD;            // Protocol Id for TC is 0xF
  ext_buf[1] = (byte)msg_type;   // TC message type 
}

/*===========================================================================
FUNCTION IS_TC_TEST_LOOP_CLOSED

DESCRIPTION
  This function returns TRUE if UE test loop is closed
  
DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
boolean is_tc_test_loop_closed (void)
{
  return tc_test_loop_closed;
}

/*===========================================================================
FUNCTION RESET_TC_LOOP

DESCRIPTION
  This function resets the TC loop
  
DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void reset_tc_loop (void)
{
   tc_test_loop_closed = FALSE;
}

/*===========================================================================
FUNCTION TC_PROCESS_RRC_LB_REJ

DESCRIPTION
  This function handles test loopback reject (RRCTC_TEST_LOOPBACK_REJ)
  message from RRC. It transitions TC from loopback test mode to normal
  mode, deregisters RLC queues, cleans up the UL/DL data structures &
  variables associated with the loopback call, notifies RABM & CM about
  transition of UE mode to NORMAL.
  
DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tc_handle_rrc_lb_rej (void)
{
    /* --------------------------------------------------------
    ** Transition TC from LB TEST mode to NORMAL mode 
    ** --------------------------------------------------------*/
    MSG_HIGH_DS_0(TC_SUB,"Changing Test mode to INACTIVE");

    tc_current_state = TC_UNKNOWN_MODE;
    tc_test_loop_closed = FALSE;
    rb_test_mode = RB_UNKNOWN_MODE;
    
    if (tc_rat_rab_established_on == SYS_RAT_UMTS_RADIO_ACCESS)
    {
#ifdef FEATURE_WCDMA
      lb_deregister_wm_rlc ();
#endif
    }
#ifdef FEATURE_TDSCDMA
    else if (tc_rat_rab_established_on == SYS_RAT_TDS_RADIO_ACCESS)
    {
      lb_deregister_wm_tdsrlc();
    }
#endif /* FEATURE_TDSCDMA */

    /* --------------------------------------------------------
    ** De-register RLC queues & cleanup the UL/DL data 
    ** structures and variables associated with LB call
    ** --------------------------------------------------------*/
    lb_cleanup();

    /* --------------------------------------------------------
    ** Notify RABM about Normal Mode if the UE is operating 
    ** in PS domain.
    ** --------------------------------------------------------*/
    tc_notify_rabm(UE_NORMAL_MODE); 

    /* ----------------------------------------------
    ** Notify CM about UE mode change to NORMAL mode
    ** ----------------------------------------------*/
	tc_cm_notify_normal_mode(); 
    
}
#endif /* FEATURE_TC */
