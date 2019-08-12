/*===========================================================================

                          P S _ S T A T _ P P P . C

GENERAL DESCRIPTION
  This program specifically deals with statistics at the ppp layer of the
  data servcies stack

EXTERNALIZED FUNCTIONS
  ps_stat_get_ppp
    This function supplies the required ppp stats.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005-2014 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/src/ps_stat_ppp.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/23/14   ssb     Fixed KW errors
08/13/13   cx      Fix security issue: third param of  strlcpy should be dst size 
06/27/13   cx      Replace memcpy and memmove with memscpy and memsmove
03/28/12   sb      Fixed crash issue seen while getting PPP stats
03/23/11   op      Data SU API cleanup
12/27/10   rp      RAM optimization changes.
12/23/10   ss      Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
01/08/09   scb     Fixed QTF LINT errors.
11/10/08   scb     Fixed QTF compilation warnings.
04/11/07   scb     Fixed High LINT errors
02/27/07   scb     Added changes to fix RCVT compilation warning
08/15/05   kr      Initial version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "IxErrno.h"
#include "err.h"

#include "ps_stat_ppp.h"
#include "ps_stat_common.h"
#include "ps_stat_logging.h"
#include "ds_Utils_DebugMsg.h"

#include "ps_ppp_fsm.h"
#include "ps_ppp_defs.h"


#include <stringl/stringl.h>
#include <stddef.h>
#include <string.h>


/*===========================================================================

                             INTERNAL DECLARATIONS

===========================================================================*/

/*===========================================================================
MACRO PPP_STAT_OFFSET_SIZE(INDEX,FIELD,SIZE)

DESCRIPTION
  This macro fills the offset and size fields of the ps_stat_ppp_i_table.
  INDEX used only for readability, not for usage

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PPP_STAT_OFFSET_SIZE(INDEX,FIELD,SIZE)                              \
{																			\
  FPOS_STAT(ppp_type, FIELD),												\
  sizeof(SIZE)																\
}

/*===========================================================================
MACRO PPP_STAT_FSM_OFFSET_SIZE(INDEX,FSMID,FIELD,SIZE)

DESCRIPTION
  This macro fills the offset and size fields of the ps_stat_ppp_i_table.
  The field passed should correspond to a fsm stat (LCP,IPCP, PV6CP).
  INDEX used only for readability, not for usage

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PPP_STAT_FSM_OFFSET_SIZE(INDEX,FSMID,FIELD,SIZE)                    \
{																			\
  FPOS_STAT(ppp_type,fsm[FSMID].ppp_i_fsm_stats.FIELD), 					\
  sizeof(SIZE)																\
}

/*---------------------------------------------------------------------------
  Names of the PPP instances as shown in QXDM
---------------------------------------------------------------------------*/
static char *pppi_dev_name_arr[] = 
{
  "UM_SN",   /* Um Service Network instance      */
  "RM",      /* the RM instance                  */
  "UM_AN",   /* The Um AN instance               */
  "UW_FMC",
  "RM_2"     /* The Second RM instance           */
};

