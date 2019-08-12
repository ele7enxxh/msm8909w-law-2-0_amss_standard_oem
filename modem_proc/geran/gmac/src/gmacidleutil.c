/*****************************************************************************
***
*** TITLE
***
***  GPRS MAC IDLE MODE MODULE UTILITY FUNCTIONS
***
***
*** DESCRIPTION
***
***  This file contains IDLE MODE utiliy functions used by IDLE MODE
***  signal handle fucntion in gmacidle.c
***
*** EXTERNALIZED FUNCTIONS
***
***
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gmac/src/gmacidleutil.c#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 24/07/01   tlx     Created
***
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h"

#include "stdlib.h"

#include "geran_msgs.h"
#include "string.h"

#include "gmac.h"
#include "gmacdef.h"
#include "gmactransfer_if.h"
#include "gmacidle.h"
#include "gmacrb.h"
#include "gmacmac.h"
#include "gmacproc.h"
#include "gmacutil.h"
#include "gmaclog.h"
#include "gmacds.h"
#include "stringl/stringl.h"

#if defined(TEST_FRAMEWORK)
#error code not present
#endif // TEST_FRAMEWORK

/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/
/*For EGPRS logging, as they are used in 4 different functions in this module.*/
#define EGPRS_TBF                 1
#define BEP_PERIOD2_NOT_SPECIFIED 0xFF

#define HYPER_FRAME 2715648

/*****************************************************************************
***
***     Private Typedefs
***
*****************************************************************************/
typedef struct
{
  uint16 log_BEP_prd2;
} gmac_idleutil_data_t;


/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/
gmac_idleutil_data_t gmac_idleutil_data[NUM_GERAN_DATA_SPACES];

/*****************************************************************************
***
***     Private Function Declarations
***
*****************************************************************************/
static pua_data_error_t gmac_tbf_starting_ie(start_frame_number_t *start_time_ptr, uint32  *des_signal_data_ptr, gas_id_t gas_id);
static boolean  gmac_1phase_contention_res(gas_id_t gas_id);
static pua_data_error_t pua_dynamic_handler(mr_ul_alloc_cnf_t *mr_ul_alloc_cnf_sig_ptr, gas_id_t gas_id );
static pua_data_error_t pua_sb_handler(gprs_mac_action_t current_mac_action, gas_id_t gas_id );
static void gmac_include_tlli_in_rlc_block(access_type_e acc_granted, mr_ul_alloc_cnf_t *mr_ul_alloc_ptr, gas_id_t gas_id);
static boolean gmac_pua_tfi_info(mr_ul_alloc_cnf_t *mr_ul_alloc_ptr, gas_id_t gas_id);
static void gmac_ul_rlc_tbf_type(access_type_e pua_access_granted, mr_ul_alloc_cnf_t *mr_ul_alloc_ptr, gas_id_t gas_id);
static void gmac_pua_rlc_nts( mr_ul_alloc_cnf_t *mr_ul_alloc_ptr, gas_id_t gas_id);
static void gmac_pua_dl_pwr_control(void *ml_sig_ptr, gas_id_t gas_id);
static pua_data_error_t pua_mb_handler(gprs_mac_action_t current_mac_action, gas_id_t gas_id );

/*****************************************************************************
***
***     Perl UTF Data Declarations
***
*****************************************************************************/
#ifdef PERLUTF
  #error code not present
#endif /*PERLUTF*/


/*****************************************************************************
***
***     Functions
***
*****************************************************************************/
/*===========================================================================
===
===  FUNCTION    gmac_get_idleutil_data_ptr()
===
===  DESCRIPTION
===  Returns a pointer to idleutil data space using the current gas_id
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===    gmac_idleutil_data_t*
=== 
===  SIDE EFFECTS
===
===
===========================================================================*/
static gmac_idleutil_data_t* gmac_get_idleutil_data_ptr(gas_id_t gas_id)
{
  gmac_idleutil_data_t *idleutil_data_ptr = &gmac_idleutil_data[0];

  /* validate the GAS ID */
  if (gas_id <= GERAN_ACCESS_STRATUM_ID_MAX)
  {
    idleutil_data_ptr = &gmac_idleutil_data[GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gas_id)];
  }
  else
  {    
    MSG_GERAN_ERROR_1_G("Idle Util bad gas_id:%d",gas_id);
  }
  return idleutil_data_ptr;
}

/*===========================================================================
===
===  FUNCTION    gmac_init_idle_util_data()
===
===  DESCRIPTION
===  Initialises the idle util data, used for lower layer resets.
===
===  PARAMS
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_init_idle_util_data( gas_id_t gas_id )
{
  gmac_idleutil_data_t *global_idleutil_ptr = gmac_get_idleutil_data_ptr(gas_id);

  global_idleutil_ptr->log_BEP_prd2 = 0;
}


/*===========================================================================
===
===  FUNCTION      gmac_tbf_starting_ie()
===
===  DESCRIPTION
===
===  ref 4.60 (chapter 12.21)
===  ref 4.08 chapter "10.5.2.38 Starting Time" for absolute frame number
===  starting time calculations
===
===  DEPENDENCIES
===
===  determines the starting time and loads the value into the signal to be sent.
===  ie mac_l1_dynamic_tbf_config or mac_l1_fixed_tbf_config
===
===  RETURN VALUE
===
===  returns  NO ERRORS  if staring time is error free
===       or  TBF_STARTING_TIME_ERROR_ACCESS_RETRY for erroneousness  conditions.
===
===  SIDE EFFECTS
===
===========================================================================*/

static pua_data_error_t gmac_tbf_starting_ie(start_frame_number_t *start_time_ptr, uint32  *des_signal_data_ptr, gas_id_t gas_id)
{
  pua_data_error_t return_value = NO_ERRORS;
  uint32 n =0;
  uint16 k =0;
  int8 t1;
  int8 t2;
  int8 t3;
  int32 starting_time_fn = 0;    /*********************************************
                                  * starting time received in PUA, represented *
                                  * as mod 42432 of the absolute fn            *
                                  *********************************************/

  int32 reception_fn_mod_42432;  /*********************************************
                                  * the absolute fn PUA was received, covered  *
                                  * to a mod 42432 format                      *
                                  *********************************************/
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  /**********************************************************************
  * note the absoulte frame number recived in pkt request ref is a mod  *
  * 42432 repesentaion and is code as 4.08 10.5.2.38  "Starting Time"   *
  **********************************************************************/
  if(start_time_ptr->abs_rel_flag == absolute_valid)
  {
     t1 = (start_time_ptr->absolute) >> 11 ;
     t3 = (((start_time_ptr->absolute) & 0x07e0) >> 5);
     t2 = ((start_time_ptr->absolute) & 0x001f) ;

     if((t3 - t2) < 0 )
     {
        starting_time_fn =( 51* ((t3 - t2) + 26) ) + ( t3 + (51*26*t1));
     }
     else
     {
        starting_time_fn =( 51* ((t3 - t2) % 26) ) + ( t3 + (51*26*t1));
     }

     /**********************************************************************
     * frame number the dl msg received on is the absolute frame number FN *
     * starting time frame number is in represented as (FN )mod 42432      *
     **********************************************************************/
     reception_fn_mod_42432 = ((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.frame_no) % 42432);

     if( (starting_time_fn - reception_fn_mod_42432) < 0 )
     {
       if( (((starting_time_fn - reception_fn_mod_42432) + 42432) >= 0)
           && (((starting_time_fn - reception_fn_mod_42432) + 42432) <= 31623))
       {
           /*              starting time has NOT elapsed                 */
           *des_signal_data_ptr =  (uint32)((starting_time_fn - reception_fn_mod_42432) + 42432)
                                    + (global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.frame_no) ;
       }
       else if( (((starting_time_fn - reception_fn_mod_42432) + 42432) >= 32024)
           && (((starting_time_fn - reception_fn_mod_42432) + 42432) <= 42431))
       {
           /*              starting time has elapsed                    */
           return_value = TBF_STARTING_TIME_ERROR_ACCESS_RETRY;
           MSG_GERAN_LOW_0_G("ERROR :- TBF starting time already occurred");
       }
     }
     else
     {
       if( ((starting_time_fn - reception_fn_mod_42432) % 42432) <= 31623 )
       {
           /*              starting time has NOT elapsed                 */
           *des_signal_data_ptr = ((uint32)(starting_time_fn - reception_fn_mod_42432) % 42432)
                                    + (global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.frame_no) ;
       }
       else if( (((starting_time_fn - reception_fn_mod_42432) % 42432) >= 32024)
            && (((starting_time_fn - reception_fn_mod_42432) % 42432) <= 42431))
       {
           /*              starting time has elapsed                    */
           return_value = TBF_STARTING_TIME_ERROR_ACCESS_RETRY;
           MSG_GERAN_LOW_0_G("ERROR :- TBF starting time already occurred  ");
       }
     }
  }
  else
  {
     /* relative fn encoding*/
     /* note if the starting time corresponds to an idle frame l1 */
     /* adds 1 to the staring time                                */
     n = global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.frame_no;
     k = start_time_ptr->relative;

     if( k != 0 )
     {
       if ( (( k % 3) == 0) || ((k % 3) == 1 ) )
       {
           *des_signal_data_ptr = ((n + 4 + 4*k + k/3)  % HYPER_FRAME);
       }
       else
       {
           *des_signal_data_ptr = ((n + 5 + 4*k + k/3)  % HYPER_FRAME);
       }
     }
     else
     {
       return_value = TBF_STARTING_TIME_ERROR_ACCESS_RETRY;
       MSG_GERAN_LOW_0_G("ERROR :- in TBF starting time ");
     }
  }

  return(return_value);
}

/*===========================================================================
===
===  FUNCTION      gmac_idle_puan_handler()
===
===  DESCRIPTION
===
===  Evaluates the data received in PUAN msg
===
===  DEPENDENCIES
===
===   ptr_csn_decode_dl_msg - global pointer to decoded CSN.1 data
===   gmac_local_tfi_store
===
===  RETURN VALUE
===
===   TRUE if contention resolution is successful
===   FALSE if it is not
===
===  SIDE EFFECTS
===
===========================================================================*/
cont_res_success_t gmac_idle_puan_handler(gas_id_t gas_id)
{
   /*csn decoded dl msg */
   packet_uplink_ack_nack_t *puan_data_ptr;

   mac_rlc_msg_t            *mr_sig_ptr;

   boolean fai_set = FALSE;

   gmac_idle_data_t *idle_data_ptr = gmac_get_idle_data_ptr(gas_id);

   /*mac_l1_power_control*/
   mac_l1_pwr_ctrl_t *pwr_ctrl_msg_ptr = &(idle_data_ptr->mac_l1_sig.mac_l1_power_control);

   cont_res_success_t contention_resolved = CON_RES_RETRY;

   gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
   

   puan_data_ptr = (packet_uplink_ack_nack_t *)global_data_ptr->csn_dec_data_ptr;

    /**********************         set up FAI         *********************************/

    if((puan_data_ptr->egprs_puan == not_valid) && (global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS))
    {
      MSG_GERAN_MED_0_G("MAC info PUAN rcvd in EGPRS mode containing GPRS IE only. Ignore ");
      return(IGNORE_PUAN);
    }

    if((puan_data_ptr->egprs_puan == valid) && (global_data_ptr->current_tbf_mode == TBF_MODE_GPRS))
    {
      MSG_GERAN_MED_0_G("MAC info PUAN rcvd in GPRS mode containing EGPRS IE only. Ignore ");
      return(IGNORE_PUAN);
    }

   fai_set = gmac_egprs_fai_set(puan_data_ptr,gas_id);

   if(gmac_mac_control_header_tfi_checking(gas_id) == MAC_HEADER_TFI_MATCH_NOT_FOUND)
   {
      /*MAC control header TFI value is invalid ignore PUAN*/
      contention_resolved = IGNORE_PUAN;
   }
   else if(puan_data_ptr->uplink_tfi != global_data_ptr->gmac_local_tfi_store.ul_tfi)
   {
      /*Ignore PUAN if incorrectly addressed by the UL TFI don't fail contention res*/
      contention_resolved = IGNORE_PUAN;
      MSG_GERAN_MED_2_G("MAC Rcvd PUAN add err. puan tfi %d,ass tfi %d ",
                        puan_data_ptr->uplink_tfi,global_data_ptr->gmac_local_tfi_store.ul_tfi);
   }
   else
   {

       MSG_GERAN_MED_3_G("MAC Rcvd PUAN FAI %d, S/P %d UL TFI %d",
                         fai_set,
                         ((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[0] & 0x08) >> 3),
                         global_data_ptr->gmac_local_tfi_store.ul_tfi);

     /*check for fai and s/p bit*/
      if ( (fai_set == TRUE) &&
         ( (global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[0] & 0x08) == 0))
      {
         /*when s/p bit not set and fai is ignore PUAN */
         /*as PCA can't be sent under thses conditions */
         contention_resolved = IGNORE_PUAN;
      }
      else
      {
        if( gmac_1phase_contention_res(gas_id) == TRUE)
        {
          gmac_stop_t3166(gas_id);

          /*   reset access_attempts_count   */
          idle_data_ptr->access_attempts_count = 0;

          /**************************************************************************************
          *               load and send MAC_L1_CONTENTION_RES to l1                             *
          **************************************************************************************/
          MSG_GERAN_MED_0_G("MAC Sent MAC_L1_CONTENTION_RES");
          idle_data_ptr->mac_l1_sig.mac_l1_contention_res.message_header.message_id = (uint8)MAC_L1_CONTENTION_RES;
          idle_data_ptr->mac_l1_sig.mac_l1_contention_res.message_header.message_set = MS_MAC_L1;
          gprs_l1_mac_acc_func(&idle_data_ptr->mac_l1_sig, gas_id);

          /**************************************************************************************
          *               load and send mr_ul_contention_ind to rlc                             *
          *               this time with packet uplink acknack info                             *
          **************************************************************************************/
          mr_sig_ptr = grlc_get_cmd_buf_mac();
          if (mr_sig_ptr == NULL)
          {
            ERR_GERAN_FATAL_0_G("MAC ERROR failed to allocated message memory");
          }
          memset(mr_sig_ptr, 0, sizeof(mac_rlc_msg_t));

          mr_sig_ptr->msg.ul_tlli_ind.tfi = puan_data_ptr->uplink_tfi;

/*          mr_sig_ptr->msg.ul_tlli_ind.cs = puan_data_ptr->chan_coding_cmd;*/
/*          gmac_local_cs_setting = puan_data_ptr->chan_coding_cmd;*/

          mr_sig_ptr->msg.ul_tlli_ind.sp = global_data_ptr->gmac_local_rrbp_valid;

          if (puan_data_ptr->pkt_ext_ta_flag == valid )
          {
            global_data_ptr->tbf_est_allowed_flag = TRUE;
          }
          else
          {
            global_data_ptr->tbf_est_allowed_flag = FALSE;
          }

          if(global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS)
          {
             /*fix for cs change handling*/
             /*update store of current CS*/
             global_data_ptr->gmac_local_cs_setting = puan_data_ptr->egprs_chan_coding_cmd;

             mr_sig_ptr->msg.ul_tlli_ind.ack_nack_info_flg = TRUE;

             /*load MR_UL_CONTENTION_IND with EGPRS PUAN data for RLC*/
             mr_sig_ptr->msg.ul_tlli_ind.mcs = puan_data_ptr->egprs_chan_coding_cmd;
             mr_sig_ptr->msg.ul_tlli_ind.resegment = puan_data_ptr->resegment ;
             mr_sig_ptr->msg.ul_tlli_ind.pre_emptive_tx = puan_data_ptr->pre_emptive_tran ;

             /*load MR_UL_CONTENTION_IND the EGPRS with Ack/Nack Description IE*/
             mr_sig_ptr->msg.ul_tlli_ind.egprs_acknack_desc = puan_data_ptr->egprs_ack_nack_des;


          }
          else if(global_data_ptr->current_tbf_mode == TBF_MODE_GPRS)
          {
             /*update store of current CS*/
             global_data_ptr->gmac_local_cs_setting = puan_data_ptr->chan_coding_cmd;

             mr_sig_ptr->msg.ul_tlli_ind.cs = puan_data_ptr->chan_coding_cmd;
             /* load mr_ul_pkt_ack/nack_ind with Ack/Nack Description IE */
             gmac_read_acknack_desc_ie(puan_data_ptr->ack_nack_desc,&(mr_sig_ptr->msg.ul_tlli_ind.acknack_desc),gas_id);
          }


          /* send MR_UL_CONTENTION_IND message (containing ack/nack info) to RLC */
          MSG_GERAN_MED_0_G( "MAC sent MR_UL_CONTENTION_IND ");
          mr_sig_ptr->msg.ul_tlli_ind.sig = MR_UL_CONTENTION_IND;
          mr_sig_ptr->msg.ul_tlli_ind.contention_res = RM_CONTENTION_RESOLVED;
          gmac_rlc_mac_insert_gas_id(mr_sig_ptr,gas_id);
          grlc_put_cmd_mac(gas_id,mr_sig_ptr);

          /**************************************************************************************
          *  check puan for any l1 configrable infromation eg Packet Timing Advance Parameters  *
          *  Power Control Parameters                                                           *
          ***************************************************************************************/
          memset(pwr_ctrl_msg_ptr,0,sizeof(mac_l1_pwr_ctrl_t));

          /*set the defaults to not present*/
          pwr_ctrl_msg_ptr->global_packet_ta_present = FALSE;
          pwr_ctrl_msg_ptr->power_control.present = FALSE;

          if((puan_data_ptr->pkt_timing_adv_flag == valid)
            || puan_data_ptr->pwr_ctrl_flag == valid)
          {

            if(gmac_update_local_ta_params(&(puan_data_ptr->pkt_timing_adv),gas_id) == TRUE)
            {

              /* TA Parameters, send to L1 if there is a change */
              if ((puan_data_ptr->pkt_timing_adv_flag == valid) &&
                  (gmac_util_handle_power_ctrl_TA_IE(puan_data_ptr->pkt_timing_adv,&(pwr_ctrl_msg_ptr->global_packet_ta),gas_id)))
              {
                pwr_ctrl_msg_ptr->global_packet_ta_present = TRUE;
              }

            }

            if(puan_data_ptr->pwr_ctrl_flag == valid)
            {
              /* valid data received in power_control_param_ie and load mac_l1_power_control signal data */
              gmac_power_control_params_ie(&(puan_data_ptr->pwr_ctrl_paramtrs), &(pwr_ctrl_msg_ptr->power_control),gas_id);
              pwr_ctrl_msg_ptr->power_control.present = TRUE;
            }

            /*When PUAN has FAI set,Ignore sending of MAC_L1_POWER_CONTROL sig if the UL is */
            /*going to be released as it is no longer relevant                              */
            if(global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS)
            {
              /*When PUAN has FAI set,Ignore sending of MAC_L1_POWER_CONTROL sig if the UL is */
              /*going to be released as it is no longer relevant                              */
              if(fai_set == FALSE )
              {
                /*send mac_l1_power_control  */
                MSG_GERAN_MED_0_G("MAC Sent MAC_L1_POWER_CONTROL");
                idle_data_ptr->mac_l1_sig.mac_l1_power_control.message_header.message_set = MS_MAC_L1;
                idle_data_ptr->mac_l1_sig.mac_l1_power_control.message_header.message_id = (uint8)MAC_L1_POWER_CONTROL;
                gprs_l1_mac_acc_func(&idle_data_ptr->mac_l1_sig,gas_id);
              }
            }
            else if(global_data_ptr->current_tbf_mode == TBF_MODE_GPRS)
            {
              if(puan_data_ptr->ack_nack_desc.final_ack_ind == 0 )
              {
                /*send mac_l1_power_control  */
                MSG_GERAN_MED_0_G("MAC Sent MAC_L1_POWER_CONTROL");
                idle_data_ptr->mac_l1_sig.mac_l1_power_control.message_header.message_set = MS_MAC_L1;
                idle_data_ptr->mac_l1_sig.mac_l1_power_control.message_header.message_id = (uint8)MAC_L1_POWER_CONTROL;
                gprs_l1_mac_acc_func(&idle_data_ptr->mac_l1_sig,gas_id);
              }
            }

          }

          if(puan_data_ptr->fixed_alloc_flag == valid)
          {
            MSG_GERAN_ERROR_0_G( "Unexpected Reallocation during contention res");
          }

          if (puan_data_ptr->r99_additions_flag == valid)
          {
            if(puan_data_ptr->pkt_ext_ta_flag)
            {
              MSG_GERAN_ERROR_0_G( "Unexpected R99 additions during contention res");
            }
            if(puan_data_ptr->tbf_est !=0)
            {
              MSG_GERAN_ERROR_0_G( "Unexpected tbf_est during contention res");
            }
          }

          /*note the extended bit IE in puan is ignored */

          /* check for FAI bit in puan, if set then release ul tbf      */
          /* data transfer successfully completed within the contention */
          /* resolution process ref 4.60, 12.3                          */
          if(global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS)
          {
            if(fai_set == TRUE )
            {
               contention_resolved = CON_RES_RELEASE;
            }
            else
            {
               contention_resolved = CON_RES_TRANSFER;
            }
          }
          else if(global_data_ptr->current_tbf_mode == TBF_MODE_GPRS)
          {
            if(puan_data_ptr->ack_nack_desc.final_ack_ind == 1 )
            {
               contention_resolved = CON_RES_RELEASE;
            }
            else
            {
               contention_resolved = CON_RES_TRANSFER;
            }
          }
        }
        else
        {
          /***********************************************************************************
          * Error, contention resolution failed. load and send mr_ul_contention_ind to rlc   *                            *
          * to rlc, rlc resets n3104 and stops sending rlc data blocks mac perform access    *
          * mac perform access retry if applicable                                   *
          ***********************************************************************************/

          if(puan_data_ptr->contention_res_flag == not_valid)
          {
            contention_resolved = IGNORE_PUAN;
          }
        }
      }


      /************************************************************************
      *  RAC info RETRANSMISSION. PUAN addressed correctly by TFI and carries *
      *  an indication to retrans RAC info. Always perform request even if    *
      *  there is NO CONTENTION  RESOLUTION_TLLI field in PUAN                *                         *
      ************************************************************************/

      if((puan_data_ptr->egprs_puan == valid) && (global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS))
      {
        if((puan_data_ptr->prr_retran_req == 1) && (global_data_ptr->c_flags.egprs_prr_sent == TRUE))
        {
          MSG_GERAN_HIGH_0_G("MAC info PUAN request for PRR retrans");
          gmac_retran_prr_msg(gas_id);
        }
        else
        {
          MSG_GERAN_MED_2_G("MAC info arac_retran_req %d, egprs_arac_sent %d",
                            puan_data_ptr->prr_retran_req,global_data_ptr->c_flags.egprs_prr_sent);
        }

        if((puan_data_ptr->arac_retran_req == 1) && (global_data_ptr->c_flags.egprs_arac_sent == TRUE))
        {
          MSG_GERAN_HIGH_0_G("MAC info PUAN request for ARAC retrans");
          gmac_retran_arac_msg(gas_id);
        }
        else
        {
          MSG_GERAN_MED_2_G("MAC info arac_retran_req %d, egprs_arac_sent %d",
                            puan_data_ptr->arac_retran_req,global_data_ptr->c_flags.egprs_arac_sent);
        }
      }
   }
   return (contention_resolved);
}

