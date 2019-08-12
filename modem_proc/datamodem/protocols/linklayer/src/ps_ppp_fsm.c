/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                           P S _ P P P _ F S M . C

GENERAL DESCRIPTION
 PPP Finite State Machine

 Acknowledgements and correction history may be found in PPP.C

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright 1992-1994 William Allen Simpson
    Licensed to Qualcomm -- 95 Jan 21

Copyright (c) 1995-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/MM_DATA/ps_ppp_fsm.c_v 1.6 12 Feb 2003 14:29:52   mlioy  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/src/ps_ppp_fsm.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/05/13    cx     Fixed warnings 
10/25/12    fn     Remove usage of fatal error log messages
11/28/11    jz     ppp control packet on dos(FEATURE_DATA_SEND_PPP_CTL_ON_DOS) 
10/17/11    ssh    eHRPD Always On support
04/19/11    op     Fixed klocwork warnings
03/23/11    op     Data SU API cleanup
02/24/11    ash    Cleaning up global variables for Q6 free floating effort.
01/16/11    ss     Added some log messages.
07/30/10    mga    Made changes to protocol reject processing
05/13/10    ssh    Added support for PDN Inactivity timer
10/16/09    mga    Made changes to use send_options_mask
10/05/09    mga    Merged changes related to diag events
09/29/09    jee    Fixed klocwork errors
08/31/09    mga    Merged from eHRPD branch
08/12/09    mga    Fix in fsm_timeout
07/14/09    pp     ppp_cb_lock changes.
06/25/09    mga    Fix in fsm_makeoptions
06/24/09    am     Renamed hton/ntoh macros with ps_ prefix.
05/07/09    pp     CMI Phase-4: SU Level API Effort.
11/10/08    scb    Fixed QTF compilation warnings.
09/12/08    pp     Metainfo optimizations.
09/02/08    scb    Added code to delineate FSM functionality from PPP.
05/27/08    rnp    Added messages and additional functionality reqd. for IPCP
                   STA tests.
05/04/08    scb    PPP STA development
12/22/07    scb    Removed a NULL check added as a part of Klocwork
                   fixes in fsm_send() for bp.
09/27/07    ifk    Removed old PPP statistics code.
04/27/07    scb    Removed LINT suppressions
03/23/07    ssh    Post PPP_STARTING_EV *after* sending the Configure-Request
01/10/07    ssh    Added support for vendor-specific packets
12/21/06    ssh    Set MSG_EXPEDITE flag for LCP Term-Req and Term-Ack
11/01/06    ssh    Set MSG_EXPEDITE flag for ECHO_REPLY message
10/23/06    ssh    Validate header length in ntohcnf()
05/25/06    ssh    Added fsm_find_option()
03/05/06    ifk    Pass rejected option to protocol specific handling
                   function.
11/05/05    rt     Fixed coverity errors.
10/27/05    ssh    Initialize remote.work to 0 in fsm_request()
08/16/05    ssh    Fixed problem where C-NAK contains acceptable options
                   after resync
08/15/05    kr     Added support for stats update
07/28/05    ssh    Added error messages for sanity checking in fsm_down(),
                   fsm_open() and fsm_close()
07/21/05    ssh    Changes pursuant to new ppp_fail_reason_e_type
06/23/05    ssh    Fixed potential issue with put32() in fsm_send()
04/15/05    ifk    Don't process truncated packet in fsm_proc()
03/25/05    ifk    Fixed fsm_n_option() handling
03/22/05    ssh    Added posting of PPP resync event to diag
03/21/05    ssh    Changes pursuant to the new typedef ps_timer_handle_type
02/25/05    ifk    Fixed bug where if retry_nak has expired the option values
                   in config reject were invalid.
01/27/05    ssh    Changed include file name dsbyte.h to ps_byte.h
12/29/04    ifk    Fix problem where config nak contains acceptable options
                   after a resync.  Send code reject for non-LCP options 8-15
12/27/04    ifk    Replaced setting of tracer field with DSM_TOUCH_ITEM
12/16/04    mct    Now posting PPP_DOWN_EV if trying to open fsm in stopped
                   state.
09/23/04    jd     Error message when c-req can't be sent due to no dsms
08/17/04    mvl    Fixed messaging to be more useful in debugging.
08/10/04    ifk    For a protocol reject cause a TLF event if moving to
                   STOPPED state and a TLD if moving to Stopping state
07/16/04    ifk    On resync call ppp_failover_on_resync() to handle failover
07/01/04    ifk    In fsm_send() use protocol field instead of dsm_kind
06/05/04    ifk    Changed resync and down event callbacks to use new event
                   architecture.  Replaced fsmi_resync_indicate() with
                   call_event_callback_fcn()
06/03/04    ifk    On a network resync start setup timer only for LCP resync
04/20/04    ifk    Move PPP_DOWN_EV callback out of fsm_down()
04/16/04    jd     Commented out change below since it breaks handoffs
03/15/04    ifk    Added call to PPP registered callback in fsm_down().
02/24/04    mvl    Added JCDMA Term Req behaviour rather than Abort().
02/10/04    ifk    Added call to fsm_down() if PROTO_REJ received for IPV6CP.
11/25/03    mct    Cancel ps timers and call fsm_irc() in fsm_init to
                   fix a timer expiration bug during multiple resyncs.
10/31/03    ifk    Fixed bug in fsmi_resync_indicate() where unsigned enum
                   wraps around in the for loop.
10/27/03    ifk    Added fsmi_resync_indicate()
10/24/03    ifk    Replaced fsm_timer() with direct calls to ps_timer_start.
10/09/03    ifk    Generate a resync event when a C_ACK is received when up.
09/16/03    usb    Added call to fsm_init() for network initoiated resync.
09/12/03    mvl    Added some JCDMA timing changes.
08/13/03    ifk    Updated event handling and seperated LCP and NCP in
                   preparation for IP6CP.
04/29/03    jd     Added back retry counter for sending LCP term-req's
04/18/03    vsk    use fsm->mode instead of fsm->passive mode to set passive
                   mode operation
04/08/03    mvl    fixed fsm_ack().
03/28/03    mvl    Cleanup.
02/06/03    mvl    Changed fsm_timeout() to return TRUE if the timeout
                   happened in the open state - as we don't want to give up
                   when already open.
11/14/02   jd/usb  Only add nak'd option to work if will
11/04/02    mvl    Starting setup timer for resyncs.
09/18/02    mvl    Added user_data to ppp event callbacks.
08/13/02    mvl    Added session handle, and using MTU from iface.
07/30/02    mvl    Put TASKLOCK() around invokation of event callbacks.
07/25/02    mvl    Removed netmodel dependency.  Updates for PPP renaming.
07/15/02    aku    Added pstimer.h and removed dssnet.h
07/11/02    mvl    Now calling resync callbacks only when C-Req received in
                   open state.
07/01/02    mvl    Added support for generating an event rather than calling
                   dssnet state machine directly.
05/22/02    mvl    removed non-netmodel ppp_outgoing() calls.
04/17/02    rc     Wrapped code in !FEATURE_DATA_WCDMA_PS as appropriate to
                   build for all targets
03/01/02    vr     Added support for PPP passive mode.
03/01/02    vsk    removed dsm_free_packet() being called in case of
                   ppp_outgoing() returns an error
12/21/01    dwp    Wrap call to dssnet_sm_post_event in !FEATURE_DATA_PS.
11/12/01    aku    Posting event to dssnet when PPP resyncs
11/12/01    dwp    Add "|| FEATURE_DATA_PS" to whole module, include of tcp.h
90/20/01  jd/mvl   Modified the resetting of the nak retry count and
                   configuration retry so it is more selective.
08/29/01  pjb/mvl  Modified the values of option bits to support DNS options.
08/02/01    mvl    general code cleanup
08/01/01    na     Fixed functions that were using the global ppp_cb_ptr
                   Return from fsm_proc() on error.
07/29/01    na     Removed multiple retries when MS sends Term-Req.
                   Also, MS does not send TReq when it receives T-Ack.
07/27/01    mvl    Changed all of the psi_*_timer() to ps_timer_*() calls
07/26/01    aku    Replaced psi_protocol_closed_event with dsiface_down().
07/23/01    mvl    Removed the string arrays with constants names, modified
                   fsm_send() such that ppp_outgoing() is called with the
                   iface in the fsm struct, removed obsolete debug macros and
                   code cleanup.
07/21/01    pjb    Return type of dsm_pushdown changed.
05/25/01    na     On a resync the retry_cnt was not being set. Now fixed.
05/18/01    mvl    General code cleanup.
08/25/00    rc     Changed dsm_pushdown() to take take pool id as fourth
                   parameter.Made changes to handle dsm_pushdown()
                   returning FALSE.
                   Handled the case when htoncnf() returns NULL.
                   Changed dsm_new_buffer() calls to take pool id as input
                   parameter. So, a new macro DSM_DS_POOL_SIZE is used to
                   return pool id based on pool size. Added Macros that
                   check if there are enough items or if we are running
                   out of items.
                   Changed FEATURE_DS_MEM_CHK to FEATURE_DSM_MEM_CHK.
05/04/00    mvl    Changed calls to ppp_outgoing() to reflect new interface.
04/27/00    na     Fixed a memory leak in fsm_request(), when a option that
                   needed to be rejected was in the middle of a Config-Req.
08/20/99    mvl    Fixed some warnings, and fixed variable names and cleaned
                   up fsm_ack()
08/17/99    smp    Added include files msg.h & err.h.
08/09/99    mvl    Added network model changes
04/08/99    na     Use dsm_kind instead of protocol to tag packets for output
01/27/98    ldg    Removed the DS_SLIM switch.
06/17/97    fkm    FEATURE_xxx Updates (via Automatic Scripts)
02/13/97    na     Removed ps_ppp_tx_wm. CTIA show build.
05/24/96    jjw    Gemini & ISS2 Data Services release.
11/09/95    jjw    IS-99 Compliant Release
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include "amssassert.h"
#include "ps_byte.h"
#include "dsm.h"
#include "ps_ppp.h"
#include "ps_pppi.h"
#include "ps_ppp_fsm.h"
#include "ps_pppi_fsm.h"
#include "err.h"
#include "pstimer.h"
#include "ps_stat_common.h"
#include "ps_stat_ppp.h"
#include "ps_crit_sect.h"
#ifdef FEATURE_DATA_PS_EHRPD
#include "ps_ppp_ncpi.h"
#endif /* FEATURE_DATA_PS_EHRPD */
#include "ds_Utils_DebugMsg.h"
#include "ps_metai_info.h"


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                     GLOBAL DEFINITIONS AND DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*---------------------------------------------------------------------------
  The PPP control block - an array as one is needed per instance.
---------------------------------------------------------------------------*/
static ppp_type ppp_cb_array[PPP_MAX_DEV];        /* PPP ctrl block array */

/*---------------------------------------------------------------------------
  REX Critical Section Lock for each PPP control block
---------------------------------------------------------------------------*/
static ps_crit_sect_type  ppp_cb_lock[PPP_MAX_DEV];

#ifdef FEATURE_STA_PPP
sta_ppp_type sta_ppp_cb_array[STA_PPP_MAX_DEV];/* PPP ctrl block array */

boolean sta_ppp_enable = FALSE;
#endif /* FEATURE_STA_PPP */

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             EXTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION HTONCNF()

DESCRIPTION
  This function will Convert header in host form to network form

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
dsm_item_type  *htoncnf
(
  struct config_hdr *cnf,
  dsm_item_type     *bp
)
{
  register uint8 *cp;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (dsm_pushdown(&bp, NULL, CONFIG_HDR_LEN,
           DSM_DS_SMALL_ITEM_POOL) == CONFIG_HDR_LEN)
  {
    cp = bp->data_ptr;
    *cp++ = cnf->code;
    *cp++ = cnf->identifier;
    (void)put16(cp, cnf->length);
  }
  else
  {
    dsm_free_packet(&bp);
    DATA_ERR_FATAL("Out of Memory");
  }

  return bp;

} /* htoncnf() */



/*===========================================================================

FUNCTION FSM_N_OPTION()

DESCRIPTION
  This function will map and option number back to it mask value.

DEPENDENCIES
  None

RETURN VALUE
  -1 in case option could not be mapped
  option number in case mapping found

SIDE EFFECTS
  None
===========================================================================*/
int fsm_n_option
(
  struct fsm_constant_s *fsm_constants_ptr,
  uint8 option
)
{
  uint8  n_option_limit = fsm_constants_ptr->n_option_limit;
  uint8  *n_option = fsm_constants_ptr->n_option;
  uint8  index;

  for(index = 0 ; index < n_option_limit ; index ++)
  {
    if(n_option[index] == option)
    {
      return index;
    }
  }

  return -1;
}


