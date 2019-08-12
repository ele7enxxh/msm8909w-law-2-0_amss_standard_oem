#ifndef PS_STAT_ARP_H
#define PS_STAT_ARP_H
/*===========================================================================

                          P S _ S T A T _ A R P . H

GENERAL DESCRIPTION
  This header file provides the structure, enum and function definitions
  for the statistics pertaining to Address Resolution Protocol.

EXTERNALIZED FUNCTIONS
  ps_stat_get_arp()
    This function supplies the required ARP stats.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when    who     what, where, why
----------  ---     ---------------------------------------------------------
02/27/07    scb     Added changes to fix RCVT compilation warning
02/21/07    scb     Replaced the INC macro implementation to use the
                    COMMON_INC macro
08/18/06    scb     Updated enum values to start with 0 for calculating
                    correct offset.
06/22/2006  scb     Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#include "ps_stat_common.h"


/*---------------------------------------------------------------------------
  ARP Statistics - Instance stats only.
---------------------------------------------------------------------------*/
typedef struct
{
  uint32 ipv4_req_tx;
  uint32 ipv4_drop_new_req_tx;
  uint32 ipv4_drop_ce_flush;
  uint32 pkts_tx;
  uint32 req_bcast_tx;
  uint32 req_bcast_prb_tx;
  uint32 req_ucast_prb_tx;
  uint32 req_bcast_rexmit;
  uint32 req_bcast_prb_rexmit;
  uint32 req_ucast_prb_rexmit;
  uint32 arp_req_fail;
  uint32 arp_probe_timeout;
  uint32 resp_tx;
  uint32 req_bcast_fail_tx;
  uint32 req_bcast_prb_fail_tx;
  uint32 req_ucast_prb_fail_tx;
  uint32 req_bcast_fail_rexmit;
  uint32 req_bcast_prb_fail_rexmit;
  uint32 req_ucast_prb_fail_rexmit;
  uint32 resp_fail_tx;
  uint32 pkts_rx;
  uint32 req_rx;
  uint32 resp_rx;
  uint32 succ_resp_pending_rx;
  uint32 unknown_pkts_rx;
  uint32 not_for_us_rx;
  uint32 no_req_pending_rx;
  uint32 cache_hits;
  uint32 cache_misses;
  uint32 arp_req_cache_hits;
  uint32 arp_req_cache_misses;
  uint32 ce_evict_timeout;
  uint32 ce_evict_new_req;
  uint32 ce_evict_prb_succ;
  uint32 ce_evict_ps_ent_clear;
  uint32 ce_update_arp_req;
  uint32 ce_update_check_succ;
  uint32 ce_update_ps_ent_set;
} ps_stat_arp_i_s_type;


