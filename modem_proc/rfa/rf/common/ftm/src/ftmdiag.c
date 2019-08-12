/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                Diagnostics Packet Processing Routines
                  Factor Test Mode specific Routines

General Description
  Diagnostic packet processing routines that are specific to Factory
  Test Mode.

Copyright (c) 2002 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History
                           
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftmdiag.c#1 $
$DateTime: 2016/12/13 07:59:52 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/04/14   ck      Add diag support for C2/C3
06/13/14   kab     Add 3xCA support
10/23/13   nrk     Fix compiler warnings
08/07/13   sml     FTM changes to support GSM RxD 
06/24/13   sar     Replaced memcpy calls with safe memscpy. 
04/23/13   ka      Initial DBDC support
12/18/12   aki     Removed FTM_RFCA_C, again, as it has been moved to rfca_task.c
11/29/12   tws     Add FTM GSM C2 ID.
10/30/12   spa     Use RFM_DEVICE_3 as diversity device on SV path
08/22/12   pl/sbm  Added FTM CA command support
08/01/12   jr      Replaced diag_free with diagpkt_free as per diag interface changes
02/14/12   sar     Reinstate TDSCDMA features.
01/28/12   sar     Feature reduction changes.
01/25/12   aki     Removed FTM_RFCA_C as it was moved to rfca_task.c
12/09/11   aki     Added FTM_RFCA_C
11/23/11   swb     Fixed scenario in which response packet size was less than header
10/20/11   adk     Added support for SV diversity.
09/07/11   sar     Clockworks error fix for (System: 7890).
07/11/11   hy      Including TD-SCDMA support.
05/24/11   bmg     CDMA 1x SV chain updates
02/22/11   aro     Added Support For FTM_RFM_C 
09/09/10   ems     Support For FTM Sequencer 
07/01/10   aki     Merging RFNV FTM changes from dev/rfnv_dev
06/29/10   kma     Added SV module support
04/01/10   adk     Merged branch scmm_mdm9k_merge_final
03/03/10   Vish    Merged from prerel/poseidon2.1.x: 02/09/10 - 03/01/10
03/01/10   lp      Removed FTM DSPDIAG support
02/09/10   sar     KW fixes.
02/18/10   mkv     Adding support for LTE Diversity
01/06/10   sar     Renamed ftm_task.h to rf_test_task_v.h.
11/18/09   pl      Added FTM_LTE_NS_C and FTM_LTE_C in ftm_diag interface 
11/18/09   sar     Removed FTM_BT_C from ftmdiag_tbl.
08/11/09   mkv     Merge MODEM_LTE_DEV into rfa2
09/09/09   aro     Removed DateTime and Author + Extra Space Removal
07/17/09   aro     Removed API to create Bad Packet
03/09/09   aro     Added Diag support for FTM_SVDO_C.
09/26/08   jfc     Add FEATURE_EXPORT_FTM_DIAGPKT for inter-processor FTM diag
03/06/08   jtn     Fix Klocwork errors.
12/17/07   tm      New reuseable event ftm_dspdiag_reuseable_event needed extern 
                   decl here. dspdiag_125_callback() changed to ignore multiple
                   callbacks.
06/16/06   jac     Fixed LINT errors.
05/18/06   tm      Initial release for dsp diagnostics driver - new FTM subsystem.
                   Event mechanism to conserve signal defs, and support for delayed
                   response.
01/18/06   dp      Add WCDMA Rx diversity command code as a "legacy" type
                   command.  Although this command code is new, we still
                   use the same packet format as the original WCDMA command
                   packet...hence, "legacy"!
04/22/05   bmg     Shorten response packets if initial allocation was larger
                   than actually needed.
01/17/05   bmg     Reserved 50 bytes for FTM V2 cal packets.
10/12/04   jac     Mainlined ASYNC packet code.
09/29/04   wd      Added code to check for rsp_pkt_size requested in the ftm
                   header. Not for legacy pkts.
08/19/04   ra      Merged with 1x targets
03/23/03   xw      Cleaned-up the asynchronous rsp process. Also added new
                   automatic rsp for bad commands.
10/02/03   ka      Added support for variable length response packet.
10/01/02   lad     Created file from diagpkt_ftm.c
08/21/02   rjr     Created file (diagpkt_ftm.c)

===========================================================================*/

#include "rfa_variation.h"
#include <string.h>
#include "stringl.h" /* for memscpy */
#include "diagpkt.h"
#include "diagcmd.h"
#include "msg.h"
#include "err.h"
#include "mobile.h"
#include "msm.h"
#include "customer.h"
#include "ftmicap.h"
#include "ftm.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "rf_test_task_v.h"
#include "ftmdiag.h"
#include "task.h"
#include "diagi.h"
#include "diagdiag.h"
#include "memheap.h"


/*============================================================================

                     EXTERNAL DATA DECLARATIONS

============================================================================*/

/*===========================================================================
 
                   LOCAL GLOBAL VARIABLES and CONSTANTS

===========================================================================*/
void *ftm_async_pkt_ptr;    /* holds async pkt for diag */
uint16 ftm_async_pkt_len;   /* holds async pkt length for diag */

ftm_pkt_data_type ftm_pkt_data; /* holds information for current ftm cmd */

/* ----------------------------------------------------------------------- */


/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION DSPDIAG_125_CALLBACK