/*===========================================================================

FUNCTION NTOHCNF()

DESCRIPTION
  This function will Extract header from incoming packet

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int ntohcnf
(
  struct config_hdr *cnf,
  dsm_item_type    **bpp
)
{
  uint8 cnfb[CONFIG_HDR_LEN];
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( cnf == NULL )
  {
    return -1;
  }

  if ( dsm_pullup( bpp, cnfb, CONFIG_HDR_LEN ) < CONFIG_HDR_LEN )
  {
    return -1;
  }

  cnf->code       = cnfb[0];
  cnf->identifier = cnfb[1];
  cnf->length     = get16(&cnfb[2]);

  if( cnf->length < CONFIG_HDR_LEN )
  {
    return -1;
  }

  return 0;

} /* ntohcnf() */



/*===========================================================================
FUNCTION HTONOPT_COPY()

DESCRIPTION
  This function will onvert configuration option header in host form to
  network form and copy data

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void htonopt_copy
(
  dsm_item_type    **bpp,
  struct option_hdr *opt,
  dsm_item_type    **copy_bpp
)
{
  dsm_item_type  *bp = *bpp;
  register uint8 *cp;
  register int used = opt->length - OPTION_HDR_LEN;
  int16 dsmret;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( bp->used + opt->length > bp->size )
  {
    return;
  }
  cp = bp->data_ptr + bp->used;
  *cp++ = opt->type;
  *cp++ = opt->length;

  while ( used-- > 0 )
  {
    dsmret = dsm_pull8(copy_bpp);
    if (-1 == dsmret)
    {
      /* Return the used field is not updated which has the effect of
       * not adding this option */
      LOG_MSG_INFO2_2( "malformed option: type %d, length %d:not adding",
                       opt->type, opt->length );
      return;
    }
    *cp++ = (uint8)dsmret;
  }
  bp->used += opt->length;

} /* htonopt_copy() */



/*===========================================================================
FUNCTION NTOHOPT()

DESCRIPTION
  This function will Extract configuration option header

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int ntohopt
(
  struct option_hdr *opt,
  dsm_item_type    **bpp
)
{
  char optb[OPTION_HDR_LEN];
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( opt == NULL )
  {
    return -1;
  }

  if ( dsm_pullup( bpp, optb, OPTION_HDR_LEN ) < OPTION_HDR_LEN )
  {
    return -1;
  }

  opt->type   = (uint8)optb[0];
  opt->length = (uint8)optb[1];
  return 0;

} /* ntohopt() */



/*===========================================================================
FUNCTION FSM_SEND()

DESCRIPTION
  This function will Send a packet to the remote host

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int fsm_send
(
  ppp_fsm_type   *fsm_ptr,
  uint8           code,
  uint16          identifier,
  dsm_item_type  *bp
)
{
  struct config_hdr     hdr;
  ps_tx_meta_info_type  *meta_info_ptr = NULL;

#ifdef FEATURE_DATA_SEND_PPP_CTL_ON_DOS
  ppp_type              *ppp_cb_ptr = NULL;
#endif
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( fsm_ptr == NULL || fsm_ptr->device >= PPP_MAX_DEV )
  {
    LOG_MSG_ERROR_0("Invalid fsm_ptr in fsm_send");
    dsm_free_packet(&bp);
    return -1;
  }

  LOG_MSG_INFO2_3( "fsm_send: dev=%d, code=%d, id=%d",
                 fsm_ptr->device, code, identifier );

#ifdef FEATURE_DATA_SEND_PPP_CTL_ON_DOS
  ppp_cb_ptr = ppp_cb_array + (uint8)fsm_ptr->device;
  if( NULL == ppp_cb_ptr )
  {
    LOG_MSG_ERROR_0("Invalid ppp_cb_ptr in fsm_send");
    dsm_free_packet(&bp);
    return -1;
  }
#endif
  PPP_INC_INSTANCE_FSM_STATS(fsm_ptr,pkts_tx,1);

  switch( hdr.code = code )
  {
  case VENDOR_SPECIFIC:
    hdr.identifier = (uint8)identifier;
    break;

  case CONFIG_REQ:
    PPP_INC_INSTANCE_FSM_STATS(fsm_ptr,config_requests_tx,1);
    /*-----------------------------------------------------------------------
      Save ID field for match against replies from remote host
    -----------------------------------------------------------------------*/
    fsm_ptr->lastid = fsm_ptr->id;
    /*-----------------------------------------------------------------------
      Use a unique ID field value
    -----------------------------------------------------------------------*/
    hdr.identifier = fsm_ptr->id++;
    break;

  case TERM_REQ:
    PPP_INC_INSTANCE_FSM_STATS(fsm_ptr,term_requests_tx,1);
    /*-----------------------------------------------------------------------
      Save ID field for match against replies from remote host
    -----------------------------------------------------------------------*/
    fsm_ptr->lastid = fsm_ptr->id;
    /*-----------------------------------------------------------------------
      Use a unique ID field value
    -----------------------------------------------------------------------*/
    hdr.identifier = fsm_ptr->id++;
#ifdef FEATURE_DATA_PS_EHRPD
#ifdef FEATURE_DATA_SEND_PPP_CTL_ON_DOS
    if( ppp_cb_ptr->dos_is_enabled )
    {
      LOG_MSG_INFO2_1( "fsm_send code:%d on dos", code);
      /*-----------------------------------------------------------------------
        Set flag in meta info indicating this message
        should be expedited
      -----------------------------------------------------------------------*/
      PS_TX_META_INFO_AND_RT_META_INFO_GET( meta_info_ptr );
      if( meta_info_ptr != NULL )
      {
        PS_PKT_META_INFO_GET(PS_TX_META_GET_PKT_META_INFO_PTR(meta_info_ptr));
        if( PS_TX_META_GET_PKT_META_INFO_PTR(meta_info_ptr) != NULL )
        {
          PS_TX_META_SET_TX_FLAGS(meta_info_ptr, MSG_EXPEDITE);
        }
      }
      else
      {
        LOG_MSG_INFO2_1( "fsm_send: allocate meta info for DOS fail, code:%d",
                         code);
      }
    }
#endif /* FEATURE_DATA_SEND_PPP_CTL_ON_DOS */

    /*-----------------------------------------------------------------------
      If this a 3GPP2-VSNCP T-Req, the PDN-ID option must be added
    -----------------------------------------------------------------------*/
    VSNCP_3GPP2I_ADD_PDN_ID_OPT( fsm_ptr, &bp );
#endif /* FEATURE_DATA_PS_EHRPD */
    break;

  case ECHO_REQ:
    PPP_INC_INSTANCE_FSM_STATS(fsm_ptr,echo_requests_tx,1);
    /*-----------------------------------------------------------------------
      Save ID field for match against replies from remote host
    -----------------------------------------------------------------------*/
    fsm_ptr->lastid = fsm_ptr->id;
    /*-----------------------------------------------------------------------
      Use a unique ID field value
    -----------------------------------------------------------------------*/
    hdr.identifier = fsm_ptr->id++;
    break;

  case PROT_REJ:
    PPP_INC_INSTANCE_FSM_STATS(fsm_ptr,proto_rejects_tx,1);
    /*-----------------------------------------------------------------------
      Use a unique ID field value
    -----------------------------------------------------------------------*/
    hdr.identifier = fsm_ptr->id++;
    break;

  case DISCARD_REQ:
    PPP_INC_INSTANCE_FSM_STATS(fsm_ptr,discard_requests_tx,1);
    /*-----------------------------------------------------------------------
      Use a unique ID field value
    -----------------------------------------------------------------------*/
    hdr.identifier = fsm_ptr->id++;
    break;

  case CONFIG_ACK:
    PPP_INC_INSTANCE_FSM_STATS(fsm_ptr,config_acks_tx, 1) ;
    /*-----------------------------------------------------------------------
      Use ID sent by remote host
    -----------------------------------------------------------------------*/
    hdr.identifier = (uint8)identifier;
    break;

  case CONFIG_NAK:
    PPP_INC_INSTANCE_FSM_STATS(fsm_ptr,config_naks_tx, 1) ;
    /*-----------------------------------------------------------------------
      Use ID sent by remote host
    -----------------------------------------------------------------------*/
    hdr.identifier = (uint8)identifier;
    break;

  case CONFIG_REJ:
    PPP_INC_INSTANCE_FSM_STATS(fsm_ptr,config_rejects_tx, 1) ;
    /*-----------------------------------------------------------------------
      Use ID sent by remote host
    -----------------------------------------------------------------------*/
    hdr.identifier = (uint8)identifier;
    break;

  case TERM_ACK:
    PPP_INC_INSTANCE_FSM_STATS(fsm_ptr,term_acks_tx, 1) ;
    /*-----------------------------------------------------------------------
      Use ID sent by remote host
    -----------------------------------------------------------------------*/
    hdr.identifier = (uint8)identifier;
#ifdef FEATURE_DATA_PS_EHRPD
#ifdef FEATURE_DATA_SEND_PPP_CTL_ON_DOS
    if( ppp_cb_ptr->dos_is_enabled )
    {
      LOG_MSG_INFO2_1( "fsm_send code:%d on dos", code);
      /*-----------------------------------------------------------------------
        If this an Term-Ack, add meta info indicating this message
        should be expedited
      -----------------------------------------------------------------------*/
      PS_TX_META_INFO_AND_RT_META_INFO_GET( meta_info_ptr );
      if( meta_info_ptr != NULL )
      {
        PS_PKT_META_INFO_GET(PS_TX_META_GET_PKT_META_INFO_PTR(meta_info_ptr));
        if( PS_TX_META_GET_PKT_META_INFO_PTR(meta_info_ptr) != NULL )
        {
          PS_TX_META_SET_TX_FLAGS(meta_info_ptr,MSG_EXPEDITE);
        }
      }
      else
      {
        LOG_MSG_INFO2_1( "fsm_send: allocate meta info for DOS fail, code:%d",
                         code);
      }
    }
#endif /* FEATURE_DATA_SEND_PPP_CTL_ON_DOS */
    /*-----------------------------------------------------------------------
      If this an 3GPP2-VSNCP T-Ack, the PDN-ID option must be added
    -----------------------------------------------------------------------*/
    VSNCP_3GPP2I_ADD_PDN_ID_OPT( fsm_ptr, &bp );
#endif /* FEATURE_DATA_PS_EHRPD */

    break;

  case CODE_REJ:
    PPP_INC_INSTANCE_FSM_STATS(fsm_ptr,code_rejects_tx, 1) ;
    /*-----------------------------------------------------------------------
      Use ID sent by remote host
    -----------------------------------------------------------------------*/
    hdr.identifier = (uint8)identifier;
#ifdef FEATURE_DATA_PS_EHRPD
    /*-----------------------------------------------------------------------
      If this an 3GPP2-VSNCP Code-Rej, the PDN-ID option must be added
    -----------------------------------------------------------------------*/
    VSNCP_3GPP2I_ADD_PDN_ID_OPT( fsm_ptr, &bp );
#endif /* FEATURE_DATA_PS_EHRPD */

    break;

  case ECHO_REPLY:
    PPP_INC_INSTANCE_FSM_STATS(fsm_ptr,echo_replies_tx, 1) ;
    /*-----------------------------------------------------------------------
      Use ID sent by remote host
    -----------------------------------------------------------------------*/
    hdr.identifier = (uint8)identifier;
    break;

  default:
    /*-----------------------------------------------------------------------
      we're in trouble
    -----------------------------------------------------------------------*/
    return -1;
  };

  switch( code )
  {
  case ECHO_REPLY:
#ifdef FEATURE_DATA_SEND_PPP_CTL_ON_DOS
    if( ppp_cb_ptr->dos_is_enabled )
    {
      LOG_MSG_INFO2_1( "fsm_send code:%d on dos", code);
      /*-----------------------------------------------------------------------
        Add meta info indicating this message should be expedited
      -----------------------------------------------------------------------*/
      PS_TX_META_INFO_AND_RT_META_INFO_GET( meta_info_ptr );
      if( meta_info_ptr != NULL )
      {
        PS_PKT_META_INFO_GET(PS_TX_META_GET_PKT_META_INFO_PTR(meta_info_ptr));
        if( PS_TX_META_GET_PKT_META_INFO_PTR(meta_info_ptr) != NULL )
        {
          PS_TX_META_SET_TX_FLAGS(meta_info_ptr,MSG_EXPEDITE);
        }
      }
      else
      {
        LOG_MSG_INFO2_1( "fsm_send: allocate meta info for DOS fail, code:%d",
                         code);
      }
    }
#endif /* FEATURE_DATA_SEND_PPP_CTL_ON_DOS */
    {
      register lcp_value_type *vp = (lcp_value_type*)fsm_ptr->local.work_pdv;
      vp->magic_number = ps_htonl(vp->magic_number);
      if (dsm_pushdown(&bp,
                       (void *)&(vp->magic_number),
                       sizeof(vp->magic_number),
                       DSM_DS_SMALL_ITEM_POOL) < sizeof(vp->magic_number))
      {
        dsm_free_packet(&bp);
        DATA_ERR_FATAL("Out of Memory");
        return -1;
      }
      vp->magic_number = ps_ntohl(vp->magic_number);
    }
    break;
  
  case ECHO_REQ:
#ifdef FEATURE_DATA_PS_3GPP2_ALWAYS_ON
    /*-----------------------------------------------------------------------
      Add meta info indicating this is an LCP Echo-Req
    -----------------------------------------------------------------------*/
    PS_TX_META_INFO_AND_RT_META_INFO_GET( meta_info_ptr );
    if( meta_info_ptr != NULL )
    {
      PS_PKT_META_INFO_GET(PS_TX_META_GET_PKT_META_INFO_PTR(meta_info_ptr));
      if( PS_TX_META_GET_PKT_META_INFO_PTR(meta_info_ptr) != NULL )
      {
        PS_TX_META_SET_TX_FLAGS(meta_info_ptr,MSG_LCP_ECHO_REQ);
      #ifdef FEATURE_DATA_SEND_PPP_CTL_ON_DOS
        if( ppp_cb_ptr->dos_is_enabled )
        {
          LOG_MSG_INFO2_1( "fsm_send code:%d on dos", code);
          /*-----------------------------------------------------------------
            Set flag to indicate this message should be expedited
          ------------------------------------------------------------------*/
          PS_TX_META_SET_TX_FLAGS(meta_info_ptr,MSG_LCP_ECHO_REQ|MSG_EXPEDITE);
        }
      #endif /*FEATURE_DATA_SEND_PPP_CTL_ON_DOS*/
      }
    }
#endif /* FEATURE_DATA_3GPP2_ALWAYS_ON */
    /* fall-through */
  case DISCARD_REQ:
  case VENDOR_SPECIFIC:
  {
    register lcp_value_type *vp = (lcp_value_type*)fsm_ptr->local.work_pdv;

    vp->magic_number = ps_htonl(vp->magic_number);
    if (dsm_pushdown(&bp,
                     (void *)&(vp->magic_number),
                     sizeof(vp->magic_number),
                     DSM_DS_SMALL_ITEM_POOL) < sizeof(vp->magic_number))
    {
      dsm_free_packet(&bp);
      DATA_ERR_FATAL("Out of Memory");
      return -1;
    }
    vp->magic_number = ps_ntohl(vp->magic_number);
  }
    break;
  default:
    break;
  };

