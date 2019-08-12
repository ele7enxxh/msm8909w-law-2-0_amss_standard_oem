/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  C A L L   M A N A G E R   L O W E R   L A Y E R


I N T E R F A C p   M O D U L E
GENERAL DESCRIPTION
  This source file contains the function definitions necessary for the
  Call Manager module and the Lower Layers to send commands to one
  another.

EXTERNALIZED FUNCTIONS
  cm_mc_rpt                        - Allows MC to send a command to CM.
  cm_mc_get_buf_else_err_fatal     - Allocates a MC command buffer.
  cm_mc_rpt_get_buf_else_err_fatal - Allocates a MC rpt command buffer.
  cm_mc_send_cmd                   - Allows CM to send a command to MC.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2002 - 2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmxll.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/09/12   gm      Integrated missing msg interface from hdr CP
03/23/12   gm      DIME integration fix: TMC deprecation and others.
01/10/12   gm      Mainlining HDR API change integration
01/06/12   gm      Data synchronization changes for freefloating platform
12/19/11   fj      Remove feature FEATURE_1XCP_MEM_OPT.
11/18/11   rn      1xCP mem opti changes
11/02/11   gm      Changes for HDR API cleanup
08/22/11   aj      Remove unlock HDR request in SVDO mode
04/11/11   am      Cleaning stubs and runnning qtf SUTE
03/21/11   aj      featurize ABORT to DO cp during reselection
03/16/11   aj      support to send ABORT to DO cp during reselection
02/28/11   aj      Updates to DO->LTE reselection
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
02/01/11   rm      Removing cm_wms_bc_partial_addr_match
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
09/23/10   xs      Superlint critical/high error fixed
03/15/10   sv      Removing use of snd API
03/24/09   sv      updated to cmtaski.h
02/25/09   sv      Remove cmxll_v.h
02/20/09   rm      Lint clean post CMI changes
06/06/08   sv      Featurizing snd.h under FEATURE_8K_TO_BE_PURGED
01/28/07   jqi/aj     Added FEATURE_FULL_TIME_SHDR_3 support.
08/07/07   ic      Lint clean up
03/09/07   sk      Lint fix.
03/07/07   jqi     Added support for FEATURE_SMS_OVER_IP
08/14/06   sk      Fixed cm_mc_rpt_free_buf() to properly free the MC rpt buf
01/12/06   ic      For specific calls / walks Lint warnings, replaced global
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup
11/18/05   sk      Added CM API cm_mc_rpt_free_buf() to free CM MC reports.
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint
                   comments for 613 Lint error
08/22/05   ic      Fixed Lint warnings cm_send_hdr_unlock_rf_cmd() not
                   referenced
08/16/05   ic      Made cm_send_hdr_unlock_rf_cmd() static
                   Added cm_mc_unlock_resources()
                   Removed call to cm_send_hdr_unlock_rf_cmd() from
                   cm_mc_send_cmd()
07/15/05   ic      Featurization changes to support CDMA + GSM build flavor
06/06/05   sk      Renamed CMDBG_PRINT_LL_CMD_FUNC to CMDBG_PRINT_MC_CMD_FUNC
01/07/05   ic      Removed all code under and referring to FEATURE_UASMS
12/03/04   ws      Support for Remote WMS.
09/09/04   ws      Add cm_mc_rpt_get_buf_else_err_fatal().
08/04/04   ic      CM Lint clean up - cmxdbm.c with Lint supression verified
                   and free of Lint issues in all CUTE configurations.
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
                   Added cmxll_dummy() to get rid of ADS 1.2 compiler warning
06/08/04   ank     Mainlined FEATURE_GPSONE_DBM.
02/12/04   ic      Merged in changes from MSMSHARED_CM.01.00.10.00.20 to
                   MSMSHARED_CM.01.00.10.00.23