/*---------------------------------------------------------------------------
  PPP stats access table
-------------------------------------------------------------------------- */
static const ps_stat_access_table_s_type ps_stat_ppp_i_table[PS_STAT_PPP_ALL + 1] = 
{
  PPP_STAT_OFFSET_SIZE(0,ppp_i_stats.bad_fcs,						uint32),
  PPP_STAT_OFFSET_SIZE(1,ppp_i_stats.bad_proto, 					uint32),
  PPP_STAT_OFFSET_SIZE(2,ppp_i_stats.unsupported_proto, 			uint32),
  PPP_STAT_OFFSET_SIZE(3,ppp_i_stats.not_enabled_proto, 			uint32),
  PPP_STAT_OFFSET_SIZE(4,ppp_i_stats.missing_addr,					uint32),
  PPP_STAT_OFFSET_SIZE(5,ppp_i_stats.missing_ctrl,					uint32),
  PPP_STAT_OFFSET_SIZE(6,ppp_i_stats.pkts_dropped_rx,				uint32),
  PPP_STAT_OFFSET_SIZE(7,ppp_i_stats.pkts_rx,						uint32),
  PPP_STAT_OFFSET_SIZE(8,ppp_i_stats.framed_bytes_rx,				uint64),
  PPP_STAT_OFFSET_SIZE(9,ppp_i_stats.unframed_bytes_rx, 			uint64),
  PPP_STAT_OFFSET_SIZE(10,ppp_i_stats.pkts_dropped_tx,				uint32),
  PPP_STAT_OFFSET_SIZE(11,ppp_i_stats.pkts_tx,						uint32),
  PPP_STAT_OFFSET_SIZE(12,ppp_i_stats.framed_bytes_tx,				uint64),
  PPP_STAT_OFFSET_SIZE(13,ppp_i_stats.unframed_bytes_tx,			uint64),
  PPP_STAT_OFFSET_SIZE(14,ppp_i_stats.ipv4_pkts_rx, 				uint32),
  PPP_STAT_OFFSET_SIZE(15,ppp_i_stats.ipv4_pkts_tx, 				uint32),
  PPP_STAT_OFFSET_SIZE(16,ppp_i_stats.vjch_pkts_rx, 				uint32),
  PPP_STAT_OFFSET_SIZE(17,ppp_i_stats.vjch_pkts_tx, 				uint32),
  PPP_STAT_OFFSET_SIZE(18,ppp_i_stats.vjuch_pkts_rx,				uint32),
  PPP_STAT_OFFSET_SIZE(19,ppp_i_stats.vjuch_pkts_tx,				uint32),
  PPP_STAT_OFFSET_SIZE(20,ppp_i_stats.vjuch_pkts_tx,				uint32),
  PPP_STAT_OFFSET_SIZE(21,ppp_i_stats.pap_pkts_tx,					uint32),
  PPP_STAT_OFFSET_SIZE(22,ppp_i_stats.pap_auth_failures,			uint32),
  PPP_STAT_OFFSET_SIZE(23,ppp_i_stats.chap_pkts_rx, 				uint32),
  PPP_STAT_OFFSET_SIZE(24,ppp_i_stats.chap_pkts_tx, 				uint32),
  PPP_STAT_OFFSET_SIZE(25,ppp_i_stats.chap_auth_failures,			uint32),  
  PPP_STAT_FSM_OFFSET_SIZE(26,PPP_PROTOCOL_LCP, pkts_rx,			uint32),
  PPP_STAT_FSM_OFFSET_SIZE(27,PPP_PROTOCOL_LCP, pkts_tx,			uint32),
  PPP_STAT_FSM_OFFSET_SIZE(28,PPP_PROTOCOL_LCP, config_requests_rx, uint32),
  PPP_STAT_FSM_OFFSET_SIZE(29,PPP_PROTOCOL_LCP, config_acks_rx, 	uint32),
  PPP_STAT_FSM_OFFSET_SIZE(30,PPP_PROTOCOL_LCP, config_naks_rx, 	uint32),
  PPP_STAT_FSM_OFFSET_SIZE(31,PPP_PROTOCOL_LCP, config_rejects_rx,	uint32),
  PPP_STAT_FSM_OFFSET_SIZE(32,PPP_PROTOCOL_LCP, term_requests_rx,	uint32),
  PPP_STAT_FSM_OFFSET_SIZE(33,PPP_PROTOCOL_LCP, term_acks_rx,		uint32),
  PPP_STAT_FSM_OFFSET_SIZE(34,PPP_PROTOCOL_LCP, code_rejects_rx,	uint32),
  PPP_STAT_FSM_OFFSET_SIZE(35,PPP_PROTOCOL_LCP, proto_rejects_rx,	uint32),
  PPP_STAT_FSM_OFFSET_SIZE(36,PPP_PROTOCOL_LCP, echo_requests_rx,	uint32),
  PPP_STAT_FSM_OFFSET_SIZE(37,PPP_PROTOCOL_LCP, echo_replies_rx,	uint32),
  PPP_STAT_FSM_OFFSET_SIZE(38,PPP_PROTOCOL_LCP, discard_requests_rx,uint32),
  PPP_STAT_FSM_OFFSET_SIZE(39,PPP_PROTOCOL_LCP, identification_rx,	uint32),
  PPP_STAT_FSM_OFFSET_SIZE(40,PPP_PROTOCOL_LCP, time_remaining_rx,	uint32),
  PPP_STAT_FSM_OFFSET_SIZE(41,PPP_PROTOCOL_LCP, config_requests_tx, uint32),
  PPP_STAT_FSM_OFFSET_SIZE(42,PPP_PROTOCOL_LCP, config_acks_tx, 	uint32),
  PPP_STAT_FSM_OFFSET_SIZE(43,PPP_PROTOCOL_LCP, config_naks_tx, 	uint32),
  PPP_STAT_FSM_OFFSET_SIZE(44,PPP_PROTOCOL_LCP, config_rejects_tx,	uint32),
  PPP_STAT_FSM_OFFSET_SIZE(45,PPP_PROTOCOL_LCP, term_requests_tx,	uint32),
  PPP_STAT_FSM_OFFSET_SIZE(46,PPP_PROTOCOL_LCP, term_acks_tx,		uint32),
  PPP_STAT_FSM_OFFSET_SIZE(47,PPP_PROTOCOL_LCP, code_rejects_tx,	uint32),
  PPP_STAT_FSM_OFFSET_SIZE(48,PPP_PROTOCOL_LCP, proto_rejects_tx,	uint32),
  PPP_STAT_FSM_OFFSET_SIZE(49,PPP_PROTOCOL_LCP, echo_requests_tx,	uint32),
  PPP_STAT_FSM_OFFSET_SIZE(50,PPP_PROTOCOL_LCP, echo_replies_tx,	uint32),
  PPP_STAT_FSM_OFFSET_SIZE(51,PPP_PROTOCOL_LCP, discard_requests_tx, uint32),
  PPP_STAT_FSM_OFFSET_SIZE(52,PPP_PROTOCOL_LCP, pkts_rx,			 uint32),
  PPP_STAT_FSM_OFFSET_SIZE(53,PPP_PROTOCOL_LCP, pkts_tx,			 uint32),
  PPP_STAT_FSM_OFFSET_SIZE(54,PPP_PROTOCOL_LCP, config_requests_rx,  uint32),
  PPP_STAT_FSM_OFFSET_SIZE(55,PPP_PROTOCOL_LCP, config_acks_rx, 	 uint32),
  PPP_STAT_FSM_OFFSET_SIZE(56,PPP_PROTOCOL_LCP, config_naks_rx, 	 uint32),
  PPP_STAT_FSM_OFFSET_SIZE(57,PPP_PROTOCOL_LCP, config_rejects_rx,	 uint32),
  PPP_STAT_FSM_OFFSET_SIZE(58,PPP_PROTOCOL_LCP, term_requests_rx,	 uint32),
  PPP_STAT_FSM_OFFSET_SIZE(59,PPP_PROTOCOL_LCP, term_acks_rx,		 uint32),
  PPP_STAT_FSM_OFFSET_SIZE(60,PPP_PROTOCOL_LCP, code_rejects_rx,	 uint32),
  PPP_STAT_FSM_OFFSET_SIZE(61,PPP_PROTOCOL_LCP, config_requests_tx,  uint32),
  PPP_STAT_FSM_OFFSET_SIZE(62,PPP_PROTOCOL_LCP, config_acks_tx, 	 uint32),
  PPP_STAT_FSM_OFFSET_SIZE(63,PPP_PROTOCOL_LCP, config_naks_tx, 	 uint32),
  PPP_STAT_FSM_OFFSET_SIZE(64,PPP_PROTOCOL_LCP, config_rejects_tx,	 uint32),
  PPP_STAT_FSM_OFFSET_SIZE(65,PPP_PROTOCOL_LCP, term_requests_tx,	 uint32),
  PPP_STAT_FSM_OFFSET_SIZE(66,PPP_PROTOCOL_LCP, term_acks_tx,		 uint32),
  PPP_STAT_FSM_OFFSET_SIZE(67,PPP_PROTOCOL_LCP, code_rejects_tx,	 uint32),
#if  defined (FEATURE_DATA_PS_IPV6)
  PPP_STAT_OFFSET_SIZE(68,ppp_i_stats.ipv6_pkts_rx, 				 uint32),
  PPP_STAT_OFFSET_SIZE(69,ppp_i_stats.ipv6_pkts_tx, 				 uint32),
  PPP_STAT_FSM_OFFSET_SIZE(70,PPP_PROTOCOL_IPV6CP, pkts_rx, 		   uint32),
  PPP_STAT_FSM_OFFSET_SIZE(71,PPP_PROTOCOL_IPV6CP, pkts_tx, 		   uint32),
  PPP_STAT_FSM_OFFSET_SIZE(72,PPP_PROTOCOL_IPV6CP, config_requests_rx, uint32),
  PPP_STAT_FSM_OFFSET_SIZE(73,PPP_PROTOCOL_IPV6CP, config_acks_rx,	   uint32),
  PPP_STAT_FSM_OFFSET_SIZE(74,PPP_PROTOCOL_IPV6CP, config_naks_rx,	   uint32),
  PPP_STAT_FSM_OFFSET_SIZE(75,PPP_PROTOCOL_IPV6CP, config_rejects_rx,  uint32),
  PPP_STAT_FSM_OFFSET_SIZE(76,PPP_PROTOCOL_IPV6CP, term_requests_rx,   uint32),
  PPP_STAT_FSM_OFFSET_SIZE(77,PPP_PROTOCOL_IPV6CP, term_acks_rx,	   uint32),
  PPP_STAT_FSM_OFFSET_SIZE(78,PPP_PROTOCOL_IPV6CP, code_rejects_rx,    uint32),
  PPP_STAT_FSM_OFFSET_SIZE(79,PPP_PROTOCOL_IPV6CP, config_requests_tx, uint32),
  PPP_STAT_FSM_OFFSET_SIZE(80,PPP_PROTOCOL_IPV6CP, config_acks_tx,	   uint32),
  PPP_STAT_FSM_OFFSET_SIZE(81,PPP_PROTOCOL_IPV6CP, config_naks_tx,	   uint32),
  PPP_STAT_FSM_OFFSET_SIZE(82,PPP_PROTOCOL_IPV6CP, config_rejects_tx,  uint32),
  PPP_STAT_FSM_OFFSET_SIZE(83,PPP_PROTOCOL_IPV6CP, term_requests_tx,   uint32),
  PPP_STAT_FSM_OFFSET_SIZE(84,PPP_PROTOCOL_IPV6CP, term_acks_tx,	   uint32),
  PPP_STAT_FSM_OFFSET_SIZE(85,PPP_PROTOCOL_IPV6CP, code_rejects_tx,    uint32),
#else
  /*---------------------------------------------------------------------------
  Indices from 70 to 85 are unused if both FEATURE_DATA_PS and 
  FEATURE_DATA_PS_IPV6 not defined. Fill with zeros.
  Index 86 is unused. Fill with zeros
  - Refer ps_stat_ppp_enum_type
  -------------------------------------------------------------------------- */
  STAT_FILL_NULL(70),
  STAT_FILL_NULL(71),
  STAT_FILL_NULL(72),
  STAT_FILL_NULL(73),
  STAT_FILL_NULL(74),
  STAT_FILL_NULL(75),
  STAT_FILL_NULL(76),
  STAT_FILL_NULL(77),
  STAT_FILL_NULL(78),
  STAT_FILL_NULL(79),
  STAT_FILL_NULL(80),
  STAT_FILL_NULL(81),
  STAT_FILL_NULL(82),
  STAT_FILL_NULL(83),
  STAT_FILL_NULL(84),
  STAT_FILL_NULL(85),
#endif /* FEATURE_DATA_PS && FEATURE_DATA_PS_IPV6 */
  STAT_FILL_NULL(86)
};