#ifdef FEATURE_DATA_PS_EHRPD
  /*-----------------------------------------------------------------------
    If this an 3GPP2-VSNCP packet, add the OUI
  -----------------------------------------------------------------------*/
  if( VSNCP_3GPP2I_ADD_OUI( fsm_ptr, &bp ) != 0 )
  {
    LOG_MSG_ERROR_0("Unable to add 3gpp2-oui");
    return -1;
  }
#endif /* FEATURE_DATA_PS_EHRPD */

  hdr.length = (uint16)dsm_length_packet(bp) + CONFIG_HDR_LEN;

  /*-------------------------------------------------------------------------
    Prepend header to packet data
  -------------------------------------------------------------------------*/
  bp = htoncnf(&hdr,bp);
  if (bp == NULL)
  {
    return -1;
  }

  /*---------------------------------------------------------------------
    Set the packet priority to the highest then enqueue to PPP ready-for-
    TX watermark.
  ---------------------------------------------------------------------*/
  DSM_TOUCH_ITEM(bp);
  bp->priority = DSM_HIGHEST;

#ifdef FEATURE_DATA_PS_EHRPD
  /*-------------------------------------------------------------------------
    If this is a VSNCP packet, call the activity callback
  -------------------------------------------------------------------------*/
  if( VSNCP_3GPP2I_FSM_IS_VALID( fsm_ptr ) )
  {
    LOG_MSG_INFO2_0("VSNCP TX: Calling activity cback");
    VSNCP_3GPP2_CALL_ACTIVITY_CBACK( fsm_ptr );
  }
#endif /* FEATURE_DATA_PS_EHRPD */
  
  /*-------------------------------------------------------------------------
    Send the built up packet
  -------------------------------------------------------------------------*/
  if( (*fsm_ptr->cbdc->outgoing_pkt_hdlr_fcn)( fsm_ptr->device,
                     fsm_ptr->pdc->protocol,
                     &bp,
                     meta_info_ptr ) == -1 )
  {
    /*---------------------------------------------------------------------
      outgoing_pkt_hdlr_fcn() frees the packet in case of an error
    ---------------------------------------------------------------------*/
    LOG_MSG_INFO1_0( "outgoing_pkt_hdlr_fcn() Error");
  }

  return 0;
} /* fsm_send() */


/*===========================================================================
FUNCTION FSM_FIND_OPTION

DESCRIPTION
  This function will examine the options contained in a PPP packet looking
  for the option of interest (opt_type).

  If a match is found, the data portion in the configuration-option is
  copied to the supplied buffer (pointed to by opt_val_ptr) in network-byte
  order. The number of bytes copied is returned.
   0                   1
   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |     Type      |    Length     |    Data ...
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

  If the size of supplied buffer (opt_val_len) is less than the bytes to
  copy, only opt_val_len bytes will be copied. The caller should handle this
  appropriately.

  If a match is not found, -1 is returned.
  The PPP packet itself is not modified.

PARAMETERS
  bpp         : Ptr to PPP packet
  opt_type    : Configuration option to look for
                (e.g. LCP Authentication-Protocol, LCP_AP)
  opt_val_ptr : Pointer to buffer where value of the option must be copied.
  opt_val_len : Length of the buffer pointed to by opt_val_ptr. This is
                expected to be atleast as large as the data portion of the
                option being sought (e.g. LCP_AP_LEN).

DEPENDENCIES
  Expects the data_ptr in the item to point to the code portion of a
  PPP packet

   0                   1                   2                   3
   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |     Code      |  Identifier   |            Length             |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |    Data ...
  +-+-+-+-+

RETURN VALUE
    -1               : Failure (bad parametrs, or if opt_type is not found in
                       the packet, or if the option is malformed)
    number of bytes  : Number of bytes actually copied to opt_val_ptr
                       if a match is found.

SIDE EFFECTS
  None
===========================================================================*/
int fsm_find_option
(
  dsm_item_type ** bpp,          /* Ptr to PPP packet                      */
  uint16           opt_type,     /* Option to look for                     */
  void           * opt_val_ptr,  /* Ptr to buffer to copy option value to  */
  uint8            opt_val_len   /* Length of buffer opt_val_ptr points to */
)
{
  struct option_hdr  opt_hdr;        /* option header                      */
  struct config_hdr  cfg_hdr;        /* Config hdr: has Code, ID, len      */
  sint31             pkt_len  = 0;   /* Length of the entire packet        */
  uint16             offset   = 0;   /* offset in the packet chain         */
  uint16             copy_len = 0;   /* Number of bytes copied into buffer */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( bpp  == NULL || *bpp == NULL )
  {
     LOG_MSG_ERROR_0( "NULL arguments passed" );
     return -1;
  }

  /*-------------------------------------------------------------------------
    Extract the code, id and length fields (4 bytes) into cfg_hdr. Convert
    the length from network to host.
  -------------------------------------------------------------------------*/
  offset = 0;
  if( dsm_extract( *bpp,
                   offset,
                   &cfg_hdr,
                   CONFIG_HDR_LEN ) != CONFIG_HDR_LEN )
  {
    LOG_MSG_ERROR_1("dsm_extract() failed, pkt = 0x%x", *bpp);
    return -1;
  }

  pkt_len  = ps_ntohs( cfg_hdr.length );
  pkt_len -= CONFIG_HDR_LEN;
  offset  += CONFIG_HDR_LEN;

  /*-------------------------------------------------------------------------
    Go through the entire PPP packet, one option at a time.
  -------------------------------------------------------------------------*/
  while( pkt_len > 0 )
  {
    /*-----------------------------------------------------------------------
      Get the option header, and update the packet length to reflect that the
      header has been grabbed.
    -----------------------------------------------------------------------*/
    if( dsm_extract( *bpp,
                     offset,
                     &opt_hdr,
                     OPTION_HDR_LEN ) != OPTION_HDR_LEN )
    {
      LOG_MSG_ERROR_1("dsm_extract() failed, pkt = 0x%x", *bpp);
      return -1;
    }

    if( opt_hdr.length < OPTION_HDR_LEN )
    {
      LOG_MSG_ERROR_1( "option length %d < required at least 2 bytes",
                       opt_hdr.length);
      return -1;
    }

    pkt_len -= opt_hdr.length;
    offset  += OPTION_HDR_LEN;

    if( opt_hdr.type == opt_type )
    {
      copy_len = MIN( opt_hdr.length - OPTION_HDR_LEN, opt_val_len );
      if( dsm_extract( *bpp, offset, opt_val_ptr, copy_len ) != copy_len )
      {
        LOG_MSG_ERROR_1("dsm_extract() failed, pkt = 0x%x", *bpp);
        return -1;
      }

      return copy_len;
    } /* if a matching option is found */

    offset += ( opt_hdr.length - OPTION_HDR_LEN );
  } /* while(more options) */

  return -1;

} /* fsm_find_option() */

/*===========================================================================
FUNCTION       FSM_GET_PPP_CB_ARRAY

DESCRIPTION    Access method for ppp_cb_array

PARAMETERS
  None
 
RETURN VALUE   
  ppp_cb_array

DEPENDENCIES   
  None

SIDE EFFECTS   
  None
===========================================================================*/
ppp_type *fsm_get_ppp_cb_array(void)
{
  return ppp_cb_array;
}

/*===========================================================================
FUNCTION FSM_MAKEOPTIONS()

DESCRIPTION
  This function will Build a list of options

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void fsm_makeoptions
(
  ppp_fsm_type    *fsm_ptr,
  void            *v_ptr,
  dsm_item_type  **item_head_ptr,
  negotiate_t      negotiating
)
{
  register uint8 o_type;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  LOG_MSG_INFO2_0( "fsm_makeoptions:" );

  /*-------------------------------------------------------------------------
    send_options_mask is updated with the negotiating values. This is further
    examined to decide the options to be added.
    Reset after adding the option.
  -------------------------------------------------------------------------*/
  fsm_ptr->send_options_mask = negotiating;

  for ( o_type = 0; o_type < fsm_ptr->pdc->n_option_limit; o_type++ )
  {
    if (negotiating & (1 << o_type))
    {
      LOG_MSG_INFO2_1( "Adding option: %d", fsm_ptr->pdc->n_option[o_type] );
      (*fsm_ptr->pdc->option)(fsm_ptr,
                              v_ptr,
                              item_head_ptr,
                              fsm_ptr->pdc->n_option[o_type]);
    }
  }
  fsm_ptr->send_options_mask = 0;
} /* fsm_makeoptions() */



/*===========================================================================
FUNCTION FSM_MAKEREQ()

DESCRIPTION
  This function will Build a request to send to remote host

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static dsm_item_type  *fsm_makereq
(
  ppp_fsm_type *fsm_ptr
)
{
  dsm_item_type  *req_bp = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  LOG_MSG_INFO2_0( "fsm_makereq:" );

  req_bp = dsm_offset_new_buffer(
    DSM_DS_SMALL_ITEM_POOL,
    PPP_MAX_HDR);

  if(req_bp != NULL )
  {
    fsm_makeoptions( fsm_ptr,
                     fsm_ptr->local.work_pdv,
                     &req_bp,
                     fsm_ptr->local.work);
  }
  return(req_bp);

} /* fsm_makereq() */



/*===========================================================================
FUNCTION FSM_CONFIGREQ()

DESCRIPTION
  This function will send a Configure Request

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static int fsm_configreq
(
  ppp_fsm_type *fsm_ptr
)
{
  dsm_item_type  *bp;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( NULL== fsm_ptr->timer_start_fcn )
  {
    DATA_ERR_FATAL("timer_start_fcn is NULL");
    return -1;
  }

  if ( fsm_ptr->retry_request == 0 )
  {
    return -1;
  }

  fsm_ptr->retry_request--;
  (void)(*fsm_ptr->timer_start_fcn)(fsm_ptr->timer,
                       (int64)fsm_ptr->pdc->ack_timeout);

  bp = fsm_makereq(fsm_ptr);
  return( fsm_send(fsm_ptr, CONFIG_REQ, 0, bp) );

} /* fsm_configreq() */



/*===========================================================================
FUNCTION FSM_TERMREQ()

DESCRIPTION
  This function will send a Terminate Request

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int fsm_termreq
(
  ppp_fsm_type *fsm_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( NULL== fsm_ptr->timer_start_fcn )
  {
    DATA_ERR_FATAL("timer_start_fcn is NULL");
    return -1;
  }

  LOG_MSG_INFO3_0 ("Send term_req");
  if ( fsm_ptr->retry_request == 0 )
  {
    return -1;
  }
  fsm_ptr->retry_request--;
  (void)(*fsm_ptr->timer_start_fcn)(fsm_ptr->timer,
                       (int64)fsm_ptr->pdc->term_timeout);
  return( fsm_send(fsm_ptr, TERM_REQ, 0, NULL) );

} /* fsm_termreq() */



/*===========================================================================
FUNCTION FSM_TERMACK()

DESCRIPTION
  This function will send a Terminate Ack

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static int fsm_termack
(
  ppp_fsm_type *fsm_ptr,
  uint8         identifier
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  LOG_MSG_INFO2_2 ("fsm_termack (dev %d, proto 0x%x)",
                   fsm_ptr->device, fsm_ptr->pdc->protocol);
  return( fsm_send(fsm_ptr, TERM_ACK, identifier, NULL) );

} /* fsm_termack() */



