#ifndef PS_STAT_PPP_H
#define PS_STAT_PPP_H

/*===========================================================================

                          P S _ S T A T _ P P P . H

GENERAL DESCRIPTION
  This header file provides the structure, enum and function definitions for
  the statistics pertaining to PPP.

EXTERNALIZED FUNCTIONS
  ps_stat_get_ppp()
    This function supplies the required ppp stats.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/inc/ps_stat_ppp.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/13/13   cx     Fix security issue: third param of  strlcpy should be dst size 
07/04/11   ttv     Fixed the issue of incorrect display of ppp stats in QXDM.
02/27/07   scb     Added changes to fix RCVT compilation warning
02/21/07   scb     Replaced the INC/DEC macro implementation to use the
                   COMMON_INC/COMMON_DEC macro
08/15/05   kr      Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "ps_stat_common.h"
#include "ps_in.h"

#define PS_STAT_INST_DESC_PPP_MAX_DEV_NAME_LEN            7

/*--------------------------------------------------------------------------
  PPP - Instance statistics Only
---------------------------------------------------------------------------*/
typedef PACKED struct
{
  uint32  bad_fcs                     ;
  uint32  bad_proto                   ;
  uint32  unsupported_proto           ;
  uint32  not_enabled_proto           ;
  uint32  missing_addr                ;
  uint32  missing_ctrl                ;
  uint32  pkts_dropped_rx             ;
  uint32  pkts_rx                     ;
  uint64  framed_bytes_rx             ;
  uint64  unframed_bytes_rx           ;
  uint32  pkts_dropped_tx             ;
  uint32  pkts_tx                     ;
  uint64  framed_bytes_tx             ;
  uint64  unframed_bytes_tx           ;
  uint32  ipv4_pkts_rx                ;
  uint32  ipv4_pkts_tx                ;
  uint32  vjch_pkts_rx                ;
  uint32  vjch_pkts_tx                ;
  uint32  vjuch_pkts_rx               ;
  uint32  vjuch_pkts_tx               ;
  uint32  pap_pkts_rx                 ;
  uint32  pap_pkts_tx                 ;
  uint32  pap_auth_failures           ;
  uint32  chap_pkts_rx                ;
  uint32  chap_pkts_tx                ;
  uint32  chap_auth_failures          ;
  uint32  lcp_pkts_rx                 ;
  uint32  lcp_pkts_tx                 ;
  uint32  lcp_config_requests_rx      ;
  uint32  lcp_config_acks_rx          ;
  uint32  lcp_config_naks_rx          ;
  uint32  lcp_config_rejects_rx       ;
  uint32  lcp_term_requests_rx        ;
  uint32  lcp_term_acks_rx            ;
  uint32  lcp_code_rejects_rx         ;
  uint32  lcp_proto_rejects_rx        ;
  uint32  lcp_echo_requests_rx        ;
  uint32  lcp_echo_replies_rx         ;
  uint32  lcp_discard_requests_rx     ;
  uint32  lcp_identification_rx       ;
  uint32  lcp_time_remaining_rx       ;
  uint32  lcp_config_requests_tx      ;
  uint32  lcp_config_acks_tx          ;
  uint32  lcp_config_naks_tx          ;
  uint32  lcp_config_rejects_tx       ;
  uint32  lcp_term_requests_tx        ;
  uint32  lcp_term_acks_tx            ;
  uint32  lcp_code_rejects_tx         ;
  uint32  lcp_proto_rejects_tx        ;
  uint32  lcp_echo_requests_tx        ;
  uint32  lcp_echo_replies_tx         ;
  uint32  lcp_discard_requests_tx     ;
  uint32  ipcp_pkts_rx                ;
  uint32  ipcp_pkts_tx                ;
  uint32  ipcp_config_requests_rx     ;
  uint32  ipcp_config_acks_rx         ;
  uint32  ipcp_config_naks_rx         ;
  uint32  ipcp_config_rejects_rx      ;
  uint32  ipcp_term_requests_rx       ;
  uint32  ipcp_term_acks_rx           ;
  uint32  ipcp_code_rejects_rx        ;
  uint32  ipcp_config_requests_tx     ;
  uint32  ipcp_config_acks_tx         ;
  uint32  ipcp_config_naks_tx         ;
  uint32  ipcp_config_rejects_tx      ;
  uint32  ipcp_term_requests_tx       ;
  uint32  ipcp_term_acks_tx           ;
  uint32  ipcp_code_rejects_tx        ;
#if  defined (FEATURE_DATA_PS_IPV6)
  uint32  ipv6_pkts_rx                ;
  uint32  ipv6_pkts_tx                ;
  uint32  ipv6cp_pkts_rx              ;
  uint32  ipv6cp_pkts_tx              ;
  uint32  ipv6cp_config_requests_rx   ;
  uint32  ipv6cp_config_acks_rx       ;
  uint32  ipv6cp_config_naks_rx       ;
  uint32  ipv6cp_config_rejects_rx    ;
  uint32  ipv6cp_term_requests_rx     ;
  uint32  ipv6cp_term_acks_rx         ;
  uint32  ipv6cp_code_rejects_rx      ;
  uint32  ipv6cp_config_requests_tx   ;
  uint32  ipv6cp_config_acks_tx       ;
  uint32  ipv6cp_config_naks_tx       ;
  uint32  ipv6cp_config_rejects_tx    ;
  uint32  ipv6cp_term_requests_tx     ;
  uint32  ipv6cp_term_acks_tx         ;
  uint32  ipv6cp_code_rejects_tx      ;
#endif /* FEATURE_DATA_PS || FEATURE_DATA_PS_IPV6 */
} ps_stat_ppp_i_s_type;