/*===========================================================================

                             INTERNAL FORWARD DECLARATIONS

===========================================================================*/
static errno_enum_type ps_stat_get_ppp_instance_all
(
  ps_stat_ppp_enum_type  stat,
  ppp_type               *pppcb,
  void                   *return_value,
  uint16                 ret_len
);

static void ps_stat_get_ppp_inst_desc
(
  int32                        handle,
  void                       * log_pkt_ptr
);

static void ps_stat_get_ppp_desc_log_pkt
(
  int32               handle
);

static void ps_stat_get_ppp_stat_log_pkt
(
  int32               handle
);
/*===========================================================================
FUNCTION PS_STAT_GET_PPP_INSTANCE_ALL()

DESCRIPTION
  This function supplies all the PPP statistics for the instance handle
  supplied. This is needed because of the need to extract and format data
  available in the ppp control block

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  stat            - IN - type of stat
  pppcb           - IN - pointer that points to the PPP control block
  return_value    - IN - pointer for return value
                  - OUT- return value (statistics)
  ret_len         - length of memory allocated

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_ppp_instance_all
(
  ps_stat_ppp_enum_type  stat,
  ppp_type               *pppcb,
  void                   *return_value,
  uint16                 ret_len
)
{
   ps_stat_ppp_i_s_type *ppp_i_stats;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    validate 'stat'; only PS_STAT_PPP_ALL should be passed to this function
  -------------------------------------------------------------------------*/
  if (stat != PS_STAT_PPP_ALL || (NULL == pppcb) )
  {
    DATA_ERR_FATAL("ps_stat_get_ppp_instance_all:Null pppcb passed or Invalid stat value");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    return_value validation : should not be null
  -------------------------------------------------------------------------*/

  if (return_value == NULL)
  {
    LOG_MSG_ERROR_0("return_value should not be NULL ");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    check the sizeof the stat requested. If the size is zero, the stat is
    invalid in the current context. return appropriate error
  -------------------------------------------------------------------------*/

  if (ret_len < sizeof(ps_stat_ppp_i_s_type))
  {
    LOG_MSG_ERROR_0(" Insufficient memory allocated for ptr return_value");
    return E_NOT_ALLOWED ;
  }

  ppp_i_stats = (ps_stat_ppp_i_s_type *) return_value ;

  /*-------------------------------------------------------------------------
    Copy the stats from ppp control block
  -------------------------------------------------------------------------*/
  ppp_i_stats->bad_fcs            = pppcb->ppp_i_stats.bad_fcs               ;
  ppp_i_stats->bad_proto          = pppcb->ppp_i_stats.bad_proto             ;
  ppp_i_stats->unsupported_proto  = pppcb->ppp_i_stats.unsupported_proto     ;
  ppp_i_stats->not_enabled_proto  = pppcb->ppp_i_stats.not_enabled_proto     ;
  ppp_i_stats->missing_addr       = pppcb->ppp_i_stats.missing_addr          ;
  ppp_i_stats->missing_ctrl       = pppcb->ppp_i_stats.missing_ctrl          ;
  ppp_i_stats->pkts_dropped_rx    = pppcb->ppp_i_stats.pkts_dropped_rx       ;
  ppp_i_stats->pkts_rx            = pppcb->ppp_i_stats.pkts_rx               ;
  memscpy(&(ppp_i_stats->framed_bytes_rx), 
          sizeof(ppp_i_stats->framed_bytes_rx),
          &(pppcb->ppp_i_stats.framed_bytes_rx), 
                                          sizeof(ppp_i_stats->framed_bytes_rx));
  memscpy(&(ppp_i_stats->unframed_bytes_rx), 
          sizeof(ppp_i_stats->unframed_bytes_rx),
          &(pppcb->ppp_i_stats.unframed_bytes_rx), 
                                            sizeof(ppp_i_stats->unframed_bytes_rx));
  ppp_i_stats->pkts_dropped_tx    = pppcb->ppp_i_stats.pkts_dropped_tx       ;
  ppp_i_stats->pkts_tx            = pppcb->ppp_i_stats.pkts_tx               ;
  memscpy(&(ppp_i_stats->framed_bytes_tx), 
           sizeof(ppp_i_stats->framed_bytes_tx),
           &(pppcb->ppp_i_stats.framed_bytes_tx), 
                                          sizeof(ppp_i_stats->framed_bytes_tx));
  memscpy(&(ppp_i_stats->unframed_bytes_tx), 
           sizeof(ppp_i_stats->unframed_bytes_tx),
           &(pppcb->ppp_i_stats.unframed_bytes_tx), 
                                            sizeof(ppp_i_stats->unframed_bytes_tx));
  ppp_i_stats->ipv4_pkts_rx       = pppcb->ppp_i_stats.ipv4_pkts_rx          ;
  ppp_i_stats->ipv4_pkts_tx       = pppcb->ppp_i_stats.ipv4_pkts_tx          ;
  ppp_i_stats->vjch_pkts_rx       = pppcb->ppp_i_stats.vjch_pkts_rx          ;
  ppp_i_stats->vjch_pkts_tx       = pppcb->ppp_i_stats.vjch_pkts_tx          ;
  ppp_i_stats->vjuch_pkts_rx      = pppcb->ppp_i_stats.vjuch_pkts_rx         ;
  ppp_i_stats->vjuch_pkts_tx      = pppcb->ppp_i_stats.vjuch_pkts_tx         ;
  ppp_i_stats->pap_pkts_rx        = pppcb->ppp_i_stats.pap_pkts_rx           ;
  ppp_i_stats->pap_pkts_tx        = pppcb->ppp_i_stats.pap_pkts_tx           ;
  ppp_i_stats->pap_auth_failures  = pppcb->ppp_i_stats.pap_auth_failures     ;
  ppp_i_stats->chap_pkts_rx       = pppcb->ppp_i_stats.chap_pkts_rx          ;
  ppp_i_stats->chap_pkts_tx       = pppcb->ppp_i_stats.chap_pkts_tx          ;
  ppp_i_stats->chap_auth_failures = pppcb->ppp_i_stats.chap_auth_failures    ;
  ppp_i_stats->lcp_pkts_rx             =
            pppcb->fsm[PPP_PROTOCOL_LCP].ppp_i_fsm_stats.pkts_rx             ;
  ppp_i_stats->lcp_pkts_tx             =
            pppcb->fsm[PPP_PROTOCOL_LCP].ppp_i_fsm_stats.pkts_tx             ;
  ppp_i_stats->lcp_config_requests_rx  =
            pppcb->fsm[PPP_PROTOCOL_LCP].ppp_i_fsm_stats.config_requests_rx  ;
  ppp_i_stats->lcp_config_acks_rx      =
            pppcb->fsm[PPP_PROTOCOL_LCP].ppp_i_fsm_stats.config_acks_rx      ;
  ppp_i_stats->lcp_config_naks_rx      =
            pppcb->fsm[PPP_PROTOCOL_LCP].ppp_i_fsm_stats.config_naks_rx      ;
  ppp_i_stats->lcp_config_rejects_rx   =
            pppcb->fsm[PPP_PROTOCOL_LCP].ppp_i_fsm_stats.config_rejects_rx   ;
  ppp_i_stats->lcp_term_requests_rx    =
            pppcb->fsm[PPP_PROTOCOL_LCP].ppp_i_fsm_stats.term_requests_rx    ;
  ppp_i_stats->lcp_term_acks_rx        =
            pppcb->fsm[PPP_PROTOCOL_LCP].ppp_i_fsm_stats.term_acks_rx        ;
  ppp_i_stats->lcp_code_rejects_rx     =
            pppcb->fsm[PPP_PROTOCOL_LCP].ppp_i_fsm_stats.code_rejects_rx     ;
  ppp_i_stats->lcp_proto_rejects_rx    =
            pppcb->fsm[PPP_PROTOCOL_LCP].ppp_i_fsm_stats.proto_rejects_rx    ;
  ppp_i_stats->lcp_echo_requests_rx    =
            pppcb->fsm[PPP_PROTOCOL_LCP].ppp_i_fsm_stats.echo_requests_rx    ;
  ppp_i_stats->lcp_echo_replies_rx     =
            pppcb->fsm[PPP_PROTOCOL_LCP].ppp_i_fsm_stats.echo_replies_rx     ;
  ppp_i_stats->lcp_discard_requests_rx =
            pppcb->fsm[PPP_PROTOCOL_LCP].ppp_i_fsm_stats.discard_requests_rx ;
  ppp_i_stats->lcp_identification_rx =
            pppcb->fsm[PPP_PROTOCOL_LCP].ppp_i_fsm_stats.identification_rx ;
  ppp_i_stats->lcp_time_remaining_rx =
            pppcb->fsm[PPP_PROTOCOL_LCP].ppp_i_fsm_stats.time_remaining_rx ;
  ppp_i_stats->lcp_config_requests_tx  =
            pppcb->fsm[PPP_PROTOCOL_LCP].ppp_i_fsm_stats.config_requests_tx  ;
  ppp_i_stats->lcp_config_acks_tx      =
            pppcb->fsm[PPP_PROTOCOL_LCP].ppp_i_fsm_stats.config_acks_tx      ;
  ppp_i_stats->lcp_config_naks_tx      =
            pppcb->fsm[PPP_PROTOCOL_LCP].ppp_i_fsm_stats.config_naks_tx      ;
  ppp_i_stats->lcp_config_rejects_tx   =
            pppcb->fsm[PPP_PROTOCOL_LCP].ppp_i_fsm_stats.config_rejects_tx   ;
  ppp_i_stats->lcp_term_requests_tx    =
            pppcb->fsm[PPP_PROTOCOL_LCP].ppp_i_fsm_stats.term_requests_tx    ;
  ppp_i_stats->lcp_term_acks_tx        =
            pppcb->fsm[PPP_PROTOCOL_LCP].ppp_i_fsm_stats.term_acks_tx        ;
  ppp_i_stats->lcp_code_rejects_tx     =
            pppcb->fsm[PPP_PROTOCOL_LCP].ppp_i_fsm_stats.code_rejects_tx     ;
  ppp_i_stats->lcp_proto_rejects_tx    =
            pppcb->fsm[PPP_PROTOCOL_LCP].ppp_i_fsm_stats.proto_rejects_tx    ;
  ppp_i_stats->lcp_echo_requests_tx    =
            pppcb->fsm[PPP_PROTOCOL_LCP].ppp_i_fsm_stats.echo_requests_tx    ;
  ppp_i_stats->lcp_echo_replies_tx     =
            pppcb->fsm[PPP_PROTOCOL_LCP].ppp_i_fsm_stats.echo_replies_tx     ;
  ppp_i_stats->lcp_discard_requests_tx =
            pppcb->fsm[PPP_PROTOCOL_LCP].ppp_i_fsm_stats.discard_requests_tx ;

  ppp_i_stats->ipcp_pkts_rx             =
            pppcb->fsm[PPP_PROTOCOL_IPCP].ppp_i_fsm_stats.pkts_rx            ;
  ppp_i_stats->ipcp_pkts_tx             =
            pppcb->fsm[PPP_PROTOCOL_IPCP].ppp_i_fsm_stats.pkts_tx            ;
  ppp_i_stats->ipcp_config_requests_rx  =
            pppcb->fsm[PPP_PROTOCOL_IPCP].ppp_i_fsm_stats.config_requests_rx ;
  ppp_i_stats->ipcp_config_acks_rx      =
            pppcb->fsm[PPP_PROTOCOL_IPCP].ppp_i_fsm_stats.config_acks_rx     ;
  ppp_i_stats->ipcp_config_naks_rx      =
            pppcb->fsm[PPP_PROTOCOL_IPCP].ppp_i_fsm_stats.config_naks_rx     ;
  ppp_i_stats->ipcp_config_rejects_rx   =
            pppcb->fsm[PPP_PROTOCOL_IPCP].ppp_i_fsm_stats.config_rejects_rx  ;
  ppp_i_stats->ipcp_term_requests_rx    =
            pppcb->fsm[PPP_PROTOCOL_IPCP].ppp_i_fsm_stats.term_requests_rx   ;
  ppp_i_stats->ipcp_term_acks_rx        =
            pppcb->fsm[PPP_PROTOCOL_IPCP].ppp_i_fsm_stats.term_acks_rx       ;
  ppp_i_stats->ipcp_code_rejects_rx     =
            pppcb->fsm[PPP_PROTOCOL_IPCP].ppp_i_fsm_stats.code_rejects_rx    ;
  ppp_i_stats->ipcp_config_requests_tx  =
            pppcb->fsm[PPP_PROTOCOL_IPCP].ppp_i_fsm_stats.config_requests_tx ;
  ppp_i_stats->ipcp_config_acks_tx      =
            pppcb->fsm[PPP_PROTOCOL_IPCP].ppp_i_fsm_stats.config_acks_tx     ;
  ppp_i_stats->ipcp_config_naks_tx      =
            pppcb->fsm[PPP_PROTOCOL_IPCP].ppp_i_fsm_stats.config_naks_tx     ;
  ppp_i_stats->ipcp_config_rejects_tx   =
            pppcb->fsm[PPP_PROTOCOL_IPCP].ppp_i_fsm_stats.config_rejects_tx  ;
  ppp_i_stats->ipcp_term_requests_tx    =
            pppcb->fsm[PPP_PROTOCOL_IPCP].ppp_i_fsm_stats.term_requests_tx   ;
  ppp_i_stats->ipcp_term_acks_tx        =
            pppcb->fsm[PPP_PROTOCOL_IPCP].ppp_i_fsm_stats.term_acks_tx       ;
  ppp_i_stats->ipcp_code_rejects_tx     =
            pppcb->fsm[PPP_PROTOCOL_IPCP].ppp_i_fsm_stats.code_rejects_tx    ;

#if  defined (FEATURE_DATA_PS_IPV6)
  ppp_i_stats->ipv6_pkts_rx       = pppcb->ppp_i_stats.ipv6_pkts_rx          ;
  ppp_i_stats->ipv6_pkts_tx       = pppcb->ppp_i_stats.ipv6_pkts_tx          ;
  ppp_i_stats->ipv6cp_pkts_rx             =
            pppcb->fsm[PPP_PROTOCOL_IPV6CP].ppp_i_fsm_stats.pkts_rx          ;
  ppp_i_stats->ipv6cp_pkts_tx             =
            pppcb->fsm[PPP_PROTOCOL_IPV6CP].ppp_i_fsm_stats.pkts_tx          ;
  ppp_i_stats->ipv6cp_config_requests_rx  =
          pppcb->fsm[PPP_PROTOCOL_IPV6CP].ppp_i_fsm_stats.config_requests_rx ;
  ppp_i_stats->ipv6cp_config_acks_rx      =
            pppcb->fsm[PPP_PROTOCOL_IPV6CP].ppp_i_fsm_stats.config_acks_rx   ;
  ppp_i_stats->ipv6cp_config_naks_rx      =
            pppcb->fsm[PPP_PROTOCOL_IPV6CP].ppp_i_fsm_stats.config_naks_rx   ;
  ppp_i_stats->ipv6cp_config_rejects_rx   =
            pppcb->fsm[PPP_PROTOCOL_IPV6CP].ppp_i_fsm_stats.config_rejects_rx;
  ppp_i_stats->ipv6cp_term_requests_rx    =
            pppcb->fsm[PPP_PROTOCOL_IPV6CP].ppp_i_fsm_stats.term_requests_rx ;
  ppp_i_stats->ipv6cp_term_acks_rx        =
            pppcb->fsm[PPP_PROTOCOL_IPV6CP].ppp_i_fsm_stats.term_acks_rx     ;
  ppp_i_stats->ipv6cp_code_rejects_rx     =
            pppcb->fsm[PPP_PROTOCOL_IPV6CP].ppp_i_fsm_stats.code_rejects_rx  ;
  ppp_i_stats->ipv6cp_config_requests_tx  =
          pppcb->fsm[PPP_PROTOCOL_IPV6CP].ppp_i_fsm_stats.config_requests_tx ;
  ppp_i_stats->ipv6cp_config_acks_tx      =
            pppcb->fsm[PPP_PROTOCOL_IPV6CP].ppp_i_fsm_stats.config_acks_tx   ;
  ppp_i_stats->ipv6cp_config_naks_tx      =
            pppcb->fsm[PPP_PROTOCOL_IPV6CP].ppp_i_fsm_stats.config_naks_tx   ;
  ppp_i_stats->ipv6cp_config_rejects_tx   =
            pppcb->fsm[PPP_PROTOCOL_IPV6CP].ppp_i_fsm_stats.config_rejects_tx;
  ppp_i_stats->ipv6cp_term_requests_tx    =
            pppcb->fsm[PPP_PROTOCOL_IPV6CP].ppp_i_fsm_stats.term_requests_tx ;
  ppp_i_stats->ipv6cp_term_acks_tx        =
            pppcb->fsm[PPP_PROTOCOL_IPV6CP].ppp_i_fsm_stats.term_acks_tx     ;
  ppp_i_stats->ipv6cp_code_rejects_tx     =
            pppcb->fsm[PPP_PROTOCOL_IPV6CP].ppp_i_fsm_stats.code_rejects_tx  ;
#endif /* FEATURE_DATA_PS || FEATURE_DATA_PS_IPV6 */

  pppcb->ppp_i_stats.queried = PS_STAT_QUERIED;

  return E_SUCCESS ;

} /* ps_stat_get_ppp_instance_all */


/*===========================================================================
FUNCTION PS_STAT_FILL_PPP_INST_DESC()

DESCRIPTION
  This function fills the description of one PPP instance into a log packet.

PARAMETERS
  handle          : Specifies which instance to retrieve the description from
  *log_pkt_ptr    : Pointer to the log packet that the description will be 
                    copied to

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_get_ppp_inst_desc
(
  int32                        handle,
  void                       * log_pkt_ptr
)
{
  ppp_type                    * ppp_ptr;
  ppp_type                    * ppp_cb_array = NULL;
  ps_stat_inst_desc_ppp_type  * ppp_desc_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Sanity check the handle value
  -------------------------------------------------------------------------*/
  if ( ( (int32)PPP_MAX_DEV <= handle) || ((int32)PPP_MIN_DEV > handle) )
  {
    LOG_MSG_ERROR_1("Invalid handle to PPP instance.  Handle: %d", handle);
    return;
  }

  /*-------------------------------------------------------------------------
    Convert the handle into the instance control block
  -------------------------------------------------------------------------*/
  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_ptr = &ppp_cb_array[handle];

  /*-------------------------------------------------------------------------
    PPP instance has not been initialized yet
  -------------------------------------------------------------------------*/

  ppp_desc_ptr = (ps_stat_inst_desc_ppp_type *)log_pkt_ptr ;

  ppp_desc_ptr->handle          = (uint8)handle;
  ppp_desc_ptr->auth_mode       = ppp_ptr->auth.mode;
  ppp_desc_ptr->local_addr_v4   =
          ((ipcp_value_type *)(ppp_ptr->fsm[IPCP].local.work_pdv))->address;
  ppp_desc_ptr->remote_addr_v4  = 
          ((ipcp_value_type *)(ppp_ptr->fsm[IPCP].remote.work_pdv))->address;

#ifdef FEATURE_DATA_PS_IPV6
  ppp_desc_ptr->local_iid_v6   = 
          ((ipv6cp_value_type *)ppp_ptr->fsm[IPV6CP].local.work_pdv)->iid;
  ppp_desc_ptr->remote_iid_v6  = 
          ((ipv6cp_value_type *)ppp_ptr->fsm[IPV6CP].remote.work_pdv)->iid;
#endif /* FEATURE_DATA_PS_IPV6 */

  ppp_desc_ptr->oui             = ppp_ptr->vendor_cb_array->oui;

  (void)strlcpy(ppp_desc_ptr->dev_name,
              pppi_dev_name_arr[handle],
              PS_STAT_INST_DESC_PPP_MAX_DEV_NAME_LEN);

} /* ps_stat_get_ppp_inst_desc */


