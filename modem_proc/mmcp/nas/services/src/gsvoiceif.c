/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/gsvoiceif.c_v   1.6   12 Jun 2002 12:04:04   cdealy  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/services/src/gsvoiceif.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/11/01   TS      Added GSM build support.
06/22/01   TS      Added UMTS WVS support.
11/21/01   TS      Added FEATURE define for references to 
                   wvs functions and includes.
                   Added GSM loopback call.

01-Apr-2002  VT    GSM: Coded the function ghdi_control_tch_gating() to
                     switch ON/OFF the FR/EFR vocoder, as required.
                   GSM: In ghdi_control_tch_gating(), replaced all ERR calls
                     with MSG_ERROR calls. Now, voc_set_gsm_enc_dtx() is 
                     called with VOC_GSM_DTX_OFF instead of VOC_GSM_DTX_ON.
                   Actually merged the above changes from rev 1.3.1.1 (branch) 
                     of this file.

02-Apr-2002  VT    GSM: Merged 1 missing change from rev 1.3.1.1 (including
                     voc.h) of this file.

06/03/02    CD     Added/modified msg logging
                   In ghdi_control_tch_gating added checks for function call returns

09/11/02    ymc    Added MVS support.

09/24/02    AB     Added MVS vocoder interface using YM's sample code.  The 
                   additional function needed to inform L1 to stop vocoder
                   data blanking will be added later when an interface is 
                   defined.

10/17/02    ymc    Moved DTX set mode out of MVS call back function. 

02/26/02    AB     Added supported for GSM AMR & Half Rate MVS configuration.

03/10/02    AB     Added new function to set the GSM DTX mode.

03/21/03    AB     Changed the feature MSMAUD_VOC_HR to FEATURE_GSM_HALF_RATE.

03/25/03    AB     Changed the GSM uplink and downlink packet status tracking 
                   to avoid flooding the F3 messages.

4/15/03     DP     Added gl1_msg_vocoder_released() to notify L1 that the
                   the MVS is in transition.

05/06/03    AB     Do not turn DTX mode off when enable any GSM vocoder.

01/14/04    DP     Added addtional checking for GSM vocoder blanking().

08/23/04    DP     Removed call to gl1_msg_start_vocoder_blanking as this is
                   now handled by L1

08/24/04    GS     Replaced the l1_msg_tch_loopback() with l1_tch_loopback()
                      for voice loopback in GSM mode.

11/09/04    AB     Update the new mvs_enable() interface function with addtional
                      additional parameters,MVS_PKT_CONTEXT_ISR.  Also, Branch out 
                      previous l1_msg_tch_loopback() changes. 
                    
01/07/05    AB     Added supports for UMTS inter-RAT handover control optimzation
                      at GSM L1, and RRC, FEATURE_INTER_RAT_HO_OPT.

03/22/05    AB     Added error recovery procedure and prevention measure to avoid
                      MVS acquisition failure.

04/12/05    AB     Added MVS_CLIENT_QVP to error prevention procedure, MVS 
                      re-acquire counter, and polled vocoder control flag on 
                      every possible task context.  Also,  Added supports for 
                      VT & Voice call switching feature.

02/01/06    ymc    Fixed the gl1_rx_pkt.semaphore in ghdi_mvs_gsm_dl_proc(), 
                      to avoid skipping GSM DL packets that are arrived too fast. 

11/29/06    NR     Fixing the no audio issue after W2G handover

03/02/07    NR     Adding changes for VCC feature under feature flag FEATURE_IMS_VCC

03/05/06    NR     Adding changes for WB_AMR

04/25/08    VSM  Guarding the QVP and VOIP Clients Support using the Feature Flag 
                 FEATURE_NAS_NO_QVP_VOIP_SUPPORT_VIA_MVS -- for 8K purpose.

11/06/10    VSM  MVS_CLIENT_QVP_TEST is now used instead of MVS_CLIENT_QVP 
12/14/11     jbk    Free floating changes for gscvoice library
07/24/12    pm    Replacing FEATURE_IMS_VCC with FEATURE_VOIP 
11/28/12    am    Fixing compilation warning 
===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "timers_v.h"
#include "gs_v.h"
#include "ghdi_exp.h"
#include "ghdi_exp_int.h"
#include "err.h"
#include "naslog_v.h"

#ifdef FEATURE_GSM
#include "gl1_msg.h"
#endif  /* FEATURE_GSM */

#include "mvs.h"
#include "mvssup.h"
#include "atomic_ops.h"

#include "geran_ghdi_api.h"
#include "rrcmnif.h"
#include "rrcdata.h"
#include "geran_test_mode_api.h"

#include "geran_grr_api.h"

#ifdef FEATURE_TDSCDMA
#include "tds_rrc_api.h"
#include "tdsrrcdataext.h"
#endif  /* FEATURE_TDSCDMA */
#ifdef FEATURE_SEGMENT_LOADING
#include "mcfg_seg_load.h"
#include "ITDSCDMA.h"
#include "IWCDMA.h"
#include "IWCDMA_ext.h"
#endif

#include "voicecfg_api.h"
#include "apr_errcodes.h"


/* Definitions used to control Multimode Voice Services call back function */

typedef enum {
  GHDI_MVS_FAILED,
  GHDI_MVS_OFF,
  GHDI_MVS_ACQUIRE,
  GHDI_MVS_CHANGING,
  GHDI_MVS_MODE_SELECT,
  GHDI_MVS_STANDBY,
  GHDI_MVS_MODE_READY,
  GHDI_MVS_ON
} ghdi_mvs_status_type;



typedef enum {
  GHDI_MVS_INTERFACE_0, /* Old GHDI-MVS interface */
  GHDI_MVS_INTERFACE_1  /* New GHDI-VS interface */
} ghdi_mvs_interface_type;


typedef enum {
  EVENT_MVS_NULL,
  EVENT_MVS_RELEASE_CALLED,
  EVENT_MVS_RELEASE_CALLBACK_RECD,
  EVENT_MVS_ACQUIRE_CALLED,
  EVENT_MVS_ACQUIRE_CALLBACK_RECD,
  EVENT_MVS_ENABLE_CALLED,
  EVENT_MVS_ENABLE_CALLBACK_RECD,
  EVENT_MVS_STANDBY_CALLED,
  EVENT_MVS_STANDBY_CALLBCK_RECD,
  EVENT_MVS_CONFIGURE_MVS_DL_CHANNEL,
  EVENT_MVS_CONFIGURE_MVS_UL_CHANNEL,
  EVENT_MVS_ENABLE_CALLBACK_MODE_INIT,
  EVENT_MVS_ENABLE_CALLBACK_MODE_READY,
  EVENT_MVS_ENABLE_CALLBACK_MODE_NOT_AVAIL,
  EVENT_MVS_ON_CALLBACK_MODE_READY,
  EVENT_MVS_CALLBACK_FAILURE_RECD,
  EVENT_MVS_CALLBACK_BUSY_RECD
} ghdi_mvs_qxdm_event_type;




#if defined(FEATURE_DUAL_SIM)
ghdi_mvs_interface_type ghdi_get_MVS_interface_val(uint32 vsid);
#endif

void ghdi_mvs_acquire(mvs_client_type);
#ifdef FEATURE_CS_VOICE_OVER_HSPA              
static mvs_packet_ul_fn_type ghdi_CSovrHSPA_ul_fn;
static mvs_packet_dl_fn_type ghdi_CSovrHSPA_dl_fn;
void ghdi_mvs_enable(mvs_client_type , mvs_mode_type , umts_wcdma_cs_voice_bearer_enum_T );
#else
void ghdi_mvs_enable(mvs_client_type, mvs_mode_type);
#endif
void ghdi_mvs_release(mvs_client_type);

void ghdi_mvs_standby(mvs_client_type client, mvs_mode_type mode);
boolean ghdi_is_acquisition_in_progress(void);

void ghdi_reset_mvs_callback_flag(void);
void gl1_msg_tch_loopback(gl1_defs_loopback_type loopback);

extern uint32        mn_asid_2_vsid_map[MAX_AS_IDS] ;
extern sys_modem_as_id_e_type ghdi_get_mmode_subs_asid(void);


#ifdef FEATURE_INTER_RAT_HO_OPT
static boolean print_vocoders_enabled_combined (void)
{
#ifdef FEATURE_SEGMENT_LOADING 
  interface_t *ptr_W = get_wcdma_interface();
  interface_t *ptr_T = get_tdscdma_interface();

#ifdef FEATURE_TDSCDMA
#if defined FEATURE_DUAL_SIM
             if (gl1_msg_is_l1_vocoder_control_enabled_multisim(ghdi_get_mmode_subs_asid()) ||
                 ((ptr_W != NULL)&&IWCDMA_rrc_vocoder_control_enabled_sub(ptr_W,ghdi_get_mmode_subs_asid())) ||
                 ((ptr_T != NULL)&&ITDSCDMA_tdsrrc_vocoder_control_enabled_sub(ptr_T,ghdi_get_mmode_subs_asid()))) /* possible context switched; check flag again */
              {
                  MSG_HIGH_3("GL1 = %d or RRC = %d or TDSRRC = %d vocoder control",
                            gl1_msg_is_l1_vocoder_control_enabled_multisim(ghdi_get_mmode_subs_asid()),
                            (ptr_W !=NULL)&&IWCDMA_rrc_vocoder_control_enabled_sub(ptr_W,ghdi_get_mmode_subs_asid()),
                            (ptr_T != NULL)&&ITDSCDMA_tdsrrc_vocoder_control_enabled_sub(ptr_T,ghdi_get_mmode_subs_asid())); 
                  return TRUE;
              }  
#else
             if (gl1_msg_is_l1_vocoder_control_enabled() ||
                  ((ptr_W != NULL)&&IWCDMA_rrc_vocoder_control_enabled(ptr_W)) ||
                  ((ptr_T != NULL)&&ITDSCDMA_tdsrrc_vocoder_control_enabled(ptr_T)))  /* possible context switched; check flag again */
              {
                  MSG_HIGH_3("GL1 = %d or RRC = %d or TDSRRC = %d vocoder control",
                            gl1_msg_is_l1_vocoder_control_enabled(),
                            (ptr_W !=NULL)&&IWCDMA_rrc_vocoder_control_enabled(ptr_W),
                            (ptr_T != NULL)&&ITDSCDMA_tdsrrc_vocoder_control_enabled(ptr_T)); 
                  return TRUE;
              }  
#endif  /* FEATURE_DUAL_SIM */

#else /* else FEATURE_TDSCDMA */

#if defined FEATURE_DUAL_SIM
             if (gl1_msg_is_l1_vocoder_control_enabled_multisim(ghdi_get_mmode_subs_asid()) ||
                 ((ptr_W != NULL)&&IWCDMA_rrc_vocoder_control_enabled_sub(ptr_W,ghdi_get_mmode_subs_asid())))  /* possible context switched; check flag again */
              {
                  MSG_HIGH_2("GL1 = %d or RRC = %d  vocoder control",
                            gl1_msg_is_l1_vocoder_control_enabled_multisim(ghdi_get_mmode_subs_asid()),
                            (ptr_W !=NULL)&&IWCDMA_rrc_vocoder_control_enabled_sub(ptr_W,ghdi_get_mmode_subs_asid()));
                  return TRUE;
              }  
#else
             if (gl1_msg_is_l1_vocoder_control_enabled() ||
                  ((ptr_W != NULL)&&IWCDMA_rrc_vocoder_control_enabled(ptr_W)))  /* possible context switched; check flag again */
              {
                  MSG_HIGH_2("GL1 = %d or RRC = %d  vocoder control",
                            gl1_msg_is_l1_vocoder_control_enabled(),
                            (ptr_W !=NULL)&&IWCDMA_rrc_vocoder_control_enabled(ptr_W)); 
                  return TRUE;
              }  
#endif  /* FEATURE_DUAL_SIM */

#endif /* FEATURE_TDSCDMA */

#else /* else FEATURE_SEGMENT_LOADING */

#ifndef FEATURE_TDSCDMA
#if defined FEATURE_DUAL_SIM
              if(gl1_msg_is_l1_vocoder_control_enabled_multisim(ghdi_get_mmode_subs_asid()) || 
                 rrc_vocoder_control_enabled_sub(ghdi_get_mmode_subs_asid()))  /* possible context switched; check flag again */
              {
                  MSG_HIGH_2("GL1 = %d or RRC = %d vocoder control=%d", gl1_msg_is_l1_vocoder_control_enabled_multisim(ghdi_get_mmode_subs_asid()),
                                                                       rrc_vocoder_control_enabled_sub(ghdi_get_mmode_subs_asid())); 
                  return TRUE;
              } 
#else
              if(gl1_msg_is_l1_vocoder_control_enabled() || rrc_vocoder_control_enabled())  /* possible context switched; check flag again */
              {
                  MSG_HIGH_2("GL1 = %d or RRC = %d vocoder control=%d", gl1_msg_is_l1_vocoder_control_enabled(),
                                                                   rrc_vocoder_control_enabled()); 
                  return TRUE;
              } 
#endif  /* FEATURE_DUAL_SIM */

#else /* else FEATURE_TDSCDMA */

#if defined FEATURE_DUAL_SIM
              if (gl1_msg_is_l1_vocoder_control_enabled_multisim(ghdi_get_mmode_subs_asid()) ||
                  rrc_vocoder_control_enabled_sub(ghdi_get_mmode_subs_asid()) ||
                   tdsrrc_vocoder_control_enabled_sub(ghdi_get_mmode_subs_asid()))  /* possible context switched; check flag again */
              {
                  MSG_HIGH_3("GL1 = %d or RRC = %d or TDSRRC = %d vocoder control",
                            gl1_msg_is_l1_vocoder_control_enabled_multisim(ghdi_get_mmode_subs_asid()),
                            rrc_vocoder_control_enabled_sub(ghdi_get_mmode_subs_asid()),
                             tdsrrc_vocoder_control_enabled_sub(ghdi_get_mmode_subs_asid())); 
                  return TRUE;
              } 
#else
              if (gl1_msg_is_l1_vocoder_control_enabled() ||
                  rrc_vocoder_control_enabled() ||
                  tdsrrc_vocoder_control_enabled())  /* possible context switched; check flag again */
              {
                  MSG_HIGH_3("GL1 = %d or RRC = %d or TDSRRC = %d vocoder control",
                            gl1_msg_is_l1_vocoder_control_enabled(),
                            rrc_vocoder_control_enabled(),
                            tdsrrc_vocoder_control_enabled()); 
                  return TRUE;
              } 
#endif  /* FEATURE_DUAL_SIM */  

#endif /* FEATURE_TDSCDMA */
#endif /* FEATURE_SEGMENT_LOADING */
             else
             {
               return FALSE;
             }
}

static boolean print_vocoders_enabled_single (void)
{
       boolean retval = TRUE;
#ifdef FEATURE_SEGMENT_LOADING      
       interface_t *ptr_W = get_wcdma_interface();
       interface_t *ptr_T = get_tdscdma_interface();

#if defined FEATURE_DUAL_SIM
       if(gl1_msg_is_l1_vocoder_control_enabled_multisim(ghdi_get_mmode_subs_asid()))
       {
           MSG_HIGH_1("GSM L1 controlling the vocoder = %d", gl1_msg_is_l1_vocoder_control_enabled_multisim(ghdi_get_mmode_subs_asid()));
       }
#else
       if(gl1_msg_is_l1_vocoder_control_enabled())
       {
           MSG_HIGH_1("GSM L1 controlling the vocoder = %d", gl1_msg_is_l1_vocoder_control_enabled());
       }
#endif

#if defined FEATURE_DUAL_SIM
       else if((ptr_W != NULL)&&(IWCDMA_rrc_vocoder_control_enabled_sub(ptr_W,ghdi_get_mmode_subs_asid())))
       {
           MSG_HIGH_1("RRC controlling the vocoder = %d", IWCDMA_rrc_vocoder_control_enabled_sub(ptr_W,ghdi_get_mmode_subs_asid()));
       }
#else
       else if((ptr_W != NULL)&&(IWCDMA_rrc_vocoder_control_enabled(ptr_W)))
       {
           MSG_HIGH_1("RRC controlling the vocoder = %d", IWCDMA_rrc_vocoder_control_enabled(ptr_W));
       }
#endif /* FEATURE_DUAL_SIM */

#ifdef FEATURE_TDSCDMA
#if defined FEATURE_DUAL_SIM
       else if ((ptr_T != NULL)&&(ITDSCDMA_tdsrrc_vocoder_control_enabled_sub(ptr_T,ghdi_get_mmode_subs_asid())))
       {
           MSG_HIGH_1( "TDSRRC controlling the vocoder = %d", ITDSCDMA_tdsrrc_vocoder_control_enabled_sub(ptr_T,ghdi_get_mmode_subs_asid()));
       }
#else
       else if ((ptr_T != NULL)&&(ITDSCDMA_tdsrrc_vocoder_control_enabled(ptr_T)))
       {
           MSG_HIGH_1( "TDSRRC controlling the vocoder = %d", ITDSCDMA_tdsrrc_vocoder_control_enabled(ptr_T));
       }
#endif /* FEATURE_DUAL_SIM */
#endif /* FEATURE_TDSCDMA */

#else

#if defined FEATURE_DUAL_SIM
       if(gl1_msg_is_l1_vocoder_control_enabled_multisim(ghdi_get_mmode_subs_asid()))
       {
           MSG_HIGH_1("GSM L1 controlling the vocoder = %d", gl1_msg_is_l1_vocoder_control_enabled_multisim(ghdi_get_mmode_subs_asid()));
       }
#else
       if(gl1_msg_is_l1_vocoder_control_enabled())
       {
           MSG_HIGH_1("GSM L1 controlling the vocoder = %d", gl1_msg_is_l1_vocoder_control_enabled());
       }
#endif /* FEATURE_DUAL_SIM */

#if defined FEATURE_DUAL_SIM
       else if(rrc_vocoder_control_enabled_sub(ghdi_get_mmode_subs_asid()))
       {
           MSG_HIGH_1("RRC controlling the vocoder = %d", rrc_vocoder_control_enabled_sub(ghdi_get_mmode_subs_asid()));
       }
#else
       else if(rrc_vocoder_control_enabled())
       {
           MSG_HIGH_1("RRC controlling the vocoder = %d", rrc_vocoder_control_enabled());
       }
#endif /* FEATURE_DUAL_SIM */

#ifdef FEATURE_TDSCDMA

#if defined FEATURE_DUAL_SIM
       else if (tdsrrc_vocoder_control_enabled_sub(ghdi_get_mmode_subs_asid()))
       {
           MSG_HIGH_1( "TDSRRC controlling the vocoder = %d", tdsrrc_vocoder_control_enabled_sub(ghdi_get_mmode_subs_asid()));
       }
#else
       else if (tdsrrc_vocoder_control_enabled())
       {
           MSG_HIGH_1( "TDSRRC controlling the vocoder = %d", tdsrrc_vocoder_control_enabled());
       }
#endif /* FEATURE_DUAL_SIM */
#endif /* FEATURE_TDSCDMA */
#endif /* FEATURE_SEGMENT_LOADING */
       else
       {
           retval = FALSE;
       }
       return retval;
}
#endif

