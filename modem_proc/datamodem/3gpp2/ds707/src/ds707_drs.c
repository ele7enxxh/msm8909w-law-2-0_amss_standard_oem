/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 
                           D S 7 0 7 _ D R S

GENERAL DESCRIPTION
  This file contains functions that are used to see if the DRS bit should be
  set on originations out of dormancy.
  
EXTERNALIZED FUNCTIONS
  ds707_drs_get_mip_drs_bit()
    Called on a pkt re-origination to see if Mobile IP requires the DRS bit
    to be set, indicating that the mobile has data to send.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

 Copyright (c) 2002-2005 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_drs.c_v   1.5   02 Dec 2002 19:40:10   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_drs.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who        what, where, why
--------   ---        ------------------------------------------------------- 
06/30/14   niv        Added wrapper functions & API change for 
                      NV_REFRESH 
04/18/12   jz      Seperate old JCDMA and Simplified JCDMA 
07/20/07   az         Fixed lint errors
11/11/05   vrk/gr Avoiding ppp resync on failed ALN on MIP
09/14/05   vrk        Don't set hdr to 1x hand off resync flag until call gets
                      connected on 1x. So, removed the setting on idm change
09/10/05   gr         Merged changes to lock out GPS in the middle of a PPP 
                      resync during a handup from 1x to DO
07/28/04   ak         on handle drs1_recon, account for ipv6 iface.
07/26/04   ifk        Added protocol to ppp_resync()
06/10/04   kvd        Added  reporting PPP resync events with resync reason.
04/19/03   jd         Fix compile error when FEATURE_DS_MOBILE_IP turned off
                      Localize dependency on regsm.h to drs_is_in_mip_call()
04/06/03   vr         Don't resync PPP if Um is still coming up
03/19/04   ak         Include sys.h
02/19/04   ak         Removed ps707_dssnet.h include.
12/03/03   kvd        unfeaturized  ds707_drs_handle_drs1_recon and
                      ds707_drs_ppp_resync_reqd from FEATURE_DS_MOBILE_IP
10/21/03   vr         Added PPP resync on handdown from HDR to 1X for simple
                      IP calls in feature FEATURE_HDR_PPP_RESYNC_ON_HANDDOWN 
                      Added DS707_DRS_IS_PPP_RESYNC_ON_HANDDOWN_SET
10/15/03   vr         Featurized access to NV_DS_MIP_QC_HANDDOWN_TO_1X_OPT_I
                      under FEATURE_HDR
10/13/03   vr/kvd     Added flags to determine if we need to do a PPP resync
                      in different handoff scenarios and functions to 
                      set/clear/check those flags.
07/09/03   ak         Better worded MSG when DRS = 1.
05/05/03   jd         Replaced ds707_drsi_is_mip_up_on_um() with 
                      mip_reg_sm_session_active()
12/02/02   ak         Fixed up compiler warnings.
11/25/02   usb        Added functionality to initiate PPP resync when drs 
                      bit is set.
09/19/02   jd         Only apply DRS opt. if turned on and in MIP call
09/13/02   jd         Added DRS optimization for mobile IP (always on)
07/29/02   ak         Initial revision.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707

#if defined(T_WINNT)
#error code not present
#endif /* WINNT */
                        
#include "msg.h"
#include "sys.h"
#include "data_msg.h"
#include "ds707_pdn_context.h"
#include "ds3gcfgmgr.h"
#ifdef FEATURE_DS_MOBILE_IP
#include "nv.h"
#include "dstaski.h"
#include "dsmip_pdsn_db.h"
#include "dsmip_regsm.h"  // future - remove this dep
#endif /* FEATURE_DS_MOBILE_IP */

#include "ds707_pkt_mgr.h"
#include "ps_ppp.h"
#include "ds707_event_defs.h"
#include "ds707_roaming.h"


/*===========================================================================
                        TYPEDEFS AND VARIABLES
===========================================================================*/

#ifdef FEATURE_DS_MOBILE_IP
LOCAL nv_item_type drs_opt_nv_item;
#endif /* FEATURE_DS_MOBILE_IP */