/*---------------------------------------------------------------------------
  PPP statistics enum
---------------------------------------------------------------------------*/

typedef enum
{
  PS_STAT_PPP_BAD_FCS                   = 0,
  PS_STAT_PPP_BAD_PROTO                 = 1,
  PS_STAT_PPP_UNSUPPORTED_PROTO         = 2,
  PS_STAT_PPP_NOT_ENABLED_PROTO         = 3,
  PS_STAT_PPP_MISSING_ADDR              = 4,
  PS_STAT_PPP_MISSING_CTRL              = 5,
  PS_STAT_PPP_PKTS_DROPPED_RX           = 6,
  PS_STAT_PPP_PKTS_RX                   = 7,
  PS_STAT_PPP_FRAMED_BYTES_RX           = 8,
  PS_STAT_PPP_UNFRAMED_BYTES_RX         = 9,
  PS_STAT_PPP_PKTS_DROPPED_TX           = 10,
  PS_STAT_PPP_PKTS_TX                   = 11,
  PS_STAT_PPP_FRAMED_BYTES_TX           = 12,
  PS_STAT_PPP_UNFRAMED_BYTES_TX         = 13,
  PS_STAT_PPP_IPV4_PKTS_RX              = 14,
  PS_STAT_PPP_IPV4_PKTS_TX              = 15,
  PS_STAT_PPP_VJCH_PKTS_RX              = 16,
  PS_STAT_PPP_VJCH_PKTS_TX              = 17,
  PS_STAT_PPP_VJUCH_PKTS_RX             = 18,
  PS_STAT_PPP_VJUCH_PKTS_TX             = 19,
  PS_STAT_PPP_PAP_PKTS_RX               = 20,
  PS_STAT_PPP_PAP_PKTS_TX               = 21,
  PS_STAT_PPP_PAP_AUTH_FAILURES         = 22,
  PS_STAT_PPP_CHAP_PKTS_RX              = 23,
  PS_STAT_PPP_CHAP_PKTS_TX              = 24,
  PS_STAT_PPP_CHAP_AUTH_FAILURES        = 25,
  PS_STAT_PPP_LCP_PKTS_RX               = 26,
  PS_STAT_PPP_LCP_PKTS_TX               = 27,
  PS_STAT_PPP_LCP_CONFIG_REQUESTS_RX    = 28,
  PS_STAT_PPP_LCP_CONFIG_ACKS_RX        = 29,
  PS_STAT_PPP_LCP_CONFIG_NAKS_RX        = 30,
  PS_STAT_PPP_LCP_CONFIG_REJECTS_RX     = 31,
  PS_STAT_PPP_LCP_TERM_REQUESTS_RX      = 32,
  PS_STAT_PPP_LCP_TERM_ACKS_RX          = 33,
  PS_STAT_PPP_LCP_CODE_REJECTS_RX       = 34,
  PS_STAT_PPP_LCP_PROTO_REJECTS_RX      = 35,
  PS_STAT_PPP_LCP_ECHO_REQUESTS_RX      = 36,
  PS_STAT_PPP_LCP_ECHO_REPLIES_RX       = 37,
  PS_STAT_PPP_LCP_DISCARD_REQUESTS_RX   = 38,
  PS_STAT_PPP_LCP_CONFIG_REQUESTS_TX    = 39,
  PS_STAT_PPP_LCP_CONFIG_ACKS_TX        = 40,
  PS_STAT_PPP_LCP_CONFIG_NAKS_TX        = 41,
  PS_STAT_PPP_LCP_CONFIG_REJECTS_TX     = 42,
  PS_STAT_PPP_LCP_TERM_REQUESTS_TX      = 43,
  PS_STAT_PPP_LCP_TERM_ACKS_TX          = 44,
  PS_STAT_PPP_LCP_CODE_REJECTS_TX       = 45,
  PS_STAT_PPP_LCP_PROTO_REJECTS_TX      = 46,
  PS_STAT_PPP_LCP_ECHO_REQUESTS_TX      = 47,
  PS_STAT_PPP_LCP_ECHO_REPLIES_TX       = 48,
  PS_STAT_PPP_LCP_DISCARD_REQUESTS_TX   = 49,
  PS_STAT_PPP_LCP_IDENTIFICATION_RX     = 50,
  PS_STAT_PPP_LCP_TIME_REMAINING_RX     = 51,
  PS_STAT_PPP_IPCP_PKTS_RX              = 52,
  PS_STAT_PPP_IPCP_PKTS_TX              = 53,
  PS_STAT_PPP_IPCP_CONFIG_REQUESTS_RX   = 54,
  PS_STAT_PPP_IPCP_CONFIG_ACKS_RX       = 55,
  PS_STAT_PPP_IPCP_CONFIG_NAKS_RX       = 56,
  PS_STAT_PPP_IPCP_CONFIG_REJECTS_RX    = 57,
  PS_STAT_PPP_IPCP_TERM_REQUESTS_RX     = 58,
  PS_STAT_PPP_IPCP_TERM_ACKS_RX         = 59,
  PS_STAT_PPP_IPCP_CODE_REJECTS_RX      = 60,
  PS_STAT_PPP_IPCP_CONFIG_REQUESTS_TX   = 61,
  PS_STAT_PPP_IPCP_CONFIG_ACKS_TX       = 62,
  PS_STAT_PPP_IPCP_CONFIG_NAKS_TX       = 63,
  PS_STAT_PPP_IPCP_CONFIG_REJECTS_TX    = 64,
  PS_STAT_PPP_IPCP_TERM_REQUESTS_TX     = 65,
  PS_STAT_PPP_IPCP_TERM_ACKS_TX         = 66,
  PS_STAT_PPP_IPCP_CODE_REJECTS_TX      = 67,
#if  defined (FEATURE_DATA_PS_IPV6)
  PS_STAT_PPP_IPV6_PKTS_RX              = 68,
  PS_STAT_PPP_IPV6_PKTS_TX              = 69,
  PS_STAT_PPP_IPV6CP_PKTS_RX            = 70,
  PS_STAT_PPP_IPV6CP_PKTS_TX            = 71,
  PS_STAT_PPP_IPV6CP_CONFIG_REQUESTS_RX = 72,
  PS_STAT_PPP_IPV6CP_CONFIG_ACKS_RX     = 73,
  PS_STAT_PPP_IPV6CP_CONFIG_NAKS_RX     = 74,
  PS_STAT_PPP_IPV6CP_CONFIG_REJECTS_RX  = 75,
  PS_STAT_PPP_IPV6CP_TERM_REQUESTS_RX   = 76,
  PS_STAT_PPP_IPV6CP_TERM_ACKS_RX       = 77,
  PS_STAT_PPP_IPV6CP_CODE_REJECTS_RX    = 78,
  PS_STAT_PPP_IPV6CP_CONFIG_REQUESTS_TX = 79,
  PS_STAT_PPP_IPV6CP_CONFIG_ACKS_TX     = 80,
  PS_STAT_PPP_IPV6CP_CONFIG_NAKS_TX     = 81,
  PS_STAT_PPP_IPV6CP_CONFIG_REJECTS_TX  = 82,
  PS_STAT_PPP_IPV6CP_TERM_REQUESTS_TX   = 83,
  PS_STAT_PPP_IPV6CP_TERM_ACKS_TX       = 84,
  PS_STAT_PPP_IPV6CP_CODE_REJECTS_TX    = 85,
#endif /* FEATURE_DATA_PS || FEATURE_DATA_PS_IPV6 */
  PS_STAT_PPP_ALL
} ps_stat_ppp_enum_type;