/* MVS control variables.  TBD: need to provides access fucntion for these */
ghdi_mvs_status_type  ghdi_mvs_status = GHDI_MVS_OFF;

mvs_mode_type         ghdi_mvs_mode = MVS_MODE_NONE;

/* is_voice_supported - Determines whether voice is supported or not
 * NAS takes that decision after reading voice API voicecfg_call() 
 * */

boolean               is_voice_supported = FALSE;
static void           ghdi_is_voice_supported(void);


mvs_packet_ul_fn_type ghdi_mvs_ul_proc = NULL;
mvs_packet_dl_fn_type ghdi_mvs_dl_proc = NULL;

#ifdef FEATURE_GSM
mvs_dtx_mode_type     ghdi_dtx_mode = MVS_DTX_OFF;
#endif

#define GHDI_MAX_NO_MVS_ACQUIRE                  2

byte                  ghdi_mvs_acquire_counter = 0;

#ifdef FEATURE_ENHANCED_AMR
/* GHDI cache to store the sampling rate of current voice call's codec 
 * Based on sampling rate in use, AS can determine if eAMR is ON or OFF.
 * If codec is narrow band and sampling rate is 16000 Hz, eAMR is ON,
 * else OFF.
 * */
uint32                speech_enc_samp_freq = GHDI_EAMR_SR_8000;
#endif

mvs_client_type  vocoder_acquired_by_client = MVS_CLIENT_NONE;
boolean  mvs_acquire_pending = FALSE;       /* acquire command buffered */
atomic_word_t  mvs_callback_pending;   /* command posted to MVS, waiting for CB */
boolean  mvs_enable_pending = FALSE;   /* enable command buffered */
boolean  mvs_standby_pending = FALSE;   /* standby command buffered */
mvs_client_type enable_pending_client_id = MVS_CLIENT_NONE; /* client for whom enable is buffered */
mvs_client_type standby_pending_client_id = MVS_CLIENT_NONE; /* client for whom standby is buffered */
mvs_mode_type enable_pending_mode_id = MVS_MODE_NONE; /* mode of the buffered enable */
mvs_mode_type standby_pending_mode_id = MVS_MODE_NONE; /* mode of the buffered enable */
atomic_word_t mvs_clients_to_be_released;       /* clients for whom release command is buffered */
mvs_client_type acquire_pending_client_id = MVS_CLIENT_NONE;  /* client for whom acquire is buffered */
#ifdef FEATURE_CS_VOICE_OVER_HSPA
umts_wcdma_cs_voice_bearer_enum_T enable_pending_rab_type = UMTS_WCDMA_CS_VOICE_OVER_INVALID;
                                                                        /* rab_type of the buffered enable */ 
#endif                                                                        

#ifdef FEATURE_MVS_UNDER_GHDI
/* Modem-MVS i/f MVS Configuration Variables */
static  rlc_lc_id_type ul_lc_class_a, ul_lc_class_b, ul_lc_class_c;
static  mvssup_wcdma_chan_class_type ul_chan_class;
    
static  rlc_lc_id_type dl_lc_class_a, dl_lc_class_b, dl_lc_class_c;
static  mvssup_wcdma_chan_class_type  dl_chan_class;

static mvs_amr_mode_type ghdi_amr_wb_ul_mode;
static  boolean ghdi_amr_wb_ul_mode_config_modified = FALSE;

static mvs_amr_mode_type ghdi_amr_ul_mode;
static  boolean ghdi_amr_ul_mode_config_modified = FALSE;

static mvs_scr_mode_type ghdi_amr_scr_ul_mode;

void ghdi_configure_mvs_wcdma_ul_channel (void);
void ghdi_configure_mvs_wcdma_dl_channel (void);
#ifdef FEATURE_TDSCDMA
static  mvssup_umts_chan_class_type umts_ul_chan_class;
static  mvssup_umts_chan_class_type umts_dl_chan_class;
void ghdi_configure_mvs_tdscdma_ul_channel(void);
void ghdi_configure_mvs_tdscdma_dl_channel(void);
#endif /* FEATURE_TDSCDMA */

#ifdef FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING
extern int rrc_register_mvs_callback_to_l1(mvssup_wcdma_blk_size_cb_set_type *);
#ifdef FEATURE_TDSCDMA
extern int tdsrrc_register_mvs_callback_to_l1(mvssup_wcdma_blk_size_cb_set_type *);
#endif /* FEATURE_TDSCDMA */
static  mvssup_wcdma_blk_size_cb_set_type dl_blk_size_cb_set;
#endif  /* FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING */
#endif /* FEATURE_MVS_UNDER_GHDI */

#ifdef FEATURE_CS_VOICE_OVER_HSPA
mvs_packet_ul_fn_type ghdi_mvs_CSovrHSPA_ul_proc;
mvs_packet_dl_fn_type ghdi_mvs_CSovrHSPA_dl_proc;
#endif

nas_crit_sect_type ghdi_mvs_crit_sec;

/*===========================================================================

FUNCTION GSSEND_EVENT_GHDI_MVS_STATE

DESCRIPTION
  This function sends the MM state log packet to DIAG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ghdi_send_event_ghdi_mvs_state
(
ghdi_mvs_qxdm_event_type event,
mvs_client_type client,
ghdi_mvs_status_type status,
mvs_mode_type mode
)
{

   byte event_packet[4];

   event_packet[0] = (byte)event;
   event_packet[1] = (byte)client;
   event_packet[2] = (byte)status;
   event_packet[3] = (byte)mode;

   event_report_payload(EVENT_GHDI_MVS_STATE, 4, (void *)event_packet);


}

/*******************************************************************************
 *
 *  Function name:  ghdi_is_voice_supported()
 *  --------------
 *
 *  Description:
 *  ------------
 *    Call Audio API and enquire if voice is supported
 *    
 *    
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 ******************************************************************************/

void  ghdi_is_voice_supported(void)
{

  uint32_t voicecfg_ret_val = TRUE;
  voicecfg_cmd_get_bool_item_t item; 
  item.id = VOICECFG_CFG_IS_VOICE_SUBSYSTEM_DISABLED; // 0x00012F06 defined in voicecfg_items.h public header file
  item.ret_value = FALSE; 
  voicecfg_ret_val = voicecfg_call( VOICECFG_CMD_GET_BOOL_ITEM, &item, sizeof( item ) );

  if ( (! voicecfg_ret_val)  && item.ret_value == TRUE )
  { 
    is_voice_supported = FALSE;
  } 
  else 
  { 
    is_voice_supported = TRUE;
  }
  MSG_HIGH_3("voicecfg_ret_val = %d, item.ret_value = %d, is_voice_supported = %d",voicecfg_ret_val,item.ret_value,is_voice_supported);

}

/*******************************************************************************
 *
 *  Function name:  ghdi_mvs_init()
 *  --------------
 *
 *  Description:
 *  ------------
 *    Initializes GSVOICE global variables and critical sections.
 *    
 *    
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 ******************************************************************************/
void ghdi_mvs_init(void)
{
  ghdi_is_voice_supported();
  NAS_INIT_CRIT_SECT(ghdi_mvs_crit_sec);
  atomic_init(&mvs_callback_pending,FALSE);
  atomic_init(&mvs_clients_to_be_released,0x0);
}

#ifdef FEATURE_WCDMA
/*******************************************************************************
 *
 *  Function name:   ghdi_amr_ul_cb()
 *  --------------
 *
 *  Description:
 *  ------------
 *    Used to transafer the UL frame/packet from the Vocoder/VDSP to L1 in
 *    WCDMA mode.
 *    
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 ******************************************************************************/
void ghdi_amr_ul_cb(uint8               *vocoder_packet,
                    mvs_frame_info_type *frame_info,
                    uint16              packet_length,
                    mvs_pkt_status_type *status)
{
  if(is_voice_supported == TRUE)
  {
    mvssup_wcdma_ul_processing(vocoder_packet, frame_info);
  
    if(packet_length == 0)
    {
        MSG_LOW_1("Received 0 length packet", packet_length);
    }
  }
  *status = MVS_PKT_STATUS_NORMAL;
}

/*******************************************************************************
 *
 *  Function name:  ghdi_amr_dl_cb()
 *  --------------
 *
 *  Description:
 *  ------------
 *    Used for transfering downlink frames/packets from the L1 to Vocoder/VDSP in
 *    WCDMA mode.
 *
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 ******************************************************************************/
void ghdi_amr_dl_cb(uint8               *vocoder_packet,
                    mvs_frame_info_type *frame_info,
                    mvs_pkt_status_type *status)
{
  if(is_voice_supported == TRUE)
  {
    mvssup_wcdma_dl_processing(vocoder_packet, frame_info);
  }
  *status = MVS_PKT_STATUS_NORMAL;
}
#endif  /* FEATURE_WCDMA */

#ifdef FEATURE_TDSCDMA
/*******************************************************************************
 *
 *  Function name:   ghdi_umts_amr_ul_cb()
 *  --------------
 *
 *  Description:
 *  ------------
 *    Used to transafer the UL frame/packet from the Vocoder/VDSP to L1 in
 *    TDSCDMA mode.
 *    
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 ******************************************************************************/
void ghdi_umts_amr_ul_cb(uint8               *vocoder_packet,
                    mvs_frame_info_type *frame_info,
                    uint16              packet_length,
                    mvs_pkt_status_type *status)
{
  if(is_voice_supported == TRUE)
  {
    mvssup_umts_ul_processing(vocoder_packet, frame_info);
  
    if(packet_length == 0)
    {
        MSG_LOW_1("Received 0 length packet", packet_length);
    }
  }
  *status = MVS_PKT_STATUS_NORMAL;
}

/*******************************************************************************
 *
 *  Function name:  ghdi_umts_1amr_dl_cb()
 *  --------------
 *
 *  Description:
 *  ------------
 *    Used for transfering downlink frames/packets from the L1 to Vocoder/VDSP in
 *    TDSCDMA mode.
 *
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 ******************************************************************************/
void ghdi_umts_amr_dl_cb(uint8               *vocoder_packet,
                    mvs_frame_info_type *frame_info,
                    mvs_pkt_status_type *status)
{
  if(is_voice_supported == TRUE)
  {
    mvssup_umts_dl_processing(vocoder_packet, frame_info);
  }
  *status = MVS_PKT_STATUS_NORMAL;
}
#endif  /* FEATURE_TDSCDMA */

#ifdef FEATURE_GSM
/*******************************************************************************
 *
 *  Function name:  ghdi_mvs_gsm_ul_proc()
 *  --------------
 *
 *  Description:
 *  ------------
 *    Used to transafer the UL frame/packet from the Vocoder/VDSP to L1 in
 *    GSM mode.
 *    
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 ******************************************************************************/
void ghdi_mvs_gsm_ul_proc(uint8                *ul_pkt,
                          mvs_frame_info_type  *frame_info,
                          uint16               pkt_len,
                          mvs_pkt_status_type  *status)
{
  if(is_voice_supported == TRUE)
  {
    /* This is within ISR context, no need to lock interrupts */
    gl1_msg_tch_ul_processing(ul_pkt,frame_info,pkt_len,status);

  }
}

/*******************************************************************************
 *
 *  Function name: ghdi_mvs_gsm_dl_proc()
 *  --------------
 *
 *  Description:
 *  ------------
 *    Used for transfering downlink frames/packets from the L1 to Vocoder/VDSP in
 *    WCDMA mode.
 *
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 ******************************************************************************/
void ghdi_mvs_gsm_dl_proc(uint8                *dl_pkt,
                          mvs_frame_info_type  *frame_info,
                          mvs_pkt_status_type  *status)
{
  if(is_voice_supported == TRUE)
  {
    /* This is within ISR context, no need to lock interrupts */
    gl1_msg_tch_dl_processing(dl_pkt,frame_info,status);
  }
}
#endif  /* FEATURE_GSM */