/*---------------------------------------------------------------------------
  These are run-time flags that are set if the AT needs to do a PPP resync
  due to data session handoff in one of handoff scenarios.
---------------------------------------------------------------------------*/
LOCAL struct
{
  boolean handoff_HDR_to_1X; /* IDLE or ACTIVE */
  boolean handoff_1X_to_1X;
  boolean handoff_ALN;
} ds707_drsi_ppp_resync_flags = {FALSE,FALSE,FALSE};

/*---------------------------------------------------------------------------
  These settings reflect the values of the corresponding NV items. 
  qcodh is TRUE is qcodh is enabled.
  mip_handdown_to_1X is TRUE if we want to do a PPP resync on handdown from
  HDR to 1X for a MIP call.
---------------------------------------------------------------------------*/
LOCAL struct
{
  boolean qcodh;
  boolean mip_handdown_to_1X;
} ds707_drsi_ppp_resync_settings = {FALSE,FALSE};


/*===========================================================================
                    FORWARD FUNCTION DEFINITIONS
===========================================================================*/
boolean ds707_drs_is_in_mip_call(void);


/*===========================================================================
                   EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/


/*===========================================================================
FUNCTION      DS707_DRS_INIT

DESCRIPTION   Initializes the Data Send Request (DSR) algorithm handler.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_drs_init(void)
{
#ifdef FEATURE_DS_MOBILE_IP
  nv_stat_enum_type   nv_status;                   /* status from NV call  */
#endif /* FEATURE_DS_MOBILE_IP */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize the flags and NV item settings to FALSE.
  -------------------------------------------------------------------------*/
  memset(&ds707_drsi_ppp_resync_settings, 0, 
          sizeof(ds707_drsi_ppp_resync_settings));
  memset(&ds707_drsi_ppp_resync_flags, 0,
          sizeof(ds707_drsi_ppp_resync_flags));

  /*-------------------------------------------------------------------------
    Retrieve the user preference for use of the zombie PPP avoidance (DRS)
    algorithm.  Default is FALSE (don't use) if not specified.
  -------------------------------------------------------------------------*/

#ifdef FEATURE_DS_MOBILE_IP
  nv_status = ds3gcfgmgr_read_legacy_nv_ex( NV_DS_MIP_QC_DRS_OPT_I, &drs_opt_nv_item, ds707_curr_subs_id());
  if( nv_status == NV_DONE_S)
  {
    ds707_drsi_ppp_resync_settings.qcodh = drs_opt_nv_item.ds_mip_qc_drs_opt;
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED,"Failed to read MIP_QC_DRS_OPT, DRS off!");
  }

#ifdef FEATURE_HDR
  /*-------------------------------------------------------------------------
    Default mip_handdown_to_1X to TRUE if FEATURE_DS_MOBILE_IP is defined.
  -------------------------------------------------------------------------*/
  ds707_drsi_ppp_resync_settings.mip_handdown_to_1X = TRUE;
  nv_status = ds3gcfgmgr_read_legacy_nv_ex( NV_DS_MIP_QC_HANDDOWN_TO_1X_OPT_I,
                               &drs_opt_nv_item, ds707_curr_subs_id());
  if( nv_status == NV_DONE_S)
  {
    ds707_drsi_ppp_resync_settings.mip_handdown_to_1X =
                   drs_opt_nv_item.ds_mip_qc_handdown_to_1x_opt;
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "Failed to read MIP_QC_HANDDOWN_TO_1X_OPT, DRS off!");
  }
#endif

#endif /* FEATURE_DS_MOBILE_IP */
} /* ds707_drs_init */


