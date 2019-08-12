/*===========================================================================

                        D S M I P _ S T A T S

GENERAL DESCRIPTION
  This file contains the functionality for the MIP interface to QXDM/DIAG.
  This implements the commands a user can issue from QXDM to the mobile, 
  which are specific to C2K data.

EXTERNALIZED FUNCTIONS
 dsmip_stats_init()
   Registers the functions in this file with DIAG.
   
INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

 Copyright (c) 2011-2013 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/mip/src/dsmip_stats.c#1 $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/13   cx      Replace memcpy and memmove with memscpy and memsmove
11/02/12   ssb     Updating MIP to SIP Fallback Stats for Socket/DUN calls
05/19/11   sn      Removed memset of response buffer in each sub command.
05/01/11   ttv     First version of MIP statistics file.
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

#ifdef FEATURE_DS_MOBILE_IP
#include "dsmip_stats.h"
#include "diagcmd.h"
#include "dsrsp.h"
#include "data_msg.h"
#include "dsmip_v.h"
#include "dsmip_solsm.h"
#include "dsmip_parse.h"
#include "dsmip_regsm.h"

#ifdef FEATURE_DATA_IS707
#include "ds707_pkt_mgr.h"
#endif /* FEATURE_DATA_IS707 */

/*===========================================================================
                              FORWARD DECLARATIONS
===========================================================================*/
PACKED void* dsmip_diag_get_ma_info
(
  PACKED void* req_pkt,
  uint16       pkt_len
);

PACKED void* dsmip_diag_get_mip_stats
(
  PACKED void* req_pkt,
  uint16       pkt_len
);

/*===========================================================================
                                VARIABLES
===========================================================================*/
static const diagpkt_user_table_entry_type  dsmip_stats_tbl[] = 
{
  {
    DSMIP_DIAG_MA_INFO,         /* Lower bound sub-command code          */
    DSMIP_DIAG_MA_INFO,         /* Upper bound sub-command code          */
    dsmip_diag_get_ma_info
  },
  {
    DSMIP_DIAG_MIP_STATS,       /* Lower bound sub-command code          */
    DSMIP_DIAG_MIP_STATS,       /* Upper bound sub-command code          */
    dsmip_diag_get_mip_stats
  }
};


/*===========================================================================
                           INTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DSMIP_DIAG_GET_MIP_STATS

DESCRIPTION   Retrieves the MIP statistics collected per power cycle.
              
DEPENDENCIES  None

RETURN VALUE
  void* - Pointer to response buffer which has MIP statistics filled.

SIDE EFFECTS  None
===========================================================================*/
/* argused */
PACKED void* dsmip_diag_get_mip_stats
(
  PACKED void* req_pkt,
  uint16       pkt_len
)
{
  dsmip_diag_mip_stats_rsp_type *rsp_ptr = NULL;
  const int                     rsp_len = sizeof(dsmip_diag_mip_stats_rsp_type);
  mip_session_info_type         *mip_session_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Allocate response buffer
  -------------------------------------------------------------------------*/
  rsp_ptr = (dsmip_diag_mip_stats_rsp_type*)
            diagpkt_subsys_alloc((diagpkt_subsys_id_type )DIAG_SUBSYS_DATA1X,
            DSMIP_DIAG_MIP_STATS,
            rsp_len);
  if (rsp_ptr == NULL)
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Response buffer allocation failed.");
    return NULL;
  }

  rsp_ptr->pkt_ver = 3;
  rsp_ptr->num_sub_pkts = 1;
  rsp_ptr->reserved = 0;

  rsp_ptr->sub_pkt_mip_stats.sub_pkt_id   = 
	           DSMIP_DIAG_MIP_STATS_SUB_PKT_MIP_STATS;
  rsp_ptr->sub_pkt_mip_stats.sub_pkt_ver  = 1;
  rsp_ptr->sub_pkt_mip_stats.sub_pkt_size = 
               sizeof(dsmip_diag_mip_stats_params_type);

  rsp_ptr->sub_pkt_mip_stats.mip_stats_params.rrp_cnt = 
                                 mip_get_mip_stat_info(RRP_CNT_ID);
  rsp_ptr->sub_pkt_mip_stats.mip_stats_params.rrq_cnt = 
                                 mip_get_mip_stat_info(RRQ_CNT_ID);
  rsp_ptr->sub_pkt_mip_stats.mip_stats_params.sol_cnt = 
                                 mip_get_mip_stat_info(SOL_CNT_ID);
  rsp_ptr->sub_pkt_mip_stats.mip_stats_params.adv_cnt = 
                                 mip_get_mip_stat_info(ADV_CNT_ID);
  rsp_ptr->sub_pkt_mip_stats.mip_stats_params.mip_to_sip_cnt = 
                                 ds707_pkt_mgr_get_num_mip_to_sip_transitions();

  mip_session_info = mip_get_ses_info_array();
  rsp_ptr->sub_pkt_mip_stats.mip_stats_params.mip_rrp_code = 
                                 mip_session_info->mip_rrp_code;

  return rsp_ptr;
} /* dsmip_diag_get_mip_stats() */