/*===========================================================================
===
===  FUNCTION      gmac_1phase_contention_res()
===
===  DESCRIPTION
===
===  checks for address errors in csn1
===  checks for tfi miss match
===  checks for tlli miss match
===
===  DEPENDENCIES
===
===   ptr_csn_decode_dl_msg - global pointer to decoded CSN.1 data
===
===  RETURN VALUE
===
===   TRUE if contention resolution is successful
===   FALSE if it is not
===
===  SIDE EFFECTS
===
===========================================================================*/

static boolean  gmac_1phase_contention_res(gas_id_t gas_id)
{
   gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
   /*csn decoded dl msg */
   packet_uplink_ack_nack_t *puan_data_ptr = (packet_uplink_ack_nack_t *)global_data_ptr->csn_dec_data_ptr;

   boolean contention_resolved = FALSE;


   mac_header_tfi_t mac_header_tfi_check = MAC_HEADER_TFI_NOT_VALID;

   if (global_data_ptr->csn_dec_hdr_ptr->error_code != ADDRESS_INFO_ERROR)
   {
      mac_header_tfi_check = gmac_mac_control_header_tfi_checking(gas_id);
      if(mac_header_tfi_check == MAC_HEADER_TFI_NOT_VALID)
      {
        if(global_data_ptr->gmac_local_tfi_store.ul_tfi == puan_data_ptr->uplink_tfi  )
        {
          /* check tlli recived in puan*/
          if(puan_data_ptr->contention_res_flag == valid)
          {
            if(global_data_ptr->rlc_ul_est_tlli == puan_data_ptr->contention_res_tlli)
            {
               contention_resolved = TRUE;
               global_data_ptr->rlc_ul_est_tlli = 0xFFFFFFFFUL;
            }
            else
            {
               contention_resolved = FALSE;
            }
          }
          else
          {
            /****************************************************************************
            * indicate that con res failed, when returning to to PUA_handler() re-check *
            * if NO CONTENTION_RESOLUTION_TLLI field in PUAN then convert con res failed*
            * to "IGNORE_PUAN" and continue cont res untill a PUAN is rcvd with         *
            * CONTENTION_RESOLUTION_TLLI field                                          *
            ****************************************************************************/

            MSG_GERAN_HIGH_0_G("MAC Rcvd PUAN without contention res TLLI field msg ignored");
          }
        }
        else
        {
          MSG_GERAN_MED_2_G("MAC PUAN add err PUAN tfi =%d, assigned TFI =%d",
                            puan_data_ptr->uplink_tfi,global_data_ptr->gmac_local_tfi_store.ul_tfi);
        }
      }
      else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_FOUND)
      {
        contention_resolved = TRUE;
      }
      else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_NOT_FOUND)
      {
        contention_resolved = FALSE;
      }
   }
   else
   {
     MSG_GERAN_ERROR_0_G("MAC Error csn.1 address error");
   }

   return(contention_resolved) ;
}