DESCRIPTION
   This function receives a 125 callback request, packs it into an event 
   request, then places it on the event Q.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/

void dspdiag_125_callback (PACKED void *req_pkt, word req_len)
{
  ftm_msg_get_ssid_ranges_req_type       *pkt;
  ftm_msg_set_rt_mask_req_type           *pkt_range;
  ftm_diag_125_event_type                *diag_req;
  // dspdiag_msg_set_all_masks_req_type     *pkt_all;
  ftm_event_type                         *ftm_evt = NULL;
  int cmd_code;
  int sub_cmd;
  uint16  ftm_subsys_code=FTM_MODE_MAX_C;
  boolean valid = FALSE;
  
  pkt = (ftm_msg_get_ssid_ranges_req_type *) req_pkt;

  cmd_code = pkt->cmd_code;

  if (cmd_code == DIAG_EXT_MSG_CONFIG_F)
  {
    sub_cmd = pkt->sub_cmd;

    switch (sub_cmd)
    {
    case MSG_EXT_SUBCMD_SET_RT_MASK: 

      pkt_range = (ftm_msg_set_rt_mask_req_type*) req_pkt;

      if (pkt_range->ssid_end >= pkt_range->ssid_start)
      {
        // other FTM subsystems if interested can put in their range check here
        // also set ftm_evt to their own reuseable event possibly
      }

      break;

    case MSG_EXT_SUBCMD_SET_ALL_RT_MASKS:

      ftm_subsys_code = FTM_MODE_MAX_C;
      break;

    default:

      valid = FALSE;
      break;
    }

    // other subsystems to ensure ftm_evt is non null if setting valid to true
    if (valid)
     {
      if (!ftm_evt)
      {
        FTM_MSG_ERROR("NULL ftm_evt in dspdiag_125_callback.",0,0,0);
        goto clean;
      }
      //note the following line is a null check for evt_data_ptr, not ftm_evt
      if (!ftm_evt->evt_data_ptr)
      {
        ftm_evt->hdr.done_q_ptr = NULL;       /* No done queue. */
        ftm_evt->evt_cat = FTM_EVENT_DIAG;
        ftm_evt->reuse = TRUE;
        ftm_evt->queued = FALSE;

        ftm_evt->evt_data_ptr = (void *) ftm_malloc(sizeof(ftm_diag_125_event_type));
        if (!ftm_evt->evt_data_ptr)
        {
          FTM_MSG_ERROR("FTM malloc failed in dspdiag_125_callback.",0,0,0);
          goto clean;
        }
        diag_req = (ftm_diag_125_event_type*)ftm_evt->evt_data_ptr;
        diag_req->ftm_subsys_code = ftm_subsys_code;
        diag_req->ftm_diag_event.ref_ct = 0;  // increase ref_ct later if you want to bypass default cleanup mechanism
        diag_req->ftm_diag_event.req_pkt_len = 0;
        diag_req->ftm_diag_event.ftm_data_ptr = NULL;
        diag_req->ftm_diag_event.max_alloc = 0;
      }

      if (ftm_evt->queued)
      {
        goto clean;    
      }

      diag_req = (ftm_diag_125_event_type*)ftm_evt->evt_data_ptr;

      if (diag_req->ftm_diag_event.req_pkt_len == 0)
      {
        // assuming req_pkt_len == 0 => never been allocated.
        diag_req->ftm_diag_event.ftm_data_ptr = (void *) ftm_malloc(req_len);
        if (!diag_req->ftm_diag_event.ftm_data_ptr)
        {
          FTM_MSG_ERROR("FTM malloc failed in dspdiag_125_callback.",0,0,0);
          goto clean;
        }
        diag_req->ftm_diag_event.max_alloc = req_len;
        diag_req->ftm_diag_event.req_pkt_len = req_len;
      }
      // nothing special for (diag_req->ftm_diag_event.max_alloc >= req_len)
      else if (diag_req->ftm_diag_event.max_alloc < req_len)
      {
        ftm_free(diag_req->ftm_diag_event.ftm_data_ptr);
        diag_req->ftm_diag_event.ftm_data_ptr = NULL;
        diag_req->ftm_diag_event.max_alloc = 0;
        diag_req->ftm_diag_event.ftm_data_ptr = (void *) ftm_malloc(req_len);
        if (!diag_req->ftm_diag_event.ftm_data_ptr)
        {
          FTM_MSG_ERROR("FTM malloc failed in dspdiag_125_callback.",0,0,0);
          goto clean;
        }

        diag_req->ftm_diag_event.max_alloc = req_len;
      }

      // common to above choices
      diag_req->ftm_diag_event.req_pkt_len = req_len;

      // Fill the req_ptr with rx packet parameters
      memscpy((void *) diag_req->ftm_diag_event.ftm_data_ptr, 
              diag_req->ftm_diag_event.req_pkt_len, 
             (void *) req_pkt, diag_req->ftm_diag_event.req_pkt_len);

      /* Put it on the FTM Event Q */
      ftm_evt->queued = TRUE;
      ftm_event( ftm_evt );
    }
  }

clean:
  return;
}