/*===========================================================================
FUNCTIOn PS_STAT_GET_PPP_DESC_LOG_PKT()

DESCRIPTION
  This function fills the description of the specified instance handle into
  a log packet.  If the handle matches the handle for all descriptions, it
  copies all of the active instance descriptions into the log packet.  If the
  handle does not specify all instances but specifies a valid active instance,
  the description associated with that instance is copied into the log packet.
  If the handle is invalid, nothing is done.

PARAMETERS
  handle          : Specifies which instance to retrieve the description from

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_get_ppp_desc_log_pkt
(
  int32               handle
)
{
  uint8               i                  = 0;
  int32               desc_size          = 0;
  uint8               inst_count         = 0;
  int32               log_pkt_avail_size = 0;
  uint8             * log_pkt_ptr        = NULL;
  uint8             * log_pkt_fill_ptr   = NULL;

  ps_stat_desc_all_inst_hdr_log_pkt_type     * desc_all_inst_pkt_ptr = NULL;
  ps_stat_desc_delta_inst_hdr_log_pkt_type   * delta_pkt_ptr         = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (PS_STAT_INST_DESC_ALL == handle)
  {
    /*-----------------------------------------------------------------------
      Iterate through all PPP devices.  PPP_MIN_DEV is the same value as 
      the first device
    -----------------------------------------------------------------------*/
    for (i = (uint8)PPP_MIN_DEV; i < (uint8)PPP_MAX_DEV; ++i)
    {
      desc_size = sizeof(ps_stat_inst_desc_ppp_type);

      /*-------------------------------------------------------------------
        Insufficient amount of space to fit this description in the log
        packet, so commit the existing log
      -------------------------------------------------------------------*/
      if (log_pkt_avail_size < desc_size)
      {
        if (NULL != log_pkt_ptr)
        {
          desc_all_inst_pkt_ptr = 
            (ps_stat_desc_all_inst_hdr_log_pkt_type *) log_pkt_ptr;

          desc_all_inst_pkt_ptr->count = inst_count;
          memset(desc_all_inst_pkt_ptr->reserved, 0, 3);

          ps_stat_logging_commit_log_pkt
          (
            log_pkt_ptr,
            (PS_STAT_MAX_LOG_PKT_SIZE - log_pkt_avail_size)
          );
        }

        inst_count         = 0;
        log_pkt_avail_size = PS_STAT_MAX_LOG_PKT_SIZE;
        log_pkt_ptr        = NULL;
        log_pkt_fill_ptr   = NULL;

        /*-------------------------------------------------------------------
          Get the pointer to the log packet
        -------------------------------------------------------------------*/
        ps_stat_logging_get_log_pkt(log_pkt_avail_size,
                                    LOG_PS_STAT_DESC_ALL_PPP_INST_C,
                                    &log_pkt_ptr);

        /*-------------------------------------------------------------------
          If we fail to get a log packet, just return
        -------------------------------------------------------------------*/
        if (NULL == log_pkt_ptr)
        {
          LOG_MSG_ERROR_0("Unable to allocate log packet");
          return;
        }

        /*-------------------------------------------------------------------
          Leave space for the describe all instance header
        -------------------------------------------------------------------*/
        log_pkt_fill_ptr =
          (log_pkt_ptr + 
            (uint8) sizeof(ps_stat_desc_all_inst_hdr_log_pkt_type));

        log_pkt_avail_size -= sizeof(ps_stat_desc_all_inst_hdr_log_pkt_type);
      }

      ps_stat_get_ppp_inst_desc(i, log_pkt_fill_ptr);

      /*-------------------------------------------------------------------
        Increment the fill pointer by the size, and decrement the count
        by the same value
      -------------------------------------------------------------------*/
      log_pkt_fill_ptr += (uint8) desc_size;
      log_pkt_avail_size -= desc_size;
      inst_count++;
    } /* for all ppp instances*/

    /*-----------------------------------------------------------------------
      Dispatch the last packet
    -----------------------------------------------------------------------*/
    if (NULL != log_pkt_ptr)
    {
      desc_all_inst_pkt_ptr = 
        (ps_stat_desc_all_inst_hdr_log_pkt_type *) log_pkt_ptr;

      desc_all_inst_pkt_ptr->count = inst_count;
      memset(desc_all_inst_pkt_ptr->reserved, 0, 3);

      ps_stat_logging_commit_log_pkt
      (
        log_pkt_ptr,
        (PS_STAT_MAX_LOG_PKT_SIZE - log_pkt_avail_size)
      );
    }
  } /* if fill all ppp instances */
  else
  {
    desc_size = sizeof(ps_stat_inst_desc_ppp_type);

    /*-----------------------------------------------------------------------
      We need to allocate enough memory for the description and the delta
      header
    -----------------------------------------------------------------------*/
    log_pkt_avail_size =
             (desc_size + sizeof(ps_stat_desc_delta_inst_hdr_log_pkt_type));

    ps_stat_logging_get_log_pkt(log_pkt_avail_size,
                                LOG_PS_STAT_DESC_DELTA_INST_C,
                                &log_pkt_ptr);

    /*-----------------------------------------------------------------------
      If we fail to get a log packet, just return
    -----------------------------------------------------------------------*/
    if (NULL == log_pkt_ptr)
    {
      LOG_MSG_ERROR_0("Unable to allocate log packet");
      return;
    }

    /*-----------------------------------------------------------------------
      Fill in the delta log parameters
    -----------------------------------------------------------------------*/
    delta_pkt_ptr = (ps_stat_desc_delta_inst_hdr_log_pkt_type *) log_pkt_ptr;

    delta_pkt_ptr->type    = (uint8)PS_STAT_MODULE_INST_PPP;
    delta_pkt_ptr->deleted = FALSE;
    memset(delta_pkt_ptr->reserved, 0, 2);

    log_pkt_fill_ptr =
    (log_pkt_ptr + (uint8) sizeof(ps_stat_desc_delta_inst_hdr_log_pkt_type));
  
    ps_stat_get_ppp_inst_desc(handle, log_pkt_fill_ptr);

    /*-----------------------------------------------------------------------
      Dispatch the packet
    -----------------------------------------------------------------------*/
    ps_stat_logging_commit_log_pkt
    (
      log_pkt_ptr,
      (desc_size + sizeof(ps_stat_desc_delta_inst_hdr_log_pkt_type))
    );
  } /* else fill one ppp instance */
} /* ps_stat_get_ppp_desc_log_pkt */