/*===========================================================================
===
===  FUNCTION      gmac_idle_pda_handler()
===
===  DESCRIPTION
===
===  Handles the PDA msg when received  in mac idle mode
===
===  DEPENDENCIES
===
===  the globally store csn.1 decoded data
===
===  RETURN VALUE
===
===    An error value that indicates what action should be taken if the
===    data received in the PDA is erroneous
===
===  SIDE EFFECTS
===
===========================================================================*/
pua_data_error_t gmac_idle_pda_handler(mr_dl_ass_ind_t *dl_ass_ind_sig_ptr,
                                       boolean send_frq_config,
                                       pda_address_check_t address_check,
                                       gas_id_t gas_id )
{
    /**************************************************************
    *                  Received signal info                       *
    ***************************************************************/

    /*    csn.1 decoded pda msg              */
    packet_downlink_assignment_t *pda_data_ptr;

    /**************************************************************
    *                 Global send signal info                     *
    **************************************************************/

    /*    mac_l1_dl_tbf_config sig. Signal data stored globally   */
    mac_l1_dl_tbf_config_t *mac_l1_dl_tbf_config_ptr;

    /**************************************************************
    *                local vars                                   *
    **************************************************************/

    /*    result of checking pda                                 */
    pua_data_error_t  pda_error = NO_ERRORS;

    uint32 local_rrps_tlli = 0 ;

    uint32 old_local_rrps_tlli = 0;

    mac_header_tfi_t mac_header_tfi_check = MAC_HEADER_TFI_NOT_VALID;

    uint16 assigned_dl_ws =0;

    pua_data_error_t freq_check_result = NO_ERRORS;

    gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
    gmac_idle_data_t *idle_data_ptr = gmac_get_idle_data_ptr(gas_id);

    /**************************************************************
    *              Assign received  signal  pointers              *
    **************************************************************/

    /*     received PDA msg                                      */
    pda_data_ptr = (packet_downlink_assignment_t *)global_data_ptr->csn_dec_data_ptr;

    /**************************************************************
    *              Assign global send signal  pointers            *
    **************************************************************/

    /*     mac_l1_dl_tbf_config (global)                          */
    mac_l1_dl_tbf_config_ptr = &(idle_data_ptr->mac_l1_sig.mac_l1_dl_tbf_config);

    memset(dl_ass_ind_sig_ptr, 0, sizeof(mr_dl_ass_ind_t));


    /************************************************************
    *           Check PDA address correctly                     *
    *    ( in passive pda will address MS by its tlli)          *
    ************************************************************/

    /*rlc must be passed the tlli current tlli value*/
    rr_read_public_store(RRPS_TLLI,&local_rrps_tlli,gas_id);
    rr_read_public_store(RRPS_TLLI_OLD,&old_local_rrps_tlli,gas_id);

    if (address_check == PDA_NO_ADDRESS_CHECK)
    {
      MSG_GERAN_HIGH_0_G("PDA address already checked");
    }
    else
    {
      if (global_data_ptr->csn_dec_hdr_ptr->error_code != ADDRESS_INFO_ERROR)
      {
        mac_header_tfi_check = gmac_mac_control_header_tfi_checking(gas_id);

        /*TLLI in DL CNF to RLC*/
        /*Check for TFI in PDA:
          1) After a PDA has already been received and MAC is waiting for L1 Release confirm of UL.
          2) After a PUAN with FAI=1 is received in 1P Cont Res and MAC is waiting for L1 release confirm of UL */
        if ((idle_data_ptr->i_flags.pda_ul_pacch == TRUE) ||
            (address_check == PDA_CHECK_TFI_AND_TLLI ))
        {
            /*PDA rcvd on UL PACCH can be addressed by TLLI or UL TFI*/
            if(mac_header_tfi_check == MAC_HEADER_TFI_NOT_VALID)
            {
                 if(pda_data_ptr->tlli_gtfi_pkt_ref_valid == global_tfi_valid)
                 {
                   /*pda rvcd while ul being released, check tlli and ul tfi*/
                   if (pda_data_ptr->global_tfi.direction == uplink_valid)
                   {
                      if ((pda_data_ptr->global_tfi.uplink_tfi != global_data_ptr->gmac_local_tfi_store.ul_tfi)
                           && (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid))
                      {
                        /*check dl_tfi matches*/
                        MSG_GERAN_MED_0_G("MAC Info  PDA, UL_TFI mismatch");
                        return(ASSIGNMENT_ADDRESSING_ERROR);
                      }
                      else
                      {
                        /*TLLI in*/
                        MSG_GERAN_HIGH_3_G("MAC gmac_store_ul_est_tlli %x, gmac_store_dl_ass_tlli %x",
                                           global_data_ptr->gmac_store_ul_est_tlli, 
                                           global_data_ptr->gmac_store_dl_ass_tlli,
                                           local_rrps_tlli);
                        global_data_ptr->gmac_store_dl_ass_tlli = global_data_ptr->gmac_store_ul_est_tlli;
                      }
                   }
                   else
                   {
                      MSG_GERAN_MED_0_G("ERROR :- PDA incorrectly address MS");
                      return(ASSIGNMENT_ADDRESSING_ERROR);
                   }
                 }
                 else  if(pda_data_ptr->tlli_gtfi_pkt_ref_valid == tlli_valid)
                 {
                    /*check tlli matches*/
                    /*TLLI in*/
                    if(pda_data_ptr->tlli != global_data_ptr->gmac_store_ul_est_tlli)
                    {
                       if(pda_data_ptr->tlli == local_rrps_tlli)
                       {
                         global_data_ptr->gmac_store_dl_ass_tlli = local_rrps_tlli;
                       }
                       else if (pda_data_ptr->tlli == old_local_rrps_tlli)
                       {
                         global_data_ptr->gmac_store_dl_ass_tlli = old_local_rrps_tlli;
                       }
                       else
                       {
                         MSG_GERAN_MED_0_G("MAC Info  PDA, TLLI mismatch");
                         return(ASSIGNMENT_ADDRESSING_ERROR);
                       }
                    }
                    else
                    {
                      MSG_GERAN_HIGH_3_G("MAC gmac_store_ul_est_tlli %x, gmac_store_dl_ass_tlli %x",
                                         global_data_ptr->gmac_store_ul_est_tlli, 
                                         global_data_ptr->gmac_store_dl_ass_tlli,
                                         local_rrps_tlli);
                      global_data_ptr->gmac_store_dl_ass_tlli = global_data_ptr->gmac_store_ul_est_tlli;
                    }
                 }
                 else
                 {
                      MSG_GERAN_MED_0_G("ERROR :- PDA incorrectly address MS");
                      return(ASSIGNMENT_ADDRESSING_ERROR);
                 }
            }
            else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_FOUND)
            {
              /*pda addressed by UL tfi */
              /*TLLI in*/
              MSG_GERAN_HIGH_3_G("MAC gmac_store_ul_est_tlli %x, gmac_store_dl_ass_tlli %x",
                                 global_data_ptr->gmac_store_ul_est_tlli, 
                                 global_data_ptr->gmac_store_dl_ass_tlli,
                                 local_rrps_tlli);
              global_data_ptr->gmac_store_dl_ass_tlli = global_data_ptr->gmac_store_ul_est_tlli;
            }
            else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_NOT_FOUND)
            {
              return(ASSIGNMENT_ADDRESSING_ERROR);
            }
        }
        else
        {
          /*PDA rcvd on PCCCH (ie no UL or DL being established)must be addressed by TLLI*/
          if(mac_header_tfi_check == MAC_HEADER_TFI_NOT_VALID)
          {
            if(pda_data_ptr->tlli_gtfi_pkt_ref_valid == tlli_valid)
            {
              if(pda_data_ptr->tlli == local_rrps_tlli)
              {
                global_data_ptr->gmac_store_dl_ass_tlli = local_rrps_tlli;
              }
              else if (pda_data_ptr->tlli == old_local_rrps_tlli)
              {
                global_data_ptr->gmac_store_dl_ass_tlli = old_local_rrps_tlli;
              }
              else
              {
                MSG_GERAN_MED_0_G("MAC Info  PDA, TLLI mismatch");
                return(ASSIGNMENT_ADDRESSING_ERROR);
              }

            }
            else
            {
                 MSG_GERAN_MED_0_G("ERROR :- PDA incorrectly address MS");
                 return(ASSIGNMENT_ADDRESSING_ERROR);
            }
          }
          else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_FOUND)
          {
            MSG_GERAN_MED_0_G("ERROR :- PDA incorrectly add by control header DL_TFI MS");
            return(ASSIGNMENT_ADDRESSING_ERROR);
          }
          else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_NOT_FOUND)
          {
            return(ASSIGNMENT_ADDRESSING_ERROR);
          }

        }
      }
      else
      {
        MSG_GERAN_ERROR_0_G("MAC Error csn.1 address error");
        return(ASSIGNMENT_DATA_ERROR);
      }
    }


    /******************************************************************
    *                 Check validity of PDA data                      *
    ******************************************************************/
    if (global_data_ptr->csn_dec_hdr_ptr->error_code != MESSAGE_ESCAPE_ERROR)
    {
        /*****************************************************************
        *             format MAC/RLC sig  MR_DL_ASS_IND                  *
        *****************************************************************/
        /*TLLI in DL CNF to RLC*/
        MSG_GERAN_HIGH_3_G("MAC gmac_store_ul_est_tlli %x, gmac_store_dl_ass_tlli %x",
                           global_data_ptr->gmac_store_ul_est_tlli, 
                           global_data_ptr->gmac_store_dl_ass_tlli,
                           local_rrps_tlli);

        if (pda_data_ptr->timeslot_alloc == 0)
        {
          MSG_GERAN_HIGH_0_G("GMAC PDA does not contain a timeslot allocation, ASSIGNMENT_DATA_ERROR");
          return(ASSIGNMENT_DATA_ERROR);
        }
        global_data_ptr->gmac_store_dl_rlc_mode = pda_data_ptr->rlc_mode;

        dl_ass_ind_sig_ptr->rlc_mode = pda_data_ptr->rlc_mode;

        /*TLLI in DL CNF to RLC*/
        dl_ass_ind_sig_ptr->tlli = global_data_ptr->gmac_store_dl_ass_tlli;

        dl_ass_ind_sig_ptr->num_dl_ts = gmac_utils_calculate_nts(global_data_ptr->gmac_local_dl_ts_alloc,gas_id);

        /*TLLI in DL CNF to RLC*/
        MSG_GERAN_HIGH_3_G("MAC loaded MR_DL_ASS_IND with tlli %x,L_tlli %x o_tlli %x",
                           dl_ass_ind_sig_ptr->tlli,local_rrps_tlli,old_local_rrps_tlli);
        MSG_GERAN_HIGH_1_G("MAC gmac_store_dl_ass_tlli  %x",global_data_ptr->gmac_store_dl_ass_tlli);

        /****************************************************************
        *             format MAC/L1 sig  MAC_L1_DL_TBF_CONFIG           *
        *****************************************************************/
#ifdef FEATURE_GSM_DTM
        if ( (pda_data_ptr->mac_mode == 0) ||
             (pda_data_ptr->mac_mode == 1) )
#else
        if (pda_data_ptr->mac_mode == 0)
#endif /*FEATURE_GSM_DTM*/
        {
           /* Ignore MAC_MODE in PDA for Rel 6, always dynamic in DL transfer states */
           mac_l1_dl_tbf_config_ptr->mac_mode = MAC_L1_MAC_MODE_DYNAMIC;

           /* store current mac mode for multislot checking */
           global_data_ptr->gmac_current_mac_mode = MAC_L1_MAC_MODE_DYNAMIC;
        }
        else
        {
           MSG_GERAN_LOW_1_G("MAC info DL MAC MODE assigned mac_mode = %d",pda_data_ptr->mac_mode);
           return(ASSIGNMENT_DATA_ERROR);
        }

        /*****************************************************************
        *                          dl tfi                                *
        *****************************************************************/
        if(pda_data_ptr->dwnlk_tfi_flag == valid)
        {
          /*send to l1*/
          mac_l1_dl_tbf_config_ptr->dl_tfi_present = TRUE;
          mac_l1_dl_tbf_config_ptr->dl_tfi = pda_data_ptr->downlink_tfi;

          /*mac stores dl tfi */
          global_data_ptr->gmac_local_tfi_store.dl_tfi_valid = TRUE;
          global_data_ptr->gmac_local_tfi_store.dl_tfi = pda_data_ptr->downlink_tfi;
        }
        else
        {
           MSG_GERAN_ERROR_0_G("MAC error no DL TFI");
           return(ASSIGNMENT_DATA_ERROR);
        }
        /*event logging changes*/
        mac_log_gprs_pda_event( pda_data_ptr->timeslot_alloc, global_data_ptr->gmac_local_tfi_store.dl_tfi,
                                gas_id );

        /******************************************************************
        *            Set global TBF mode                                  *
        ******************************************************************/
        if(pda_data_ptr->egprs_options_flag == valid)
        {
          global_data_ptr->current_tbf_mode = TBF_MODE_EGPRS;
        }
        else
        {
          global_data_ptr->current_tbf_mode = TBF_MODE_GPRS;
        }

        /******************************************************************
        *            check multislot compatiblity (only rx relevent)      *
        *            multislot class 12 , max 4 rx ts                     *
        ******************************************************************/

        global_data_ptr->gmac_dtm_cs_ts_alloc = GMAC_NO_TS_ALLOCATED_IN_BITMAP;
#ifdef FEATURE_GPRS_PS_HANDOVER
        if ( gmac_util_multislot_class_check(
                                GMAC_NO_TS_ALLOCATED_IN_BITMAP,
                                pda_data_ptr->timeslot_alloc,
                                MAC_L1_MAC_MODE_DYNAMIC,
                                global_data_ptr->gmac_dtm_cs_ts_alloc,
                                FALSE, PSHO_UPDATE_PARAMS_NOT_REQUIRED,
                                gas_id ) == FALSE )
#else
        if ( gmac_util_multislot_class_check(
                                GMAC_NO_TS_ALLOCATED_IN_BITMAP,
                                pda_data_ptr->timeslot_alloc,
                                MAC_L1_MAC_MODE_DYNAMIC,
                                global_data_ptr->gmac_dtm_cs_ts_alloc,
                                FALSE, gas_id ) == FALSE )
#endif /*FEATURE_GPRS_PS_HANDOVER*/
        {
          /*ref 4.60 7.2.2 abnormal case */
          MSG_GERAN_LOW_0_G("MAC info PDA Multislot error ");
          return(MULTISLOT_ERROR_ACCESS_RETRY);
        }
        else
        {
          mac_l1_dl_tbf_config_ptr->ts_allocation = pda_data_ptr->timeslot_alloc;
        }

        /******************************************************************
        *              check tbf starting time is valid                   *
        ******************************************************************/
        if(pda_data_ptr->tbf_start_flag == valid )
        {
           if (gmac_tbf_starting_ie(&(pda_data_ptr->tbf_start_frm_num),
                                    &(mac_l1_dl_tbf_config_ptr->tbf_start_time),
                                     gas_id)
                  == TBF_STARTING_TIME_ERROR_ACCESS_RETRY)
           {
             /* no tbf starting time or starting time has occurred, dl assignment still goes to L1*/
             /* network MAY send pkt polling request ..see ref 4.60 , 7.2.1.3*/
             MSG_GERAN_LOW_0_G("MAC info PDA TBF stating time already occurred, wait for pkt polling request ");
           }
           else
           {
              mac_l1_dl_tbf_config_ptr->tbf_start_time_valid = TRUE;
           }
        }
        else
        {
            mac_l1_dl_tbf_config_ptr->tbf_start_time_valid = FALSE;
        }

        /******************************************************************
        *                    power control params                         *
        ******************************************************************/
        /*cr71042*/
        if(pda_data_ptr->pwr_ctrl_flag == valid )
        {
          mac_l1_dl_tbf_config_ptr->power_control.present = TRUE;

          /*load power control params into MAC_L1_DL_TBF_CONFIG*/
          gmac_power_control_params_ie(&(pda_data_ptr->pwr_ctrl_paramtrs),
                                       &(mac_l1_dl_tbf_config_ptr->power_control),
                                        gas_id);
        }
        else
        {
          mac_l1_dl_tbf_config_ptr->power_control.present = FALSE;
        }

        /******************************************************************
        *               downlink power control params                     *
        ******************************************************************/
        if(pda_data_ptr->dwnlk_pwr_ctrl_flag == pwr_ctrl_valid)
        {
          mac_l1_dl_tbf_config_ptr->downlink_pwr.params_valid = TRUE;
          mac_l1_dl_tbf_config_ptr->downlink_pwr.po =  pda_data_ptr->dwnlk_pwr_ctrl.po;
          mac_l1_dl_tbf_config_ptr->downlink_pwr.bts_pwr_ctrl_mode = pda_data_ptr->dwnlk_pwr_ctrl.bts_pwr_ctrl_mode;
          mac_l1_dl_tbf_config_ptr->downlink_pwr.pr_mode =  pda_data_ptr->dwnlk_pwr_ctrl.pr_mode;
        }
        else
        {
          mac_l1_dl_tbf_config_ptr->downlink_pwr.params_valid = FALSE;
        }

        /******************************************************************
        *                           check  TA info                        *
        ******************************************************************/
        /*load TA params into MAC_L1_DL_TBF_CONFIG*/
        (void)gmac_packet_ta_ie( &(pda_data_ptr->pkt_timing_adv),
                                 &(mac_l1_dl_tbf_config_ptr->ta_params),
                                   gas_id );

        /*update local "gmac_update_local_ta_params" with ta info */
        (void)gmac_update_local_ta_params(&(pda_data_ptr->pkt_timing_adv),
                                            gas_id);


        /* Measurement Mapping */
        /* WARNING : Not applicable to this release and is ignored */

        /* R99 extension : EGPRS, Extended TA, COMPACT */
        /* WARNING : Not applicable to this release and is ignored */

        global_data_ptr->current_rcvd_dl_ctrl_ack = pda_data_ptr->ctrl_ack;
        dl_ass_ind_sig_ptr->tbf_mode = global_data_ptr->current_rcvd_dl_ctrl_ack;
        
        if(pda_data_ptr->egprs_options_flag ==  valid)
        {
          /*****************************************************************
          *             ADD EGPRS params to  MAC/RLC sig  MR_DL_ASS_IND    *
          *****************************************************************/

          dl_ass_ind_sig_ptr->tbf_mode = TBF_MODE_EGPRS;

          /*Check validity of DL ws*/
          assigned_dl_ws = gmac_egprs_decode_dl_window_size(pda_data_ptr->egprs_win_sz, pda_data_ptr->timeslot_alloc,
                                                            gas_id);
          if (assigned_dl_ws != WS_ALLOCATION_INVALID)
          {
             dl_ass_ind_sig_ptr->ws = assigned_dl_ws;

             /*save DL window size used in transfer to validate any new PDA assignments*/
             global_data_ptr->current_egprs_dl_ws = assigned_dl_ws;
          }
          else
          {
             MSG_GERAN_HIGH_2_G("MAC error in DL WS assignment ignored.DL ws bitmap 0x%x ts alloc 0x%x", 
                                pda_data_ptr->egprs_win_sz,
                                pda_data_ptr->timeslot_alloc);
             return(ASSIGNMENT_DATA_ERROR);
          }

          /****************************************************************
          *             ADD EGPRS params to  MAC_L1_DL_TBF_CONFIG         *
          *****************************************************************/
          mac_l1_dl_tbf_config_ptr->tbf_mode = TBF_MODE_EGPRS;

          mac_l1_dl_tbf_config_ptr->rlc_mode = (mac_rlc_mode_T)pda_data_ptr->rlc_mode;

          mac_l1_dl_tbf_config_ptr->link_qual_meas_mode = pda_data_ptr->egprs_link_qual;

          if (pda_data_ptr->egprs_bep_prd2_flag == valid)
          {
            mac_l1_dl_tbf_config_ptr->bep_period2.bep_period2 = pda_data_ptr->egprs_bep_prd2;
            mac_l1_dl_tbf_config_ptr->bep_period2.present = TRUE;
          }
          else
          {
            mac_l1_dl_tbf_config_ptr->bep_period2.present = FALSE;
          }

        }
        else
        {
          dl_ass_ind_sig_ptr->tbf_mode = TBF_MODE_GPRS;
          mac_l1_dl_tbf_config_ptr->tbf_mode = TBF_MODE_GPRS;
          mac_l1_dl_tbf_config_ptr->rlc_mode = RLC_MODE_INVALID;
          mac_l1_dl_tbf_config_ptr->link_qual_meas_mode = 0;
          mac_l1_dl_tbf_config_ptr->bep_period2.bep_period2 = 0;
          mac_l1_dl_tbf_config_ptr->bep_period2.present = FALSE;
        }

        /*****************************************************************
        *             format MAC/L1 sig  MAC_L1_FREQ_CONFIG              *
        *****************************************************************/
        if(pda_data_ptr->freq_param_flag == valid )
        {
          /*********************************************************************************
          *       determine and load "MAC_L1_FREQ_CONFIG" signal data for the  DL TBF      *
          **********************************************************************************/

            /*if no errors in frquency alloc, gmac_util_frequency_param_ie sends  */
            /*l1 MAC_L1_FREQ_CONFIG sig */
            freq_check_result = gmac_util_frequency_param_ie(FALSE,send_frq_config,gas_id);

            if(freq_check_result != NO_ERRORS)
            {
               MSG_GERAN_LOW_0_G("ERROR :- PDA. Frequency params error");
               pda_error = freq_check_result;
            }
        }
        else
        {
           if(idle_data_ptr->i_flags.pda_ul_pacch == FALSE)
           {
              /* error case there must be a frequency assignment*/
              /* ref 4.60 chapter 5.5.1.7                       */
              MSG_GERAN_HIGH_0_G("MAC info, No frequency params, Ignore PDA");
              pda_error = FREQ_ERROR_ACCESS_RETRY;
           }
        }

        /* Log DL TBF Establish 0x522A */
        if (global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS)
        {
           mac_log_dl_etbf_establish(
                                      global_data_ptr->gmac_local_tfi_store.dl_tfi,
                                      global_data_ptr->gmac_local_dl_ts_alloc,
                                      pda_data_ptr->rlc_mode,
                                      (byte) pda_data_ptr->egprs_options_flag,
                                      global_data_ptr->current_egprs_dl_ws,
                                      pda_data_ptr->egprs_link_qual,
                                      pda_data_ptr->egprs_bep_prd2,
                                      gas_id
                                    );

        }
        else
        {
          mac_log_dl_tbf_establish(
                                    global_data_ptr->gmac_local_tfi_store.dl_tfi,
                                    global_data_ptr->gmac_local_dl_ts_alloc,
                                    pda_data_ptr->rlc_mode,
                                    gas_id
                                  );

        }

    }
    else
    {
       MSG_GERAN_ERROR_0_G("MAC Error csn.1 msg error");
    }

 return(pda_error);

}

/*===========================================================================
===
===  FUNCTION    pua_dynamic_handler()
===
===  DESCRIPTION
===
===  Handles the PUA msg when received  in mac idle mode
===
===  DEPENDENCIES
===
===  global pointer to the msg data section of the csn.1 decode dl msg
===  global mac_l1_sig var used to store mac_l1_dynamic_tbf_config sig data
===  global pointer mac_rlc_sig_ptr to store mr_ul_alloc_cnf sig data
===
===  RETURN VALUE
===
===  An error value that indicates what action should be taken if the
===  data received in the PUA is erroneous
===
===  SIDE EFFECTS
===
===========================================================================*/

