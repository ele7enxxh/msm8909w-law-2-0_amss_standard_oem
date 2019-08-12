/*===========================================================================
                         D S 7 0 7 _ E X T I F
GENERAL DESCRIPTION
  This file is the external interface exported by ds707 to the other layers
  in the DMSS code than need to query ds707 for its state or other 
  information. This file is intended to provide an interface to other layers 
  in the DMSS code and is not intended to be used as an interface by
  entities outside of DMSS.
  
  This is specific to packet data calls. 

EXTERNALIZED FUNCTIONS
 DS707_EXTIF_PKT_IS_DORMANT
   Checks if there is a dormant packet data session on CDMA or HDR system.

 DS707_EXTIF_IS_PKT_DATA_SESS_ON_HDR
   Checks if there is a packet data session that is active or dormant on an 
   HDR system.
 
 DS707_EXTIF_IS_PKT_DATA_SESS_ON_CDMA
   Checks if there is a packet data session that is active or dormant on a 
   CDMA system.
 
 INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

 Copyright (c) 2003-2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_extif.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when       who        what, where, why
--------   ---        -------------------------------------------------------
11/18/11   vpk     	  HDR SU API cleanup
01/06/11   scb        Changes for UW FMC Feature
01/31/11   ttv        Removed feature wrapping in api files.
12/15/09   ls         Added Rev-B bearer tech support
10/20/08   ms         Fixed compilation error for low tier builds
09/10/05   gr         Merged changes to lock out GPS in the middle of a PPP 
                      resync during a handup from 1x to DO
04/12/05   vrk        Included wrapper function ds707_extif_verify_so
02/13/04   ak         Do get_dormant on phys link instead of iface.
01/27/04   vr         Added functions
                      ds707_extif_was_last_data_network_cdma and
                      ds707_extif_was_last_data_network_hdr.
10/02/03   vr         Changed function names.
09/16/03   vr         First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "data_msg.h"
#include "ds707_extif.h"
#include "ds707_pkt_mgr.h"
#include "ps_phys_link.h"
#include "ps_ppp.h"
#include "hdrcom_api.h"
#include "hdrcp_api.h"
#include "ds707_sys_chg_hdlr.h"

/*===========================================================================
                          PUBLIC CONSTANTS
===========================================================================*/

/*---------------------------------------------------------------------------
  Maximum number of pkt data calls supportable by IS707/IS2000 packet data
  module.
---------------------------------------------------------------------------*/

#define DS707_EXTIF_DATA_RATE_CDMA_RS1      9600            /* 9.6         */
#define DS707_EXTIF_DATA_RATE_CDMA_RS2      14400           /* 14.4        */

#ifdef FEATURE_IS2000_REL_A
#define DS707_EXTIF_MAX_NUM_SCH             32
#else
#define DS707_EXTIF_MAX_NUM_SCH             16
#endif /* FEATURE_IS2000_REL_A */

#define DS707_EXTIF_DATA_RATE_HDR_FL        2400000         /* HDR TX RATE */
#define DS707_EXTIF_DATA_RATE_HDR_RL        153600          /* HDR RX RATE */

#define DS707_EXTIF_DATA_RATE_HDR_REVA_FL   3100000   /* HDR REV A TX RATE */
#define DS707_EXTIF_DATA_RATE_HDR_REVA_RL   1800000   /* HDR REV A RX RATE */

#define DS707_EXTIF_DATA_RATE_HDR_REVB_FL   9300000   /* HDR REV B TX RATE */
#define DS707_EXTIF_DATA_RATE_HDR_REVB_RL   5400000   /* HDR REV B RX RATE */

/*===========================================================================
                            VARIABLES
===========================================================================*/

/*===========================================================================
                        INTERNAL FUNCTIONS
===========================================================================*/

/*===========================================================================
                        EXTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_EXTIF_PKT_IS_DORMANT

DESCRIPTION   Checks if there is a dormant packet data session on a CDMA 
              or HDR system.

DEPENDENCIES  None.

RETURN VALUE  TRUE if there is a dormant packet data session, 
              FALSE otherwise.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_extif_pkt_is_dormant( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ds707_pkt_is_dormant(
              ds707_pkt_get_pri_phys_link_ptr());
} /* ds707_extif_pkt_is_dormant() */