/*******************************************************************************
 *
 *  Function name:  ghdi_mvs_cb_func()
 *  --------------
 *
 *  Description:
 *  ------------
 *    Used to acquiring (load vocoder image) and enabling of the MVS (multi-mode 
 *    vocoder services).  
 *
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 ******************************************************************************/
void ghdi_mvs_cb_func(mvs_event_type *event) 
{
  if(is_voice_supported == TRUE)
  {
    byte mvs_event_pkt[2];
  
  #ifdef FEATURE_SEGMENT_LOADING
    interface_t *ptr_W = get_wcdma_interface();
    interface_t *ptr_T = get_tdscdma_interface();
  #endif
    MSG_HIGH_3("MVS event:%d status:%d cli:%d ",event->hdr.event, 
                                              event->cmd.cmd_status, 
                                              event->hdr.client);
  
    MSG_HIGH_3("MVS mode:%d, GHDI status:%d GHDI mode:%d",event->mode.mode, 
                                                        ghdi_mvs_status,
                                                        ghdi_mvs_mode);
  
    NAS_ENTER_CRIT_SECT(ghdi_mvs_crit_sec);
  
    switch(event->hdr.event) 
    {
      case MVS_EV_COMMAND:
  
        if(event->cmd.cmd_status == MVS_CMD_SUCCESS) 
        {
            atomic_set(&mvs_callback_pending,FALSE);
            MSG_HIGH_1("MVS cmd callback :%d",mvs_callback_pending.value);
            if(ghdi_mvs_status == GHDI_MVS_ACQUIRE) 
            {
  
               ghdi_send_event_ghdi_mvs_state(EVENT_MVS_ACQUIRE_CALLBACK_RECD, event->hdr.client, ghdi_mvs_status, ghdi_mvs_mode);
  
  
                
  /* Check if any of the MVS Commands are buffered, here. */
  /* Release is possible here, W->G HO Failure, case. If so, trigger for standby/enable should be bypassed. */
  /* TBD */
                if(event->hdr.client == MVS_CLIENT_GSM)
            {
#ifdef FEATURE_INTER_RAT_HO_OPT                  
               gl1_msg_vocoder_acquired();
#endif
  
#ifdef FEATURE_MVS_UNDER_GHDI
#ifdef FEATURE_VOC_AMR_WB                  
               if(ghdi_amr_wb_ul_mode_config_modified == TRUE)
               {
                  mvssup_amr_set_awb_mode(ghdi_amr_wb_ul_mode);
                  ghdi_amr_wb_ul_mode_config_modified = FALSE;
               }
#endif                  
                       
               if(ghdi_amr_ul_mode_config_modified == TRUE)
               {
                  mvssup_amr_set_amr_mode(ghdi_amr_ul_mode);
                  ghdi_amr_ul_mode_config_modified = FALSE;
               }
#endif /* FEATURE_MVS_UNDER_GHDI */ 
#ifdef FEATURE_INTER_RAT_HO_OPT
#if defined FEATURE_DUAL_SIM
              if(gl1_msg_is_l1_vocoder_control_enabled_multisim(ghdi_get_mmode_subs_asid()))          
#else
              if(gl1_msg_is_l1_vocoder_control_enabled())
#endif
               {
                  MSG_HIGH_0("GSM L1 controlling the vocoder");
               }
               else
#endif
               {
                   MSG_HIGH_1("NAS Enabling MVS(ghdi) for cli %d",event->hdr.client);
                   ghdi_mvs_enable(event->hdr.client, ghdi_mvs_mode);
               }
            }//if for the GSM
  
  
      else if(event->hdr.client == MVS_CLIENT_WCDMA)
      {
#if !defined(FEATURE_MVS_UNDER_GHDI) && defined(FEATURE_WCDMA)
#ifdef FEATURE_SEGMENT_LOADING
          if (ptr_W != NULL)
          {
             rrc_restore_mvs(ptr_W);
          }
#else /* FEATURE_SEGMENT_LOADING */
          rrc_restore_mvs();
#endif  /* FEATURE_SEGMENT_LOADING */
#endif
#if defined(FEATURE_MVS_UNDER_GHDI) && defined(FEATURE_WCDMA)
#ifdef FEATURE_SEGMENT_LOADING
          if (get_wcdma_interface())
#endif
          {
           /* Config the MVS with the stored W info & call RRC function to Conf WL1 */
             /* UL */
               ghdi_configure_mvs_wcdma_ul_channel();
           /* DL  RRC function to Configure WL1 is called inside the below fn */
               ghdi_configure_mvs_wcdma_dl_channel();
          
#ifdef FEATURE_VOC_AMR_WB
               mvssup_amr_set_awb_mode(ghdi_amr_wb_ul_mode);
#endif                  
               mvssup_amr_set_amr_mode(ghdi_amr_ul_mode);
               mvssup_amr_set_scr_mode(ghdi_amr_scr_ul_mode);
          }
#endif /*#if defined(FEATURE_MVS_UNDER_GHDI) && defined(FEATURE_WCDMA) */
  
#ifdef FEATURE_INTER_RAT_HO_OPT
#ifdef FEATURE_SEGMENT_LOADING
#if defined FEATURE_DUAL_SIM
           if(( ptr_W != NULL) && IWCDMA_rrc_vocoder_control_enabled_sub(ptr_W,ghdi_get_mmode_subs_asid()))
#else
           if(( ptr_W != NULL) && IWCDMA_rrc_vocoder_control_enabled(ptr_W))
#endif /* FEATURE_DUAL_SIM */
           {                 
              MSG_HIGH_0("RRC controlling the vocoder");
              IWCDMA_rrc_vocoder_acquired(ptr_W);
           }
#else
  
#if defined FEATURE_DUAL_SIM
           if( rrc_vocoder_control_enabled_sub(ghdi_get_mmode_subs_asid()))
#else
           if( rrc_vocoder_control_enabled())
#endif /* FEATURE_DUAL_SIM */
           {                 
              MSG_HIGH_0("RRC controlling the vocoder");
              rrc_vocoder_acquired();
           }
#endif /* FEATURE_SEGMENT_LOADING */
           else
#endif /* FEATURE_INTER_RAT_HO_OPT */
           {
               MSG_HIGH_1("NAS Enabling MVS(ghdi) for cli %d",event->hdr.client);
               ghdi_mvs_enable(event->hdr.client, ghdi_mvs_mode);
           }
  
  
      }//if for the wcdma
  
   else if (event->hdr.client == MVS_CLIENT_TDSCDMA)
          {
#ifdef FEATURE_TDSCDMA           
#ifdef FEATURE_SEGMENT_LOADING        
           if (get_tdscdma_interface())
#endif
           {
             /* Config the MVS with the stored TDS info &
                         call RRC function to Conf TDS L1 UL */
             ghdi_configure_mvs_tdscdma_ul_channel();
                      /* DL RRC function to Configure TDSL1 is called
                         inside the below fn */
             ghdi_configure_mvs_tdscdma_dl_channel();
  
#ifdef FEATURE_VOC_AMR_WB
             mvssup_amr_set_awb_mode(ghdi_amr_wb_ul_mode);
#endif                  
             mvssup_amr_set_amr_mode(ghdi_amr_ul_mode);
             mvssup_amr_set_scr_mode(ghdi_amr_scr_ul_mode);
           }
#endif /* FEATURE_TDSCDMA */
#ifdef FEATURE_INTER_RAT_HO_OPT
#ifdef FEATURE_TDSCDMA 
#ifdef FEATURE_SEGMENT_LOADING
#if defined FEATURE_DUAL_SIM
            if ((ptr_T != NULL)&&ITDSCDMA_tdsrrc_vocoder_control_enabled_sub(ptr_T,ghdi_get_mmode_subs_asid()))
#else
            if ((ptr_T != NULL)&&ITDSCDMA_tdsrrc_vocoder_control_enabled(ptr_T))
#endif /* FEATURE_DUAL_SIM */
            {                 
              MSG_HIGH_0("TDSRRC controlling the vocoder");
  		    ITDSCDMA_tdsrrc_vocoder_acquired(ptr_T);
            }
  
#else /* else FEATURE_SEGMENT_LOADING */
  
#if defined FEATURE_DUAL_SIM
            if ( tdsrrc_vocoder_control_enabled_sub(ghdi_get_mmode_subs_asid()))
#else
            if ( tdsrrc_vocoder_control_enabled())
#endif /* FEATURE_DUAL_SIM */
            {                 
              MSG_HIGH_0("TDSRRC controlling the vocoder");
              tdsrrc_vocoder_acquired();
            }
#endif/* FEATURE_SEGMENT_LOADING */
           else
#endif /* FEATURE_TDSCDMA */
#endif /* FEATURE_INTER_RAT_HO_OPT */
           {
               MSG_HIGH_1("NAS Enabling MVS(ghdi) for cli %d",event->hdr.client);
               ghdi_mvs_enable(event->hdr.client, ghdi_mvs_mode);
           }
   
          }//if for the tdscdma
  
  
  
  
            } 
            else if(ghdi_mvs_status == GHDI_MVS_MODE_SELECT) 
            {
                ghdi_send_event_ghdi_mvs_state(EVENT_MVS_ENABLE_CALLBACK_RECD, event->hdr.client, ghdi_mvs_status, ghdi_mvs_mode);
  /* Check if any of the MVS Commands are buffered, here. */
  /* Release followed by Acquire are possible. W->G HO Failure case */
  /* Standby is possible. G->G HO Case */
                if(mvs_clients_to_be_released.value)
                {
                  /* by-passing standby*/
                  mvs_standby_pending = FALSE;      
                  MSG_HIGH_0("rel_pending");           
  
                  if(mvs_clients_to_be_released.value & 0x01)
                  {
                    atomic_and(&mvs_clients_to_be_released, ~0x01);
                    ghdi_mvs_release(MVS_CLIENT_WCDMA);
                  }
                
                  if(mvs_clients_to_be_released.value & 0x02)
                  {
                    atomic_and(&mvs_clients_to_be_released, ~0x02);
                    ghdi_mvs_release(MVS_CLIENT_GSM);
                  }
                 
#ifdef FEATURE_TDSCDMA
#ifdef FEATURE_SEGMENT_LOADING
                  if (get_tdscdma_interface() && (mvs_clients_to_be_released.value & 0x04))
#else
                  if(mvs_clients_to_be_released.value & 0x04)
#endif 
                  {
                    atomic_and(&mvs_clients_to_be_released, ~0x04);
                    ghdi_mvs_release(MVS_CLIENT_TDSCDMA);
                  }
#endif /* FEATURE_TDSCDMA */
                }
                else if (mvs_standby_pending)
                {
                  mvs_standby_pending = FALSE;
                  MSG_HIGH_0("stdby_pending");                
                  ghdi_mvs_standby(standby_pending_client_id,standby_pending_mode_id);              
                }
            } 
            else if(ghdi_mvs_status == GHDI_MVS_STANDBY)
            {
                ghdi_send_event_ghdi_mvs_state(EVENT_MVS_STANDBY_CALLBCK_RECD, event->hdr.client, GHDI_MVS_ACQUIRE, ghdi_mvs_mode);
  
                ghdi_mvs_status = GHDI_MVS_ACQUIRE;
  /* Check if any of the MVS Commands are buffered, here. */
  /* Enable(G) followed by Release(G) followed by Acquire(W) are possible. */
  /* Release(G) followed by Acquire(W) are possible. */
  /* Enable(G) is possible genuine G->G and W->G HO cases */
  
                if(mvs_clients_to_be_released.value)
                {
                  /* by-passing standby and enable */
                  mvs_standby_pending = FALSE;
                  mvs_enable_pending = FALSE;        
                  MSG_HIGH_0("rel_pending");                
  
                  if(mvs_clients_to_be_released.value & 0x01)
                  {
                      atomic_and(&mvs_clients_to_be_released, ~0x01);
                      ghdi_mvs_release(MVS_CLIENT_WCDMA);
                  }
  
                  if(mvs_clients_to_be_released.value & 0x02)
                  {
                      atomic_and(&mvs_clients_to_be_released, ~0x02);
                      ghdi_mvs_release(MVS_CLIENT_GSM);
                  }
  
#ifdef FEATURE_TDSCDMA
#ifdef FEATURE_SEGMENT_LOADING
                  if (get_tdscdma_interface() && (mvs_clients_to_be_released.value & 0x04))                
#else
                  if(mvs_clients_to_be_released.value & 0x04)
#endif
                  {
                      atomic_and(&mvs_clients_to_be_released, ~0x04);
                      ghdi_mvs_release(MVS_CLIENT_TDSCDMA);
                  }
#endif /* FEATURE_TDSCDMA */
                }
                
                else if (mvs_enable_pending)
                {
                  mvs_enable_pending = FALSE;
                  MSG_HIGH_0("enable_pending");
#ifdef FEATURE_CS_VOICE_OVER_HSPA
                  ghdi_mvs_enable(enable_pending_client_id, enable_pending_mode_id, enable_pending_rab_type);
#else
                  ghdi_mvs_enable(enable_pending_client_id, enable_pending_mode_id);
#endif
                }
            }
            else if(ghdi_mvs_status == GHDI_MVS_ON) 
            {
                MSG_HIGH_1("MVS is enabled = %d",ghdi_mvs_status);
            } 
            else if(ghdi_mvs_status == GHDI_MVS_OFF) 
            {
                MSG_HIGH_1("Disabled MVS = %d",ghdi_mvs_status);
            }
            else if(ghdi_mvs_status == GHDI_MVS_MODE_READY) 
            {
                MSG_HIGH_1("MVS is READY = %d",ghdi_mvs_status);
            }
            else if(ghdi_mvs_status == GHDI_MVS_CHANGING) 
            {
                ghdi_send_event_ghdi_mvs_state(EVENT_MVS_RELEASE_CALLBACK_RECD, event->hdr.client, ghdi_mvs_status, ghdi_mvs_mode);
  
                MSG_HIGH_1("MVS mode is changing = %d",ghdi_mvs_status);
  
#ifdef FEATURE_GSM
                if(event->hdr.client == MVS_CLIENT_GSM)
                {
                    // add l1 notification code here. 
                    gl1_msg_vocoder_released();
                }
#endif 
  
#if !defined(FEATURE_MVS_UNDER_GHDI) && defined(FEATURE_WCDMA)
                if(event->hdr.client == MVS_CLIENT_WCDMA)
                {
#ifdef FEATURE_SEGMENT_LOADING
                    if(ptr_W != NULL)
                    {
                        IWCDMA_rrc_save_mvs(ptr_W);
                    }
#else
                    rrc_save_mvs();
#endif
                }
#endif
  
                vocoder_acquired_by_client =  MVS_CLIENT_NONE;
                /* Check if any of the MVS Commands are buffered, here. */
                /* Enable(G) followed by Release(G) followed by Acquire(W) are possible. */
                /* Release(G) followed by Acquire(W) are possible. */
                /* Enable(G) is possible genuine G->G and W->G HO cases */
                
                if(mvs_clients_to_be_released.value)
                {
                  /* by-passing standby and enable */
                  mvs_standby_pending = FALSE;
                  mvs_enable_pending = FALSE;        
                  MSG_HIGH_0("rel_pending");                
  
                  if(mvs_clients_to_be_released.value & 0x01)
                  {
                      atomic_and(&mvs_clients_to_be_released, ~0x01);
                      ghdi_mvs_release(MVS_CLIENT_WCDMA);
                  }
  
                  if(mvs_clients_to_be_released.value & 0x02)
                  {
                      atomic_and(&mvs_clients_to_be_released, ~0x02);
                      ghdi_mvs_release(MVS_CLIENT_GSM);
                  }
  
#ifdef FEATURE_TDSCDMA
#ifdef FEATURE_SEGMENT_LOADING
                  if (get_tdscdma_interface() && (mvs_clients_to_be_released.value & 0x04))
#else
                  if(mvs_clients_to_be_released.value & 0x04)
#endif                              
                  {
                      atomic_and(&mvs_clients_to_be_released, ~0x04);
                      ghdi_mvs_release(MVS_CLIENT_TDSCDMA);
                  }
#endif /* FEATURE_TDSCDMA */
                }
                  if(mvs_acquire_pending)
                  {
#ifdef FEATURE_WCDMA
                     if( acquire_pending_client_id == MVS_CLIENT_WCDMA)
                     {
                        MSG_HIGH_3("ghdi_mvs_acquire() client=%d, status=%d, mode=%d", acquire_pending_client_id, ghdi_mvs_status, ghdi_mvs_mode);
  
                        MSG_HIGH_1("vocoder being acquired(ghdi), by cleint = %d",MVS_CLIENT_WCDMA);
                        ghdi_mvs_acquire(MVS_CLIENT_WCDMA);
                     }
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_TDSCDMA
                     if (acquire_pending_client_id == MVS_CLIENT_TDSCDMA)
                     {
                        MSG_HIGH_3("ghdi_mvs_acquire() client=%d, status=%d, mode=%d", acquire_pending_client_id, ghdi_mvs_status, ghdi_mvs_mode);
  
                        MSG_HIGH_1("vocoder being acquired(ghdi), by client = %d",MVS_CLIENT_TDSCDMA);
                        ghdi_mvs_acquire(MVS_CLIENT_TDSCDMA);
                     }
#endif /* FEATURE_TDSCDMA */
                     if(acquire_pending_client_id == MVS_CLIENT_GSM)
                     {
                        MSG_HIGH_3("ghdi_mvs_acquire() client=%d, status=%d, mode=%d", acquire_pending_client_id, ghdi_mvs_status, ghdi_mvs_mode);
  
                        MSG_HIGH_1("vocoder being acquired(ghdi), by cleint = %d",MVS_CLIENT_GSM);
                        ghdi_mvs_acquire(MVS_CLIENT_GSM);
#ifdef FEATURE_GSM
                        mvssup_gsm_set_dtx_mode(ghdi_dtx_mode);  /* GSM only */
#endif /*FEATURE_GSM*/
                     }
                     else
                     {
                       MSG_ERROR_1("GHDI unsupported client = %d", acquire_pending_client_id);
                     }
  
                     ghdi_mvs_acquire_counter = 0;
                     mvs_acquire_pending = FALSE;
                     acquire_pending_client_id = MVS_CLIENT_NONE;
                  }
  
            }
            else if(ghdi_mvs_status == GHDI_MVS_FAILED) 
            {
                MSG_ERROR_1("MVS had FAILED = %d",ghdi_mvs_status);
            }
            else
            {
                MSG_ERROR_1("MVS return Unknown status = %d",ghdi_mvs_status);
            }
        } /* MVS CMD_SUCCESS */ 
        else if(event->cmd.cmd_status == MVS_CMD_FAILURE) 
        {
            ghdi_send_event_ghdi_mvs_state(EVENT_MVS_CALLBACK_FAILURE_RECD, event->hdr.client, ghdi_mvs_status, ghdi_mvs_mode);
  
            MSG_ERROR_0("MVS command failed");
            atomic_set(&mvs_callback_pending,FALSE);
            MSG_HIGH_1("MVS cmd callback :%d",mvs_callback_pending.value);
        
        } 
        else if(event->cmd.cmd_status == MVS_CMD_BUSY)      /* ERROR RECOVERY procedure */
        {
            ghdi_send_event_ghdi_mvs_state(EVENT_MVS_CALLBACK_BUSY_RECD, event->hdr.client, ghdi_mvs_status, ghdi_mvs_mode);
  
            MSG_ERROR_0("MVS busy, try later");
  
            atomic_set(&mvs_callback_pending,FALSE);
            MSG_HIGH_1("MVS cmd callback :%d",mvs_callback_pending.value);
  
   
        }
        else
        {
            MSG_ERROR_1("MVS invalid command = %d",event->cmd.cmd_status);
            ghdi_mvs_status = GHDI_MVS_OFF;
        }
        break;
  
      case MVS_EV_MODE:
  
        if(ghdi_mvs_status == GHDI_MVS_MODE_SELECT) 
        {
            if(event->mode.mode_status == MVS_MODE_INIT) 
            {
                ghdi_send_event_ghdi_mvs_state(EVENT_MVS_ENABLE_CALLBACK_MODE_INIT, event->hdr.client, ghdi_mvs_status, ghdi_mvs_mode);
                MSG_HIGH_1("MVS mode = MVS_MODE_INIT, %d",event->mode.mode_status);
            } 
            else if(event->mode.mode_status == MVS_MODE_READY) 
            {
                MSG_HIGH_1("MVS mode = MVS_MODE_READY, %d",event->mode.mode_status);
                ghdi_mvs_status = GHDI_MVS_ON;
                ghdi_send_event_ghdi_mvs_state(EVENT_MVS_ENABLE_CALLBACK_MODE_READY, event->hdr.client, ghdi_mvs_status, ghdi_mvs_mode);
  
#ifdef FEATURE_GSM
                /* TBD - needs to call L1 to stop Vocoder Data Blanking */
                if (event->hdr.client == MVS_CLIENT_GSM)
                {
                   /* Call L1 to stop Vocoder Data Blanking */
                   gl1_msg_vocoder_change_complete(*event,ghdi_mvs_mode);
                }
#endif /* FEATURE_GSM */
            } 
            else if(event->mode.mode_status == MVS_MODE_NOT_AVAIL) 
            {
                MSG_ERROR_1("MVS mode = MVS_MODE_NOT_AVAIL, %d",(event->mode.mode_status));
                ghdi_mvs_status = GHDI_MVS_OFF;
                ghdi_send_event_ghdi_mvs_state(EVENT_MVS_ENABLE_CALLBACK_MODE_NOT_AVAIL, event->hdr.client, ghdi_mvs_status, ghdi_mvs_mode);
            }
        } 
        /* Added by David for vocoder blanking */
        else if(ghdi_mvs_status == GHDI_MVS_ON) 
        {
#ifdef FEATURE_GSM
            if(event->mode.mode_status == MVS_MODE_READY) 
            {
                ghdi_send_event_ghdi_mvs_state(EVENT_MVS_ON_CALLBACK_MODE_READY, event->hdr.client, ghdi_mvs_status, ghdi_mvs_mode);
                MSG_HIGH_1("MVS mode = MVS_MODE_READY, %d",(event->mode.mode_status));
                 
                /* TBD - needs to call L1 to stop Vocoder Data Blanking */
                if (event->hdr.client == MVS_CLIENT_GSM)
                {
                   /* Call L1 to stop Vocoder Data Blanking */
                   gl1_msg_vocoder_change_complete(*event,ghdi_mvs_mode);
                }
            } 
            else 
            {
                MSG_ERROR_2 ("MVS mode = %d GHDI status status = %d",event->mode.mode_status,ghdi_mvs_status);
            }
#endif /* FEATURE_GSM */
        }
        else 
        {
            MSG_ERROR_1("GHDI invalid status = %d",ghdi_mvs_status);
        }
        break;
  
#ifdef FEATURE_ENHANCED_AMR      
        /* eAMR --> Switch case to handle MVS callback.
         * MVS calls this callback with this event (MVS_EV_EAMR_SR)
         * when it has to report sampling rate of current codec.
         * */
        case MVS_EV_EAMR_SR:
          {
              MSG_HIGH_3("GHDI Received MVS_EV_EAMR_SR from MVS with sampling rate = %d for client = %d in ghdi_mvs_status = %d",event->eamr.sr,event->hdr.client,ghdi_mvs_status);
            
              /* !to check: do we need to put setting of this shared variable under critical section.
               * This file in this branch doesn't use critical sections */
  
              /* Update GHDI global variable with latest sampling rate */
  
              if(event->eamr.sr == MVS_EAMR_SR_16000) 
              {
                speech_enc_samp_freq = GHDI_EAMR_SR_16000; 
              }
              else  
              {
                speech_enc_samp_freq = GHDI_EAMR_SR_8000; 
              } 
            
              if(event->hdr.client == MVS_CLIENT_WCDMA)
              {
                rrc_update_codec_info_to_cm();
              }
              else if(event->hdr.client == MVS_CLIENT_GSM)
              {
                 (void) geran_grr_set_codec_ho_sample_rate(speech_enc_samp_freq, FALSE);
              }
#ifdef FEATURE_TDSCDMA
              else if(event->hdr.client == MVS_CLIENT_TDSCDMA)
              {          
                tdsrrc_update_codec_info_to_cm(speech_enc_samp_freq);
              }
#endif
  
          }
          break;
  
#endif
  
      default:
        MSG_ERROR_1("MVS unknown event %d =",event->hdr.event);
        break;
    }
  
    NAS_EXIT_CRIT_SECT(ghdi_mvs_crit_sec);
  
    /* Log MVS event, EVENT_MVS_STATE */
    mvs_event_pkt[0] = (byte) event->hdr.client; /* GSM or WCDMA */
  
    if(event->hdr.event ==  MVS_EV_COMMAND)
        mvs_event_pkt[1] = ((byte) event->cmd.cmd_status) << 4;  /* MVS command status */
    else
        mvs_event_pkt[1] = ((byte) event->mode.mode_status) << 6; /* MVS mode status */
  
    mvs_event_pkt[1] |= ((byte) ghdi_mvs_status & 0x0F);  /* GHDI status/state */
  
    event_report_payload(EVENT_MVS_STATE, 2, (void *)mvs_event_pkt);
  
  }
}



/*******************************************************************************************
 *
 *  Function name: ghdi_control_tch_gating --> Wrapper on top of actual tch_gating functions
 *  -------------------------------------------------------------------------
 *  Description:
 *  ------------
 *  
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 *******************************************************************************************/