static pua_data_error_t pua_dynamic_handler(mr_ul_alloc_cnf_t *mr_ul_alloc_cnf_sig_ptr, gas_id_t gas_id )
{
  /*    result of checking pua            */
  pua_data_error_t  pua_result = NO_ERRORS;

  /* multislot class vars */
  uint16 tx_ts_alloc_bitmap;

  /*    csn.1 decoded pua                 */
  packet_uplink_assignment_t *pua_data_ptr;

  /*    mac_l1_dynamic_tbf_config sig       */
  mac_dynamic_tbf_information_T *ml_dynamic_tbf_info_ptr;

  /* multislot TS conversion parameters */
  uint8 ts_alloc_rx_bitmap = 0, ts_alloc_tx_bitmap = 0;
  uint8 usf_mask = 0x01;
  uint8 ts_mask = 0x80;
  uint8 i;

  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_idle_data_t *idle_data_ptr = gmac_get_idle_data_ptr(gas_id);

  /*     assign signal pointers           */
  pua_data_ptr = (packet_uplink_assignment_t *)global_data_ptr->csn_dec_data_ptr;
  ml_dynamic_tbf_info_ptr = &(idle_data_ptr->mac_l1_sig.mac_l1_dynamic_tbf_config.dynamic_tbf);

  /*check for GPRS/EGRPS tbf mode conflicts */
  if(global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS && pua_data_ptr->egprs_pua == not_valid)
  {
    MSG_GERAN_MED_0_G("MAC info ERROR PUA assigned GPRS in tbf mode EGPRS. ignore PUA");
    return (ASSIGNMENT_DATA_ERROR);
    
  }

  if(global_data_ptr->current_tbf_mode == TBF_MODE_GPRS && pua_data_ptr->egprs_pua == valid)
  {
    MSG_GERAN_MED_0_G("MAC info ERROR PUA assigned EGPRS in tbf mode GPRS. ignore PUA");
    return (ASSIGNMENT_DATA_ERROR);
    
  }

#ifdef FEATURE_GSM_DTM
  /* Do not inhibit EDA when FEATURE_GSM_DTM is featured */

#else
  if(pua_data_ptr->dynamic_alloc.ext_dynamic_alloc == GMAC_EDA_IND_EXTENDED_DYNAMIC_ALLOC)
  {
    /*perform access retry*/
    MSG_GERAN_ERROR _1_G("MAC Error extended dynamic alloc currently not supported ",0 );
    return(MULTISLOT_ERROR_ACCESS_RETRY);
  }
  else
#endif /* FEATURE_GSM_DTM */
  {
      /***************************************************************
      *       determine and load "mr_ul_alloc_cnf" signal data       *
      *                    for a DYNAMIC UL TBF                      *
      ***************************************************************/

      memset(mr_ul_alloc_cnf_sig_ptr, 0, sizeof(mr_ul_alloc_cnf_t));

      /*determine the RLC tbf access phase etiher 1 phase,2phase or short access*/
      if(idle_data_ptr->access_granted == one_phase)
      {
       mr_ul_alloc_cnf_sig_ptr->access_phase = GMAC_ONE_PHASE_ACCESS;
      }
      else if(idle_data_ptr->access_granted == two_phase)
      {
       mr_ul_alloc_cnf_sig_ptr->access_phase = GMAC_TWO_PHASE_ACCESS;
      }

      if(global_data_ptr->access_requested == short_acc)
       mr_ul_alloc_cnf_sig_ptr->access_phase = GMAC_SHORT_ACCESS;

      gmac_include_tlli_in_rlc_block(idle_data_ptr->access_granted, mr_ul_alloc_cnf_sig_ptr,gas_id);

#ifdef FEATURE_GSM_DTM
      if ( pua_data_ptr->dynamic_alloc.ext_dynamic_alloc == GMAC_EDA_IND_EXTENDED_DYNAMIC_ALLOC )
      {
        mr_ul_alloc_cnf_sig_ptr->access_mode = RM_EXT_DYNAMIC_ALLOC;
      }
      else
      {
        mr_ul_alloc_cnf_sig_ptr->access_mode = RM_DYNAMIC_ALLOC;
      }
#else
      mr_ul_alloc_cnf_sig_ptr->access_mode = RM_DYNAMIC_ALLOC;
#endif /*FEATURE_GSM_DTM*/

      /*store current mac mode for multislot checking */
      if ( pua_data_ptr->dynamic_alloc.ext_dynamic_alloc == GMAC_EDA_IND_EXTENDED_DYNAMIC_ALLOC )
      {
        global_data_ptr->gmac_current_mac_mode = GMAC_MAC_MODE_EXTENDED_DYNAMIC_ALLOC;
      }
      else
      {
        global_data_ptr->gmac_current_mac_mode = GMAC_MAC_MODE_DYNAMIC_ALLOC;
      }

      mr_ul_alloc_cnf_sig_ptr->retry = (uint8)global_data_ptr->c_flags.mac_ctrl_header_r_bit;
/*      mr_ul_alloc_cnf_sig_ptr->cs  = pua_data_ptr->chan_coding_cmd;*/

/*      gmac_local_cs_setting = pua_data_ptr->chan_coding_cmd;*/

      /***************************************************************
      *                 load tfi info for rlc                        *
      ***************************************************************/
      if ((gmac_pua_tfi_info(mr_ul_alloc_cnf_sig_ptr,gas_id)) == FALSE)
      {
        /* no TFI assigned to UL, return error code exit PUA handler */
        MSG_GERAN_LOW_0_G("MAC info PUA no UL TFI");
        return(ASSIGNMENT_DATA_ERROR);
      }

      /***************************************************************
      *                 determine tbf_type for rlc                   *
      ***************************************************************/
      gmac_ul_rlc_tbf_type(idle_data_ptr->access_granted,mr_ul_alloc_cnf_sig_ptr, gas_id);

      /***************************************************************
      *        calculate number of ts allocated for the ul tbf       *
      ***************************************************************/
      gmac_pua_rlc_nts(mr_ul_alloc_cnf_sig_ptr, gas_id);

      /***************************************************************
      *                                                              *
      *   determine and load "MAC_L1_DYNAMIC_TBF_CONFIG" signal      *
      *             data for a DYNAMIC UL TBF                        *
      ***************************************************************/
      memset (ml_dynamic_tbf_info_ptr,
              0,
              sizeof(mac_dynamic_tbf_information_T));

      /***************************************************************
      *  load ul & dl power control parmas, ts alloc, usf alloc,     *
      *  usf ganularity                                              *
      ***************************************************************/
      gmac_global_dynamic_alloc_ie(ml_dynamic_tbf_info_ptr, pua_data_ptr, gas_id);


      /******************************************************************
      *            Set global TBF mode                                  *
      ******************************************************************/
      if(pua_data_ptr->egprs_pua == valid)
      {
        global_data_ptr->current_tbf_mode = TBF_MODE_EGPRS;
      }
      else
      {
        global_data_ptr->current_tbf_mode = TBF_MODE_GPRS;
      }


      /***************************************************************
      *              check multi-slot capabilities                   *
      ****************************************************************/
      if(pua_data_ptr->dynamic_alloc.timeslot_alloc_flag == usf_only)
      {
        tx_ts_alloc_bitmap = pua_data_ptr->dynamic_alloc.usf_bitmap;
      }
      else
      {
        tx_ts_alloc_bitmap = pua_data_ptr->dynamic_alloc.gamma_bitmap;
      }

      if ( tx_ts_alloc_bitmap == 0 )
      {
        MSG_GERAN_HIGH_0_G("GMAC PUA contains no timeslot allocation, ASSIGNMENT_DATA_ERROR");
        return(ASSIGNMENT_DATA_ERROR);
      }

      /* reverse usf_bitmap to coincide with TS bitmap */
      for (i=0;i<8;i++)
      {
        if ((uint8)tx_ts_alloc_bitmap & usf_mask)
        {
          ts_alloc_tx_bitmap |= ts_mask;
          ts_alloc_rx_bitmap |= ts_mask;
        }
        usf_mask <<= 1;
        ts_mask >>= 1;
      }

      global_data_ptr->gmac_dtm_cs_ts_alloc = GMAC_NO_TS_ALLOCATED_IN_BITMAP;

#ifdef FEATURE_GPRS_PS_HANDOVER
      if ( gmac_util_multislot_class_check(
                              ts_alloc_rx_bitmap,
                              GMAC_NO_TS_ALLOCATED_IN_BITMAP,
                              pua_data_ptr->dynamic_alloc.ext_dynamic_alloc,
                              GMAC_NO_TS_ALLOCATED_IN_BITMAP,
                              FALSE, PSHO_UPDATE_PARAMS_NOT_REQUIRED,
                              gas_id ) == FALSE )
#else
      if ( gmac_util_multislot_class_check(
                              ts_alloc_rx_bitmap,
                              GMAC_NO_TS_ALLOCATED_IN_BITMAP,
                              pua_data_ptr->dynamic_alloc.ext_dynamic_alloc,
                              GMAC_NO_TS_ALLOCATED_IN_BITMAP,
                              FALSE,
                              gas_id ) == FALSE )
#endif /*FEATURE_GPRS_PS_HANDOVER*/
      {
         /*multislot error exit, return error code exit pua handler */
         MSG_GERAN_HIGH_2_G("MAC Info Error PUA multislot: rx_ts=%d, tx_ts=%d", ts_alloc_rx_bitmap ,ts_alloc_tx_bitmap);
         return(MULTISLOT_ERROR_ACCESS_RETRY);
      }

      /* gmac_local_ul_ts_alloc now set in gmac_util_multislot_class_check */
      global_data_ptr->gmac_local_ul_dl_ctrl_ts = global_data_ptr->gmac_local_ul_dl_ctrl_ts; /* avoid LINT error */

      /***************************************************************
      *               timing_advance_params                          *
      ***************************************************************/
      (void)gmac_packet_ta_ie( &(pua_data_ptr->pkt_timing_adv),
                               &(ml_dynamic_tbf_info_ptr->timing_advance_params),
                                 gas_id );

      /*update local "gmac_update_local_ta_params" with ta info */
      (void)gmac_update_local_ta_params(&(pua_data_ptr->pkt_timing_adv), gas_id);

      /***************************************************************
      *                        starting_time                         *
      ***************************************************************/
      if(pua_data_ptr->dynamic_alloc.tbf_start_time_flag == valid)
      {
        if (gmac_tbf_starting_ie(&(pua_data_ptr->dynamic_alloc.tbf_start_frm_num),
                                 &(ml_dynamic_tbf_info_ptr->starting_time),
                                   gas_id)
                                 == TBF_STARTING_TIME_ERROR_ACCESS_RETRY)
        {
           /*tbf starting time error exit, return error code exit pua handler */
           return(TBF_STARTING_TIME_ERROR_ACCESS_RETRY);
        }
        else
        {
           ml_dynamic_tbf_info_ptr->tbf_start_time_valid = TRUE;
        }
      }
      else
      {
        ml_dynamic_tbf_info_ptr->tbf_start_time_valid = FALSE;
      }

      ml_dynamic_tbf_info_ptr->test_mode = global_data_ptr->current_test_mode;

      if(pua_data_ptr->egprs_pua == valid)
      {
        /*fix for cs change handling*/
        /*update store of current CS*/
        global_data_ptr->gmac_local_cs_setting = pua_data_ptr->egprs_chan_coding_cmd;

        /***************************************************************************
        ********** Assigned EGPRS UL, add EGPRS L1 UL PARAMTERS to Ul config *******
        ****************************************************************************/
        ml_dynamic_tbf_info_ptr->tbf_mode = TBF_MODE_EGPRS;

        /***************************  BEP PERIOD  **********************************/
        if(pua_data_ptr->bep_period2_flag == valid)
        {
          ml_dynamic_tbf_info_ptr->bep_period2.bep_period2 = pua_data_ptr->bep_period2;
          ml_dynamic_tbf_info_ptr->bep_period2.present = TRUE;
        }
        else
        {
           ml_dynamic_tbf_info_ptr->bep_period2.present = FALSE;
        }

        /*L1 needs to know RLC mode to determine if IR applicable */
        ml_dynamic_tbf_info_ptr->rlc_mode = (mac_rlc_mode_T)global_data_ptr->gmac_local_stored_ul_chan_req.rlc_mode;

        /***************************************************************************
        ******** Assigned EGPRS UL, add EGPRS RLC UL PARAMTERS to Ul config ********
        ****************************************************************************/
        mr_ul_alloc_cnf_sig_ptr->tbf_mode = TBF_MODE_EGPRS;

        if (gmac_util_egprs_mcs_valid(pua_data_ptr->egprs_chan_coding_cmd, gas_id) == TRUE)
        {
          mr_ul_alloc_cnf_sig_ptr->mcs = pua_data_ptr->egprs_chan_coding_cmd;
        }
        else
        {
          MSG_GERAN_ERROR_0_G("GMAC PUA EGPRS MCS Invalid, Ignore Msg");
          return(ASSIGNMENT_DATA_ERROR);
        }

        mr_ul_alloc_cnf_sig_ptr->resegment = pua_data_ptr->resegment;
 
        global_data_ptr->current_egprs_ul_ws = gmac_egprs_decode_ul_window_size(
                              pua_data_ptr->egprs_window_size,
                              pua_data_ptr->dynamic_alloc.usf_bitmap,
                              gas_id);
                              
        /*****************************************************************************
        *               check to see if WS is valid                                  *
        * note each rcvd PUA is considered as an new assignment that supersedes any  *
        * previous assignment thus there is no need to check the current ws with the *
        * assigned                                                                   *
        *****************************************************************************/
        if(global_data_ptr->current_egprs_ul_ws != WS_ALLOCATION_INVALID )
        {
          mr_ul_alloc_cnf_sig_ptr->ws = global_data_ptr->current_egprs_ul_ws;
        }
        else
        {
          /*invlaid access retry*/
          return(ASSIGNMENT_DATA_ERROR);
        }
/*2p rac*/
        /****************************************************************************
        *                   Check if RAC info has been requested                    *
        *****************************************************************************/

        MSG_GERAN_MED_2_G("MAC info RAC data in PUA access_tech_req %d, arac_retran_req %d",
                          pua_data_ptr->access_technologies_req_flag,
                          pua_data_ptr->arac_retran_req);

        if (pua_data_ptr->access_technologies_req_flag == valid)
        {
          /*store the Networks list of supported bands*/
          global_data_ptr->access_tech_req_present = TRUE;
          global_data_ptr->access_tech_req_count = pua_data_ptr->access_tech_reg_length;
          memset(global_data_ptr->access_tech_req,0,MAX_ACCESS_TECH_TYPE);
          memscpy(global_data_ptr->access_tech_req ,sizeof(global_data_ptr->access_tech_req),
                  pua_data_ptr->access_technologies_req,MAX_ACCESS_TECH_TYPE);

        }
        else
        {
          global_data_ptr->access_tech_req_present = FALSE;
        }

        /*changed 2p rac*/
        global_data_ptr->c_flags.egprs_rac_retran_req = pua_data_ptr->arac_retran_req;

      }
      else
      {
        /********** EGPRS UL was not assigned default to GPRS **********/

        /*update store of current CS*/
        global_data_ptr->gmac_local_cs_setting = pua_data_ptr->chan_coding_cmd;

        /*mr_ul_alloc_cnf config params*/
        mr_ul_alloc_cnf_sig_ptr->cs  = pua_data_ptr->chan_coding_cmd;
        mr_ul_alloc_cnf_sig_ptr->tbf_mode = TBF_MODE_GPRS;

        /*MAC_L1_DYNAMIC_TBF_CONFIG config params*/
        ml_dynamic_tbf_info_ptr->tbf_mode = TBF_MODE_GPRS;

      }

  }

/*event logging changes*/
  mac_log_gprs_pua_event( global_data_ptr->gmac_local_ul_ts_alloc, pua_data_ptr->chan_coding_cmd, gas_id );

  return(pua_result);
}

/*===========================================================================
===
===  FUNCTION    pua_sb_handler()
===
===  Handles the PUA msg when received  in mac idle mode
===
===  DEPENDENCIES
===
===  global pointer to the msg data section of the csn.1 decode dl msg
===  global mac_l1_sig var used to store mac_l1_sb_tbf_config sig data
===
===  RETURN VALUE
===  An error value that indicates what action should be taken if the
===  data received in the PUA is erroneous
===
===  SIDE EFFECTS
===
===========================================================================*/

static pua_data_error_t pua_sb_handler(gprs_mac_action_t current_mac_action,
                                       gas_id_t gas_id )
{

  boolean egprs_capable_cell = FALSE;

  /*    result of checking pua            */
  pua_data_error_t  pua_result = NO_ERRORS;

  /*    csn.1 decoded pua                 */
  packet_uplink_assignment_t *pua_data_ptr;

  /*    mac_l1_sb_tbf_config sig       */
  mac_sb_tbf_information_T *ml_sb_tbf_info_ptr;

  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_idle_data_t *idle_data_ptr = gmac_get_idle_data_ptr(gas_id);

  /*     assign signal pointers           */
  pua_data_ptr = (packet_uplink_assignment_t *)global_data_ptr->csn_dec_data_ptr;
  ml_sb_tbf_info_ptr = &(idle_data_ptr->mac_l1_sig.mac_l1_single_block_config.sb_tbf);

  /*******************************************************************
  *   determine and load "MAC_L1_SINGLE_BLOCK_CONFIG" signal data    *
  *                     for a SB UL TBF                              *
  *******************************************************************/
  memset (ml_sb_tbf_info_ptr,
          0,
          sizeof(mac_sb_tbf_information_T));

  /*When EGPRS is supported on the cell but EGPRS PRAChing is not. The MS will   */
  /*use GPRS PRACHing with of 2 phase access type to establsih a EGPRS ul. Under */
  /*these conditions the NW will send a PUA with a GPRS SB alloc. As the MS is   */
  /*requesting EGPRS ul est it handles the SB alloc as if it were a MB alloc of  */
  /*1 block and will assume that " Access Technologies Request" is present       */
  /*and report the access technology type for the frequency of the BCCH          */
  /* ref   4.60 7.1.3.2      */

  rr_read_public_store( RRPS_CELL_OPTIONS_EGPRS_CAPABLE_CELL, &egprs_capable_cell, gas_id );
  if(egprs_capable_cell == TRUE)
  {
     /*EGPRS UL tbf requested*/
     MSG_GERAN_HIGH_0_G("MAC info EGPRS 2p access initiated using GPRS_PRACH");
     global_data_ptr->egprs_2p_sb_mb_alloc = EGPRS_SB_ALLOC;

     /*as there are no EGPRS params*/
     global_data_ptr->c_flags.egprs_rac_retran_req = FALSE;
     global_data_ptr->access_tech_req_present = FALSE;

     /*indicate that EGPRS params are present in the MAC_L1_SINGLE_BLOCK_CONFIG*/
     ml_sb_tbf_info_ptr->tbf_mode = TBF_MODE_EGPRS;
         ml_sb_tbf_info_ptr->num_rb_alloc = 1;

     /*there will be no BEP when PUA with SB alloc is RCVD*/
     ml_sb_tbf_info_ptr->bep_period2.present = FALSE;
  }
  else
  {
     /*GPRS UL tbf requested*/
     global_data_ptr->egprs_2p_sb_mb_alloc = GPRS_SB_ALLOC;

     /*indicate that NO EGPRS params are present in the MAC_L1_SINGLE_BLOCK_CONFIG*/
     ml_sb_tbf_info_ptr->tbf_mode = TBF_MODE_GPRS;

     /*there will be no BEP when PUA with SB alloc is RCVD*/
     ml_sb_tbf_info_ptr->bep_period2.present = FALSE;
  }

  /*determine the RLC tbf access phase etiher 1 phase,2phase or short access*/

  if((idle_data_ptr->access_granted == two_phase) ||(idle_data_ptr->access_granted == one_phase))
  {
    ml_sb_tbf_info_ptr->two_phase_access = TRUE;
  }
  else
  {
    ml_sb_tbf_info_ptr->two_phase_access = FALSE;
  }

  if(current_mac_action == PMR_EST)
  {
    ml_sb_tbf_info_ptr->meas_report = TRUE;
    ml_sb_tbf_info_ptr->nc_non_drx_period = global_data_ptr->gmac_local_pmr_params.nc_non_drx_period;
    ml_sb_tbf_info_ptr->nco = global_data_ptr->gmac_local_pmr_params.nco;
  }
  else
  {
    ml_sb_tbf_info_ptr->meas_report = FALSE;
  }

  ml_sb_tbf_info_ptr->ts_number = pua_data_ptr->sngl_blk_alloc.timeslot_num;

  if(pua_data_ptr->sngl_blk_alloc.alpha_gamma_flag == valid)
  {
      ml_sb_tbf_info_ptr->alpha_present = TRUE;
      ml_sb_tbf_info_ptr->alpha  = pua_data_ptr->sngl_blk_alloc.alpha;
  }
  else
  {
      ml_sb_tbf_info_ptr->alpha_present = FALSE;
  }

  ml_sb_tbf_info_ptr->gamma_tn = pua_data_ptr->sngl_blk_alloc.gamma_tn;

  gmac_pua_dl_pwr_control(ml_sb_tbf_info_ptr,gas_id);

  /*******************************************************************
  *                    timing_advance_params                         *
  *******************************************************************/
  (void)gmac_packet_ta_ie( &(pua_data_ptr->pkt_timing_adv),
                           &(ml_sb_tbf_info_ptr->timing_advance_params),
                           gas_id );

  /*update local "gmac_update_local_ta_params" with ta info */
  (void)gmac_update_local_ta_params(&(pua_data_ptr->pkt_timing_adv), gas_id);

  /********************************************************************
  *                        starting_time                              *
  *********************************************************************/
  if (gmac_tbf_starting_ie(&(pua_data_ptr->sngl_blk_alloc.tbf_start_frm_num),
                           &(ml_sb_tbf_info_ptr->starting_time),
                             gas_id)
                           == TBF_STARTING_TIME_ERROR_ACCESS_RETRY)
  {
     /*tbf starting time error exit, return error code*/
     /* exit pua handler                              */
     return(TBF_STARTING_TIME_ERROR_ACCESS_RETRY);
  }

#ifdef FEATURE_GSM_DTM
  global_data_ptr->gmac_dtm_cs_ts_alloc = GMAC_NO_TS_ALLOCATED_IN_BITMAP;
#endif /*FEATURE_GSM_DTM*/

/*event logging changes*/
  mac_log_gprs_pua_event( mac_log_convert_single_ts_to_bitmp( pua_data_ptr->sngl_blk_alloc.timeslot_num,gas_id), pua_data_ptr->chan_coding_cmd,
                                                              gas_id );

  return(pua_result);
}


/*===========================================================================
===
===  FUNCTION    gmac_ul_est_access_granted()
===
===  DESCRIPTION
===
===  Loads  "mr_ul_alloc_cnf" signal with access phase value
===  Determines the access granted
===
===  DEPENDENCIES
===
===  pointer to the msg data section of the csn.1 decode dl msg
===
===  RETURN VALUE
===
===  Access granted value , 1 phase, 2 phase or sb alloc
===
===  SIDE EFFECTS
===
===========================================================================*/