/*===========================================================================

FUNCTION FTM_DIAG_DELAYED

DESCRIPTION
  This procedure handles pkts from diag, and puts them in cmd que for 
  ftm_task to process.

============================================================================*/
PACKED void *ftm_diag_delayed(
  PACKED void* req_pkt,/* pointer to valid request packet */
  uint16 pkt_len       /* length of the request packet */
)
{
  uint16                         rsp_pkt_len;
  ftm_event_type                 *ftm_evt = NULL;
  ftm_diag_128_event_type        *diag_req;
  ftm_subsys_rsp_packet_type     *dspdiag_rsp_ptr;
  // ftm_composite_cmd_header_type  *dspdiag_composite_hdr_ptr;
  ftm_diagpkt_subsys_header_type *pkt;

  pkt = (ftm_diagpkt_subsys_header_type*)req_pkt;

  /* Allocate memory */
  ftm_evt = ftm_malloc(sizeof(ftm_event_type));

  if (!ftm_evt)
  {
    goto clean;
  }

  ftm_evt->hdr.done_q_ptr = NULL;       /* No done queue. */

  ftm_evt->evt_cat = FTM_EVENT_DIAG;
  ftm_evt->reuse = FALSE;
  ftm_evt->queued = FALSE;
  ftm_evt->evt_data_ptr = NULL;

  /* Must allocate a new instance for every command linked on the queue */
  ftm_evt->evt_data_ptr = (void *) ftm_malloc(sizeof(ftm_diag_128_event_type));
  if (!ftm_evt->evt_data_ptr)
  {
    ftm_free(ftm_evt);
    goto clean;
  }

  diag_req = (ftm_diag_128_event_type*)ftm_evt->evt_data_ptr;
  
  diag_req->ftm_diag_event.req_pkt_len = pkt_len;
  diag_req->ftm_diag_event.ftm_data_ptr = NULL;
  diag_req->ftm_diag_event.ftm_data_ptr = (void *) ftm_malloc(pkt_len);
  if (!diag_req->ftm_diag_event.ftm_data_ptr)
  {
    ftm_free(ftm_evt->evt_data_ptr);
    ftm_free(ftm_evt);
    goto clean;
  }

  /* Fill the req_ptr with rx packet parameters */
  memscpy((void *) diag_req->ftm_diag_event.ftm_data_ptr,
         diag_req->ftm_diag_event.req_pkt_len, 
         (void *) req_pkt, diag_req->ftm_diag_event.req_pkt_len);

  rsp_pkt_len = sizeof(ftm_diagpkt_subsys_hdr_type_v2); // + 
             // sizeof(ftm_cmd_header_type);

  /* send an immediate response */
  dspdiag_rsp_ptr = 
      (ftm_subsys_rsp_packet_type*) 
          diagpkt_subsys_alloc_v2(
              (diagpkt_subsys_id_type)DIAG_SUBSYS_FTM, 
              (diagpkt_subsys_cmd_code_type)pkt->subsys_cmd_code,
              rsp_pkt_len);

  if(!dspdiag_rsp_ptr)
  {
    FTM_MSG_ERROR("NULL dspdiag_rsp_ptr in ftm_diag.",0,0,0);
    ftm_free(ftm_evt->evt_data_ptr);
    ftm_free(ftm_evt);
    goto clean;
  }
  /* first save the packet id for later use in the delayed respose */
  diag_req->delay_rsp_id = diagpkt_subsys_get_delayed_rsp_id(dspdiag_rsp_ptr);

  /* increase ref_ct later if you want to bypass default cleanup mechanism */
  diag_req->ftm_diag_event.ref_ct = 0;

  // memcpy((void*)dspdiag_rsp_ptr->data, 
  //        (void*)((byte*) &(dspdiag_composite_hdr_ptr->ftm_hdr)), 
  //        sizeof(ftm_cmd_header_type));

  dspdiag_rsp_ptr->header.status = (uint32)FTM_RSP_NOP;
  diagpkt_commit(dspdiag_rsp_ptr);

  /* Put it on the FTM Event Q */
  ftm_evt->queued = TRUE;
  ftm_event( ftm_evt );

clean:
  return NULL;
}