/*===========================================================================
FUNCTION      DS707_EXTIF_IS_PPP_UP

DESCRIPTION   

DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_extif_is_ppp_up( void )
{
  if( (ppp_get_phase(PPP_UM_SN_DEV)== pppDEAD) || 
      (ppp_get_phase(PPP_UM_SN_DEV)== pppTERMINATE) )
  {
    return FALSE;
  }
  
  return TRUE;
}

/*===========================================================================
FUNCTION      DS707_EXTIF_IS_PKT_DATA_SESS_ON_HDR

DESCRIPTION   Checks if there is a packet data session that is active or 
              dormant on an HDR system.

DEPENDENCIES  None.

RETURN VALUE  TRUE if a packet data session is active or dormant on HDR, 
              FALSE otherwise.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_extif_is_pkt_data_sess_on_hdr( void )
{
  ds707_pkt_data_sess_state_type data_sess_state;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  data_sess_state = ds707_get_pkt_data_sess_state();

  /*-------------------------------------------------------------------------
    Return TRUE if the data session is dormant or active on an IS-856 system
  -------------------------------------------------------------------------*/
  if((data_sess_state == DS707_PKT_DATA_SESS_HDR_DORMANT) ||
     (data_sess_state == DS707_PKT_DATA_SESS_HDR_ACTIVE)
    )
  {
    return TRUE;
  }
  else
  {
  return FALSE;
  }

} /* ds707_extif_is_pkt_data_sess_on_hdr() */


/*===========================================================================
FUNCTION      DS707_EXTIF_IS_PKT_DATA_SESS_ON_CDMA

DESCRIPTION   Checks if there is a packet data session that is active or 
              dormant on an CDMA system.

DEPENDENCIES  None.

RETURN VALUE  TRUE if a packet data session is active or dormant on CDMA, 
              FALSE otherwise.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_extif_is_pkt_data_sess_on_cdma( void )
{
  ds707_pkt_data_sess_state_type data_sess_state;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  data_sess_state = ds707_get_pkt_data_sess_state();

  /*-------------------------------------------------------------------------
    Return TRUE if the data session is dormant or active on an IS-707 system
  -------------------------------------------------------------------------*/
  if((data_sess_state == DS707_PKT_DATA_SESS_CDMA_DORMANT) ||
     (data_sess_state == DS707_PKT_DATA_SESS_CDMA_ACTIVE)
    )
  {
    return TRUE;
  }
  else
  {
  return FALSE;
  }
} /* ds707_extif_is_pkt_data_sess_on_cdma() */


/*===========================================================================
FUNCTION      DS707_EXTIF_INFORM_HDR_SESS_STATUS

DESCRIPTION   Informs the HDR protocols module whether the current is on
              HDR or not. This function is called everytime the session
              status changes.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_extif_inform_hdr_sess_status( void )
{
#ifdef FEATURE_HDR
  hdrcp_idle_set_pkt_data_sess_status( ds707_extif_is_pkt_data_sess_on_hdr());
#endif
}

/*===========================================================================
FUNCTION      DS707_EXTIF_WAS_LAST_DATA_NETWORK_CDMA

DESCRIPTION   Checks if there is a dormant or active packet data session that
              was previously on an CDMA system before the current system it
              is on.

DEPENDENCIES  None.

RETURN VALUE  TRUE if a packet data session is active or dormant and was on 
                   CDMA prior to the current system it is on,
              FALSE otherwise.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_extif_was_last_data_network_cdma( void)
{
  return (ds707_pkt_get_last_data_sess_network() == SYS_SYS_MODE_CDMA);
}


/*===========================================================================
FUNCTION      DS707_EXTIF_WAS_LAST_DATA_NETWORK_HDR

DESCRIPTION   Checks if there is a dormant or active packet data session that
              was previously on an HDR system before the current system it
              is on.

DEPENDENCIES  None.

RETURN VALUE  TRUE if a packet data session is active or dormant and was on 
                   HDR prior to the current system it is on,
              FALSE otherwise.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_extif_was_last_data_network_hdr ( void )
{
  return (ds707_pkt_get_last_data_sess_network() == SYS_SYS_MODE_HDR);
}


/*===========================================================================
FUNCTION      DS_EXTIF_VERIFY_SO

DESCRIPTION   Wrapper for DS_VERIFY_SO. Called by MC.  
	      Asks DS if the originating SO is correct.

DEPENDENCIES  None

RETURN VALUE  service option to orig with

SIDE EFFECTS  None.
===========================================================================*/

word ds707_extif_verify_so(word so_to_verify)
{
  return (ds_verify_so(so_to_verify));
}

/*===========================================================================
FUNCTION DS707_EXTIF_CHECK_PPP_RESYNC_LOCK()

DESCRIPTION
  This is a wrapper function for the ds707_check_ppp_resync_lock. It checks 
  the PPP resync lock status (locked/unlocked) for all the ifaces(IPV4 
  and IPV6) and returns a boolean (TRUE if locked and FALSE if unlocked). It
  also registers a callback to notify the lock release event, if 
  it is currently locked.
  

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
  boolean lock_status = TRUE  if locked 
                        FALSE if unlocked 

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds707_extif_check_ppp_resync_lock
(
  void (* ppp_resync_lock_release_cb)(void)  
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return (ds707_pkt_check_ppp_resync_lock(ppp_resync_lock_release_cb));

} /* ds707_extif_check_ppp_resync_lock () */