access_type_e gmac_ul_est_access_granted(access_type_e rlc_access_requested,  gprs_mac_action_t action,
                                         gas_id_t gas_id)
{
  /*default, error case as this option is not supported.*/
    access_type_e granted = single_block;

  /*    csn.1 decoded pua                 */
  packet_uplink_assignment_t *pua_data_ptr;

  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  pua_data_ptr = (packet_uplink_assignment_t *)global_data_ptr->csn_dec_data_ptr;

  if(action == UL_DATA_EST)
  {
        switch( rlc_access_requested )
        {
         case one_phase :

              if( (pua_data_ptr->allocation_flag == single_blk_alloc)
                  || (pua_data_ptr->allocation_flag == multi_blk_allocation) )
              {
                granted = two_phase;
              }
              else
              {
                granted = one_phase;
              }
         break;

         case two_phase :

              if( (pua_data_ptr->allocation_flag == single_blk_alloc)
                  || (pua_data_ptr->allocation_flag == multi_blk_allocation) )
              {
                granted = two_phase;
              }
              else
              {
                granted = one_phase;
              }
         break;

         case single_block:
              /*used for pmr and pccf */
              if(pua_data_ptr->allocation_flag == single_blk_alloc)
              {
                granted = single_block;
              }
              else
              {
               
                MSG_GERAN_LOW_2_G("MAC Infor error In access granted, acc_req=%d,acc grant=%d" ,
                                  global_data_ptr->access_requested ,
                                  pua_data_ptr->allocation_flag);
              }
         break;


         case signalling:
         case mm_proc:
         case short_acc:
         case cell_update:
         case page_response:
              if( (pua_data_ptr->allocation_flag == single_blk_alloc)
                  || (pua_data_ptr->allocation_flag == multi_blk_allocation) )
              {
                granted = two_phase;
              }
              else
              {
                granted = one_phase;
              }
         break;
         default: break;
        }

  }
  else if ((action == PMR_EST) || (action == PCCF_EST))
  {
      granted = single_block;
  }

  MSG_GERAN_MED_1_G("MAC DEBUG access_granted: %d", granted);

  return(granted);
}

/*===========================================================================
===
===  FUNCTION    gmac_include_tlli_in_rlc_block()
===
===  DESCRIPTION
===
===  For 1 phase access.
===  Loads "mr_ul_alloc_cnf" signal with relevant tlli data required by
===  RLC for inclusion in RLC data block during 1 phase contention resolution
===
===  DEPENDENCIES
===
===  pointer to the msg data section of the csn.1 decode dl msg
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
static void gmac_include_tlli_in_rlc_block( access_type_e acc_granted,mr_ul_alloc_cnf_t *mr_ul_alloc_ptr,
                                            gas_id_t gas_id)
{

  /*TLLI in loging*/
  uint32 local_rrps_tlli = 0 ;

  /*    csn.1 decoded pua                 */
  packet_uplink_assignment_t *pua_data_ptr;

  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  pua_data_ptr = (packet_uplink_assignment_t *)global_data_ptr->csn_dec_data_ptr;

  /*TLLI in*/
  rr_read_public_store(RRPS_TLLI,&local_rrps_tlli,gas_id);

  if(acc_granted == one_phase)
  {
    /* determine tlli includion in rlc data block*/
    if(pua_data_ptr->tlli_blk_chan_coding == 0)
    {
       /* 0 the MS shall use CS-1*/
       mr_ul_alloc_ptr->tlli_block_cs = 0 ;  /* cs-1 */
    }
    else
    {
      /*MAC sets TLLI Blk to 1, which indicates to RLC that it should use (EGPRS)CHANNEL_CODING_COMMAND*/
      mr_ul_alloc_ptr->tlli_block_cs = pua_data_ptr->tlli_blk_chan_coding;
    }

    mr_ul_alloc_ptr->ti = RM_TLLI_PRESENT;

    /*TLLI in*/
    MSG_GERAN_HIGH_2_G(" gmac_store_ul_est_tlli %x, local_rrps_tlli =%x",global_data_ptr->gmac_store_ul_est_tlli,local_rrps_tlli);
    mr_ul_alloc_ptr->tlli = global_data_ptr->gmac_store_ul_est_tlli;
  }
  else if(acc_granted == two_phase)
  {
    mr_ul_alloc_ptr->ti = RM_TLLI_NOT_PRESENT;
  }
}

/*===========================================================================
===
===  FUNCTION    gmac_pua_tfi_info()
===
===  DESCRIPTION
===
===  Loads "mr_ul_alloc_cnf" signal with the ul tfi data
===  Update mac's local copy of ul_tfi
===
===  DEPENDENCIES
===
===  pointer to the msg data section of the csn.1 decode dl msg
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
static boolean gmac_pua_tfi_info(mr_ul_alloc_cnf_t *mr_ul_alloc_ptr, gas_id_t gas_id)
{
  boolean result = FALSE;

  /*    csn.1 decoded PUA                */
  packet_uplink_assignment_t *pua_data_ptr;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_idle_data_t *idle_data_ptr = gmac_get_idle_data_ptr(gas_id);
  pua_data_ptr = (packet_uplink_assignment_t *)global_data_ptr->csn_dec_data_ptr;  

  if(pua_data_ptr->allocation_flag == fixed_alloc )
  {
    if(pua_data_ptr->uplk_tfi_assignment_flag == valid)
    {
      mr_ul_alloc_ptr->ul_tfi = pua_data_ptr->uplk_tfi_assignment;
      global_data_ptr->gmac_local_tfi_store.ul_tfi = pua_data_ptr->uplk_tfi_assignment;
      global_data_ptr->gmac_local_tfi_store.ul_tfi_valid = TRUE;
      result = TRUE;
    }
  }
  else if (pua_data_ptr->allocation_flag == dynamic_alloc_e)
  {
    
    if(pua_data_ptr->dynamic_alloc.uplk_tfi_assignment_flag == valid)
    {    
      /*for case where we are in 1 phase contention res state, have rcvd a PUA */
      /*in response to requesting ul resource reallocation NEED to update      */
      /*gmac_re_assign_ul_tfi so it can be used in transfer for the case where */
      /*contention res is completed by a PUAN before idle gets the connect_ind */
      /*for the re allocated assignment                                        */            


      if( (idle_data_ptr->i_flags.idle_ul_realloc_state == IDLE_REALLOC_REQUESTED)
          || (idle_data_ptr->i_flags.rcvd_unsolicited_pua_waiting_for_connected_ind == TRUE))
      
      {
      if(idle_data_ptr->i_flags.idle_ul_realloc_state == IDLE_REALLOC_REQUESTED)
      {
        MSG_GERAN_MED_0_G("GMAC info Idle UL re-alloc ");
        }
        
        if(idle_data_ptr->i_flags.rcvd_unsolicited_pua_waiting_for_connected_ind == TRUE)
        {
          MSG_GERAN_MED_0_G("GMAC info unsolicited GPRS pua rcvd in idle contention res");
        } 
        
        global_data_ptr->gmac_re_assign_ul_tfi = pua_data_ptr->dynamic_alloc.uplk_tfi_assignment;
      }
      else
      {         
        global_data_ptr->gmac_local_tfi_store.ul_tfi = pua_data_ptr->dynamic_alloc.uplk_tfi_assignment;
      }
      
      mr_ul_alloc_ptr->ul_tfi = pua_data_ptr->dynamic_alloc.uplk_tfi_assignment;
      global_data_ptr->gmac_local_tfi_store.ul_tfi_valid = TRUE;
      result = TRUE;
            
    }
  }
  return (result);
}

/*===========================================================================
===
===  FUNCTION    gmac_ul_rlc_tbf_type()
===
===  DESCRIPTION
===
===  Determines "tbf type" for rlc and loads it into "mr_ul_alloc_cnf" signal
===
===  DEPENDENCIES
===
===  pointer to the msg data section of the csn.1 decode dl msg
===  the paket channel description ie received in RM_UL_EST_TBF_REQ
===  ie gmac_local_stored_ul_chan_req
===
===  rlc tbf type , RM_CLOSE_ENDED_TBF  or RM_OPEN_ENDED_TBF
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/

static void gmac_ul_rlc_tbf_type(access_type_e pua_access_granted, mr_ul_alloc_cnf_t *mr_ul_alloc_ptr,
                                 gas_id_t gas_id)
{
   /*    csn.1 decoded pua                 */
   packet_uplink_assignment_t *pua_data_ptr;
   gmac_global_data_t         *global_data_ptr = gmac_get_global_data_ptr(gas_id);
   pua_data_ptr = (packet_uplink_assignment_t *)global_data_ptr->csn_dec_data_ptr;   

   if(pua_data_ptr->allocation_flag == fixed_alloc )
   {
      /*ref 4.60 , 8.1.1.3*/
     if(pua_access_granted == one_phase)
     {
       mr_ul_alloc_ptr->tbf_type = RM_OPEN_ENDED_TBF;
     }
     else if (pua_access_granted == two_phase)
     {
       /*note ref 4.60 , 8.1.1.3 */
       if( global_data_ptr->gmac_local_stored_ul_chan_req.rlc_octet_cnt == 0)
       {
         mr_ul_alloc_ptr->tbf_type = RM_OPEN_ENDED_TBF;
       }
       else
       {
         mr_ul_alloc_ptr->tbf_type = RM_CLOSE_ENDED_TBF;
       }
     }
   }
   else if (pua_data_ptr->allocation_flag == dynamic_alloc_e)
   {
      /*******************************************************************
      *         determine tbf_type(open or close ended for rlc           *
      *               for dynamic ref 4.60 chap 8.1.1.1                  *
      *******************************************************************/
      if(pua_data_ptr->dynamic_alloc.rlc_data_blks_granted_flag == valid)
      {
        mr_ul_alloc_ptr->tbf_type = RM_CLOSE_ENDED_TBF;
        mr_ul_alloc_ptr->blocks_granted = pua_data_ptr->dynamic_alloc.rlc_data_blks_granted + 9;
      }
      else
      {
        mr_ul_alloc_ptr->tbf_type = RM_OPEN_ENDED_TBF;
        mr_ul_alloc_ptr->blocks_granted = global_data_ptr->gmac_local_stored_ul_chan_req.rlc_octet_cnt;
      }
   }
}

/*===========================================================================
===
===  FUNCTION    gmac_pua_rlc_nts()
===
===  DESCRIPTION
===
===  calculates and loads it into "mr_ul_alloc_cnf" signal with the number of ul
===  ts that have been in assigned.
===
===  DEPENDENCIES
===
===  pointer to the msg data section of the csn.1 decode dl msg
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/

static void gmac_pua_rlc_nts( mr_ul_alloc_cnf_t *mr_ul_alloc_ptr,
                              gas_id_t gas_id)
{
   /*    csn.1 decoded pua                 */
   packet_uplink_assignment_t *pua_data_ptr;
   gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
   pua_data_ptr = (packet_uplink_assignment_t *)global_data_ptr->csn_dec_data_ptr;

   if(pua_data_ptr->allocation_flag == fixed_alloc )
   {
     if(pua_data_ptr->ts_alloc_pwr_flag == timeslot_alloc)
     {
       mr_ul_alloc_ptr->num_timeslots = gmac_utils_calculate_nts(pua_data_ptr->ts_allocation, gas_id);
     }
     else
     {
        mr_ul_alloc_ptr->num_timeslots = gmac_utils_calculate_nts(pua_data_ptr->pwr_ctrl_paramtrs.gamma_bitmap,
                                                                  gas_id);
     }
   }
   else if(pua_data_ptr->allocation_flag == dynamic_alloc_e )
   {
     if(pua_data_ptr->dynamic_alloc.timeslot_alloc_flag == usf_only)
     {
        mr_ul_alloc_ptr->num_timeslots = gmac_utils_calculate_nts(pua_data_ptr->dynamic_alloc.usf_bitmap,
                                                                  gas_id);
     }
     else
     {
        mr_ul_alloc_ptr->num_timeslots = gmac_utils_calculate_nts(pua_data_ptr->dynamic_alloc.gamma_bitmap,
                                                                  gas_id);
     }
   }
}

/*===========================================================================
===
===  FUNCTION    gmac_pua_dl_pwr_control()
===
===  DESCRIPTION
===
===  DEPENDENCIES
===
===  pointer to the msg data section of the csn.1 decode dl msg
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
static void gmac_pua_dl_pwr_control(void *ml_sig_ptr,
                                    gas_id_t gas_id)
{
   /*    csn.1 decoded pua                 */
   packet_uplink_assignment_t *pua_data_ptr;
   gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
   pua_data_ptr = (packet_uplink_assignment_t *)global_data_ptr->csn_dec_data_ptr;

   if(pua_data_ptr->allocation_flag == fixed_alloc )
   {
      /* downlink power control  P0, BTS_PWR_CTRL_MODE, PR_MODE to message*/
      if (pua_data_ptr->po_prmode_flag == valid)
      {
        ((mac_fixed_tbf_information_T *)ml_sig_ptr)->downlink_pwr.params_valid = TRUE;
        ((mac_fixed_tbf_information_T *)ml_sig_ptr)->downlink_pwr.po = pua_data_ptr->po;
        ((mac_fixed_tbf_information_T *)ml_sig_ptr)->downlink_pwr.pr_mode = pua_data_ptr->pr_mode;
        ((mac_fixed_tbf_information_T *)ml_sig_ptr)->downlink_pwr.bts_pwr_ctrl_mode
                                                  = pua_data_ptr->bts_pwr_ctrl_mode;
      }
      else
      {
        ((mac_fixed_tbf_information_T *)ml_sig_ptr)->downlink_pwr.params_valid = FALSE;
      }
   }
   else if(pua_data_ptr->allocation_flag == dynamic_alloc_e )
   {
      if (pua_data_ptr->dynamic_alloc.po_prmode_flag == valid)
      {
        ((mac_dynamic_tbf_information_T *)ml_sig_ptr)->dl_ctrl_param.params_valid = TRUE;
        ((mac_dynamic_tbf_information_T *)ml_sig_ptr)->dl_ctrl_param.po = pua_data_ptr->dynamic_alloc.po;
        ((mac_dynamic_tbf_information_T *)ml_sig_ptr)->dl_ctrl_param.pr_mode = pua_data_ptr->dynamic_alloc.pr_mode;
      }
      else
      {
        ((mac_dynamic_tbf_information_T *)ml_sig_ptr)->dl_ctrl_param.params_valid = FALSE;
      }
   }
   else if(pua_data_ptr->allocation_flag == single_blk_alloc )
   {
      /* downlink power control  P0, BTS_PWR_CTRL_MODE, PR_MODE to message*/
      if (pua_data_ptr->sngl_blk_alloc.po_prmode_flag == valid)
      {
        ((mac_sb_tbf_information_T *)ml_sig_ptr)->downlink_pwr.params_valid = TRUE;
        ((mac_sb_tbf_information_T *)ml_sig_ptr)->downlink_pwr.po = pua_data_ptr->sngl_blk_alloc.po;
        ((mac_sb_tbf_information_T *)ml_sig_ptr)->downlink_pwr.pr_mode = pua_data_ptr->sngl_blk_alloc.pr_mode;
        ((mac_sb_tbf_information_T *)ml_sig_ptr)->downlink_pwr.bts_pwr_ctrl_mode = pua_data_ptr->sngl_blk_alloc.bts_pwr_ctrl_mode;
      }
      else
      {
        ((mac_sb_tbf_information_T *)ml_sig_ptr)->downlink_pwr.params_valid = FALSE;
      }
   }
   else if(pua_data_ptr->allocation_flag == multi_blk_allocation )
   {
      /* downlink power control  P0, BTS_PWR_CTRL_MODE, PR_MODE to message*/
      if (pua_data_ptr->multi_blk_alloc.po_prmode_flag == valid)
      {
        ((mac_sb_tbf_information_T *)ml_sig_ptr)->downlink_pwr.params_valid = TRUE;
        ((mac_sb_tbf_information_T *)ml_sig_ptr)->downlink_pwr.po = pua_data_ptr->multi_blk_alloc.po;
        ((mac_sb_tbf_information_T *)ml_sig_ptr)->downlink_pwr.pr_mode = pua_data_ptr->multi_blk_alloc.pr_mode;
        ((mac_sb_tbf_information_T *)ml_sig_ptr)->downlink_pwr.bts_pwr_ctrl_mode = pua_data_ptr->multi_blk_alloc.bts_pwr_ctrl_mode;
      }
      else
      {
        ((mac_sb_tbf_information_T *)ml_sig_ptr)->downlink_pwr.params_valid = FALSE;
      }
   }
}

/*===========================================================================
===
===  FUNCTION    gmac_dl_msg_data_valid()
===
===  DESCRIPTION
===
===   Checks the csn.1 decode dl msg for fatal data corruption
===
===  DEPENDENCIES
===
===  pointer to the msg data section of the csn.1 decode dl msg
===
===  RETURN VALUE
===
===  TRUE if a fatal error occurs.
===
===  SIDE EFFECTS
===
===========================================================================*/
boolean gmac_dl_msg_data_valid(gas_id_t gas_id)
{
  boolean error = FALSE;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  if (global_data_ptr->csn_dec_hdr_ptr->error_code == MESSAGE_ESCAPE_ERROR)
  {
    error = TRUE;
    MSG_GERAN_LOW_0_G("MAC Info CSN.1 non fatal msg data error");
  }
  return(error);
}