/*===========================================================================
FUNCTION      DS707_DRS_GET_MIP_DRS_BIT

DESCRIPTION   This function returns TRUE if the DRS bit should be set.  Else
              returns FALSE if the DRS bit should not be set.

DEPENDENCIES  None.

RETURN VALUE  See description.

SIDE EFFECTS  sets ds707_drsi_ppp_resync_flags.handoff_1X_to_1X
===========================================================================*/
boolean ds707_drs_get_mip_drs_bit(void)
{
  /*-------------------------------------------------------------------------
    If this function is going to be used for something OTHER than MIP it will
    likely be necessagy to modify the ds707_drs_handle_snpzid_change_recon()
    function to handle the change!
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DS_MOBILE_IP
  pdsn_db_kind_enum_type curr_pdsn_kind;  /* Have we seen this PDSN        */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    For Mobile IP calls, if handoff optimization algorithm is enabled,
    see if the MS needs to set DRS to 1 to get around zombie PPP
    instances in the network.
  -------------------------------------------------------------------------*/
  if (ds707_drsi_ppp_resync_settings.qcodh && ds707_drs_is_in_mip_call())
  {
    /*-----------------------------------------------------------------
      If we have been to this PDSN before then there is a chance that
      there is a zombie PPP instance - so set DRS to 1 for MIP calls.
      Also, if this is a new PCF, the mobile doesn't know which PDSN
      this PCF may be connected to - so set DRS conservatively to 1.
      Note, if the carrier has disabled this optimization or the MS is
      in a prev less than 6, then pdsn_db_get will return DISABLED - so
      regular DRS algorithm will be used.
    -----------------------------------------------------------------*/
    curr_pdsn_kind = pdsn_db_get_kind();
    if ((curr_pdsn_kind == VISITED_PDSN) || (curr_pdsn_kind == UNKNOWN_PDSN))
    {
      DATA_IS707_MSG0(MSG_LEGACY_MED, "MIP PDSN handoff requires orig with DRS=1");
      ds707_drsi_ppp_resync_flags.handoff_1X_to_1X = TRUE;
      return (TRUE);
    }
  }
#endif /* FEATURE_DS_MOBILE_IP */

  return(FALSE);
} /* ds707_drs_get_mip_drs_bit() */

#ifdef FEATURE_HDR

/*===========================================================================
FUNCTION      DS707_DRS_GET_DRS_BIT_ON_IDM_CHANGE

DESCRIPTION   This function returns TRUE if currently a mip session is active
              and if ds707_drsi_ppp_resync_settings.mip_handdown_to_1X is set
              in NV and sets the ds707_drsi_ppp_resync_flags.handoff_HDR_to_1X
              so that ppp_resyc is initiated upon call_connect

DEPENDENCIES  None.

RETURN VALUE  See description.

SIDE EFFECTS  None.
===========================================================================*/

boolean ds707_drs_get_drs_bit_on_idm_change
(
  sys_sys_mode_e_type new_network
)
{

  if(new_network == SYS_SYS_MODE_CDMA)
  {
#ifdef FEATURE_DS_MOBILE_IP
    if(ds707_drs_is_in_mip_call())
    {
      if(ds707_drsi_ppp_resync_settings.mip_handdown_to_1X)
      {
        DATA_IS707_MSG0(MSG_LEGACY_MED, "MIP HDR to 1X handoff requires orig with DRS=1");
        return TRUE;
      }
    }
    else
    {
#endif /* FEATURE_DS_MOBILE_IP */
      if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
      {
        DATA_IS707_MSG0(MSG_LEGACY_MED, "SIP HDR to 1X handoff requires orig with DRS=1");
        return TRUE;
      }
      
#ifdef FEATURE_DS_MOBILE_IP
    }
#endif /* FEATURE_DS_MOBILE_IP */
  }

  return FALSE;
} /* ds707_drs_get_drs_bit_on_idm_change() */


/*===========================================================================
FUNCTION      DS707_DRS_GET_DRS_BIT_ON_ALN

DESCRIPTION   This function returns TRUE if currently a mip session is active
              called when we INITIATE_ALN_CDM form HDRMC
              This sets ds707_drsi_ppp_resync_flags.handoff_ALN to TRUE, so
              that ppp_resync is initiated upon call_connect.

DEPENDENCIES  None.

RETURN VALUE  See description.

SIDE EFFECTS  None.
===========================================================================*/

boolean ds707_drs_get_drs_bit_on_ALN(void)
{
#ifdef FEATURE_DS_MOBILE_IP
  if(ds707_drs_is_in_mip_call())
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "MIP ALN requires orig with DRS=1");
    ds707_drsi_ppp_resync_flags.handoff_ALN = TRUE;
    return TRUE;
  }
