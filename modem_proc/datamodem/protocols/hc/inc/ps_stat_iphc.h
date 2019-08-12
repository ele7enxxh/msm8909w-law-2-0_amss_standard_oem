#ifndef PS_STAT_IPHC_H
#define PS_STAT_IPHC_H

/*===========================================================================

                          P S _ S T A T _ I P H C . H

GENERAL DESCRIPTION
 This header file provides the structure, enum and function definitions
 for the statistics pertaining to IP Header Compression Protocol.

EXTERNALIZED FUNCTIONS
  ps_stat_get_iphc()
    This function supplies the required IPHC stats.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2006-2008 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when        who     what, where, why
----------  ---     ---------------------------------------------------------
11/21/08    pp      Lint fixes.
02/27/07    scb     Added changes to fix RCVT compilation warning
02/21/07    scb     Replaced the INC macro implementation to use the
                    COMMON_INC macro
11/17/06    ifk     Fixed lint errors
08/18/06    scb     Updated enum values to start with 0 for calculating
                    correct offset.
07/11/2006  scb     Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#include "ps_stat_common.h"
#include "ps_ppp_defs.h"


/*---------------------------------------------------------------------------
  IPHC Statistics - Instance stats only.
---------------------------------------------------------------------------*/
typedef struct
{
  /*-------------------------------------------------------------------------
    Compression statistics:
    comp_hdr_bytes              Number of octets in the uncompressed headers
                                which are compressed by the compresser
    comp_hdr_bytes_comp         Number of octets of compressed header
                                generated as a result of compression
    comp_ipv4_frags             Number of IPv4 fragments received for
                                compression
    comp_ipv4_full_hdr_tos_67   Number of IPv4 full header packets
                                transmitted because the type of service bits
                                6 and 7 had changed and the packet was a
                                TCP packet

    comp_ipv6_full_hdr_tos_67   Number of IPv6 full header packets
                                transmitted because the traffic class type of
                                service bits had changed and the packet was a
                                TCP packet
    comp_tcp_full_hdr_rexmit    Number of full header TCP packets that were
                                sent as a result of a retransmission.
  -------------------------------------------------------------------------*/

  uint32  comp_hdr_bytes;
  uint32  comp_hdr_bytes_comp;
  uint32  comp_ipv4_frags;
  uint32  comp_ipv4_full_hdr_tos_67;
  uint32  comp_ipv6_full_hdr_tos_67;
  uint32  comp_tcp_full_hdr_rexmit;

  /*-------------------------------------------------------------------------
    Decompression statistics:

    decomp_hdr_bytes            Number of octets in the compressed header
                                received that are to be decompressed.
    decomp_hdr_bytes_decomp     Number of octets of the header that have been
                                retrieved after decompression.
    decomp_tcp_twice_alg        Number of contexts that have been
                                successfully repaired after running the twice
                                algorithm.
    decomp_ctx_state_pkt_tx     Number of context state packets that were
                                transmitted as a part of decompression.
    decomp_ctx_state_pkt_rx     Number of context state packets that were
                                received.
    decomp_ctx_state_bytes_rx   Number of octets of context state packets that
                                were received.
    decomp_ctx_state_bytes_tx   Number of octets of context state packets that
                                were transmitted.

  -------------------------------------------------------------------------*/

  uint32  decomp_hdr_bytes;
  uint32  decomp_hdr_bytes_decomp;
  uint32  decomp_tcp_twice_alg;
  uint32  decomp_ctx_state_pkt_tx;
  uint32  decomp_ctx_state_bytes_tx;
  uint32  decomp_ctx_state_pkt_rx;
  uint32  decomp_ctx_state_bytes_rx;

  /*-------------------------------------------------------------------------
    Compressor error statistics:

    comp_gen_errors             Number of compressions that have failed due
                                to general errors (these errors have been
                                captured in the iphc_compress function in
                                ps_iphc.c and other auxiliary functions
                                called by this function)
    comp_dsm_errors             Number of compressions that have failed due
                                to dsm errors
    comp_full_hdr_fail          Number of full header packets that failed to
                                be sent from the compressor context.
    comp_ipv4_fail              Number of ipv4 packets that failed to
                                be compressed.
    comp_ipv4_malformed_pkt     Number of ipv4 packets that failed to be
                                compressed because the packet was malformed.
    comp_ipv6_fail              Number of ipv6 packets that failed to
                                be compressed.
    comp_ipv6_malformed_pkt     Number of ipv6 packets that failed to be
                                compressed because the packet was malformed.
    comp_tcp_fail               Number of tcp packets that failed to be
                                compressed.
    comp_non_tcp_fail           Number of non-tcp packets that failed to be
                                compressed.
  -------------------------------------------------------------------------*/

  uint32  comp_gen_errors;
  uint32  comp_dsm_errors;
  uint32  comp_full_hdr_fail;
  uint32  comp_ipv4_fail;
  uint32  comp_ipv4_malformed_pkt;
  uint32  comp_ipv6_fail;
  uint32  comp_ipv6_malformed_pkt;
  uint32  comp_tcp_fail;
  uint32  comp_non_tcp_fail;

  /*-------------------------------------------------------------------------
    Decompression error statistics
    decomp_invalid_ip           Number of packets received that failed to be
                                decompressed because of invalid IP.
    decomp_gen_errors           Number of decompressions that have failed
                                due to general errors (these errors have
                                been captured in the iphc_decompress fucntion
                                in ps_iphc.c and other auxiliary functions
                                called by the above function)
    decomp_dsm_errors           Number of decompressions that have failed
                                due to dsm errors
    decomp_full_hdr_fail        Number of full header packets received that
                                failed to be decompressed at the
                                decompressors context
    decomp_ipv4_fail            Number of ipv4 packets that failed to be
                                decompressed.
    decomp_ipv4_malformed_pkt   Number of ipv4 packets that failed
                                decompression because packet was malformed.
    decomp_ipv6_fail            Number of ipv6 packets that failed to be
                                decompressed.
    decomp_ipv6_malformed_pkt   Number of ipv6 packets that failed
                                decompression because packet was malformed.
    decomp_tcp_fail             Number of tcp packets that failed to be
                                decompressed.
    decomp_tcp_mis_cksum        Number of tcp packets that failed to be
                                decompressed because of improper checksum
    decomp_tcp_nodelta_fail     Number of tcp nodelta packets that failed
                                to be decompressed.
    decomp_non_tcp_fail         Number of non tcp packets that failed to be
                                decompressed.
 -------------------------------------------------------------------------*/

  uint32  decomp_invalid_ip;
  uint32  decomp_gen_errors;
  uint32  decomp_dsm_errors;
  uint32  decomp_full_hdr_fail;
  uint32  decomp_ipv4_fail;
  uint32  decomp_ipv4_malformed_pkt;
  uint32  decomp_ipv6_fail;
  uint32  decomp_ipv6_malformed_pkt;
  uint32  decomp_tcp_fail;
  uint32  decomp_tcp_mis_cksum;
  uint32  decomp_tcp_nodelta_fail;
  uint32  decomp_non_tcp_fail;

  /*-------------------------------------------------------------------------
    Compression packet statistics:

    comp_pkt_ipv4                     Number of ipv4 packets successfully
                                      compressed
    comp_pkt_ipv6                     Number of ipv6 packets successfully
                                      compressed
    comp_pkt_full_header              Number of full header packets that
                                      were sent from the compressor context
    comp_pkt_compressed_tcp           Number of tcp packets successfully
                                      compressed
    comp_pkt_compressed_tcp_nodelta   Number of tcp nodelta packets
                                      successfully compressed
    comp_pkt_compressed_non_tcp       Number of non tcp packets that were
                                      successfully compressed

    Note: The number of compressed packets are right now calculated as
    follows:
  -------------------------------------------------------------------------*/

  uint32  comp_pkt_ipv4;
  uint32  comp_pkt_ipv6;
  uint32  comp_pkt_full_header;
  uint32  comp_pkt_compressed_tcp;
  uint32  comp_pkt_compressed_tcp_nodelta;
  uint32  comp_pkt_compressed_non_tcp;

  /*-------------------------------------------------------------------------
    Decompression packet statistics

    decomp_pkt_ipv4                     Number of ipv4 packets successfully
                                        decompressed
    decomp_pkt_ipv6                     Number of ipv6 packets successfully
                                        decompressed
    decomp_pkt_full_header              Number of full header packets that
                                        were received at the decompressor
                                        context
    decomp_pkt_compressed_tcp           Number of tcp packets successfully
                                        decompressed
    decomp_pkt_compressed_tcp_nodelta   Number of tcp nodelta packets
                                        successfully decompressed
    decomp_pkt_compressed_non_tcp       Number of non tcp packets that were
                                        successfully decompressed
  -------------------------------------------------------------------------*/

  uint32  decomp_pkt_ipv4;
  uint32  decomp_pkt_ipv6;
  uint32  decomp_pkt_full_header;
  uint32  decomp_pkt_compressed_tcp;
  uint32  decomp_pkt_compressed_tcp_nodelta;
  uint32  decomp_pkt_compressed_non_tcp;

} ps_stat_iphc_i_s_type;