/*===========================================================================
===
===  FUNCTION      gmac_util_send_mac_grr_mac_status()
===
===  DESCRIPTION
===
===  Send mac_grr_mac_status to grr
===
===  DEPENDENCIES
===
===  global definition used by idle mode to store mac to grr signal .
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void gmac_util_send_mac_grr_mac_status(mac_grr_sig_t *grr_sig, rr_mac_status_t status,
                                       gas_id_t gas_id)
{
   grr_sig->mac_grr_mac_status.mac_status = status;
   grr_sig->mac_grr_mac_status.message_header.message_set = MS_MAC_RR;
   grr_sig->mac_grr_mac_status.message_header.message_id = (uint8)MAC_GRR_MAC_STATUS;
   gprs_grr_mac_acc_func(grr_sig,gas_id);
}

/*===========================================================================
===
===  FUNCTION      gmac_idle_send_tbf_config_sigs()
===
===  DESCRIPTION
===
===  send MAC_L1_FIXED_TBF_CONFIG or MAC_L1_DYNAMIC_TBF_CONFIG to l1
===  and MR_UL_ALLOC_CNF to RLC on a successful PUA msg (ie
===  frequency confirmation form L1 is positive)
===
===  DEPENDENCIES
===
===  global definition used by idle mode to store mac to l1 signal
===  and mac to rlc signal
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void gmac_idle_send_tbf_config_sigs
(
mac_l1_sig_t *l1_sig_ptr,
mac_rlc_msg_t *rlc_sig_ptr,
mac_l1_sig_id_t l1_sig_id,
mr_ul_alloc_cnf_t *mr_ul_alloc_cnf_sig_ptr,
gas_id_t gas_id
)
{
   gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
   gmac_idleutil_data_t *global_idleutil_ptr = gmac_get_idleutil_data_ptr(gas_id);

   if(l1_sig_id == MAC_L1_FIXED_TBF_CONFIG)
   {
      l1_sig_ptr->mac_l1_fixed_tbf_config.message_header.message_set = MS_MAC_L1;
      l1_sig_ptr->mac_l1_fixed_tbf_config.message_header.message_id = (uint8)MAC_L1_FIXED_TBF_CONFIG;
      gprs_l1_mac_acc_func(l1_sig_ptr,gas_id);
   }
   else if(l1_sig_id == MAC_L1_DYNAMIC_TBF_CONFIG )
   {
      l1_sig_ptr->mac_l1_dynamic_tbf_config.message_header.message_set = MS_MAC_L1;
      l1_sig_ptr->mac_l1_dynamic_tbf_config.message_header.message_id = (uint8)MAC_L1_DYNAMIC_TBF_CONFIG;
      gprs_l1_mac_acc_func(l1_sig_ptr,gas_id);
   }

   /*  get a buffer for storing MR_UL_ALLOC_CNF signal data   */
   rlc_sig_ptr = grlc_get_cmd_buf_mac();
   if (rlc_sig_ptr != NULL)
   {
      /*copy stored MR_UL_ALLOC_CNF data into signal*/
      rlc_sig_ptr->msg.ul_alloc_cnf = *mr_ul_alloc_cnf_sig_ptr;

      rlc_sig_ptr->msg.sig = MR_UL_ALLOC_CNF;
      gmac_rlc_mac_insert_gas_id(rlc_sig_ptr,gas_id);
      grlc_put_cmd_mac(gas_id,rlc_sig_ptr);


      /*Featured ul_tbf_est logging. Consider both feature and both option in EDGE. */

      /*First decide the BEP_prd2*/

      if( l1_sig_ptr->mac_l1_dl_tbf_config.bep_period2.present == TRUE)
      {
        global_idleutil_ptr->log_BEP_prd2= l1_sig_ptr->mac_l1_dl_tbf_config.bep_period2.bep_period2 ;
      }
      else
      {
        global_idleutil_ptr->log_BEP_prd2=BEP_PERIOD2_NOT_SPECIFIED;
      }



      if (global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS)
      {
        mac_log_ul_etbf_establish(
          mr_ul_alloc_cnf_sig_ptr->ul_tfi,
          global_data_ptr->gmac_local_ul_ts_alloc,
          (byte) EGPRS_TBF,
          mr_ul_alloc_cnf_sig_ptr->ws,
          mr_ul_alloc_cnf_sig_ptr->resegment ,
          (uint8)global_idleutil_ptr->log_BEP_prd2,
          gas_id);

      }
      else
      {
        mac_log_ul_tbf_establish(mr_ul_alloc_cnf_sig_ptr->ul_tfi,
          global_data_ptr->gmac_local_ul_ts_alloc,gas_id);
      }
   }
   else
   {
      ERR_GERAN_FATAL_0_G("MAC ERROR failed to allocated message memory");
   }

}
 
/*===========================================================================
===
===  FUNCTION      gmac_idle_send_mr_ul_alloc_cnf()
===
===  DESCRIPTION
===  MR_UL_ALLOC_CNF to RLC on a successful PUA msg (ie
===  frequency confirmation form L1 is positive)
===
===  DEPENDENCIES
===
===  global definition used by idle mode to store mac to l1 signal
===  and mac to rlc signal
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void gmac_idle_send_mr_ul_alloc_cnf
(
mac_rlc_msg_t *rlc_sig_ptr,
mr_ul_alloc_cnf_t *mr_ul_alloc_cnf_sig_ptr,
gas_id_t gas_id
)
{
   gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
   gmac_idleutil_data_t *global_idleutil_ptr = gmac_get_idleutil_data_ptr(gas_id);

   /*  get a buffer for storing MR_UL_ALLOC_CNF signal data   */
   rlc_sig_ptr = grlc_get_cmd_buf_mac();
   if (rlc_sig_ptr != NULL)
   {
      /*copy stored MR_UL_ALLOC_CNF data into signal*/
      rlc_sig_ptr->msg.ul_alloc_cnf = *mr_ul_alloc_cnf_sig_ptr;

      rlc_sig_ptr->msg.sig = MR_UL_ALLOC_CNF;
      gmac_rlc_mac_insert_gas_id(rlc_sig_ptr,gas_id);
      grlc_put_cmd_mac(gas_id,rlc_sig_ptr);


      if (global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS)
      {
        mac_log_ul_etbf_establish(
          mr_ul_alloc_cnf_sig_ptr->ul_tfi,
          global_data_ptr->gmac_local_ul_ts_alloc,
          (byte) EGPRS_TBF,
          mr_ul_alloc_cnf_sig_ptr->ws,
          mr_ul_alloc_cnf_sig_ptr->resegment ,
          (uint8)global_idleutil_ptr->log_BEP_prd2,
          gas_id);

      }
      else
      {
        mac_log_ul_tbf_establish(mr_ul_alloc_cnf_sig_ptr->ul_tfi,
                                 global_data_ptr->gmac_local_ul_ts_alloc,
                                 gas_id);
      }
   }
   else
   {
      ERR_GERAN_FATAL_0_G("MAC ERROR failed to allocated message memory");
   }

}


/*===========================================================================
===
===  FUNCTION      gmac_idle_send_mac_internal_sig()
===
===  DESCRIPTION
===
===  Send any internal mac signals
===
===  DEPENDENCIES
===
===  global definition used by idle mode to store mac to mac signal
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void gmac_idle_send_mac_internal_sig(mac_int_sig_t *mac_sig,mac_mac_sig_id_t mac_sig_id,
                                     gas_id_t gas_id)
{

 mac_sig->sig_id = mac_sig_id;
 mac_sig->task_id = MS_MAC_MAC;
 gprs_mac_mac_acc_func (mac_sig, gas_id);

}

/*===========================================================================
===
===  FUNCTION      gmac_idle_send_l1_release_sig()
===
===  DESCRIPTION
===
===  Send l1_release_sig to l1
===
===  DEPENDENCIES
===
===  global definition used by idle mode to store mac to l1 signal
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void gmac_idle_send_l1_release_sig(mac_l1_sig_t *l1_sig_ptr,
                                   release_tbf_t release_type,
                                   mac_log_ul_tbf_release_t ul_failure_reason,
                                   mac_log_dl_tbf_release_t dl_failure_reason,
                                   gas_id_t gas_id)
{
   gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
   gmac_idle_data_t *idle_data_ptr = gmac_get_idle_data_ptr(gas_id);


   gmac_idle_stop_access_timers(idle_data_ptr->mac_idle_substate,gas_id);

   if(global_data_ptr->c_flags.tbf_release_pending_flg == FALSE)
   {

    MSG_GERAN_MED_3_G( "MAC sent MAC_L1_RELEASE type %d ul reason %d dl reason %d ",release_type,ul_failure_reason,dl_failure_reason);

    global_data_ptr->c_flags.tbf_release_pending_flg = TRUE;

    if(idle_data_ptr->i_flags.idle_ul_realloc_state != IDLE_REALLOC_NO_ACTION)
    {                     
      MSG_GERAN_MED_0_G("GMAC info set idle_ul_realloc_state = IDLE_REALLOC_NO_ACTION");
      idle_data_ptr->i_flags.idle_ul_realloc_state = IDLE_REALLOC_NO_ACTION;
      idle_data_ptr->i_flags.pending_realloc_req = FALSE;
    }



    idle_data_ptr->i_flags.rcvd_unsolicited_pua_waiting_for_connected_ind = FALSE;

    l1_sig_ptr->mac_l1_release.release_tbf = release_type ;
    l1_sig_ptr->mac_l1_release.message_header.message_set = MS_MAC_L1;
    l1_sig_ptr->mac_l1_release.message_header.message_id = (uint8)MAC_L1_RELEASE;
    gprs_l1_mac_acc_func(l1_sig_ptr,gas_id);


    /*logging*/
    mac_log_packet_tbf_rel_event(release_type,gas_id);

    if(release_type == UL_TBF_NORMAL || release_type == UL_TBF_ABNORMAL)
    {
       mac_log_ul_tbf_release(ul_failure_reason,gas_id);
    }
    else if (release_type == DL_TBF_NORMAL || release_type == DL_TBF_ABNORMAL)
    {
       mac_log_dl_tbf_release(dl_failure_reason,gas_id);
    }
    else
    {
      /* UL_AND_DL_TBF_NORMAL or UL_AND_DL_TBF_ABNORMAL is not applicale to idle mode*/
    }
  }
  else
  {
     MSG_GERAN_MED_0_G( "MAC has already sent MAC_L1_RELEASE don't send subsequent");
  }
}

/*===========================================================================
===
===  FUNCTION      gmac_idle_check_pua_addressing()
===
===  DESCRIPTION
===
===  Checks the MS addressing associated with the PUA msg. The type of
===  addressing used depends in which state PUA msg was received.
===
===  DEPENDENCIES
===
===  csn_dec_data_ptr
===
===  RETURN VALUE
===
===  return_value,  TRUE if addressed correctly
===
===  SIDE EFFECTS
===
===========================================================================*/
boolean gmac_idle_check_pua_addressing
(
  idle_mode_ul_tbf_release_action_t rel_action,
  gprs_mac_idle_substates_t current_idle_substate,
  gas_id_t gas_id
)
{
  boolean return_value = FALSE;

    /*    csn.1 decoded pua                 */
  packet_uplink_assignment_t *pua_data_ptr;
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  pua_data_ptr = (packet_uplink_assignment_t *)global_data_ptr->csn_dec_data_ptr;


  if(rel_action == NO_RE_EST )
  {
     /*****************************************************************
     * pua address checking when pua is part of initial ul assignment *
     * pua sent as a result of praching are addressed using the pkt   *
     * channel request ref                                            *
     * pua sent as a result of PRR are addressed using the tlli       *
     *****************************************************************/
     if(current_idle_substate == UL_2P_ASS)
     {
       if (pua_data_ptr->tlli_gtfi_pkt_ref_valid == tlli_valid)
       {
          /*TLLI in*/
          MSG_GERAN_HIGH_2_G(" gmac_store_ul_est_tlli %x, received TLLI in OTA msg %x",
                             global_data_ptr->gmac_store_ul_est_tlli,pua_data_ptr->tlli);
          if(pua_data_ptr->tlli == global_data_ptr->gmac_store_ul_est_tlli )
             return_value = TRUE;
       }
     }
     else if (current_idle_substate == UL_1P_CONTENTION_RES)
     {
            if (pua_data_ptr->tlli_gtfi_pkt_ref_valid == tlli_valid)
            {
                /*TLLI in*/
                MSG_GERAN_HIGH_2_G(" gmac_store_ul_est_tlli %x, received TLLI in OTA msg %x",
                                   global_data_ptr->gmac_store_ul_est_tlli,pua_data_ptr->tlli);
               if(pua_data_ptr->tlli == global_data_ptr->gmac_store_ul_est_tlli )
                  return_value = TRUE;

            }
            else if(pua_data_ptr->tlli_gtfi_pkt_ref_valid == global_tfi_valid)
            {
               if( (pua_data_ptr->global_tfi.direction == uplink_valid)
                 && (pua_data_ptr->global_tfi.uplink_tfi == global_data_ptr->gmac_local_tfi_store.ul_tfi))
               {
                 return_value = TRUE;
               }
            }
     }
  }
  else
  {
     /****************************************************************
     * pua address checking when pua is part of a re-establishment   *
     * during the ul release process. Note as the previous tbf has   *
     * been released the ul tfi is no longer valid, the pua's        *
     * received during re-est process will be addressed via the tlli *
     ****************************************************************/
     if (pua_data_ptr->tlli_gtfi_pkt_ref_valid == tlli_valid)
     {
        /*TLLI in*/
        MSG_GERAN_HIGH_2_G(" gmac_store_ul_est_tlli %x, received TLLI in OTA msg %x",
                           global_data_ptr->gmac_store_ul_est_tlli,pua_data_ptr->tlli);
        if(pua_data_ptr->tlli == global_data_ptr->gmac_store_ul_est_tlli )
           return_value = TRUE;
     }
  }
  return(return_value);
}

/*===========================================================================
===
===  FUNCTION      check_idle_par_addressing()
===
===  DESCRIPTION
===
===  Determines if the PAR msg addresses the MS correctly.
===  checks for a "wait indication " in reject structure and starts t3172 if
===  its present.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  return_value,  TRUE if addressed correctly
===
===  SIDE EFFECTS
===
===========================================================================*/
boolean check_idle_par_addressing(access_type_e access_type,
                                  gas_id_t gas_id)
{
  boolean return_value = FALSE;
  uint32 local_rrps_tlli =0 ;
  int8 no_reject_structs = 0;
  uint8 i;

  packet_access_reject_t *par_data_ptr;
  reject_struct_t *reject_stuct_ptr;

  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  par_data_ptr = (packet_access_reject_t *)global_data_ptr->csn_dec_data_ptr;
  reject_stuct_ptr = &(((packet_access_reject_t *)global_data_ptr->csn_dec_data_ptr)->reject_struct0);

  no_reject_structs = (int8)par_data_ptr->no_addition_rej_structs;

  for (i = 0; i<= no_reject_structs; i++ )
  {
    if(access_type == two_phase)
    {
        if (reject_stuct_ptr->tlli_gtfi_pkt_ref_valid == tlli_valid)
        {
          /*check if addressed correctly ie tlli */
          /*TLLI in loging*/
          rr_read_public_store(RRPS_TLLI,&local_rrps_tlli,gas_id);

          /*TLLI in*/
          if (global_data_ptr->gmac_store_ul_est_tlli == reject_stuct_ptr->tlli)
          {
            MSG_GERAN_HIGH_2_G(" gmac_store_ul_est_tlli %x, local_rrps_tlli %x",global_data_ptr->gmac_store_ul_est_tlli,local_rrps_tlli);
            MSG_GERAN_LOW_0_G( "MAC info PAR address is TLLI");

            /*check if wait in is present */
            if(reject_stuct_ptr->wait_valid == wait_ind_valid)
               gmac_start_t3172(reject_stuct_ptr->wait_indication, reject_stuct_ptr->wait_indication_size,
                                gas_id);

            /*match found*/
            return_value = TRUE;

            break; /*for loop*/
          }

          /*check next reject struct*/
          reject_stuct_ptr++;
        }
        else
        {
          /*check next reject struct*/
          reject_stuct_ptr++;
        }
    }

  }/*end of for loop*/

  return(return_value);
}
/*===========================================================================
===
===  FUNCTION      gmac_idle_build_rlc_alloc_and_l1_config_sigs()
===
===  DESCRIPTION
===
===  Display DL logical channel msg received
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  Returns one of "pua_data_error_t" depending on the validity of the data
===  in  PUA msg
===
===  SIDE EFFECTS
===
===========================================================================*/

pua_data_error_t gmac_idle_build_rlc_alloc_and_l1_config_sigs
(
uint8 ul_tbf_type,
gprs_mac_action_t current_action,
mr_ul_alloc_cnf_t *mr_ul_alloc_cnf_sig_ptr,
gas_id_t gas_id
)
{
  pua_data_error_t  tbf_assignment_result = NO_ERRORS;

  /*build up mr_ul_alloc_cnf and MAC_L1_XXX_TBF_CONFIG sigs*/
  switch(ul_tbf_type)
  {

   case dynamic_alloc_e:  tbf_assignment_result =  pua_dynamic_handler(mr_ul_alloc_cnf_sig_ptr,gas_id);
   break;

   case single_blk_alloc:
        tbf_assignment_result =  pua_sb_handler(current_action,gas_id);
   break;

   case multi_blk_allocation:
        /* To have rcvd a MB assigment CEll must support EGPRS and EGPRS PRACHING also */
        /* MS must have sent an EGRPS PRACH and rcvd a PUA with EGPRS params           */

        MSG_GERAN_HIGH_0_G("MAC info EGPRS 2p access initiated using EGPRS_PRACH");
        tbf_assignment_result =  pua_mb_handler(current_action,gas_id);
   break;

   case extension_alloc: /*Reserved for future protocol extension ref 4.60 11.2.29.1*/
        tbf_assignment_result = ASSIGNMENT_DATA_ERROR;
   break;

   default:
        tbf_assignment_result = ASSIGNMENT_DATA_ERROR;
   break;

  }
  return(tbf_assignment_result);
}