02/06/04   ws      Initial jaguar/mainline merge.
01/18/04   sj      Added debug to profile cm_rpt_q.
11/03/03   ic      Fixed a typo in cm_mc_send_cmd() comment
10/23/03   ic      Removed FEATURE_MSM6500_CM_TEMP
10/20/03   ic      Replaced MC_UNLOCK_HDR_RF_F with HDRMC_UNLOCK_HDR_RF_F
10/07/03   sj      Send unlock RF cmd to HDR before doing 1x origs.
05/15/03   vt      Fixed a compile problem in err_fatal function.
04/28/03   AT      Lint cleanup.
02/18/03   vt      Fixed problems to build for HDR.
01/24/03   vt      Fixed the Header path information.
01/21/03   ws      Updated copyright information for 2003
08/06/02   HQ      Added FEATURE_CDSMS.
03/22/02   PRK     Initial version
===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/
/*lint -esym(766,customer.h)*/
#include "mmcp_variation.h"
#include "customer.h"                       /* Customer configuration file */

#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
#include "comdef.h"               /* Definition for basic types and macros */
#include "mc.h"                        /* Interface to cdma protocol stack */
#include "mc_v.h"
#include "cmdbg.h"                       /* Interface to CM debug services */
#include "cmcall.h"                      /* Interface to CM call object */
#include "cmtaski.h"                      /* Interface to CM task services */
#include "cmxll.h"                     /* MC to CM interface */
#include "cmph.h"                        /* Interface to CM phone object */
#include "cmdbg.h"              /* Interface to CM debug utility functions */
#include "cm_msgr_msg.h"
#include "cmi.h"

#if defined(FEATURE_CDSMS)
#include "cmxsms.h"
#endif /* defined(FEATURE_CDSMS) */

#ifdef FEATURE_GPSONE
#include "cmxpd.h"
#endif /* FEATURE_GPSONE */

#include "wms_modem.h"

#ifdef FEATURE_HDR_HYBRID
#include "cmxdbm.h"     /* Interface to Data Burst Messages */
#endif

#ifdef CM_DEBUG
#error code not present
#endif


/**--------------------------------------------------------------------------
** Functions
** --------------------------------------------------------------------------
*/

/****************************************************************************
              FUNCTIONS CALLED BY MC TO COMMUNICATE WITH CM
                              FOR MC'S USE
****************************************************************************/