/*===========================================================================
FUNCTION PS_STAT_GET_PPP_STAT_LOG_PKT()

DESCRIPTION
  This function fills the statistics of the specified instance handle into
  a log packet.  If the handle matches the handle for all statistics, it
  copies all of the active instance statistics into the log packet.  If the
  handle does not specify all instances but specifies a valid active instance,
  the statistics associated with that instance is copied into the log packet.
  If the handle is invalid, nothing is done.

PARAMETERS
  handle          : Specifies which instance to retrieve the stats from

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_get_ppp_stat_log_pkt
(
  int32               handle
)
{
  uint8                               i                    = 0;
  ppp_type                          * ppp_ptr              = NULL;
  ppp_type                          * ppp_cb_array         = NULL;
  uint8                             * log_pkt_ptr          = NULL;
  ps_stat_inst_ppp_log_pkt_type     * inst_ppp_log_pkt_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ppp_cb_array = fsm_get_ppp_cb_array();
  if (PS_STAT_INST_DESC_ALL == handle)
  {
    /*-----------------------------------------------------------------------
      Iterate through all PPP devices.  PPP_MIN_DEV is the same value as 
      the first device
    -----------------------------------------------------------------------*/
    for (i = (uint8)PPP_MIN_DEV; i < (uint8)PPP_MAX_DEV; ++i)
    {
      /*---------------------------------------------------------------------
        Convert the handle into the instance control block
      ---------------------------------------------------------------------*/
      ppp_ptr = &ppp_cb_array[i];

      ps_stat_logging_get_log_pkt(sizeof(ps_stat_inst_ppp_log_pkt_type),
                                  LOG_PS_STAT_PPP_INST_C,
                                  &log_pkt_ptr);
      
      if (NULL ==log_pkt_ptr)
      {
        LOG_MSG_ERROR_0("Log packet allocation for PPP failed");
        return;
      }

      inst_ppp_log_pkt_ptr = (ps_stat_inst_ppp_log_pkt_type *) log_pkt_ptr;

      inst_ppp_log_pkt_ptr->handle  = i;

      (void) ps_stat_get_ppp(PS_STAT_PPP_ALL,
                              (void *) ppp_ptr,
                              &(inst_ppp_log_pkt_ptr->inst_ppp_stats),
                              sizeof(ps_stat_ppp_i_s_type));

      ps_stat_logging_commit_log_pkt
      (
        log_pkt_ptr,
        sizeof(ps_stat_inst_ppp_log_pkt_type)
      );

    } /* for all ppp instances*/
  } /* if fill all ppp instances */
  else
  {
    if( ( (int32)PPP_MAX_DEV <= handle) || ((int32)PPP_MIN_DEV > handle) )
    {
      LOG_MSG_ERROR_1("Invalid handle to PPP instance.  Handle: %d", handle);
      return;
    }
    /*---------------------------------------------------------------------
      Convert the handle into the instance control block
    ---------------------------------------------------------------------*/
    ppp_ptr = &ppp_cb_array[handle];


    ps_stat_logging_get_log_pkt(sizeof(ps_stat_inst_ppp_log_pkt_type),
                                LOG_PS_STAT_PPP_INST_C,
                                &log_pkt_ptr);

    if (NULL == log_pkt_ptr)
    {
      LOG_MSG_ERROR_0("Log packet allocation for PPP failed");
      return;
    }

    inst_ppp_log_pkt_ptr = (ps_stat_inst_ppp_log_pkt_type *) log_pkt_ptr;
    inst_ppp_log_pkt_ptr->handle  = handle;

    (void) ps_stat_get_ppp(PS_STAT_PPP_ALL,
                            (void *) ppp_ptr,
                            &(inst_ppp_log_pkt_ptr->inst_ppp_stats),
                            sizeof(ps_stat_ppp_i_s_type));

    ps_stat_logging_commit_log_pkt
    (
      log_pkt_ptr,
      sizeof(ps_stat_inst_ppp_log_pkt_type)
    );
  } /* else fill one ppp instance */
} /* ps_stat_get_ppp_stat_log_pkt */