/*---------------------------------------------------------------------------
 IPHC statistics enum along with the description and type of each entity
---------------------------------------------------------------------------*/
typedef enum
{
  /*-------------------------------------------------------------------------
    Desc: Number of Original header bytes to be compressed
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_COMP_HDR_BYTES               = 0     ,

  /*-------------------------------------------------------------------------
    Desc: Number of compressed header bytes
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_COMP_HDR_BYTES_COMP          = 1     ,

  /*-------------------------------------------------------------------------
    Desc: Number of IPv4 fragments compressed
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_COMP_IPV4_FRAGMENTS          = 2     ,

  /*-------------------------------------------------------------------------
    Desc: Number of IPv4 full header packets sent because of the TOS
          6 and 7 bits for TCP have changed
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_COMP_IPV4_FULL_HDR_TOS_67    = 3     ,

  /*-------------------------------------------------------------------------
    Desc: Number of IPv6 full header packets sent because the TOS
          6th and 7th bits had changed
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_COMP_IPV6_FULL_HDR_TOS_67    = 4     ,

  /*-------------------------------------------------------------------------
    Desc: Number of TCP full header packets sent as retransmits
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_COMP_TCP_FULL_HDR_REXMIT     = 5     ,

  /*-------------------------------------------------------------------------

    Desc: Number of original header bytes received
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_DECOMP_HDR_BYTES             = 6     ,

  /*-------------------------------------------------------------------------
    Desc: Number of bytes retrieved after decompression
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_DECOMP_HDR_BYTES_DECOMP      = 7     ,

  /*-------------------------------------------------------------------------
    Desc: Number of TCP packets which succeeded the twice algorithm
          check and hence has had its context repaired
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_DECOMP_TCP_TWICE_ALG         = 8     ,

  /*-------------------------------------------------------------------------
    Desc: Number of Context state packets received
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_DECOMP_CTX_STATE_PACKET_TX   = 9    ,

  /*-------------------------------------------------------------------------
    Desc: Number of Context state bytes received
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_DECOMP_CTX_STATE_BYTES_TX    = 10    ,

  /*-------------------------------------------------------------------------
    Desc: Number of context state packets received
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_DECOMP_CTX_STATE_PACKET_RX   = 11    ,

  /*-------------------------------------------------------------------------
    Desc: Number of context state bytes received
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_DECOMP_CTX_STATE_BYTES_RX    = 12    ,

  /*-------------------------------------------------------------------------
    Desc: Number of general errors encountered during compression
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_COMP_GEN_ERRORS              = 13    ,

 /*-------------------------------------------------------------------------
    Desc: Number of DSM errors encountered during compression
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_COMP_DSM_ERRORS              = 14    ,

  /*-------------------------------------------------------------------------
    Desc: Number of ful header packets failed to send.
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_COMP_FULL_HDR_FAIL           = 15    ,

  /*-------------------------------------------------------------------------
    Desc: Number of IPv4 packets failed to compress
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_COMP_IPV4_FAIL               = 16    ,

  /*-------------------------------------------------------------------------
    Desc: Number of IPv4 malformed packets to be compressed.
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_COMP_IPV4_MALFORMED_PKT      = 17    ,

  /*-------------------------------------------------------------------------
    Desc: Number of IPv6 packets failed to compress
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_COMP_IPV6_FAIL               = 18    ,

  /*-------------------------------------------------------------------------
    Desc: Number of IPv6 packets failed to compress because
          it was malformed
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_COMP_IPV6_MALFORMED_PKT      = 19    ,

  /*-------------------------------------------------------------------------
    Desc: Number of TCP packets failed to compress
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_COMP_TCP_FAIL                = 20    ,

  /*-------------------------------------------------------------------------
    Desc: Number of NON TCP packets failed to compress
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_COMP_NON_TCP_FAIL            = 21    ,

  /*-------------------------------------------------------------------------
    Desc: Number of invalid IP packets to be decompressed
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_DECOMP_INVALID_IP            = 22    ,

  /*-------------------------------------------------------------------------
    Desc: Number of general errors encountered during decompression
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_DECOMP_GEN_ERRORS            = 23    ,

  /*-------------------------------------------------------------------------
    Desc: Number of DSM errors encountered during decompression
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_DECOMP_DSM_ERRORS            = 24    ,

  /*-------------------------------------------------------------------------
    Desc: Number of full header packet failed to decompress
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_DECOMP_FULL_HDR_FAIL         = 25    ,

  /*-------------------------------------------------------------------------
    Desc: Number of IPv4 packets failed to decompress
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_DECOMP_IPV4_FAIL             = 26    ,

  /*-------------------------------------------------------------------------
    Desc: Number of IPv4 packets failed because of malformed packet
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_DECOMP_IPV4_MALFORMED_PKT    = 27    ,

  /*-------------------------------------------------------------------------
    Desc: Number if IPv6 packets failed to decompress
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_DECOMP_IPV6_FAIL             = 28    ,

  /*-------------------------------------------------------------------------
    Desc: Number of IPv6 malformed packets failed to decompress
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_DECOMP_IPV6_MALFORMED_PKT    = 29    ,

  /*-------------------------------------------------------------------------
    Desc: Number of TCP packets failed to decompress
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_DECOMP_TCP_FAIL              = 30    ,

  /*-------------------------------------------------------------------------
    Desc: Number of TCP packets failed to decompress because of
          mismatch in checksum.
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_DECOMP_TCP_MIS_CKSUM         = 31    ,

  /*-------------------------------------------------------------------------
    Desc: Number of TCP no delta packets failed to decompress
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_DECOMP_TCP_NO_DELTA_FAIL     = 32    ,

  /*-------------------------------------------------------------------------
    Desc: Number of NON TCP packets failed to decompress
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_DECOMP_NON_TCP_FAIL          = 33    ,

 /*-------------------------------------------------------------------------
    Desc: Number of IPv4 packets actually compressed
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_COMP_IPV4                    = 34    ,

  /*-------------------------------------------------------------------------
    Desc: Number of IPv6 packets actually compressed
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_COMP_IPV6                    = 35    ,

  /*-------------------------------------------------------------------------
    Desc: Number of full header packets sent.
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_COMP_FULL_HDR                = 36    ,

  /*-------------------------------------------------------------------------
    Desc: Number of TCP packets actually compressed
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_COMP_TCP                     = 37    ,

  /*-------------------------------------------------------------------------
    Desc: Number of TCP no delta packets sent
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_COMP_TCP_NODELTA             = 38    ,

  /*-------------------------------------------------------------------------
    Desc: Number of NON TCP packets actually compressed
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_COMP_NON_TCP                 = 39    ,

  /*-------------------------------------------------------------------------
    Desc: Number of IPv4 packets actually decompressed
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_DECOMP_IPV4                  = 40    ,

  /*-------------------------------------------------------------------------
    Desc: Number of IPv6 packets actually decompressed
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_DECOMP_IPV6                  = 41    ,

  /*-------------------------------------------------------------------------
    Desc: Number of full header packets actually decompressed
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_DECOMP_FULL_HDR              = 42    ,

  /*-------------------------------------------------------------------------
    Desc: Number of TCP packets decompressed
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_DECOMP_TCP                   = 43    ,

  /*-------------------------------------------------------------------------
    Desc: Number of TCP no delta packets failed actually decompressed
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_DECOMP_TCP_NODELTA           = 44    ,

  /*-------------------------------------------------------------------------
    Desc: Number of NON TCP packets actually decompressed
    Type: uint32
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_DECOMP_NON_TCP               = 45    ,

  /*-------------------------------------------------------------------------
    Desc: Enum value of the entire IPHC statistics structure
    Type: sizeof(struct ps_stats_iphc_i_s_type)
  -------------------------------------------------------------------------*/

  PS_STAT_IPHC_ALL
} ps_stat_iphc_enum_type ;