/*===========================================================================
FUNCTION FSM_IRC()

DESCRIPTION
  This function will Initialize the Restart Counter

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void fsm_irc_rx_ack
(
  ppp_fsm_type *fsm_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  fsm_ptr->retry_request = fsm_ptr->try_configure;

} /* fsm_irc_rx_ack() */

static void fsm_irc_tx_ack
(
  ppp_fsm_type *fsm_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  fsm_ptr->retry_nak = fsm_ptr->try_nak;

} /* fsm_irc_tx_ack() */

void fsm_irc
(
  ppp_fsm_type *fsm_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  fsm_irc_tx_ack(fsm_ptr);
  fsm_irc_rx_ack(fsm_ptr);

} /* fsm_irc() */

/*===========================================================================
FUNCTION FSM_ZRC()

DESCRIPTION
  This function will restart the Zero counter

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void fsm_zrc
(
  ppp_fsm_type *fsm_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( NULL == fsm_ptr->timer_start_fcn )
  {
    DATA_ERR_FATAL("timer_start_fcn is NULL");
    return;
  }
  fsm_ptr->retry_request = 0;
  (void)(*fsm_ptr->timer_start_fcn)(fsm_ptr->timer,
                       (int64)fsm_ptr->pdc->ack_timeout);

} /* fsm_zrc() */



/*===========================================================================
FUNCTION FSM_OPENING()

DESCRIPTION
  This function will Configuration negotiation complete

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void fsm_opening
(
  ppp_fsm_type *fsm_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( NULL == fsm_ptr->timer_cancel_fcn )
  {
    DATA_ERR_FATAL("timer_cancel_fcn is NULL");
    return;
  }
  (void)(*fsm_ptr->timer_cancel_fcn)(fsm_ptr->timer);

  fsm_ptr->state = fsmOPENED;
  (*fsm_ptr->pdc->opening)(fsm_ptr);

} /* fsm_opening() */



/*===========================================================================
FUNCTION FSM_REQUEST()

DESCRIPTION
  This function will Check configuration options requested by the remote host

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static int fsm_request
(
  ppp_fsm_type      *fsm_ptr,
  struct config_hdr *config,
  dsm_item_type     *bp
)
{
  struct option_hdr option;                       /* option header storage */
  dsm_item_type    *reply_bp = NULL;              /* reply packet          */
  dsm_item_type    *option_bp = NULL;             /* option copy           */
  sint31            signed_length = config->length;
  int               option_result ;               /* option reply          */
  int               remaining;
  negotiate_t       desired;                      /* desired to negotiate  */
  uint8             reply_result = CONFIG_ACK;    /* reply to request      */
  int               option_index;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifdef FEATURE_DATA_PS_EHRPD
  /*-------------------------------------------------------------------------
    Call appropriate handler if this is the 3GPP2-VSNCP FSM
  -------------------------------------------------------------------------*/
  if( VSNCP_3GPP2I_FSM_IS_VALID( fsm_ptr ) )
  {
    return vsncp_3gpp2_request( fsm_ptr, config, bp );
  }
#endif /* FEATURE_DATA_PS_EHRPD */

  /*-------------------------------------------------------------------------
    Set remote.work to 0 (clear all options), we start afresh for this C-Req
  -------------------------------------------------------------------------*/
  fsm_ptr->remote.work = 0;

  reply_bp = dsm_offset_new_buffer(
    DSM_DS_SMALL_ITEM_POOL,
    PPP_MAX_HDR);
  if(reply_bp == NULL )
  {
    LOG_MSG_ERROR_0 ("no dsm item for reply!");
    dsm_free_packet(&bp);
    return -1;
  }

  LOG_MSG_INFO2_3("fsm_request(device %d), proto 0x%x, retry_nak = %d",
          fsm_ptr->device,
          fsm_ptr->pdc->protocol,
          fsm_ptr->retry_nak);
  /* Process options requested by remote host */
  while (signed_length > 0  &&  ntohopt(&option, &bp) != -1)
  {
    LOG_MSG_INFO2_1( "Remote requests option: %d", option.type );
    if ( (signed_length -= option.length) < 0 )
    {
      LOG_MSG_ERROR_0("fsm_request(): option length greater than packet length");
      dsm_free_packet(&bp);
      dsm_free_packet(&reply_bp);
      return -1;
    }

    /* save rest of option for later */
    if ( (remaining = option.length - OPTION_HDR_LEN) > 0 )
    {
      (void)dsm_dup_packet( &option_bp, bp, 0, (uint16)remaining );
      if ( option_bp == NULL )
      {
        LOG_MSG_ERROR_0("fsm_request(): dsm_dup_packet failed");
        dsm_free_packet(&bp);
        dsm_free_packet(&reply_bp);
        return -1;
      }
    }

    if ( (option_result = (*fsm_ptr->pdc->check)( fsm_ptr, &option,
        &bp, TRUE )) < 0 )
    {
      LOG_MSG_ERROR_0("fsm_request(): option check failed");
      dsm_free_packet(&bp);
      dsm_free_packet(&reply_bp);
      dsm_free_packet(&option_bp);
      return -1;
    }

    if( option_result == CONFIG_NAK &&
        fsm_ptr->retry_nak == 0 )
    {
      option_result = CONFIG_REJ;
    }

    if (-1 == (option_index = fsm_n_option(fsm_ptr->pdc, option.type)))
    {
      LOG_MSG_INFO2_3( "fsm_request(): dev %d, fsm %d, unrecognized option %d",
               fsm_ptr->device, fsm_ptr->pdc->fsmi, option.type);
      option_result = CONFIG_REJ;
    }

    if( option_result < reply_result )
    {
      fsm_ptr->remote.work |= (1 << option_index);
      dsm_free_packet( &option_bp );
      continue;
    }

    if ( option_result > reply_result )
    {
      /* Discard current list of replies */
      reply_bp->used = 0;
      reply_result = (uint8)option_result;
    }

    switch ( option_result )
    {
    case CONFIG_ACK:
      fsm_ptr->remote.work |= (1 << option_index);
      htonopt_copy( &reply_bp, &option, &option_bp );
      break;

    case CONFIG_NAK:
      fsm_ptr->remote.work |= (1 << option_index);
      /*-----------------------------------------------------------------------
        send_options_mask is updated with the negotiating values. This is
        further examined to decide the options to be added.
        Reset after adding the option.
      -----------------------------------------------------------------------*/
       fsm_ptr->send_options_mask =  fsm_ptr->remote.work;
      (*fsm_ptr->pdc->option)(fsm_ptr,
                              fsm_ptr->remote.work_pdv,
                              &reply_bp,
                              option.type );
      fsm_ptr->send_options_mask =  0;
      break;

    case CONFIG_REJ:
      htonopt_copy( &reply_bp, &option, &option_bp );
      break;
    default:
      break;
    };
    dsm_free_packet( &option_bp );
  }

  /* Now check for any missing options which are desired */
  if ( fsm_ptr->retry_nak > 0 &&
       (desired = fsm_ptr->remote.want & ~fsm_ptr->remote.work) != 0 )
  {
    switch ( reply_result )
    {
    case CONFIG_ACK:
      reply_bp->used = 0;
      reply_result = CONFIG_NAK;
      /* fallthru */

    case CONFIG_NAK:
      fsm_makeoptions( fsm_ptr, fsm_ptr->remote.want_pdv,
          &reply_bp, desired );
      fsm_ptr->retry_nak--;
      break;

    case CONFIG_REJ:
      /* do nothing */
      break;
    default:
      break;
    };
  }
  else if ( reply_result == CONFIG_NAK )
  {
    /* if too many NAKs, reject instead */
    if ( fsm_ptr->retry_nak > 0 )
    {
      fsm_ptr->retry_nak--;
    }
    else
    {
      reply_result = CONFIG_REJ;
    }
  }

  if (reply_result == CONFIG_ACK)
  {
    fsm_irc_tx_ack(fsm_ptr);
  }

  /* Send ACK/NAK/REJ to remote host */
  (void)fsm_send(fsm_ptr, reply_result, config->identifier, reply_bp);
  dsm_free_packet(&bp);
  return (reply_result != CONFIG_ACK);

} /* fsm_request() */



/*===========================================================================
FUNCTION FSM_ACK()

DESCRIPTION
  This function will Process configuration ACK sent by remote host - this is
  done by comparing the last request with this ACK.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static int fsm_ack
(
  ppp_fsm_type      *fsm_ptr,                  /* pointer to the fsm block */
  struct config_hdr *config_hdr,     /* pointer to the header for this ACK */
  dsm_item_type     *item_ptr    /* pointer to the dsm_item containing ACK */
)
{
  dsm_item_type *request_item_ptr; /* item pointer for the original request*/
  int ret_val = 0;                              /* was an error generated? */
  sint15 req_char;    /* character from request for comparison to ack_char */
  sint15 ack_char;  /* character from the ack for comparison with req_char */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifdef FEATURE_DATA_PS_EHRPD
  /*-------------------------------------------------------------------------
    Call appropriate handler if this is the 3GPP2-VSNCP FSM
  -------------------------------------------------------------------------*/
  if( VSNCP_3GPP2I_FSM_IS_VALID( fsm_ptr ) )
  {
    return vsncp_3gpp2_ack( fsm_ptr, config_hdr, item_ptr );
  }
#endif /* FEATURE_DATA_PS_EHRPD */

  /*-------------------------------------------------------------------------
    ID field must match last request we sent
  -------------------------------------------------------------------------*/
  if (config_hdr->identifier != fsm_ptr->lastid)
  {
    dsm_free_packet(&item_ptr);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Get a copy of last request we sent
  -------------------------------------------------------------------------*/
  request_item_ptr = fsm_makereq(fsm_ptr);

  /*-------------------------------------------------------------------------
    Overall buffer length should match
  -------------------------------------------------------------------------*/
  if (config_hdr->length != dsm_length_packet(request_item_ptr))
  {
    ret_val = -1;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Make sure Each byte matches, but first dup one of the packets so that
      there is something to compare.
    -----------------------------------------------------------------------*/
    while((req_char = dsm_pull8(&request_item_ptr)) != -1)
    {
      if ((ack_char = dsm_pull8(&item_ptr)) == -1 || ack_char != req_char )
      {
        ret_val = -1;;
        break;
      }
    }
  }

  dsm_free_packet(&request_item_ptr);
  dsm_free_packet(&item_ptr);

  return ret_val;

} /* fsm_ack() */



/*===========================================================================
FUNCTION FSM_NAK()

DESCRIPTION
  This function will Process configuration NAK sent by remote host

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static int fsm_nak
(
  ppp_fsm_type      *fsm_ptr,
  struct config_hdr *config,
  dsm_item_type     *bp
)
{
  sint31 signed_length = config->length;
  struct option_hdr option;
  int last_option = 0;
  int result;
  int option_index;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* ID field must match last request we sent */
  if (config->identifier != fsm_ptr->lastid)
  {
    dsm_free_packet(&bp);
    return -1;
  }

  /* First, process in order.  Then, process extra "important" options */
  while (signed_length > 0  &&  ntohopt(&option, &bp) != -1)
  {
    if ((signed_length -= option.length) < 0)
    {
      dsm_free_packet(&bp);
      return -1;
    }
    if (-1 == (option_index = fsm_n_option(fsm_ptr->pdc, option.type)))
    {
      LOG_MSG_INFO2_3( "fsm_nak(): dev %d, fsm %d, unrecognized option %d",
               fsm_ptr->device, fsm_ptr->pdc->fsmi, option.type);
      continue;
    }

    if ( option.type > fsm_ptr->pdc->option_limit )
    {
      /* option is allowed, but ignored */
      continue;
    }
    else if ( !(fsm_ptr->local.work & (1 << option_index)))
    {
      if ( fsm_ptr->local.will & (1 << option_index) )
      {
        fsm_ptr->local.work |= (1 << option_index);
      }
      last_option = fsm_ptr->pdc->option_limit + 1;
    }
    else if ( option.type < last_option )
    {
      /* assumes options are always in numerical order */
      dsm_free_packet(&bp);
      return -1;
    }
    else
    {
      last_option = option.type;
    }
    if ( (result =
          (*fsm_ptr->pdc->check)( fsm_ptr, &option,
        &bp, FALSE )) < 0 )
    {
      dsm_free_packet(&bp);
      return -1;
    }
    /* update the negotiation status */
    if ( result == CONFIG_REJ )
    {
      fsm_ptr->local.work &= ~(unsigned int)(1 << option_index);
    }
  }
  dsm_free_packet(&bp);
  return 0;

} /* fsm_nak() */



/*===========================================================================
FUNCTION FSM_REJECT()

DESCRIPTION
  This function will Process configuration Reject sent by remote host

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static int fsm_reject
(
  ppp_fsm_type      *fsm_ptr,
  struct config_hdr *config,
  dsm_item_type     *bp
)
{
  sint31 signed_length = config->length;
  struct option_hdr option;
  int option_index;
  int k;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifdef FEATURE_DATA_PS_EHRPD
  /*-------------------------------------------------------------------------
    Call appropriate handler if this is the 3GPP2-VSNCP FSM
  -------------------------------------------------------------------------*/
  if( VSNCP_3GPP2I_FSM_IS_VALID( fsm_ptr ) )
  {
    return vsncp_3gpp2_reject( fsm_ptr, config, bp );
  }