/*===========================================================================
FUNCTION      DSMIP_DIAG_GET_MA_INFO

DESCRIPTION   Retrieves the MA info.
              
DEPENDENCIES  None

RETURN VALUE
  void* - Pointer to response buffer which has MA information filled.

SIDE EFFECTS  None
===========================================================================*/
/* argused */
PACKED void* dsmip_diag_get_ma_info
(
  PACKED void* req_pkt,
  uint16       pkt_len
)
{
  dsmip_diag_ma_info_rsp_type *rsp_ptr = NULL;
  const int                   rsp_len = sizeof(dsmip_diag_ma_info_rsp_type);
  mip_ma_info_type            *ma_info;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Allocate response buffer
  -------------------------------------------------------------------------*/
    rsp_ptr = (dsmip_diag_ma_info_rsp_type*)
              diagpkt_subsys_alloc((diagpkt_subsys_id_type )DIAG_SUBSYS_DATA1X,
              DSMIP_DIAG_MA_INFO,
              rsp_len);
  if( rsp_ptr == NULL )
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Response buffer allocation failed.");
    return NULL;
  }
  
  ma_info = mip_get_ma_info_ptr();

  rsp_ptr->pkt_ver = 3;
  rsp_ptr->num_sub_pkts = 1;
  rsp_ptr->reserved = 0;

  rsp_ptr->sub_pkt_ma_info.sub_pkt_id = DSMIP_DIAG_MA_INFO_SUB_PKT_MA_INFO;
  rsp_ptr->sub_pkt_ma_info.sub_pkt_ver = 1;
  rsp_ptr->sub_pkt_ma_info.sub_pkt_size = 
               sizeof(dsmip_diag_ma_info_params_type);

  rsp_ptr->sub_pkt_ma_info.ma_info_params.ma_addr = ma_info->ma_addr;
  rsp_ptr->sub_pkt_ma_info.ma_info_params.ma_lifetime = ma_info->ma_lifetime;
  rsp_ptr->sub_pkt_ma_info.ma_info_params.last_seq_num = ma_info->last_seq_num;
  memscpy(rsp_ptr->sub_pkt_ma_info.ma_info_params.mn_AAA_SPI, 
          sizeof(rsp_ptr->sub_pkt_ma_info.ma_info_params.mn_AAA_SPI),
          ma_info->mn_AAA_SPI, 
          sizeof(rsp_ptr->sub_pkt_ma_info.ma_info_params.mn_AAA_SPI));
  rsp_ptr->sub_pkt_ma_info.ma_info_params.ma_supp_rev_tun = ma_info->ma_supp_rev_tun;
  rsp_ptr->sub_pkt_ma_info.ma_info_params.ma_prefix_len = ma_info->ma_prefix_len;

  return rsp_ptr;
} /* dsmip_diag_get_ma_info() */

/*===========================================================================
                           EXTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DSMIP_DIAG_INIT

DESCRIPTION   Registers functions with DIAG.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
#ifdef __cplusplus
  DIAGPKT_DISPATCH_AUTOREGISTER(DIAG_SUBSYS_DATA1X, dsmip_stats_tbl);
#else

void dsmip_stats_init(void)
{
  DIAGPKT_DISPATCH_TABLE_REGISTER((word)DIAG_SUBSYS_DATA1X, dsmip_stats_tbl);
} /* dsmip_stats_init() */
#endif /* __cplusplus */

#endif /* FEATURE_DS_MOBILE_IP */

#ifdef __cplusplus
  }
#endif /* __cplusplus */