ghdi_status_T ghdi_control_tch_gating(ghdi_tch_control_T   tch_control,
                                      channel_mode_T       channel_mode
                                    , uint32                vsid
                                            )
{

  ghdi_status_T         ghdi_status = GHDI_FAILURE;
  ghdi_mvs_interface_type ghdi_mvs_interface_value = GHDI_MVS_INTERFACE_0;

#if defined(FEATURE_DUAL_SIM)
    ghdi_mvs_interface_value = ghdi_get_MVS_interface_val(vsid);
#endif

  if(ghdi_mvs_interface_value == GHDI_MVS_INTERFACE_0)
  {
    ghdi_status = ghdi_control_tch_gating_vs_0(tch_control, channel_mode);
  }
#if defined(FEATURE_DUAL_SIM)  
  else if(ghdi_mvs_interface_value == GHDI_MVS_INTERFACE_1)
  {
    ghdi_status = ghdi_control_tch_gating_vs_1(tch_control, channel_mode);
  }
#endif  
  else
  {
    ERR_FATAL("Invalid subscription = %d in DSDA",ghdi_mvs_interface_value,0,0);
  }
  return ghdi_status;

}



/*******************************************************************************************
 *
 *  Function name: ghdi_control_subs_tch_gating --> Wrapper on top of actual tch_gating functions
 *  -------------------------------------------------------------------------
 *  Description:
 *  ------------
 *  
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 *******************************************************************************************/

ghdi_status_T ghdi_control_subs_tch_gating(ghdi_tch_control_T        tch_control,
                                      channel_mode_T                      channel_mode
                                    , uint32                              vsid
                                    , sys_modem_as_id_e_type              as_id
                                            )
{

  ghdi_status_T         ghdi_status = GHDI_FAILURE;
  ghdi_mvs_interface_type ghdi_mvs_interface_value = GHDI_MVS_INTERFACE_0;

#if defined(FEATURE_DUAL_SIM)
    ghdi_mvs_interface_value = ghdi_get_MVS_interface_val(vsid);
#endif

  if(ghdi_mvs_interface_value == GHDI_MVS_INTERFACE_0)
  {
    ghdi_status = ghdi_control_tch_gating_vs_0(tch_control, channel_mode);
  }
#if defined(FEATURE_DUAL_SIM)
  else if(ghdi_mvs_interface_value == GHDI_MVS_INTERFACE_1)
  {
    ghdi_status = ghdi_control_tch_gating_subs_vs_1(tch_control, channel_mode, as_id);
  }
#endif  
  else
  {
    ERR_FATAL("Invalid subscription = %d in DSDA",ghdi_mvs_interface_value,0,0);
  }
  return ghdi_status;

}


/*******************************************************************************************
 *
 *  Function name: ghdi_control_tch_gating_subs --> Wrapper on top of actual tch_gating functions
 *  -------------------------------------------------------------------------
 *  Description:
 *  ------------
 *  
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 *******************************************************************************************/

ghdi_status_T ghdi_control_tch_gating_subs(ghdi_tch_control_T   tch_control,
                                      channel_mode_T       channel_mode,
                                      uint32                vsid
                                            )
{

  return ghdi_control_tch_gating(tch_control, channel_mode
                                            , vsid
                                );

}



/*******************************************************************************
 *
 *  Function name: ghdi_control_tch_gating_vs_1 --> NAS interface with new MVS
 *  -------------------------------------------------------------------------
 *  Description:
 *  ------------
 *  Used to controls(enabling and disabling) the vocoder services in GSM mode,
 *  by controlling the transmission channel gating.
 *
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 ******************************************************************************/

ghdi_status_T ghdi_control_tch_gating_vs_1(ghdi_tch_control_T   tch_control,
                                                    channel_mode_T       channel_mode)
{

   ghdi_status_T         ghdi_status = GHDI_SUCCESS;
#if defined(FEATURE_DUAL_SIM) 
   mvs_mode_type         local_ghdi_mvs_mode = MVS_MODE_NONE;
   ghdi_mvs_status_type  local_ghdi_mvs_status = GHDI_MVS_OFF;



   if ((tch_control == GHDI_TURN_SPEECH_TCH_F_ON) || 
       (tch_control == GHDI_TURN_SPEECH_TCH_H_0_ON) ||
       (tch_control == GHDI_TURN_SPEECH_TCH_H_1_ON))
   {
       switch(channel_mode)
       {
           case SPEECH_V1 :
               /* This is normal/default Full Rate. Do nothing  */
               break;

           case SPEECH_V2 :
               /* This is Full Rate Speech Version 2, aka EFR */
               tch_control = GHDI_TURN_SPEECH_TCH_EF_ON;
               break;

           case SPEECH_V3 :
               tch_control = GHDI_TURN_SPEECH_TCH_AMR_ON;
               break;

#ifdef FEATURE_GSM_AMR_WB
           case SPEECH_V5:
               tch_control = GHDI_TURN_SPEECH_TCH_AMR_WB_ON;
               break;
#endif               

           default :
               MSG_ERROR_0("Invalid/unsupported channel_mode = %d for speech");
               ghdi_status = GHDI_FAILURE;
               break;
       }
   }
   else if ((tch_control == GHDI_TURN_SPEECH_TCH_F_OFF) ||
            (tch_control == GHDI_TURN_SPEECH_TCH_H_0_OFF) ||
            (tch_control == GHDI_TURN_SPEECH_TCH_H_1_OFF))
   {
       switch(channel_mode)
       {
           case SPEECH_V1 :
               /* This is normal/default Full Rate. Do nothing  */
               break;

           case SPEECH_V2 :
               /* This is Full Rate Speech Version 2, aka EFR */
               tch_control = GHDI_TURN_SPEECH_TCH_EF_OFF;
               break;

           case SPEECH_V3 :
               tch_control = GHDI_TURN_SPEECH_TCH_AMR_OFF;
               break;

#ifdef FEATURE_GSM_AMR_WB
           case SPEECH_V5:
               tch_control = GHDI_TURN_SPEECH_TCH_AMR_WB_OFF;
               break;
#endif               

           default :
               MSG_ERROR_1("Invalid/unsupported channel_mode=%d ",channel_mode);
               ghdi_status = GHDI_FAILURE;
               break;
       }
   }
   else if ((tch_control == GHDI_TURN_DATA_TCH_F_ON) || 
            (tch_control == GHDI_TURN_DATA_TCH_H_0_ON) ||
            (tch_control == GHDI_TURN_DATA_TCH_H_1_ON))
   {
       MSG_HIGH_1("Connecting CS DATA TCH = %d", tch_control); 
   }
   else if ((tch_control == GHDI_TURN_DATA_TCH_F_OFF) ||
            (tch_control == GHDI_TURN_DATA_TCH_H_0_OFF) ||
            (tch_control == GHDI_TURN_DATA_TCH_H_1_OFF))
   {
       MSG_HIGH_1("Disconnecting CS DATA TCH = %d", tch_control); 
   }
   else
   {
       MSG_ERROR_2("Invalid TCH rate=%d, mode=%d", tch_control, channel_mode);
   }

   /*lint -e546 */
   switch(tch_control)
   {
      case GHDI_TURN_SPEECH_TCH_F_ON :
        local_ghdi_mvs_mode = MVS_MODE_FR;
        local_ghdi_mvs_status = GHDI_MVS_ACQUIRE;
        MSG_HIGH_1("Acquiring FR vocoder, mode =%d",channel_mode);
        break;

      case GHDI_TURN_SPEECH_TCH_EF_ON :
        local_ghdi_mvs_mode = MVS_MODE_EFR;
        local_ghdi_mvs_status = GHDI_MVS_ACQUIRE;
        MSG_HIGH_1("Acquiring EFR Vocoder, mode =%d",channel_mode);
        break;

      case GHDI_TURN_SPEECH_TCH_H_0_ON:
      case GHDI_TURN_SPEECH_TCH_H_1_ON:
#ifdef FEATURE_GSM_HALF_RATE 
        /* enable MVS Half Rate */
        local_ghdi_mvs_mode = MVS_MODE_HR;
        local_ghdi_mvs_status = GHDI_MVS_ACQUIRE;
        MSG_HIGH_2("Acquiring HALF Rate Vocoder,type=%d,mode=%d",tch_control,channel_mode);
#else
        MSG_HIGH_2("HALF Rate Vocoder not supported,type=%d,mode=%d",tch_control,channel_mode);
#endif /* FEATURE_GSM_HALF_RATE */
        break;

      case GHDI_TURN_SPEECH_TCH_AMR_ON:
#ifdef FEATURE_GSM_AMR
        /* enable MVS AMR */
        local_ghdi_mvs_mode = MVS_MODE_AMR;
        local_ghdi_mvs_status = GHDI_MVS_ACQUIRE;
        MSG_HIGH_2("Acquiring GSM AMR, type=%d,mode=%d",tch_control,channel_mode);
#else
        MSG_ERROR_2("Unsupported GSM AMR, type=%d,mode=%d",tch_control,channel_mode);
#endif /* FEATURE_GSM_ARM */
        break;

      case GHDI_TURN_SPEECH_TCH_AMR_WB_ON:
#if defined (FEATURE_GSM_AMR) && defined (FEATURE_GSM_AMR_WB)
        /* enable MVS AMR */
        local_ghdi_mvs_mode = MVS_MODE_AMR_WB;
        local_ghdi_mvs_status = GHDI_MVS_ACQUIRE;
        MSG_HIGH_2("Acquiring GSM AMR WB, type=%d,mode=%d",tch_control,channel_mode);
#else
        MSG_ERROR_2("Unsupported GSM AMR WB, type=%d,mode=%d",tch_control,channel_mode);
#endif /* FEATURE_GSM_ARM */
        break;

      case GHDI_TURN_SPEECH_TCH_F_OFF :
        local_ghdi_mvs_status = GHDI_MVS_CHANGING;
        MSG_HIGH_1("Releasing FR Vocoder, mode =%d",channel_mode);
        break;

      case GHDI_TURN_SPEECH_TCH_EF_OFF :
        local_ghdi_mvs_status = GHDI_MVS_CHANGING;
        MSG_HIGH_1("Releasing EFR Vocoder, mode =%d",channel_mode);
        break;

      case GHDI_TURN_SPEECH_TCH_H_0_OFF:
        local_ghdi_mvs_status = GHDI_MVS_CHANGING;
        MSG_HIGH_1("Releasing HALF Rate Vocoder, mode =%d",channel_mode);
        break;

      case GHDI_TURN_SPEECH_TCH_H_1_OFF:
        local_ghdi_mvs_status = GHDI_MVS_CHANGING;
        MSG_HIGH_1("Releasing HALF Rate Vocoder, mode =%d",channel_mode);
        break;

      case GHDI_TURN_SPEECH_TCH_AMR_OFF:
        local_ghdi_mvs_status = GHDI_MVS_CHANGING;
        MSG_HIGH_1("Releasing GSM AMR Vocoder, mode =%d",channel_mode);
        break;

      case GHDI_TURN_SPEECH_TCH_AMR_WB_OFF:
        local_ghdi_mvs_status = GHDI_MVS_CHANGING;
        MSG_HIGH_1("Releasing GSM AMR Vocoder, mode =%d",channel_mode);
        break;

      default :
        MSG_ERROR_2("Invalid GSM voice/data services %d, %d",tch_control,channel_mode);
        ghdi_status = GHDI_FAILURE;
        break;
   }

   if(local_ghdi_mvs_status == GHDI_MVS_ACQUIRE)
   {
     MSG_HIGH_0("NAS called gl1_vs_ghdi_control with GL1_GHDI_VS_ENABLE");
     /* Check Error Handling when GL1 return failure*/
     gl1_vs_ghdi_control(GL1_GHDI_VS_ENABLE, local_ghdi_mvs_mode);
   }
   else if(local_ghdi_mvs_status == GHDI_MVS_CHANGING)
   {
     MSG_HIGH_0("NAS called gl1_vs_ghdi_control with GL1_GHDI_VS_DISABLE");
     /* Check Error Handling when GL1 return failure*/
     gl1_vs_ghdi_control(GL1_GHDI_VS_DISABLE, local_ghdi_mvs_mode);
   }
   else
   {
     MSG_ERROR_1("Invalid status = %d",local_ghdi_mvs_status);
   }
#endif   

   return ghdi_status;


}



/*******************************************************************************
 *
 *  Function name: ghdi_control_tch_gating_subs_vs_1 --> NAS interface with new MVS
 *  -------------------------------------------------------------------------
 *  Description:
 *  ------------
 *  Used to controls(enabling and disabling) the vocoder services in GSM mode,
 *  by controlling the transmission channel gating.
 *
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 ******************************************************************************/

ghdi_status_T ghdi_control_tch_gating_subs_vs_1(ghdi_tch_control_T      tch_control,
                                                       channel_mode_T          channel_mode,
                                                       sys_modem_as_id_e_type  as_id)
{

   ghdi_status_T         ghdi_status = GHDI_SUCCESS;
   
#ifdef FEATURE_DUAL_SIM
   mvs_mode_type         local_ghdi_mvs_mode = MVS_MODE_NONE;
   ghdi_mvs_status_type  local_ghdi_mvs_status = GHDI_MVS_OFF;



   if ((tch_control == GHDI_TURN_SPEECH_TCH_F_ON) || 
       (tch_control == GHDI_TURN_SPEECH_TCH_H_0_ON) ||
       (tch_control == GHDI_TURN_SPEECH_TCH_H_1_ON))
   {
       switch(channel_mode)
       {
           case SPEECH_V1 :
               /* This is normal/default Full Rate. Do nothing  */
               break;

           case SPEECH_V2 :
               /* This is Full Rate Speech Version 2, aka EFR */
               tch_control = GHDI_TURN_SPEECH_TCH_EF_ON;
               break;

           case SPEECH_V3 :
               tch_control = GHDI_TURN_SPEECH_TCH_AMR_ON;
               break;

#ifdef FEATURE_GSM_AMR_WB
           case SPEECH_V5:
               tch_control = GHDI_TURN_SPEECH_TCH_AMR_WB_ON;
               break;
#endif               

           default :
               MSG_ERROR_1("Invalid/unsupported channel_mode = %d for speech",channel_mode);
               ghdi_status = GHDI_FAILURE;
               break;
       }
   }
   else if ((tch_control == GHDI_TURN_SPEECH_TCH_F_OFF) ||
            (tch_control == GHDI_TURN_SPEECH_TCH_H_0_OFF) ||
            (tch_control == GHDI_TURN_SPEECH_TCH_H_1_OFF))
   {
       switch(channel_mode)
       {
           case SPEECH_V1 :
               /* This is normal/default Full Rate. Do nothing  */
               break;

           case SPEECH_V2 :
               /* This is Full Rate Speech Version 2, aka EFR */
               tch_control = GHDI_TURN_SPEECH_TCH_EF_OFF;
               break;

           case SPEECH_V3 :
               tch_control = GHDI_TURN_SPEECH_TCH_AMR_OFF;
               break;

#ifdef FEATURE_GSM_AMR_WB
           case SPEECH_V5:
               tch_control = GHDI_TURN_SPEECH_TCH_AMR_WB_OFF;
               break;
#endif               

           default :
               MSG_ERROR_1("Invalid/unsupported channel_mode=%d ",channel_mode);
               ghdi_status = GHDI_FAILURE;
               break;
       }
   }
   else if ((tch_control == GHDI_TURN_DATA_TCH_F_ON) || 
            (tch_control == GHDI_TURN_DATA_TCH_H_0_ON) ||
            (tch_control == GHDI_TURN_DATA_TCH_H_1_ON))
   {
       MSG_HIGH_1("Connecting CS DATA TCH = %d", tch_control); 
   }
   else if ((tch_control == GHDI_TURN_DATA_TCH_F_OFF) ||
            (tch_control == GHDI_TURN_DATA_TCH_H_0_OFF) ||
            (tch_control == GHDI_TURN_DATA_TCH_H_1_OFF))
   {
       MSG_HIGH_1("Disconnecting CS DATA TCH = %d", tch_control); 
   }
   else
   {
       MSG_ERROR_2("Invalid TCH rate=%d, mode=%d", tch_control, channel_mode);
   }

   /*lint -e546 */
   switch(tch_control)
   {
      case GHDI_TURN_SPEECH_TCH_F_ON :
        local_ghdi_mvs_mode = MVS_MODE_FR;
        local_ghdi_mvs_status = GHDI_MVS_ACQUIRE;
        MSG_HIGH_1("Acquiring FR vocoder, mode =%d",channel_mode);
        break;

      case GHDI_TURN_SPEECH_TCH_EF_ON :
        local_ghdi_mvs_mode = MVS_MODE_EFR;
        local_ghdi_mvs_status = GHDI_MVS_ACQUIRE;
        MSG_HIGH_1("Acquiring EFR Vocoder, mode =%d",channel_mode);
        break;

      case GHDI_TURN_SPEECH_TCH_H_0_ON:
      case GHDI_TURN_SPEECH_TCH_H_1_ON:
#ifdef FEATURE_GSM_HALF_RATE 
        /* enable MVS Half Rate */
        local_ghdi_mvs_mode = MVS_MODE_HR;
        local_ghdi_mvs_status = GHDI_MVS_ACQUIRE;
        MSG_HIGH_2("Acquiring HALF Rate Vocoder,type=%d,mode=%d",tch_control,channel_mode);
#else
        MSG_HIGH_2("HALF Rate Vocoder not supported,type=%d,mode=%d",tch_control,channel_mode);
#endif /* FEATURE_GSM_HALF_RATE */
        break;

      case GHDI_TURN_SPEECH_TCH_AMR_ON:
#ifdef FEATURE_GSM_AMR
        /* enable MVS AMR */
        local_ghdi_mvs_mode = MVS_MODE_AMR;
        local_ghdi_mvs_status = GHDI_MVS_ACQUIRE;
        MSG_HIGH_2("Acquiring GSM AMR, type=%d,mode=%d",tch_control,channel_mode);
#else
        MSG_ERROR_2("Unsupported GSM AMR, type=%d,mode=%d",tch_control,channel_mode);
#endif /* FEATURE_GSM_ARM */
        break;

      case GHDI_TURN_SPEECH_TCH_AMR_WB_ON:
#if defined (FEATURE_GSM_AMR) && defined (FEATURE_GSM_AMR_WB)
        /* enable MVS AMR */
        local_ghdi_mvs_mode = MVS_MODE_AMR_WB;
        local_ghdi_mvs_status = GHDI_MVS_ACQUIRE;
        MSG_HIGH_2("Acquiring GSM AMR WB, type=%d,mode=%d",tch_control,channel_mode);
#else
        MSG_ERROR_2("Unsupported GSM AMR WB, type=%d,mode=%d",tch_control,channel_mode);
#endif /* FEATURE_GSM_ARM */
        break;

      case GHDI_TURN_SPEECH_TCH_F_OFF :
        local_ghdi_mvs_status = GHDI_MVS_CHANGING;
        MSG_HIGH_1("Releasing FR Vocoder, mode =%d",channel_mode);
        break;

      case GHDI_TURN_SPEECH_TCH_EF_OFF :
        local_ghdi_mvs_status = GHDI_MVS_CHANGING;
        MSG_HIGH_1("Releasing EFR Vocoder, mode =%d",channel_mode);
        break;

      case GHDI_TURN_SPEECH_TCH_H_0_OFF:
        local_ghdi_mvs_status = GHDI_MVS_CHANGING;
        MSG_HIGH_1("Releasing HALF Rate Vocoder, mode =%d",channel_mode);
        break;

      case GHDI_TURN_SPEECH_TCH_H_1_OFF:
        local_ghdi_mvs_status = GHDI_MVS_CHANGING;
        MSG_HIGH_1("Releasing HALF Rate Vocoder, mode =%d",channel_mode);
        break;

      case GHDI_TURN_SPEECH_TCH_AMR_OFF:
        local_ghdi_mvs_status = GHDI_MVS_CHANGING;
        MSG_HIGH_1("Releasing GSM AMR Vocoder, mode =%d",channel_mode);
        break;

      case GHDI_TURN_SPEECH_TCH_AMR_WB_OFF:
        local_ghdi_mvs_status = GHDI_MVS_CHANGING;
        MSG_HIGH_1("Releasing GSM AMR Vocoder, mode =%d",channel_mode);
        break;

      default :
        MSG_ERROR_2("Invalid GSM voice/data services %d, %d",tch_control,channel_mode);
        ghdi_status = GHDI_FAILURE;
        break;
   }

   if(local_ghdi_mvs_status == GHDI_MVS_ACQUIRE)
   {
     MSG_HIGH_0("NAS called gl1_vs_ghdi_control_subs with GL1_GHDI_VS_ENABLE");
     /* Check Error Handling when GL1 return failure*/
     gl1_vs_ghdi_control_subs(GL1_GHDI_VS_ENABLE, local_ghdi_mvs_mode, as_id);
   }
   else if(local_ghdi_mvs_status == GHDI_MVS_CHANGING)
   {
     MSG_HIGH_0("NAS called gl1_vs_ghdi_control_subs with GL1_GHDI_VS_DISABLE");
     /* Check Error Handling when GL1 return failure*/
     gl1_vs_ghdi_control_subs(GL1_GHDI_VS_DISABLE, local_ghdi_mvs_mode, as_id);
   }
   else
   {
     MSG_ERROR_1("Invalid status = %d",local_ghdi_mvs_status);
   }
#endif   

   return ghdi_status;
}