/*===========================================================================

FUNCTION FTM_DIAG

DESCRIPTION
  This procedure handles pkts from diag, and puts them in cmd que for 
  ftm_task to process.

============================================================================*/
PACKED void *ftm_diag(
  PACKED void* req_pkt,/* pointer to valid request packet */
  uint16 pkt_len       /* length of the request packet */
)
{
  void *rsp_ptr;
  ftm_diagpkt_subsys_header_type *pkt = (ftm_diagpkt_subsys_header_type*)req_pkt;
  
  /* Must allocate a new instance for every command linked on the queue */
  ftm_msg_type *ftm_msg = ftm_malloc(sizeof(ftm_msg_type));
  if (!ftm_msg)
  {
    FTM_MSG_ERROR("NULL ftm_msg in ftm_diag.",0,0,0);
    goto clean;
  }

  /* temp packet length, we need to move ftm_cmd_header_type to ftm from ftm_hwtc!! */
  ftm_msg->req_pkt_len = pkt_len;

  /* Allocate memory */
  ftm_msg->ftm_data_ptr = (void *) ftm_malloc(ftm_msg->req_pkt_len);
  if(ftm_msg->ftm_data_ptr == NULL)
  {
    FTM_MSG_ERROR("Failed to allocate memory for req. pkt. ",0,0,0);
    ftm_free(ftm_msg); // free the memory allocated for the ftm_msg.
    goto clean;
  }
  
  /* Fill the req_ptr with rx packet parameters */
  memscpy((void *) ftm_msg->ftm_data_ptr, ftm_msg->req_pkt_len,
          (void *) req_pkt, ftm_msg->req_pkt_len);

  ftm_msg->hdr.done_q_ptr = NULL;       /* No done queue. */

  /* check for legacy ftm pkts and do the legacy thing */
  if ((pkt->subsys_id == FTM && pkt->subsys_cmd_code == FTM_1X_C0_C) ||
      (pkt->subsys_id == FTM && pkt->subsys_cmd_code == FTM_1X_C1_C) ||
      (pkt->subsys_id == FTM && pkt->subsys_cmd_code == FTM_1X_C2_C) ||
      (pkt->subsys_id == FTM && pkt->subsys_cmd_code == FTM_1X_C3_C) ||
      (pkt->subsys_id == FTM && pkt->subsys_cmd_code == FTM_RFM_C) ||
      (pkt->subsys_id == FTM && pkt->subsys_cmd_code == FTM_WCDMA_C) ||
      (pkt->subsys_id == FTM && pkt->subsys_cmd_code == FTM_WCDMA_RX_2_C) ||
      (pkt->subsys_id == FTM && pkt->subsys_cmd_code == FTM_WCDMA_RX_3_C) ||
      (pkt->subsys_id == FTM && pkt->subsys_cmd_code == FTM_WCDMA_RX_4_C) ||
      (pkt->subsys_id == FTM && pkt->subsys_cmd_code == FTM_GSM_C0_C) ||
      (pkt->subsys_id == FTM && pkt->subsys_cmd_code == FTM_GSM_C1_C) ||
      (pkt->subsys_id == FTM && pkt->subsys_cmd_code == FTM_GSM_C2_C) ||
      (pkt->subsys_id == FTM && pkt->subsys_cmd_code == FTM_GSM_C3_C) ||
      (pkt->subsys_id == FTM && pkt->subsys_cmd_code == FTM_LTE_NS_C) ||
      (pkt->subsys_id == FTM && pkt->subsys_cmd_code == FTM_LTE_C) ||
      (pkt->subsys_id == FTM && pkt->subsys_cmd_code == FTM_LTE_RX_2_C) ||
      (pkt->subsys_id == FTM && pkt->subsys_cmd_code == FTM_LTE_CAR1_C) ||
      (pkt->subsys_id == FTM && pkt->subsys_cmd_code == FTM_LTE_CAR1_RX_2_C) ||
      (pkt->subsys_id == FTM && pkt->subsys_cmd_code == FTM_TDSCDMA_C) ||
      (pkt->subsys_id == FTM && pkt->subsys_cmd_code == FTM_TDSCDMA_RX_2_C) ||
      (pkt->subsys_id == FTM && pkt->subsys_cmd_code == FTM_TDSCDMA_C2_C) ||
      (pkt->subsys_id == FTM && pkt->subsys_cmd_code == FTM_TDSCDMA_C3_C) ||
      (pkt->subsys_id == FTM && pkt->subsys_cmd_code == FTM_TDSCDMA_BER_C) || 
      (pkt->subsys_id == FTM && pkt->subsys_cmd_code == FTM_LTE_CAR2_C) || 
      (pkt->subsys_id == FTM && pkt->subsys_cmd_code == FTM_LTE_CAR2_RX_2_C) 
      
      )
  {
    /* Allocate the memory for this */
    rsp_ptr = (void*) diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_FTM, 1, ftm_msg->req_pkt_len);
    ftm_msg->rsp_pkt_len = ftm_msg->req_pkt_len;
  }
#ifdef FEATURE_CDMA1X
  else if (pkt->subsys_id == FTM && pkt->subsys_cmd_code == FTM_1X_CAL_V2_C)
  {
    const int ftm_1x_cal_v2_max_msg_len = 50;
    rsp_ptr = (void*) diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_FTM, 1, ftm_1x_cal_v2_max_msg_len);
    ftm_msg->rsp_pkt_len = ftm_1x_cal_v2_max_msg_len;
  }
#endif /* FEATURE_CDMA1X */
  else
  {
    /* this pkt should be using the ftm hrd thus we can find out
       what thier rsp pkt size requirements are, if 0 or length is 
       bigger then what diag can handle then use legacy. 
       The actual diag heap at the moment is 2048 bytes and is 
       never all available for just ftm.
    */
    ftm_composite_cmd_header_type *pkthdr = (ftm_composite_cmd_header_type*)req_pkt;

    if (pkthdr->ftm_hdr.cmd_rsp_pkt_size < 4 || pkthdr->ftm_hdr.cmd_rsp_pkt_size > 1024)
    {
      /* Allocate the memory for this */
      rsp_ptr = (void*) diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_FTM, 1, ftm_msg->req_pkt_len);      
      ftm_msg->rsp_pkt_len = ftm_msg->req_pkt_len;
    }
    else
    {
      rsp_ptr = (void*) diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_FTM, 1, pkthdr->ftm_hdr.cmd_rsp_pkt_size);      
      ftm_msg->rsp_pkt_len = pkthdr->ftm_hdr.cmd_rsp_pkt_size;
    }
  }

  ftm_msg->data_rsp_ptr = rsp_ptr;

  /* Put it on the FTM Command queue */
  ftm_cmd( ftm_msg );