/*---------------------------------------------------------------------------
  ARP statistics enum along with the description and type of each entity
---------------------------------------------------------------------------*/
typedef enum
{
  /*
  Desc: Number of IPv4 packets that request transmission
  Type: uint32
  */
  PS_STAT_ARP_IPV4_REQ_TX = 0,
  /*
  Desc: Number of IPv4 packets that were dropped from the ARP cache on
        arrival of a new IPv4 packet for tranmission.
  Type: uint32
  */
  PS_STAT_IPV4_DROPPED_ON_NEW_REQ_TX = 1,
  /*
  Desc: Number of IPv4 packets that were dropped from the ARP cache
        while flushing the coresponding cache entry.
  Type: uint32
  */
  PS_STAT_IPV4_DROPPED_ON_CE_FLUSH = 2,
  /*
  Desc: Number of ARP packets transmitted includes ARP requests and
        replies.
  Type: uint32
  */
  PS_STAT_ARP_TX = 3,
  /*
  Desc: Number of ARP request packet broadcast.
  Type: uint32
  */
  PS_STAT_ARP_REQUEST_BROADCAST_TX = 4,
  /*
  Desc: Number of ARP broadcast request probes transmitted in response
        to a DHCP client request.
  Type: uint32
  */
  PS_STAT_ARP_REQUEST_BROADCAST_PROBE_TX = 5,
  /*
  Desc: Number of ARP unicast request probes transmitted in response
        to cache consistency maintenance requests.
  Type: uint32
  */
  PS_STAT_ARP_REQUEST_UNICAST_PROBE_TX = 6,
   /*
  Desc: Number of ARP request packet broadcast as retransmits.
  Type: uint32
  */
  PS_STAT_ARP_REQUEST_BROADCAST_REXMIT = 7,
  /*
  Desc: Number of ARP broadcast request probes retransmitted in response
        to a DHCP client request.
  Type: uint32
  */
  PS_STAT_ARP_REQUEST_BROADCAST_PROBE_REXMIT = 8,
  /*
  Desc: Number of ARP unicast request probes retransmitted in response
        to cache consistency maintenance requests.
  Type: uint32
  */
  PS_STAT_ARP_REQUEST_UNICAST_PROBE_REXMIT = 9,
  /*
  Desc: Number of ARP requests that failed to get a response even after
        the maximum number of retries and finally had to be timed out.
  Type: uint32
  */
  PS_STAT_ARP_REQUEST_FAIL = 10,
  /*
  Desc: Number of ARP retries that timed out when in ARP_STATUS_PROBING
        mode.
  Type: uint32
  */
  PS_STAT_ARP_PROBE_TIMEOUT = 11,
 /*
  Desc: Number of ARP response packets transmitted i.e both requests
        and replies.
  Type: uint32
  */
  PS_STAT_ARP_RESPONSE_TX = 12,
  /*
  Desc: Number of ARP broadcast requests failed to transmit.
  Type: uint32
  */
  PS_STAT_ARP_REQUEST_BROADCAST_FAIL_TX = 13,
  /*
  Desc: Number of ARP broadcast request probes failed to transmit.
  Type: uint32
  */
  PS_STAT_ARP_REQUEST_BROADCAST_PROBE_FAIL_TX = 14,
  /*
  Desc: Number of ARP unicast request probes failed to transmit.
  Type: uint32
  */
  PS_STAT_ARP_REQUEST_UNICAST_PROBE_FAIL_TX = 15,
  /*
  Desc: Number of ARP broadcast requests failed to retransmit.
  Type: uint32
  */
  PS_STAT_ARP_REQUEST_BROADCAST_FAIL_REXMIT = 16,
  /*
  Desc: Number of ARP broadcast request probes failed to retransmit.
  Type: uint32
  */
  PS_STAT_ARP_REQUEST_BROADCAST_PROBE_FAIL_REXMIT = 17,
  /*
  Desc: Number of ARP unicast request probes failed to retransmit.
  Type: uint32
  */
  PS_STAT_ARP_REQUEST_UNICAST_PROBE_FAIL_REXMIT = 18,
  /*
  Desc: Number of ARP response packets failed to transmit.
  Type: uint32
  */
  PS_STAT_ARP_RESPONSE_FAIL_TX = 19,
  /*
  Desc: Number of ARP packets received i.e both request and response.
  Type: uint32
  */
  PS_STAT_ARP_RX = 20,
  /*
  Desc: Number of ARP request packets received.
  Type: uint32
  */
  PS_STAT_ARP_REQUEST_RX = 21,
  /*
  Desc: Number of ARP response packets received.
  Type: uint32
  */
  PS_STAT_ARP_RESPONSE_RX = 22,
  /*
  Desc: Number of ARP response packets received corresponding to a
        pending request.
  Type: uint32
  */
  PS_STAT_ARP_SUCCESS_RESPONSE_PENDING_RX = 23,
  /*
  Desc: Number of packets received of type unknown frame
  Type: uint32
  */
  PS_STAT_ARP_UNKNOWN_PKT_RX = 24,
  /*
  Desc: Number of ARP packets received not destined to us.
  Type: uint32
  */
  PS_STAT_ARP_NOT_FOR_US_RX = 25,
  /*
  Desc: Number of ARP packets received for which no request was
        pending.
  Type: uint32
  */
  PS_STAT_ARP_NO_REQ_PENDING_RX = 26,
  /*
  Desc: Number of successful ARP cache hits.
  Type: uint32
  */
  PS_STAT_ARP_CACHE_HITS = 27,
  /*
  Desc: Number of ARP cache misses.
  Type: uint32
  */
  PS_STAT_ARP_CACHE_MISSES = 28,
  /*
  Desc: Number of successful ARP cache hits on receiving ARP
        request packet.
  Type: uint32
  */
  PS_STAT_ARP_REQUEST_CACHE_HITS = 29,
  /*
  Desc: Number of ARP cache misses on receiving ARP request packet.
  Type: uint32
  */
  PS_STAT_ARP_REQUEST_CACHE_MISSES = 30,
  /*
  Desc: Number of Cache entries evicted due to timeouts
  Type: uint32
  */
  PS_STAT_ARP_CE_EVICTED_TIMEOUT = 31,
  /*
  Desc: Number of Cache entries evicted due to arrival of new requests
  Type: uint32
  */
  PS_STAT_ARP_CE_EVICTED_NEW_REQ = 32,
  /*
  Desc: Number of Cache entries evicted as response was received for a
        corresponding broadcast probe.
  Type: uint32
  */
  PS_STAT_ARP_CE_EVICTED_PROBE_SUCCESS = 33,
  /*
  Desc: Number of cache entries evicted on request from a PS entity
        like the DHCP server.
  Type: uint32
  */
  PS_STAT_ARP_CE_EVICTED_PS_ENTITY_CLEAR = 34,
  /*
  Desc: Number of cache entries updated on receiving the ARP request.
  Type: uint32
  */
  PS_STAT_ARP_CE_UPDATES_ARP_REQ = 35,
  /*
  Desc: Number of cache entries updated on when a response frame is
        received corresponding to a unicast check request.
  Type: uint32
  */
  PS_STAT_ARP_CE_UPDATES_CHECKING_SUCCESS = 36,
  /*
  Desc: Number of cache entries updated on request from an external PS
        entity like the DHCP server.
  Type: uint32
  */
  PS_STAT_ARP_CE_UPDATES_PS_ENTITY_SET = 37,

  PS_STAT_ARP_ALL
} ps_stat_arp_enum_type;