#endif /* FEATURE_DATA_PS_EHRPD */

  /* ID field must match last request we sent */
  if (config->identifier != fsm_ptr->lastid)
  {
    dsm_free_packet(&bp);
    return -1;
  }

  /* Process in order, checking for errors */
  while (signed_length > 0  &&  ntohopt(&option, &bp) != -1)
  {
    if ((signed_length -= option.length) < 0)
    {
      dsm_free_packet(&bp);
      return -1;
    }

    if (-1 == (option_index = fsm_n_option(fsm_ptr->pdc, option.type)))
    {
      LOG_MSG_INFO2_3( "fsm_reject(): dev %d, fsm %d, unrecognized option %d",
               fsm_ptr->device, fsm_ptr->pdc->fsmi, option.type);
      continue;
    }

    if ( option.type > fsm_ptr->pdc->option_limit )
    {
      dsm_free_packet(&bp);
      return -1;
    }
    else if ( !(fsm_ptr->local.work & (1 << option_index)))
    {
      dsm_free_packet(&bp);
      return -1;
    }

    /* currently, contents of options are not checked.
     * just check that the data is all there.
     */
    if ( (*fsm_ptr->pdc->reject)(fsm_ptr, &option, bp) != 0 )
    {
      dsm_free_packet(&bp);
      return -1;
    }

    k = option.length - OPTION_HDR_LEN;
    if ( dsm_pullup( &bp, NULL, (uint16)k ) != k )
    {
      return -1;
    }
  }

  dsm_free_packet(&bp);
  return 0;

} /* fsm_reject() */


/*===========================================================================
FUNCTION FSM_CODE_REJECT()

DESCRIPTION
  This function will process a Code-Reject packet received from the peer.
  In general, all rejects are treated as benign.
  If the VENDOR_SPECIFIC code is being rejected, allow_vendor_pkt is set
  to FALSE to indicate that vendor-specific packets should not be sent.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void fsm_code_reject
(
  ppp_fsm_type  * fsm_ptr,
  dsm_item_type * bp
)
{
  uint8  rejected_code;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch(fsm_ptr->state)
  {
    case fsmACK_Rcvd:
      fsm_ptr->state = fsmREQ_Sent;
      break;

    default:
      break;
  }; /* switch( fsm_ptr->state ) */

  if( dsm_extract( bp, 0, &rejected_code, sizeof(uint8) ) != sizeof(uint8) )
  {
    LOG_MSG_ERROR_0("Unable to extract code field from rejected packet");
    dsm_free_packet( &bp );
    return;
  }

  switch( rejected_code )
  {
    case VENDOR_SPECIFIC:
      fsm_ptr->allow_vendor_pkt = FALSE;
      break;

    default:
      break;
  } /* switch( rejected_code ) */

  dsm_free_packet( &bp );
} /* fsm_code_reject() */


/*===========================================================================
FUNCTION FSM_PROC()

DESCRIPTION
  This function will process the incoming PPP packet

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void fsm_proc
(
  ppp_fsm_type  *fsm_ptr,
  dsm_item_type *bp
)
{
  struct config_hdr    hdr;
  int                  rejected;
  ppp_type            *ppp_cb_ptr = NULL;
  ppp_fsm_type        *tmp_fsm_ptr = NULL;
  ppp_protocol_e_type  protocol = PPP_PROTOCOL_MIN;
#ifdef FEATURE_DATA_PS_EHRPD
  uint8                buf[PPP_OUI_LEN];
#endif /* FEATURE_DATA_PS_EHRPD */
#ifdef FEATURE_STA_PPP
    sta_ppp_type        *sta_cb_ptr = NULL;
#endif /* FEATURE_STA_PPP*/
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( bp == NULL )
  {
    LOG_MSG_ERROR_0("Invalid dsm in fsm_proc");
    return;
  }

  if ( fsm_ptr == NULL      ||
       fsm_ptr->pdc == NULL ||
       fsm_ptr->device >= PPP_MAX_DEV )
  {
    LOG_MSG_ERROR_0("Invalid args in fsm_proc" );
    dsm_free_packet( &bp);
    return;
  }

  if(ntohcnf(&hdr, &bp) == -1)
  {
    LOG_MSG_ERROR_3("ppp%d Short pkt st=%d proto=0x%x",
        fsm_ptr->device,
        fsm_ptr->state,
        fsm_ptr->pdc->protocol);
    dsm_free_packet( &bp);
    return;
  }

  hdr.length -= CONFIG_HDR_LEN;    /* Length includes envelope */

  /*------------------------------------------------------------------------
    When a packet is received with an invalid Length field, the packet is
    silently discarded without affecting the automaton
  ------------------------------------------------------------------------*/
  if (hdr.length > dsm_length_packet(bp))
  {
    dsm_free_packet( &bp);
    return;
  }

#ifdef FEATURE_DATA_PS_EHRPD
  if( VSNCP_3GPP2I_FSM_IS_VALID( fsm_ptr ) )
  {
    if ( dsm_pullup( &bp, buf, PPP_OUI_LEN ) < PPP_OUI_LEN )
    {
      LOG_MSG_ERROR_0("Unable to remove OUI!");
      dsm_free_packet( &bp);
      return;
    }
    hdr.length -= PPP_OUI_LEN;
  }
#endif /* FEATURE_DATA_PS_EHRPD */

  dsm_trim_packet(&bp, hdr.length);        /* Trim off padding */

  ppp_cb_ptr = ppp_cb_array + (uint8)fsm_ptr->device;
#ifdef FEATURE_STA_PPP
  if(sta_ppp_enable)
  {
    sta_cb_ptr = sta_ppp_cb_array + fsm_ptr->device;
  }
#endif/* FEATURE_STA_PPP */

#ifdef MSG_4
  MSG_4(MSG_SSID_DS,
        MSG_LEGACY_MED,
        "fsm_proc(dev %d, protocol 0x%x, state %d) with msg %d",
        fsm_ptr->device,
        fsm_ptr->pdc->protocol,
        fsm_ptr->state,
        hdr.code);
#else
  LOG_MSG_INFO2_3("fsm_proc(dev %d state %d) for msg %d",
          fsm_ptr->device,
          fsm_ptr->state,
          hdr.code);