clean:
  rsp_ptr = NULL;

  return rsp_ptr;
} /* ftm_diag */

/*===========================================================================

FUNCTION TYPE FTMDIAG_ERROR

DESCRIPTION

===========================================================================*/
PACKED void *ftmdiag_error(
  PACKED void* req_pkt,/* pointer to valid request packet */
  uint16 pkt_len       /* length of the request packet */
)
{
  pkt_len = pkt_len;  /* lint error */

  //Note: You may just want to send back an error response.  This
  //is already part of the DIAG service.  diagpkt_err_rsp(). -LD   

  /* added code to send back all zeros */
  return(req_pkt);
} /* End ftmdiag_error */

/*===========================================================================

FUNCTION TYPE FTMDIAG_IDLE_PROCESSING_CB

DESCRIPTION
  A function that when registered with diag, is called when the DIAG task has 
  idle processing time. This function executes in DIAG task context.

===========================================================================*/
void ftmdiag_idle_processing_cb (void *param)
{
  ftm_rsp_pkt_type *pkt_data = (ftm_rsp_pkt_type*)param;
  void             *diag_mem_ptr = pkt_data->pkt_payload;

  switch (pkt_data->cmd)
  {
  case FTM_RSP_DO_NEW_PKT:
    /* free up previosly allocated memeory */
    if (diag_mem_ptr != NULL)
    {
      diagpkt_free(diag_mem_ptr);
    }

    /* Allocate the memory for rsp pkt */
    pkt_data->pkt_payload = (void*) diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_FTM, 1, pkt_data->pkt_len);
    break;

  case FTM_RSP_NO_RSP:
    /* free up previosly allocated memeory */
    if (diag_mem_ptr != NULL)
    {
      diagpkt_free(diag_mem_ptr);
    }
    break;
  default:
    FTM_MSG_ERROR("FTM diag_idle_cp invalid command [%d].",pkt_data->cmd,0,0);
    break;
  }

  /* de-register this call back func */
  (void)diag_idle_processing_unregister (ftmdiag_idle_processing_cb, param);

  /* signal ftm to continue */
  ftm_sig_rex_set(FTM_ASYNC_PKT_SIG);
} /* End ftmdiag_idle_processing_cb */

/*===========================================================================

FUNCTION FTMDIAG_SEND_ASYNC_PKT

DESCRIPTION
  This procedure sets signals diag_task so it will send an asyc pkt.

============================================================================*/
void ftmdiag_send_async_pkt(ftm_msg_type *ftm_msg)
{
  /* copy pkt to diag allocated memory space */
  memscpy (ftm_msg->data_rsp_ptr, ftm_msg->rsp_pkt_len, 
           ftm_msg->ftm_data_ptr, ftm_msg->rsp_pkt_len);

  /* send async packet */
  diagpkt_commit(ftm_msg->data_rsp_ptr);

} /* End ftmdiag_send_async_pkt */

/*===========================================================================

FUNCTION FTMDIAG_CREATE_NEW_PKT

DESCRIPTION
  This procedure creates a new paket in the ftm heap.

============================================================================*/
ftm_rsp_pkt_type ftmdiag_create_new_pkt (dword len)
{
  ftm_rsp_pkt_type new_pkt = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
  
  /* allocate space in memory heap for new pkt */
  new_pkt.pkt_payload = (byte*)ftm_malloc(len);  

  if (new_pkt.pkt_payload != NULL)
  {
    /* init rsp_pkt_type fields */
    new_pkt.cmd = FTM_RSP_DO_NEW_PKT;
    new_pkt.pkt_len = len;
    new_pkt.delete_payload = TRUE;
  }

  return new_pkt;
} /* End ftmdiag_create_new_pkt */

/*===========================================================================

FUNCTION FTMDIAG_CREATE_NEW_PKT_WITH_DATA

DESCRIPTION
  This procedure creates a new paket in the ftm heap from given information 
  about header size and data size.

============================================================================*/
ftm_rsp_pkt_type ftmdiag_create_new_pkt_with_data (byte *hdr, dword hdr_len, byte *data, dword data_len)
{
  ftm_rsp_pkt_type new_pkt;
  
  /* create new pkt */
  new_pkt = ftmdiag_create_new_pkt(hdr_len + data_len);

  if (new_pkt.pkt_payload != NULL)
  {
    /* copy information from header */
    if (hdr != NULL)
    {
      memscpy((void*)new_pkt.pkt_payload, hdr_len, (void*)hdr, hdr_len); 
    }

    /* copy information from data */
    if (data != NULL)
    {
      memscpy((void*)((byte*)new_pkt.pkt_payload + hdr_len), 
              data_len, (void*)data, data_len); 
    }
  }

  return new_pkt;
} /* End ftmdiag_create_new_pkt */