/*===========================================================================

                          LOG PACKET DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Log structure for LOG_PS_STAT_IPHC_INST_C

  Log header is defined ps_stat_logging.h

  Fields are
    handle           : handle to the instance
    inst_iphc_stats  : instance IPHC stats
---------------------------------------------------------------------------*/
typedef struct
{
  int32                     handle;
  ps_stat_iphc_i_s_type     inst_iphc_stats;
} ps_stat_inst_iphc_log_pkt_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_STAT_INST_DESC_IPHC_TYPE

DESCRIPTION
  The structure describing a ps_iphc instance.

  handle    : Instance handle, index into the ps_iphc cb array
  reserved  : Used to maintain a 4-byte alignment
---------------------------------------------------------------------------*/
typedef struct
{
  uint8  handle;
  uint8  reserved[3];
} ps_stat_inst_desc_iphc_type;


/*===========================================================================

                         EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
MACRO IPHC_INC_INSTANCE_STATS()

DESCRIPTION
  This macro increments the IPHC statistic(field) by the necessary
  quantity (inc).

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define IPHC_INC_INSTANCE_STATS(PTR, FIELD, INC)          \
    PS_COMMON_INC_STATS(PTR->iphc_i_stats.FIELD, 4, INC)


/*===========================================================================
FUNCTION PS_STAT_INIT_IPHC()

DESCRIPTION

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_init_iphc
(
  void
);


/*===========================================================================
FUNCTION PS_STAT_GET_IPHC()

DESCRIPTION
  This function supplies the required IPHC layer statistics. The first
  argument passed to the function is an enum that denotes the metric of
  interest.The second argument will be  a pointer that provides an
  handle to an instance ( For per-instance statistics only).
  The third argument points to the memory location where the results
  will be copied. The result ( statistic) will be copied to this memory
  address. The fourth argument passes the length of memory that
  allocated for the return value and should be greater than or equal to
  the memory size of the statistic requested.

  Note: Some statistics are incremented by looping into the context
  space and adding them into the corresponding instance wise statistics
  as and when required.

DEPENDENCIES
  None

RETURN VALUE

  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h. The function also calls err_fatal in
  the case of any error.

PARAMETERS
  stat            - IN - type of stat
  instance_ptr    - IN - pointer to instance;
  return_value    - IN - pointer for return value
                  - OUT- return value (statistics)
  ret_len         - length of memory allocated

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_iphc
(
  ps_stat_iphc_enum_type     stat,
  void                       *instance_ptr,
  void                       *return_value,
  uint16                     ret_len
);


/*===========================================================================
FUNCTION PS_STAT_GET_IPHC_CONTROL_BLOCK_FOR_PPP_DEV()

DESCRIPTION
  This function supplies the address of the IPHC control block
  corresponding to the PPP control block for the specified device.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  device         - IN -PPP device
  iphccb_address - OUT - address of the iphc control blaock from
                         the ppp control block corresponding to the
                         device. This is a void ** so that a void*
                         value can be returned back and can be used to
                         call ps_stat_get_ppp

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_iphc_control_block_for_ppp_dev
(
  ppp_dev_enum_type     device,
  void                **iphccb_address
);


/*===========================================================================
FUNCTION PS_STAT_RESET_IPHC()

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
void ps_stat_reset_iphc
(
  int32       handle
);
#endif /* PS_STAT_IPHC_H */