/*===========================================================================
FUNCTION      DS707_EXTIF_GET_BEARER_TECH_MAX_RATE

DESCRIPTION   Returns the max rate that a bearer technology can carry. 
              This function can be used by other modules to query for a given
              technology.

DEPENDENCIES  None.

PARAMETERS    bearer_tech_query: The memory needs to be allocated by the 
                module querying.  Return values will be populated inside this
                structure. 

RETURN VALUE  TRUE: If the values are filled in.
              FALSE:  If the values are not filled in.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_extif_get_bearer_tech_max_rate
( 
  ds707_extif_bearer_tech_rate_type *bearer_tech_query 
)
{
  boolean             ret_val = FALSE;
  uint32              data_rate;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( ( bearer_tech_query != NULL ) &&
       ( bearer_tech_query->bearer_tech > DS707_EXTIF_INVALID_TECH ) && 
       ( bearer_tech_query->bearer_tech < DS707_EXTIF_MAX_BEARER_TECH )
     )  
  {
    switch ( bearer_tech_query->bearer_tech )
    {
      
      case DS707_EXTIF_1X:
        {
          data_rate = DS707_EXTIF_MAX_NUM_SCH*DS707_EXTIF_DATA_RATE_CDMA_RS1 + DS707_EXTIF_DATA_RATE_CDMA_RS1;
          bearer_tech_query->max_tx_bearer_tech_rate = data_rate;
          bearer_tech_query->max_rx_bearer_tech_rate = data_rate;
          ret_val = TRUE;
        }
        break;
#ifdef FEATURE_HDR
      case DS707_EXTIF_HDR_REV0:
        {
          bearer_tech_query->max_rx_bearer_tech_rate = DS707_EXTIF_DATA_RATE_HDR_FL;
          bearer_tech_query->max_tx_bearer_tech_rate = DS707_EXTIF_DATA_RATE_HDR_RL;
          ret_val = TRUE;
        }
        break;

      case DS707_EXTIF_HDR_REVA:
        {
          bearer_tech_query->max_rx_bearer_tech_rate = DS707_EXTIF_DATA_RATE_HDR_REVA_FL;
          bearer_tech_query->max_tx_bearer_tech_rate = DS707_EXTIF_DATA_RATE_HDR_REVA_RL;
          ret_val = TRUE;
        }
        break;

      case DS707_EXTIF_HDR_REVB:
        {
          bearer_tech_query->max_rx_bearer_tech_rate = DS707_EXTIF_DATA_RATE_HDR_REVB_FL;
          bearer_tech_query->max_tx_bearer_tech_rate = DS707_EXTIF_DATA_RATE_HDR_REVB_RL;
          ret_val = TRUE;
        }
        break;

#endif /* FEATURE_HDR*/
      default:
        {
          //ERROR MESSAGE
          ret_val = FALSE;
        }
    } /* End Switch */
  } /* End if */

  return ret_val;

} /* ds707_extif_get_bearer_tech_max_rate() */

/*===========================================================================
FUNCTION      DS707_EXTIF_IS_PKT_DATA_SESS_TRANFERABLE

DESCRIPTION   This function checks if the packet data session is tranferable. 
              On session change, HDR-CP queries this function, with the new
              system on which the session is negotiated. Since DS hasnt yet 
              processed the session change, the system info is what it was 
              before the session change. Depending on the new and old system
              this function returns true or false depending on whether the 
              session can be transfered or not

DEPENDENCIES  None.

RETURN VALUE  TRUE if packet data session is tranferable FALSE otherwise.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_extif_is_pkt_data_sess_transferable( 
  ds707_extif_system_info sys_info
)
{

  DATA_IS707_MSG2( MSG_LEGACY_MED, 
                   "_is_pkt_data_sess_transferable: Sys %d -> Sys %d",  
                   ds707_sys_chg_hdlr_get_current_system(),
                   sys_info);

  if ( ds707_sys_chg_hdlr_get_current_system() == DS707_SYS_INFO_EPC_SYSTEM )
  {
    if ( sys_info == DS707_EXTIF_SYSTEM_EPC )
    {
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }
  else if ( ds707_sys_chg_hdlr_get_current_system() == DS707_SYS_INFO_NON_EPC_SYSTEM )
  {
    if ( sys_info == DS707_EXTIF_SYSTEM_NON_EPC )
    {
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }
  else
  {
    return FALSE;
  }
}