#endif
  PPP_INC_INSTANCE_FSM_STATS(fsm_ptr,pkts_rx,1);
  switch(hdr.code)
  {
  case VENDOR_SPECIFIC:
    LOG_MSG_INFO2_2("PPP vendor-specific packet received, dev %d, proto 0x%x",
                    fsm_ptr->device, fsm_ptr->pdc->protocol);
    switch( fsm_ptr->state )
    {
      case fsmOPENED:
      case fsmINITIAL:
      case fsmSTARTING:
      case fsmCLOSED:
      case fsmSTOPPED:
      case fsmCLOSING:
      case fsmSTOPPING:
      case fsmREQ_Sent:
      case fsmACK_Rcvd:
      case fsmACK_Sent:
        LOG_MSG_INFO2_1("Invoking vendor extension handler, dev %d",
                        fsm_ptr->device);
        (*fsm_ptr->cbdc->vendor_ext_hdlr_fcn)( fsm_ptr->device,
                              fsm_ptr->pdc->protocol,
                              &hdr,
                              bp );
        break;
      default:
        /* unexpected */
        dsm_free_packet(&bp);
        break;
    }; /* switch( fsm_ptr->state ) */
    break;

  case CONFIG_REQ:
    PPP_INC_INSTANCE_FSM_STATS(fsm_ptr,config_requests_rx,1);
    switch(fsm_ptr->state)
    {
    case fsmOPENED:
      LOG_MSG_INFO2_2 ("PPP resync from network, dev %d, proto 0x%x",
                       fsm_ptr->device, fsm_ptr->pdc->protocol);
      /*--------------------------------------------------------------------
        The mobile received a LCP/IPCP C_REQ in the OPENED state. This
        indicates a PPP resync - if there is a callback registered call it.
      --------------------------------------------------------------------*/
      (*fsm_ptr->cbdc->call_event_callback_fcn) (fsm_ptr,
                                                 fsm_ptr->pdc->protocol,
                                                 PPP_RESYNC_EV,
                                                 PS_PPP_EVENT_OPEN,
                                                 PS_PPP_EVENT_RESYNC
                                                );

      (*fsm_ptr->pdc->closing)(fsm_ptr);
      (*fsm_ptr->pdc->starting)(fsm_ptr);
      fsm_init(fsm_ptr);
      /*---------------------------------------------------------------------
        We are resync'ing so if the timer is valid and the timeout is set,
        start the setup timer.  Start the guard timer only for LCP resync
      ---------------------------------------------------------------------*/
#ifdef FEATURE_STA_PPP
      if(!sta_ppp_enable)
      {

        if(PPP_LCP_PROTOCOL == fsm_ptr->pdc->protocol &&
           ppp_cb_ptr->setup_timer != PS_TIMER_INVALID_HANDLE &&
           ppp_cb_ptr->setup_timeout > 0)
        {
          if( NULL == fsm_ptr->timer_start_fcn )
          {
            LOG_MSG_ERROR_0("timer_start_fcn is NULL");
            ASSERT(0);
            return ;
          }
          (void)(*fsm_ptr->timer_start_fcn)(ppp_cb_ptr->setup_timer,
                                            (int64)ppp_cb_ptr->setup_timeout);
        }
      }
#else
      if(PPP_LCP_PROTOCOL == fsm_ptr->pdc->protocol &&
         ppp_cb_ptr->setup_timer != PS_TIMER_INVALID_HANDLE &&
         ppp_cb_ptr->setup_timeout > 0)
      {
        if( NULL == fsm_ptr->timer_start_fcn )
        {
          DATA_ERR_FATAL("timer_start_fcn is NULL");
          return ;
        }
        (void)(*fsm_ptr->timer_start_fcn)(ppp_cb_ptr->setup_timer,
                             (int64)ppp_cb_ptr->setup_timeout);
      }
#endif /* FEATURE_STA_PPP */

      (void)fsm_configreq(fsm_ptr);
      fsm_ptr->state =
        ((fsm_request(fsm_ptr, &hdr, bp) == 0) ? fsmACK_Sent : fsmREQ_Sent);
      /* STARTING event */
      (*fsm_ptr->cbdc->call_event_callback_fcn) (fsm_ptr,
                                                 fsm_ptr->pdc->protocol,
                                                 PPP_STARTING_EV,
                                                 PS_PPP_EVENT_UNKNOWN,
                                                 PS_PPP_EVENT_UNKNOWN
                                                );

      break;

    case fsmSTARTING:  /* can't happen? */
    case fsmSTOPPED:
      LOG_MSG_INFO2_0("Processing req in starting/stopped state");
      fsm_irc(fsm_ptr);
      (*fsm_ptr->pdc->starting)(fsm_ptr);
      (void)fsm_configreq(fsm_ptr);
      /* STARTING event */
      (*fsm_ptr->cbdc->call_event_callback_fcn) (fsm_ptr,
                                                 fsm_ptr->pdc->protocol,
                                                 PPP_STARTING_EV,
                                                 PS_PPP_EVENT_UNKNOWN,
                                                 PS_PPP_EVENT_UNKNOWN
                                                );      /* fallthru */


      fsm_ptr->state =
      (fsm_request(fsm_ptr, &hdr, bp) == 0)
        ? fsmACK_Sent : fsmREQ_Sent;
      LOG_MSG_INFO2_1("Sent request in state %d", fsm_ptr->state);
      break;

    case fsmREQ_Sent:
    case fsmACK_Sent:
      fsm_ptr->state =
      (fsm_request(fsm_ptr, &hdr, bp) == 0)
        ? fsmACK_Sent : fsmREQ_Sent;
      LOG_MSG_INFO2_1("Sent request in state %d", fsm_ptr->state);
      break;

    case fsmACK_Rcvd:
      if (fsm_request(fsm_ptr, &hdr, bp) == 0)
      {
        fsm_opening(fsm_ptr);
      }
      break;

    case fsmINITIAL:  /* can't happen? */
    case fsmCLOSED:
      /* Don't accept any connections */
      (void)fsm_termack(fsm_ptr, hdr.identifier);
      /* fallthru */

    case fsmCLOSING:
    case fsmSTOPPING:
      /* We are attempting to close connection; */
      /* wait for timeout to resend a Terminate Request */
      dsm_free_packet(&bp);
      break;
    default:
      break;
    };
    break;

  case CONFIG_ACK:
    PPP_INC_INSTANCE_FSM_STATS(fsm_ptr,config_acks_rx,1);
    switch(fsm_ptr->state)
    {
    case fsmREQ_Sent:
      if (fsm_ack(fsm_ptr, &hdr, bp) == 0)
      {
        fsm_irc_rx_ack(fsm_ptr);
        fsm_ptr->state = fsmACK_Rcvd;
      }
      break;

    case fsmACK_Sent:
      if (fsm_ack(fsm_ptr, &hdr, bp) == 0)
      {
        fsm_irc_rx_ack(fsm_ptr);
        fsm_opening(fsm_ptr);
      }
      break;

    case fsmOPENED:
      /*--------------------------------------------------------------------
        The mobile received a LCP/IPCP C_ACK in the OPENED state. This
        indicates a PPP resync - if there is a callback registered call it.
      --------------------------------------------------------------------*/
      (*fsm_ptr->cbdc->call_event_callback_fcn) (fsm_ptr,
                                                 fsm_ptr->pdc->protocol,
                                                 PPP_RESYNC_EV,
				                 PS_PPP_EVENT_OPEN,
                                                 PS_PPP_EVENT_RESYNC
                                                );

      (*fsm_ptr->pdc->closing)(fsm_ptr);
      (*fsm_ptr->pdc->starting)(fsm_ptr);
      dsm_free_packet(&bp);
      (void)fsm_configreq(fsm_ptr);
      fsm_ptr->state = fsmREQ_Sent;
      /* STARTING event */
      (*fsm_ptr->cbdc->call_event_callback_fcn) (fsm_ptr,
                                                 fsm_ptr->pdc->protocol,
                                                 PPP_STARTING_EV,
                                                 PS_PPP_EVENT_UNKNOWN,
                                                 PS_PPP_EVENT_UNKNOWN
                                                );
      break;

    case fsmACK_Rcvd:  /* Unexpected event? */
      dsm_free_packet(&bp);
      (void)fsm_configreq(fsm_ptr);
      fsm_ptr->state = fsmREQ_Sent;
      break;

    case fsmINITIAL:  /* can't happen? */
    case fsmSTARTING:  /* can't happen? */
    case fsmCLOSED:
    case fsmSTOPPED:
      /* Out of Sync */
      (void)fsm_termack(fsm_ptr, hdr.identifier);
      /* fallthru */

    case fsmCLOSING:
    case fsmSTOPPING:
      /* We are attempting to close connection; */
      /* wait for timeout to resend a Terminate Request */
      dsm_free_packet(&bp);
      break;
    default:
      break;
    };
    break;

  case CONFIG_NAK:
    PPP_INC_INSTANCE_FSM_STATS(fsm_ptr,config_naks_rx,1);
#ifdef FEATURE_DATA_PS_EHRPD
    /* send code-rej for c-nak if this is 3gpp2 vsncp */
    if( VSNCP_3GPP2I_FSM_IS_VALID( fsm_ptr ) )
    {
      hdr.length += CONFIG_HDR_LEN;  /* restore length */
      bp = htoncnf( &hdr, bp );  /* put header back on */
      if (bp != NULL)
      {
        (void)fsm_send( fsm_ptr, CODE_REJ, hdr.identifier, bp );
      }
      break;
    }
#endif /* FEATURE_DATA_PS_EHRPD */
    switch(fsm_ptr->state)
    {
    case fsmREQ_Sent:
    case fsmACK_Sent:
      /* Update our request to reflect NAKed options */
      if ( fsm_nak(fsm_ptr, &hdr, bp) == 0 )
      {
        fsm_irc_rx_ack(fsm_ptr);
        /* Send updated config request */
        (void)fsm_configreq(fsm_ptr);
        /* stay in same state */
      }
      break;

    case fsmOPENED:   /* Unexpected event? */
      /*--------------------------------------------------------------------
        Inform this layer and any upper layers of the resync
      --------------------------------------------------------------------*/
      (*fsm_ptr->cbdc->call_event_callback_fcn) (fsm_ptr,
                                                 fsm_ptr->pdc->protocol,
                                                 PPP_RESYNC_EV,
				                 PS_PPP_EVENT_OPEN,
                                                 PS_PPP_EVENT_RESYNC
                                                );

      (*fsm_ptr->pdc->closing)(fsm_ptr);
      (*fsm_ptr->pdc->starting)(fsm_ptr);
      dsm_free_packet(&bp);
      (void)fsm_configreq(fsm_ptr);
      fsm_ptr->state = fsmREQ_Sent;
      /* STARTING event */
      (*fsm_ptr->cbdc->call_event_callback_fcn) (fsm_ptr,
                                                 fsm_ptr->pdc->protocol,
                                                 PPP_STARTING_EV,
                                                 PS_PPP_EVENT_UNKNOWN,
                                                 PS_PPP_EVENT_UNKNOWN
                                                );
      break;

    case fsmACK_Rcvd:  /* Unexpected event? */
      dsm_free_packet(&bp);
      (void)fsm_configreq(fsm_ptr);
      fsm_ptr->state = fsmREQ_Sent;
      break;

    case fsmINITIAL:  /* can't happen? */
    case fsmSTARTING:  /* can't happen? */
    case fsmCLOSED:
    case fsmSTOPPED:
      /* Out of Sync */
      (void)fsm_termack(fsm_ptr, hdr.identifier);
      /* fallthru */

    case fsmCLOSING:
    case fsmSTOPPING:
      /* We are attempting to close connection; */
      /* wait for timeout to resend a Terminate Request */
      dsm_free_packet(&bp);
      break;
    default:
      break;
    };
    break;

  case CONFIG_REJ:
    PPP_INC_INSTANCE_FSM_STATS(fsm_ptr,config_rejects_rx,1);
    switch(fsm_ptr->state)
    {
    case fsmREQ_Sent:
    case fsmACK_Sent:
      /* Update our request to reflect REJECTed options */
      if ( fsm_reject(fsm_ptr, &hdr, bp) == 0 )
      {
        fsm_irc_rx_ack(fsm_ptr);
        /* Send updated config request */
        (void)fsm_configreq(fsm_ptr);
        /* stay in same state */
      }
      break;

    case fsmOPENED:   /* Unexpected event? */
      /*--------------------------------------------------------------------
        Inform this layer and any upper layers of the resync
      --------------------------------------------------------------------*/
      (*fsm_ptr->cbdc->call_event_callback_fcn) (fsm_ptr,
                                                 fsm_ptr->pdc->protocol,
                                                 PPP_RESYNC_EV,
				                 PS_PPP_EVENT_OPEN,
                                                 PS_PPP_EVENT_RESYNC
                                                );

      (*fsm_ptr->pdc->closing)(fsm_ptr);
      (*fsm_ptr->pdc->starting)(fsm_ptr);
      dsm_free_packet(&bp);
      (void)fsm_configreq(fsm_ptr);
      fsm_ptr->state = fsmREQ_Sent;
      /* STARTING event */
      (*fsm_ptr->cbdc->call_event_callback_fcn) (fsm_ptr,
                                                 fsm_ptr->pdc->protocol,
                                                 PPP_STARTING_EV,
                                                 PS_PPP_EVENT_UNKNOWN,
                                                 PS_PPP_EVENT_UNKNOWN
                                                );
      break;

    case fsmACK_Rcvd:  /* Unexpected event? */
      dsm_free_packet(&bp);
      (void)fsm_configreq(fsm_ptr);
      fsm_ptr->state = fsmREQ_Sent;
      break;

    case fsmINITIAL:  /* can't happen? */
    case fsmSTARTING:  /* can't happen? */
    case fsmCLOSED:
    case fsmSTOPPED:
      /* Out of Sync */
      (void)fsm_termack(fsm_ptr, hdr.identifier);
      /* fallthru */

    case fsmCLOSING:
    case fsmSTOPPING:
      /* We are attempting to close connection; */
      /* wait for timeout to resend a Terminate Request */
      dsm_free_packet(&bp);
      break;
    default:
      break;
    };
    break;

  case TERM_REQ:
    PPP_INC_INSTANCE_FSM_STATS(fsm_ptr,term_requests_rx,1);
    switch(fsm_ptr->state)
    {
    case fsmOPENED:
      if (PPP_LCP_PROTOCOL == fsm_ptr->pdc->protocol &&
          pppAUTHENTICATE == ppp_cb_ptr->phase)
      {
        fsm_ptr->fail_reason =  PPP_FAIL_REASON_AUTH_FAILURE;
      }
#ifdef FEATURE_DATA_PS_EHRPD
      else if( PPP_VSNCP_PROTOCOL == fsm_ptr->pdc->protocol )
      {
        vsncp_3gpp2_term_req(fsm_ptr, &hdr, &bp); 
      }
#endif /*FEATURE_DATA_PS_EHRPD*/
#ifdef FEATURE_STA_PPP
      if(sta_ppp_enable)
      {
        if (PPP_LCP_PROTOCOL == fsm_ptr->pdc->protocol &&
            pppAUTHENTICATE == sta_cb_ptr->phase)
        {
          fsm_ptr->fail_reason =  PPP_FAIL_REASON_AUTH_FAILURE;
        }
      }
#endif/* FEATURE_STA_PPP */
      (*fsm_ptr->pdc->closing)(fsm_ptr);
      fsm_zrc(fsm_ptr);
      (void)fsm_termack(fsm_ptr, hdr.identifier);
      fsm_ptr->state = fsmSTOPPING;
      break;

    case fsmACK_Rcvd:
    case fsmACK_Sent:
      fsm_ptr->state = fsmREQ_Sent;
      /* fallthru */

    case fsmREQ_Sent:
    case fsmCLOSING:
    case fsmSTOPPING:
      /* waiting for timeout */
      /* fallthru */

    case fsmINITIAL:  /* can't happen? */
    case fsmSTARTING:  /* can't happen? */
    case fsmCLOSED:
    case fsmSTOPPED:
      /* Unexpected, but make them happy */
      (void)fsm_termack(fsm_ptr, hdr.identifier);
      break;
    default:
      break;
    };
    dsm_free_packet(&bp);
    break;

  case TERM_ACK:
    PPP_INC_INSTANCE_FSM_STATS(fsm_ptr,term_acks_rx,1);
    switch(fsm_ptr->state)
    {
    case fsmCLOSING:
      if( NULL == fsm_ptr->timer_cancel_fcn )
      {
        DATA_ERR_FATAL("timer_cancel_fcn is NULL");
        return;
      }
      (void)(*fsm_ptr->timer_cancel_fcn)( fsm_ptr->timer );
      (*fsm_ptr->pdc->stopping)(fsm_ptr);
      fsm_ptr->state = fsmCLOSED;
      break;

    case fsmSTOPPING:
      if( NULL == fsm_ptr->timer_cancel_fcn )
      {
        DATA_ERR_FATAL("timer_cancel_fcn is NULL");
        return;
      }
      (void)(*fsm_ptr->timer_cancel_fcn)( fsm_ptr->timer);
      (*fsm_ptr->pdc->stopping)(fsm_ptr);
      fsm_ptr->state = fsmSTOPPED;
      break;

    case fsmOPENED:
      /* Remote host has abruptly closed connection */
      /*--------------------------------------------------------------------
        Inform this layer and any upper layers of the resync
      --------------------------------------------------------------------*/
      (*fsm_ptr->cbdc->call_event_callback_fcn) (fsm_ptr,
                                                 fsm_ptr->pdc->protocol,
                                                 PPP_RESYNC_EV,
				                 PS_PPP_EVENT_OPEN,
                                                 PS_PPP_EVENT_RESYNC
                                                );

      (*fsm_ptr->pdc->closing)(fsm_ptr);
      (*fsm_ptr->pdc->starting)(fsm_ptr);
      (void)fsm_configreq(fsm_ptr);
      fsm_ptr->state = fsmREQ_Sent;
      /* STARTING event */
      (*fsm_ptr->cbdc->call_event_callback_fcn) (fsm_ptr,
                                                 fsm_ptr->pdc->protocol,
                                                 PPP_STARTING_EV,
                                                 PS_PPP_EVENT_UNKNOWN,
                                                 PS_PPP_EVENT_UNKNOWN
                                                );
      break;

    case fsmACK_Rcvd:
      fsm_ptr->state = fsmREQ_Sent;
      /* fallthru */

    case fsmREQ_Sent:
    case fsmACK_Sent:
      /* waiting for timeout */
      /* fallthru */

    case fsmINITIAL:  /* can't happen? */
    case fsmSTARTING:  /* can't happen? */
    case fsmCLOSED:
    case fsmSTOPPED:
      /* Unexpected, but no action needed */
      break;
    default:
      break;
    };
    dsm_free_packet(&bp);
    break;

  case CODE_REJ:
    PPP_INC_INSTANCE_FSM_STATS(fsm_ptr,code_rejects_rx,1);
    fsm_code_reject( fsm_ptr, bp );
    break;

  case PROT_REJ:
    PPP_INC_INSTANCE_FSM_STATS(fsm_ptr,proto_rejects_rx,1);
    /* In general, treat protocol rejects as catastrophic
     */
    if( PPP_LCP_PROTOCOL != fsm_ptr->pdc->protocol )
    {
      hdr.length += CONFIG_HDR_LEN;  /* restore length */
      bp = htoncnf( &hdr, bp );  /* put header back on */
      if (bp != NULL)
      {
        (void)fsm_send( fsm_ptr, CODE_REJ, hdr.identifier, bp );
      }
      break;
    }

    /* Protocol-Reject packets received in any state other than the LCP
       Opened state SHOULD be silently discarded */
    if( fsm_ptr->state != fsmOPENED )
    {
      dsm_free_packet(&bp);
      break;
    }

    rejected = dsm_pull16( &bp );
    if( rejected == -1 )
    {
      dsm_free_packet(&bp);
      break;
    }

#ifdef FEATURE_DATA_PS_EHRPD
    if( rejected == PPP_VSNCP_PROTOCOL )
    {
      vsncp_3gpp2_prot_rej( fsm_ptr->device, bp );
      return;
    }
#endif /* FEATURE_DATA_PS_EHRPD */

    switch ( rejected )
    {
    case PPP_LCP_PROTOCOL:
      protocol = PPP_PROTOCOL_LCP;
      tmp_fsm_ptr = &ppp_cb_ptr->fsm[LCP];      
#ifdef FEATURE_STA_PPP
      if(sta_ppp_enable)
      {
        tmp_fsm_ptr = &sta_cb_ptr->fsm;
      }
#endif /* FEATURE_STA_PPP */
        break;
    case PPP_IPCP_PROTOCOL:
      protocol = PPP_PROTOCOL_IPCP;
      tmp_fsm_ptr = &ppp_cb_ptr->fsm[IPCP];
      break;

#ifdef FEATURE_DATA_PS_IPV6
      case PPP_IPV6CP_PROTOCOL:
        protocol = PPP_PROTOCOL_IPV6CP;
        tmp_fsm_ptr = &ppp_cb_ptr->fsm[IPV6CP];      
        break;
#endif /* FEATURE_DATA_PS_IPV6 */
    default:
      break;
    };

    if( NULL != tmp_fsm_ptr )
    {

      LOG_MSG_INFO1_3("got proto_rej on dev %d in state %d for protocol 0x%x",
                tmp_fsm_ptr->device,
                tmp_fsm_ptr->state,
                rejected);

      if( PPP_PROTOCOL_LCP != protocol )
      {
        if( 0 != ( ppp_cb_ptr->ncp_mask & ( 1 << protocol ) ) )
        {
          LOG_MSG_INFO1_1("Supported protocol %d ", protocol);
          if( ( ( protocol == PPP_PROTOCOL_IPCP )&&
                ( PS_PPP_IS_FSM_INACTIVE( ppp_cb_ptr->fsm[IPV6CP].state ) ) ) ||
              ( ( protocol == PPP_PROTOCOL_IPV6CP )&&
                ( PS_PPP_IS_FSM_INACTIVE( ppp_cb_ptr->fsm[IPCP].state )  ) ) 
              )
          {
            LOG_MSG_INFO1_0("last NCP, tear down LCP");
            tmp_fsm_ptr = &ppp_cb_ptr->fsm[LCP];
          }
        }
        else
        {
          LOG_MSG_INFO1_1( "Received Protocol reject for an unsupported protocol %d ",
                           protocol );
          dsm_free_packet(&bp);
          break;
        }
      }/*protcol != LCP*/

      switch ( tmp_fsm_ptr->state )
      {
      case fsmCLOSING:
        /* TLF */
        tmp_fsm_ptr->pdc->stopping( tmp_fsm_ptr );
        tmp_fsm_ptr->state = fsmCLOSED;
        break;

      case fsmSTOPPING:
      case fsmREQ_Sent:
      case fsmACK_Rcvd:
      case fsmACK_Sent:
        /* TLF */
        tmp_fsm_ptr->pdc->stopping( tmp_fsm_ptr );
        tmp_fsm_ptr->state = fsmSTOPPED;
        break;

      case fsmOPENED:
        /*TLD*/
        (*tmp_fsm_ptr->pdc->closing)(tmp_fsm_ptr);
        /*IRC*/
        fsm_ptr->retry_request = fsm_ptr->try_terminate;
        (void)fsm_termreq(tmp_fsm_ptr);
        tmp_fsm_ptr->state = fsmSTOPPING;
        break;

      default:
        break;
      };
    }

    dsm_free_packet(&bp);
    break;

  case ECHO_REQ:
    PPP_INC_INSTANCE_FSM_STATS(fsm_ptr,echo_requests_rx,1);
    if( PPP_LCP_PROTOCOL == fsm_ptr->pdc->protocol )
    {
      if(fsmOPENED == fsm_ptr->state)
      {
        (void)fsm_send( fsm_ptr, ECHO_REPLY, hdr.identifier, bp );
      }
      else
      {
        dsm_free_packet(&bp);
      }
    }
    /* send protocol reject if not LCP */
    else
    {
      hdr.length += CONFIG_HDR_LEN;  /* restore length */
      bp = htoncnf( &hdr, bp );  /* put header back on */
      if (bp != NULL)
      {
        (void)fsm_send( fsm_ptr, CODE_REJ, hdr.identifier, bp );
      }
    }
    break;

  case ECHO_REPLY:
    PPP_INC_INSTANCE_FSM_STATS(fsm_ptr,echo_replies_rx,1);
    if( PPP_LCP_PROTOCOL == fsm_ptr->pdc->protocol )
    {
#ifdef FEATURE_DATA_3GPP2_ALWAYS_ON
      /*-----------------------------------------------------------------------
        Declare LSD success, free the timer and reset the control block
      -----------------------------------------------------------------------*/
      if( ppp_cb_ptr->lsd.config.result_cback != NULL )
      {
        LOG_MSG_INFO1_1("Dev %d, Link Status Determination succesful!",
                        fsm_ptr->device);
        ppp_cb_ptr->lsd.config.result_cback
        ( fsm_ptr->device,
          TRUE,
          ppp_cb_ptr->lsd.config.user_data 
        );
        ps_timer_free( ppp_cb_ptr->lsd.timer );
        memset( &ppp_cb_ptr->lsd.config, 0, sizeof( ppp_lsd_config_type ) );
      }
#endif /* FEATURE_DATA_3GPP2_ALWAYS_ON */
      dsm_free_packet(&bp);
    }
    /* send protocol reject if not LCP */
    else
    {
      hdr.length += CONFIG_HDR_LEN;  /* restore length */
      bp = htoncnf( &hdr, bp );  /* put header back on */
      if (bp != NULL)
      {
        (void)fsm_send( fsm_ptr, CODE_REJ, hdr.identifier, bp );
      }
    }
    break;

  case DISCARD_REQ:
    PPP_INC_INSTANCE_FSM_STATS(fsm_ptr,discard_requests_rx,1);
    if( PPP_LCP_PROTOCOL == fsm_ptr->pdc->protocol )
    {
      dsm_free_packet(&bp);
    }
    /* send protocol reject if not LCP */
    else
    {
      hdr.length += CONFIG_HDR_LEN;  /* restore length */
      bp = htoncnf( &hdr, bp );  /* put header back on */
      if (bp != NULL)
      {
        (void)fsm_send( fsm_ptr, CODE_REJ, hdr.identifier, bp );
      }
    }
    break;

  case IDENTIFICATION:
    PPP_INC_INSTANCE_FSM_STATS(fsm_ptr,identification_rx,1);
    if( PPP_LCP_PROTOCOL == fsm_ptr->pdc->protocol )
    {
      dsm_free_packet(&bp);
      break;
    }
    hdr.length += CONFIG_HDR_LEN;  /* restore length */
    bp = htoncnf( &hdr, bp );  /* put header back on */
    if (bp != NULL)
    {
      (void)fsm_send( fsm_ptr, CODE_REJ, hdr.identifier, bp );
    }
    break;

  case TIME_REMAINING:
    PPP_INC_INSTANCE_FSM_STATS(fsm_ptr,time_remaining_rx,1);
    if( PPP_LCP_PROTOCOL == fsm_ptr->pdc->protocol )
    {
      dsm_free_packet(&bp);
      break;
    }
    /* else fall through to default to send protocol reject */
  default:
    hdr.length += CONFIG_HDR_LEN;  /* restore length */
    bp = htoncnf( &hdr, bp );  /* put header back on */
    if (bp != NULL)
    {
      (void)fsm_send( fsm_ptr, CODE_REJ, hdr.identifier, bp );
    }
    break;
  };

  LOG_MSG_INFO2_3("leaving fsm_proc(device %d, protocol 0x%x, state %d)",
                  fsm_ptr->device, fsm_ptr->pdc->protocol, fsm_ptr->state);

} /* fsm_proc() */