#endif /* FEATURE_DS_MOBILE_IP */
  return FALSE;
} /* ds707_drs_get_drs_bit_on_ALN() */

#endif /* FEATURE_HDR */

/*===========================================================================
FUNCTION      DS707_DRS_RESET_PPP_RESYNC_FLAGS

DESCRIPTION   This function is used to reset the ppp_resync flags on
              call_connect after ppp_resync(), if needed done. Called from
              ds707_call_conn_hdlr()

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/

void ds707_drs_reset_ppp_resync_flags(void)
{
    DATA_IS707_MSG0(MSG_LEGACY_MED, "Resetting PPP resync flags");
    ds707_drsi_ppp_resync_flags.handoff_HDR_to_1X = FALSE;
    ds707_drsi_ppp_resync_flags.handoff_1X_to_1X = FALSE;
    ds707_drsi_ppp_resync_flags.handoff_ALN = FALSE;
} /* ds707_drs_reset_ppp_resync_flags() */



/*===========================================================================
FUNCTION      DS707_DRS_PPP_RESYNC_REQUIRED

DESCRIPTION   This function  returns TRUE if we need to do a ppp resync
              following a call_connect.

DEPENDENCIES  None.

RETURN VALUE  Boolean

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_drs_ppp_resync_reqd(void)
{
  ds707_event_payload_type  ds707_event_payload;
  boolean resync_flag=FALSE;

  ds707_event_payload.ds707_event  = EVENT_DS707_PPP_RESYNC;

  if( ds707_drsi_ppp_resync_flags.handoff_HDR_to_1X )
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "PPP resync reqd on handoff from HDR to 1X");
    ds707_event_payload.info.i_resync = DS707_RESYNC_ON_HDR_1X_HANDOFF;
    resync_flag=TRUE;
  }
  else if( ds707_drsi_ppp_resync_flags.handoff_ALN )
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "PPP resync reqd on ALN");
    ds707_event_payload.info.i_resync = DS707_RESYNC_ON_ALN;
    resync_flag=TRUE;
  }
  else if( ds707_drsi_ppp_resync_flags.handoff_1X_to_1X )
  {
    if( ds707_drs_get_mip_drs_bit())
    {
      DATA_IS707_MSG0(MSG_LEGACY_MED, "PPP resync reqd on QCODH");
      ds707_event_payload.info.i_resync = DS707_RESYNC_ON_QCODH;
      resync_flag=TRUE;
    }
  }
  if(resync_flag)
  {
    /*---------------------------------------------------------------------
      Acquire the ppp resync lock from DSSNET before proceeding with
      the Resync procedure. If the lock is acquired just after GPS checked
      the status to do a fix, it would atleast prevent GPS from doing
      another back-to-back fix after the current fix
    ---------------------------------------------------------------------*/
    ds707_pkt_acquire_ppp_resync_lock();

    event_report_payload( EVENT_DS707,
                          sizeof (ds707_event_payload),
                          &ds707_event_payload );
  }
  return resync_flag;
} /* ds707_drs_ppp_resync_reqd() */




/*===========================================================================
FUNCTION      DS707_DRS_HANDLE_DRS1_RECON

DESCRIPTION   This function handles the reconection due to SID/NID/PZID 
              change. If the drs bit was set during reconnection then it 
              initiates a PPP resync on UM interface.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_drs_handle_drs1_recon(void)
{
  ps_iface_type          *iface_ptr  = NULL;
  boolean                 resync_ppp_flag = FALSE;
  uint8                   curr_iface_index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for ( curr_iface_index = 0;
        curr_iface_index < DS707_MAX_IFACES;
        curr_iface_index++ )
  {
    iface_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(curr_iface_index);

    if ( (iface_ptr != NULL) &&
         ( (ps_iface_state(iface_ptr) == IFACE_UP)          ||
           (ps_iface_state(iface_ptr) == IFACE_ROUTEABLE)   ||
           (ps_iface_state(iface_ptr) == IFACE_CONFIGURING)
         )
       )
    {
      resync_ppp_flag = TRUE;
    }
  }  

  /*-------------------------------------------------------------------------
    This function will be called only if Um iface is COMING_UP or 
    UP/ROUTEABLE/CONFIGURING. If Um iface is COMING_UP, we don't want to do 
    a "resync" since initial PPP is not yet established. Let the initial PPP 
    establishment continue. We rely on the network to do a resync if needed
    in that scenario.
  -------------------------------------------------------------------------*/
  if ( ds707_drs_ppp_resync_reqd() && ( resync_ppp_flag == TRUE) )
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "Doing a PPP resync");
    /*-----------------------------------------------------------------------
      Do a PPP resync on the UM interface. This will fail if it is a relay
      model call.
    -----------------------------------------------------------------------*/
    if(ppp_resync(PPP_UM_SN_DEV, PPP_PROTOCOL_LCP, NULL) < 0)
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Failed to resync PPP on SNPZID change");
    }
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "Not doing PPP resync - not reqd or Um still coming up");
  }

} /* ds707_drs_handle_drs1_recon() */

