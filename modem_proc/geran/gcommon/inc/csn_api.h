/*===========================================================================

                             CSN.1 ENCODER DECODER
                       P U B L I C   I N T E R F A C E S

DESCRIPTION
  The following declarations are for use with the CSN.1 encoder/decoder.

Copyright (c) 2007-2013 Qualcomm Technologies, Inc.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gcommon/inc/csn_api.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10-04-01   tjw     Add customer.h and comdef.h to all SU API files
07-06-15   sjw     Mainlined the refactored CSN files. Feature def is not required.
06-05-17   tjw     Created from //depot/asic/msmshared/geran/core/gcommon/csn.h#16.

===========================================================================*/

#ifndef CSN_API_H
#define CSN_API_H

/* Include files */

#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "csn_apt.h"
#include "geran_multi_sim.h"

/* to maintain backwards-compatibility: */
#define size_of_rcvd_dl_control(x) gcsn1_decoder_size_of_rcvd_dl_control(x)
#define free_csn_decoder_ptrs_t(item) gcsn1_decoder_memory_free(item)

/* Further backwards-compatibility - valid/not_valid has been replaced by
   TRUE/FALSE but some clients have elected not to make this change */
#define option_status_e boolean
enum
{
  not_valid = FALSE,
  valid     = TRUE
};



/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
===
===  FUNCTION      gcsn1_decoder
===
===  DESCRIPTION
===
===    Translate RLC/MAC Control header from external compressed format into
===    internal decompressed format
===
===  DEPENDENCIES
===
===    none
===
===  RETURN VALUE
===
===    Return a structure containing two pointers, one to the header and one
===    to the data.
===
===  SIDE EFFECTS
===
===    Memory is allocated and must be freed by a call to gcsn1_decoder_memory_free()
===
===========================================================================*/

csn_decoder_ptrs_t gcsn1_decoder(
  uint8 *src_ota,
  uint16 *bits_read,
  uint16 ota_msg_size_octets,
  const gas_id_t gas_id
);

/*===========================================================================
===
===  FUNCTION      gcsn1_decoder_size_of_rcvd_dl_control
===
===  DESCRIPTION
===
===    Passes CSN the size in octets of a rcvd DL control msg that
===    is available for control data. It is not an indication of the
===    amount of data contained in the msg.
===
===  DEPENDENCIES
===
===    none
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
void gcsn1_decoder_size_of_rcvd_dl_control (uint8 size_in_octets);

/*===========================================================================
===
===  FUNCTION      gcsn1_decoder_memory_free
===
===  DESCRIPTION
===
===    Frees the memory allocated by gcsn1_decoder.
===
===  DEPENDENCIES
===
===    none
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    Memory may be returned to the heap.
===
===========================================================================*/
void gcsn1_decoder_memory_free(csn_decoder_ptrs_t *item);

/*===========================================================================
===
===  FUNCTION      gcsn1_rr_decoder
===
===  DESCRIPTION
===
===    Translate RR Sys Info Rest Octets from compressed format into
===    internal decompressed format
===
===  DEPENDENCIES
===
===    none
===
===  RETURN VALUE
===
===    CSN decoder error report
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/

csn_decoder_error_t gcsn1_rr_decoder(
  csn_rr_decoder_msg_id_t message_type,
  uint8 *csn1_ie_src,
  void *dst,
  uint16 len_to_end_of_buf,
  const gas_id_t gas_id
);
/*===========================================================================
===
===  FUNCTION      gcsn1_encoder
===
===  DESCRIPTION
===
===    Translate internal decompressed format into external compressed format.
===
===  DEPENDENCIES
===
===    none
===
===  RETURN VALUE
===
===    PASS/FAIL
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
uint8 gcsn1_encoder(uint8 message_type, uint8 *src, int_data_t *int_data);

/*===========================================================================
===
===  FUNCTION      gcsn1_rr_encoder
===
===  DESCRIPTION
===
===    Translate internal decompressed format into external compressed format.
===
===  DEPENDENCIES
===
===    none
===
===  RETURN VALUE
===
===    number of octets
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
uint8 gcsn1_rr_encoder(csn_rr_encoder_msg_id_t message_type, void *src, uint8 *dst);

#endif /* CSN_API_H */

