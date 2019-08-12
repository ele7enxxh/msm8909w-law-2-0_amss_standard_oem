/*===========================================================================

                        D S 7 0 7 _ D I A G 

GENERAL DESCRIPTION
  This file contains the functionality for the IS-707 interface to QXDM/DIAG.
  This implements the commands a user can issue from QXDM to the mobile, 
  which are specific to 1X data.

EXTERNALIZED FUNCTIONS
 ds707_diag_init()
   Registers the functions in this file with DIAG.
   
INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

 Copyright (c) 2009 - 2013 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_diag.c#1 $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/03/13   sd      ATCoP decoupling changes
05/19/11   sn      Removed memset of response buffer in each sub command.
05/04/11   ttv     Made changes to compute 707 statistics for diag.
03/15/11   mg      Global variable cleanup
10/13/10   op      Migrated to MSG 2.0 macros
02/17/10   ps      Fixed KlocWork errors.
03/03/09   mv      Off-target lint fixes
08/16/07   spn     Lint changes.
04/22/04   ak      Lint changes.
11/13/03   ak      Compiler fix.
07/14/03   vas     Commented out #error temporarily for HDR. Based on 
                   ds707_diag.c#1        
05/01/03   ak      First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#ifdef __cplusplus
  extern "C" {
#endif

#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "ds707_diag.h"
#include "ds707_pkt_mgr.h"
#include "diagcmd.h"
#include "dsrlp_v.h"
#include "dsrsp.h"
#include "data_msg.h"
#include "dsati.h"
#include "dsatapi.h"

#ifdef FEATURE_IS95B_MDR
  #include "mdrrlp.h"
#else
  #include "rlp.h"
#endif


/*===========================================================================
                              FORWARD DECLARATIONS
===========================================================================*/
PACKED void* ds707_diagi_get_rlp_stats
(
  PACKED void* req_pkt,
  uint16       pkt_len
);

PACKED void* ds707_diagi_get_rlp_stats_ext
(
  PACKED void* req_pkt,
  uint16       pkt_len
);

PACKED void* ds707_diagi_get_707_stats
(
  PACKED void* req_pkt,
  uint16       pkt_len
);

/*===========================================================================
                                VARIABLES
===========================================================================*/

static const diagpkt_user_table_entry_type  ds707_diag_tbl[] = 
{
  {
    DS707_DIAG_RLP_STATS,         /* Lower bound sub-command code          */
    DS707_DIAG_RLP_STATS,         /* Upper bound sub-command code          */
    ds707_diagi_get_rlp_stats
  },
  {
    DS707_DIAG_RLP_STATS_EXT,     /* Lower bound sub-command code          */
    DS707_DIAG_RLP_STATS_EXT,     /* Upper bound sub-command code          */
    ds707_diagi_get_rlp_stats_ext
  },
  {
    DS707_DIAG_707_STATS,         /* Lower bound sub-command code          */
    DS707_DIAG_707_STATS,         /* Upper bound sub-command code          */
    ds707_diagi_get_707_stats
  }
};