/*===========================================================================
FUNCTION      DS707_DRS_MOVED_TO_1X

DESCRIPTION   set ds707_drsi_ppp_resync_flags.handoff_HDR_to_1X if MIP call
              UP

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_drs_moved_to_1X(void)
{

#ifdef FEATURE_DS_MOBILE_IP
  if(ds707_drs_is_in_mip_call())
  {
    if(ds707_drsi_ppp_resync_settings.mip_handdown_to_1X)
    {
      DATA_IS707_MSG0(MSG_LEGACY_MED, "MIP handdown HDR to 1X requires PPP resync");
      ds707_drsi_ppp_resync_flags.handoff_HDR_to_1X = TRUE;
    }
  }
  else
  {
#endif /* FEATURE_DS_MOBILE_IP */
/*Simplified JCDMA: 4.2.5 Reconnect after hand down to 1x from HDR [D1-61]*/
    if(ds707_roaming_is_curr_mode_jcdma()||
       ds707_roaming_is_simplified_jcdma_mode())
    {
      DATA_IS707_MSG0(MSG_LEGACY_MED, "SIP HDR to 1X handoff requires PPP resync");
      ds707_drsi_ppp_resync_flags.handoff_HDR_to_1X = TRUE;
    }
#ifdef FEATURE_DS_MOBILE_IP
  }
#endif /* FEATURE_DS_MOBILE_IP */

} /* ds707_drs_moved_to_1X() */


/*===========================================================================
FUNCTION      DS707_DRS_IS_IN_MIP_CALL

DESCRIPTION   Wrapper function that returns TRUE if we are in a mobile IP
              call.  This is provided because all the checks in this file
              address zombie PPP which is only a problem for Mobile IP.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_drs_is_in_mip_call(void)
{
#ifdef FEATURE_DS_MOBILE_IP
  // future - decouple from mip client (MIP status active bit in ps_iface)
  if(mip_reg_sm_session_active(NULL))
  {
    return TRUE;
  }
#endif /* FEATURE_DS_MOBILE_IP */
  return FALSE;
} /* ds707_drs_is_in_mip_call() */

/*===========================================================================
FUNCTION      DS707_DRS_IS_PPP_RESYNC_ON_HANDDOWN_SET

DESCRIPTION   Wrapper function that returns TRUE if the flag to do PPP resync
              due to handdown from HDR to 1X is set.

DEPENDENCIES  None.

RETURN VALUE  handoff_HDR_to_1X flag.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_drs_is_ppp_resync_on_handdown_set( void )
{
  return ds707_drsi_ppp_resync_flags.handoff_HDR_to_1X;
}

/*===========================================================================
FUNCTION      DS707_DRS_RESET_ALN_PPP_RESYNC_FLAG

DESCRIPTION   This function RESETs the ALN PPP resync flag to FALSE
              This is to handle the scenario where ALN fails and the
              packet data session is connected on 1x

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/

void ds707_drs_reset_aln_ppp_resync_flag(void)
{
  ds707_drsi_ppp_resync_flags.handoff_ALN = FALSE;
} /* ds707_drs_reset_aln_ppp_resync_flag() */

#endif /* FEATURE_DATA_IS707 */