/*===========================================================================

                          LOG PACKET DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Log structure for LOG_PS_STAT_PPP_INST_C

  Header defined in ps_stat_logging.h

  Fields are
    handle           : handle to the instance
    inst_ppp_stats   : instance PPP stats
---------------------------------------------------------------------------*/
typedef PACKED struct
{
  int32                     handle;
  ps_stat_ppp_i_s_type      inst_ppp_stats;
} ps_stat_inst_ppp_log_pkt_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_STAT_INST_DESC_PPP_TYPE

DESCRIPTION
  The structure describing a PPP instance

  handle          : Instance handle, index into the global ppp device array
  auth_mode       : Authentication mode negotiated (i.e. PAP, CHAP, ...)
  reserved        : Used to maintain a 4-byte alignment
  local_addr_v4   : IPv4 address of the local end of the connection
  remote_addr_v4  : IPv4 address of the remote end of the connection
  local_iid_v6    : IPv6 address of the local end of the connection
  remote_iid_v6   : IPv6 address of the remote end of the connection
  oui             : Vendor-specific OUI
  dev_name        : Name of the PPP device (i.e. Am, Un, ...)
---------------------------------------------------------------------------*/
typedef struct
{
  uint8             handle;
  uint8             auth_mode;
  char              dev_name[PS_STAT_INST_DESC_PPP_MAX_DEV_NAME_LEN];
  struct ps_in_addr local_addr_v4;
  struct ps_in_addr remote_addr_v4;
  uint64            local_iid_v6;
  uint64            remote_iid_v6;
  uint32            oui;
} ps_stat_inst_desc_ppp_type;