/*******************************************************************************
 *
 *  Function name: ghdi_control_tch_gating_vs_0 --> NAS interface with old MVS
 *  -------------------------------------------------------------------------
 *  Description:
 *  ------------
 *  Used to controls(enabling and disabling) the vocoder services in GSM mode,
 *  by controlling the transmission channel gating.
 *
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 ******************************************************************************/
ghdi_status_T ghdi_control_tch_gating_vs_0(ghdi_tch_control_T   tch_control,
                                      channel_mode_T       channel_mode)
{
  ghdi_status_T   ghdi_status = GHDI_SUCCESS;
  if(is_voice_supported == TRUE)
  {
    ghdi_mvs_status_type  local_ghdi_mvs_status = ghdi_mvs_status;
    boolean         voc_acquisition_in_progress = FALSE;
  #ifdef FEATURE_SEGMENT_LOADING  
    interface_t *ptr_W = NULL;
    interface_t *ptr_T = NULL;
  
    ptr_W = get_wcdma_interface();
    ptr_T = get_tdscdma_interface();
  #endif
    voc_acquisition_in_progress = ghdi_is_acquisition_in_progress();
    
    
    if((tch_control >= GHDI_TURN_DATA_TCH_F_OFF) && (tch_control <= GHDI_TURN_SPEECH_TCH_AMR_WB_OFF))
    {
      voc_acquisition_in_progress = FALSE; /* enqueue the release */
    }
  
    if (voc_acquisition_in_progress != TRUE)
    {
     MSG_HIGH_2("NAS: tch_control %d channel_mode %d", tch_control, channel_mode);
  
  #ifdef FEATURE_GSM
     if ((tch_control == GHDI_TURN_SPEECH_TCH_F_ON) || 
         (tch_control == GHDI_TURN_SPEECH_TCH_H_0_ON) ||
         (tch_control == GHDI_TURN_SPEECH_TCH_H_1_ON))
     {
         switch(channel_mode)
         {
             case SPEECH_V1 :
                 /* This is normal/default Full Rate. Do nothing  */
                 break;
  
             case SPEECH_V2 :
                 /* This is Full Rate Speech Version 2, aka EFR */
                 tch_control = GHDI_TURN_SPEECH_TCH_EF_ON;
                 break;
  
             case SPEECH_V3 :
                 tch_control = GHDI_TURN_SPEECH_TCH_AMR_ON;
                 break;
  
  #ifdef FEATURE_GSM_AMR_WB
             case SPEECH_V5:
                 tch_control = GHDI_TURN_SPEECH_TCH_AMR_WB_ON;
                 break;
  #endif               
  
             default :
                 MSG_ERROR_0("Invalid/unsupported channel_mode = %d for speech");
                 ghdi_status = GHDI_FAILURE;
                 break;
         }
     }
     else if ((tch_control == GHDI_TURN_SPEECH_TCH_F_OFF) ||
              (tch_control == GHDI_TURN_SPEECH_TCH_H_0_OFF) ||
              (tch_control == GHDI_TURN_SPEECH_TCH_H_1_OFF))
     {
         switch(channel_mode)
         {
             case SPEECH_V1 :
                 /* This is normal/default Full Rate. Do nothing  */
                 break;
  
             case SPEECH_V2 :
                 /* This is Full Rate Speech Version 2, aka EFR */
                 tch_control = GHDI_TURN_SPEECH_TCH_EF_OFF;
                 break;
  
             case SPEECH_V3 :
                 tch_control = GHDI_TURN_SPEECH_TCH_AMR_OFF;
                 break;
  
  #ifdef FEATURE_GSM_AMR_WB
             case SPEECH_V5:
                 tch_control = GHDI_TURN_SPEECH_TCH_AMR_WB_OFF;
                 break;
  #endif               
  
             default :
                 MSG_ERROR_1("Invalid/unsupported channel_mode=%d ",channel_mode);
                 ghdi_status = GHDI_FAILURE;
                 break;
         }
     }
     else if ((tch_control == GHDI_TURN_DATA_TCH_F_ON) || 
              (tch_control == GHDI_TURN_DATA_TCH_H_0_ON) ||
              (tch_control == GHDI_TURN_DATA_TCH_H_1_ON))
     {
         MSG_HIGH_1("Connecting CS DATA TCH = %d", tch_control); 
     }
     else if ((tch_control == GHDI_TURN_DATA_TCH_F_OFF) ||
              (tch_control == GHDI_TURN_DATA_TCH_H_0_OFF) ||
              (tch_control == GHDI_TURN_DATA_TCH_H_1_OFF))
     {
         MSG_HIGH_1("Disconnecting CS DATA TCH = %d", tch_control); 
     }
     else
     {
         MSG_ERROR_2("Invalid TCH rate=%d, mode=%d", tch_control, channel_mode);
     }
  
     /*lint -e546 */
     switch(tch_control)
     {
        case GHDI_TURN_SPEECH_TCH_F_ON :
          ghdi_mvs_mode = MVS_MODE_FR;
          local_ghdi_mvs_status = GHDI_MVS_ACQUIRE;
          MSG_HIGH_1("Acquiring FR vocoder, mode =%d",channel_mode);
          break;
  
        case GHDI_TURN_SPEECH_TCH_EF_ON :
          ghdi_mvs_mode = MVS_MODE_EFR;
          local_ghdi_mvs_status = GHDI_MVS_ACQUIRE;
          MSG_HIGH_1("Acquiring EFR Vocoder, mode =%d",channel_mode);
          break;
  
        case GHDI_TURN_SPEECH_TCH_H_0_ON:
        case GHDI_TURN_SPEECH_TCH_H_1_ON:
  #ifdef FEATURE_GSM_HALF_RATE 
          /* enable MVS Half Rate */
          ghdi_mvs_mode = MVS_MODE_HR;
          local_ghdi_mvs_status = GHDI_MVS_ACQUIRE;
          MSG_HIGH_2("Acquiring HALF Rate Vocoder,type=%d,mode=%d",tch_control,channel_mode);
  #else
          MSG_HIGH_2("HALF Rate Vocoder not supported,type=%d,mode=%d",tch_control,channel_mode);
  #endif /* FEATURE_GSM_HALF_RATE */
          break;
  
        case GHDI_TURN_SPEECH_TCH_AMR_ON:
  #ifdef FEATURE_GSM_AMR
          /* enable MVS AMR */
          ghdi_mvs_mode = MVS_MODE_AMR;
          local_ghdi_mvs_status = GHDI_MVS_ACQUIRE;
          MSG_HIGH_2("Acquiring GSM AMR, type=%d,mode=%d",tch_control,channel_mode);
  #else
          MSG_ERROR_2("Unsupported GSM AMR, type=%d,mode=%d",tch_control,channel_mode);
  #endif /* FEATURE_GSM_ARM */
          break;
  
        case GHDI_TURN_SPEECH_TCH_AMR_WB_ON:
  #if defined (FEATURE_GSM_AMR) && defined (FEATURE_GSM_AMR_WB)
          /* enable MVS AMR */
          ghdi_mvs_mode = MVS_MODE_AMR_WB;
          local_ghdi_mvs_status = GHDI_MVS_ACQUIRE;
          MSG_HIGH_2("Acquiring GSM AMR WB, type=%d,mode=%d",tch_control,channel_mode);
  #else
          MSG_ERROR_2("Unsupported GSM AMR WB, type=%d,mode=%d",tch_control,channel_mode);
  #endif /* FEATURE_GSM_ARM */
          break;
  
        case GHDI_TURN_SPEECH_TCH_F_OFF :
          local_ghdi_mvs_status = GHDI_MVS_CHANGING;
          MSG_HIGH_1("Releasing FR Vocoder, mode =%d",channel_mode);
          break;
  
        case GHDI_TURN_SPEECH_TCH_EF_OFF :
          local_ghdi_mvs_status = GHDI_MVS_CHANGING;
          MSG_HIGH_1("Releasing EFR Vocoder, mode =%d",channel_mode);
          break;
  
        case GHDI_TURN_SPEECH_TCH_H_0_OFF:
          local_ghdi_mvs_status = GHDI_MVS_CHANGING;
          MSG_HIGH_1("Releasing HALF Rate Vocoder, mode =%d",channel_mode);
          break;
  
        case GHDI_TURN_SPEECH_TCH_H_1_OFF:
          local_ghdi_mvs_status = GHDI_MVS_CHANGING;
          MSG_HIGH_1("Releasing HALF Rate Vocoder, mode =%d",channel_mode);
          break;
  
        case GHDI_TURN_SPEECH_TCH_AMR_OFF:
          local_ghdi_mvs_status = GHDI_MVS_CHANGING;
          MSG_HIGH_1("Releasing GSM AMR Vocoder, mode =%d",channel_mode);
          break;
  
        case GHDI_TURN_SPEECH_TCH_AMR_WB_OFF:
          local_ghdi_mvs_status = GHDI_MVS_CHANGING;
          MSG_HIGH_1("Releasing GSM AMR Vocoder, mode =%d",channel_mode);
          break;
  
        default :
          MSG_ERROR_2("Invalid GSM voice/data services %d, %d",tch_control,channel_mode);
          ghdi_status = GHDI_FAILURE;
          break;
     }
  
     if(local_ghdi_mvs_status == GHDI_MVS_ACQUIRE)
     {
  
  #ifdef FEATURE_INTER_RAT_HO_OPT
  
  #ifdef FEATURE_SEGMENT_LOADING
  
  #if defined FEATURE_DUAL_SIM
         if(gl1_msg_is_l1_vocoder_control_enabled_multisim(ghdi_get_mmode_subs_asid()))
         {
             MSG_HIGH_1("GSM L1 controlling the vocoder = %d", gl1_msg_is_l1_vocoder_control_enabled_multisim(ghdi_get_mmode_subs_asid()));
         }
  #else
         if(gl1_msg_is_l1_vocoder_control_enabled())
         {
             MSG_HIGH_1("GSM L1 controlling the vocoder = %d", gl1_msg_is_l1_vocoder_control_enabled());
         }
  #endif /* FEATURE_DUAL_SIM */
  
  #if defined FEATURE_DUAL_SIM
         else if ((ptr_W != NULL)&&IWCDMA_rrc_vocoder_control_enabled_sub(ptr_W,ghdi_get_mmode_subs_asid()))          
         {
             MSG_HIGH_1("RRC controlling the vocoder = %d", IWCDMA_rrc_vocoder_control_enabled_sub(ptr_W,ghdi_get_mmode_subs_asid()));
         }
  #else
         else if ((ptr_W != NULL)&&IWCDMA_rrc_vocoder_control_enabled(ptr_W))          
         {
             MSG_HIGH_1("RRC controlling the vocoder = %d", IWCDMA_rrc_vocoder_control_enabled(ptr_W));
         }
  #endif /* FEATURE_DUAL_SIM */
  
  #ifdef FEATURE_TDSCDMA
  #if defined FEATURE_DUAL_SIM
         else if ((ptr_T != NULL)&&ITDSCDMA_tdsrrc_vocoder_control_enabled_sub(ptr_T,ghdi_get_mmode_subs_asid()))          
         {
             MSG_HIGH_1( "TDSRRC controlling the vocoder = %d", ITDSCDMA_tdsrrc_vocoder_control_enabled_sub(ptr_T,ghdi_get_mmode_subs_asid()));
         }
  #else
         else if ((ptr_T != NULL)&&ITDSCDMA_tdsrrc_vocoder_control_enabled(ptr_T))          
         {
             MSG_HIGH_1( "TDSRRC controlling the vocoder = %d", ITDSCDMA_tdsrrc_vocoder_control_enabled(ptr_T));
         }
  #endif /* FEATURE_DUAL_SIM */
  #endif /* FEATURE_TDSCDMA */
  
  #else /* seg loading */
        
  #if defined FEATURE_DUAL_SIM
         if(gl1_msg_is_l1_vocoder_control_enabled_multisim(ghdi_get_mmode_subs_asid()))
         {
             MSG_HIGH_1("GSM L1 controlling the vocoder = %d", gl1_msg_is_l1_vocoder_control_enabled_multisim(ghdi_get_mmode_subs_asid()));
         }
  #else
         if(gl1_msg_is_l1_vocoder_control_enabled())
         {
             MSG_HIGH_1("GSM L1 controlling the vocoder = %d", gl1_msg_is_l1_vocoder_control_enabled());
         }
  #endif
  
  #if defined FEATURE_DUAL_SIM
         else if(rrc_vocoder_control_enabled_sub(ghdi_get_mmode_subs_asid()))
         {
             MSG_HIGH_1("RRC controlling the vocoder = %d", rrc_vocoder_control_enabled_sub(ghdi_get_mmode_subs_asid()));
         }       
  #else
         else if(rrc_vocoder_control_enabled())
         {
             MSG_HIGH_1("RRC controlling the vocoder = %d", rrc_vocoder_control_enabled());
         }
         
  #endif
  
  #ifdef FEATURE_TDSCDMA
  #if defined FEATURE_DUAL_SIM
         else if (tdsrrc_vocoder_control_enabled_sub(ghdi_get_mmode_subs_asid()))
         {
             MSG_HIGH_1( "TDSRRC controlling the vocoder = %d", tdsrrc_vocoder_control_enabled_sub(ghdi_get_mmode_subs_asid()));
         }
  #else
         else if (tdsrrc_vocoder_control_enabled())
         {
             MSG_HIGH_1( "TDSRRC controlling the vocoder = %d", tdsrrc_vocoder_control_enabled());
         }
  #endif   
  #endif /* FEATURE_TDSCDMA */
  #endif /* seg loading*/
  
  
         else
  #endif      
         { 
             /* Sequence.1 */
             MSG_HIGH_2("NAS MVS release (GSM), GHDI status=%d, local_ghdi_mvs_status = %d",ghdi_mvs_status,local_ghdi_mvs_status);   
             ghdi_mvs_release(MVS_CLIENT_GSM);
  
             /* Sequence.2 */
             MSG_HIGH_2("NAS MVS release (WCDMA), GHDI status=%d, local_ghdi_mvs_status = %d",ghdi_mvs_status,local_ghdi_mvs_status);   
             ghdi_mvs_release(MVS_CLIENT_WCDMA);  /* pre-caution client */
  
  #ifdef FEATURE_TDSCDMA
             MSG_HIGH_2("NAS MVS release (TDSCDMA), GHDI status=%d, local_ghdi_mvs_status = %d",ghdi_mvs_status,local_ghdi_mvs_status);   
             ghdi_mvs_release(MVS_CLIENT_TDSCDMA);  /* pre-caution client */
  #endif /* FEATURE_TDSCDMA */
  
  
  #if defined(FEATURE_QCHAT) && defined(FEATURE_WCDMA)
  
             /* Sequence.1 */
             MSG_HIGH_1("NAS MVS release (MVS_CLIENT_QCHAT), GHDI status=%d",ghdi_mvs_status);   
             ghdi_mvs_status = GHDI_MVS_CHANGING;
             ghdi_send_event_ghdi_mvs_state(EVENT_MVS_RELEASE_CALLED, MVS_CLIENT_QCHAT,
                                            ghdi_mvs_status, ghdi_mvs_mode);
  
             mvs_release(MVS_CLIENT_QCHAT);
  #endif                 
  
  #if !defined(FEATURE_NAS_NO_QVP_VOIP_SUPPORT_VIA_MVS)
             /* Sequence.3 */
             MSG_HIGH_2("NAS MVS release (QVP), GHDI status=%d, local_ghdi_mvs_status = %d",ghdi_mvs_status,local_ghdi_mvs_status);   
             ghdi_mvs_status = GHDI_MVS_CHANGING;
             ghdi_send_event_ghdi_mvs_state(EVENT_MVS_RELEASE_CALLED, MVS_CLIENT_QVP_TEST,
                                            ghdi_mvs_status, ghdi_mvs_mode);
  
             mvs_release(MVS_CLIENT_QVP_TEST);  /* pre-caution client */
  #endif /* !defined(FEATURE_NAS_NO_QVP_VOIP_SUPPORT_VIA_MVS */
  
  #ifdef FEATURE_VOIP
  #if !defined(FEATURE_NAS_NO_QVP_VOIP_SUPPORT_VIA_MVS)
             MSG_HIGH_2("NAS MVS release (VOIP), GHDI status=%d, local_ghdi_mvs_status = %d",ghdi_mvs_status,local_ghdi_mvs_status);   
             ghdi_mvs_status = GHDI_MVS_CHANGING;
             ghdi_send_event_ghdi_mvs_state(EVENT_MVS_RELEASE_CALLED, MVS_CLIENT_VOIP,
                                            ghdi_mvs_status, ghdi_mvs_mode);
  
             mvs_release(MVS_CLIENT_VOIP);  /* pre-caution client */
  #endif /* !defined(FEATURE_NAS_NO_QVP_VOIP_SUPPORT_VIA_MVS) */                         
  
  #endif   /* FEATURE_VOIP */
             /* Sequence.4 */
             MSG_HIGH_2("NAS MVS release (GSM), GHDI status=%d, local_ghdi_mvs_status = %d",ghdi_mvs_status,local_ghdi_mvs_status);
  #ifdef FEATURE_GSM
             ghdi_mvs_ul_proc = &ghdi_mvs_gsm_ul_proc;
             ghdi_mvs_dl_proc = &ghdi_mvs_gsm_dl_proc;
  #endif /*FEATURE_GSM*/
             MSG_HIGH_1("vocoder being acquired by cleint = %d",MVS_CLIENT_GSM);
             ghdi_mvs_acquire(MVS_CLIENT_GSM);
  #ifdef FEATURE_GSM
             mvssup_gsm_set_dtx_mode(ghdi_dtx_mode);
  #endif /*FEATURE_GSM*/
  
             ghdi_status = GHDI_SUCCESS;
         } /* gl1 or rrc control */
     } /* GHDI_MVS_ACQUIRE */
     else 
     {
         MSG_HIGH_2("NAS releasing all MVS_CLIENT in GSM, local GHDI status=%d, ghdi_mvs_status = %d",local_ghdi_mvs_status,ghdi_mvs_status);
  
         ghdi_mvs_release(MVS_CLIENT_GSM);
  
         ghdi_mvs_release(MVS_CLIENT_WCDMA);  /* pre-caution client */
  
  #ifdef FEATURE_TDSCDMA
         ghdi_mvs_release(MVS_CLIENT_TDSCDMA);  /* pre-caution client */
  #endif /* FEATURE_TDSCDMA */
  
        /* resetting the variable during the call release */
         mvs_acquire_pending = FALSE;
         acquire_pending_client_id = MVS_CLIENT_NONE;
     }  /* GHDI_MVS_CHANGING */
  
  #endif  /* FEATURE_GSM */
  
     ghdi_mvs_acquire_counter = 0;
    }
    else
    {
      MSG_HIGH_2("NAS: tch_control %d channel_mode %d", tch_control, channel_mode);
  
  #ifdef FEATURE_GSM
      if((tch_control == GHDI_TURN_SPEECH_TCH_F_ON) || 
         (tch_control == GHDI_TURN_SPEECH_TCH_H_0_ON) ||
         (tch_control == GHDI_TURN_SPEECH_TCH_H_1_ON))
      {
         switch(channel_mode)
         {
             case SPEECH_V1 :
                 /* This is normal/default Full Rate. Do nothing  */
                 break;
  
             case SPEECH_V2 :
                 /* This is Full Rate Speech Version 2, aka EFR */
                 tch_control = GHDI_TURN_SPEECH_TCH_EF_ON;
                 break;
  
             case SPEECH_V3 :
                 tch_control = GHDI_TURN_SPEECH_TCH_AMR_ON;
                 break;
  
  #ifdef FEATURE_GSM_AMR_WB
             case SPEECH_V5:
                 tch_control = GHDI_TURN_SPEECH_TCH_AMR_WB_ON;
                 break;
  #endif               
  
             default :
                 MSG_ERROR_0("Invalid/unsupported channel_mode = %d for speech");
                 ghdi_status = GHDI_FAILURE;
                 break;
         }
      }
     
      else if((tch_control == GHDI_TURN_DATA_TCH_F_ON) || 
              (tch_control == GHDI_TURN_DATA_TCH_H_0_ON) ||
              (tch_control == GHDI_TURN_DATA_TCH_H_1_ON))
      {
         MSG_HIGH_1("Connecting CS DATA TCH = %d", tch_control); 
      }
      
      else
      {
         MSG_ERROR_2("Invalid TCH rate=%d, mode=%d", tch_control, channel_mode);
      }
      
     /*lint -e546 */
      switch(tch_control)
      {
        case GHDI_TURN_SPEECH_TCH_F_ON :
          ghdi_mvs_mode = MVS_MODE_FR;
          MSG_HIGH_1("Acquiring FR vocoder, mode =%d",channel_mode);
          break;
  
        case GHDI_TURN_SPEECH_TCH_EF_ON :
          ghdi_mvs_mode = MVS_MODE_EFR;
          MSG_HIGH_1("Acquiring EFR Vocoder, mode =%d",channel_mode);
          break;
  
        case GHDI_TURN_SPEECH_TCH_H_0_ON:
        case GHDI_TURN_SPEECH_TCH_H_1_ON:
  #ifdef FEATURE_GSM_HALF_RATE 
          /* enable MVS Half Rate */
          ghdi_mvs_mode = MVS_MODE_HR;
          MSG_HIGH_2("Acquiring HALF Rate Vocoder,type=%d,mode=%d",tch_control,channel_mode);
  #else
          MSG_HIGH_2("HALF Rate Vocoder not supported,type=%d,mode=%d",tch_control,channel_mode);
  #endif /* FEATURE_GSM_HALF_RATE */
          break;
  
        case GHDI_TURN_SPEECH_TCH_AMR_ON:
  #ifdef FEATURE_GSM_AMR
          /* enable MVS AMR */
          ghdi_mvs_mode = MVS_MODE_AMR;
          MSG_HIGH_2("Acquiring GSM AMR, type=%d,mode=%d",tch_control,channel_mode);
  #else
          MSG_ERROR_2("Unsupported GSM AMR, type=%d,mode=%d",tch_control,channel_mode);
  #endif /* FEATURE_GSM_ARM */
          break;
  
        case GHDI_TURN_SPEECH_TCH_AMR_WB_ON:
  #if defined (FEATURE_GSM_AMR) && defined (FEATURE_GSM_AMR_WB)
          /* enable MVS AMR */
          ghdi_mvs_mode = MVS_MODE_AMR_WB;
          MSG_HIGH_2("Acquiring GSM AMR WB, type=%d,mode=%d",tch_control,channel_mode);
  #else
          MSG_ERROR_2("Unsupported GSM AMR WB, type=%d,mode=%d",tch_control,channel_mode);
  #endif /* FEATURE_GSM_ARM */
          break;
  
        default :
          MSG_ERROR_2("Invalid GSM voice/data services %d, %d",tch_control,channel_mode);
          ghdi_status = GHDI_FAILURE;
          break;
      }   
  #endif/* FEATURE_GSM */ 
    MSG_HIGH_0("NAS updating codec"); 
    }
  }
  return ghdi_status;
}