/*===========================================================================

FUNCTION CM_MC_RPT

DESCRIPTION
  Allows MC to queue up messages to the CM.  Grab a buffer from the
  cm_mc_rpt_free_q, fill it in, and call cm_mc_rpt with it.  All this will
  do is queue it and set a signal for the CM task.  When the command
  is done, the buffer will be queued as directed in the command header,
  and a task you specify may be signaled.

DEPENDENCIES
  cm_rpt_q already initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cm_mc_rpt
(
  cm_mc_rpt_type *cmd_ptr                           /* command block to CM */
)
{
  cm_cmd_q_type *cm_rpt_q = cmtask_get_cm_rpt_q();

  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(cmd_ptr           != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_MSG_HIGH_2("RPT Q cnt %d %d ",
               q_cnt( cm_rpt_q ),
               cmd_ptr->hdr.cmd);

  (void) q_link( cmd_ptr, &cmd_ptr->hdr.link );               /* init link */
  q_put( cm_rpt_q, &cmd_ptr->hdr.link );                  /* and queue it */
  (void) rex_set_sigs( get_cm_tcb(), CM_RPT_Q_SIG );      /* signal the CM task */
} /* cm_mc_rpt() */


/*===========================================================================

FUNCTION CM_AUTONAM_IS_ENABLED

DESCRIPTION
  Check whether auto-NAM is currently enabled.

DEPENDENCIES
  Phone object must be initialized

RETURNS
  TRUE if auto NAM is enabled. FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_autonam_is_enabled( void )
{
  return cmph_autonam_is_enabled();
}


#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))

/*===========================================================================

FUNCTION CM_EXT_DISP_BLD_INFO

DESCRIPTION
  This function translates a CAII-format extended display information
  record into a structure that can be iterated to get at the information.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes of the caii_ext_disp_type structure's var field that
  were used to build the packed structure

SIDE EFFECTS
  None

===========================================================================*/
word cm_ext_disp_bld_info
(
  caii_ext_disp_type* disp_ptr,             /* ptr to caii-format info  */
  cm_ext_disp_info*   info_ptr               /* ptr to packed info struct */
)
{
  #ifndef FEATURE_MMODE_REMOVE_1X
  byte* from_ptr;
  cm_ext_disp_subrecord* to_ptr;
  word curr_indx;
  word next_indx;    /* index where next record would start */
  word i;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(disp_ptr           != NULL);
  CM_ASSERT(info_ptr           != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* header info */
  info_ptr->display = disp_ptr->fix.display_type;

  /* variable length part */
  curr_indx = 0;
  from_ptr = &(disp_ptr->var[0]);

  while (curr_indx < disp_ptr->fix.rec_len)
  { /* we have another subrecord to read */
    /* calculate where we will end up if we copy */
    next_indx = curr_indx + (word) FPOS(cm_ext_disp_subrecord,chari) +
                ((caii_ext_disp_var_type*)from_ptr)->display_len;

    if (next_indx <= sizeof(info_ptr->data))
    { /* it will fit */
      to_ptr = (cm_ext_disp_subrecord*)&(info_ptr->data[curr_indx]);

      to_ptr->display_tag = ((caii_ext_disp_var_type*)from_ptr)->display_tag;
      to_ptr->display_len = ((caii_ext_disp_var_type*)from_ptr)->display_len;

      curr_indx += (word) FPOS(cm_ext_disp_subrecord,chari);
      for (i=0 ; 
           ((curr_indx<next_indx)&&(i<CM_EXTDISP_MAX_SUBREC_LEN)&&
           (i<to_ptr->display_len));
           curr_indx++,i++)
      {
        to_ptr->chari[i] = ((caii_ext_disp_var_type*)from_ptr)->chari[i];
      }
    }
    else
    { /* no more will fit */
      break;
    }

    /* --------------------------------------------------
    ** Update sub-record pointer in the internal format
    ** -------------------------------------------------- */
    from_ptr += FPOS( caii_ext_disp_var_type, chari) +
                ((caii_ext_disp_var_type *)from_ptr)->display_len;

  } /* while */

  /* record the length of the packed data */
  info_ptr->datalen = (byte)curr_indx;

  /* return number of bytes used */
  return((word)(from_ptr - &(disp_ptr->var[0])));
  #else
  #error code not present
#endif
}
#endif /* #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */


#if(defined(FEATURE_CDSMS) || defined(FEATURE_GPSONE))
/*===========================================================================

FUNCTION cm_mt_dbm_msg

DESCRIPTION
  This function should be used as the **one** function MC should call when
  a mobile-terminated data burst message is received. This function will
  dispatch either the SMS callback or PD callback depending on the
  type of message received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void cm_mt_dbm_msg(
  uint8 dbm_burst_type,
    /* The data burst type */
  uint8 format,
    /* The format of the data burst type. Relevant only for SMS */
  const uint8* data,
    /* Pointer to the paylaod */
  uint16 data_len
    /* The number of bytes in the payload */
  )
{
  #ifndef FEATURE_MMODE_REMOVE_1X
  switch(dbm_burst_type)
  {

    case CAI_SHORT_MSG_SERVICES:
      #ifdef FEATURE_CDSMS
      cm_mt_sms_msg(format, data_len, data);
      #else  /* !FEATURE_CDSMS */
      /* This should never happen, according to MC... but doesn't hurt to check */
      CM_MSG_HIGH_0("CM rcvd MT SMS message, but FEATURE_CDSMS not enabled");
      #endif  /* !FEATURE_CDSMS */

      break;

    default:
      sys_err_fatal_invalid_value_exception((int) dbm_burst_type);
  }
  #else
  #error code not present
#endif /* FEATURE_MMODE_REMOVE_1X */

} /* cm_mt_dbm_msg() */
#endif /* defined(FEATURE_CDSMS) || (defined(FEATURE_GPSONE) ) */


#ifdef FEATURE_CDSMS
/*===========================================================================

FUNCTION CM_MT_SMS_MSG

DESCRIPTION
 CM provides this function for MC to deliver mobile terminated sms message

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cm_mt_sms_msg
(
    uint8         format,                        /* MT SMS message format */
    uint16        len,                                      /* msg length */
    const uint8   *data                             /* msg buffer pointer */
)
{

  cmxsms_mt_msg( format, len, data);

} /* cm_mt_sms_msg() */
#endif /* FEATURE_CDSMS */