/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION PS_STAT_RESET_PPP()

DESCRIPTION
  This function resets the instance statistics associated with the specified
  instance.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  handle : Handle to the instance

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_reset_ppp
(
  int32              handle
)
{
  ppp_type         * ppp_cb_ptr;
  ppp_type         * ppp_cb_array = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ( (int32)PPP_MAX_DEV <= handle) || ((int32)PPP_MIN_DEV > handle) )
  {
    LOG_MSG_ERROR_1("Invalid handle to PPP instance.  Handle: %d", handle);
    return;
  }

  ppp_cb_array = fsm_get_ppp_cb_array();
  ppp_cb_ptr = &ppp_cb_array[handle];

  if (PS_STAT_QUERIED == ppp_cb_ptr->ppp_i_stats.queried)
  {
    ps_stat_get_ppp_stat_log_pkt(handle);
  };

  /*-------------------------------------------------------------------------
    Need to clear the PPP instance stats, as well as all of the FSM
    statistics as well
  -------------------------------------------------------------------------*/
  memset(&(ppp_cb_ptr->ppp_i_stats), 0, sizeof(ppp_cb_ptr->ppp_i_stats));

  memset(&(ppp_cb_ptr->fsm[PPP_PROTOCOL_LCP].ppp_i_fsm_stats),
         0,
         sizeof(ppp_cb_ptr->fsm[PPP_PROTOCOL_LCP].ppp_i_fsm_stats));

  memset(&(ppp_cb_ptr->fsm[PPP_PROTOCOL_IPCP].ppp_i_fsm_stats), 
         0, 
         sizeof(ppp_cb_ptr->fsm[PPP_PROTOCOL_IPCP].ppp_i_fsm_stats));