/*===========================================================================
                           INTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_DIAGI_GET_RLP_STATS

DESCRIPTION   Retrieves the RLP statistics for RLP I/II and III.  Also 
              returns the last pkt_so that was used.
              
              Note that the function definition below is spec'd by DIAG.

DEPENDENCIES  None

RETURN VALUE
  void* - Pointer to response buffer which has RLP statistics filled.

SIDE EFFECTS  None
===========================================================================*/
/* argused */
PACKED void* ds707_diagi_get_rlp_stats
(
  PACKED void* req_pkt,
  uint16       pkt_len
)
{
  rlp_ext_stats_type        rlp_stats;
  dsrlp_stats_info_type    *rlp3_stats_ptr = NULL;
  boolean                   ret_val = FALSE;

  DIAG_SUBSYS_DATA1X_DS707_DIAG_RLP_STATS_rsp_type  *rsp_ptr;

  const int rsp_len = 
                    sizeof(DIAG_SUBSYS_DATA1X_DS707_DIAG_RLP_STATS_rsp_type);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Allocate response buffer
  -------------------------------------------------------------------------*/
  rsp_ptr = (DIAG_SUBSYS_DATA1X_DS707_DIAG_RLP_STATS_rsp_type*)
            diagpkt_subsys_alloc((diagpkt_subsys_id_type )DIAG_SUBSYS_DATA1X,
            DS707_DIAG_RLP_STATS,
            rsp_len);
  if (rsp_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Response buffer allocation failed.");
    return NULL;
  }
   
  /*-------------------------------------------------------------------------
    Allocate response buffer
  -------------------------------------------------------------------------*/
  rsp_ptr->last_pkt_so   = ds707_pkt_get_prev_call_so();

  /*-------------------------------------------------------------------------
  Retrieve RLP 3 statistics for RLP I/II.  Note that these could be weird
  if there was an aysnc data call beforehand.
  -------------------------------------------------------------------------*/
  rlp_get_stats(&rlp_stats);
  rsp_ptr->rlp_rx_bytes  = rlp_stats.rx_data_cnt;
  rsp_ptr->rlp_tx_bytes  = rlp_stats.tx_data_cnt;

  /*-------------------------------------------------------------------------
    Retrieve RLP 3 statistics for 1X.
  -------------------------------------------------------------------------*/
  ret_val = dsrlp_get_stats_ex(DSRLP_FIXED_SR_ID_INDEX,
                            DSRSP_ROUTE_A,
                            &rlp3_stats_ptr);

  if ((ret_val == TRUE) && (rlp3_stats_ptr != NULL))
  {
    rsp_ptr->rlp3_rx_bytes = rlp3_stats_ptr->rx_total_bytes;
    rsp_ptr->rlp3_tx_bytes = rlp3_stats_ptr->tx_total_bytes;
  }
  else
  {
    rsp_ptr->rlp3_rx_bytes = 0;
    rsp_ptr->rlp3_tx_bytes = 0;
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Could not retrieve RLP 3 stats");
  }

#ifdef FEATURE_HDR
  /*-------------------------------------------------------------------------
    Retrieve HDR RLP's statistics. Need to implement for HDR
  -------------------------------------------------------------------------*/
  rsp_ptr->rlp_hdr_SN_rx_bytes = 0;
  rsp_ptr->rlp_hdr_SN_tx_bytes = 0;
  rsp_ptr->rlp_hdr_AN_rx_bytes = 0;
  rsp_ptr->rlp_hdr_AN_tx_bytes = 0;
#else
  rsp_ptr->rlp_hdr_SN_rx_bytes = 0;
  rsp_ptr->rlp_hdr_SN_tx_bytes = 0;
  rsp_ptr->rlp_hdr_AN_rx_bytes = 0;
  rsp_ptr->rlp_hdr_AN_tx_bytes = 0;
#endif

  return(rsp_ptr);
} /* ds707_diagi_get_rlp_stats() */

/*===========================================================================
FUNCTION      DS707_DIAGI_GET_RLP_STATS_EXT

DESCRIPTION   Retrieves the RLP statistics for RLP I/II and III.  Also 
              returns the last pkt_so that was used.
              This is extended version of ds707_diagi_get_rlp_stats, where
              we provide only few RLP parameters statistics. This extended 
              function provides statistics for all RLP parameters.

DEPENDENCIES  None

RETURN VALUE
  void* - Pointer to response buffer which has RLP statistics filled.

SIDE EFFECTS  None
===========================================================================*/
/*argused*/
PACKED void* ds707_diagi_get_rlp_stats_ext
(
  PACKED void* req_pkt,
  uint16       pkt_len
)
{
  rlp_ext_stats_type                 rlp_stats;
  dsrlp_stats_info_type              *rlp3_stats_ptr = NULL;
  boolean                            ret_val = FALSE;
  ds707_diag_rlp_stats_ext_rsp_type  *rsp_ptr = NULL;
  const int  rsp_len = sizeof(ds707_diag_rlp_stats_ext_rsp_type);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Allocate response buffer
  -------------------------------------------------------------------------*/
  rsp_ptr = (ds707_diag_rlp_stats_ext_rsp_type*)
            diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_DATA1X,
                                 DS707_DIAG_RLP_STATS_EXT,
                                 rsp_len);
  if (rsp_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Response buffer allocation failed.");
    return NULL;
  }

  rsp_ptr->pkt_ver = 3;
  rsp_ptr->num_sub_pkts = 1;
  rsp_ptr->reserved = 0;

  rsp_ptr->sub_pkt_rlp_stats.sub_pkt_id = 
               DS707_DIAG_RLP_STATS_EXT_SUB_PKT_RLP_STATS;
  rsp_ptr->sub_pkt_rlp_stats.sub_pkt_ver = 1;
  rsp_ptr->sub_pkt_rlp_stats.sub_pkt_size = 
               sizeof(ds707_diag_rlp_stats_ext_params_type);

  /*-------------------------------------------------------------------------
    Allocate response buffer
  -------------------------------------------------------------------------*/
  rsp_ptr->sub_pkt_rlp_stats.rlp_params.last_pkt_so = 
                                 ds707_pkt_get_prev_call_so();

  /*-------------------------------------------------------------------------
  Retrieve RLP 3 statistics for RLP I/II.  Note that these could be weird
  if there was an aysnc data call beforehand.
  -------------------------------------------------------------------------*/
  rlp_get_stats(&rlp_stats);
  rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp_rx_bytes  = rlp_stats.rx_data_cnt;
  rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp_tx_bytes  = rlp_stats.tx_data_cnt;