/*===========================================================================

FUNCTION cm_mt_dbm_reply_is_within_t2m

DESCRIPTION
 CM provides this function for MC to decide if the response to an incoming
 PD DBM is timing-constrainted.

DEPENDENCIES
  none

RETURN VALUE
  True if there is a PD response to be sent out right away for a given request.
  Otherwise False.

SIDE EFFECTS
  Nothing.

===========================================================================*/
boolean cm_mt_dbm_reply_is_within_t2m (
  uint8 dbm_msg_type,                               /* The data burst type */
  uint8 format,/* The format of the data burst type. Relevant only for SMS */
  uint8      *payload,                               /* msg buffer pointer */
  uint32     numBytes                                        /* msg length */
)
/*lint -esym(715,format)*/
{
  cm_dbm_s_type    *dbm_ptr = cmdbm_ptr();
  /* point at position determination interface object */
  
  cm_pd_resp_is_within_t2m_f_type *t2m_cb_func = NULL;
  /* Function pointer to check if time constraint response is needed */

  CM_ASSERT( dbm_ptr != NULL );

  t2m_cb_func = dbm_ptr->cm_pd_resp_is_within_t2m_func;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(dbm_msg_type)
  {
    #ifndef FEATURE_MMODE_REMOVE_1X
    case CAI_POSITION_DET:
      #ifdef FEATURE_GPSONE
      if (t2m_cb_func == NULL )
      {
        /* By default, no time constraints resp. */
        return FALSE;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Consult the position determination callback function to see if time
      ** constaint response is needed.
      */
      return t2m_cb_func( payload, numBytes );

      #else
      /* This should never happen, according to MC... but doesn't hurt to
      ** check */
      CM_ERR_0("CM rcvd IS-801 message, but FEATURE_GPSONE not enabled");
      return FALSE;
      #endif


    case CAI_SHORT_MSG_SERVICES:
      #ifdef FEATURE_CDSMS
      /* Call SMS call back function to determine
      ** if response is within T2M
      */
      #else
      /* This should never happen, according to MC... but doesn't hurt to
      ** check */
      CM_MSG_HIGH_0("CM rcvd MT SMS message, but FEATURE_CDSMS not enabled");
      #endif
      break;
    #endif /*FEATURE_MMODE_REMOVE_1X*/

    default:
      sys_err_fatal_invalid_value_exception((int) dbm_msg_type);
      break;
  }

  SYS_ARG_NOT_USED(numBytes);
  SYS_ARG_NOT_USED(t2m_cb_func);
  SYS_ARG_NOT_USED(payload);
  SYS_ARG_NOT_CONST(payload);

  /* For any case coming to here, always return FALSE. */
  return FALSE;
}
/*lint +esym(715,format)*/


/****************************************************************************
           FUNCTIONS CALLED BY CM TO COMMUNICATE WITH LOWER LAYERS
                        ** FOR INTERNAL USE OF CM **
****************************************************************************/



/*===========================================================================

FUNCTION CM_MC_GET_BUF_ELSE_ERR_FATAL

DESCRIPTION
  Allocate a MC Task command buffer. Error fatal if allocation is NOT
  successful.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated MC command buffer.

SIDE EFFECTS
  none

===========================================================================*/
mc_msg_type *cm_mc_get_buf_else_err_fatal( void )
{

  mc_msg_type *mc_cmd_ptr = (mc_msg_type*) mc_alloc_queue_buf( MC_CMD_BUF_Q );
  /* Allocate memeory from modem heap */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If allocation failed, error fatal.
  */
  if( mc_cmd_ptr == NULL )
  {
    sys_err_fatal_null_ptr_exception();
  }

  #ifndef FEATURE_MMODE_QTF
  #ifndef FEATURE_MMODE_REMOVE_1X
  /* If we got here, allocation is successful
  */
  /* specify NO task to be signalled when done
  */
  mc_cmd_ptr->hdr.cmd_hdr.task_ptr    = NULL;
  
  #else
  #error code not present
#endif
  #endif /* !FEATURE_MMODE_QTF */

  return mc_cmd_ptr;

} /* cm_mc_get_buf_else_err_fatal() */


#ifdef FEATURE_HDR_HYBRID
/*========================================================================

FUNCTION CM_SEND_HDR_UNLOCK_RF_CMD

DESCRIPTION
  This function sends a unlock RF command to HDR.

DEPENDENCIES
  The HDRMC tasks must have been started up already.

RETURN VALUE
  None

SIDE EFFECTS
  None
==========================================================================*/
void cm_send_hdr_unlock_rf_cmd(void)
{
  hdrcp_msg_req_u      *hdrcp_req;

  hdrcp_req = (hdrcp_msg_req_u*) cm_mem_malloc(sizeof(hdrcp_msg_req_u));
  if(hdrcp_req == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  memset(hdrcp_req, 0, sizeof(hdrcp_msg_req_u));

  CM_MSG_HIGH_0("send HDR_CP_UNLOCK_HDR_RF_REQ");
  (void) cm_msgr_send( HDR_CP_UNLOCK_HDR_RF_REQ, MSGR_MM_CM, &(hdrcp_req->hdr), sizeof(hdrcp_msg_req_u) );

  cm_mem_free(hdrcp_req);
  
}

/*========================================================================

FUNCTION CM_SEND_HDR_ABORT_RESEL_CMD

DESCRIPTION
  This function sends an ABORT_RESEL command to HDR.

DEPENDENCIES
  The HDRMC tasks must have been started up already.

RETURN VALUE
  None

SIDE EFFECTS
  None
==========================================================================*/
void cm_send_hdr_abort_resel_cmd(sys_modem_as_id_e_type        asubs_id)
{
  #ifdef FEATURE_MMODE_LTE_RESEL

  hdrcp_msg_req_u      *hdrcp_req;

  hdrcp_req = (hdrcp_msg_req_u*) cm_mem_malloc(sizeof(hdrcp_msg_req_u));
  if(hdrcp_req == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  memset(hdrcp_req, 0, sizeof(hdrcp_msg_req_u));

  CM_MSG_HIGH_0("send HDR_CP_CM_ABORT_LTE_RESEL_REQ");
  (void) cm_msgr_send( HDR_CP_CM_ABORT_LTE_RESEL_REQ, MSGR_MM_CM, &(hdrcp_req->hdr), sizeof(hdrcp_msg_req_u) );

  cm_mem_free(hdrcp_req);


  return;
  #endif
}
#endif // FEATURE_HDR_HYBRID

#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))

/*========================================================================

FUNCTION 1.	MC_ABORT_1X_TO_LTE_RESEL_F 

DESCRIPTION
  This function sends an ABORT_RESEL command to 1X.

DEPENDENCIES
  The HDRMC tasks must have been started up already.

RETURN VALUE
  None

SIDE EFFECTS
  None
==========================================================================*/
void cm_send_1x_abort_resel_cmd(sys_modem_as_id_e_type        asubs_id)
{
  #ifndef FEATURE_MMODE_REMOVE_1X
  mc_msg_type               *mc_ptr = NULL;
    /* MC command pointer */
    
  mc_ptr                     = cm_mc_get_buf_else_err_fatal();
  
  CM_ASSERT( mc_ptr != NULL );
  
  mc_ptr->hdr.cmd                       = MC_ABORT_1X_TO_LTE_RESEL_F;
  
  CM_MSG_HIGH_1( "Domsel: Send MC_ABORT_1X_TO_LTE_RESEL_F to 1XCP %d", asubs_id);
  
  cm_mc_send_cmd( mc_ptr );
  #else
  #error code not present
#endif
}

#endif

/*========================================================================

FUNCTION CM_MC_UNLOCK_RESOURCES

DESCRIPTION

  This function is called by mccdma.c from mcc_wait() when MC task got a
  command queue signal, just after srch_rude_wakeup() was called.

  Send HDRMC_UNLOCK_HDR_RF_F if we have a call on 1X of type
  voice, sms, pd, otasp, std_otasp, e911 in origination state or
  when sms / dbm is in progress

DEPENDENCIES
  The HDRMC tasks must have been started up already.

RETURN VALUE
  None

SIDE EFFECTS
  None
==========================================================================*/
void cm_mc_unlock_resources(void)
{
  #ifdef FEATURE_HDR_HYBRID

  unsigned int                 i    = 0;
    /* Loop counter */

  cm_call_type_e_type call_types[]  = {
                                        CM_CALL_TYPE_VOICE,
    /* Call types to check */           CM_CALL_TYPE_SMS,
                                        CM_CALL_TYPE_PD,
                                        CM_CALL_TYPE_STD_OTASP,
                                        CM_CALL_TYPE_NON_STD_OTASP,
                                        CM_CALL_TYPE_EMERGENCY
                                      };

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* If UE supports SVDO or it is not in hybrid operation, 
  ** do not send Unlock HDR command
  */
  if( cmph_ptr()->is_svdo_supported ||
      !cm_get_cur_hybr_status()
    )
  {
     return;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check for SMS or DBM in progress, if so
  ** send HDRMC_UNLOCK_HDR_RF_F
  */
  if(cmxsms_is_mo_sms_in_progress() ||
     cmxdbm_is_mo_dbm_in_progress())
  {
    cm_send_hdr_unlock_rf_cmd();
    return;
  }

  /* If we find a call of required type and it's in origination state,
  ** send HDRMC_UNLOCK_HDR_RF_F
  */
  for(i = 0; i < ARR_SIZE(call_types); i++)
  {
    if(cmcall_check_call_type_in_call_state( call_types[i],
                                             CM_CALL_STATE_ORIG))
    {
      cm_send_hdr_unlock_rf_cmd();
      break;
    }
  }
  #endif
}

/*===========================================================================

FUNCTION CM_MC_SEND_CMD

DESCRIPTION
  Send a command to the MC task.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_mc_send_cmd
(
  mc_msg_type *mc_cmd_ptr                         /* Pointer to mc command */
)
{
  CMDBG_PRINT_MC_CMD_FUNC( mc_cmd_ptr );

  #ifndef FEATURE_MMODE_REMOVE_1X
  mc_cmd( mc_cmd_ptr );
  #else
  #error code not present
#endif

} /* cm_mc_send_cmd() */


/*===========================================================================

FUNCTION cm_mc_rpt_get_buf_else_err_fataL

DESCRIPTION
  Allocate a MC rpt buffer. Error fatal if allocation is NOT
  successful.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated MC rpt buffer.

SIDE EFFECTS
  none

===========================================================================*/
cm_mc_rpt_type *cm_mc_rpt_get_buf_else_err_fatal
(
  void
)
{

  cm_mc_rpt_type         *mc_rpt_ptr;
    /* CM MC report pointer */

  /* Allocate memory */
  mc_rpt_ptr = (cm_mc_rpt_type*) cm_mem_malloc(sizeof(cm_mc_rpt_type));

  /* If allocation failed, error fatal.
  */
  if( mc_rpt_ptr == NULL )
  {
    sys_err_fatal_null_ptr_exception();
  }

  mc_rpt_ptr->hdr.task_ptr = NULL;
  #ifdef CM_DEBUG
  #error code not present
#endif

  return mc_rpt_ptr;

}  /* cm_mc_rpt_get_buf_else_err_fatal */


/*===========================================================================

FUNCTION cm_mc_rpt_free_buf

DESCRIPTION
  Free a MC report buffer allocated from CM. Do NOT use to function to
  indicate the completion of processing of the report. Use cm_ret_rpt_buf for
  that.

DEPENDENCIES
  none

RETURN VALUE
  none.

SIDE EFFECTS
  none

===========================================================================*/
void cm_mc_rpt_free_buf( cm_mc_rpt_type *mc_rpt_ptr )
{

  CM_ASSERT( mc_rpt_ptr != NULL );

  cm_mem_free(mc_rpt_ptr);

} /* cm_mc_rpt_free_buf() */


/*=========================================================================
FUNCTION
  cm_wms_bc_addr_match

DESCRIPTION
  This function checks the BC_ADDR of a received broadcast page or
  broadcast message to see if it passes the filter.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - the mobile should process the received message
  FALSE - the mobile should discard the received message

SIDE EFFECTS
  None

=========================================================================*/
boolean cm_wms_bc_addr_match
(
  byte *bc_addr_ptr
)
/*lint -esym(818,bc_addr_ptr) */
{

  return wms_bc_addr_match( bc_addr_ptr );

}  /* cm_wms_bc_addr_match */
/*lint +esym(818,bc_addr_ptr) */


#else /* if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) */

/*===========================================================================
FUNCTION cmxll_dummy

DESCRIPTION

  Dummy function, defined to get rid of
  'Warning: C3040E: no external declaration in translation unit'
  ADS 1.2 compiler warning

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmxll_dummy( void )
/*lint -esym(714,cmxll_dummy) */
/*lint -esym(765,cmxll_dummy)
** Can't be static as it would result in compiler warnings
*/
{
}

#endif /* if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) */