/*===========================================================================
FUNCTION FSM_TIMEOUT()

DESCRIPTION
  This function will Timeout while waiting for reply from remote host

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  if it has not given up - may try another send...
  FALSE: if it has given up

SIDE EFFECTS
  None
===========================================================================*/
boolean fsm_timeout
(
  ppp_fsm_type *fsm_ptr
)
{
 
  if( PPP_MAX_DEV <= fsm_ptr->device )
  {
    LOG_MSG_ERROR_1( "Invalid device : %d", fsm_ptr->device);
    ASSERT(0);
    return FALSE;
  }
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*------------------------------------------------------------------------- 
    If the session handle is 0 then the device is already freed.
    Ignore timeout here.
  -------------------------------------------------------------------------*/

  if( ppp_cb_array[fsm_ptr->device].session_handle == 0 )
  {
    LOG_MSG_ERROR_1( "Timeout on free devive %d", fsm_ptr->device );
    return TRUE;  
  }

  switch(fsm_ptr->state)
  {
  case fsmREQ_Sent:
  case fsmACK_Sent:
    if (fsm_ptr->retry_request > 0)
    {
      (void)fsm_configreq(fsm_ptr);
      return TRUE;
    }
    else
    {
      return FALSE;
    }

  case fsmACK_Rcvd:
    if (fsm_ptr->retry_request > 0)
    {
      (void)fsm_configreq(fsm_ptr);
      fsm_ptr->state = fsmREQ_Sent;
      return TRUE;
    }
    else
    {
      return FALSE;
    }

  case fsmCLOSING:
    if (fsm_ptr->retry_request > 0)
    {
      /*---------------------------------------------------------------------
        Timeout after tx Term-Req.  Haven't hit LCP_TERM_TRY's, so try again
      ---------------------------------------------------------------------*/
      (void)fsm_termreq(fsm_ptr);
      return TRUE;
    }
    else
    {
      /*---------------------------------------------------------------------
      The mobile timed out while expecting a T-Ack. Give up on PPP and call
      LCP stopping.
      ---------------------------------------------------------------------*/
      LOG_MSG_INFO3_0( "Terminate Retry Exceeded, Stopping LCP");
      LOG_MSG_INFO2_1("if%d CLOSING timeout; PPP Done", fsm_ptr->device);
      (*fsm_ptr->pdc->stopping)(fsm_ptr);
      fsm_ptr->state = fsmCLOSED;
      return FALSE;
    }

  case fsmSTOPPING:
    /*-----------------------------------------------------------------------
      The mobile timed out after sending a T-ACK in response to the peer's
      T-Req.
    -----------------------------------------------------------------------*/
    if (fsm_ptr->retry_request > 0)
    {
      (void)fsm_termreq(fsm_ptr);
      return TRUE;
    }
    else
    {
      LOG_MSG_INFO2_1( "if%d STOPPING timeout: PPP Done", fsm_ptr->device);
      (*fsm_ptr->pdc->stopping)(fsm_ptr);
      fsm_ptr->state = fsmSTOPPED;
      return FALSE;
    }

  case fsmOPENED:
    /*-----------------------------------------------------------------------
      Timer expired but PPP protocol is now opened (This can happen, there is
      a very small timing window). Do not perform any work as the protocol
      is now Opened.
    -----------------------------------------------------------------------*/
    LOG_MSG_INFO1_0( "Timer Expired, PPP prtcl Open");
    return TRUE;

  default:  /* can't happen */
    DATA_ERR_FATAL("Invalid PPP State");
    return FALSE;
  }
} /* fsm_timeout() */


/*===========================================================================
FUNCTION FSM_PASSIVE_WAKEUP()

DESCRIPTION
  This function will perform the lower layer Up Event for the case when the
  FSM is in passive mode. It will force the fsm to send a config request to
  the peer (ie: wakeup in active mode)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void fsm_passive_wakeup
(
  ppp_fsm_type *fsm_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if ( fsm_ptr == NULL                 ||
       fsm_ptr->pdc == NULL            ||
       fsm_ptr->mode != PPP_CP_PASSIVE ||
       fsm_ptr->device >= PPP_MAX_DEV )
  {
    LOG_MSG_ERROR_0("Invalid args in fsm_passive_wakeup");
    return;
  }

  switch ( fsm_ptr->state )
  {
  case fsmSTARTING:
    fsm_irc(fsm_ptr);
    (*fsm_ptr->pdc->starting)(fsm_ptr);
    (void)fsm_configreq(fsm_ptr);
    fsm_ptr->state = fsmREQ_Sent;
    /* STARTING event */
    (*fsm_ptr->cbdc->call_event_callback_fcn) (fsm_ptr,
                                               fsm_ptr->pdc->protocol,
                                               PPP_STARTING_EV,
                                               PS_PPP_EVENT_UNKNOWN,
                                               PS_PPP_EVENT_UNKNOWN
                                              );
    break;
  default:
    /* Do nothing */
    break;
  }
} /* fsm_passive_wakeup() */