/*===========================================================================

                         EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
MACRO PPP_INC_INSTANCE_STATS(device,field, inc)

DESCRIPTION
  This macro increments the ppp statistic (field) by the necessary quantity
  (inc). It also locks the task before incrementing and frees the task after
  incrementing

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PPP_INC_INSTANCE_STATS(DEVICE, FIELD, INC)         \
    PS_COMMON_INC_STATS(ppp_cb_array[DEVICE].ppp_i_stats.FIELD, 4, INC)

/*===========================================================================
MACRO PPP_DEC_INSTANCE_STATS(device,field, inc)

DESCRIPTION
  This macro decrements the ppp statistic (field) by the necessary quantity
  (inc). It also locks the task before decrementing and frees the task after
  decrementing as needed.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PPP_DEC_INSTANCE_STATS(DEVICE, FIELD, DEC)         \
    PS_COMMON_DEC_STATS(ppp_cb_array[DEVICE].ppp_i_stats.FIELD, 4, DEC)

/*===========================================================================
MACRO PPP_INC_INSTANCE_STATS_PTR(PPPCBPTR,field, inc)

DESCRIPTION
  This macro increments the ppp statistic (field) by the necessary quantity
  (inc). It also locks the task before incrementing and frees the task after
  incrementing

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PPP_INC_INSTANCE_STATS_PTR(PPPCBPTR, FIELD, INC)  \
    PS_COMMON_INC_STATS(PPPCBPTR->ppp_i_stats.FIELD, 4, INC)

/*===========================================================================
MACRO PPP_INC_INSTANCE_FSM_STATS(device,fsmid,field, inc)

DESCRIPTION
  This macro increments the ppp statistic (field) by the necessary quantity
  (inc). It also locks the task before incrementing and frees the task after
  incrementing

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PPP_INC_INSTANCE_FSM_STATS(FSMPTR,FIELD, INC)      \
    PS_COMMON_INC_STATS(FSMPTR->ppp_i_fsm_stats.FIELD, 4, INC)

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
);

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
  instance_ptr    - IN - Handle to instance
  return_value    - IN - pointer for return value
                  - OUT- return value (statistics)
  ret_len         - length of memory allocated

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_ppp
(
  ps_stat_ppp_enum_type   stat,
  void                    *instance_ptr,
  void                    *return_value,
  uint16                   ret_len
);

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
  int32         handle
);

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
                  device.

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_ppp_control_block
(
  int     device ,
  void    **pppcb_address
) ;

#endif /* PS_STAT_PPP_H */