#ifdef FEATURE_HDR
  /*-------------------------------------------------------------------------
    Retrieve HDR RLP's statistics. Need to implement for HDR
  -------------------------------------------------------------------------*/
  rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp_hdr_SN_rx_bytes = 0;
  rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp_hdr_SN_tx_bytes = 0;
  rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp_hdr_AN_rx_bytes = 0;
  rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp_hdr_AN_tx_bytes = 0;
#else
  rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp_hdr_SN_rx_bytes = 0;
  rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp_hdr_SN_tx_bytes = 0;
  rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp_hdr_AN_rx_bytes = 0;
  rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp_hdr_AN_tx_bytes = 0;
#endif

  /*-------------------------------------------------------------------------
    Retrieve RLP 3 statistics for 1X.
  -------------------------------------------------------------------------*/
  ret_val = dsrlp_get_stats_ex(DSRLP_FIXED_SR_ID_INDEX,
                            DSRSP_ROUTE_A,
                            &rlp3_stats_ptr);

  if ((ret_val == TRUE) && (rlp3_stats_ptr != NULL))
  {
    rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp3_at_reset_request_count = 
                                   rlp3_stats_ptr->at_reset_request_count;
    rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp3_an_reset_ack_count = 
                                   rlp3_stats_ptr->an_reset_ack_count;
    rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp3_an_reset_request_count = 
                                   rlp3_stats_ptr->an_reset_request_count;
    rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp3_rx_dup_fr_cnt = 
                                   rlp3_stats_ptr->rx_dup_fr_cnt;
    rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp3_rx_rexmit_fr_cnt = 
                                   rlp3_stats_ptr->rx_rexmit_fr_cnt;
    rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp3_rx_new_data_fr_cnt = 
                                   rlp3_stats_ptr->rx_new_data_fr_cnt;
    rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp3_rx_bytes = 
                                   rlp3_stats_ptr->rx_total_bytes;
    rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp3_tx_bytes = 
                                   rlp3_stats_ptr->tx_total_bytes;
    rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp3_tx_rexmit_fr_cnt = 
                                   rlp3_stats_ptr->tx_rexmit_fr_cnt;
    rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp3_tx_new_data_fr_cnt = 
                                   rlp3_stats_ptr->tx_new_data_fr_cnt;
    rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp3_rx_idle_fr_cnt = 
                                   rlp3_stats_ptr->rx_idle_fr_cnt;
    rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp3_rx_fill_fr_cnt = 
                                   rlp3_stats_ptr->rx_fill_fr_cnt;
    rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp3_rx_blank_fr_cnt = 
                                   rlp3_stats_ptr->rx_blank_fr_cnt;
    rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp3_rx_null_fr_cnt = 
                                   rlp3_stats_ptr->rx_null_fr_cnt;
    rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp3_rx_20ms_fr_cnt = 
                                   rlp3_stats_ptr->rx_20ms_fr_cnt;
    rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp3_tx_idle_fr_cnt = 
                                   rlp3_stats_ptr->tx_idle_fr_cnt;
    rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp3_tx_20ms_fr_cnt = 
                                   rlp3_stats_ptr->tx_20ms_fr_cnt;
    rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp3_naks_received = 
                                   rlp3_stats_ptr->naks_received;
    rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp3_rx_rlp_erasures = 
                                   rlp3_stats_ptr->rx_rlp_erasures;
    rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp3_rx_mux_erasures = 
                                   rlp3_stats_ptr->rx_mux_erasures;
    rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp3_resets = 
                                   rlp3_stats_ptr->resets;
    rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp3_state = 
                                   rlp3_stats_ptr->state;
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Could not retrieve RLP 3 stats");
  }