/*===========================================================================
===
===  FUNCTION      gmac_idle_check_pcco_addressing()
===
===  DESCRIPTION
===
===  The function check the Packet Cell Change Order address the mobile correctly.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
boolean gmac_idle_check_pcco_addressing(gas_id_t gas_id)
{
  boolean addr_match = FALSE;


  mac_header_tfi_t mac_header_tfi_check = MAC_HEADER_TFI_NOT_VALID;

  packet_cell_change_order_t *pcco_data_ptr=NULL;

  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  pcco_data_ptr = (packet_cell_change_order_t *)global_data_ptr->csn_dec_data_ptr;

  if (global_data_ptr->csn_dec_hdr_ptr->error_code != ADDRESS_INFO_ERROR)
  {
    mac_header_tfi_check = gmac_mac_control_header_tfi_checking(gas_id);
    if(mac_header_tfi_check == MAC_HEADER_TFI_NOT_VALID)
    {
      if(pcco_data_ptr->tlli_gtfi_pkt_ref_valid == tlli_valid)
      {
        addr_match = gmac_match_old_or_new_tlli(pcco_data_ptr->tlli,gas_id);
      }
      else
      {
        if (pcco_data_ptr->tlli_gtfi_pkt_ref_valid == global_tfi_valid)
        {
          addr_match = gmac_util_global_tfi(pcco_data_ptr->global_tfi,gas_id);
        }
      }
    }
    else if(mac_header_tfi_check == MAC_HEADER_TFI_MATCH_FOUND)
    {
      addr_match = TRUE;
    }
    else if(mac_header_tfi_check == MAC_HEADER_TFI_MATCH_NOT_FOUND)
    {
      addr_match = FALSE;
    }


  }
  return(addr_match);
}
/*===========================================================================
===
===  FUNCTION      gmac_idle_reset_parameters()
===
===  DESCRIPTION
===
===  While mac_action is not NONE, timers and stored parameters need to be cleared
===  when mac_action is reset.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===========================================================================*/
void gmac_idle_reset_parameters(gprs_mac_idle_substates_t current_idle_substate,
                                gas_id_t gas_id)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_idle_data_t *idle_data_ptr = gmac_get_idle_data_ptr(gas_id);

  memset(&global_data_ptr->gmac_local_pmr_params,0,sizeof(gmac_pmr_params_t));
  memset(&global_data_ptr->gmac_local_pccf_params,0,sizeof(gmac_pccf_params_t));
  memset(&global_data_ptr->gmac_null_store_l1_dl_config,0,sizeof(mac_l1_dl_tbf_config_t));
  memset(&global_data_ptr->gmac_51_sb_params,0,sizeof(mac_l1_single_block_config_t));
  memset(&global_data_ptr->gmac_local_stored_ul_chan_req,0,sizeof(channel_req_desc_t));
                       
  if(idle_data_ptr->i_flags.idle_ul_realloc_state != IDLE_REALLOC_NO_ACTION)
  {
    idle_data_ptr->i_flags.idle_ul_realloc_state = IDLE_REALLOC_NO_ACTION;
    idle_data_ptr->i_flags.pending_realloc_req = FALSE;
    MSG_GERAN_MED_0_G("GMAC info releasing idle_ul_realloc_state, resetting TO IDLE_REALLOC_NO_ACTION");
  }


  idle_data_ptr->i_flags.rcvd_unsolicited_pua_waiting_for_connected_ind = FALSE;
 
  global_data_ptr->gmac_util_data_req_cause = GRR_CAUSE_LAY3_DATA;

  global_data_ptr->c_flags.mac_ctrl_header_r_bit = NOT_SET;

  global_data_ptr->access_requested=one_phase;
  idle_data_ptr->access_granted = one_phase;
  global_data_ptr->idle_mode_ul_tbf_release_action = NO_RE_EST;
/*   tbf_type = 0;  extension alloc */

  if (current_idle_substate == UL_1P_ASS)
  {
    global_data_ptr->idle_mode_ul_tbf_release_action = NO_RE_EST;
  }
  else if (current_idle_substate == UL_2P_ASS)
  {
    global_data_ptr->idle_mode_ul_tbf_release_action = NO_RE_EST;
    global_data_ptr->prr_re_est_rrbp_fn=0;
    global_data_ptr->prr_re_est_rrbp_ts=0;
  }
  else if (current_idle_substate == UL_1P_CONTENTION_RES)
  {
    global_data_ptr->idle_mode_ul_tbf_release_action = NO_RE_EST;
    global_data_ptr->access_requested=one_phase;
    idle_data_ptr->access_granted = one_phase;    
    global_data_ptr->c_flags.gmac_prr_on_pacch_state = GMAC_PRR_ON_PACCH_AWAIT_REQ;         
  }
  else if (current_idle_substate == DL_ASS)
  {
    global_data_ptr->access_requested=one_phase;
    idle_data_ptr->access_granted = one_phase;
    global_data_ptr->gmac_store_dl_rlc_mode =0;
  }
}

/*===========================================================================
===
===  FUNCTION    gmac_idle_send_pmr_cnf_to_grr()
===
===  DESCRIPTION
===  Send measurement report confirmation to GRR. The result indicates
===  whether the measurement report was sent or not.
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_idle_send_pmr_cnf_to_grr(success_failure_t result,
                                   gas_id_t gas_id)
{
  mac_grr_sig_t local_mac_grr_sig;

  MSG_GERAN_LOW_1_G("MAC Sent MAC_GRR_MEAS_RPT_CNF result %d", result);
  local_mac_grr_sig.mac_grr_meas_rpt_cnf.status = result;
  local_mac_grr_sig.mac_grr_meas_rpt_cnf.message_header.message_set = MS_MAC_RR;
  local_mac_grr_sig.mac_grr_meas_rpt_cnf.message_header.message_id = (uint8)MAC_GRR_MEAS_RPT_CNF;
  gprs_grr_mac_acc_func(&local_mac_grr_sig,gas_id);
}
/*===========================================================================
===
===  FUNCTION      gmac_idle_packet_tbf_release()
===
===  DESCRIPTION
===
===  Handles PACKET TBF RELEASE recived in idle mode
===
===  On the dl when waiting to transfer from idle to transfer(ie waiting for the
===  1st data block to be recived.
===
===  On the ul when waiting to move from idle to transfer in 1 phase contention
===  resolution state
===
===  DEPENDENCIES
===
===  RETURN VALUE
===  release_ind - indicating which TBF has been released abnormally
===
===  SIDE EFFECTS
===
===
===========================================================================*/
release_tbf_t gmac_idle_packet_tbf_release (boolean int_ul_tbf,
                                            gas_id_t gas_id)
{
  release_tbf_t release_ind = UL_AND_DL_TBF_ABNORMAL; /* msg incorrectly adresses MS*/
                                                      /* don't release TBF          */

  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  packet_tbf_release_t *packet_rel_ptr = (packet_tbf_release_t *)global_data_ptr->csn_dec_data_ptr;


  mac_header_tfi_t mac_header_tfi_check = MAC_HEADER_TFI_NOT_VALID;


    if (global_data_ptr->csn_dec_hdr_ptr->error_code != ADDRESS_INFO_ERROR)
    {


      mac_header_tfi_check = gmac_mac_control_header_tfi_checking(gas_id);
      if(mac_header_tfi_check == MAC_HEADER_TFI_NOT_VALID)
      {



        /*original code*/
        if(int_ul_tbf == TRUE)
        {
            if(packet_rel_ptr->global_tfi.direction == uplink_valid)
            {
               /*ul tbf rel*/
               if((packet_rel_ptr->global_tfi.uplink_tfi == global_data_ptr->gmac_local_tfi_store.ul_tfi)
                 && (global_data_ptr->gmac_local_tfi_store.ul_tfi_valid == TRUE))
               {
                  /*tfi match     */
                  rrbp_control_block_handler_2(PCA_UL_MSG,gas_id);

                   /* 0 = don't rel, 1 do rel tbf.*/
                  if(packet_rel_ptr->uplink_release == 1)
                  {
                    if(packet_rel_ptr->tbf_release_cause == 0x2)
                    {
                     /* abnormal ul tbf rel*/

                     /*When pkt tbf rel is rcvd during contetnion res, release L1 and*/
                     /*peform access retry if this fails then release RLC            */

                     release_ind = UL_TBF_ABNORMAL;
                     mac_log_ul_tbf_release(MAC_LOG_UL_TBF_RELEASE,gas_id);
                    }
                    else if(packet_rel_ptr->tbf_release_cause == 0x0)
                    {
                     /*normal ul tbf rel*/
                     /*inform rlc of release,once pdu boundary reched rlc will active release of tbf*/
                     gmac_util_send_ul_release_to_rlc(REL_UL_NORMAL,gas_id);
                     release_ind = UL_TBF_NORMAL;
                     mac_log_ul_tbf_release(MAC_LOG_UL_NORMAL_RELEASE,gas_id);
                    }
                  }
                  else
                  {
                    /*no dl to release*/
                    if(packet_rel_ptr->dwnlink_release == 1)
                    {
                      MSG_GERAN_LOW_0_G("MAC Info PKT TBF RELEASE no DL to rel");
                    }
                  }
               }
            }
            else if(packet_rel_ptr->global_tfi.direction == dwnlink_valid)
            {
              MSG_GERAN_LOW_0_G("MAC Info PKT TBF RELEASE no DL to rel");
            }
        }
        else
        {
          /* forced release of dl tbf*/
          if (packet_rel_ptr->global_tfi.direction == dwnlink_valid)
          {
            if ((packet_rel_ptr->global_tfi.dwnlink_tfi == global_data_ptr->gmac_local_tfi_store.dl_tfi)
                 && (global_data_ptr->gmac_local_tfi_store.dl_tfi_valid))
            {
                /* tfi_match */
                rrbp_control_block_handler_2(PCA_DL_MSG,gas_id);

               if ((packet_rel_ptr->tbf_release_cause == 0x2) &&
                    (packet_rel_ptr->dwnlink_release) &&
                    (packet_rel_ptr->global_tfi.direction == dwnlink_valid))
               {
                  /* DL TBF released so TFI no longer valid */
                  global_data_ptr->gmac_local_tfi_store.dl_tfi_valid = FALSE;
                  release_ind = DL_TBF_ABNORMAL;
                  mac_log_dl_tbf_release(MAC_LOG_DL_TBF_RELEASE,gas_id);
               }
            }
          }
          else if(packet_rel_ptr->global_tfi.direction == uplink_valid)
          {
            MSG_GERAN_LOW_0_G("MAC Info PKT TBF RELEASE no UL to rel");
          }
        }
        /*original code*/


      }
      else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_FOUND)
      {

        if(int_ul_tbf == TRUE)
        {
            /*UL tfi*/
            if((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[2] & 0x01 ) == 0x00)
            {
                /*tfi match     */
                rrbp_control_block_handler_2(PCA_UL_MSG,gas_id);

                 /* 0 = don't rel, 1 do rel tbf.*/
                if(packet_rel_ptr->uplink_release == 1)
                {
                  if(packet_rel_ptr->tbf_release_cause == 0x2)
                  {
                   /* abnormal ul tbf rel*/

                   /*When pkt tbf rel is rcvd during contetnion res, release L1 and*/
                   /*peform access retry if this fails then release RLC            */

                   release_ind = UL_TBF_ABNORMAL;
                   mac_log_ul_tbf_release(MAC_LOG_UL_TBF_RELEASE,gas_id);
                  }
                  else if(packet_rel_ptr->tbf_release_cause == 0x0)
                  {
                   /*normal ul tbf rel*/
                   /*inform rlc of release,once pdu boundary reched rlc will active release of tbf*/
                   gmac_util_send_ul_release_to_rlc(REL_UL_NORMAL,gas_id);
                   release_ind = UL_TBF_NORMAL;
                   mac_log_ul_tbf_release(MAC_LOG_UL_NORMAL_RELEASE,gas_id);
                  }
                }
                else
                {
                  /*no dl to release*/
                  if(packet_rel_ptr->dwnlink_release == 1)
                  {
                    MSG_GERAN_LOW_0_G("MAC Info PKT TBF RELEASE no DL to rel");
                  }
                }

            }
            else
            {
              MSG_GERAN_LOW_0_G("MAC Info PKT TBF RELEASE no DL to rel");
            }
        }
        else
        {
          /* forced release of dl tbf*/
          if((global_data_ptr->msg_ptr->l1_mac_sig.msg.data_ind.dl_ctrl_block[2] & 0x01 ) == 0x01)
          {
             /* tfi_match */
             rrbp_control_block_handler_2(PCA_DL_MSG,gas_id);

             if ((packet_rel_ptr->tbf_release_cause == 0x2) &&
                  (packet_rel_ptr->dwnlink_release) &&
                  (packet_rel_ptr->global_tfi.direction == dwnlink_valid))
             {
                /* DL TBF released so TFI no longer valid */
                global_data_ptr->gmac_local_tfi_store.dl_tfi_valid = FALSE;
                release_ind = DL_TBF_ABNORMAL;
                mac_log_dl_tbf_release(MAC_LOG_DL_TBF_RELEASE,gas_id);
             }
          }
          else
          {
            MSG_GERAN_LOW_0_G("MAC Info PKT TBF RELEASE no UL to rel");
          }
        }

      }
      else if (mac_header_tfi_check == MAC_HEADER_TFI_MATCH_NOT_FOUND)
      {
        release_ind = UL_AND_DL_TBF_ABNORMAL;
      }



    }
    else
    {
      MSG_GERAN_ERROR_0_G("MAC Error csn.1 address error");
    }

  return release_ind;
}


/*===========================================================================
===
===  FUNCTION      gmac_idle_abnormal_release_handler()
===
===  DESCRIPTION
===
===  No reception of L1_MAC_TBF_REL_CONFIRM, release GRR /RLC depending
===  on the current MAC ACTION
===
===  NOTE For the case where RLC has been blocked because of another MAC
===       ACTION, it MUSt be deblocked at this stage.

===
===   NOTE this function replaces gmac_idle_abnormal_access_release()
===   remoove this comment once swaping out task has been completed
===
===  DEPENDENCIES
===
===  RETURN VALUE
===
===  returns the blocked state of RLC
===
===  SIDE EFFECTS
===
===========================================================================*/
/*This function is called only when MAC initiated abnormal release*/
void gmac_idle_abnormal_release_handler
(
gprs_mac_action_t  current_mac_action,
gprs_mac_idle_substates_t current_mac_state,
gas_id_t gas_id
)
{
  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
  gmac_idle_data_t *idle_data_ptr = gmac_get_idle_data_ptr(gas_id);
  gmac_timers_t* mac_timers_ptr = gmac_get_timers_ptr(gas_id);

  /*mac_action of NONE infers that mac is in PASSIVE state*/
  if(current_mac_action != NONE)
  {
    MSG_GERAN_MED_0_G("MAC_GRR_MAC_STATUS");
    gmac_util_send_mac_grr_mac_status(&idle_data_ptr->mac_grr_sig,MAC_IDLE,gas_id);
  }
  switch (current_mac_action)
  {
    case UL_DATA_EST :

         /* If not doing an access retry reset PCCO */
         if (idle_data_ptr->access_attempts_count == 0)
         {
           gmac_util_complete_pcco_process(PCCO_FAILURE_NO_RESPONSE,MAC_INITIATED,GMAC_IDLEUTIL,__LINE__,gas_id);
           gmac_util_complete_pccf_process(FAILED_TO_SEND_PCCF,MAC_INITIATED,GMAC_IDLEUTIL,__LINE__,gas_id);
         }

         if( rex_get_timer(&mac_timers_ptr->timer_t3172) == 0 )
         {
           gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_IDLEUTIL,__LINE__,gas_id);
         }
         global_data_ptr->mac_mode = NULL_MODE;
    break;

    case PMR_EST :
         gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_IDLEUTIL,__LINE__,gas_id);

         MSG_GERAN_LOW_0_G("MAC_GRR_MEAS_RPT_CNF");
         gmac_util_send_pmr_cnf_to_grr(MAC_GRR_FAILURE,gas_id);
         global_data_ptr->mac_mode = NULL_MODE;
    break;

    case PCCF_EST:
         gmac_util_unblock_rlc(NO_CELL_CHANGE_NO_PURGE,GMAC_IDLEUTIL,__LINE__,gas_id);

         MSG_GERAN_LOW_0_G("MAC_GRR_MEAS_RPT_CNF");
         gmac_util_complete_pccf_process(FAILED_TO_SEND_PCCF,MAC_INITIATED,GMAC_IDLEUTIL,__LINE__,gas_id);
         global_data_ptr->mac_mode = NULL_MODE;
    break;

    default:
    break;
  }
}
/******************************************************************************
*                               EGPRS FUNCTIONS                               *
*******************************************************************************/