/*******************************************************************************
 *
 *  Function name: ghdi_control_umts_voice()
 *  --------------
 *  Description:
 *  ------------
 *  Controls (enabeling and disabling) the vocoder services in WCDMA mode.
 *
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 ******************************************************************************/
#ifdef FEATURE_VOC_AMR_WB 
#ifdef FEATURE_CS_VOICE_OVER_HSPA
ghdi_status_T ghdi_control_umts_voice
(
   ghdi_umts_voice_control_T voice_control,
   umts_codecs_list_T channel_mode,
   umts_wcdma_cs_voice_bearer_enum_T rab_type,
   sys_radio_access_tech_e_type current_rat
)
#else
ghdi_status_T ghdi_control_umts_voice
(
   ghdi_umts_voice_control_T  voice_control,
   umts_codecs_list_T channel_mode,
   sys_radio_access_tech_e_type current_rat
)
#endif
#else
#ifdef FEATURE_CS_VOICE_OVER_HSPA
ghdi_status_T ghdi_control_umts_voice
(
   ghdi_umts_voice_control_T voice_control,
   umts_wcdma_cs_voice_bearer_enum_T rab_type,
   sys_radio_access_tech_e_type current_rat
)
#else
ghdi_status_T ghdi_control_umts_voice
(
   ghdi_umts_voice_control_T  voice_control,
   sys_radio_access_tech_e_type current_rat
)
#endif
#endif 
{
   ghdi_status_T   ghdi_status = GHDI_SUCCESS;

   if(is_voice_supported == TRUE)
   {
  
  
  #ifdef FEATURE_WCDMA
     switch(voice_control)
     {
        case GHDI_TURN_AMR_ON :  /* UMTS/WCDMA AMR */
        {
  #ifdef FEATURE_INTER_RAT_HO_OPT
            if (!print_vocoders_enabled_single ())
  #endif    /* FEATURE_INTER_RAT_HO_OPT */      
            { 
                /* Sequence.1 */
                MSG_HIGH_1("NAS MVS release(WCDMA), GHDI status=%d",ghdi_mvs_status);   
                ghdi_mvs_release(MVS_CLIENT_WCDMA);
  #ifdef FEATURE_TDSCDMA
                MSG_HIGH_1("NAS MVS release(TD-SCDMA), GHDI status=%d",ghdi_mvs_status);   
                ghdi_mvs_release(MVS_CLIENT_TDSCDMA);
  #endif /* FEATURE_TDSCDMA */
  #ifdef FEATURE_INTER_RAT_HO_OPT   
                if (!print_vocoders_enabled_combined ())
  #endif      /* FEATURE_INTER_RAT_HO_OPT */        
                {
                    /* Sequence.2 */
                    MSG_HIGH_1("NAS MVS release(GSM), GHDI status=%d",ghdi_mvs_status);   
                    ghdi_mvs_release(MVS_CLIENT_GSM);  /* pre-caution client */

  #ifdef FEATURE_INTER_RAT_HO_OPT 
                    if (!print_vocoders_enabled_combined ())
  #endif                  
                    {
  #if defined(FEATURE_QCHAT) && defined(FEATURE_WCDMA)
                        /* Sequence.2 */
                        MSG_HIGH_1("NAS MVS release(MVS_CLIENT_QCHAT), GHDI status=%d",ghdi_mvs_status);    
                        ghdi_mvs_status = GHDI_MVS_CHANGING;
                        ghdi_send_event_ghdi_mvs_state(EVENT_MVS_RELEASE_CALLED, MVS_CLIENT_QCHAT,
                                                       ghdi_mvs_status, ghdi_mvs_mode);
  
                        mvs_release(MVS_CLIENT_QCHAT);  /* pre-caution client */
  #endif  

  #ifdef FEATURE_INTER_RAT_HO_OPT                  
                        if (!print_vocoders_enabled_combined ())
  #endif                      
                        { 
  
  #if !defined(FEATURE_NAS_NO_QVP_VOIP_SUPPORT_VIA_MVS)
                            /* Sequence.3 */
                            MSG_HIGH_1("NAS MVS release (QVP), GHDI status=%d",ghdi_mvs_status);   
                            ghdi_mvs_status = GHDI_MVS_CHANGING;
                            ghdi_send_event_ghdi_mvs_state(EVENT_MVS_RELEASE_CALLED, MVS_CLIENT_QVP_TEST,
                                                           ghdi_mvs_status, ghdi_mvs_mode);
  
                            mvs_release(MVS_CLIENT_QVP_TEST);  /* pre-caution client */
  #endif /* !defined(FEATURE_NAS_NO_QVP_VOIP_SUPPORT_VIA_MVS) */
   

  #ifdef FEATURE_INTER_RAT_HO_OPT
                            if (!print_vocoders_enabled_combined ())
  #endif                          
                            { 
#ifdef FEATURE_VOIP
#if !defined(FEATURE_NAS_NO_QVP_VOIP_SUPPORT_VIA_MVS)
                              MSG_HIGH_1("NAS MVS release (VOIP), GHDI status=%d",ghdi_mvs_status);   
                              ghdi_mvs_status = GHDI_MVS_CHANGING;
                              ghdi_send_event_ghdi_mvs_state(EVENT_MVS_RELEASE_CALLED, MVS_CLIENT_VOIP, ghdi_mvs_status, ghdi_mvs_mode);
  
                              mvs_release(MVS_CLIENT_VOIP);  /* pre-caution client */
#endif /* #if !defined(FEATURE_NAS_NO_QVP_VOIP_SUPPORT_VIA_MVS) */                            
#endif /* FEATURE_VOIP */


#ifdef FEATURE_INTER_RAT_HO_OPT
                             if (!print_vocoders_enabled_combined ())
#endif                              


                              {    
                                /* Sequence.4 */
                                MSG_HIGH_1("NAS acquiring MVS_UMTS_CLIENT, GHDI status = %d",ghdi_mvs_status);
#ifdef FEATURE_VOC_AMR_WB 
                                if(channel_mode == UMTS_WB_AMR)
                                {
                                   ghdi_mvs_mode = MVS_MODE_AMR_WB;
                                }
                                else
                                {
                                   ghdi_mvs_mode = MVS_MODE_AMR;
                                }
#else
                                ghdi_mvs_mode = MVS_MODE_AMR;
#endif
#ifdef FEATURE_CS_VOICE_OVER_HSPA
                                if(rab_type == UMTS_WCDMA_CS_VOICE_OVER_HSPA)
                                {
                                  ghdi_mvs_ul_proc = ghdi_mvs_CSovrHSPA_ul_proc;
                                  ghdi_mvs_dl_proc = ghdi_mvs_CSovrHSPA_dl_proc;                              
                                }
                                else
                                {
                                  ghdi_mvs_ul_proc = &ghdi_amr_ul_cb;
                                  ghdi_mvs_dl_proc = &ghdi_amr_dl_cb;
                                }
#else
                                ghdi_mvs_ul_proc = &ghdi_amr_ul_cb;
                                ghdi_mvs_dl_proc = &ghdi_amr_dl_cb;
#endif                              
                                if (current_rat == SYS_RAT_UMTS_RADIO_ACCESS)
                                {
                                  
                                  MSG_HIGH_1("vocoder being acquired by client = %d",MVS_CLIENT_WCDMA);
  
                                ghdi_mvs_acquire( MVS_CLIENT_WCDMA );
                                }
                                else if (current_rat == SYS_RAT_TDS_RADIO_ACCESS)
                                {
#ifdef FEATURE_TDSCDMA
                                  
                                  MSG_HIGH_1("vocoder being acquired by client = %d",MVS_CLIENT_TDSCDMA);
  
                                ghdi_mvs_acquire( MVS_CLIENT_TDSCDMA );
#endif /* FEATURE_TDSCDMA */
                                }
                                else
                                {
                                  ERR_FATAL( "Invalid RAT = %d", (int) current_rat, 0,0 );
                                }
  
                                
                                
                                ghdi_status = GHDI_SUCCESS;
                              } 
        
                            }
  
                        } /* Sequence.3 */
  
                    } /* Sequence.2 */
  
                } /* Sequence.1 */
  
            } /* gl1 or rrc control */
        } /* GHDI_TURN_AMR_ON */
        break;
  
        case GHDI_TURN_AMR_OFF :
        {
              MSG_HIGH_1("NAS releasing all MVS_CLIENT in GHDI status = %d",ghdi_mvs_status);
  
              ghdi_mvs_release(MVS_CLIENT_WCDMA);
  
#ifdef FEATURE_TDSCDMA
              ghdi_mvs_release(MVS_CLIENT_TDSCDMA);
#endif /* FEATURE_TDSCDMA */
  
              ghdi_mvs_release(MVS_CLIENT_GSM);
  
              ghdi_status = GHDI_SUCCESS;
              /* resetting the variable during the call release */
              mvs_acquire_pending = FALSE;
              acquire_pending_client_id = MVS_CLIENT_NONE;
        }
        break;
  
        default :
        {
              ERR("GHDI ERROR: Invalid voice services command",0,0,0);
        }
        break;
     }
#endif /* FEATURE_WCDMA */
  
     ghdi_mvs_acquire_counter = 0;
  
  }
   return ghdi_status;
}

/*******************************************************************************
 *
 *  Function name: ghdi_mvs_acquire()
 *  --------------
 *  Description:
 *  ------------
 *    Performed MVS acquire, mvs_acquired(), and set the corresponding 
 *    GHDI state that is used by the registered client and related 
 *    call back function, ghdi_mvs_cb_func().  
 * 
 *    The same call back function is used to synchronize various multi-media 
 *    vocoder services (MVS), that is perform by different software layer 
 *    including: NAS, GSM (GL1), and WCDMA (RRC).
 *
 *    The MVS interface functions are used to initialize, configured, and 
 *    released the vocoder at various phase of the voice call during a
 *    call setup, call clearing, intra-handover, inter-handover, and inter-RAT 
 *    handover.   
 * 
 *    The MVS functions that is used by GHDI (Generic Hardware Driver Interface 
 *    Fucntion) includes:
 *
 *        mvs_acquire() - Register the client + call back function.
 *        mvs_enable()  - Configured the registered client.
 *        mvs_release() - Release or de-Register the client.  
 *
 *    The MVS would returns the registered call back function for given client  
 *    on every MVS function call.
 *
 *  Parameters:
 *  -----------
 *    mvs_client_type client
 * 
 *  Returns:
 *  --------
 *
 ******************************************************************************/
void ghdi_mvs_acquire(mvs_client_type client)
{
  if(is_voice_supported == TRUE)
  {
    NAS_ENTER_CRIT_SECT(ghdi_mvs_crit_sec);
  
    if (atomic_compare_and_set(&mvs_callback_pending, FALSE, TRUE))
    {
  
      MSG_HIGH_3("ghdi_mvs_acquire() client=%d, status=%d, mode=%d", client, ghdi_mvs_status, ghdi_mvs_mode);
  
      ghdi_mvs_status = GHDI_MVS_ACQUIRE;
      vocoder_acquired_by_client = client;
  
      ghdi_send_event_ghdi_mvs_state(EVENT_MVS_ACQUIRE_CALLED, client, ghdi_mvs_status, ghdi_mvs_mode);
  
      mvs_acquire(client, ghdi_mvs_cb_func);
      ghdi_mvs_acquire_counter = 0;
  
  #ifdef FEATURE_GSM
      if(client == MVS_CLIENT_GSM)
      {
        mvssup_gsm_set_dtx_mode(ghdi_dtx_mode);  /* GSM only */
      }
  #endif /*FEATURE_GSM*/
  
    }
    else
    {
      mvs_acquire_pending = TRUE;
      acquire_pending_client_id = client;
      MSG_HIGH_2("acquire pended for cli = %d, CB pending =%d", client,mvs_callback_pending.value);
    }
    NAS_EXIT_CRIT_SECT(ghdi_mvs_crit_sec);
  
  }
}