/*===========================================================================

FUNCTION FTMDIAG_DIAG_MALLOC

DESCRIPTION
  This procedure will allocate memory in diag's heap.
  
  NOTE: Do not use outside this module.

============================================================================*/
void *ftmdiag_diag_malloc (dword len)
{
  ftm_rsp_pkt_type new_pkt;
  void *new_ptr = NULL;
  
  /* need more memory then previously allocated */

  /* init required size and pass in ptr to old memory to be freedup */
  new_pkt.cmd = FTM_RSP_DO_NEW_PKT;
  new_pkt.pkt_len = len;
  new_pkt.pkt_payload = NULL;

  /* register call back function with diag */
  (void)diag_idle_processing_register (ftmdiag_idle_processing_cb, (void*)&new_pkt);

  /* wait for diag to be done */
  (void)ftm_sig_clk_rex_wait(FTM_ASYNC_PKT_SIG, FTM_WAIT_FOREVER);
  
  new_ptr = new_pkt.pkt_payload;

  if (new_ptr == NULL)
  {
    FTM_MSG_ERROR("Can't allocate diag memeory.",0,0,0);
  }

  return new_ptr;
} /* End ftmdiag_diag_malloc */

/*===========================================================================

FUNCTION FTMDIAG_DIAG_FREE

DESCRIPTION
  This procedure will free-up allocate memory in diag's heap.
  
  NOTE: Do not use outside this module.

============================================================================*/
void ftmdiag_diag_free (void *ptr)
{
  ftm_rsp_pkt_type pkt_data;

  /* init required size and pass in ptr to old memory to be free-up */
  pkt_data.cmd = FTM_RSP_NO_RSP;
  pkt_data.pkt_payload = ptr;

  /* register call back function with diag */
  (void)diag_idle_processing_register (ftmdiag_idle_processing_cb, (void*)&pkt_data);

  /* wait for diag to be done */
  (void)ftm_sig_clk_rex_wait(FTM_ASYNC_PKT_SIG, FTM_WAIT_FOREVER);
} /* End ftmdiag_diag_free */

/*===========================================================================

FUNCTION ftmdiag_diag_free_and_malloc

DESCRIPTION
  This procedure will free-up and allocate memory in diag's heap.
  
  NOTE: Do not use outside this module.

============================================================================*/
void *ftmdiag_diag_free_and_malloc (void* freeup_ptr, dword len)
{
  ftm_rsp_pkt_type new_pkt;
  void *new_ptr = NULL;
  
  /* need more memory then previously allocated */

  /* init required size and pass in ptr to old memory to be freedup */
  new_pkt.cmd = FTM_RSP_DO_NEW_PKT;
  new_pkt.pkt_len = len;
  new_pkt.pkt_payload = freeup_ptr;

  /* register call back function with diag */
  (void)diag_idle_processing_register (ftmdiag_idle_processing_cb, (void*)&new_pkt);

  /* wait for diag to be done */
  (void)ftm_sig_clk_rex_wait(FTM_ASYNC_PKT_SIG, FTM_WAIT_FOREVER);
  
  new_ptr = new_pkt.pkt_payload;

  if (new_ptr == NULL)
  {
    FTM_MSG_ERROR("Can't allocate diag memeory.",0,0,0);
  }

  return new_ptr;
} /* End ftmdiag_diag_malloc */