/*===========================================================================
FUNCTION FSM_UP()

DESCRIPTION
  This function will perform the lower layer Up Event

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void fsm_up
(
  ppp_fsm_type *fsm_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( fsm_ptr == NULL                 ||
       fsm_ptr->pdc == NULL            ||
       fsm_ptr->device >= PPP_MAX_DEV )
  {
    LOG_MSG_ERROR_0("Invalid args in fsm_up");
    return;
  }

  if( fsm_ptr->mode == PPP_CP_PASSIVE )
  {
    /* No action necessary for Up in Passive mode */
    return;
  }

  LOG_MSG_INFO2_3("fsm_up(device %d proto 0x%x state %d)",
           fsm_ptr->device, fsm_ptr->pdc->protocol, fsm_ptr->state);
  switch ( fsm_ptr->state )
  {
  case fsmINITIAL:
    (*fsm_ptr->pdc->stopping)(fsm_ptr);
    fsm_ptr->state = fsmCLOSED;
    break;

  case fsmSTARTING:
    fsm_irc(fsm_ptr);
    (*fsm_ptr->pdc->starting)(fsm_ptr);
    (void)fsm_configreq(fsm_ptr);
    fsm_ptr->state = fsmREQ_Sent;
    /* STARTING event */
    (*fsm_ptr->cbdc->call_event_callback_fcn) (fsm_ptr,
                                               fsm_ptr->pdc->protocol,
                                               PPP_STARTING_EV,
                                               PS_PPP_EVENT_UNKNOWN,
                                               PS_PPP_EVENT_UNKNOWN
                                              );
    break;

  default:
    /* already up */
    break;
  };
} /* fsm_up() */



/*===========================================================================
FUNCTION FSM_DOWN()

DESCRIPTION
  This function will perform the Lower Layer Down Event

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void fsm_down
(
  ppp_fsm_type *fsm_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if ((NULL == fsm_ptr) || (NULL == fsm_ptr->pdc))
  {
    DATA_ERR_FATAL("fsm_down: One or more values are NULL");
    return;
  }

  LOG_MSG_INFO2_3("fsm_down(dev %d, proto 0x%x, state %d)",
           fsm_ptr->device, fsm_ptr->pdc->protocol, fsm_ptr->state);

  switch ( fsm_ptr->state )
  {
  case fsmCLOSED:
    fsm_ptr->state = fsmINITIAL;
    break;

  case fsmSTOPPED:
    /*TLS*/
    fsm_ptr->state = fsmSTARTING;
    break;

  case fsmCLOSING:
    if( NULL == fsm_ptr->timer_cancel_fcn )
    {
      DATA_ERR_FATAL("timer_cancel_fcn is NULL");
      return;
    }
    (void)(*fsm_ptr->timer_cancel_fcn)( fsm_ptr->timer );
    fsm_ptr->state = fsmINITIAL;
    break;

  case fsmSTOPPING:
  case fsmREQ_Sent:
  case fsmACK_Rcvd:
  case fsmACK_Sent:
    if( NULL == fsm_ptr->timer_cancel_fcn )
    {
      DATA_ERR_FATAL("timer_cancel_fcn is NULL");
      return;
    }
    (void)(*fsm_ptr->timer_cancel_fcn)( fsm_ptr->timer );
    fsm_ptr->state = fsmSTARTING;
    break;

  case fsmOPENED:
    (*fsm_ptr->pdc->closing)(fsm_ptr);
    fsm_ptr->state = fsmSTARTING;
    break;

  default:
    /* already down */
    break;
  };
} /* fsm_down() */



/*===========================================================================
FUNCTION FSM_OPEN()

DESCRIPTION
  This function will Administratively Open the link

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void fsm_open
(
  ppp_fsm_type *fsm_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( fsm_ptr == NULL       ||
       fsm_ptr->pdc == NULL  ||
       fsm_ptr->device >= PPP_MAX_DEV )
  {
    DATA_ERR_FATAL("Invalid args in fsm_open");
    return;
  }

  LOG_MSG_INFO2_3("fsm_open(dev %d, proto 0x%x, state %d)",
           fsm_ptr->device, fsm_ptr->pdc->protocol, fsm_ptr->state);

  switch ( fsm_ptr->state )
  {
  case fsmINITIAL:
    /*TLS*/
    fsm_ptr->state = fsmSTARTING;
    break;

  case fsmCLOSED:
    fsm_irc(fsm_ptr);
    (*fsm_ptr->pdc->starting)(fsm_ptr);
    (void)fsm_configreq(fsm_ptr);
    fsm_ptr->state = fsmREQ_Sent;
    /* STARTING event */
    (*fsm_ptr->cbdc->call_event_callback_fcn) (fsm_ptr,
                                               fsm_ptr->pdc->protocol,
                                               PPP_STARTING_EV,
                                               PS_PPP_EVENT_UNKNOWN,
                                               PS_PPP_EVENT_UNKNOWN
                                              );
    break;

  case fsmCLOSING:
    fsm_ptr->state = fsmSTOPPING;
    break;

  case fsmSTOPPED:
    (*fsm_ptr->cbdc->call_event_callback_fcn) (fsm_ptr,
                                               fsm_ptr->pdc->protocol,
                                               PPP_DOWN_EV,
                                               PS_PPP_EVENT_UNKNOWN,
                                               PS_PPP_EVENT_UNKNOWN
                                              );
    break;

  default:
    /* already open */
    break;
  };
} /* fsm_open() */



/*===========================================================================
FUNCTION FSM_CLOSE()

DESCRIPTION
  This function will Administratively Close the link

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void fsm_close
(
  ppp_fsm_type *fsm_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( fsm_ptr->pdc == NULL )
  {
    LOG_MSG_ERROR_2("NULL fsm_ptr->pdc for fsm 0x%x for device %d",
                    	 fsm_ptr, fsm_ptr->device );
    ASSERT(0);
    return;
  }

  switch ( fsm_ptr->state )
  {
  case fsmSTARTING:
    (*fsm_ptr->pdc->stopping)(fsm_ptr);
    fsm_ptr->state = fsmINITIAL;
    break;

  case fsmSTOPPED:
    fsm_ptr->state = fsmCLOSED;
    break;

  case fsmSTOPPING:
    fsm_ptr->state = fsmCLOSING;
    break;

  case fsmOPENED:
    (*fsm_ptr->pdc->closing)(fsm_ptr);
    /* fallthru */
  case fsmREQ_Sent:
  case fsmACK_Rcvd:
  case fsmACK_Sent:
    fsm_ptr->retry_request = fsm_ptr->try_terminate;
    (void)fsm_termreq(fsm_ptr);
    fsm_ptr->state = fsmCLOSING;
    break;

  default:
    /* already closed */
    break;
  };
} /* fsm_close() */



/*===========================================================================
FUNCTION FSM_INIT()

DESCRIPTION
  This function will Initialize the fsm for this protocol

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void fsm_init
(
  ppp_fsm_type *fsm_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  LOG_MSG_INFO2_3("fsm_init(dev %d, proto 0x%x, state %d)",
           fsm_ptr->device, fsm_ptr->pdc->protocol, fsm_ptr->state);

  if( NULL == fsm_ptr->timer_cancel_fcn )
  {
    DATA_ERR_FATAL("timer_cancel_fcn is NULL");
    return ;
  }

  fsm_ptr->state = fsmINITIAL;

  fsm_ptr->try_configure = fsm_ptr->pdc->try_configure;
  fsm_ptr->try_nak = fsm_ptr->pdc->try_nak;
  fsm_ptr->try_terminate = fsm_ptr->pdc->try_terminate;
  fsm_ptr->allow_vendor_pkt = TRUE; /* allow sending vendor-specific pkts  */
  (void)(*fsm_ptr->timer_cancel_fcn)( fsm_ptr->timer);
  fsm_ptr->send_options_mask = 0; /* reset the send options bit mask */
  fsm_irc(fsm_ptr);

} /* fsm_init() */



/*===========================================================================
FUNCTION FSM_FREE()

DESCRIPTION
  This function is used to free the fsm data structure.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void fsm_free
(
  ppp_fsm_type *fsm_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (fsm_ptr->pdc != NULL)
  {
    if (fsm_ptr->pdc->fsm_free != NULL)
    {
      (*fsm_ptr->pdc->fsm_free)(fsm_ptr);
    }
    fsm_ptr->pdc = NULL;
  }
} /* fsm_free() */

/*===========================================================================
FUNCTION PPPI_GET_CRIT_SECT_LOCK()

DESCRIPTION
  Access function for PPP critical section array
 
PARAMETERS
  Device: Device for which the critical section structure is requested.
 
RETURN VALUE
  Critical section structure.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ps_crit_sect_type* pppi_get_crit_sect_lock
( 
  ppp_dev_enum_type device 
)
{
  return &ppp_cb_lock[device];
}

#ifdef FEATURE_STA_PPP
/*===========================================================================
FUNCTION FSM_GETCONST()

DESCRIPTION
 To get fsm constants from the phone

PARAMETERS
 fsm ptr      : pointer to ppp_fsm_typ
 ppp_fsm_enum : the fsm constant to fetch

RETURN VALUE
 Integer value of the constant, -1 in case of an error.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int fsm_getconst
(
  ppp_fsm_type *fsm_ptr,
  ppp_fsm_type_enum_type ppp_fsm_enum
)
{
  if(fsm_ptr == NULL || ppp_fsm_enum >= PPP_FSM_MAX_PARAM)
  {
    LOG_MSG_ERROR_2( "Invalid arguments: fsm_ptr %d, ppp_fsm_enum %d",
                     fsm_ptr, ppp_fsm_enum );
    ASSERT(0);
    return -1;
  }

  switch (ppp_fsm_enum)
  {
  case PPP_FSM_STATE:
     return fsm_ptr->state;
  case PPP_FSM_RX_IFACE_PTR:
     return (int32)fsm_ptr->rx_iface_ptr;
  case PPP_FSM_CONFIGURE_TRIES:
     return fsm_ptr->try_configure;
  case PPP_FSM_CONFIGURE_TRIES_REMAINING:
     return fsm_ptr->retry_request;
  case PPP_FSM_TERMINATE_TRIES:
     return fsm_ptr->try_terminate;
  case PPP_FSM_NAK_TRIES:
     return fsm_ptr->try_nak;
  case PPP_FSM_NAK_TRIES_REMAINING:
     return fsm_ptr->retry_nak;
  case PPP_FSM_ACK_TIMEOUT:
     return fsm_ptr->pdc->ack_timeout;
  case PPP_FSM_TERM_TIMEOUT:
     return fsm_ptr->pdc->term_timeout;
  default:
     LOG_MSG_ERROR_1( "Unknown ppp_fsm_enum %d", ppp_fsm_enum );
     return -1;
  }
}/*fsm_getconst*/
/*===========================================================================
FUNCTION FSM_SETCONST()

DESCRIPTION
 To set fsm constants on the phone

PARAMETERS
 fsm ptr      : pointer to ppp_fsm_typ
 ppp_fsm_enum : the fsm constant to set
 value        : byte-array of data for new value

RETURN VALUE
 0 for success, -1 in case of an error.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int fsm_setconst
(
  ppp_fsm_type *fsm_ptr,
  ppp_fsm_type_enum_type ppp_fsm_enum,
  uint32 value
)
{
  if(fsm_ptr == NULL || ppp_fsm_enum >= PPP_FSM_MAX_PARAM)
  {
    LOG_MSG_ERROR_2( "Invalid arguments: fsm_ptr %d, ppp_fsm_enum %d",
                     fsm_ptr, ppp_fsm_enum );
    ASSERT(0);
    return -1;
  }

  switch (ppp_fsm_enum)
  {
  case PPP_FSM_STATE:
     fsm_ptr->state = (uint8)value;
     break;
  case PPP_FSM_RX_IFACE_PTR:
     fsm_ptr->rx_iface_ptr = (ps_iface_type *)value;
     break;
  case PPP_FSM_CONFIGURE_TRIES:
     fsm_ptr->try_configure = (uint8)value;
     break;
  case PPP_FSM_CONFIGURE_TRIES_REMAINING:
     fsm_ptr->retry_request = (uint8)value;
     break;
  case PPP_FSM_TERMINATE_TRIES:
     fsm_ptr->try_terminate = (uint8)value;
     break;
  case PPP_FSM_NAK_TRIES:
     fsm_ptr->try_nak = (uint8)value;
     break;
  case PPP_FSM_NAK_TRIES_REMAINING:
     fsm_ptr->retry_nak = (uint8)value;
     break;
  case PPP_FSM_ACK_TIMEOUT:
     fsm_ptr->pdc->ack_timeout = (uint16)value;
     break;
  case PPP_FSM_TERM_TIMEOUT:
     fsm_ptr->pdc->term_timeout = (uint16)value;
     break;
   default:
     LOG_MSG_ERROR_1( "Unknown ppp_fsm_enum %d", ppp_fsm_enum );
     return -1;
  }
  return 0;
}/*fsm_setconst*/
#endif /* FEATURE_STA_PPP */
