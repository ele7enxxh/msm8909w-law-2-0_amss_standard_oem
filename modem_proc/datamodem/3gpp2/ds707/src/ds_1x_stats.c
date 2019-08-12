
/*===========================================================================

                        D A T A   S E R V I C E S
                           1 X S T A T S

DESCRIPTION
  This file contains  definitions and functions corresponding to 1x data rate
  reporting to be used by the RMNET Connection manager. 
  called from the PS_IFACE_IOCTL_GET_DATA_BEARER_RATE ioctl Hdlr.

EXTERNALIZED FUNCTIONS
KVD : Add the externalized functions.
  None
Copyright (c) 2005-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds_1x_stats.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/18/11   vpk     HDR SU API cleanup
03/15/11   mg      Global variable cleanup
03/25/10   spn     Give bearer tech indications when we are not in call
03/04/10   spn     Updated the Data Bearer Indications
12/15/09   ls      Added Rev-B bearer tech and statistics support
10/22/08   cs      Compiler warning fix.
08/16/07   spn     Lint changes.
07/25/06   ksu     EMPA: 1x RLP code review comments
08/25/05   kvd     Initial version of file.

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707

#include "ds707.h"
#include "dsrlp_v.h"

#include "ps_iface_defs.h"
#include "ds707_extif.h"
#include "ds707_p_rev.h"
#include "ds707_pkt_mgr.h"
#include "ds_1x_stats.h"
#include "dsat707extctab.h"
#include "ds707_sec_pkt_mgr_util.h"
#include "hdrcom_api.h"
#include "data_msg.h"
#include "ds707_sys_chg_hdlr.h"
#ifdef FEATURE_EHRPD
#include "ds707_sec_pkt_mgr_util.h"
#endif


/*===========================================================================
                   FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION: ds_1x_get_bearer_data_stats

DESCRIPTION:
  Update Data Call Statistics in  stats structure passed.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

===========================================================================*/

boolean ds_1x_get_bearer_data_stats
(
  ps_iface_ioctl_data_bearer_rate *data_stats
)
{
  static dsrlp_stats_info_type *dsrlp_stats_ptr;
  byte                         rscb_index = DSRLP_FIXED_SR_ID_INDEX;

  sys_sys_mode_e_type sys_mode = SYS_SYS_MODE_NONE;
  ds707_extif_bearer_tech_rate_type bearer_tech_query; 
  boolean ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  data_stats->current_tx_bearer_data_rate = -1;
  data_stats->current_rx_bearer_data_rate = -1;
  data_stats->avg_tx_bearer_data_rate = -1;
  data_stats->avg_rx_bearer_data_rate = -1;
  data_stats->max_tx_bearer_data_rate = -1;
  data_stats->max_rx_bearer_data_rate = -1;

  /* If phone did not acquire signal or has lost signal send no service indication
   * to the application
   */
  if (ds707_get_has_phone_acquired_signal() == FALSE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "get_bearer_stats ioctl: Phone has not yet acquired signal");
    return ret_val;
  }


  /*-------------------------------------------------------------------------
    Update  avg data rate for current traffic
  -------------------------------------------------------------------------*/
  sys_mode = ds707_pkt_mgr_get_current_sys_mode();
  
  if (sys_mode == SYS_SYS_MODE_CDMA)
  {
    rscb_index = DSRLP_FIXED_SR_ID_INDEX;
  }
#ifdef FEATURE_HDR
  else if (sys_mode == SYS_SYS_MODE_HDR)
  {
    rscb_index = DSRLP_HDR_SN_RLP_SR_ID_INDEX;
  }
#endif /* FEATURE_HDR */
  if(dsrlp_get_stats_ex(rscb_index,
                     DSRSP_ROUTE_A,
                     &dsrlp_stats_ptr))
  {
    data_stats->avg_rx_bearer_data_rate = 
                    (uint32)dsrlp_stats_ptr->rx_throuput;
    data_stats->avg_tx_bearer_data_rate = 
                    (uint32)dsrlp_stats_ptr->tx_throuput;

    ret_val = TRUE;                    
  }
  /*-------------------------------------------------------------------------
    Update max rate.
  -------------------------------------------------------------------------*/

  if (sys_mode == SYS_SYS_MODE_CDMA)
  {
    bearer_tech_query.bearer_tech = DS707_EXTIF_1X;
    /* Not using the return value, as we aer passing in a proper value */
    ret_val = ds707_extif_get_bearer_tech_max_rate ( &bearer_tech_query );
    data_stats->max_rx_bearer_data_rate = bearer_tech_query.max_rx_bearer_tech_rate;
    data_stats->max_tx_bearer_data_rate = bearer_tech_query.max_tx_bearer_tech_rate;
  }
#ifdef FEATURE_HDR
  else if (sys_mode == SYS_SYS_MODE_HDR)
  {
    sys_active_prot_e_type hdr_protocol = 
      ds707_sys_chg_hdlr_get_active_hdr_protocol();
    if ( hdr_protocol == SYS_ACTIVE_PROT_HDR_RELA )
    {
      bearer_tech_query.bearer_tech = DS707_EXTIF_HDR_REVA;
      /* Not using the return value, as we are passing in a proper value */
      ret_val = ds707_extif_get_bearer_tech_max_rate ( &bearer_tech_query );
      data_stats->max_rx_bearer_data_rate = bearer_tech_query.max_rx_bearer_tech_rate;
      data_stats->max_tx_bearer_data_rate = bearer_tech_query.max_tx_bearer_tech_rate;
    }
    else if ( hdr_protocol == SYS_ACTIVE_PROT_HDR_RELB )
    {
      bearer_tech_query.bearer_tech = DS707_EXTIF_HDR_REVB;
      /* Not using the return value, as we are passing in a proper value */
      ret_val = ds707_extif_get_bearer_tech_max_rate ( &bearer_tech_query );
      data_stats->max_rx_bearer_data_rate = bearer_tech_query.max_rx_bearer_tech_rate;
      data_stats->max_tx_bearer_data_rate = bearer_tech_query.max_tx_bearer_tech_rate;
    }
    else
    {
      bearer_tech_query.bearer_tech = DS707_EXTIF_HDR_REV0;
      /* Not using the return value, as we are passing in a proper value */
      ret_val = ds707_extif_get_bearer_tech_max_rate ( &bearer_tech_query );
      data_stats->max_rx_bearer_data_rate = bearer_tech_query.max_rx_bearer_tech_rate;
      data_stats->max_tx_bearer_data_rate = bearer_tech_query.max_tx_bearer_tech_rate;
    }
  }
#endif /* FEATURE_HDR */

  return ret_val;
} /* ds_1x_get_bearer_data_stats() */

#endif /* FEATURE_DATA_IS707 */