/*===========================================================================

FUNCTION FTMDIAG_PROCESS_ASYNC_RSP_PKT

DESCRIPTION
  This procedure will send asyc pkts to diag.

============================================================================*/
boolean ftmdiag_process_async_rsp_pkt (ftm_rsp_pkt_type rsp_pkt)
{
  boolean good_status = TRUE;


  switch (rsp_pkt.cmd)
  {
  case FTM_RSP_BAD_CMD:
  case FTM_RSP_BAD_PARM:
  case FTM_RSP_BAD_LEN:
  case FTM_RSP_BAD_MODE:
    {
      byte bad_rsp = 0;

      switch (rsp_pkt.cmd)
      {
      case FTM_RSP_BAD_CMD:
        bad_rsp = DIAG_BAD_CMD_F;
        break;
      case FTM_RSP_BAD_PARM:
        bad_rsp = DIAG_BAD_PARM_F;
        break;
      case FTM_RSP_BAD_LEN:
        bad_rsp = DIAG_BAD_LEN_F;
        break;
      case FTM_RSP_BAD_MODE:
        bad_rsp = DIAG_BAD_MODE_F;
        break;
      default:
        FTM_MSG_ERROR("Bad command", 0,0,0);
        break;
      }

      /* if packet lenght of new pkt is same or smaller then previously
      allocated diag memeory, re-use it, is fater then trying to allocate
      the exact memory
      */
      if ((DIAGPKT_MAX_ERR + 1) > ftm_pkt_data.rsp_pkt_len)
      {
        ftm_pkt_data.data_rsp_ptr = ftmdiag_diag_free_and_malloc(ftm_pkt_data.data_rsp_ptr, DIAGPKT_MAX_ERR + 1);
      }

      if (ftm_pkt_data.data_rsp_ptr != NULL)
      {
        byte* ptr = (byte*)ftm_pkt_data.data_rsp_ptr;
        
        /* copy command code to first byte of allocated meemory */
        *ptr = bad_rsp;

        ptr++;

        /* copy pkt first 16 bytes to diag allocated memory space */
        if (DIAGPKT_MAX_ERR > ftm_pkt_data.rsp_pkt_len)
        {
          memscpy ((void*)ptr, ftm_pkt_data.rsp_pkt_len, ftm_pkt_data.ftm_data_ptr, ftm_pkt_data.rsp_pkt_len);
        }
        else
        {
          memscpy ((void*)ptr, DIAGPKT_MAX_ERR, ftm_pkt_data.ftm_data_ptr, DIAGPKT_MAX_ERR);
        }
      }
      else
      {
        good_status = FALSE;
      }
    }
    break;

  case FTM_RSP_NO_RSP:
    if (ftm_pkt_data.data_rsp_ptr != NULL)
    {
      ftmdiag_diag_free(ftm_pkt_data.data_rsp_ptr);
      ftm_pkt_data.data_rsp_ptr = NULL;
    }
    good_status = TRUE;
    break;

  case FTM_RSP_DO_LEGACY:
    if (ftm_pkt_data.ftm_data_ptr == NULL)
    {
      FTM_MSG_ERROR("FTM legacy pkt ptr is null.",0,0,0);
      good_status = FALSE;
    }
    else
    {
      if (ftm_pkt_data.data_rsp_ptr == NULL)
      {
        ftm_pkt_data.data_rsp_ptr = ftmdiag_diag_malloc(ftm_pkt_data.rsp_pkt_len);
      }

      if (ftm_pkt_data.data_rsp_ptr != NULL)
      {
        /* copy pkt to diag allocated memory space */
        memscpy (ftm_pkt_data.data_rsp_ptr, ftm_pkt_data.rsp_pkt_len, 
                 ftm_pkt_data.ftm_data_ptr, ftm_pkt_data.rsp_pkt_len);
      }
      else
      {
        good_status = FALSE;
      }
    }
    break;

  case FTM_RSP_DO_NEW_PKT:
    if (rsp_pkt.pkt_payload == NULL)
    {
      FTM_MSG_ERROR("FTM new pkt ptr is null.",0,0,0);
      good_status = FALSE;
    }
    else
    {
      /* if packet lenght of new pkt is same or smaller then previously
      allocated diag memeory, re-use it, is fater then trying to allocate
      the exact memory
      */
      if (rsp_pkt.pkt_len > ftm_pkt_data.rsp_pkt_len ||
          ftm_pkt_data.data_rsp_ptr == NULL)
      {
        ftm_pkt_data.data_rsp_ptr = ftmdiag_diag_free_and_malloc(ftm_pkt_data.data_rsp_ptr, rsp_pkt.pkt_len);
        ftm_pkt_data.rsp_pkt_len = (ftm_pkt_data.data_rsp_ptr != NULL)? rsp_pkt.pkt_len : 0;
      }

      if (ftm_pkt_data.data_rsp_ptr != NULL)
      {
        /* If the original response packet pre-allocated too much space,
           shorten the response packet so Diag doesn't send too many bytes */
        if ( ftm_pkt_data.rsp_pkt_len > rsp_pkt.pkt_len )
        {
          diagpkt_shorten( ftm_pkt_data.data_rsp_ptr, rsp_pkt.pkt_len );
          ftm_pkt_data.rsp_pkt_len = rsp_pkt.pkt_len;
        }

        /* copy pkt to diag allocated memory space */
        memscpy (ftm_pkt_data.data_rsp_ptr, rsp_pkt.pkt_len, 
                 rsp_pkt.pkt_payload, rsp_pkt.pkt_len);
      }
      else
      {
        good_status = FALSE;
      }

      /* deallocate ftm pkt payload memory from heap */
      if (rsp_pkt.delete_payload &&
          rsp_pkt.pkt_payload != ftm_pkt_data.ftm_data_ptr)
      {
        (void) ftm_free(rsp_pkt.pkt_payload);
      }
    }
    break;

  default:
    FTM_MSG_ERROR("FTM process_async_rsp_pkt invalid command [%d].",rsp_pkt.cmd,0,0);
    break;
  }

  if (ftm_pkt_data.data_rsp_ptr != NULL)
  {
    /* send async packet */
    diagpkt_commit(ftm_pkt_data.data_rsp_ptr);

    /* invalidate rsp pointer since it now belongs to diag */
    ftm_pkt_data.data_rsp_ptr = NULL;
  }
  else if (good_status == FALSE)
  {
    FTM_MSG_ERROR("Can't send data to diag because RSP PKT ptr is null.",0,0,0);
    good_status = FALSE;
  }

  return good_status;
} /* End ftmdiag_send_async_pkt */


/**************************************************************************
** DISPATCH TABLE.
Note: all Rx 75, 11, xx and 75, 10, xx commands are dispatched here.
****************************************************************************/