/*******************************************************************************
 *
 *  Function name: ghdi_mvs_enable()
 *  --------------
 *  Description:
 *  ------------
 *  Performs the mvs_release() for each client and set the appropriated GHDI state.
 *
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 ******************************************************************************/
#ifdef FEATURE_CS_VOICE_OVER_HSPA
void ghdi_mvs_enable(mvs_client_type client, mvs_mode_type mode, umts_wcdma_cs_voice_bearer_enum_T rab_type)
#else
void ghdi_mvs_enable(mvs_client_type client, mvs_mode_type mode)
#endif
{
  if(is_voice_supported == TRUE)
  {
  
  
    NAS_ENTER_CRIT_SECT(ghdi_mvs_crit_sec);
    if (atomic_compare_and_set(&mvs_callback_pending, FALSE, TRUE))
    {
  #ifdef FEATURE_WCDMA  
      if(client == MVS_CLIENT_WCDMA)
      {
          MSG_HIGH_3("WCDMA enabling MVS, ghdi_mvs_enable() client=%d, status=%d, mode=%d", client,ghdi_mvs_status,mode);
  
          ghdi_mvs_mode = mode;
          ghdi_mvs_status = GHDI_MVS_MODE_SELECT;
  
  #ifdef FEATURE_CS_VOICE_OVER_HSPA
          if(rab_type == UMTS_WCDMA_CS_VOICE_OVER_HSPA)
          {
            ghdi_send_event_ghdi_mvs_state(EVENT_MVS_ENABLE_CALLED, client, ghdi_mvs_status, ghdi_mvs_mode);
            mvs_enable(client, mode, ghdi_mvs_CSovrHSPA_ul_proc, ghdi_mvs_CSovrHSPA_dl_proc, MVS_PKT_CONTEXT_ISR);
          }
          else if(rab_type == UMTS_WCDMA_CS_VOICE_OVER_DCH)
          {
            ghdi_send_event_ghdi_mvs_state(EVENT_MVS_ENABLE_CALLED, client, ghdi_mvs_status, ghdi_mvs_mode);
            mvs_enable(client, mode, ghdi_amr_ul_cb, ghdi_amr_dl_cb, MVS_PKT_CONTEXT_ISR);
          }
          else
          {
            MSG_HIGH_0("WRONG W-RAB Type");
          }
  #else
          ghdi_send_event_ghdi_mvs_state(EVENT_MVS_ENABLE_CALLED, client, ghdi_mvs_status, ghdi_mvs_mode);
  
          mvs_enable(client, mode, ghdi_amr_ul_cb, ghdi_amr_dl_cb, MVS_PKT_CONTEXT_ISR);
  #endif
      } else
  #endif  
  #ifdef FEATURE_TDSCDMA  
      if(client == MVS_CLIENT_TDSCDMA)
      {
          MSG_HIGH_3("TD-SCDMA enabling MVS, ghdi_mvs_enable() client=%d, status=%d, mode=%d", client,ghdi_mvs_status,mode);
  
          ghdi_mvs_mode = mode;
          ghdi_mvs_status = GHDI_MVS_MODE_SELECT;
  
  #ifdef FEATURE_CS_VOICE_OVER_HSPA
          if(rab_type == UMTS_WCDMA_CS_VOICE_OVER_HSPA)
          {
            ghdi_send_event_ghdi_mvs_state(EVENT_MVS_ENABLE_CALLED, client, ghdi_mvs_status, ghdi_mvs_mode);
            mvs_enable(client, mode, ghdi_mvs_CSovrHSPA_ul_proc, ghdi_mvs_CSovrHSPA_dl_proc, MVS_PKT_CONTEXT_ISR);
          }
          else if(rab_type == UMTS_WCDMA_CS_VOICE_OVER_DCH)
          {
            ghdi_send_event_ghdi_mvs_state(EVENT_MVS_ENABLE_CALLED, client, ghdi_mvs_status, ghdi_mvs_mode);
            mvs_enable(client, mode, ghdi_amr_ul_cb, ghdi_amr_dl_cb, MVS_PKT_CONTEXT_ISR);
          }
          else
          {
            MSG_HIGH_3("WRONG W-RAB Type");
          }
  #else
          ghdi_send_event_ghdi_mvs_state(EVENT_MVS_ENABLE_CALLED, client, ghdi_mvs_status, ghdi_mvs_mode);
          mvs_enable(client, mode, ghdi_umts_amr_ul_cb, ghdi_umts_amr_dl_cb, MVS_PKT_CONTEXT_ISR);
  #endif /* FEATURE_CS_VOICE_OVER_HSPA */
      } else
  #endif /* FEATURE_TDSCDMA */
      if(client == MVS_CLIENT_GSM)
      {
  #ifdef FEATURE_MVS_UNDER_GHDI    
          /* In inter RAT cases if config is done just before enabling on GSM side 
          the below code would take care */
  #ifdef FEATURE_VOC_AMR_WB        
          if(ghdi_amr_wb_ul_mode_config_modified == TRUE)
          {
              ghdi_amr_wb_ul_mode_config_modified = FALSE;
              mvssup_amr_set_awb_mode(ghdi_amr_wb_ul_mode);
          }
  #endif        
                       
          if(ghdi_amr_ul_mode_config_modified == TRUE)
          {
              ghdi_amr_ul_mode_config_modified = FALSE;
              mvssup_amr_set_amr_mode(ghdi_amr_ul_mode);
          }
  #endif        
  
          MSG_HIGH_3("GSM enabling MVS, ghdi_mvs_enable() client=%d, status=%d, mode=%d", client,ghdi_mvs_status,mode);
  
          ghdi_mvs_mode = mode;
          ghdi_mvs_status = GHDI_MVS_MODE_SELECT;
  
  #ifdef FEATURE_GSM
          ghdi_send_event_ghdi_mvs_state(EVENT_MVS_ENABLE_CALLED, client, ghdi_mvs_status, ghdi_mvs_mode);
  
          mvs_enable(client, mode, ghdi_mvs_gsm_ul_proc, ghdi_mvs_gsm_dl_proc, MVS_PKT_CONTEXT_ISR);
  #endif /*FEATURE_GSM*/
      }
      else
      {
          MSG_ERROR_1("GHDI unsupported client in ghdi_mvs_enable = %d", client);
          atomic_set(&mvs_callback_pending,FALSE);
      }
  
      ghdi_mvs_acquire_counter = 0;
    }
    else
    {
       mvs_enable_pending = TRUE;
       enable_pending_client_id = client;
       enable_pending_mode_id = mode;
  #ifdef FEATURE_CS_VOICE_OVER_HSPA
       enable_pending_rab_type = rab_type;
  #endif
  
  #ifdef FEATURE_CS_VOICE_OVER_HSPA
       MSG_HIGH_1("enable pended rab_type = %d", enable_pending_rab_type);
  #endif
       MSG_HIGH_3("enable pended for cli = %d, CB pending =%d, mode = %d", client,mvs_callback_pending.value,enable_pending_mode_id);
    }
    NAS_EXIT_CRIT_SECT(ghdi_mvs_crit_sec);
  }
}

/*******************************************************************************
 *
 *  Function name: ghdi_mvs_release()
 *  --------------
 *  Description:
 *  ------------
 *  Perform mvs_release() per given client and set the appropriate GHDI state.
 *
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 ******************************************************************************/
void ghdi_mvs_release(mvs_client_type client)
{

  if(is_voice_supported == TRUE)
  {
  
    boolean voc_client_release = FALSE;
  
    NAS_ENTER_CRIT_SECT(ghdi_mvs_crit_sec);
    if((vocoder_acquired_by_client == client) &&
       (!mvs_callback_pending.value))
    {
      mvs_callback_pending.value = TRUE;
      voc_client_release = TRUE;
      MSG_HIGH_1("Release callback pending is set to true client id = %d", client);
    }
  
    if (!mvs_callback_pending.value || voc_client_release)
    {
      if((client == MVS_CLIENT_WCDMA) ||
  #ifdef FEATURE_TDSCDMA
         (client == MVS_CLIENT_TDSCDMA) ||
  #endif /* FEATURE_TDSCDMA */
         (client == MVS_CLIENT_GSM))
      {
        MSG_HIGH_3("RRC or GSM L1 releasing MVS, ghdi_mvs_rel client=%d, status=%d, mode=%d", client, ghdi_mvs_status, ghdi_mvs_mode);
  
        ghdi_mvs_status = GHDI_MVS_CHANGING;
        ghdi_send_event_ghdi_mvs_state(EVENT_MVS_RELEASE_CALLED, client, ghdi_mvs_status, ghdi_mvs_mode);
  
        mvs_release(client);
      }
      else
      {
        MSG_ERROR_1("GHDI unsupported client=%d", client);
      }
  
      ghdi_mvs_acquire_counter = 0;
    }  
    else
    {
      if (client == MVS_CLIENT_WCDMA)
      {
        atomic_or(&mvs_clients_to_be_released, 0x1);
      }
      else if (client == MVS_CLIENT_GSM)
      {
        atomic_or(&mvs_clients_to_be_released, 0x2);
      }
  #ifdef FEATURE_TDSCDMA
      else if (client == MVS_CLIENT_TDSCDMA)
      {
        atomic_or(&mvs_clients_to_be_released, 0x4);
      }
  #endif /* FEATURE_TDSCDMA */
      MSG_HIGH_2("rel has been pended, clients = %d , callback_pending = %d", mvs_clients_to_be_released.value, mvs_callback_pending.value);
    }
    NAS_EXIT_CRIT_SECT(ghdi_mvs_crit_sec);
  }
}

/* lint +e546 */

/*
 * Function name :  ghdi_control_tch_loopback()
 * -------------
 *
 * Description :
 * -----------
 *   Used for loopback debuging and/or testing.
 *
 * Uses :
 * ----
 *
 * Parameters:
 * ----------
 *
 * Returns:
 * --------
 *
 */

ghdi_status_T ghdi_control_tch_loopback( loopback_type_T  loopback_type,
                                         tch_T            tch )
{
   ghdi_status_T ghdi_status = GHDI_SUCCESS;

#ifdef FEATURE_GSM
   

   MSG_MED_2("LIB-GHDI: tch loopback %d, %d", loopback_type,tch);

   (void)gl1_msg_tch_loopback((gl1_defs_loopback_type)loopback_type); 
#endif

   return ghdi_status;
}

/*******************************************************************************
 *
 *  Function name: ghdi_mvs_standby(mvs_client_type , mvs_mode_type )
 *  --------------
 *  Description:
 *  ------------
 *  This function .
 *
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 ******************************************************************************/
void ghdi_mvs_standby (mvs_client_type client, mvs_mode_type mode)
{

  if(is_voice_supported == TRUE)
  {
    NAS_ENTER_CRIT_SECT(ghdi_mvs_crit_sec);
    if (atomic_compare_and_set(&mvs_callback_pending, FALSE, TRUE))
    {
      ghdi_mvs_status = GHDI_MVS_STANDBY;
      MSG_HIGH_2("ghdi_stdby() mode %x, GHDI staus = %d",mode,ghdi_mvs_status);
      ghdi_send_event_ghdi_mvs_state(EVENT_MVS_STANDBY_CALLED, client, ghdi_mvs_status, ghdi_mvs_mode);
  
      mvs_standby (client, mode);
    }
    else
    {
       mvs_standby_pending = TRUE;
       standby_pending_client_id = client;
       standby_pending_mode_id = mode;
       MSG_HIGH_3("standby pended for cli = %d, CB pending =%d, mode = %d", client,mvs_callback_pending.value,standby_pending_mode_id);
    }
    NAS_EXIT_CRIT_SECT(ghdi_mvs_crit_sec);
  
  }

}

#ifdef FEATURE_MVS_UNDER_GHDI
/********************************************************************************
 * Function name :  ghdi_set_gsm_dtx_mode()
 * -------------
 *
 * Description :
 * -----------
 *   Used to set the GSM DTX mode.
 *
 * Uses :
 * ----
 *
 * Parameters:
 * ----------
 *
 * Returns:
 * --------
 *
 ********************************************************************************/
void ghdi_gsm_set_dtx_mode(mvs_dtx_mode_type dtx_mode)
{

  if(is_voice_supported == TRUE)
  {
  
  
#ifdef FEATURE_GSM
     ghdi_dtx_mode =  dtx_mode;
#endif
     mvssup_gsm_set_dtx_mode(dtx_mode);   
  }
}
#else
/********************************************************************************
 * Function name :  ghdi_set_gsm_dtx_mode()
 * -------------
 *
 * Description :
 * -----------
 *   Used to set the GSM DTX mode.
 *
 * Uses :
 * ----
 *
 * Parameters:
 * ----------
 *
 * Returns:
 * --------
 *
 ********************************************************************************/
void ghdi_gsm_set_dtx_mode(boolean dtx_mode)
{
#ifdef FEATURE_GSM
    if(dtx_mode)
        ghdi_dtx_mode =  MVS_DTX_ON;
    else
        ghdi_dtx_mode =  MVS_DTX_OFF;
#endif
}
#endif /* #ifdef FEATURE_MVS_UNDER_GHDI */

#ifdef FEATURE_MVS_UNDER_GHDI
#ifdef FEATURE_WCDMA
/*******************************************************************************
 *
 *  Function name: ghdi_mvs_wcdma_set_ul_channel()
 *  --------------
 *  Description:
 *  ------------
 *  This function stores the MVS UL configuration parameters until the VOCODER is acquired and
 *  forwards the same to MVS after the VOCODER Acquisition. If VOCODER is already acquired
 *  then it immediately passes on the configuration to the MVS.
 *
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 ******************************************************************************/
void ghdi_mvs_wcdma_set_ul_channel (
  rlc_lc_id_type lc_class_a,
  rlc_lc_id_type lc_class_b,
  rlc_lc_id_type lc_class_c,
  mvssup_wcdma_chan_class_type chan_class
)
{

   ul_lc_class_a = lc_class_a;
   ul_lc_class_b = lc_class_b;
   ul_lc_class_c = lc_class_c;
   ul_chan_class = chan_class;

   MSG_HIGH_3("GSVoiceif :: Storing W UL conf A %x, B %x, C %x",lc_class_a,lc_class_b,lc_class_c);
   MSG_HIGH_1("GSVoiceif :: chan_class %d",chan_class);
   if( (ghdi_mvs_status == GHDI_MVS_MODE_SELECT  ||
       ghdi_mvs_status == GHDI_MVS_ON) &&
       (vocoder_acquired_by_client == MVS_CLIENT_WCDMA)
       )
   {
    /* Since the VOC is already acquired, Pass the Configuration to MVS 
         And after this configure the WL1 */
      ghdi_configure_mvs_wcdma_ul_channel();
   }

}

/*******************************************************************************
 *
 *  Function name: ghdi_configure_mvs_wcdma_ul_channel(void)
 *  --------------
 *  Description:
 *  ------------
 *  Perform ghdi_configure_mvs_wcdma_ul_channel() per given client and set the appropriate GHDI state.
 *
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 ******************************************************************************/
void ghdi_configure_mvs_wcdma_ul_channel (void)
{

  if(is_voice_supported == TRUE)
  {
  
     {
        ghdi_send_event_ghdi_mvs_state(EVENT_MVS_CONFIGURE_MVS_UL_CHANNEL, MVS_CLIENT_WCDMA, ghdi_mvs_status, ghdi_mvs_mode);
        mvssup_wcdma_set_ul_channel(ul_lc_class_a,
                                      ul_lc_class_b,
                                      ul_lc_class_c,
                                      ul_chan_class);
        
        MSG_HIGH_0("GSVoiceif :: Wcdma UL config");
     }
  }
}


/*******************************************************************************
 *
 *  Function name: ghdi_mvs_wcdma_set_dl_channel()
 *  --------------
 *  Description:
 *  ------------
 *  Perform mvs_release() per given client and set the appropriate GHDI state.
 *
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 ******************************************************************************/
void ghdi_mvs_wcdma_set_dl_channel(
  rlc_lc_id_type lc_class_a,
  rlc_lc_id_type lc_class_b,
  rlc_lc_id_type lc_class_c,
  mvssup_wcdma_chan_class_type chan_class
)
{

   dl_lc_class_a = lc_class_a;
   dl_lc_class_b = lc_class_b;
   dl_lc_class_c = lc_class_c;
   dl_chan_class = chan_class;   

   MSG_HIGH_3("GSVoiceif :: Storing W DL conf A %x, B %x, C %x",lc_class_a,lc_class_b,lc_class_c);
   MSG_HIGH_1("GSVoiceif :: chan_class %d",chan_class);
   if( (ghdi_mvs_status == GHDI_MVS_MODE_SELECT  ||
       ghdi_mvs_status == GHDI_MVS_ON) &&
       (vocoder_acquired_by_client == MVS_CLIENT_WCDMA)
       )
   {
    /* Since the VOC is already acquired, Pass the Configuration to MVS 
         And after this configure the WL1 */
      ghdi_configure_mvs_wcdma_dl_channel();
   }

}


/*******************************************************************************
 *
 *  Function name: ghdi_configure_mvs_wcdma_dl_channel(void)
 *  --------------
 *  Description:
 *  ------------
 *  Perform ghdi_configure_mvs_wcdma_dl_channel() per given client and set the appropriate GHDI state.
 *
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 ******************************************************************************/
void ghdi_configure_mvs_wcdma_dl_channel (void)
{

  if(is_voice_supported == TRUE)
  {
  
     {
  
       ghdi_send_event_ghdi_mvs_state(EVENT_MVS_CONFIGURE_MVS_DL_CHANNEL, MVS_CLIENT_WCDMA, ghdi_mvs_status, ghdi_mvs_mode);
  
#ifndef FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING
        mvssup_wcdma_set_dl_channel(dl_lc_class_a,
                                    dl_lc_class_b,
                                    dl_lc_class_c,
                                    dl_chan_class);
#else /* FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING */
        mvssup_wcdma_set_dl_channel(dl_lc_class_a,
                                    dl_lc_class_b,
                                    dl_lc_class_c,
                                    dl_chan_class,
                                    &dl_blk_size_cb_set);
  
        rrc_register_mvs_callback_to_l1(&dl_blk_size_cb_set);
#endif  /* FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING */         
        
        MSG_HIGH_0("GSVoiceif :: Wcdma DL config");
     }
     
  }
}
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_TDSCDMA
/*******************************************************************************
 *
 *  Function name: ghdi_mvs_tdscdma_set_ul_channel()
 *  --------------
 *  Description:
 *  ------------
 *  This function stores the MVS UL configuration parameters until the VOCODER is acquired and
 *  forwards the same to MVS after the VOCODER Acquisition. If VOCODER is already acquired
 *  then it immediately passes on the configuration to the MVS.
 *
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 ******************************************************************************/
void ghdi_mvs_tdscdma_set_ul_channel
(
  rlc_lc_id_type lc_class_a,
  rlc_lc_id_type lc_class_b,
  rlc_lc_id_type lc_class_c,
  mvssup_umts_chan_class_type chan_class
)
{
   ul_lc_class_a = lc_class_a;
   ul_lc_class_b = lc_class_b;
   ul_lc_class_c = lc_class_c;
   umts_ul_chan_class = chan_class;

   MSG_HIGH_3("GSVoiceif :: Storing TDS UL conf A %x, B %x, C %x",lc_class_a,lc_class_b,lc_class_c);
   MSG_HIGH_1("GSVoiceif :: chan_class %d",chan_class);
   if( (ghdi_mvs_status == GHDI_MVS_MODE_SELECT  ||
       ghdi_mvs_status == GHDI_MVS_ON) &&
       (vocoder_acquired_by_client == MVS_CLIENT_TDSCDMA)
       )
   {
    /* Since the VOC is already acquired, Pass the Configuration to MVS 
         And after this configure the WL1 */
      ghdi_configure_mvs_tdscdma_ul_channel();
   }
}
/*******************************************************************************
 *
 *  Function name: ghdi_configure_mvs_tdscdma_ul_channel(void)
 *  --------------
 *  Description:
 *  ------------
 *  Perform ghdi_configure_mvs_tdscdma_ul_channel() per given client
    and set the appropriate GHDI state.
 *
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 ******************************************************************************/
void ghdi_configure_mvs_tdscdma_ul_channel(void)
{

   if(is_voice_supported == TRUE)
   {
      mvssup_tdscdma_set_ul_channel(ul_lc_class_a,
                                    ul_lc_class_b,
                                    ul_lc_class_c,
                                    umts_ul_chan_class);
      
      MSG_HIGH_0("GSVoiceif :: TS-SCDMA UL config");
   }

}