#ifdef FEATURE_DATA_PS_IPV6
  memset(&(ppp_cb_ptr->fsm[PPP_PROTOCOL_IPV6CP].ppp_i_fsm_stats), 
         0, 
         sizeof(ppp_cb_ptr->fsm[PPP_PROTOCOL_IPV6CP].ppp_i_fsm_stats));
#endif /* FEATURE_DATA_PS_IPV6 */

  return;
} /* ps_stat_reset_ppp */


/*===========================================================================
FUNCTION PS_STAT_INIT_PPP()

DESCRIPTION

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_init_ppp
(
  void
)
{
  ps_stat_logging_inst_reg_fill_desc_cback(PS_STAT_MODULE_INST_PPP,
                                           ps_stat_get_ppp_desc_log_pkt);

  ps_stat_logging_inst_reg_get_stat_cback(PS_STAT_MODULE_INST_PPP,
                                          ps_stat_get_ppp_stat_log_pkt);

  ps_stat_logging_inst_reg_reset_stat_cback(PS_STAT_MODULE_INST_PPP,
                                            ps_stat_reset_ppp);

} /* ps_stat_init_ppp */


/*===========================================================================
FUNCTION PS_STAT_GET_PPP()

DESCRIPTION

  This function supplies the required ppp layer statistics. The first
  argument passed to the function is an enum that denotes the metric of
  interest.The second argument will be  a pointer to thecontrol block of the
  necessary PPP device. The third argument points to the memory location where
  the results will be copied.The result ( statistic) will be copied to this
  memory address. The fourth argument passes the length of memory allocated
  for the return value and should be greater than or equal to the memory size
  of the statistic requested.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  stat            - IN - type of stat
  instance_ptr    - IN - pointer that has the value of socket descriptor
  return_value    - IN - pointer for return value
                  - OUT- return value (statistics)
  ret_len         - length of memory allocated

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_ppp
(
  ps_stat_ppp_enum_type  stat,
  void                   *instance_ptr,
  void                   *return_value,
  uint16                 ret_len
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check Instance pointer; Instance pointer should not be null
  -------------------------------------------------------------------------*/

  if (instance_ptr == NULL)
  {
    LOG_MSG_ERROR_0(" Instance pointer should NOT be NULL for PPP stats");
    return E_INVALID_ARG ;
  }

  if (stat == PS_STAT_PPP_ALL)
  {
     return ps_stat_get_ppp_instance_all(stat,
                                         instance_ptr,
                                         return_value,
                                         ret_len
                                        ) ;
  }
  else
  {
    return ps_stat_get_common( (uint32)stat,
                               ps_stat_ppp_i_table,
                               (uint16)PS_STAT_PPP_ALL,
                               instance_ptr,
                               return_value,
                               ret_len
                               ) ;
  }

} /* ps_stat_get_ppp */

/*===========================================================================
FUNCTION PS_STAT_GET_PPP_CONTROL_BLOCK()

DESCRIPTION
  This function supplies the address of the PPP control block corresponding
  the PPP device passed.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  device        - IN -PPP device
  pppcb_address - OUT - address of the ppp control block corresponding to the
                  device. This is a void ** so that a void* value can be
                  returned back that can be used to call ps_stat_get_ppp

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_ppp_control_block
(
  int     device ,
  void    **pppcb_address
)
{
  ppp_type *ppp_cb_array = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (device >= (int)PPP_MAX_DEV || device < (int)PPP_MIN_DEV)
  {
    LOG_MSG_ERROR_0(" Invalid device number passed");
    return E_INVALID_ARG ;
  }

  if (pppcb_address == NULL)
  {
    return E_INVALID_ARG;
  }

  /*-------------------------------------------------------------------------
    Copy the control block address
  -------------------------------------------------------------------------*/
  ppp_cb_array = fsm_get_ppp_cb_array();
  *pppcb_address = (void *) (&ppp_cb_array[device]) ;
  return E_SUCCESS ;
}