static const diagpkt_user_table_entry_type ftmdiag_tbl[] = 
{

      {FTM_1X_C0_C,FTM_1X_C0_C,ftm_diag},
      
      {FTM_1X_C1_C,FTM_1X_C1_C,ftm_diag},

      {FTM_1X_C2_C,FTM_1X_C2_C,ftm_diag},

      {FTM_1X_C3_C,FTM_1X_C3_C,ftm_diag},

      {FTM_RFM_C,FTM_RFM_C,ftm_diag},

      {FTM_PMIC_C,FTM_PMIC_C,ftm_diag},

      {FTM_MC_C,FTM_MC_C,ftm_diag},

      {FTM_HDR_C,FTM_HDR_C,ftm_diag},

      {FTM_GSM_C0_C,FTM_GSM_C0_C,ftm_diag},
  
      {FTM_GSM_C1_C,FTM_GSM_C1_C,ftm_diag},

      {FTM_GSM_C2_C,FTM_GSM_C2_C,ftm_diag},

      {FTM_GSM_C3_C,FTM_GSM_C3_C,ftm_diag},

      {FTM_GSM_EXTENDED_C0_C,FTM_GSM_EXTENDED_C0_C,ftm_diag},

      {FTM_GSM_EXTENDED_C1_C,FTM_GSM_EXTENDED_C1_C,ftm_diag},

      {FTM_GSM_EXTENDED_C2_C,FTM_GSM_EXTENDED_C2_C,ftm_diag},

      {FTM_GSM_EXTENDED_C3_C,FTM_GSM_EXTENDED_C3_C,ftm_diag},

      {FTM_WCDMA_C,FTM_WCDMA_C,ftm_diag},

      {FTM_WCDMA_RX_2_C,FTM_WCDMA_RX_2_C,ftm_diag},

      {FTM_WCDMA_RX_3_C,FTM_WCDMA_RX_3_C,ftm_diag},

      {FTM_WCDMA_RX_4_C,FTM_WCDMA_RX_4_C,ftm_diag},

      {FTM_LOG_C,FTM_LOG_C,ftm_diag},

      {FTM_AGPS_C,FTM_AGPS_C,ftm_diag},

      {FTM_GSM_BER_C0_C,FTM_GSM_BER_C0_C,ftm_diag},

      {FTM_GSM_BER_C1_C,FTM_GSM_BER_C1_C,ftm_diag},

      {FTM_GSM_BER_C2_C,FTM_GSM_BER_C2_C,ftm_diag},

      {FTM_GSM_BER_C3_C,FTM_GSM_BER_C3_C,ftm_diag},

      {FTM_WCDMA_BER_C,FTM_WCDMA_BER_C,ftm_diag},

      {FTM_SE_BER_C,FTM_SE_BER_C,ftm_diag},

      {FTM_MODE_FACTORY_C,FTM_MODE_FACTORY_C,ftm_diag},

      {FTM_1X_CAL_V2_C,FTM_1X_CAL_V2_C,ftm_diag},

      {FTM_COMMON_C,FTM_COMMON_C,ftm_diag},

#ifdef FEATURE_LTE

      {FTM_LTE_C,FTM_LTE_C,ftm_diag},

      {FTM_LTE_NS_C,FTM_LTE_NS_C,ftm_diag},

      {FTM_LTE_RX_2_C,FTM_LTE_RX_2_C,ftm_diag},

      {FTM_LTE_CAR1_C,FTM_LTE_CAR1_C,ftm_diag},

      {FTM_LTE_CAR1_RX_2_C,FTM_LTE_CAR1_RX_2_C,ftm_diag},

      {FTM_LTE_CAR2_C,FTM_LTE_CAR2_C,ftm_diag},

      {FTM_LTE_CAR2_RX_2_C,FTM_LTE_CAR2_RX_2_C,ftm_diag},

#endif
#ifdef FEATURE_CGPS

      {FTM_GNSS_C,FTM_GNSS_C,ftm_diag},

#endif

#ifdef FTM_HAS_SV_API
      {FTM_SV_C,FTM_SV_C,ftm_diag},
#endif
      {FTM_SEQ_C,FTM_SEQ_C,ftm_diag},

      {FTM_INO_C,FTM_INO_C,ftm_diag},

      {FTM_MEAS,FTM_MEAS,ftm_diag},

      {FTM_HWTC,FTM_HWTC,ftm_diag},

      {FTM_RFNV_C,FTM_RFNV_C,ftm_diag},

#ifdef FTM_HAS_TDSCDMA
      
      {FTM_TDSCDMA_C,FTM_TDSCDMA_C,ftm_diag},
      
      {FTM_TDSCDMA_RX_2_C,FTM_TDSCDMA_RX_2_C,ftm_diag},
	  
      {FTM_TDSCDMA_C2_C,FTM_TDSCDMA_C2_C,ftm_diag},
      
      {FTM_TDSCDMA_C3_C,FTM_TDSCDMA_C3_C,ftm_diag},
      
      {FTM_TDSCDMA_BER_C,FTM_TDSCDMA_BER_C,ftm_diag},
      
#endif /*FTM_HAS_TDSCDMA*/
      {FTM_RESERVED_C,FTM_RESERVED_C,ftm_diag}

};

// If there are other subsystems registering for 128 messages, remove the
// ifdef from here and from around the registration macro, and put it around
// individual elements in this table. 

#ifdef FEATURE_FTM_HWTC
// All HWTC commands should go to ftm_diag, which will dispatch
// through ftm_hwtc_dispatch()
static const diagpkt_user_table_entry_type ftmhwtcdiag_tbl[] = 
{
  {0x0000, 0xFFFF, ftm_diag},
};
#endif 

/*===========================================================================

FUNCTION FTMDIAG_INIT

DESCRIPTION
  Register the subsystem function with diag.

============================================================================*/
void ftmdiag_init (void)
{
  DIAGPKT_DISPATCH_TABLE_REGISTER ((uint16)DIAG_SUBSYS_FTM, ftmdiag_tbl);
#ifdef FEATURE_FTM_HWTC
  DIAGPKT_DISPATCH_TABLE_REGISTER ((uint16)DIAG_SUBSYS_HWTC, ftmhwtcdiag_tbl); 
#endif
}

#endif   /* FEATURE_FACTORY_TESTMODE */