#ifdef FEATURE_HDR
  /*-------------------------------------------------------------------------
    Retrieve HDR RLP's statistics. Need to implement for HDR
  -------------------------------------------------------------------------*/
  rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp_hdr_SN_rx_bytes = 0;
  rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp_hdr_SN_tx_bytes = 0;
  rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp_hdr_AN_rx_bytes = 0;
  rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp_hdr_AN_tx_bytes = 0;
#else
  rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp_hdr_SN_rx_bytes = 0;
  rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp_hdr_SN_tx_bytes = 0;
  rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp_hdr_AN_rx_bytes = 0;
  rsp_ptr->sub_pkt_rlp_stats.rlp_params.rlp_hdr_AN_tx_bytes = 0;
#endif

  return(rsp_ptr);
} /* ds707_diagi_get_rlp_stats_ext() */

/*===========================================================================
FUNCTION      DS707_DIAGI_GET_707_STATS

DESCRIPTION   Retrieves the 707 statistics. Currently only CTA timer value is 
              supported.

DEPENDENCIES  None

RETURN VALUE
  void* - Pointer to response buffer which has 707 statistics filled.

SIDE EFFECTS  None
===========================================================================*/
/* argused */
PACKED void* ds707_diagi_get_707_stats
(
  PACKED void* req_pkt,
  uint16       pkt_len
)
{
  ds707_diag_707_stats_rsp_type  *rsp_ptr = NULL;
  const int  rsp_len  = sizeof(ds707_diag_707_stats_rsp_type);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Allocate response buffer
  -------------------------------------------------------------------------*/
  rsp_ptr = (ds707_diag_707_stats_rsp_type*)
            diagpkt_subsys_alloc( (diagpkt_subsys_id_type )DIAG_SUBSYS_DATA1X,
                                  DS707_DIAG_707_STATS,
                                  rsp_len 
                                 );
  if (rsp_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Response buffer allocation failed.");
    return NULL;
  }

  rsp_ptr->pkt_ver = 3;
  rsp_ptr->num_sub_pkts = 1;
  rsp_ptr->reserved = 0;

  rsp_ptr->sub_pkt_707_stats.sub_pkt_id = DS707_DIAG_707_STATS_SUB_PKT_CTA_VAL;
  rsp_ptr->sub_pkt_707_stats.sub_pkt_ver = 1;
  rsp_ptr->sub_pkt_707_stats.sub_pkt_size = sizeof(dword);
  (void) ds707_pkt_mgr_get_1x_profile_val(DS707_CTA_VAL,
                        (void *)&(rsp_ptr->sub_pkt_707_stats.ds707_cta_val),
                        DS707_NON_ARRAY);

  return rsp_ptr;
} /* ds707_diagi_get_707_stats */

/*===========================================================================
                           EXTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_DIAG_INIT

DESCRIPTION   Registers functions with DIAG.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
#ifdef __cplusplus
  DIAGPKT_DISPATCH_AUTOREGISTER(DIAG_SUBSYS_DATA1X, ds707_diag_tbl);
#else

  void ds707_diag_init(void)
  {
    DIAGPKT_DISPATCH_TABLE_REGISTER((word)DIAG_SUBSYS_DATA1X, ds707_diag_tbl);
  } /* ds707_diag_init() */

#endif /* __cplusplus */

#endif /* FEATURE_DATA_IS707 */

#ifdef __cplusplus
  }
#endif