/*******************************************************************************
 *
 *  Function name: ghdi_mvs_tdscdma_set_dl_channel()
 *  --------------
 *  Description:
 *  ------------
 *  Perform mvs_release() per given client and set the appropriate GHDI state.
 *
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 ******************************************************************************/
void ghdi_mvs_tdscdma_set_dl_channel
(
  rlc_lc_id_type lc_class_a,
  rlc_lc_id_type lc_class_b,
  rlc_lc_id_type lc_class_c,
  mvssup_umts_chan_class_type chan_class
)
{
   dl_lc_class_a = lc_class_a;
   dl_lc_class_b = lc_class_b;
   dl_lc_class_c = lc_class_c;
   umts_dl_chan_class = chan_class;   

   MSG_HIGH_3("GSVoiceif :: Storing TDS DL conf A %x, B %x, C %x",lc_class_a,lc_class_b,lc_class_c);
   MSG_HIGH_1("GSVoiceif :: chan_class %d",chan_class);
   if( (ghdi_mvs_status == GHDI_MVS_MODE_SELECT  ||
       ghdi_mvs_status == GHDI_MVS_ON) &&
       (vocoder_acquired_by_client == MVS_CLIENT_TDSCDMA)
       )
   {
    /* Since the VOC is already acquired, Pass the Configuration to MVS 
         And after this configure the WL1 */
      ghdi_configure_mvs_tdscdma_dl_channel();
   }
}

/*******************************************************************************
 *
 *  Function name: ghdi_configure_mvs_tdscdma_dl_channel(void)
 *  --------------
 *  Description:
 *  ------------
 *  Perform ghdi_configure_mvs_tdscdma_dl_channel() per given client and
 *  set the appropriate GHDI state.
 *
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 ******************************************************************************/
void ghdi_configure_mvs_tdscdma_dl_channel(void)
{
   if(is_voice_supported == TRUE)
   {
#ifndef FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING
      mvssup_tdscdma_set_dl_channel(dl_lc_class_a,
                                    dl_lc_class_b,
                                    dl_lc_class_c,
                                    umts_dl_chan_class);
#else /* FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING */
      mvssup_tdscdma_set_dl_channel(dl_lc_class_a,
                                    dl_lc_class_b,
                                    dl_lc_class_c,
                                    umts_dl_chan_class,
                                    &dl_blk_size_cb_set);

      tdsrrc_register_mvs_callback_to_l1(&dl_blk_size_cb_set);
#endif  /* FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING */         
      
      MSG_HIGH_0("GSVoiceif :: TD-SCDMA DL config");
   }
}
#endif /* FEATURE_TDSCDMA */

#if defined(FEATURE_VOC_AMR_WB) || defined(FEATURE_GSM_AMR_WB)
/*******************************************************************************
 *
 *  Function name: ghdi_mvs_amr_set_awb_mode(mvs_amr_mode_type)
 *  --------------
 *  Description:
 *  ------------
 *  Perform mvs_release() per given client and set the appropriate GHDI state.
 *
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 ******************************************************************************/
void ghdi_mvs_amr_set_awb_mode(mvs_amr_mode_type amr_ul_mode)
{

  ghdi_amr_wb_ul_mode = amr_ul_mode;
  ghdi_amr_wb_ul_mode_config_modified = TRUE;

  if(is_voice_supported == TRUE)
  {
    MSG_HIGH_1("GSVoiceif ::  amr_wb_mode %x, ",amr_ul_mode);
  
    if( ghdi_mvs_status == GHDI_MVS_MODE_SELECT  ||
        ghdi_mvs_status == GHDI_MVS_ON )
    {  
       mvssup_amr_set_awb_mode(ghdi_amr_wb_ul_mode);
       ghdi_amr_wb_ul_mode_config_modified = FALSE;
       MSG_HIGH_0("GSVoiceif :: W AWB config");     
    }
  
  }
}
#endif

/*******************************************************************************
 *
 *  Function name: ghdi_mvs_release()
 *  --------------
 *  Description:
 *  ------------
 *  Perform mvs_release() per given client and set the appropriate GHDI state.
 *
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 ******************************************************************************/
void ghdi_mvs_amr_set_amr_mode( mvs_amr_mode_type amr_ul_mode)
{

  ghdi_amr_ul_mode = amr_ul_mode;
  ghdi_amr_ul_mode_config_modified = TRUE;

  if(is_voice_supported == TRUE)
  {
    MSG_HIGH_1("GSVoiceif ::  amr_mode %x, ",amr_ul_mode);
  
    if( ghdi_mvs_status == GHDI_MVS_MODE_SELECT  ||
        ghdi_mvs_status == GHDI_MVS_ON )
    {  
       mvssup_amr_set_amr_mode(ghdi_amr_ul_mode);
       ghdi_amr_ul_mode_config_modified = FALSE;
       MSG_HIGH_0("GSVoiceif :: W AMR config");          
    }
  }
}


/*******************************************************************************
 *
 *  Function name: ghdi_mvs_release()
 *  --------------
 *  Description:
 *  ------------
 *  Perform mvs_release() per given client and set the appropriate GHDI state.
 *
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 ******************************************************************************/
void ghdi_mvs_amr_set_scr_mode(mvs_scr_mode_type scr_mode)
{

  ghdi_amr_scr_ul_mode = scr_mode;

  if(is_voice_supported == TRUE)
  {
    MSG_HIGH_1("GSVoiceif ::  amr_scr_mode %x, ",scr_mode);
  
     if( (ghdi_mvs_status == GHDI_MVS_MODE_SELECT  ||
         ghdi_mvs_status == GHDI_MVS_ON) &&
         (vocoder_acquired_by_client == MVS_CLIENT_TDSCDMA ||
         vocoder_acquired_by_client == MVS_CLIENT_WCDMA))
    {  
       mvssup_amr_set_scr_mode(ghdi_amr_scr_ul_mode);
       MSG_HIGH_0("GSVoiceif :: W SCR config");          
    }
  }
}

#ifdef FEATURE_VOC_AMR_WB
/*******************************************************************************
 *
 *  Function name: ghdi_mvs_get_ul_awb_mode()
 *  --------------
 *  Description:
 *  ------------
 *  GHDI wrapper for mvs function mvssup_get_ul_awb_mode() which returns amr-wb mode.
 *
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 ******************************************************************************/
mvs_amr_mode_type ghdi_mvs_get_ul_awb_mode(void)
{

  if(is_voice_supported == TRUE)
  {
    if( ghdi_mvs_status == GHDI_MVS_MODE_SELECT  ||
        ghdi_mvs_status == GHDI_MVS_ON )
    { 
      MSG_HIGH_0("ghdi_mvs_get_ul_awb_mode - voc is acq");
      return(mvssup_get_ul_awb_mode());
    }
    else
    {
      MSG_HIGH_1("ghdi_mvs_get_ul_awb_mode - voc not acq, ghdi_amr_wb_ul_mode = %d ",ghdi_amr_wb_ul_mode);
      return ghdi_amr_wb_ul_mode;
    }
  }
  else
  {
    return (MVS_AMR_MODE_0475);
  }
}
#endif
/*******************************************************************************
 *
 *  Function name: ghdi_mvs_get_ul_amr_mode()
 *  --------------
 *  Description:
 *  ------------
 *  GHDI wrapper for mvs function mvssup_get_ul_awb_mode() which returns amr-wb mode.
 *
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 ******************************************************************************/
mvs_amr_mode_type ghdi_mvs_get_ul_amr_mode(void)
{

  if(is_voice_supported == TRUE)
  {
     if( ghdi_mvs_status == GHDI_MVS_MODE_SELECT  ||
         ghdi_mvs_status == GHDI_MVS_ON )
     {
       return(mvssup_get_ul_amr_mode());
     }
     else
     {
       MSG_HIGH_1("GSVoiceif :: query for amr_mode %x, ",ghdi_amr_ul_mode);
       return ghdi_amr_ul_mode;
     }   
  }
  else
  {
    return (MVS_AMR_MODE_0475);
  }

}
#endif/* FEATURE_MVS_UNDER_GHDI */

#ifdef FEATURE_CS_VOICE_OVER_HSPA
static void ghdi_CSoHS_ul_fn(
  uint8                       *voc_packet,
  mvs_frame_info_type         *voc_frame_info,
  uint16                       packet_length,
  mvs_pkt_status_type         *status
)
{
  if( ghdi_CSovrHSPA_ul_fn != NULL )
  {
    ghdi_CSovrHSPA_ul_fn( voc_packet,
                          voc_frame_info,
                           packet_length,
                          status );
  }
  else
  {
    MSG_ERROR_0( "CSoHS: no UL fn registered with GSVoice");
  }
  return;
}

static void ghdi_CSoHS_dl_fn(
  uint8                  *voc_pkt,
  mvs_frame_info_type    *voc_frame_info,
  mvs_pkt_status_type    *status
)
{
  if( ghdi_CSovrHSPA_dl_fn != NULL )
  {
    ghdi_CSovrHSPA_dl_fn( voc_pkt,
                          voc_frame_info,
                          status );
  }
  else
  {
    MSG_ERROR_0( "CSoHS: no DL fn registered with GSVoice");
  }
  return;
}
/*******************************************************************************
 *
 *  Function name: ghdi_mvs_get_CSoverHSPA_ULDL_handles()
 *  --------------
 *  Description:
 *  ------------
 *  This function stores in the GHDI the UL and DL handle functions that need to be registered for
 *  CSoverHSPA stack. These functions will be passed on the MVS when enabling the MVS for a call
 *  of RAB TYPE = HSPA.
 *
 *  Parameters: ( mvs_packet_ul_fn_type, mvs_packet_dl_fn_type )
 *  -----------
 *
 *  Returns:
 *  --------
 *
 ******************************************************************************/
void ghdi_mvs_set_CSoverHSPA_ULDL_handles(mvs_packet_ul_fn_type  ul_func,mvs_packet_dl_fn_type dl_func)
{
  MSG_HIGH_2( "CSoHS: set UL [%0x] / DL [%0x] hdlrs", (uint32)ul_func, (uint32)dl_func);

  /* INTLOCK */

  /* Store the Handle functions that need to be registered with the MVS later */
  ghdi_CSovrHSPA_ul_fn = ul_func;
  ghdi_CSovrHSPA_dl_fn = dl_func;

  /* register local routines */
  ghdi_mvs_CSovrHSPA_ul_proc = ghdi_CSoHS_ul_fn;
  ghdi_mvs_CSovrHSPA_dl_proc = ghdi_CSoHS_dl_fn;

  /* INTFREE */
}
#endif

/********************************************************************************
 * Function name :  ghdi_is_acquisition_in_progress()
 * -------------
 *
 * Description :
 * -----------
 *   This Function tells, if vocoder acquisition is in progress or not.
 *
 * Uses :
 * ----
 *
 * Parameters:
 * ----------
 *
 * Returns: boolean
 * --------
 *
 ********************************************************************************/
boolean ghdi_is_acquisition_in_progress(void)
{
   MSG_HIGH_1("GSVoice ghdi_mvs_status = %d", ghdi_mvs_status);
   if(ghdi_mvs_status == GHDI_MVS_ACQUIRE)
   {
      return TRUE;   
   }
   else
   {
      return FALSE;   
   }    
}

/********************************************************************************
 * Function name :  ghdi_reset_mvs_callback_flag()
 * -------------
 *
 * Description :
 * -----------
 *   This Function resets mvs global variables.
 *
 * Uses :
 *           
 * ----
 *
 * Parameters:
 * ----------
 *
 * Returns: void
 * --------
 *
 ********************************************************************************/

void ghdi_reset_mvs_callback_flag(void)
{
NAS_ENTER_CRIT_SECT(ghdi_mvs_crit_sec);
   MSG_HIGH_0("Resetting mvs flag mvs_callback_pending");
   atomic_set(&mvs_callback_pending,FALSE);
NAS_EXIT_CRIT_SECT(ghdi_mvs_crit_sec);   

}


/********************************************************************************
 * Function name :  ghdi_update_mvs_mode()
 * -------------
 *
 * Description :
 * -----------
 *   This Function updates ghdi_mvs_mode.
 *
 * Uses :
 *           
 * ----
 *
 * Parameters:
 * ----------
 *
 * Returns: void
 * --------
 *
 ********************************************************************************/


void  ghdi_update_mvs_mode(umts_codecs_list_T channel_mode)
{

#ifdef FEATURE_VOC_AMR_WB 
     if(channel_mode == UMTS_WB_AMR)
     {
       ghdi_mvs_mode = MVS_MODE_AMR_WB;
     }
     else
     {
       ghdi_mvs_mode = MVS_MODE_AMR;
     }
#else
     ghdi_mvs_mode = MVS_MODE_AMR;
#endif


}


/*******************************************************************************
 *
 *  Function name:   ghdi_get_sampling_rate()
 *  --------------
 *
 *  Description:
 *  ------------
 *    RRC/TDSRRC/RR can use this API to get the sampling rate given by MVS for current codec.
 *    MVS can give sampling rate of 16kHz when NB codec is used.
 *    
 *  Parameters:
 *  -----------
 *
 *  Returns: sampling rate stored in GHDI cache.
 *  --------
 *
 ******************************************************************************/

uint32  ghdi_get_sampling_rate(void)
{
#ifdef FEATURE_ENHANCED_AMR
   MSG_HIGH_1("Returning GHDI cached sampling rate as %d",speech_enc_samp_freq); 
   return speech_enc_samp_freq;
#else
   MSG_HIGH_1("FEATURE_ENHANCED_AMR undefined. Returning default sampling rate %d",GHDI_EAMR_SR_8000); 
   return GHDI_EAMR_SR_8000;   
#endif   
}



/*******************************************************************************
 *
 *  Function name:   ghdi_reset_sampling_rate()
 *  --------------
 *
 *  Description:
 *  ------------
 *    RRC/TDSRRC/RR/NAS can use this API to reset the sampling rate in GHDI cache.
 *    NAS does it at call initiation and call clearing (when no other call is in progress).
 *    AS does it when codec changes to a non narrow band type.
 *    
 *  Parameters:
 *  -----------
 *
 *  Returns: None
 *  --------
 *
 ******************************************************************************/

void    ghdi_reset_sampling_rate(void)
{
#ifdef FEATURE_ENHANCED_AMR
   NAS_ENTER_CRIT_SECT(ghdi_mvs_crit_sec);
   speech_enc_samp_freq = GHDI_EAMR_SR_8000;
   NAS_EXIT_CRIT_SECT(ghdi_mvs_crit_sec);
   MSG_HIGH_1("Reset GHDI cached sampling rate to %d",speech_enc_samp_freq);
#else
   MSG_HIGH_0("Can't reset SR. FEATURE_ENHANCED_AMR undefined"); 
#endif   
}

/*===========================================================================

FUNCTION: ghdi_get_MVS_interface_val

DESCRIPTION: This function checks if current subs is G only subs,
If Yes, it returns New Interface
else it return Old interface

DEPENDENCIES:

PARAMETERS: none

RETURN VALUE: Boolean 
    TRUE if at least one of the Speech Call is in Active Conversational State else FALSE

SIDE EFFECTS:
  None
===========================================================================*/

#if defined(FEATURE_DUAL_SIM) 

ghdi_mvs_interface_type ghdi_get_MVS_interface_val(uint32 vsid)
{

  boolean is_G_only_subs = ((vsid & (1<<18)) == 0)?FALSE:TRUE;
  MSG_HIGH_2("ghdi_get_MVS_interface_val called with vsid = %d, is_G_only_subs = %d",vsid,is_G_only_subs);
  if(is_G_only_subs)
  {
    return GHDI_MVS_INTERFACE_1;
  }
  else /* Multimode subscription */
  {
    return GHDI_MVS_INTERFACE_0;
  }
}

#endif

/*******************************************************************************
 *
 *  Function name:   ghdi_reset_sr_and_inform_AS()
 *  --------------
 *
 *  Description:
 *  ------------
 * Reset sampling rate and give a trigger to AS to call SPEECH_CODEC_INFO to CM.
 *    
 *  Parameters:
 *  -----------
 *
 *  Returns: None
 *  --------
 *
 ******************************************************************************/

void    ghdi_reset_sr_and_inform_AS(sys_radio_access_tech_e_type active_rat)
{
#ifdef FEATURE_ENHANCED_AMR
   ghdi_reset_sampling_rate();

   if(active_rat == SYS_RAT_GSM_RADIO_ACCESS)
   {
       (void) geran_grr_set_codec_ho_sample_rate(speech_enc_samp_freq, TRUE);
   }
   else if(active_rat == SYS_RAT_UMTS_RADIO_ACCESS)
   {
       rrc_update_codec_info_to_cm();
   }
#ifdef FEATURE_TDSCDMA
   else if(active_rat == SYS_RAT_TDS_RADIO_ACCESS)
   {
     tdsrrc_update_codec_info_to_cm(speech_enc_samp_freq);
   }
#endif
   else
   {
     MSG_ERROR_1("Incorrect active RAT = %d",active_rat);
   }
#else
   MSG_ERROR_1("Incorrect reset_sr_and_inform_AS call, eAMR Feature not defined",active_rat);
#endif   
}

#if defined(FEATURE_DUAL_SIM)
sys_modem_as_id_e_type ghdi_get_mmode_subs_asid(void)
{

  byte subs_ctr;
  for(subs_ctr = 0;subs_ctr < MAX_AS_IDS;subs_ctr++)
  {
    
    if((mn_asid_2_vsid_map[subs_ctr] & (1<<18)) == 0)
    {
      MSG_LOW_1("ghdi_get_mmode_subs_asid returns %d",subs_ctr);
      return subs_ctr;
    }
  }
  MSG_LOW_0("ghdi_get_mmode_subs_asid returns -1");
  return SYS_MODEM_AS_ID_NONE;
}

#endif

