/*===========================================================================

                             CSN.1 ENCODER DECODER

                            D E C L A R A T I O N S

DESCRIPTION
  The following declarations are for use with the CSN.1 encoder/decoder.

Copyright (c) 2007-2013 Qualcomm Technologies, Inc.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gcommon/inc/csn.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10-04-01   tjw     Add customer.h and comdef.h to all SU API files
07-06-15   sjw     Mainlined the refactored CSN files. Feature def is not required.
06-06-01   tjw     Added section under FEATURE_CSN_REFACTORED to work with legacy code
                   that uses the refactored CSN decode/encode but does not use new-style
                   header files. There are now 3 kinds of client:
                   * ones that use the original decode/encode
                   * ones that use the refactored decode/encode
                   * ones that use the refactored decode/encode and new-style
                     header files
03-09-25   tjw     Suppress Lint warning
15/11/02   pjr     Modified access_capability in structure packet_resource_request_t
09/07/02   gdj     Added chan_desc_t (part of p1_rest_octets_t).
21/05/02   pjr     Changed high rate len,  csn_psi_cnt_hr_len to 4.
16/05/02   gdj     Addition of SI7 and SI8 rest octets structs.
14/05/02   gdj     Changed TBF Starting Time to uint16, added mobile allocation
                   length to ia_rest_octets_freq_params_before_time;
14/05/02   pjr     Added change_mark_2_valid to enum change_mark_flag_e
10/05/02   gdj     Added structs for SI9, mobile classmark 3 and others
01/05/02   gdj     Added struct/defs for RR SI messages
15/04/02   pjr     Added bts_pwr_ctrl_mode to single block allocation.
11/04/02   pjr     Added nc and ext flags to packet_measurement_order_t
26/03/02   pjr     Change value of CSN_DECODE_HDR_SZ, see note by change.
21/03/02   pjr     Changed definition of the rlc_mac_hdr in gcsn_decoder
                   to remove the union of message types as this adversely
                   affected the gprs_mem_malloc.
21/02/02   pjr     Modified parameters to bit manipulation routines to
                   uint16 instead of int16.
                   Modified received_blk_btmp in ack_nack_desc_t from uint64
                   to uint8 received_blk_btmp[8]
12/11/01   pjr     Included gbitutil.h, changed BLOCKS to CSN_BLOCKS to avoid
                   more clashes. Removed switch CSN_HOST_TEST as not required
12/11/01   pjr     Renamed chan_req_desc_t to channel_req_desc_t to avoid
                   clash of names in RLC.
12/06/01   pjr     Brought up to Release 99 of 4.60
05/22/01   pjr     Created.

===========================================================================*/

#ifndef CSN_H
#define CSN_H

#ifndef CUSTOMER_H
  #include "customer.h"
#endif

#include "csn_api.h"

#endif /* CSN_H */