/*===========================================================================
===
===  FUNCTION      gmac_idle_egprs_pua_contention_res_checking()
===
===  DESCRIPTION
===
===  When a PUA is rcvd during contention resolution as part of the RAC procedure
===  if address MS correctly by the UL TFI and contains the CONTENTION_RESOLUTION_TLLI
===  field then perform contention on the TLLI.  ref 7.1.2.3
===
===  DEPENDENCIES
===
===  csn_dec_data_ptr
===
===  RETURN VALUE
===
===  return_value,    PUA_CONT_RES_NOT_APPLICABLE
===                   PUA_CONT_RES_FAILED
===                   PUA_CONT_RES_PASSED
===  SIDE EFFECTS
===
===========================================================================*/
pua_cont_res_t gmac_idle_egprs_pua_contention_res_checking(gprs_mac_idle_substates_t idle_substate,
                                                           gas_id_t gas_id)
{
  pua_cont_res_t return_value = PUA_CONT_RES_NOT_APPLICABLE;
  uint32         local_rrps_tlli = 0 ;

  packet_uplink_assignment_t *pua_data_ptr;

  mac_rlc_msg_t           *mr_sig_ptr;

  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  gmac_idle_data_t *idle_data_ptr = gmac_get_idle_data_ptr(gas_id);

  pua_data_ptr = (packet_uplink_assignment_t *)global_data_ptr->csn_dec_data_ptr;


  if(global_data_ptr->current_tbf_mode == TBF_MODE_EGPRS)
  {
    if(gmac_idle_check_pua_addressing(global_data_ptr->idle_mode_ul_tbf_release_action,idle_substate, gas_id ) == TRUE)
    {
      /*PUA is addressed to mobile.  Check if contention is resolved. (PUA is addressed by TFI and
        contention TLLI matches)*/        
      if(pua_data_ptr->cont_res_tlli_flag == valid)
      {
       
        if (pua_data_ptr->cont_res_tlli == global_data_ptr->gmac_store_ul_est_tlli)
        {
          mr_sig_ptr = grlc_get_cmd_buf_mac();

          if (mr_sig_ptr != NULL)
          {
            return_value = PUA_CONT_RES_PASSED;

            /**************************************************************************************
            *               load and send MAC_L1_CONTENTION_RES to l1                             *
            **************************************************************************************/

            /* reset idle access_attempts_count on move to transfer from cont res state. */
            idle_data_ptr->access_attempts_count = 0;

            MSG_GERAN_MED_0_G("MAC Sent MAC_L1_CONTENTION_RES");
            idle_data_ptr->mac_l1_sig.mac_l1_contention_res.message_header.message_id = (uint8)MAC_L1_CONTENTION_RES;
            idle_data_ptr->mac_l1_sig.mac_l1_contention_res.message_header.message_set = MS_MAC_L1;
            gprs_l1_mac_acc_func(&idle_data_ptr->mac_l1_sig, gas_id);

            memset(mr_sig_ptr, 0, sizeof(mac_rlc_msg_t));

            mr_sig_ptr->msg.ul_tlli_ind.ack_nack_info_flg = FALSE;

            /* send MR_UL_CONTENTION_IND message (containing ack/nack info) to RLC */
            MSG_GERAN_MED_0_G( "MAC sent MR_UL_CONTENTION_IND (TLLI in PUA resolved cont res) ");
            mr_sig_ptr->msg.ul_tlli_ind.sig = MR_UL_CONTENTION_IND;
            mr_sig_ptr->msg.ul_tlli_ind.contention_res = RM_CONTENTION_RESOLVED;
            gmac_rlc_mac_insert_gas_id(mr_sig_ptr,gas_id);
            grlc_put_cmd_mac(gas_id,mr_sig_ptr);
          }
          else
          {
            return_value = PUA_CONT_RES_FAILED;
            ERR_GERAN_FATAL_0_G("MAC ERROR failed to allocate message memory");
          }
        }
        else
        {
          MSG_GERAN_MED_2_G("MAC info PUA contention TLLI mismatch. PUA TLLI:0x%x, MS TLLI:0x%x",
                            pua_data_ptr->cont_res_tlli,local_rrps_tlli);
          return_value = PUA_CONT_RES_FAILED;
        }
      }
      else
      {
        MSG_GERAN_HIGH_0_G("MAC Rcvd PUA without contention res TLLI field");
        return_value = PUA_CONT_RES_NOT_APPLICABLE;
      }         
    }
    else
    {
      MSG_GERAN_MED_0_G("MAC info PUA address checking failed");
      return_value = PUA_CONT_RES_ADDRESS_CHECK_FAILED;
    }

    switch(return_value)
    {
      case PUA_CONT_RES_PASSED:
      case PUA_CONT_RES_NOT_APPLICABLE:

        if (pua_data_ptr->arac_retran_req == 1)
        {
          if ( (global_data_ptr->access_tech_req_present == TRUE) &&
               (global_data_ptr->c_flags.egprs_arac_sent == TRUE) )
          {
            MSG_GERAN_HIGH_0_G("GMAC info PUA request for ARAC retrans");
            gmac_retran_arac_msg(gas_id);
          }
          else
          {
            MSG_GERAN_ERROR_2_G("GMAC PUA req ARAC retran, ARAC not sent before, access_tech_req_present:%d, c_flags.egprs_arac_sent:%d ",
                                global_data_ptr->access_tech_req_present, 
                                global_data_ptr->c_flags.egprs_arac_sent);
          }
        }
        else
        {
          MSG_GERAN_HIGH_0_G("GMAC PUA rcvd, ARAC retrans not requested");
        }

      break;

      case PUA_CONT_RES_FAILED:
           /*******************************************************************
           * contention resolution failed, perform access retry if applicable *
           *******************************************************************/
           MSG_GERAN_MED_0_G("MAC info PUA rcvd with request for RAC info cont res failed, access retry");
      break;

      case PUA_CONT_RES_ADDRESS_CHECK_FAILED:
           /*******************************************************************
           * Address checking failed, PUA is not for MS                       *
           *******************************************************************/
           MSG_GERAN_MED_0_G("MAC info PUA address checking failed, ignore PUA");

      break;

      default:
      break;
    }

  }
  else
  {
    return_value = PUA_CONT_RES_NOT_APPLICABLE;
  }
  return(return_value);
}
/*changed 2p rac*/
/*===========================================================================
===
===  FUNCTION    pua_mb_handler()
===
===  Handles the PUA msg when received  in mac idle mode
===
===  To have rcvd a MB assigment the CEll must support EGPRS and EGPRS PRACHING also
===  MS must have sent an EGRPS PRACH and rcvd a PUA with EGPRS params containing a MB
===  allocation
===
===  DEPENDENCIES
===
===  global pointer to the msg data section of the csn.1 decode dl msg
===  global mac_l1_sig var used to store mac_l1_sb_tbf_config sig data
===
===  RETURN VALUE
===  An error value that indicates what action should be taken if the
===  data received in the PUA is erroneous
===
===  SIDE EFFECTS
===
===========================================================================*/

static pua_data_error_t pua_mb_handler(gprs_mac_action_t current_mac_action, gas_id_t gas_id )
{

  boolean egprs_capable_cell = FALSE;

  /*    result of checking pua            */
  pua_data_error_t  pua_result = NO_ERRORS;

  /*    csn.1 decoded pua                 */
  packet_uplink_assignment_t *pua_data_ptr;

  /*    mac_l1_sb_tbf_config sig       */
  mac_sb_tbf_information_T *ml_sb_tbf_info_ptr;

  gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

  gmac_idle_data_t *idle_data_ptr = gmac_get_idle_data_ptr(gas_id);

  /*     assign signal pointers           */
  pua_data_ptr = (packet_uplink_assignment_t *)global_data_ptr->csn_dec_data_ptr;
  ml_sb_tbf_info_ptr = &(idle_data_ptr->mac_l1_sig.mac_l1_single_block_config.sb_tbf);

  rr_read_public_store( RRPS_CELL_OPTIONS_EGPRS_CAPABLE_CELL, &egprs_capable_cell, gas_id );

  MSG_GERAN_MED_2_G("MAC info RAC data in PUA access_tech_req %d, arac_retran_req %d",
                    pua_data_ptr->access_technologies_req_flag,pua_data_ptr->arac_retran_req);

  /*set mac_l1_single_block_config to zero*/
  memset (ml_sb_tbf_info_ptr,
          0,
          sizeof(mac_sb_tbf_information_T));

  /******************************************************************************
  *            determine the number of blocks allocated in MB alloc             *
  ******************************************************************************/

  if(global_data_ptr->c_flags.egprs_arac_sent == FALSE)
  {

    if(pua_data_ptr->arac_retran_req == 0)
    {
      MSG_GERAN_HIGH_1_G("MAC MB alloc, blocks allocated %d",(pua_data_ptr->multi_blk_alloc.number_of_blks_alloc + 1));
      if(pua_data_ptr->multi_blk_alloc.number_of_blks_alloc == 0)
      {
        global_data_ptr->egprs_2p_sb_mb_alloc = MB_ALLOC_OF_1_BLOCK;
        ml_sb_tbf_info_ptr->num_rb_alloc = 1;
      }
      else if (pua_data_ptr->multi_blk_alloc.number_of_blks_alloc == 1)
      {
        global_data_ptr->egprs_2p_sb_mb_alloc = MB_ALLOC_OF_2_BLOCKS;
        ml_sb_tbf_info_ptr->num_rb_alloc = 2;
      }
      else
      {
        MSG_GERAN_HIGH_1_G("MAC Error in MB alloc,PUA ignored. Reserved blocks allocated %d",
                           pua_data_ptr->multi_blk_alloc.number_of_blks_alloc + 1);
        return(ASSIGNMENT_DATA_ERROR);
      }
    }
    else
    {
       MSG_GERAN_HIGH_1_G("MAC Error: Incorrect req for ARAC retran during EGPRS 2p access.PUA ignored. arac_retran_req %d",
                          pua_data_ptr->arac_retran_req);
       return(ASSIGNMENT_DATA_ERROR);
    }
  }
  else
  {

    /*2nd MB alloc if ARAC info was sent as part of a 2 block MB alloc and the network*/
    /*does not receive the ARAC msg then MS will be assigned another MB alloc BUT with only 1 */
    /*block allocated and indicated ARAC retrans . ref 4.60  7.1.3.2.1*/
    MSG_GERAN_HIGH_0_G("MAC info NW did not RCVD ARAC. when ARAC was sent as a MB alloc for 2 blocks");

    if( (pua_data_ptr->arac_retran_req == 1 )&&
        (pua_data_ptr->multi_blk_alloc.number_of_blks_alloc == 0))

    {
        global_data_ptr->egprs_2p_sb_mb_alloc = MB_ALLOC_OF_1_BLOCK;
        ml_sb_tbf_info_ptr->num_rb_alloc = 1;
    }
    else
    {
      if(pua_data_ptr->arac_retran_req == 0)
      {
        MSG_GERAN_HIGH_1_G("ERROR in ARAC retrans request,PUA ignored. arac_retran_req %d ",pua_data_ptr->arac_retran_req);
      }

      if(pua_data_ptr->multi_blk_alloc.number_of_blks_alloc !=0)
      {
        MSG_GERAN_HIGH_1_G("ERROR in ARAC retrans request,PUA ignored. incorrect num of blocks allocated %d",
                           (pua_data_ptr->multi_blk_alloc.number_of_blks_alloc +1));
      }

      return(ASSIGNMENT_DATA_ERROR);
    }
  }

  /******************************************************************************
  *                         Check if RAC info has been requested                *
  ******************************************************************************/

  if (pua_data_ptr->access_technologies_req_flag == valid)
  {
     /*store the Networks list of supported bands*/
     global_data_ptr->access_tech_req_present = (boolean)pua_data_ptr->access_technologies_req_flag;
     global_data_ptr->access_tech_req_count = pua_data_ptr->access_tech_reg_length;
     memset(global_data_ptr->access_tech_req,0,MAX_ACCESS_TECH_TYPE);
     memscpy(global_data_ptr->access_tech_req ,sizeof(global_data_ptr->access_tech_req),
             pua_data_ptr->access_technologies_req,MAX_ACCESS_TECH_TYPE);
  }
  else
  {
     global_data_ptr->access_tech_req_present = (boolean)pua_data_ptr->access_technologies_req_flag;
  }

  global_data_ptr->c_flags.egprs_rac_retran_req = pua_data_ptr->arac_retran_req;

  /*****************************************************************************
  *                                  BEP PERIOD                                *
  ******************************************************************************/
  if(pua_data_ptr->bep_period2_flag == valid)
  {
    ml_sb_tbf_info_ptr->bep_period2.bep_period2 = pua_data_ptr->bep_period2;
    ml_sb_tbf_info_ptr->bep_period2.present = TRUE;
  }
  else
  {
    ml_sb_tbf_info_ptr->bep_period2.present = FALSE;
  }

  /*determine the RLC tbf access phase etiher 1 phase,2phase or short access*/

  if((idle_data_ptr->access_granted == two_phase) ||(idle_data_ptr->access_granted == one_phase))
  {
    ml_sb_tbf_info_ptr->two_phase_access = TRUE;
  }
  else
  {
    ml_sb_tbf_info_ptr->two_phase_access = FALSE;
  }

  if(current_mac_action == PMR_EST)
  {
    ml_sb_tbf_info_ptr->meas_report = TRUE;
    ml_sb_tbf_info_ptr->nc_non_drx_period = global_data_ptr->gmac_local_pmr_params.nc_non_drx_period;
    ml_sb_tbf_info_ptr->nco = global_data_ptr->gmac_local_pmr_params.nco;
  }
  else
  {
    ml_sb_tbf_info_ptr->meas_report = FALSE;
  }

  ml_sb_tbf_info_ptr->ts_number = pua_data_ptr->multi_blk_alloc.timeslot_num;  /*sngl_blk_alloc.timeslot_num;*/

  if(pua_data_ptr->multi_blk_alloc.alpha_gamma_flag == valid)
  {
      ml_sb_tbf_info_ptr->alpha_present = TRUE;
      ml_sb_tbf_info_ptr->alpha  = pua_data_ptr->multi_blk_alloc.alpha;
  }
  else
  {
      ml_sb_tbf_info_ptr->alpha_present = FALSE;
  }

  ml_sb_tbf_info_ptr->gamma_tn = pua_data_ptr->multi_blk_alloc.gamma_tn;

  gmac_pua_dl_pwr_control(ml_sb_tbf_info_ptr, gas_id);

  /*******************************************************************
  *                    timing_advance_params                         *
  *******************************************************************/
  (void)gmac_packet_ta_ie( &(pua_data_ptr->pkt_timing_adv),
                           &(ml_sb_tbf_info_ptr->timing_advance_params),
                           gas_id );

  /*update local "gmac_update_local_ta_params" with ta info */
  (void)gmac_update_local_ta_params(&(pua_data_ptr->pkt_timing_adv), gas_id);

  /********************************************************************
  *                        starting_time                              *
  *********************************************************************/
  if (gmac_tbf_starting_ie(&(pua_data_ptr->multi_blk_alloc.tbf_start_frm_num),
                           &(ml_sb_tbf_info_ptr->starting_time),
                             gas_id)
                           == TBF_STARTING_TIME_ERROR_ACCESS_RETRY)
  {
     /*tbf starting time error exit, return error code*/
     /* exit pua handler                              */
     return(TBF_STARTING_TIME_ERROR_ACCESS_RETRY);
  }

  /*indicate that EGPRS params are present in the MAC_L1_SINGLE_BLOCK_CONFIG*/
  ml_sb_tbf_info_ptr->tbf_mode = TBF_MODE_EGPRS;

#ifdef FEATURE_GSM_DTM
  global_data_ptr->gmac_dtm_cs_ts_alloc = GMAC_NO_TS_ALLOCATED_IN_BITMAP;
#endif /*FEATURE_GSM_DTM*/

  return(pua_result);
}

/*===========================================================================
===
===  FUNCTION    gmac_idle_send_no_access_to_rlc()
===
===  DESCRIPTION
===  This functin will block RLC and also sends NO_PS_ACCESS IND with delete/no_delete.
===  Delete/no_delete will be determined depending upon number of access attemps or if PCCO
===  procedure is on GMAC always sends Delete
===
===  PARAMS
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_idle_send_no_access_to_rlc(gas_id_t gas_id)
{
    gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);
    gmac_idle_data_t *idle_data_ptr = gmac_get_idle_data_ptr(gas_id);

    if(global_data_ptr->mac_action == UL_DATA_EST)
    {
      if(gmac_util_get_pcco_state(gas_id) == NO_PCCO_ACTIVE)
      {
        if(idle_data_ptr->access_attempts_count < 4)
        {
          MSG_GERAN_MED_1_G( "MAC has not reached maximum number of access attempts and access_attempts_count value is %d",
                             idle_data_ptr->access_attempts_count);
          idle_data_ptr->access_attempts_count ++;
          gmac_util_block_rlc(NO_DEL,GMAC_IDLEUTIL,__LINE__,gas_id);
        }
        else
        {
          MSG_GERAN_MED_0_G( "MAC has reached maximum number of access attempts so resetting access_attempts_count value is ");
          idle_data_ptr->access_attempts_count = 0;
          gmac_util_block_rlc(DEL_ONE,GMAC_IDLEUTIL,__LINE__,gas_id);
        }
      }
      else
      {
        if (idle_data_ptr->access_attempts_count < 4)
        {
          MSG_GERAN_MED_1_G( "MAC has not reached maximum number of access attempts and access_attempts_count value is %d",
                             idle_data_ptr->access_attempts_count);
          idle_data_ptr->access_attempts_count ++;
          gmac_util_block_rlc(NO_DEL,GMAC_IDLEUTIL,__LINE__,gas_id);
        }
        else
        {
          if(global_data_ptr->gmac_util_data_req_cause == GRR_CAUSE_GMM_CELL_UPDATE)
          {
            /*PCCO is ACTIVE and the establishment cause is CELL UPDATE. So GRLC has to
              delete the cell update. so GMAC will send DEL ONE to GRLC*/
            gmac_util_block_rlc(DEL_ONE,GMAC_IDLE,__LINE__,gas_id);
          }
          else
          {
            /*PCCO ACTIVE and the establishment cause is not CELL UPDATE. So GMAC will
              sends NO DEL to GRLC*/
            gmac_util_block_rlc(NO_DEL,GMAC_IDLE,__LINE__,gas_id);
          }
          idle_data_ptr->access_attempts_count = 0;
        }
      }
    }
}
/*===========================================================================
===
===  FUNCTION    gmac_idle_stop_access_timers()
===
===  DESCRIPTION
===  This function will stop the access timers depending upon the mac idle substate
===
===  PARAMS
===   mac_idle_substate --- This is the current mac idle sub state
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_idle_stop_access_timers(gprs_mac_idle_substates_t mac_idle_substate,
                                  gas_id_t gas_id)
{
  switch(mac_idle_substate)
  {
      case UL_1P_ASS:
           MSG_GERAN_MED_0_G( "GMAC is in UL_1P_ASS state so stopping T3164 timer ");
           gmac_stop_t3164(gas_id);
      break;

      case UL_2P_ASS:
           MSG_GERAN_MED_0_G( "GMAC is in UL_2P_ASS state so stopping T3168 timer ");
           gmac_stop_t3168(gas_id);
      break;

      case UL_1P_CONTENTION_RES:
           MSG_GERAN_MED_0_G( "GMAC is in UL_1P_CONTENTION_RES state so stopping T3164, T3166 and T3180 timers ");
           gmac_stop_t3164(gas_id);
           gmac_stop_t3166(gas_id);
           gmac_stop_t3180(gas_id);
           gmac_stop_t3168(gas_id);
      break;
      case DL_ASS:
           MSG_GERAN_MED_0_G( "GMAC no need to stop any timers in DL_ASS state");
      break;

      default:
           MSG_GERAN_HIGH_1_G( "NO action in state %d", mac_idle_substate);
      break;
  }
}

/*===========================================================================
===
===  FUNCTION    gmac_idle_send_del_one_or_no_del_to_rlc()
===
===  DESCRIPTION
===  This functin will send del or no del to rlc depending upon pcco active or not.
===  if pcco is active and establishment cause is GMM CELL UPDATE, gmac will send
===  del one to RLC. For all other establishment causes gmac will send NO DEL to GRLC.
===  if pcco is not active, gmac will always sends DEL ONE to GRLC
===
===  PARAMS
===
===
===  RETURN VALUE
===
===
===  SIDE EFFECTS
===
===
===========================================================================*/
void gmac_idle_send_del_one_or_no_del_to_rlc(gas_id_t gas_id)
{
    gmac_global_data_t *global_data_ptr = gmac_get_global_data_ptr(gas_id);

    if(gmac_util_get_pcco_state(gas_id) == PCCO_ACTIVE)
    {
      if(global_data_ptr->gmac_util_data_req_cause == GRR_CAUSE_GMM_CELL_UPDATE)
      {
        /*PCCO is ACTIVE and the establishment cause is CELL UPDATE. So GRLC has to
          delete the cell update. so GMAC will send DEL ONE to GRLC*/
        gmac_util_block_rlc(DEL_ONE,GMAC_IDLE,__LINE__,gas_id);
      }
      else
      {
        /*PCCO is ACTIVE and the establishment cause is not CELL UPDATE. So GMAC will
          sends NO DEL to GRLC*/
        gmac_util_block_rlc(NO_DEL,GMAC_IDLE,__LINE__,gas_id);
      }
    }
    else
    {
      /*NO PCCO is ACTIVE so always send DEL ONE to GRLC*/
      gmac_util_block_rlc(DEL_ONE,GMAC_IDLE,__LINE__,gas_id);
    }
}
/*** EOF: don't remove! ***/