/*===========================================================================

                          LOG PACKET DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Log structure for LOG_PS_STAT_ARP_INST_C

  Header defined in ps_stat_logging.h

  Fields are
    handle           : handle to the instance
    inst_arp_stats   : instance ARP stats
---------------------------------------------------------------------------*/
typedef struct
{
  int32                     handle;
  ps_stat_arp_i_s_type      inst_arp_stats;
} ps_stat_inst_arp_log_pkt_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_STAT_INST_DESC_ARP_TYPE

DESCRIPTION
  The structure describing an ARP instance.

  handle        : Handle to the ARP instance.  Index into the ARP array
  iface_handle  : Handle to the iface the ARP instance is associated with
  reserved      : Used to maintain a 4-byte alignment
---------------------------------------------------------------------------*/
typedef struct
{
  uint8   handle;
  uint8   iface_handle;
  uint8   reserved[2];
} ps_stat_inst_desc_arp_type;


/*===========================================================================

                         EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
MACRO ARP_INC_INSTANCE_STATS()

DESCRIPTION
  This macro increments the ARP statistic(field) by the necessary
  quantity (inc).

DEPENDENCIES
  None

PARAMETERS
  INSTANCE_ID - Id of the ARP instance control block
  FIELD       - The field of the corresponding statistics structure that is
                to be incremented.
  INC         - The increment by which the field is to be incremented.

SIDE EFFECTS
  None
===========================================================================*/
#define ARP_INC_INSTANCE_STATS(INSTANCE_ID, FIELD, INC)     \
    PS_COMMON_INC_STATS(arp_array[INSTANCE_ID].arp_i_stats.FIELD, 4, INC)

/*===========================================================================
FUNCTION PS_STAT_INIT_ARP()

DESCRIPTION

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_init_arp
(
  void
);

/*===========================================================================
FUNCTION PS_STAT_GET_ARP()

DESCRIPTION
  This function supplies the required ARP layer statistics. The first
  argument passed to the function is an enum that denotes the metric of
  interest.The second argument will be a pointer that provides a
  handle to an instance ( For per-instance statistics only).
  The third argument points to the memory location where the results
  will be copied. The result ( statistic) will be copied to
  this memory address. The fourth argument passes the length of memory
  thus allocated for the return value and should be greater than or equal
  to the memory size of the statistic requested.

DEPENDENCIES
  None

RETURN VALUE

  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h. The function also calls err_fatal in
  the case of any error.

PARAMETERS
  stat            - IN - type of stat
  instance_ptr    - IN - pointer to instance
  return_value    - IN - pointer for return value
                  - OUT- return value (statistics)
  ret_len         - length of memory allocated

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_arp
  (
    ps_stat_arp_enum_type stat,
    void                  *instance_ptr,
    void                  *return_value,
    uint16                 ret_len
);

/*===========================================================================
FUNCTION PS_STAT_RESET_ARP()

DESCRIPTION
  This function resets the instance statistics associated with the specified
  instance.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  handle  : Handle to the instance

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_reset_arp
(
  int32           handle
) ;

/*===========================================================================
FUNCTION PS_STAT_GET_ARP_CONTROL_BLOCK()

DESCRIPTION
  This function supplies the address of the ARP control block corresponding
  the ARP device passed.

DEPENDENCIES
  None

RETURN VALUE

  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  device        - IN -  ARP device
  arpcb_address - OUT - address of the arp control block corresponding
                        to the device.

SIDE EFFECTS
  None
===========================================================================*/

errno_enum_type ps_stat_get_arp_control_block
(
 int device,
 void **arpcb_address
);

#endif  /* PS_STAT_ARP_H */
