/*****************************************************************************

                      C S N . 1   D E C O D E

 GPRS - CSN.1 Decoder functions.


DESCRIPTION

  Translation services for RLC/MAC Control Messages, over-the-air external
  data format and internal data structure representation.


EXTERNALIZED FUNCTIONS
  gcsn1_decoder
  gcsn1_rr_decoder
  gcsn1_decoder_size_of_rcvd_dl_control
  gcsn1_decoder_memory_free

INITIALIZATION AND SEQUENCING REQUIREMENTS

  None required


Copyright (c) 2007-2013 Qualcomm Technologies, Inc.

*****************************************************************************/


/*****************************************************************************

                       EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gcommon/src/csn_decode.c#1 $

when      who     what, where, why
--------  ---     ------------------------------------------------------
07-06-15  sjw     Mainlined the refactored CSN files. Feature def is not required.
06-08-14  tjw     To make gcsn1_decoder() re-entrant, added horrible
                  complexity to memory allocation and de-allocation.
                  Now each instance of gcsn1_decoder() has a separate
                  instance of the memory allocation data structure.
                  The first of these is statically allocated for
                  efficiency, the remainder are malloc'd.
06-06-02  tjw     Use enums for error status (and message types)
                  Optimised arg error-checking in gcsn1_decoder
                  Changed ERR_FATALs to MSG_FATAL
                  Removed now-redundant csn_decode_ext_meas_params() entirely.
                  Deleted old malloc handler.
06-05-11  tjw     gcsn1_decoder() refactored to use a table
06-05-09  tjw     Initial version carved from
                  //depot/asic/msmshared/geran/core/gcommon/csn_decode_encode.c#26
                  as a baseline for refactoring.
*****************************************************************************/


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#include "comdef.h"
#include "gbitutil.h"
#include "sys_type.h"
#include "csn_aptx.h"
#include "csn_api.h"
#include "gprs_mem.h"
#include "rr_multiband.h"
#include "rr_multiband_g.h"
#include <stringl.h>

/* added for GMACUTF */
#include "geran_msgs.h"

#include "geran_multi_sim.h"


/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/


#define CSN_BAND_INITIALISER(arfcn_struct) \
  {(arfcn_struct).band = (sys_band_T)rr_get_band((arfcn_struct).num, gas_id);}

/* decoding/encoding operates on the "num" part of an ARFCN_T structure,
   and the band has to be initialised */
#define CSN_DECODE_ARFCN(msg_ptr, msg_pos_ptr)\
{\
  (msg_ptr)->arfcn.num = gunpackw(src, (msg_pos_ptr), CSN_ARFCN_LEN);\
  CSN_BAND_INITIALISER((msg_ptr)->arfcn);\
}


/*****************************************************************************
***
***     Private Typedefs
***
*****************************************************************************/
#define CSN_MAX_MALLOCS 100   /* 87 is the maximum seen during testing
                                (csn_decode_malloc_list_index_max) */
typedef struct
{
  void * csn_decode_malloc_list[CSN_MAX_MALLOCS];
  uint8 csn_decode_malloc_list_index;        /* stores the number of malloc
                                                pointers that are requested */
  /* gcsn1_decode() always requires a minimum amount of memory. Instead of
     mallocing it every time, allocate a block permanently and only malloc()
     if more memory is requested.
     The size is decided empirically by inspecting malloc requests - it doesn't
     matter what the size is: too small and malloc is called more often; too
     large and some memory is denied to the heap.
     It is defined in uint32 to prevent memory alignment errors.
  */
  #define CSN_DECODE_RESERVED_MEMORY_SIZE_IN_BYTES  220
  #define CSN_DECODE_RESERVED_MEMORY_SIZE_IN_UINT32 (CSN_DECODE_RESERVED_MEMORY_SIZE_IN_BYTES / 4)

  uint32 csn_decode_reserved_memory[CSN_DECODE_RESERVED_MEMORY_SIZE_IN_UINT32];
  boolean csn_decode_reserved_memory_used;
} csn_decode_memory_instance_t;

/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/

/*stores the size of the data part of dl control msg in octets*/
static uint16 dl_control_msg_size = 0;
#define MAX_DECODE_BITS (dl_control_msg_size << 3)

/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/


/*  CSN.1 message processing Functions */

static csn_decoder_error_t csn_pkt_acc_rej(
  csn_decode_memory_instance_t *m,
  uint8 *src,
  uint16 *pos_ptr,
  packet_access_reject_t *msg_ptr,
  const gas_id_t gas_id
);
static csn_decoder_error_t csn_pkt_dwnlk_assgn(
  csn_decode_memory_instance_t *m,
  uint8 *src,
  uint16 *pos_ptr,
  packet_downlink_assignment_t *msg_ptr,
  const gas_id_t gas_id
);
static csn_decoder_error_t csn_pkt_page_req(
  csn_decode_memory_instance_t *m,
  uint8 src[],
  uint16 *pos_ptr,
  packet_paging_request_t *msg_ptr,
  const gas_id_t gas_id
);
static csn_decoder_error_t csn_pkt_pdch_release(
  csn_decode_memory_instance_t *m,
  uint8 *src,
  uint16 *pos_ptr,
  packet_pdch_release_t *dst,
  const gas_id_t gas_id
);
static csn_decoder_error_t csn_pkt_dwnlk_dummy_cntrl_blk(
  csn_decode_memory_instance_t *m,
  uint8 *src,
  uint16 *pos_ptr,
  packet_dwnlk_dummy_cntrl_blk_t *dst,
  const gas_id_t gas_id
);
static csn_decoder_error_t csn_pkt_poll_req(
  csn_decode_memory_instance_t *m,
  uint8 src[],
  uint16 *pos_ptr,
  packet_polling_request_t *msg_ptr,
  const gas_id_t gas_id
);
static csn_decoder_error_t csn_pkt_pwrctrl_ta(
  csn_decode_memory_instance_t *m,
  uint8 src[],
  uint16 *pos_ptr,
  packet_pwr_ctrl_tmng_adv_t *msg_ptr,
  const gas_id_t gas_id
);
static csn_decoder_error_t csn_pkt_timeslot_reconf(
  csn_decode_memory_instance_t *m,
  uint8 src[],
  uint16 *pos_ptr,
  packet_timeslot_reconfigure_t *msg_ptr,
  const gas_id_t gas_id
);
static csn_decoder_error_t csn_pkt_tbf_release(
  csn_decode_memory_instance_t *m,
  uint8 src[],
  uint16 *pos_ptr,
  packet_tbf_release_t *msg_ptr,
  const gas_id_t gas_id
);
static csn_decoder_error_t csn_pkt_uplk_ack_nack(
  csn_decode_memory_instance_t *m,
  uint8 src[],
  uint16 *pos_ptr,
  packet_uplink_ack_nack_t *msg_ptr,
  const gas_id_t gas_id
);
static csn_decoder_error_t csn_pkt_uplk_ass(
  csn_decode_memory_instance_t *m,
  uint8 src[],
  uint16 *pos_ptr,
  packet_uplink_assignment_t *msg_ptr,
  const gas_id_t gas_id
);
static csn_decoder_error_t csn_pkt_psi13(
  csn_decode_memory_instance_t *m,
  uint8 src[],
  uint16 *pos_ptr,
  packet_psi13_t *msg_ptr,
  const gas_id_t gas_id
);
static csn_decoder_error_t csn_pkt_psi14(
  csn_decode_memory_instance_t *m,
  uint8 src[],
  uint16 *pos_ptr,
  packet_psi14_t *msg_ptr,
  const gas_id_t gas_id
);

/*  Discard functions (for parameters that are not required) */

#ifdef PERLUTF
#error code not present
#else
static void csn_discard_lsa_information(uint8 src[], uint16 *pos_ptr);
#endif /*PERLUTF*/

static void csn_discard_lsa_parameters(uint8 src[], uint16 *pos_ptr);

static void csn_discard_iu_mode_cell_selection(uint8 src[], uint16 *pos_ptr);
static void csn_discard_iu_mode_ncell_params(uint8 src[], uint16 *pos_ptr);
static void csn_discard_iu_mode_only_cell_selection(uint8 src[], uint16 *pos_ptr);
static void csn_discard_iu_mode_only_cell_list(uint8 src[], uint16 *pos_ptr);

/*  Message part extract functions */

static csn_decoder_error_t csn_reject_struct_extract(uint8 *src, uint16 *pos_ptr, const packet_access_reject_t *message_ptr);
static csn_decoder_error_t csn_cell_sel_params_extract(uint8 src[], uint16 *pos_ptr, cell_sel_params_t *msg_ptr);
static void  csn_ccn_support_description_extract(uint8 src[], uint16 *pos_ptr, csn_ccn_support_desc_t *msg_ptr);

static void csn_repeated_page_extract(uint8 src[], uint16 *pos_ptr, repeated_page_t *msg_ptr);
static void csn_dynamic_alloc_extract(uint8 src[], uint16 *pos_ptr, dynamic_alloc_t *msg_ptr, uint8);
static void csn_sngl_blk_alloc_extract(uint8 src[], uint16 *pos_ptr, sngl_blk_alloc_t *msg_ptr);
static void csn_compact_reduced_ma_ie(uint8 src[], uint16 *pos_ptr, compact_reduced_ma_t *msg_ptr);
static void csn_multi_blk_alloc_extract(uint8 src[], uint16 *pos_ptr, multi_blk_alloc_t *msg_ptr);
static void csn_egprs_puan_desc_ie(uint8 src[], uint16 *pos_ptr, egprs_ack_nack_des_t *msg_ptr);

/*  Message IE processing functions */

static csn_decoder_error_t csn_frequency_params_ie(
  uint8 src[],
  uint16 *pos_ptr,
  freq_params_t *msg_ptr,
  const gas_id_t gas_id
);
static void csn_ack_nack_desc_ie(uint8 src[], uint16 *pos_ptr, ack_nack_desc_t *msg_ptr);
static void csn_global_pwr_ctrl_ie(uint8 src[], uint16 *pos_ptr, global_pwr_ctrl_t *msg_ptr);
static void csn_global_tfi_ie(uint8 *src, uint16 *pos_ptr, global_tfi_t *msg_ptr);
static csn_decoder_error_t csn_gprs_mobile_alloc_ie(uint8 src[], uint16 *pos_ptr, gprs_mobile_alloc_t *msg_ptr);
static void csn_pkt_timing_advance_ie(uint8 *src, uint16 *pos_ptr, pkt_timing_adv_t *message_ptr);
static void csn_global_pkt_timing_ie(uint8 srci[], uint16 *pos_ptr, global_pkt_timing_t *msg_ptr);
static void csn_pwr_cntrl_params_ie(uint8 src[], uint16 *pos_ptr, pwr_ctrl_paramtrs_t *msg_ptr);
static void csn_start_frm_num_ie(uint8 src[], uint16 *pos_ptr, start_frame_number_t *msg_ptr);
static void csn_gprs_cell_options_ie(uint8 src[], uint16 *pos_ptr, gprs_cell_options_t *msg_ptr);
static void csn_extension_bits_ie(uint8 src[], uint16 *pos_ptr, extension_bits_t *msg_ptr);

static csn_decoder_error_t csn_decode_si1_rest_octets(uint8 *src, si1_rest_octets_t *dst);
static csn_decoder_error_t csn_decode_si3_rest_octets(uint8 *src, si3_rest_octets_t *dst);
static csn_decoder_error_t csn_decode_si4_rest_octets(uint8 *src, si4_rest_octets_t *dst);
static csn_decoder_error_t csn_decode_si6_rest_octets(uint8 *src, si6_rest_octets_t *dst);
static csn_decoder_error_t csn_decode_si7_si8_rest_octets(uint8 *src, struct si7_8_rest_octets_t *dst);
static csn_decoder_error_t csn_decode_si9_rest_octets(uint8 *src, si9_rest_octets_t *dst);
static csn_decoder_error_t csn_decode_si13_rest_octets(uint8 *src, si13_rest_octets_t *dst);
static csn_decoder_error_t csn_decode_p1_rest_octets(
  uint8 *src,
  p1_rest_octets_t *dst,
  const gas_id_t gas_id
);
static csn_decoder_error_t csn_decode_p2_rest_octets(uint8 *src, p2_rest_octets_t *dst);
static csn_decoder_error_t csn_decode_p3_rest_octets(uint8 *src, p3_rest_octets_t *dst);
static csn_decoder_error_t csn_decode_packet_channel_description(
  uint8 *src,
  packet_channel_description_t *dst,
  const gas_id_t gas_id
);
static csn_decoder_error_t csn_decode_ia_rest_octets(uint8 *src, ia_rest_octets_t *dst);
static csn_decoder_error_t csn_decode_iar_rest_octets(uint8 *src, iar_rest_octets_t *dst);

static csn_decoder_error_t csn_decode_packet_measurement_order(
  csn_decode_memory_instance_t *mem,
  uint8 *src,
  uint16 *pos_ptr,
  packet_measurement_order_t *dst,
  const gas_id_t gas_id
);
static csn_decoder_error_t csn_decode_packet_cell_change_order(
  csn_decode_memory_instance_t *mem,
  uint8 *src,
  uint16 *pos_ptr,
  packet_cell_change_order_t *dst,
  const gas_id_t gas_id
);

static csn_decoder_error_t csn_decode_packet_cell_change_continue(
  csn_decode_memory_instance_t *mem,
  uint8 *src,
  uint16 *pos_ptr,
  packet_cell_change_continue_t *msg_ptr,
  const gas_id_t gas_id
);
static csn_decoder_error_t csn_decode_packet_ncell_data(
  csn_decode_memory_instance_t *mem,
  uint8 *src,
  uint16 *pos_ptr,
  packet_ncell_data_t *dst,
  const gas_id_t gas_id
);

static csn_decoder_error_t csn_decode_packet_scell_data(
  csn_decode_memory_instance_t *mem,
  uint8 *src,
  uint16 *pos_ptr,
  packet_scell_data_t *dst,
  const gas_id_t gas_id
);
static csn_decoder_error_t csn_decode_packet_scell_si(
  csn_decode_memory_instance_t *mem,
  uint8 *src,
  uint16 *pos_ptr,
  packet_serving_cell_si_t *msg_ptr,
  const gas_id_t gas_id
);
static csn_decoder_error_t csn_decode_pkt_cs_command(
  csn_decode_memory_instance_t *mem,
  uint8 *src,
  uint16 *pos_ptr,
  packet_cs_command_t *msg_ptr,
  const gas_id_t gas_id
);
static csn_decoder_error_t csn_decode_packet_si_data(
  csn_decode_memory_instance_t *mem,
  uint8 *src,
  uint16 *pos_ptr,
  packet_si_data_t *msg_ptr,
  const gas_id_t gas_id
);

#ifdef FEATURE_GSM_DTM
static csn_decoder_error_t csn_decode_gprs_broadcast_information(uint8 *src, gprs_broadcast_information_t *dst);
static csn_decoder_error_t csn_decode_dtm_information_details(uint8 *src, dtm_information_details_t *dst);
static void csn_decode_rr_pua_dynamic_alloc(uint8 *src, uint16 *msg_pos, rr_pua_dynamic_alloc_t *dst);
static csn_decoder_error_t csn_decode_rr_packet_uplink_assignment(uint8 *src, rr_packet_uplink_assignment_t *dst);
static csn_decoder_error_t csn_decode_rr_packet_downlink_assignment(uint8 *src, rr_packet_downlink_assignment_t *dst);
#ifdef FEATURE_GSM_EDTM
static csn_decoder_error_t csn_decode_packet_cs_release_ind(
  csn_decode_memory_instance_t *mem,
  uint8 *src,
  uint16 *pos_ptr,
  packet_cs_release_ind_t *msg_ptr,
  const gas_id_t gas_id
);
#endif /* FEATURE_GSM_EDTM */
#endif /* FEATURE_GSM_DTM */
#ifdef FEATURE_GPRS_PS_HANDOVER
static csn_decoder_error_t csn_decode_ps_handover_radio_resources(
  uint8 src[],
  uint16 *pos_ptr,
  ps_handover_radio_resources_t *msg_ptr,
  const gas_id_t gas_id
);
static void csn_decode_gprs_mode_extract(
  uint8 src[],
  uint16 *pos_ptr,
  gprs_mode_t *msg_ptr
);
static void csn_decode_egprs_mode_extract(
  uint8 src[],
  uint16 *pos_ptr,
  egprs_mode_t *msg_ptr
);
static void csn_decode_egprs_downlink_assignments(
  uint8 src[],
  uint16 *pos_ptr,
  egprs_dl_tbf_t *msg_ptr
);
static void csn_decode_egprs_uplink_assignments(
  uint8 src[],
  uint16 *pos_ptr,
  egprs_ul_tbf_t *msg_ptr
);
static void csn_decode_ul_tbf_assign(
  uint8 src[],
  uint16 *pos_ptr,
  ul_tbf_assignment_t *msg_ptr,uint8 num_slots
);
static void csn_decode_downlink_assign(
  uint8 src[],
  uint16 *pos_ptr,
  dl_assignment_t *msg_ptr
);

static csn_decoder_error_t csn_decode_ps_handover_command(
  csn_decode_memory_instance_t *mem,
  uint8 *src,
  uint16 *pos_ptr,
  ps_handover_command_t *msg_ptr,
  const gas_id_t gas_id
);
 static csn_decoder_error_t csn_decode_packet_physical_information(
  csn_decode_memory_instance_t *mem,
  uint8 *src,
  uint16 *pos_ptr,
  packet_physical_information_t *msg_ptr,
  const gas_id_t gas_id
);
#endif /*FEATURE_GPRS_PS_HANDOVER*/


/*****************************************************************************

            Memory allocation and de-allocation

*****************************************************************************/
static csn_decode_memory_instance_t* csn_decode_malloc_start(void);
static void *csn_decode_malloc(csn_decode_memory_instance_t *mem, size_t size);
#define CSN_DECODE_MEM_MALLOC(size) csn_decode_malloc(mem, size)

/*****************************************************************************
***
***     Public Data Declarations
***
***     Ensure an external reference is added to the module's header file!
***
*****************************************************************************/

#define gunpackb(src, pos_ptr, len) csn_safe_ul_msg_gunpackb(src, pos_ptr, len)

#define gunpackw(src, pos_ptr, len) csn_safe_ul_msg_gunpackw(src, pos_ptr, len)

#define gunpackd(src, pos_ptr, len) csn_safe_ul_msg_gunpackd(src, pos_ptr, len)

#define gunpackb_lh(src, pos_ptr) csn_safe_ul_msg_gunpackb_lh(src, pos_ptr)

#define gunpackq(src, pos_ptr, len) csn_safe_ul_msg_gunpackq(src, pos_ptr, len)

/*===========================================================================
===
===  FUNCTION      csn_safe_ul_msg_gunpackb
===
===  DESCRIPTION
===
===    Given a buffer and an offset, unpacks the requested number of bits
===    into a byte and advances the position pointer.
===
===  PARAMETERS
===
===    src:     byte value representing external over-the-air data.
===
===    pos_ptr: pointer to the position var. Ranges from 0 (MSBit) to 7 (LSBit)
===
===              MSB           LSB
===             +-+-+-+-+-+-+-+-+
===             |8|7|6|5|4|3|2|1|
===             +-+-+-+-+-+-+-+-+
===             |0 0 0 1 1 1 1 0|
===             +-+-+-+-+-+-+-+-+
===              ------> *pos_ptr = 3, len = 4
===
===    len:     length of data item in bits
===
===  DEPENDENCIES
===
===    Invokes b_unpackb()
===
===  RETURN VALUE
===
===    Right-aligned byte representing a field extracted from the external
===    data byte given the bit position offset from the left-hand-side and
===    the length of the byte.
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
uint8 csn_safe_ul_msg_gunpackb
(
   uint8  src[],
   uint16 *pos_ptr,
   uint16 len
)
{
  uint8 by =0;

  if((*pos_ptr + len) <= MAX_CSN_OTA_MSG_IN_OCTETS * 8)
  {
    by = b_unpackb(src, *pos_ptr, len);
    *pos_ptr += len;
  }
  else
  {
    MSG_GERAN_MED_3("over read occured gunpack B current pos %d  len %d  max space %d ",
               *pos_ptr,len,(MAX_CSN_OTA_MSG_IN_OCTETS * 8));
  }

  return by;
}

/*===========================================================================
===
===  FUNCTION      csn_safe_ul_msg_gunpackw
===
===    Given a buffer and an offset, unpacks the requested number of bits
===    into a word and advances the position pointer.
===
===  PARAMETERS
===
===    src:     byte value representing external over-the-air data.
===
===    pos_ptr: pointer to the position var.
===
===    len:     length of data item in bits
===
===  DEPENDENCIES
===
===    Invokes b_unpackw()
===
===  RETURN VALUE
===
===    Right-aligned word
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
uint16 csn_safe_ul_msg_gunpackw
(
   uint8  src[],
   uint16 *pos_ptr,
   uint16 len
)
{

  uint16 by =0;

  if((*pos_ptr + len) <= MAX_CSN_OTA_MSG_IN_OCTETS * 8 )
  {
    by = b_unpackw(src, *pos_ptr, len);
    *pos_ptr += len;
  }
  else
  {
    MSG_GERAN_MED_3("over read occured gunpack w current pos %d  len %d  max space %d ",
               *pos_ptr,len,(MAX_CSN_OTA_MSG_IN_OCTETS * 8));
  }

  return by;
}

/*===========================================================================
===
===  FUNCTION      GUNPACKD
===
===  DESCRIPTION
===
===    Given a buffer and an offset, unpacks the requested number of bits
===    into a dword and advances the position pointer.
===
===  PARAMETERS
===
===    src:     byte value representing external over-the-air data.
===
===    pos_ptr: pointer to the position var.
===
===    len:     length of data item in bits
===
===  DEPENDENCIES
===
===    Invokes b_unpackd()
===
===  RETURN VALUE
===
===    Right-aligned dword
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
uint32 csn_safe_ul_msg_gunpackd
(
   uint8  src[],
   uint16 *pos_ptr,
   uint16 len
)
{

  uint32 by =0;

  if((*pos_ptr + len) <= MAX_CSN_OTA_MSG_IN_OCTETS * 8 )
  {
    by = b_unpackd(src, *pos_ptr, len);
    *pos_ptr += len;
  }
  else
  {
    MSG_GERAN_MED_3("over read occured gunpack d current pos %d  len %d  max space %d ",
               *pos_ptr,len,(MAX_CSN_OTA_MSG_IN_OCTETS * 8));
  }

  return by;
}


/*===========================================================================
===
===  FUNCTION      GUNPACKB_LH
===
===  DESCRIPTION
===
===    Given a buffer and an offset, unpacks a single bit, advancing the offset
===    (position pointer). Calculates the value of the bit position in CSN1 L|H
===    padding, (0x2b padding pattern), and returns the bit XOR'd with this value
===
===  PARAMETERS
===
===    src:     byte value representing external over-the-air data.
===
===    pos_ptr: pointer to the position var. Ranges from 0 (MSBit) to 7 (LSBit)
===
===              MSB           LSB
===             +-+-+-+-+-+-+-+-+
===             |0|0|1|0|1|0|1|1|  0x2b padding pattern
===             +-+-+-+-+-+-+-+-+
===             |8|7|6|5|4|3|2|1|  bit numbering
===             +-+-+-+-+-+-+-+-+
===             |0 0 0 1 1 1 1 0|  example bit stream
===             +-+-+-+-+-+-+-+-+
===                ^ ^   ^
===                | |   | position pointer 4, return value is 0
===                | |
===                | | position pointer 2, return value is 1
===                |
===                | position pointer 1, return value is 0
===
=== eg a bit is considered set if it's value is the inverse of the same bit position
=== in the csn1 padding pattern, 0010 1011 (0x2b)
===
===
===  DEPENDENCIES
===
===    Invokes b_unpackb()
===
===  RETURN VALUE
===
===    Right-aligned byte representing a field extracted from the external
===    data byte given the bit position offset from the left-hand-side and
===    the value of the same bit position in the padding pattern 0x2b.
===    This function returns the meaning of the bit, not the value.
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
uint8 csn_safe_ul_msg_gunpackb_lh
(
   uint8  src[],
   uint16 *pos_ptr
)
{
  uint8 bit_offset = *pos_ptr % 8;
  uint8 csn1_padding_bit;
  uint8 by =0;


  /*only decodes 1 bit*/
  if((*pos_ptr + 1) <= MAX_CSN_OTA_MSG_IN_OCTETS * 8 )
  {
    csn1_padding_bit = (0x2b >> (7 - bit_offset)) & 0x01;
    by = b_unpackb(src, *pos_ptr, 1) ^ csn1_padding_bit;
    *pos_ptr += 1;
  }
  else
  {
    MSG_GERAN_MED_3("over read occured gunpack lh current pos %d  len 1  max space %d ",
              *pos_ptr,0,(MAX_CSN_OTA_MSG_IN_OCTETS * 8));
  }

  return by;
}


/*===========================================================================
===
===  FUNCTION      GUNPACKQ
===
===  DESCRIPTION
===
===    Given a buffer and an offset, unpacks the requested number of bits
===    into a dword and advances the position pointer.
===
===  PARAMETERS
===
===    src:     byte value representing external over-the-air data.
===
===    pos_ptr: pointer to the position var.
===
===    len:     length of data item in bits
===
===  DEPENDENCIES
===
===    Invokes b_unpackd()
===
===  RETURN VALUE
===
===    Right-aligned dword
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/

uint64 csn_safe_ul_msg_gunpackq (uint8 src[], uint16 *pos_ptr, uint16 len)
{
  uint32 by = 0;
  uint64 result = 0;

  /*only decodes 1 bit*/
  if((*pos_ptr + len) <= MAX_CSN_OTA_MSG_IN_OCTETS * 8 )
  {
    while (len > 16)
    {
      len      -= 16;
      by        = b_unpackd(src, *pos_ptr, 16);
      *pos_ptr += 16;
      result   |= (uint64)by;
      result  <<= 16;
    }

    /*  Account for non integer 16 values */
    result >>= 16;

    if (len)
    {
      result  <<= len;
      by        = b_unpackd(src, *pos_ptr, len);
      *pos_ptr += len;
      result   |= (uint64)by;
    }
  }
  else
  {
    MSG_GERAN_MED_3("over read occured gunpack q current pos %d  len %d  max space %d ",
               *pos_ptr,len,(MAX_CSN_OTA_MSG_IN_OCTETS * 8));
  }

  return (result);
}


/*****************************************************************************
***
***     Private Macros
***
*****************************************************************************/

/*===========================================================================
===
===  MACRO         MIN_IA_RO_*_R99_ADDITIONS_LENGTH
===
===  DESCRIPTION
===
===    The minimum number of bits required to contain the R99 additions
===    to the RR IA Rest Octets [Packet Downlink Assignment|Packet Uplink
===    Assignment|Second Part Packet Assignment]
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/
#define MIN_IA_RO_PDA_R99_ADDITIONS_LENGTH \
        (CSN_EGPRS_WINDOW_SIZE_LEN + CSN_LINK_QUALITY_MEASUREMENT_MODE_LEN + 2)
#define MIN_IA_RO_PUA_R99_ADDITIONS_LENGTH    2
/* Tell lint not to complain about macro that is not referenced */
/*lint -esym(750,MIN_IA_RO_SPPA_R99_ADDITIONS_LENGTH)*/
#define MIN_IA_RO_SPPA_R99_ADDITIONS_LENGTH   2

/* Macros to extract boolean type from a message bit */
#define GUNPACKS(src, msg_pos, bits) gunpackb(src, msg_pos, bits) == 0 ? FALSE : TRUE;
#define GUNPACKS_LH(src, msg_pos)    gunpackb_lh(src, msg_pos)    == 0 ? FALSE : TRUE;

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

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
===    none
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/


/*****************************************************************************

  Much of the processing involved in decoding CSN.1 messages is identical.
  This table has an entry for each message type (for speedy look-up it has
  empty entries where no message type is defined) that define the factors
  specific to each message:
    * the message type (used to validate the table)
    * the size of the message when decoded (used to allocate memory)
    * the function that decodes the message

*****************************************************************************/

#define CSN_DECODE_MEMORY_IN_USE(mptr){\
 mptr->csn_decode_reserved_memory_used = mptr->csn_decode_reserved_memory_used;}

static csn_decoder_error_t csn_decode_bad_mess_type(
  csn_decode_memory_instance_t *mem,
  uint8 *src, uint16 *msg_pos, uint8 *dst
)
{
  CSN_DECODE_MEMORY_IN_USE(mem);

  if (msg_pos != NULL)
  {
    /* reset the decode position to the start of the message */
    *msg_pos = 0;

    /* then extract the first six bits of message type */
    *dst = gunpackb(src, msg_pos, CSN_MESSAGE_TYPE_LEN);

    /* and output the result */
    MSG_GERAN_ERROR_1("CSN.1: unknown message type 0x%02X", (int32)*dst);
  }

  return UNKNOWN_MESSAGE_TYPE;
}

typedef csn_decoder_error_t (*gcsn1_decoder_fn_ptr_t) (
                                csn_decode_memory_instance_t *,
                                uint8 *,
                                uint16 *,
                                void *,
                                const gas_id_t gas_id
                              );

typedef struct
{
  uint8 mess_type;  /* (used only to validate table) */
  uint32 size;
  gcsn1_decoder_fn_ptr_t fn_ptr;
} gcsn1_decoder_table_entry_t;

/* Macro to simplify the table entries */
#define GCSN1_DECODER_TABLE_ENTRY(mess_type, message, function) \
  { mess_type, sizeof(message), (gcsn1_decoder_fn_ptr_t)function }

/* Used when the index to the table does not have a corresponding
   message type */
#define EMPTY_ENTRY \
        GCSN1_DECODER_TABLE_ENTRY(PACKET_INVALID_MSG, uint8, csn_decode_bad_mess_type)

/* The table */
static gcsn1_decoder_table_entry_t gcsn1_decoder_table[] =
{
  /*0x00*/GCSN1_DECODER_TABLE_ENTRY(PACKET_SI_DATA,\
                                    packet_si_data_t,\
                                    csn_decode_packet_si_data),
  /*0x01*/GCSN1_DECODER_TABLE_ENTRY(PACKET_CELL_CHANGE_ORDER,\
                                    packet_cell_change_order_t,\
                                    csn_decode_packet_cell_change_order),
  /*0x02*/GCSN1_DECODER_TABLE_ENTRY(PACKET_DOWNLINK_ASSIGNMENT,\
                                    packet_downlink_assignment_t,\
                                    csn_pkt_dwnlk_assgn),
  /*0x03*/GCSN1_DECODER_TABLE_ENTRY(PACKET_MEASUREMENT_ORDER,\
                                    packet_measurement_order_t,\
                                    csn_decode_packet_measurement_order),
  /*0x04*/GCSN1_DECODER_TABLE_ENTRY(PACKET_POLLING_REQUEST,\
                                    packet_polling_request_t,\
                                    csn_pkt_poll_req),
  /*0x05*/GCSN1_DECODER_TABLE_ENTRY(PACKET_PWR_CTRL_TMNG_ADV,\
                                    packet_pwr_ctrl_tmng_adv_t,\
                                    csn_pkt_pwrctrl_ta),
  /*0x06*/EMPTY_ENTRY,
  /*0x07*/GCSN1_DECODER_TABLE_ENTRY(PACKET_TIMESLOT_RECONFIGURE,\
                                    packet_timeslot_reconfigure_t,\
                                    csn_pkt_timeslot_reconf),
  /*0x08*/GCSN1_DECODER_TABLE_ENTRY(PACKET_TBF_RELEASE,\
                                    packet_tbf_release_t,\
                                    csn_pkt_tbf_release),
  /*0x09*/GCSN1_DECODER_TABLE_ENTRY(PACKET_UPLINK_ACK_NACK,\
                                    packet_uplink_ack_nack_t,\
                                    csn_pkt_uplk_ack_nack),
  /*0x0A*/GCSN1_DECODER_TABLE_ENTRY(PACKET_UPLINK_ASSIGNMENT,\
                                    packet_uplink_assignment_t,\
                                    csn_pkt_uplk_ass),
  /*0x0B*/GCSN1_DECODER_TABLE_ENTRY(PACKET_CELL_CHANGE_CONTINUE,\
                                    packet_cell_change_continue_t,\
                                    csn_decode_packet_cell_change_continue),
  /*0x0C*/GCSN1_DECODER_TABLE_ENTRY(PACKET_NEIGHBOUR_CELL_DATA,\
                                    packet_ncell_data_t,\
                                    csn_decode_packet_ncell_data),
  /*0x0D*/GCSN1_DECODER_TABLE_ENTRY(PACKET_SERVING_CELL_DATA,\
                                    packet_scell_data_t,\
                                    csn_decode_packet_scell_data),
  /*0x0E*/EMPTY_ENTRY,
  /*0x0F*/EMPTY_ENTRY,
  /*0x10*/EMPTY_ENTRY,
  /*0x11*/EMPTY_ENTRY,
  /*0x12*/EMPTY_ENTRY,
  /*0x13*/EMPTY_ENTRY,
  /*0x14*/EMPTY_ENTRY,
#ifdef FEATURE_GPRS_PS_HANDOVER
  /*0x15*/GCSN1_DECODER_TABLE_ENTRY(PACKET_PS_HANDOVER,\
                                    ps_handover_command_t,\
                                    csn_decode_ps_handover_command),
  /*0x16*/GCSN1_DECODER_TABLE_ENTRY(PACKET_PHYSICAL_INFORMATION,\
                                    packet_physical_information_t,\
                                    csn_decode_packet_physical_information),
#else
  /*0x15*/EMPTY_ENTRY,
  /*0x16*/EMPTY_ENTRY,
#endif /*FEATURE_GPRS_PS_HANDOVER*/
  /*0x17*/EMPTY_ENTRY,
  /*0x18*/EMPTY_ENTRY,
  /*0x19*/EMPTY_ENTRY,
  /*0x1A*/EMPTY_ENTRY,
  /*0x1B*/EMPTY_ENTRY,
  /*0x1C*/EMPTY_ENTRY,
  /*0x1D*/EMPTY_ENTRY,
  /*0x1E*/EMPTY_ENTRY,
  /*0x1F*/EMPTY_ENTRY,
  /*0x20*/GCSN1_DECODER_TABLE_ENTRY(PACKET_SERVING_CELL_SI,\
                                    packet_serving_cell_si_t,\
                                    csn_decode_packet_scell_si),
  /*0x21*/GCSN1_DECODER_TABLE_ENTRY(PACKET_ACCESS_REJECT,\
                                    packet_access_reject_t,\
                                    csn_pkt_acc_rej),
  /*0x22*/GCSN1_DECODER_TABLE_ENTRY(PACKET_PAGING_REQUEST,\
                                    packet_paging_request_t,\
                                    csn_pkt_page_req),
  /*0x23*/GCSN1_DECODER_TABLE_ENTRY(PACKET_PDCH_RELEASE,\
                                    packet_pdch_release_t,\
                                    csn_pkt_pdch_release),
  /*0x24*/EMPTY_ENTRY,
  /*0x25*/GCSN1_DECODER_TABLE_ENTRY(PACKET_DOWNLINK_DUMMY_CNTRL_BLK,\
                                    packet_dwnlk_dummy_cntrl_blk_t,\
                                    csn_pkt_dwnlk_dummy_cntrl_blk),
  /*0x26*/EMPTY_ENTRY,
  /*0x27*/GCSN1_DECODER_TABLE_ENTRY(PACKET_CS_COMMAND,\
                                    packet_cs_command_t,\
                                    csn_decode_pkt_cs_command),
  /*0x28*/EMPTY_ENTRY,
#if defined(FEATURE_GSM_DTM) && defined(FEATURE_GSM_EDTM)
  /*0x29*/GCSN1_DECODER_TABLE_ENTRY(PACKET_CS_RELEASE,\
                                    packet_cs_release_ind_t,\
                                    csn_decode_packet_cs_release_ind),
#else
  /*0x29*/EMPTY_ENTRY,
#endif /* FEATURE_GSM_DTM && FEATURE_GSM_EDTM */
  /*0x2A*/EMPTY_ENTRY,
  /*0x2B*/EMPTY_ENTRY,
  /*0x2C*/EMPTY_ENTRY,
  /*0x2D*/EMPTY_ENTRY,
  /*0x2E*/EMPTY_ENTRY,
  /*0x2F*/EMPTY_ENTRY,
  /*0x30*/EMPTY_ENTRY,
  /*0x31*/EMPTY_ENTRY,
  /*0x32*/EMPTY_ENTRY,
  /*0x33*/EMPTY_ENTRY,
  /*0x34*/EMPTY_ENTRY,
  /*0x35*/EMPTY_ENTRY,
  /*0x36*/EMPTY_ENTRY,
  /*0x37*/GCSN1_DECODER_TABLE_ENTRY(PACKET_PSI13,\
                                    packet_psi13_t,\
                                    csn_pkt_psi13),
  /*0x38*/EMPTY_ENTRY,
  /*0x39*/EMPTY_ENTRY,
  /*0x3a*/GCSN1_DECODER_TABLE_ENTRY(PACKET_PSI14,\
                                    packet_psi14_t,\
                                    csn_pkt_psi14),
  /*0x3b*/EMPTY_ENTRY,
  /*0x3c*/EMPTY_ENTRY,
  /*0x3d*/EMPTY_ENTRY,
  /*0x3e*/EMPTY_ENTRY,
  /*0x3f*/EMPTY_ENTRY,
};

/*****************************************************************************/

csn_decoder_ptrs_t gcsn1_decoder(
  uint8 *src_ota,
  uint16 *bits_read,
  uint16 ota_msg_size_octets,
  const gas_id_t gas_id
)
{
  csn_decoder_ptrs_t  csn_decoder_ptrs = {NULL,NULL,NULL};
  rlc_mac_cntrl_t     *rlc_mac_hdr = NULL;    /* destination internal format */
  uint8               *unstruct_ptr = NULL;   /*  Unstructured pointer */
  uint8               mess_type;
  uint8               pg_mode;
  uint16              msg_pos;
  static rlc_mac_cntrl_t error_return; /* static_dual_spaced_ignore - only used if fatal error */
  gcsn1_decoder_table_entry_t *table_entry;
  static boolean      initialised = FALSE; /* static_dual_spaced_ignore - only used if checking for fatal error */
  csn_decode_memory_instance_t* mem;

  /*use local buffer of max size to protect against over reads of OTA buffer*/
  uint8 src[MAX_CSN_OTA_MSG_IN_OCTETS];


  memset(src,0,MAX_CSN_OTA_MSG_IN_OCTETS);

  if(ota_msg_size_octets > MAX_CSN_OTA_MSG_IN_OCTETS )
  {
   memscpy(src,MAX_CSN_OTA_MSG_IN_OCTETS,src_ota,MAX_CSN_OTA_MSG_IN_OCTETS);
  }
  else
  {
   memscpy(src,MAX_CSN_OTA_MSG_IN_OCTETS,src_ota,ota_msg_size_octets);
  }

  error_return.error_code = CSN_DECODE_NO_ERROR;

  if (!initialised)
  { /* Do anything necessary to initialise this function */
    uint8 i;
    /* Validate table - always a good idea with table-driven software */
    for (i = 0;
        i < (sizeof(gcsn1_decoder_table) / sizeof(gcsn1_decoder_table[0]));
        i++)
    {
      if ((gcsn1_decoder_table[i].mess_type != i) &&
          (gcsn1_decoder_table[i].fn_ptr != (gcsn1_decoder_fn_ptr_t)csn_decode_bad_mess_type))
      { /* (It's an EMPTY_ENTRY) */
        MSG_GERAN_FATAL_1("Broken gcsn1_decoder_table 0x%02x", i);
        error_return.error_code = UNKNOWN_MESSAGE_TYPE;
        csn_decoder_ptrs.header_ptr = &error_return;
        return csn_decoder_ptrs;
      }
    }
    /*lint -e{506} Constant value Boolean */
    /*lint -e{774} Boolean within 'if' always evaluates to False */
    if ((sizeof(gcsn1_decoder_table) / sizeof(gcsn1_decoder_table[0])) !=
        (1 << CSN_MESSAGE_TYPE_LEN))
    {
      MSG_GERAN_FATAL_1("gcsn1_decoder_table wrong size 0x%02x", i);
      error_return.error_code = UNKNOWN_MESSAGE_TYPE;
      csn_decoder_ptrs.header_ptr = &error_return;
      return csn_decoder_ptrs;
    }
    initialised = TRUE;
  }

  mem = csn_decode_malloc_start();

  /*  Pointer to bit position. */
  msg_pos=0;

  /* Extract the Message type, first 6 bits. Extract to local variable. */
  /* (No need to range check mess_type, the table it indexes has 0x3F entries) */
  mess_type = gunpackb(src, &msg_pos, CSN_MESSAGE_TYPE_LEN);

  /* Extract the Page Mode, 2 bits. Extract to local variable. */
  pg_mode = gunpackb(src, &msg_pos, CSN_PAGE_MODE_LEN);

  table_entry = &gcsn1_decoder_table[mess_type];
  /*  Malloc appropriate area and set unstruct_ptr to point to it. */
  unstruct_ptr = (uint8 *)CSN_DECODE_MEM_MALLOC(table_entry->size /
                                         sizeof(uint8) + CSN_DECODE_HDR_SZ);

  if (unstruct_ptr == NULL)
  {
    MSG_GERAN_FATAL_0("CSN_1: ERROR MEM MALLOC FAILED");
    error_return.error_code = NO_MEMORY;
    csn_decoder_ptrs.header_ptr = &error_return;
    return csn_decoder_ptrs;
  }

  /*  Set structed pointer to start as well to allow header bytes to be */
  /*  added later. */
  /*lint -e{826} Suspicious pointer-to-pointer conversion (area too small) */
  rlc_mac_hdr = (rlc_mac_cntrl_t *)unstruct_ptr;

  /*  Move unstruct_ptr to make room for header bytes. */
  unstruct_ptr += CSN_DECODE_HDR_SZ;
  rlc_mac_hdr->error_code = table_entry->fn_ptr(
                                           mem,
                                           src,
                                           &msg_pos,
                                           unstruct_ptr,
                                           gas_id
                                         );

  /* F3 the PSI Messages. */
  if ((mess_type & 0x30) == 0x30)
  {
    MSG_GERAN_MED_1("CSN PSI Type=0x%02X",mess_type);
  }

  /*  Update message type and page mode in structure, from local variables. */
  rlc_mac_hdr->message_type = (uint8)mess_type;
  rlc_mac_hdr->page_mode = pg_mode;

  /*  Return a structure containing three pointers, one to the allocated
      memory, one to the header and one to the data.
  */
  csn_decoder_ptrs.allocated_memory_ptr = mem;
  csn_decoder_ptrs.header_ptr = rlc_mac_hdr;
  csn_decoder_ptrs.data_ptr = unstruct_ptr;
  *bits_read = msg_pos;
  return (csn_decoder_ptrs);
}

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

/*public*/void gcsn1_decoder_size_of_rcvd_dl_control(uint8 size_in_octets)
{
    dl_control_msg_size = (uint16)size_in_octets;
}



/*****************************************************************************
***
***     Message Extract functions to follow
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      csn_pkt_acc_rej
===
===  DESCRIPTION
===
===    This function extracts the information from the PACKET ACCESS REJECT
===    message.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/

static csn_decoder_error_t csn_pkt_acc_rej(
  csn_decode_memory_instance_t *mem,
  uint8 src[], uint16 *pos_ptr,
  packet_access_reject_t *msg_ptr,
  const gas_id_t gas_id
)
{
  csn_decoder_error_t return_status = CSN_DECODE_NO_ERROR;
  uint8 trap_count=MAX_NO_REJ_STRUCTS;

  CSN_DECODE_MEMORY_IN_USE(mem);

  /* First information packet is Reject Structure */
  /* Store contents in first reject structure, reject_struct0 */
  msg_ptr->rej_ptr = &(msg_ptr->reject_struct0);

  return_status = csn_reject_struct_extract(src, pos_ptr, msg_ptr);

  /* Determine if reject structure is repeated. */
  /* Structure can be repeated upto three more times, put trap in */
  /* to catch infinite repeat */
  while (gunpackb(src, pos_ptr, (int16)1) && trap_count)
  {
    /* Increment rej_ptr to allow all results to be stored. */
    msg_ptr->rej_ptr++;

    /* Extract next reject structure */
    return_status = csn_reject_struct_extract(src, pos_ptr, msg_ptr);

    /*  Manage trap counter to ensure execution does not */
    /*  get stuck in loop */
    trap_count--;
  };

  /*  Record number of additional reject structures */
  msg_ptr->no_addition_rej_structs = (uint8)(MAX_NO_REJ_STRUCTS - trap_count);

  return(return_status);

}

/*===========================================================================
===
===  FUNCTION      csn_pkt_dwnlk_assgn
===
===  DESCRIPTION
===
===    This function extracts the information from the PACKET DOWNLINK ASSIGNMENT
===    message.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/

static csn_decoder_error_t csn_pkt_dwnlk_assgn(
  csn_decode_memory_instance_t *mem,
  uint8 src[], uint16 *pos_ptr,
  packet_downlink_assignment_t *msg_ptr,
  const gas_id_t gas_id
)
{
  csn_decoder_error_t return_status = CSN_DECODE_NO_ERROR;
  int8  persistence_count=0;

  CSN_DECODE_MEMORY_IN_USE(mem);

  msg_ptr->egprs_options_flag = FALSE;

  /*  Clear flag to indicate persistence levels present. */
  msg_ptr->persistence_flag = FALSE;

  /* First information field is optional, may be Persistance Level */
  if (gunpackb(src, pos_ptr, (int16)1))
  {
    while (persistence_count < MAX_PERSIST_VALUES)
    {
      /*  Persistence levels present. */
      msg_ptr->persistence[persistence_count++] =
                                gunpackb(src, pos_ptr, CSN_PERSISTENCE_LEN);
    };

    /*  Set flag to indicate persistence levels present. */
    msg_ptr->persistence_flag = TRUE;
  }

  /*  Next field is the addressing field, determine type of addressing. */
  if (!(gunpackb(src, pos_ptr, (int16)1)))
  {
    /* Address bits are Global TFI */
    csn_global_tfi_ie(src, pos_ptr, &(msg_ptr->global_tfi));

    /*  Set option enum to indicate that the Global TFI is valid */
    msg_ptr->tlli_gtfi_pkt_ref_valid = global_tfi_valid;
  }
  else
  {
    /*  Address should be TLLI but check it's not invalid */
    if (!(gunpackb(src, pos_ptr, (int16)1)))
    {
      /* Address is TLLI, now extract TLLI */
      msg_ptr->tlli = gunpackd(src, pos_ptr, CSN_TLLI_LEN);

      /*  Set option enum to indicate that the TLLI is valid */
      msg_ptr->tlli_gtfi_pkt_ref_valid = tlli_valid;
    }
    else
    {
      /*  An illegally encoded address has been detected. */
      /*  Set return_status. */
      return_status = ADDRESS_INFO_ERROR;
    }
  }

  /*  If an address error has occurred then message escape is implemented */
  if (return_status == CSN_DECODE_NO_ERROR)
  {
    /*  Check message escape bit */
    if (!(gunpackb(src, pos_ptr, (int16)1)))
    {
      /*  Continue extracting info. */
      /*  Next bits are MAC_MODE */
      msg_ptr->mac_mode = gunpackb(src, pos_ptr, CSN_MAC_MODE_LEN);

      /*  Next bit is rlc_mode */
      msg_ptr->rlc_mode = gunpackb(src, pos_ptr, CSN_RLC_MODE_LEN);

      /*  Next bit is control_ack */
      msg_ptr->ctrl_ack = gunpackb(src, pos_ptr, CSN_CTRL_ACK_LEN);

      /*  Next bits are timeslot_allocation */
      msg_ptr->timeslot_alloc =
                      (uint8)gunpackw(src, pos_ptr, CSN_TIMESLOT_ALLOC_LEN);

      /*  Following bits define the packet_timing_advance structure */
      csn_pkt_timing_advance_ie(src, pos_ptr, &(msg_ptr->pkt_timing_adv));

      /* Clear flag before checking power control fields. */
      msg_ptr->dwnlk_pwr_ctrl_flag = pwr_ctrl_not_valid;

      /*  Next field is an optional field */
      if (gunpackb(src, pos_ptr, (int16)1))
      {
        /*  Power Control fields present */
        msg_ptr->dwnlk_pwr_ctrl.po = gunpackb(src, pos_ptr, CSN_PO_LEN);

        msg_ptr->dwnlk_pwr_ctrl.bts_pwr_ctrl_mode =
                          gunpackb(src, pos_ptr, CSN_BTS_PWR_CTRL_MODE_LEN);

        msg_ptr->dwnlk_pwr_ctrl.pr_mode =
                                    gunpackb(src, pos_ptr, CSN_PR_MODE_LEN);

        /* Set flag to indicate power control fields TRUE. */
        msg_ptr->dwnlk_pwr_ctrl_flag = pwr_ctrl_valid;
      }

      /*  Clear frequency parameters present flag. */
      msg_ptr->freq_param_flag = FALSE;

      /*  Next field is an optional field */
      if (gunpackb(src, pos_ptr, (int16)1))
      {
        /*  Frequency parameters are present */
        return_status = csn_frequency_params_ie(src, pos_ptr, &(msg_ptr->freq_params), gas_id);

        if (return_status != CSN_DECODE_NO_ERROR)
        {
          return return_status;
        }

        /*  Set frequency parameters present flag. */
        msg_ptr->freq_param_flag = TRUE;
      }

      /*  Clear Downlink tfi present flag. */
      msg_ptr->dwnlk_tfi_flag = FALSE;

      /*  Next field is an optional field */
      if (gunpackb(src, pos_ptr, (int16)1))
      {
        /*  Extract Downlink tfi assignment */
        msg_ptr->downlink_tfi =
                              gunpackb(src, pos_ptr, CSN_DOWNLINK_TFI_LEN);

        /*  Set Downlink tfi present flag. */
        msg_ptr->dwnlk_tfi_flag = TRUE;
      }

      /*  Clear power control parameters present flag. */
      msg_ptr->pwr_ctrl_flag = FALSE;

      /*  Next field is an optional field */
      if (gunpackb(src, pos_ptr, (int16)1))
      {
        /*  Extract power control parameters */
        csn_pwr_cntrl_params_ie(src, pos_ptr, &(msg_ptr->pwr_ctrl_paramtrs));

        /*  Set power control parameters present flag. */
        msg_ptr->pwr_ctrl_flag = TRUE;
      }

      /*  Clear TBF starting time present flag. */
      msg_ptr->tbf_start_flag = FALSE;

      /*  Next field is an optional field */
      if (gunpackb(src, pos_ptr, (int16)1))
      {
        /*  TBF starting time present */
        csn_start_frm_num_ie(src, pos_ptr, &(msg_ptr->tbf_start_frm_num));

        /*  Set TBF starting time present flag. */
        msg_ptr->tbf_start_flag = TRUE;
      }

      /*  Clear measurement mapping present flag. */
      msg_ptr->meas_map_flag = FALSE;

      /*  Next field is an optional field */
      if (gunpackb(src, pos_ptr, (int16)1))
      {
        /*  Measurement Mapping present */
        /*  Extract measurement start time */
        csn_start_frm_num_ie(src,
                        pos_ptr, &(msg_ptr->measure_mapping.meas_start_time));

        /*  Extract Measurement interval */
        msg_ptr->measure_mapping.measure_interval =
                          gunpackb(src, pos_ptr, CSN_MEASURE_INTERVAL_LEN);

        /*  Extract Measurement bitmap */
        msg_ptr->measure_mapping.measure_bitmap =
                          gunpackb(src, pos_ptr, CSN_MEASURE_BITMAP_LEN);

        /*  Set measurement mapping present flag. */
        msg_ptr->meas_map_flag = TRUE;
      }

      /*  Clear option flag before checking if optional field present */
      msg_ptr->r99_additions_flag = FALSE;

      /*  Add Additions for R99 */
      if (gunpackb(src, pos_ptr, (int16)1))
      {
        /*  Clear option flag before checking if optional field present */

        if (gunpackb(src, pos_ptr, (int16)1))
        {
          /* EGPRS options present, extract window size */
          msg_ptr->egprs_win_sz =
                                gunpackb(src, pos_ptr, CSN_EGPRS_WINDOW_SIZE_LEN);

          /* Extract link quality */
          msg_ptr->egprs_link_qual =
                            gunpackb(src, pos_ptr, CSN_EGPRS_LINK_QUAL_LEN);

          /*  Clear option flag before checking if optional field present */
          msg_ptr->egprs_bep_prd2_flag = FALSE;

          if (gunpackb(src, pos_ptr, (int16)1))
          {
            /* EGPRS bit period field */
            msg_ptr->egprs_bep_prd2 =
                              gunpackb(src, pos_ptr, CSN_EGPRS_BEP_PRD2_LEN);

            /*  Set optional BEP flag */
            msg_ptr->egprs_bep_prd2_flag = TRUE;

          }

          /*  Set optional EGPRS flag */
          msg_ptr->egprs_options_flag = TRUE;
        }

        /*  Clear option flag before checking if optional field present */
        msg_ptr->pkt_ext_ta_flag = FALSE;

        if (gunpackb(src, pos_ptr, (int16)1))
        {
          /*  Packet Extended Timing Advance present */
          msg_ptr->pkt_ext_ta = gunpackb(src, pos_ptr, CSN_PKT_EXT_TA_LEN);

          /* Set conditional flag */
          msg_ptr->pkt_ext_ta_flag = TRUE;
        }


   /* { 0 | 1 < COMPACT reduced MA : < COMPACT reduced MA IE >> } */
        /*  Check next bit */
        if ((gunpackb(src, pos_ptr, (int16)1)))
        {
            /*  Extract COMPACT reduced MA */
            csn_compact_reduced_ma_ie(src, pos_ptr, &(msg_ptr->compact_reduced_ma));
            msg_ptr->compact_reduced_ma_flag = TRUE;
        }
        else
        {
            msg_ptr->compact_reduced_ma_flag = FALSE;
        }


        /*  Set option flag */
        msg_ptr->r99_additions_flag = TRUE;
      }

    }
    else
    {
      /*  A Message Escape error has occurred */
      return_status = MESSAGE_ESCAPE_ERROR;
    }

  }

  return(return_status);

}


/*
The following functions decode the UTRAN FDD Description of the 3G Neighbour
Cell Description of the ENH Measurement Parameters of the Packet Measurement
Order.
*/

static const uint8 range_1024_conversion_table[] = { 0, 10, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 7, 7, 6, 0 };

static csn_decoder_error_t csn_decode_utran_fdd_neighbour_cell(uint8 src[], uint16 *pos_ptr, utran_fdd_neighbour_cell_t *msg_ptr)
{
  csn_decoder_error_t retval = CSN_DECODE_NO_ERROR;

  (void) gunpackb(src, pos_ptr, 1); /* spare bit, should be 0 */

  msg_ptr->fdd_arfcn = gunpackw(src, pos_ptr, 14);
  msg_ptr->fdd_indic0 = gunpackb(src, pos_ptr, 1);
  msg_ptr->nr_of_fdd_cells = gunpackb(src, pos_ptr, CSN_NR_OF_FDD_CELLS_LEN);

  if (msg_ptr->nr_of_fdd_cells > 0 && msg_ptr->nr_of_fdd_cells < 17)
  {
    int32 i;
    for (i = 1; i <= msg_ptr->nr_of_fdd_cells; i++)
    {
      msg_ptr->fdd_cell_information[i] = gunpackw(src, pos_ptr, (uint16) range_1024_conversion_table[i]);
    }
  }
  else
  {
    msg_ptr->nr_of_fdd_cells = 0;
  }
  return retval;
}

static csn_decoder_error_t csn_decode_utran_fdd_description(
  csn_decode_memory_instance_t *mem,
  uint8 src[], uint16 *pos_ptr, utran_fdd_description_t *msg_ptr)
{
  csn_decoder_error_t retval = CSN_DECODE_NO_ERROR;
  uint8 trap_count = MAX_UTRAN_FDD_NEIGHBOUR_CELLS;

  if (gunpackb(src, pos_ptr, (uint16) 1))
  {
    msg_ptr->bandwidth_fdd_flag = TRUE;
    msg_ptr->bandwidth_fdd = gunpackb(src, pos_ptr, CSN_BANDWIDTH_FDD_LEN);
  }
  else
  {
    msg_ptr->bandwidth_fdd_flag = FALSE;
  }
  msg_ptr->utran_fdd_neighbour_cell_count = 0;
  while (gunpackb(src, pos_ptr, (uint16) 1) && (trap_count != 0))
  {
    utran_fdd_neighbour_cell_t *cell;
    cell = (utran_fdd_neighbour_cell_t *)CSN_DECODE_MEM_MALLOC(sizeof(utran_fdd_neighbour_cell_t));
    if (cell == NULL) return NO_MEMORY;
    retval = csn_decode_utran_fdd_neighbour_cell(src, pos_ptr, cell);
    if (retval != CSN_DECODE_NO_ERROR) return retval;
    msg_ptr->utran_fdd_neighbour_cells[msg_ptr->utran_fdd_neighbour_cell_count] = cell;
    msg_ptr->utran_fdd_neighbour_cell_count++;
    trap_count--;
  }
  return retval;
}

/*
The following functions decode the CDMA2000 Cell of CDMA2000 Description of
the 3G Neighbour Cell Description of the ENH Measurement Parameters of the
Packet Measurement Order.
*/

static void csn_decode_cdma2000_cell(uint8 src[], uint16 *pos_ptr, cdma2000_cell_t *msg_ptr)
{
  msg_ptr->pilot_pn_offset = gunpackw(src, pos_ptr, CSN_PILOT_PN_OFFSET_LEN);
  if (gunpackb(src, pos_ptr, (uint16) 1))
  {
    switch (gunpackb(src, pos_ptr, (uint16) 3))
    {
    case 0:
      msg_ptr->cdma2000_pilot = CDMA2000_1X_COMMON_PILOT_WITH_TRANSMIT_DIVERSITY;
      msg_ptr->td_mode = gunpackb(src, pos_ptr, CSN_TD_MODE_LEN);
      msg_ptr->td_power_level = gunpackb(src, pos_ptr, CSN_TD_POWER_LEVEL_LEN);
      break;
    case 1:
      msg_ptr->cdma2000_pilot = CDMA2000_1X_AUXILIARY_PILOT;
      msg_ptr->qof = gunpackb(src, pos_ptr, CSN_QOF_LEN);
      msg_ptr->walsh_len_a = gunpackb(src, pos_ptr, CSN_WALSH_LEN_A_LEN);
      msg_ptr->aux_pilot_walsh = gunpackw(src, pos_ptr, (uint16)(msg_ptr->walsh_len_a + 6));
      break;
    case 2:
      msg_ptr->cdma2000_pilot = CDMA2000_1X_AUXILIARY_PILOT_WITH_TRANSMIT_DIVERSITY;
      msg_ptr->qof = gunpackb(src, pos_ptr, CSN_QOF_LEN);
      msg_ptr->walsh_len_b = gunpackb(src, pos_ptr, CSN_WALSH_LEN_B_LEN);
      msg_ptr->aux_td_walsh = gunpackw(src, pos_ptr, (uint16)(msg_ptr->walsh_len_b + 6));
      msg_ptr->aux_td_power_level = gunpackb(src, pos_ptr, CSN_AUX_TD_POWER_LEVEL_LEN);
      msg_ptr->td_mode = gunpackb(src, pos_ptr, CSN_TD_MODE_LEN);
      break;
    case 3:
      msg_ptr->cdma2000_pilot = CDMA2000_3X_COMMON_PILOT;
      msg_ptr->sr3_prim_pilot = gunpackb(src, pos_ptr, CSN_SR3_PRIM_PILOT_LEN);
      msg_ptr->sr3_pilot_power1 = gunpackb(src, pos_ptr, CSN_SR3_PILOT_POWER1_LEN);
      msg_ptr->sr3_pilot_power2 = gunpackb(src, pos_ptr, CSN_SR3_PILOT_POWER2_LEN);
      break;
    case 6:
      msg_ptr->cdma2000_pilot = CDMA2000_3X_AUXILIARY_PILOT;
      msg_ptr->sr3_prim_pilot = gunpackb(src, pos_ptr, CSN_SR3_PRIM_PILOT_LEN);
      msg_ptr->sr3_pilot_power1 = gunpackb(src, pos_ptr, CSN_SR3_PILOT_POWER1_LEN);
      msg_ptr->sr3_pilot_power2 = gunpackb(src, pos_ptr, CSN_SR3_PILOT_POWER2_LEN);
      msg_ptr->qof = gunpackb(src, pos_ptr, CSN_QOF_LEN);
      msg_ptr->walsh_len_c = gunpackb(src, pos_ptr, CSN_WALSH_LEN_C_LEN);
      msg_ptr->aux_walsh_len = gunpackb(src, pos_ptr, (uint16)(msg_ptr->walsh_len_c + 6));
      if (gunpackb(src, pos_ptr, (uint16) 1))
      {
        msg_ptr->walsh1_flag = TRUE;
        msg_ptr->qof1 = gunpackb(src, pos_ptr, CSN_QOF1_LEN);
        msg_ptr->walsh_length1 = gunpackb(src, pos_ptr, CSN_WALSH_LENGTH1_LEN);
        msg_ptr->aux_pilot_walsh1 = gunpackb(src, pos_ptr, (uint16)(msg_ptr->walsh_length1 + 6));
      }
      else
      {
        msg_ptr->walsh1_flag = FALSE;
      }
      if (gunpackb(src, pos_ptr, (uint16) 1))
      {
        msg_ptr->walsh2_flag = TRUE;
        msg_ptr->qof2 = gunpackb(src, pos_ptr, CSN_QOF2_LEN);
        msg_ptr->walsh_length2 = gunpackb(src, pos_ptr, CSN_WALSH_LENGTH2_LEN);
        msg_ptr->aux_pilot_walsh2 = gunpackb(src, pos_ptr, (uint16)(msg_ptr->walsh_length2 + 6));
      }
      else
      {
        msg_ptr->walsh2_flag = FALSE;
      }
      break;
    default:
      msg_ptr->cdma2000_pilot = CDMA2000_PILOT_UNKNOWN;
    }
  }
  else
  {
    msg_ptr->cdma2000_pilot = CDMA2000_1X_COMMON_PILOT;
  }
}

static csn_decoder_error_t csn_decode_cdma2000_description(
  csn_decode_memory_instance_t *mem,
  uint8 src[], uint16 *pos_ptr, cdma2000_description_t *msg_ptr)
{
  csn_decoder_error_t retval = CSN_DECODE_NO_ERROR;
  int32 i;

  msg_ptr->cdma2000_frequency_band = gunpackb(src, pos_ptr, CSN_CDMA2000_FREQUENCY_BAND_LEN);
  msg_ptr->cdma2000_frequency = gunpackw(src, pos_ptr, CSN_CDMA2000_FREQUENCY_LEN);
  msg_ptr->number_cdma2000_cells = gunpackb(src, pos_ptr, CSN_NUMBER_CDMA2000_CELLS_LEN);
  msg_ptr->cdma2000_cells = (cdma2000_cell_t *)CSN_DECODE_MEM_MALLOC(msg_ptr->number_cdma2000_cells * sizeof(cdma2000_cell_t));
  if (msg_ptr->cdma2000_cells == NULL) return NO_MEMORY;
  for (i=0; i<msg_ptr->number_cdma2000_cells; i++)
  {
    csn_decode_cdma2000_cell(src, pos_ptr, &msg_ptr->cdma2000_cells[i]);
  }
  return retval;
}

/*
The following functions decode the REMOVED 3G Cell Description of the 3G
Neighbour Cell Description of the ENH Measurement Parameters of the Packet
Measurement Order.
*/

static csn_decoder_error_t csn_decode_removed_cell_sublist(uint8 src[], uint16 *pos_ptr, removed_cell_sublist_t *msg_ptr)
{
  csn_decoder_error_t retval = CSN_DECODE_NO_ERROR;
  int32 i;

  msg_ptr->number_of_removed_cells = gunpackb(src, pos_ptr, CSN_N2_LEN) + 1;
  for (i=0; i<msg_ptr->number_of_removed_cells; i++)
  {
    removed_cell_t *cell = &msg_ptr->removed_cells[i];
    cell->removed_three_g_cell_index = gunpackb(src, pos_ptr, CSN_REMOVED_THREE_G_CELL_INDEX_LEN);
    cell->three_g_cell_diff_length = gunpackb(src, pos_ptr, CSN_THREE_G_CELL_DIFF_LENGTH_LEN);
    cell->three_g_cell_diff = gunpackb(src, pos_ptr, cell->three_g_cell_diff_length);
  }
  return retval;
}

static csn_decoder_error_t csn_decode_removed_3g_cell_description(uint8 src[], uint16 *pos_ptr, removed_3g_cell_description_t *msg_ptr)
{
  csn_decoder_error_t retval = CSN_DECODE_NO_ERROR;
  int32 i;

  msg_ptr->number_of_sublists = gunpackb(src, pos_ptr, CSN_N1_LEN) + 1;

  for (i=0; i<msg_ptr->number_of_sublists; i++)
  {
    retval = csn_decode_removed_cell_sublist(src, pos_ptr, &msg_ptr->removed_cell_sublists[i]);
    if (retval != CSN_DECODE_NO_ERROR)
    {
      return retval;
    }
  }
  return retval;
}

/*
The following functions decode the UTRAN TDD Description of the 3G Neighbour
Cell Description of the ENH Measurement Parameters of the Packet Measurement
Order.
*/
static const uint8 range_512_conversion_table[] = { 0, 9, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 0 };

static csn_decoder_error_t csn_decode_utran_tdd_neighbour_cell(uint8 src[], uint16 *pos_ptr, utran_tdd_neighbour_cell_t *msg_ptr)
{
  csn_decoder_error_t retval = CSN_DECODE_NO_ERROR;

  msg_ptr->tdd_indic0 = gunpackb(src, pos_ptr, CSN_TDD_INDIC0_LEN);

  if (gunpackb(src, pos_ptr, (uint16) 1) == 0)
  {
    msg_ptr->arfcn_or_arfcn_index_valid = tdd_arfcn_valid;
    msg_ptr->tdd_arfcn = gunpackw(src, pos_ptr, CSN_TDD_ARFCN_LEN);
  }
  else
  {
    MSG_GERAN_ERROR_0("CSN decode error, TDD expected 0 before TDD-ARFCN");
    return IGNORE_ERROR;
  }

  msg_ptr->nr_of_tdd_cells = gunpackb(src, pos_ptr, CSN_NR_OF_TDD_CELLS_LEN);

  if (msg_ptr->nr_of_tdd_cells > 0 && msg_ptr->nr_of_tdd_cells < 21)
  {
    int32 i;
    for (i = 1; i <= msg_ptr->nr_of_tdd_cells; i++)
    {
      msg_ptr->tdd_cell_information[i] = gunpackw(src, pos_ptr, (uint16) range_512_conversion_table[i]);
    }
  }
  else
  {
    msg_ptr->nr_of_tdd_cells = 0;
  }
  return retval;
}

static csn_decoder_error_t csn_decode_utran_tdd_description(
  csn_decode_memory_instance_t *mem,
  uint8 src[], uint16 *pos_ptr, utran_tdd_description_t *msg_ptr)
{
  csn_decoder_error_t retval = CSN_DECODE_NO_ERROR;
  uint8 trap_count = MAX_UTRAN_TDD_NEIGHBOUR_CELLS;

  if (gunpackb(src, pos_ptr, (uint16) 1))
  {
    msg_ptr->bandwidth_tdd_flag = TRUE;
    msg_ptr->bandwidth_tdd = gunpackb(src, pos_ptr, CSN_BANDWIDTH_TDD_LEN);
  }
  else
  {
    msg_ptr->bandwidth_tdd_flag = FALSE;
  }
  msg_ptr->utran_tdd_neighbour_cell_count = 0;
  while (gunpackb(src, pos_ptr, (uint16) 1) && (trap_count != 0))
  {
    utran_tdd_neighbour_cell_t *cell;
    cell = (utran_tdd_neighbour_cell_t *)CSN_DECODE_MEM_MALLOC(sizeof(utran_tdd_neighbour_cell_t));
    if (cell == NULL) return NO_MEMORY;
    retval = csn_decode_utran_tdd_neighbour_cell(src, pos_ptr, cell);
    if (retval != CSN_DECODE_NO_ERROR)
    {
      return retval;
    }
    msg_ptr->utran_tdd_neighbour_cells[msg_ptr->utran_tdd_neighbour_cell_count] = cell;
    msg_ptr->utran_tdd_neighbour_cell_count++;
    trap_count--;
  }
  return retval;
}


/**
 * Decode the PACKET MEASUREMENT ORDER/PACKET CELL CHANGE ORDER messages
 */

/**
 * csn_decode_nc_meas_params_nc_freq_list
 *
 * Decodes an NC Frequency List occuring in:
 *   PACKET MEASUREMENT ORDER: NC Measurement Params
 *   PACKET CELL CHANGE ORDER: NC Measurement Params
 */
static csn_decoder_error_t csn_decode_nc_meas_params_nc_freq_list(
  csn_decode_memory_instance_t *mem,
  uint8 src[], uint16 *pos_ptr, csn_nc_meas_params_nc_freq_list_t *dst)
{
  csn_decoder_error_t retval = CSN_DECODE_NO_ERROR;
  uint8 i;

  if (gunpackb(src, pos_ptr, 1))
  {

    /* there are removed frequencies, so extract them */
    dst->num_of_removed_freqs = gunpackb(src, pos_ptr, 5) + 1;
    for (i = 0; i < dst->num_of_removed_freqs; i++)
    {
      dst->removed_freqs[i] = gunpackb(src, pos_ptr, 6);
    }
  }

  i = 0;
  while (gunpackb(src, pos_ptr, 1))
  {
    uint16 start_freq;
    int32 nr_of_frequencies;
    uint8 freq_diff_length;
    int32 j;

    if (i >= ARR_SIZE(dst->added_freqs) )
      break;
    /* this is a two part process, there is always at least one element,
    then possibly several more. */
    dst->added_freqs[i] = CSN_DECODE_MEM_MALLOC(sizeof(csn_nc_meas_params_nc_freq_list_added_freq_t));
    if (dst->added_freqs[i] == NULL)
    {
      return NO_MEMORY;
    }

    start_freq = gunpackw(src, pos_ptr, 10);
    dst->added_freqs[i]->frequency = start_freq;
    dst->added_freqs[i]->bsic = gunpackb(src, pos_ptr, 6);
    dst->added_freqs[i]->cell_selection_params_valid = gunpackb(src, pos_ptr, 1);
    if (dst->added_freqs[i]->cell_selection_params_valid)
    {
      retval = csn_cell_sel_params_extract(src, pos_ptr, &dst->added_freqs[i]->cell_selection_params);

      if (retval != CSN_DECODE_NO_ERROR)
      {
        return retval;
      }
    }
    i++;
    if (i >= ARR_SIZE(dst->added_freqs) )
      break;

    /* now, that's the first one out of the way, so on to the next ones */
    nr_of_frequencies = gunpackb(src, pos_ptr, 5);
    freq_diff_length = gunpackb(src, pos_ptr, 3) + 1;

    for (j = 0; j < nr_of_frequencies; j++)
    {
      if (i >= ARR_SIZE(dst->added_freqs) )
        break;

      dst->added_freqs[i] = CSN_DECODE_MEM_MALLOC(sizeof(csn_nc_meas_params_nc_freq_list_added_freq_t));
      if (dst->added_freqs[i] == NULL)
      {
        return NO_MEMORY;
      }

      dst->added_freqs[i]->frequency = start_freq + gunpackb(src, pos_ptr, freq_diff_length);
      start_freq = dst->added_freqs[i]->frequency;
      dst->added_freqs[i]->bsic = gunpackb(src, pos_ptr, 6);
      dst->added_freqs[i]->cell_selection_params_valid = gunpackb(src, pos_ptr, 1);
      if (dst->added_freqs[i]->cell_selection_params_valid)
      {
        retval = csn_cell_sel_params_extract(src, pos_ptr, &dst->added_freqs[i]->cell_selection_params);

        if (retval != CSN_DECODE_NO_ERROR)
        {
          return retval;
        }
      }
      i++;
    }
    dst->num_of_added_freqs = i;
  }
  return retval;
}

static void csn_discard_iu_mode_cell_selection(uint8 src[], uint16 *pos_ptr)
{
  /* CELL BAR QUALIFY 3 */
  (void)gunpackb(src, pos_ptr, 2);
  if (gunpackb(src, pos_ptr, 1))
  {
    /* SI13 PBCCH Location */
    if (gunpackb(src, pos_ptr, 1) == 0)
    {
      /* SI13 location */
      (void)gunpackb(src, pos_ptr, 1);
    }
    else
    {
      /* PBCCH location */
      (void)gunpackb(src, pos_ptr, 2);
      (void)gunpackb(src, pos_ptr, 4);
    }
  }
}

static void csn_discard_iu_mode_ncell_params(uint8 src[], uint16 *pos_ptr)
{
  uint8 num_freqs;

  if (gunpackb(src, pos_ptr, 1))
  {
    csn_discard_iu_mode_cell_selection(src, pos_ptr);
  }

  num_freqs = gunpackb(src, pos_ptr, 5);

  if (gunpackb(src, pos_ptr, 1))
  {
    while (num_freqs)
    {
      csn_discard_iu_mode_cell_selection(src, pos_ptr);
      num_freqs--;
    }
  }
}

static void csn_discard_iu_mode_only_cell_selection(uint8 src[], uint16 *pos_ptr)
{
  /* CELL BAR QUALIFY 3 */
  (void)gunpackb(src, pos_ptr, 2);

  /* SAME RA AS SERVING  CELL */
  (void)gunpackb(src, pos_ptr, 1);

  if (gunpackb(src, pos_ptr, 1))
  {
    /* GPRS_RXLEV_ACCESS_MIN */
    (void)gunpackb(src, pos_ptr, 6);
    /* GPRS_MS_TXPWR_MAX_CCH */
    (void)gunpackb(src, pos_ptr, 5);
  }

  if (gunpackb(src, pos_ptr, 1))
  {
    /* GPRS_TEMPORARY_OFFSET */
    (void)gunpackb(src, pos_ptr, 3);
    /* GPRS_PENALTY_TIME */
    (void)gunpackb(src, pos_ptr, 5);
  }

  if (gunpackb(src, pos_ptr, 1))
  {
    /* GPRS_RESELECT_OFFSET */
    (void)gunpackb(src, pos_ptr, 5);
  }

  if (gunpackb(src, pos_ptr, 1))
  {
    /* PRIORITY_CLASS */
    (void)gunpackb(src, pos_ptr, 3);
    /* HCS_THR */
    (void)gunpackb(src, pos_ptr, 5);
  }

  if (gunpackb(src, pos_ptr, 1))
  {
    /* SI13 PBCCH Location */
    if (gunpackb(src, pos_ptr, 1) == 0)
    {
      /* SI13 location */
      (void)gunpackb(src, pos_ptr, 1);
    }
    else
    {
      /* PBCCH location */
      (void)gunpackb(src, pos_ptr, 2);
      (void)gunpackb(src, pos_ptr, 4);
    }
  }
}

static void csn_discard_iu_mode_only_cell_list(uint8 src[], uint16 *pos_ptr)
{
  uint8 num_freqs;
  uint8 diff_len;

  while (gunpackb(src, pos_ptr, 1))
  {
    /* START_FREQUENCY */
    (void)gunpackw(src, pos_ptr, 10);
    /* BSIC */
    (void)gunpackb(src, pos_ptr, 6);
    if (gunpackb(src, pos_ptr, 1))
    {
      csn_discard_iu_mode_only_cell_selection(src, pos_ptr);
    }
    num_freqs = gunpackb(src, pos_ptr, 5);
    diff_len = gunpackb(src, pos_ptr, 3);
    while (num_freqs)
    {
      /* FREQUENCY_DIFF */
      (void)gunpackb(src, pos_ptr, diff_len);
      /* BSIC */
      (void)gunpackb(src, pos_ptr, 6);
      if (gunpackb(src, pos_ptr, 1))
      {
        csn_discard_iu_mode_only_cell_selection(src, pos_ptr);
      }
      num_freqs--;
    }
  }
}
//tlx dime 2.1 with fr3009 FEATURE_GSM_TO_LTE  added new func
static void csn_decode_gsm_priority_params_ie(
  uint8                        * src,
  uint16                       * pos_ptr,
  csn_gsm_priority_params_ie_t * dst
)
{
  csn_gsm_priority_params_ie_t gsm_priority_params;
  memset(&gsm_priority_params, 0, sizeof(csn_gsm_priority_params_ie_t));

  if (src == NULL)
  {
    MSG_ERROR("csn_decode_gsm_priority_params_ie: src == NULL", 0, 0, 0);
    return;
  }

  if (pos_ptr == NULL)
  {
    MSG_ERROR("csn_decode_gsm_priority_params_ie: pos_ptr == NULL", 0, 0, 0);
    return;
  }

  gsm_priority_params.geran_priority = gunpackb(src, pos_ptr, 3);
  gsm_priority_params.thresh_priority_search = gunpackb(src, pos_ptr, 4);
  gsm_priority_params.thresh_gsm_low = gunpackb(src, pos_ptr, 4);
  gsm_priority_params.h_prio = gunpackb(src, pos_ptr, 2);
  gsm_priority_params.t_reselection = gunpackb(src, pos_ptr, 2);

  if (dst != NULL)
  {
    *dst = gsm_priority_params;
  }
}

static void csn_decode_utran_priority_params(
  uint8                       * src,
  uint16                      * pos_ptr,
  csn_utran_priority_params_t * dst
)
{
  csn_utran_priority_params_t utran_priority_params;
  memset(&utran_priority_params, 0, sizeof(csn_utran_priority_params_t));

  if (src == NULL)
  {
    MSG_ERROR("csn_decode_utran_priority_params: src == NULL", 0, 0, 0);
    return;
  }

  if (pos_ptr == NULL)
  {
    MSG_ERROR("csn_decode_utran_priority_params: pos_ptr == NULL", 0, 0, 0);
    return;
  }

  while (gunpackb(src, pos_ptr, 1))
  {
    uint8 frequency_index = gunpackb(src, pos_ptr, 5);
    SET_BIT((1 << frequency_index), utran_priority_params.frequency_index_bitmap);
  }

  if (gunpackb(src, pos_ptr, 1))
  {
    utran_priority_params.priority_valid = TRUE;
    utran_priority_params.priority = gunpackb(src, pos_ptr, 3);
  }

  utran_priority_params.thresh_high = gunpackb(src, pos_ptr, 5);

  if (gunpackb(src, pos_ptr, 1))
  {
    utran_priority_params.thresh_low_valid = TRUE;
    utran_priority_params.thresh_low = gunpackb(src, pos_ptr, 5);
  }

  if (gunpackb(src, pos_ptr, 1))
  {
    utran_priority_params.qrxlevmin_valid = TRUE;
    utran_priority_params.qrxlevmin = gunpackb(src, pos_ptr, 5);
  }

  if (dst != NULL)
  {
    *dst = utran_priority_params;
  }
}

static void csn_decode_utran_priority_params_ie(
  uint8                          * src,
  uint16                         * pos_ptr,
  csn_utran_priority_params_ie_t * dst
)
{
  csn_utran_priority_params_ie_t utran_priority_params_ie;
  memset(&utran_priority_params_ie, 0, sizeof(csn_utran_priority_params_ie_t));

  if (src == NULL)
  {
    MSG_ERROR("csn_decode_utran_priority_params_ie: src == NULL", 0, 0, 0);
    return;
  }

  if (pos_ptr == NULL)
  {
    MSG_ERROR("csn_decode_utran_priority_params_ie: pos_ptr == NULL", 0, 0, 0);
    return;
  }

  if (gunpackb(src, pos_ptr, 1))
  {
    utran_priority_params_ie.default_params_valid = TRUE;
    utran_priority_params_ie.default_params.priority = gunpackb(src, pos_ptr, 3);
    utran_priority_params_ie.default_params.thresh = gunpackb(src, pos_ptr, 5);
    utran_priority_params_ie.default_params.qrxlevmin = gunpackb(src, pos_ptr, 5);
  }

  while (gunpackb(src, pos_ptr, 1))
  {
    /* Decode each Repeated UTRAN Priority Parameters structure to temporary
    memory, then copy in to the destination structure if there is sufficient
    storage space remaining.  This is done to preserve decoder integrity even
    in the unlikely event that the statically sized destination data structure
    is not large enough to contain all information in the OTA message. */

    csn_utran_priority_params_t utran_priority_params;
    memset(&utran_priority_params, 0, sizeof(csn_utran_priority_params_t));

    csn_decode_utran_priority_params(
      src,
      pos_ptr,
      &utran_priority_params
    );

    if (utran_priority_params_ie.utran_priority_params_count < CSN_UTRAN_PRIORITY_PARAMS_DESC_MAX)
    {
      utran_priority_params_ie.utran_priority_params[utran_priority_params_ie.utran_priority_params_count] = utran_priority_params;
      ++utran_priority_params_ie.utran_priority_params_count;
    }
  }

  if (dst != NULL)
  {
    *dst = utran_priority_params_ie;
  }
}

static void csn_decode_pcid_group_ie(
  uint8               * src,
  uint16              * pos_ptr,
  csn_pcid_group_ie_t * dst
)
{
  csn_pcid_group_ie_t pcid_group;

  memset(&pcid_group, 0, sizeof(csn_pcid_group_ie_t));

  if (src == NULL)
  {
    MSG_ERROR("src == NULL", 0, 0, 0);
    return;
  }

  if (pos_ptr == NULL)
  {
    MSG_ERROR("pos_ptr == NULL", 0, 0, 0);
    return;
  }

  /* Explicit PCID coding */
  while (gunpackb(src, pos_ptr, 1))
  {
    uint16 pcid = gunpackw(src, pos_ptr, 9);
    if (pcid < CSN_PCID_GROUP_IE_MAX_PCID)
    {
      CSN_PCID_GROUP_IE_SET(pcid_group.bitmap, pcid);
    }
    else
    {
      MSG_GERAN_ERROR_1("Invalid PCID: %d", pcid);
    }
  }

  /* PCID Bitmap Group */
  if (gunpackb(src, pos_ptr, 1))
  {
    /* Each bit number n, where n can take the value from 1 to 6, represents
    the group of PCID values in the range (n-1) * 84 to (n*84)-1 inclusive. */
    uint32 pcid_bitmap_group = gunpackb(src, pos_ptr, 6);
    uint32 n;

    for (n = 1; n < 7; ++n)
    {
      uint32 i;

      if (pcid_bitmap_group & (1 << (n - 1)))
      {
        uint32 lower = (n - 1) * 84;
        uint32 upper = (n * 84) - 1;
        MSG_MED("PCID Bitmap Group %d is in PCID group (from %d to %d inclusive)", n, lower, upper);
        for (i = lower; i <= upper && i <= CSN_PCID_GROUP_IE_MAX_PCID; ++i)
        {
          CSN_PCID_GROUP_IE_SET(pcid_group.bitmap, i);
        }
      }
    }
  }

  /* PCID Pattern */
  while (gunpackb(src, pos_ptr, 1))
  {
    uint8 length = gunpackb(src, pos_ptr, 3) + 1;
    uint8 value = gunpackb(src, pos_ptr, length);
    uint8 sense = gunpackb(src, pos_ptr, 1);

    if (pcid_group.pcid_pattern_count < CSN_PCID_GROUP_IE_MAX_PATTERNS)
    {
      pcid_group.pcid_patterns[pcid_group.pcid_pattern_count].length = length;
      pcid_group.pcid_patterns[pcid_group.pcid_pattern_count].value = value;
      pcid_group.pcid_patterns[pcid_group.pcid_pattern_count].sense = sense;
      ++pcid_group.pcid_pattern_count;
    }
  }

  if (dst != NULL)
  {
    *dst = pcid_group;
  }
}

static void csn_decode_eutran_params_ie(
  uint8                  * src,
  uint16                 * pos_ptr,
  csn_eutran_params_ie_t * dst
)
{
  if (src == NULL)
  {
    MSG_ERROR("csn_decode_eutran_params_ie: src == NULL", 0, 0, 0);
    return;
  }

  if (pos_ptr == NULL)
  {
    MSG_ERROR("csn_decode_eutran_params_ie: pos_ptr == NULL", 0, 0, 0);
    return;
  }

  if (dst == NULL)
  {
    MSG_ERROR("csn_decode_eutran_params_ie: dst == NULL", 0, 0, 0);
    return;
  }

  memset(dst, 0, sizeof(csn_eutran_params_ie_t));

  dst->ccn_active = gunpackb(src, pos_ptr, 1);

  /* GPRS E-UTRAN Measurement Parameters */
  if (gunpackb(src, pos_ptr, 1))
  {
    dst->gprs_meas_params_valid = TRUE;
    dst->gprs_meas_params.qsearch_p = gunpackb(src, pos_ptr, 4);
    dst->gprs_meas_params.rep_quant = gunpackb(src, pos_ptr, 1);
    dst->gprs_meas_params.multirat_reporting = gunpackb(src, pos_ptr, 2);

    /* FDD Parameters */
    if (gunpackb(src, pos_ptr, 1))
    {
      dst->gprs_meas_params.fdd_valid = TRUE;
      dst->gprs_meas_params.fdd_reporting_threshold = gunpackb(src, pos_ptr, 3);
      if (gunpackb(src, pos_ptr, 1))
      {
        dst->gprs_meas_params.fdd_reporting_threshold_2_valid = TRUE;
        dst->gprs_meas_params.fdd_reporting_threshold_2 = gunpackb(src, pos_ptr, 6);
      }
      if (gunpackb(src, pos_ptr, 1))
      {
        dst->gprs_meas_params.fdd_reporting_offset_valid = TRUE;
        dst->gprs_meas_params.fdd_reporting_offset = gunpackb(src, pos_ptr, 3);
      }
    }

    /* TDD Parameters */
    if (gunpackb(src, pos_ptr, 1))
    {
      dst->gprs_meas_params.tdd_valid = TRUE;
      dst->gprs_meas_params.tdd_reporting_threshold = gunpackb(src, pos_ptr, 3);
      if (gunpackb(src, pos_ptr, 1))
      {
        dst->gprs_meas_params.tdd_reporting_threshold_2_valid = TRUE;
        dst->gprs_meas_params.tdd_reporting_threshold_2 = gunpackb(src, pos_ptr, 6);
      }
      if (gunpackb(src, pos_ptr, 1))
      {
        dst->gprs_meas_params.tdd_reporting_offset_valid = TRUE;
        dst->gprs_meas_params.tdd_reporting_offset = gunpackb(src, pos_ptr, 3);
      }
    }
  }

  /* Repeated E-UTRAN Neighbour Cells */
  while (gunpackb(src, pos_ptr, 1))
  {
    csn_eutran_neighbour_cells_t neighbour_cells;
    memset(&neighbour_cells, 0, sizeof(csn_eutran_neighbour_cells_t));

    while (gunpackb(src, pos_ptr, 1))
    {
      uint16  earfcn;
      uint8   measurement_bandwidth = 0;

      earfcn = gunpackw(src, pos_ptr, 16);

      if (gunpackb(src, pos_ptr, 1))
      {
        measurement_bandwidth = gunpackb(src, pos_ptr, 3);
      }

      if (neighbour_cells.earfcn_count < CSN_EUTRAN_NEIGHBOUR_CELLS_MAX_EARFCNS)
      {
        neighbour_cells.earfcns[neighbour_cells.earfcn_count].earfcn = earfcn;
        neighbour_cells.earfcns[neighbour_cells.earfcn_count].measurement_bandwidth = measurement_bandwidth;
        ++neighbour_cells.earfcn_count;
      }
      else
      {
        MSG_ERROR("Discarding EARFCN, overflow", 0, 0, 0);
      }
    }

    if (gunpackb(src, pos_ptr, 1))
    {
      neighbour_cells.priority_valid = TRUE;
      neighbour_cells.priority = gunpackb(src, pos_ptr, 3);
    }

    neighbour_cells.thresh_high = gunpackb(src, pos_ptr, 5);

    if (gunpackb(src, pos_ptr, 1))
    {
      neighbour_cells.thresh_low_valid = TRUE;
      neighbour_cells.thresh_low = gunpackb(src, pos_ptr, 5);
    }
    else
    {
      neighbour_cells.thresh_low = neighbour_cells.thresh_high;
    }

    if (gunpackb(src, pos_ptr, 1))
    {
      neighbour_cells.qrxlevmin_valid = TRUE;
      neighbour_cells.qrxlevmin = gunpackb(src, pos_ptr, 5);
    }

    if (dst->neighbour_cells_count < CSN_EUTRAN_PARAMS_MAX_NEIGHBOUR_CELLS)
    {
      dst->neighbour_cells[dst->neighbour_cells_count] = neighbour_cells;
      dst->neighbour_cells_count++;

    }
    else
    {
      MSG_ERROR("Discarding Repeated E-UTRAN Neighbour Cells, overflow", 0, 0, 0);
    }
  }

  /* Repeated E-UTRAN Not Allowed Cells */
  while (gunpackb(src, pos_ptr, 1))
  {
    csn_pcid_group_ie_t pcid_group;
    uint8               eutran_frequency_indices_bitmap = 0;

    memset(&pcid_group, 0, sizeof(csn_pcid_group_ie_t));

    csn_decode_pcid_group_ie(src, pos_ptr, &pcid_group);

    while (gunpackb(src, pos_ptr, 1))
    {
      uint8 eutran_frequency_index = gunpackb(src, pos_ptr, 3);
      if (eutran_frequency_index < 8)
      {
        eutran_frequency_indices_bitmap |= (1 << eutran_frequency_index);
      }
      else
      {
        MSG_GERAN_ERROR_1("Invalid eutran_frequency_index: %d", eutran_frequency_index);
      }
    }

    if (dst->not_allowed_cells_count < CSN_EUTRAN_PARAMS_MAX_NEIGHBOUR_CELLS)
    {
      dst->not_allowed_cells[dst->not_allowed_cells_count].pcid_group = pcid_group;
      dst->not_allowed_cells[dst->not_allowed_cells_count].eutran_frequency_indices_bitmap = eutran_frequency_indices_bitmap;
      ++dst->not_allowed_cells_count;
    }
    else
    {
      MSG_GERAN_ERROR_0("Overflow decoding E-UTRAN Not Allowed Cells");
    }
  }

  /* Repeated E-UTRAN PCID to TA mapping */
  /* This structure is not supported by RR currently, so is just discarded. */
  while (gunpackb(src, pos_ptr, 1))
  {
    while (gunpackb(src, pos_ptr, 1))
    {
      /* PCID to TA mapping */
      csn_decode_pcid_group_ie(src, pos_ptr, NULL);
    }

    while (gunpackb(src, pos_ptr, 1))
    {
      /* E-UTRAN_FREQUENCY_INDEX */
      (void) gunpackb(src, pos_ptr, 3);
    }
  }
}

static void csn_decode_individual_priorities_ie(
  uint8                          * src,
  uint16                         * pos_ptr,
  csn_individual_priorities_ie_t * dst
)
{
  csn_individual_priorities_ie_t individual_priorities;

  memset(&individual_priorities, 0, sizeof(csn_individual_priorities_ie_t));

  if (src == NULL)
  {
    MSG_ERROR("src == NULL", 0, 0, 0);
    return;
  }

  if (pos_ptr == NULL)
  {
    MSG_ERROR("pos_ptr == NULL", 0, 0, 0);
    return;
  }

  if (gunpackb(src, pos_ptr, 1))
  {
    individual_priorities.valid = TRUE;
    individual_priorities.geran_priority = gunpackb(src, pos_ptr, 3);

    if (gunpackb(src, pos_ptr, 1))
    {
      individual_priorities.utran_valid = TRUE;

      if (gunpackb(src, pos_ptr, 1))
      {
        individual_priorities.utran.default_priority_valid = TRUE;
        individual_priorities.utran.default_priority = gunpackb(src, pos_ptr, 3);
      }

      /* Repeated Individual UTRAN Priority Parameters */
      while (gunpackb(src, pos_ptr, 1))
      {
        /* Temporary storage space. */
        uint32           i;
        csn_utran_type_e type;
        uint8            uarfcn_count = 0;
        uint16           uarfcns[CSN_INDIVIDUAL_PRIORITIES_MAX_UARFCNS];
        uint8            priority;

        if (gunpackb(src, pos_ptr, 1) == 0) /* FDD */
        {
          type = CSN_UTRAN_TYPE_FDD;
        }
        else /* TDD */
        {
          type = CSN_UTRAN_TYPE_TDD;
        }

        while (gunpackb(src, pos_ptr, 1))
        {
          uint16 uarfcn = gunpackw(src, pos_ptr, 14);
          if (uarfcn_count < CSN_INDIVIDUAL_PRIORITIES_MAX_UARFCNS)
          {
            uarfcns[uarfcn_count++] = uarfcn;
          }
        }

        priority = gunpackb(src, pos_ptr, 3);

        /* Now that we have unpacked the UTRAN priorities in this instance
        of the repeated structure, we can copy them in to the destination
        structure. */
        for (i = 0;
             i < uarfcn_count &&
             individual_priorities.utran.uarfcn_count < CSN_INDIVIDUAL_PRIORITIES_MAX_UARFCNS;
             ++i)
        {
          individual_priorities.utran.uarfcns[individual_priorities.utran.uarfcn_count].type = type;
          individual_priorities.utran.uarfcns[individual_priorities.utran.uarfcn_count].uarfcn = uarfcns[i];
          individual_priorities.utran.uarfcns[individual_priorities.utran.uarfcn_count].priority = priority;
          ++individual_priorities.utran.uarfcn_count;
        }
      }
    }

    if (gunpackb(src, pos_ptr, 1))
    {
      individual_priorities.eutran_valid = TRUE;

      if (gunpackb(src, pos_ptr, 1))
      {
        individual_priorities.eutran.default_priority_valid = TRUE;
        individual_priorities.eutran.default_priority = gunpackb(src, pos_ptr, 3);
      }

      /* Repeated Individual E-UTRAN Priority Parameters */
      while (gunpackb(src, pos_ptr, 1))
      {
        /* Temporary storage space. */
        uint32           i;
        uint8            earfcn_count = 0;
        uint16           earfcns[CSN_INDIVIDUAL_PRIORITIES_MAX_EARFCNS];
        uint8            priority;

        while (gunpackb(src, pos_ptr, 1))
        {
          uint16 earfcn = gunpackw(src, pos_ptr, 16);
          if (earfcn_count < CSN_INDIVIDUAL_PRIORITIES_MAX_EARFCNS)
          {
            earfcns[earfcn_count++] = earfcn;
          }
        }

        priority = gunpackb(src, pos_ptr, 3);

        /* Now that we have unpacked the ETRAN priorities in this instance
        of the repeated structure, we can copy them in to the destination
        structure. */
        for (i = 0;
             i < earfcn_count &&
             individual_priorities.eutran.earfcn_count < CSN_INDIVIDUAL_PRIORITIES_MAX_EARFCNS;
             ++i)
        {
          individual_priorities.eutran.earfcns[individual_priorities.eutran.earfcn_count].earfcn = earfcns[i];
          individual_priorities.eutran.earfcns[individual_priorities.eutran.earfcn_count].priority = priority;
          ++individual_priorities.eutran.earfcn_count;
        }
      }
    }

    if (gunpackb(src, pos_ptr, 1))
    {
      individual_priorities.t3230_timeout_value_valid = TRUE;
      individual_priorities.t3230_timeout_value = gunpackb(src, pos_ptr, 3);
    }
  }
  else
  {
    individual_priorities.valid = FALSE;
  }

  if (dst != NULL)
  {
    *dst = individual_priorities;
  }
}

static void csn_decode_psc_group_ie(
  uint8              * src,
  uint16             * pos_ptr,
  csn_psc_group_ie_t * dst
)
{
  csn_psc_group_ie_t psc_group;

  memset(&psc_group, 0, sizeof(csn_psc_group_ie_t));

  if (src == NULL)
  {
    MSG_ERROR("src == NULL", 0, 0, 0);
    return;
  }

  if (pos_ptr == NULL)
  {
    MSG_ERROR("pos_ptr == NULL", 0, 0, 0);
    return;
  }

  /* Explicit PSC coding */
  while (gunpackb(src, pos_ptr, 1))
  {
    uint16 psc = gunpackw(src, pos_ptr, 9);
    if (psc < CSN_PSC_GROUP_IE_MAX_PSC)
    {
      CSN_PSC_GROUP_IE_SET(psc_group, psc);
    }
    else
    {
      MSG_GERAN_ERROR_1("Invalid PSC: %d", psc);
    }
  }

  /* PSC Pattern */
  while (gunpackb(src, pos_ptr, 1))
  {
    uint8 length = gunpackb(src, pos_ptr, 3) + 1;
    uint8 value = gunpackb(src, pos_ptr, length);
    uint8 sense = gunpackb(src, pos_ptr, 1);

    if (psc_group.psc_pattern_count < CSN_PSC_GROUP_IE_MAX_PATTERNS)
    {
      psc_group.psc_patterns[psc_group.psc_pattern_count].length = length;
      psc_group.psc_patterns[psc_group.psc_pattern_count].value = value;
      psc_group.psc_patterns[psc_group.psc_pattern_count].sense = sense;
      ++psc_group.psc_pattern_count;
    }
  }

  if (dst != NULL)
  {
    *dst = psc_group;
  }
}

static void csn_decode_utran_csg_description_ie(
  uint8                          * src,
  uint16                         * pos_ptr,
  csn_utran_csg_description_ie_t * dst
)
{
  csn_utran_csg_description_ie_t utran_csg_description;

  memset(&utran_csg_description, 0, sizeof(csn_utran_csg_description_ie_t));

  if (src == NULL)
  {
    MSG_ERROR("src == NULL", 0, 0, 0);
    return;
  }

  if (pos_ptr == NULL)
  {
    MSG_ERROR("pos_ptr == NULL", 0, 0, 0);
    return;
  }

  while (gunpackb(src, pos_ptr, 1))
  {
    csn_psc_group_ie_t csg_psc_split;
    uint32             utran_frequency_indices_bitmap = 0;

    memset(&csg_psc_split, 0, sizeof(csn_psc_group_ie_t));

    csn_decode_psc_group_ie(
      src,
      pos_ptr,
      &csg_psc_split
    );

    while (gunpackb(src, pos_ptr, 1))
    {
      uint8 utran_frequency_index = gunpackb(src, pos_ptr, 5);
      if (utran_frequency_index < 32)
      {
        utran_frequency_indices_bitmap |= (1 << utran_frequency_index);
      }
      else
      {
        MSG_GERAN_ERROR_1("Invalid utran_frequency_index: %d", utran_frequency_index);
      }
    }

    if (utran_csg_description.count < CSN_UTRAN_CSG_DESCRIPTION_MAX_ENTRIES)
    {
      utran_csg_description.entries[utran_csg_description.count].csg_psc_split = csg_psc_split;
      utran_csg_description.entries[utran_csg_description.count].utran_frequency_indices_bitmap = utran_frequency_indices_bitmap;
      ++utran_csg_description.count;
    }
    else
    {
      MSG_GERAN_ERROR_0("Overflow decoding UTRAN CSG Description");
    }
  }

  if (dst != NULL)
  {
    *dst = utran_csg_description;
  }
}

static void csn_decode_eutran_csg_description_ie(
  uint8                           * src,
  uint16                          * pos_ptr,
  csn_eutran_csg_description_ie_t * dst
)
{
  csn_eutran_csg_description_ie_t eutran_csg_description;

  memset(&eutran_csg_description, 0, sizeof(csn_eutran_csg_description_ie_t));

  if (src == NULL)
  {
    MSG_ERROR("src == NULL", 0, 0, 0);
    return;
  }

  if (pos_ptr == NULL)
  {
    MSG_ERROR("pos_ptr == NULL", 0, 0, 0);
    return;
  }

  while (gunpackb(src, pos_ptr, 1))
  {
    csn_pcid_group_ie_t csg_pci_split;
    uint8               eutran_frequency_indices_bitmap = 0;

    memset(&csg_pci_split, 0, sizeof(csn_pcid_group_ie_t));

    csn_decode_pcid_group_ie(
      src,
      pos_ptr,
      &csg_pci_split
    );

    while (gunpackb(src, pos_ptr, 1))
    {
      uint8 eutran_frequency_index = gunpackb(src, pos_ptr, 3);
      if (eutran_frequency_index < 8)
      {
        eutran_frequency_indices_bitmap |= (1 << eutran_frequency_index);
      }
      else
      {
        MSG_GERAN_ERROR_1("Invalid eutran_frequency_index: %d", eutran_frequency_index);
      }
    }

    if (eutran_csg_description.count < CSN_EUTRAN_CSG_DESCRIPTION_MAX_ENTRIES)
    {
      eutran_csg_description.entries[eutran_csg_description.count].csg_pci_split = csg_pci_split;
      eutran_csg_description.entries[eutran_csg_description.count].eutran_frequency_indices_bitmap = eutran_frequency_indices_bitmap;
      ++eutran_csg_description.count;
    }
    else
    {
      MSG_GERAN_ERROR_0("Overflow decoding E-UTRAN CSG Description");
    }
  }

  if (dst != NULL)
  {
    *dst = eutran_csg_description;
  }
}

/**
  @brief Decodes Measurement Control Parameters Description IE.

  @param[in] src the byte array containing the encoded message / IE.
  @param[in,out] pos_ptr indicates the current bit position in the encoded
      message.
  @param[out] dst data structure in which to store the decoded IE. May be NULL.
*/
static void csn_decode_measurement_control_params_ie(
  uint8                               * src,
  uint16                              * pos_ptr,
  csn_measurement_control_params_ie_t * dst
)
{
  csn_measurement_control_params_ie_t measurement_control_params;

  memset(&measurement_control_params, 0, sizeof(csn_measurement_control_params_ie_t));

  if (src == NULL)
  {
    MSG_ERROR("src == NULL", 0, 0, 0);
    return;
  }

  if (pos_ptr == NULL)
  {
    MSG_ERROR("pos_ptr == NULL", 0, 0, 0);
    return;
  }

  if (gunpackb(src, pos_ptr, 1))
  {
    measurement_control_params.eutran_valid = TRUE;
    measurement_control_params.eutran.measurement_control = gunpackb(src, pos_ptr, 1);
    do
    {
      uint8 frequency_index = gunpackb(src, pos_ptr, 3);
      if (frequency_index < 8)
      {
        measurement_control_params.eutran.frequency_indices_bitmap |= (1 << frequency_index);
      }
      else
      {
        MSG_GERAN_ERROR_1("Invalid eutran_frequency_index: %d", frequency_index);
      }
    } while (gunpackb(src, pos_ptr, 1));
  }

  if (gunpackb(src, pos_ptr, 1))
  {
    measurement_control_params.utran_valid = TRUE;
    measurement_control_params.utran.measurement_control = gunpackb(src, pos_ptr, 1);
    do
    {
      uint8 frequency_index = gunpackb(src, pos_ptr, 5);
      if (frequency_index < 32)
      {
        measurement_control_params.utran.frequency_indices_bitmap |= (1 << frequency_index);
      }
      else
      {
        MSG_GERAN_ERROR_1("Invalid utran_frequency_index: %d", frequency_index);
      }
    } while (gunpackb(src, pos_ptr, 1));
  }

  if (dst != NULL)
  {
    *dst = measurement_control_params;
  }
}

//tlx dime 2.1 with fr3009 FEATURE_GSM_TO_LTE end
/**
 * csn_decode_packet_measurement_order
 */
static csn_decoder_error_t csn_decode_packet_measurement_order(
  csn_decode_memory_instance_t *mem,
  uint8 *src,
  uint16 *pos_ptr,
  packet_measurement_order_t *dst,
  const gas_id_t gas_id
)
{
  csn_decoder_error_t retval = CSN_DECODE_NO_ERROR;

  CSN_DECODE_MEMORY_IN_USE(mem);

  memset(dst, 0, sizeof(packet_measurement_order_t));

  if (gunpackb(src, pos_ptr, (uint16) 1) == 0)
  {
    /* extract the Global TFI */
    dst->tlli_gtfi_pkt_ref_valid = global_tfi_valid;
    csn_global_tfi_ie(src, pos_ptr, &(dst->global_tfi));
  }
  else if (gunpackb(src, pos_ptr, (uint16) 1) == 0)
  {
    /* extract the TLLI */
    dst->tlli_gtfi_pkt_ref_valid = tlli_valid;
    dst->tlli = gunpackd(src, pos_ptr, 32);
  }
  else
  {
    return ADDRESS_INFO_ERROR;
  }

  dst->pmo_index = gunpackb(src, pos_ptr, 3);
  dst->pmo_count = gunpackb(src, pos_ptr, 3);

  /* NC Measurement Parameters */
  if (gunpackb(src, pos_ptr, 1))
  {
    dst->nc_meas_params_valid = TRUE;

    dst->nc_meas_params = CSN_DECODE_MEM_MALLOC(sizeof(csn_pmo_pcco_nc_meas_params_t));
    if (dst->nc_meas_params == NULL) return NO_MEMORY;
    memset(dst->nc_meas_params, 0, sizeof(csn_pmo_pcco_nc_meas_params_t));

    dst->nc_meas_params->network_control_order = gunpackb(src, pos_ptr, 2);
    if (gunpackb(src, pos_ptr, 1))
    {
      dst->nc_meas_params->nc_period_valid = TRUE;
      dst->nc_meas_params->nc_non_drx_period = gunpackb(src, pos_ptr, 3);
      dst->nc_meas_params->nc_reporting_period_i = gunpackb(src, pos_ptr, 3);
      dst->nc_meas_params->nc_reporting_period_t = gunpackb(src, pos_ptr, 3);
    }
    if (gunpackb(src, pos_ptr, 1))
    {
      dst->nc_meas_params->nc_frequency_list_valid = TRUE;
      retval = csn_decode_nc_meas_params_nc_freq_list(mem, src, pos_ptr, &dst->nc_meas_params->nc_frequency_list);
      if (retval != CSN_DECODE_NO_ERROR)
      {
        return retval;
      }
    }
  }

  /* EXT Measurement Parameters - removed in Release 6 specifications */
  if (gunpackb(src, pos_ptr, 1))
  {
    MSG_GERAN_ERROR_0("Ext Measurement Parameters no longer in spec");
    return NON_DISTRIBUTION_ERROR;
  }

  if (gunpackb(src, pos_ptr, 1))
  {
    /* Additions in Release 98 */

    if (gunpackb(src, pos_ptr, 1))
    {
      /* Extract and discard the LSA parameters */
      csn_discard_lsa_parameters(src, pos_ptr);
    }

    if (gunpackb(src, pos_ptr, 1))
    {
      /* Additions in Release 99 */

      if (gunpackb(src, pos_ptr, 1))
      {
        /* ENH Measurement Parameters */
        dst->gprs_meas_params_valid = TRUE;

        dst->gprs_meas_params = CSN_DECODE_MEM_MALLOC(sizeof(csn_gprs_meas_params_t));
        if (dst->gprs_meas_params == NULL) return NO_MEMORY;
        memset(dst->gprs_meas_params, 0, sizeof(csn_gprs_meas_params_t));

        if (gunpackb(src, pos_ptr, 1))
        {
          /* discard psi3_change_mark */
          (void)gunpackb(src, pos_ptr, 2);
        }
        else
        {
          /* discard ba_ind */
          (void)gunpackb(src, pos_ptr, 1);
          /* discard 3g_ba_ind */
          (void)gunpackb(src, pos_ptr, 1);
        }

        dst->gprs_meas_params->pmo_ind_valid = TRUE;
        dst->gprs_meas_params->pmo_ind = gunpackb(src, pos_ptr, 1);
        dst->gprs_meas_params->report_type_valid = TRUE;
        dst->gprs_meas_params->report_type = gunpackb(src, pos_ptr, 1);
        dst->gprs_meas_params->reporting_rate_valid = TRUE;
        dst->gprs_meas_params->reporting_rate = gunpackb(src, pos_ptr, 1);
        dst->gprs_meas_params->invalid_bsic_reporting_valid = TRUE;
        dst->gprs_meas_params->invalid_bsic_reporting = gunpackb(src, pos_ptr, 1);

        if (gunpackb(src, pos_ptr, 1))
        {
          /* 3G Neighbour Cell Description is present */
          dst->_3g_neighbour_cells_valid = TRUE;

          dst->_3g_neighbour_cells = CSN_DECODE_MEM_MALLOC(sizeof(csn_3g_neighbour_cells_t));
          if (dst->_3g_neighbour_cells == NULL) return NO_MEMORY;
          memset(dst->_3g_neighbour_cells , 0, sizeof(csn_3g_neighbour_cells_t));

          if (gunpackb(src, pos_ptr, 1))
          {
            dst->_3g_neighbour_cells->index_start_3g_valid = TRUE;
            dst->_3g_neighbour_cells->index_start_3g = gunpackb(src, pos_ptr, 7);
          }
          else
          {
            dst->_3g_neighbour_cells->index_start_3g_valid = FALSE;
          }

          if (gunpackb(src, pos_ptr, 1))
          {
            dst->_3g_neighbour_cells->absolute_index_start_emr_valid = TRUE;
            dst->_3g_neighbour_cells->absolute_index_start_emr = gunpackb(src, pos_ptr, 7);
          }
          else
          {
            dst->_3g_neighbour_cells->absolute_index_start_emr_valid = FALSE;
          }

          if (gunpackb(src, pos_ptr, 1))
          {
            dst->_3g_neighbour_cells->utran_fdd_description_valid = TRUE;

            dst->_3g_neighbour_cells->utran_fdd_description = CSN_DECODE_MEM_MALLOC(sizeof(utran_fdd_description_t));
            if (dst->_3g_neighbour_cells->utran_fdd_description == NULL) return NO_MEMORY;
            memset(dst->_3g_neighbour_cells->utran_fdd_description, 0, sizeof(utran_fdd_description_t));

            retval = csn_decode_utran_fdd_description(
                         mem, src, pos_ptr, dst->_3g_neighbour_cells->utran_fdd_description);
            if (retval != CSN_DECODE_NO_ERROR)
            {
              return retval;
            }
          }
          else
          {
            dst->_3g_neighbour_cells->utran_fdd_description_valid = FALSE;
          }

          if (gunpackb(src, pos_ptr, 1))
          {
            dst->_3g_neighbour_cells->utran_tdd_description_valid = TRUE;

            dst->_3g_neighbour_cells->utran_tdd_description = CSN_DECODE_MEM_MALLOC(sizeof(utran_tdd_description_t));
            if (dst->_3g_neighbour_cells->utran_tdd_description == NULL) return NO_MEMORY;
            memset(dst->_3g_neighbour_cells->utran_tdd_description, 0, sizeof(utran_tdd_description_t));

            retval = csn_decode_utran_tdd_description(
                                                 mem, src, pos_ptr, dst->_3g_neighbour_cells->utran_tdd_description);
            if (retval != CSN_DECODE_NO_ERROR)
            {
              return retval;
            }
          }
          else
          {
            dst->_3g_neighbour_cells->utran_tdd_description_valid = FALSE;
          }

          if (gunpackb(src, pos_ptr, 1))
          {
            dst->_3g_neighbour_cells->cdma2000_description_valid = TRUE;

            dst->_3g_neighbour_cells->cdma2000_description = CSN_DECODE_MEM_MALLOC(sizeof(cdma2000_description_t));
            if (dst->_3g_neighbour_cells->cdma2000_description == NULL) return NO_MEMORY;
            memset(dst->_3g_neighbour_cells->cdma2000_description, 0, sizeof(cdma2000_description_t));

            retval = csn_decode_cdma2000_description(
                                                 mem, src, pos_ptr, dst->_3g_neighbour_cells->cdma2000_description);
            if (retval != CSN_DECODE_NO_ERROR)
            {
              return retval;
            }
          }
          else
          {
            dst->_3g_neighbour_cells->cdma2000_description_valid = FALSE;
          }

          if (gunpackb(src, pos_ptr, 1))
          {
            dst->_3g_neighbour_cells->removed_3g_cell_description_valid = TRUE;

            dst->_3g_neighbour_cells->removed_3g_cell_description = CSN_DECODE_MEM_MALLOC(sizeof(removed_3g_cell_description_t));
            if (dst->_3g_neighbour_cells->removed_3g_cell_description == NULL) return NO_MEMORY;
            memset(dst->_3g_neighbour_cells->removed_3g_cell_description, 0, sizeof(removed_3g_cell_description_t));


            retval = csn_decode_removed_3g_cell_description(src, pos_ptr, dst->_3g_neighbour_cells->removed_3g_cell_description);
            if (retval != CSN_DECODE_NO_ERROR)
            {
              return retval;
            }
          }
          else
          {
            dst->_3g_neighbour_cells->removed_3g_cell_description_valid = FALSE;
          }
        }

        /* GPRS REP PRIORITY */
        if (gunpackb(src, pos_ptr, 1))
        {
          int32 i;

          dst->gprs_rep_priority_valid = TRUE;

          dst->gprs_rep_priority = CSN_DECODE_MEM_MALLOC(sizeof(csn_gprs_rep_priority_t));
          if (dst->gprs_rep_priority == NULL) return NO_MEMORY;
          memset(dst->gprs_rep_priority, 0, sizeof(csn_gprs_rep_priority_t));

          dst->gprs_rep_priority_valid = TRUE;
          dst->gprs_rep_priority->num_of_cells = gunpackb(src, pos_ptr, 7);
          for (i = 0; i < dst->gprs_rep_priority->num_of_cells; i++)
          {
            dst->gprs_rep_priority->priority[i] = gunpackb(src, pos_ptr, 1);
          }
        } /* END OF GPRS REP PRIORITY */

        if (gunpackb(src, pos_ptr, 1))
        {
          if (gunpackb(src, pos_ptr, 1))
          {
            dst->gprs_meas_params->multiband_reporting_valid = TRUE;
            dst->gprs_meas_params->multiband_reporting = gunpackb(src, pos_ptr, 2);
          }

          if (gunpackb(src, pos_ptr, 1))
          {
            dst->gprs_meas_params->serving_band_reporting_valid = TRUE;
            dst->gprs_meas_params->serving_band_reporting = gunpackb(src, pos_ptr, 2);
          }

          dst->gprs_meas_params->scale_ord_valid = TRUE;
          dst->gprs_meas_params->scale_ord = gunpackb(src, pos_ptr, 2);

          if (gunpackb(src, pos_ptr, 1))
          {
            dst->gprs_meas_params->reporting_900_valid = TRUE;
            dst->gprs_meas_params->reporting_900_offset = gunpackb(src, pos_ptr, 3);
            dst->gprs_meas_params->reporting_900_threshold = gunpackb(src, pos_ptr, 3);
          }

          if (gunpackb(src, pos_ptr, 1))
          {
            dst->gprs_meas_params->reporting_1800_valid = TRUE;
            dst->gprs_meas_params->reporting_1800_offset = gunpackb(src, pos_ptr, 3);
            dst->gprs_meas_params->reporting_1800_threshold = gunpackb(src, pos_ptr, 3);
          }

          if (gunpackb(src, pos_ptr, 1))
          {
            dst->gprs_meas_params->reporting_450_valid = TRUE;
            dst->gprs_meas_params->reporting_450_offset = gunpackb(src, pos_ptr, 3);
            dst->gprs_meas_params->reporting_450_threshold = gunpackb(src, pos_ptr, 3);
          }

          if (gunpackb(src, pos_ptr, 1))
          {
            dst->gprs_meas_params->reporting_1900_valid = TRUE;
            dst->gprs_meas_params->reporting_1900_offset = gunpackb(src, pos_ptr, 3);
            dst->gprs_meas_params->reporting_1900_threshold = gunpackb(src, pos_ptr, 3);
          }

          if (gunpackb(src, pos_ptr, 1))
          {
            dst->gprs_meas_params->reporting_850_valid = TRUE;
            dst->gprs_meas_params->reporting_850_offset = gunpackb(src, pos_ptr, 3);
            dst->gprs_meas_params->reporting_850_threshold = gunpackb(src, pos_ptr, 3);
          }
        }

        if (gunpackb(src, pos_ptr, 1))
        {
          /* GPRS 3G Measurement Parameters Description is present */
          dst->gprs_3g_meas_params_valid = TRUE;

          dst->gprs_3g_meas_params = CSN_DECODE_MEM_MALLOC(sizeof(csn_gprs_3g_meas_params_t));
          if (dst->gprs_3g_meas_params == NULL) return NO_MEMORY;
          memset(dst->gprs_3g_meas_params, 0, sizeof(csn_gprs_3g_meas_params_t));

          dst->gprs_3g_meas_params->qsearch_p_valid = TRUE;
          dst->gprs_3g_meas_params->qsearch_p = gunpackb(src, pos_ptr, 4);
          dst->gprs_3g_meas_params->_3g_search_prio_valid = TRUE;
          dst->gprs_3g_meas_params->_3g_search_prio = gunpackb(src, pos_ptr, 1);

          if (gunpackb(src, pos_ptr, 1))
          {
            dst->gprs_3g_meas_params->fdd_rep_quant_valid = TRUE;
            dst->gprs_3g_meas_params->fdd_rep_quant = gunpackb(src, pos_ptr, 1);
            dst->gprs_3g_meas_params->fdd_multirat_reporting_valid = TRUE;
            dst->gprs_3g_meas_params->fdd_multirat_reporting = gunpackb(src, pos_ptr, 2);
          }

          if (gunpackb(src, pos_ptr, 1))
          {
            dst->gprs_3g_meas_params->fdd_reporting_offset_valid = TRUE;
            dst->gprs_3g_meas_params->fdd_reporting_offset = gunpackb(src, pos_ptr, 3);
            dst->gprs_3g_meas_params->fdd_reporting_threshold_valid = TRUE;
            dst->gprs_3g_meas_params->fdd_reporting_threshold = gunpackb(src, pos_ptr, 3);
          }

          if (gunpackb(src, pos_ptr, 1))
          {
            dst->gprs_3g_meas_params->tdd_multirat_reporting_valid = TRUE;
            dst->gprs_3g_meas_params->tdd_multirat_reporting = gunpackb(src, pos_ptr, 2);
          }

          if (gunpackb(src, pos_ptr, 1))
          {
            dst->gprs_3g_meas_params->tdd_reporting_offset_valid = TRUE;
            dst->gprs_3g_meas_params->tdd_reporting_offset = gunpackb(src, pos_ptr, 3);
            dst->gprs_3g_meas_params->tdd_reporting_threshold_valid = TRUE;
            dst->gprs_3g_meas_params->tdd_reporting_threshold = gunpackb(src, pos_ptr, 3);
          }

          if (gunpackb(src, pos_ptr, 1))
          {
            dst->gprs_3g_meas_params->cdma2000_multirat_reporting_valid = TRUE;
            dst->gprs_3g_meas_params->cdma2000_multirat_reporting = gunpackb(src, pos_ptr, 2);
          }

          if (gunpackb(src, pos_ptr, 1))
          {
            dst->gprs_3g_meas_params->cdma2000_reporting_offset_valid = TRUE;
            dst->gprs_3g_meas_params->cdma2000_reporting_offset = gunpackb(src, pos_ptr, 3);
            dst->gprs_3g_meas_params->cdma2000_reporting_threshold_valid = TRUE;
            dst->gprs_3g_meas_params->cdma2000_reporting_threshold = gunpackb(src, pos_ptr, 3);
          }
        }
      }

      if (gunpackb(src, pos_ptr, 1))
      {
        /* Additions in Release 4 */
        dst->r4_additions_flag = TRUE;
        dst->ccn_active = gunpackb(src, pos_ptr, 1);
        if (gunpackb(src, pos_ptr, 1))
        {
          dst->ccn_support_flag = TRUE;
          csn_ccn_support_description_extract(src, pos_ptr, &(dst->ccn_support_desc));
        }

        if (gunpackb(src, pos_ptr, 1))
        {
          /* Additions in Release 5 - extracted but discarded */
          dst->r5_additions_flag = FALSE;

          if (gunpackb(src, pos_ptr, 1))
          {
            /* discard G-RNTI extension */
            (void)gunpackb(src, pos_ptr, 4);
          }

          if (gunpackb(src, pos_ptr, 1))
          {
            /* discard Iu Mode Neighbour Cell Parameters */
            while (gunpackb(src, pos_ptr, 1))
            {
              csn_discard_iu_mode_ncell_params(src, pos_ptr);
            }
          }

          if (gunpackb(src, pos_ptr, 1))
          {
            /* discard NC Iu Mode Only Cell List */
            csn_discard_iu_mode_only_cell_list(src, pos_ptr);
          }

          if (gunpackb(src, pos_ptr, 1))
          {
            /* discard GPRS 3G Additional Measurement Parameters Description */
            if (gunpackb(src, pos_ptr, 1))
            {
              /* FDD_REPORTING_THRESHOLD_2 */
              (void)gunpackb(src, pos_ptr, 6);
            }
          }

          if (gunpackb(src, pos_ptr, 1))
          {
            /* Additions in Release 6 */
            dst->r6_additions_flag  = TRUE;
            dst->three_g_ccn_active = gunpackb(src, pos_ptr, 1);
//tlx dime 2.1 with fr3009 FEATURE_GSM_TO_LTE added new func
            /* Release 7 Additions */
            if (gunpackb(src, pos_ptr, 1))
            {
              if (gunpackb(src, pos_ptr, 1))
              {
                (void) gunpackb(src, pos_ptr, 3); /* 700_REPORTING_OFFSET */
                (void) gunpackb(src, pos_ptr, 3); /* 700_REPORTING_THRESHOLD */
              }

              if (gunpackb(src, pos_ptr, 1))
              {
                (void) gunpackb(src, pos_ptr, 3); /* 810_REPORTING_OFFSET */
                (void) gunpackb(src, pos_ptr, 3); /* 810_REPORTING_THRESHOLD */
              }

              /* Release 8 Additions */
              if (gunpackb(src, pos_ptr, 1))
              {
                if (gunpackb(src, pos_ptr, 1))
                {
                  dst->ba_3g_ind_pmo_ind_valid = TRUE;
                  dst->ba_3g_ind = gunpackb(src, pos_ptr, 1); /* 3G_BA_IND */
                  dst->pmo_ind = gunpackb(src, pos_ptr, 1); /* PMO_IND */
                }

                /* Priority and E-UTRAN Parameters Description */
                if (gunpackb(src, pos_ptr, 1))
                {
                  dst->priority_and_eutran_params_valid = TRUE;

                  /* Serving Cell Priority Parameters Description */
                  if (gunpackb(src, pos_ptr, 1))
                  {
                    dst->priority_and_eutran_params.serving_cell_priority_params = CSN_DECODE_MEM_MALLOC(sizeof(csn_gsm_priority_params_ie_t));
                    csn_decode_gsm_priority_params_ie(
                      src,
                      pos_ptr,
                      dst->priority_and_eutran_params.serving_cell_priority_params
                    );
                  }

                  /* 3G Priority Parameters Description */
                  if (gunpackb(src, pos_ptr, 1))
                  {
                    dst->priority_and_eutran_params.utran_priority_params = CSN_DECODE_MEM_MALLOC(sizeof(csn_utran_priority_params_ie_t));
                    csn_decode_utran_priority_params_ie(
                      src,
                      pos_ptr,
                      dst->priority_and_eutran_params.utran_priority_params
                    );
                  }

                  /* E-UTRAN Parameters Description */
                  if (gunpackb(src, pos_ptr, 1))
                  {
                    dst->priority_and_eutran_params.eutran_params = CSN_DECODE_MEM_MALLOC(sizeof(csn_eutran_params_ie_t));
                    csn_decode_eutran_params_ie(
                      src,
                      pos_ptr,
                      dst->priority_and_eutran_params.eutran_params
                    );
                  }
                }

                /* Individual Priorities */
                if (gunpackb(src, pos_ptr, 1))
                {
                  dst->individual_priorities = CSN_DECODE_MEM_MALLOC(sizeof(csn_individual_priorities_ie_t));
                  csn_decode_individual_priorities_ie(
                    src,
                    pos_ptr,
                    dst->individual_priorities
                  );
                }

                /* 3G CSG Description */
                if (gunpackb(src, pos_ptr, 1))
                {
                  dst->utran_csg_description = CSN_DECODE_MEM_MALLOC(sizeof(csn_utran_csg_description_ie_t));
                  csn_decode_utran_csg_description_ie(
                    src,
                    pos_ptr,
                    dst->utran_csg_description
                  );
                }

                /* E-UTRAN CSG Description */
                if (gunpackb(src, pos_ptr, 1))
                {
                  dst->eutran_csg_description = CSN_DECODE_MEM_MALLOC(sizeof(csn_eutran_csg_description_ie_t));
                  csn_decode_eutran_csg_description_ie(
                    src,
                    pos_ptr,
                    dst->eutran_csg_description
                  );
                }

                /* Measurement Control Parameters Description */
                if (gunpackb(src, pos_ptr, 1))
                {
                  dst->measurement_control_params = CSN_DECODE_MEM_MALLOC(sizeof(csn_measurement_control_params_ie_t));
                  csn_decode_measurement_control_params_ie(
                    src,
                    pos_ptr,
                    dst->measurement_control_params
                  );
                }

                /* Release 9 Additions */
                if (gunpackb(src, pos_ptr, 1))
                {
                  /* Enhanced Cell Reselection Parameters Description */
                  if (gunpackb(src, pos_ptr, 1))
                  {

                  }

                  /* CSG Cells Reporting Description */
                  if (gunpackb(src, pos_ptr, 1))
                  {

                  }
                }
              }

            } /* end of Release 7 additions */
//tlx dime 2.1 with fr3009 FEATURE_GSM_TO_LTE end
          } /* end of Release 6 additions */
        } /* end of Release 5 additions */
      } /* end of Release 4 additions */
    } /* end of Release 99 additions */
  }

  return retval;
}

static csn_decoder_error_t csn_decode_packet_cell_change_order(
  csn_decode_memory_instance_t *mem,
  uint8 *src,
  uint16 *pos_ptr,
  packet_cell_change_order_t *dst,
  const gas_id_t gas_id
)
{
  csn_decoder_error_t retval = CSN_DECODE_NO_ERROR;

  CSN_DECODE_MEMORY_IN_USE(mem);

  memset(dst, 0, sizeof(packet_cell_change_order_t));

  if (gunpackb(src, pos_ptr, (uint16) 1) == 0)
  {
    /* extract the Global TFI */
    dst->tlli_gtfi_pkt_ref_valid = global_tfi_valid;
    csn_global_tfi_ie(src, pos_ptr, &(dst->global_tfi));
  }
  else if (gunpackb(src, pos_ptr, (uint16) 1) == 0)
  {
    /* extract the TLLI */
    dst->tlli_gtfi_pkt_ref_valid = tlli_valid;
    dst->tlli = gunpackd(src, pos_ptr, 32);
  }
  else
  {
    return ADDRESS_INFO_ERROR;
  }

  if (gunpackb(src, pos_ptr, 1) == 0)
  {
    dst->gsm_3g_cell_type = CSN_PCCO_GSM_TARGET_CELL;
    dst->immediate_rel = gunpackb(src, pos_ptr, 1);

    CSN_DECODE_ARFCN(&(dst->cell.gsm), pos_ptr);

    dst->cell.gsm.bsic = gunpackb(src, pos_ptr, 6);

    dst->cell.gsm.nc_meas_params.network_control_order = gunpackb(src, pos_ptr, 2);
    if (gunpackb(src, pos_ptr, 1))
    {
      dst->cell.gsm.nc_meas_params.nc_period_valid = TRUE;
      dst->cell.gsm.nc_meas_params.nc_non_drx_period = gunpackb(src, pos_ptr, 3);
      dst->cell.gsm.nc_meas_params.nc_reporting_period_i = gunpackb(src, pos_ptr, 3);
      dst->cell.gsm.nc_meas_params.nc_reporting_period_t = gunpackb(src, pos_ptr, 3);
    }
    if (gunpackb(src, pos_ptr, 1))
    {
      dst->cell.gsm.nc_meas_params.nc_frequency_list_valid = TRUE;
      retval = csn_decode_nc_meas_params_nc_freq_list(
                                                 mem, src, pos_ptr, &dst->cell.gsm.nc_meas_params.nc_frequency_list);
      if (retval != CSN_DECODE_NO_ERROR)
      {
        return retval;
      }
    }

    /* check for presence of Rel 98 features */
    if (gunpackb(src, pos_ptr, 1))
    {
      /* Additions in Release 98 */
      if (gunpackb(src, pos_ptr, 1))
      {
        csn_discard_lsa_parameters(src, pos_ptr);
      }

      /* check for presence of Rel 99 features */
      if (gunpackb(src, pos_ptr, 1))
      { /* < ENH Measurement parameters struct > */
        csn_3g_neighbour_cells_t *_3g_neighbour_cells;
        csn_gprs_rep_priority_t *gprs_rep_priority;
        csn_gprs_meas_params_t *gprs_meas_params;
        csn_gprs_3g_meas_params_t *gprs_3g_meas_params;

        if (gunpackb(src, pos_ptr, 1))
        {
          dst->cell.gsm.psi3_change_mark_valid = TRUE;
          dst->cell.gsm.psi3_change_mark = gunpackb(src, pos_ptr, 2);
        }
        else
        {
          dst->cell.gsm.psi3_change_mark_valid = FALSE;
          dst->cell.gsm.ba_ind = gunpackb(src, pos_ptr, 1);
          dst->cell.gsm.ba_ind_3g = gunpackb(src, pos_ptr, 1);
        }

        gprs_meas_params = CSN_DECODE_MEM_MALLOC(sizeof(csn_gprs_meas_params_t));
        if (gprs_meas_params == NULL) return NO_MEMORY;
        dst->cell.gsm.gprs_meas_params = gprs_meas_params;
        dst->cell.gsm.gprs_meas_params_valid = TRUE;

        /* unpack the first few mandatory elements that are part of the GPRS Meas Params struct */
        gprs_meas_params->pmo_ind_valid = TRUE;
        gprs_meas_params->pmo_ind = gunpackb(src, pos_ptr, 1);
        gprs_meas_params->report_type_valid = TRUE;
        gprs_meas_params->report_type = gunpackb(src, pos_ptr, 1);
        gprs_meas_params->reporting_rate_valid = TRUE;
        gprs_meas_params->reporting_rate = gunpackb(src, pos_ptr, 1);
        gprs_meas_params->invalid_bsic_reporting_valid = TRUE;
        gprs_meas_params->invalid_bsic_reporting = gunpackb(src, pos_ptr, 1);

        /* unpack the 3G neighbour cell description */
        if (gunpackb(src, pos_ptr, 1))
        {
          _3g_neighbour_cells = CSN_DECODE_MEM_MALLOC(sizeof(csn_3g_neighbour_cells_t));
          if (_3g_neighbour_cells == NULL) return NO_MEMORY;
          dst->cell.gsm._3g_neighbour_cells = _3g_neighbour_cells;
          dst->cell.gsm._3g_neighbour_cells_valid = TRUE;
          memset(_3g_neighbour_cells,0,sizeof(csn_3g_neighbour_cells_t));

          if (gunpackb(src, pos_ptr, 1))
          {
            _3g_neighbour_cells->index_start_3g_valid = TRUE;
            _3g_neighbour_cells->index_start_3g = gunpackb(src, pos_ptr, 7);
          }
          else
          {
            _3g_neighbour_cells->index_start_3g_valid = FALSE;
          }

          if (gunpackb(src, pos_ptr, 1))
          {
            _3g_neighbour_cells->absolute_index_start_emr_valid = TRUE;
            _3g_neighbour_cells->absolute_index_start_emr = gunpackb(src, pos_ptr, 7);
          }
          else
          {
            _3g_neighbour_cells->absolute_index_start_emr_valid = FALSE;
          }

          if (gunpackb(src, pos_ptr, 1))
          {
            _3g_neighbour_cells->utran_fdd_description_valid = TRUE;

            _3g_neighbour_cells->utran_fdd_description = CSN_DECODE_MEM_MALLOC(sizeof(utran_fdd_description_t));
            if (_3g_neighbour_cells->utran_fdd_description == NULL) return NO_MEMORY;
            memset(_3g_neighbour_cells->utran_fdd_description, 0, sizeof(utran_fdd_description_t));

            retval = csn_decode_utran_fdd_description(
                                mem, src, pos_ptr, _3g_neighbour_cells->utran_fdd_description);
            if (retval != CSN_DECODE_NO_ERROR) return retval;
          }
          else
          {
            _3g_neighbour_cells->utran_fdd_description_valid = FALSE;
          }

          if (gunpackb(src, pos_ptr, 1))
          {
            _3g_neighbour_cells->utran_tdd_description_valid = TRUE;

            _3g_neighbour_cells->utran_tdd_description = CSN_DECODE_MEM_MALLOC(sizeof(utran_tdd_description_t));
            if (_3g_neighbour_cells->utran_tdd_description == NULL) return NO_MEMORY;
            memset(_3g_neighbour_cells->utran_tdd_description, 0, sizeof(utran_tdd_description_t));

            retval = csn_decode_utran_tdd_description(
                                mem, src, pos_ptr, _3g_neighbour_cells->utran_tdd_description);
            if (retval != CSN_DECODE_NO_ERROR) return retval;
          }
          else
          {
            _3g_neighbour_cells->utran_tdd_description_valid = FALSE;
          }

          if (gunpackb(src, pos_ptr, 1))
          {
            _3g_neighbour_cells->removed_3g_cell_description_valid = TRUE;

            _3g_neighbour_cells->removed_3g_cell_description = CSN_DECODE_MEM_MALLOC(sizeof(removed_3g_cell_description_t));
            if (_3g_neighbour_cells->removed_3g_cell_description == NULL) return NO_MEMORY;
            memset(_3g_neighbour_cells->removed_3g_cell_description, 0, sizeof(removed_3g_cell_description_t));


            retval = csn_decode_removed_3g_cell_description(src, pos_ptr, _3g_neighbour_cells->removed_3g_cell_description);
            if (retval != CSN_DECODE_NO_ERROR) return retval;
          }
          else
          {
            _3g_neighbour_cells->removed_3g_cell_description_valid = FALSE;
          }
        }

        /* GPRS REP PRIORITY */
        if (gunpackb(src, pos_ptr, 1))
        {
          int32 i;
          gprs_rep_priority = CSN_DECODE_MEM_MALLOC(sizeof(csn_gprs_rep_priority_t));
          if (gprs_rep_priority == NULL) return NO_MEMORY;
          dst->cell.gsm.gprs_rep_priority = gprs_rep_priority;
          dst->cell.gsm.gprs_rep_priority_valid = TRUE;

          gprs_rep_priority->num_of_cells = gunpackb(src, pos_ptr, 7);
          for (i = 0; i < gprs_rep_priority->num_of_cells; i++)
          {
            gprs_rep_priority->priority[i] = gunpackb(src, pos_ptr, 1);
          }
        } /* END OF GPRS REP PRIORITY */

        /* GPRS MEASUREMENT PARAMETERS */
        if (gunpackb(src, pos_ptr, 1))
        {
          if (gunpackb(src, pos_ptr, 1))
          {
            gprs_meas_params->multiband_reporting_valid = TRUE;
            gprs_meas_params->multiband_reporting = gunpackb(src, pos_ptr, 2);
          }

          if (gunpackb(src, pos_ptr, 1))
          {
            gprs_meas_params->serving_band_reporting_valid = TRUE;
            gprs_meas_params->serving_band_reporting = gunpackb(src, pos_ptr, 2);
          }

          /* mandatory in PMO/PCCO */
          gprs_meas_params->scale_ord_valid = TRUE;
          gprs_meas_params->scale_ord = gunpackb(src, pos_ptr, 2);

          if (gunpackb(src, pos_ptr, 1))
          {
            gprs_meas_params->reporting_900_valid = TRUE;
            gprs_meas_params->reporting_900_offset = gunpackb(src, pos_ptr, 3);
            gprs_meas_params->reporting_900_threshold = gunpackb(src, pos_ptr, 3);
          }

          if (gunpackb(src, pos_ptr, 1))
          {
            gprs_meas_params->reporting_1800_valid = TRUE;
            gprs_meas_params->reporting_1800_offset = gunpackb(src, pos_ptr, 3);
            gprs_meas_params->reporting_1800_threshold = gunpackb(src, pos_ptr, 3);
          }

          if (gunpackb(src, pos_ptr, 1))
          {
            gprs_meas_params->reporting_450_valid = TRUE;
            gprs_meas_params->reporting_450_offset = gunpackb(src, pos_ptr, 3);
            gprs_meas_params->reporting_450_threshold = gunpackb(src, pos_ptr, 3);
          }

          if (gunpackb(src, pos_ptr, 1))
          {
            gprs_meas_params->reporting_1900_valid = TRUE;
            gprs_meas_params->reporting_1900_offset = gunpackb(src, pos_ptr, 3);
            gprs_meas_params->reporting_1900_threshold = gunpackb(src, pos_ptr, 3);
          }

          if (gunpackb(src, pos_ptr, 1))
          {
            gprs_meas_params->reporting_850_valid = TRUE;
            gprs_meas_params->reporting_850_offset = gunpackb(src, pos_ptr, 3);
            gprs_meas_params->reporting_850_threshold = gunpackb(src, pos_ptr, 3);
          }
        } /* END OF GPRS MEASUREMENT PARAMETERS */

        /* GPRS 3G MEASUREMENT PARAMETERS */
        if (gunpackb(src, pos_ptr, 1))
        {
          gprs_3g_meas_params = CSN_DECODE_MEM_MALLOC(sizeof(csn_gprs_3g_meas_params_t));
          if (gprs_3g_meas_params == NULL) return NO_MEMORY;
          dst->cell.gsm.gprs_3g_meas_params = gprs_3g_meas_params;
          dst->cell.gsm.gprs_3g_meas_params_valid = TRUE;

          gprs_3g_meas_params->qsearch_p_valid = TRUE;
          gprs_3g_meas_params->qsearch_p = gunpackb(src, pos_ptr, 4);
          gprs_3g_meas_params->_3g_search_prio_valid = TRUE;
          gprs_3g_meas_params->_3g_search_prio = gunpackb(src, pos_ptr, 1);

          if (gunpackb(src, pos_ptr, 1))
          {
            gprs_3g_meas_params->fdd_rep_quant_valid = TRUE;
            gprs_3g_meas_params->fdd_rep_quant = gunpackb(src, pos_ptr, 1);
            gprs_3g_meas_params->fdd_multirat_reporting_valid = TRUE;
            gprs_3g_meas_params->fdd_multirat_reporting = gunpackb(src, pos_ptr, 2);
          }

          if (gunpackb(src, pos_ptr, 1))
          {
            gprs_3g_meas_params->fdd_reporting_offset_valid = TRUE;
            gprs_3g_meas_params->fdd_reporting_offset = gunpackb(src, pos_ptr, 3);
            gprs_3g_meas_params->fdd_reporting_threshold_valid = TRUE;
            gprs_3g_meas_params->fdd_reporting_threshold = gunpackb(src, pos_ptr, 3);
          }

          if (gunpackb(src, pos_ptr, 1))
          {
            gprs_3g_meas_params->tdd_multirat_reporting_valid = TRUE;
            gprs_3g_meas_params->tdd_multirat_reporting = gunpackb(src, pos_ptr, 2);
          }

          if (gunpackb(src, pos_ptr, 1))
          {
            gprs_3g_meas_params->tdd_reporting_offset_valid = TRUE;
            gprs_3g_meas_params->tdd_reporting_offset = gunpackb(src, pos_ptr, 3);
            gprs_3g_meas_params->tdd_reporting_threshold_valid = TRUE;
            gprs_3g_meas_params->tdd_reporting_threshold = gunpackb(src, pos_ptr, 3);
          }
        } /* END OF GPRS 3G MEASUREMENT PARAMETERS */

        if (gunpackb(src, pos_ptr, 1))
        {
          dst->cell.gsm.r4_additions_flag = TRUE;
          dst->cell.gsm.ccn_active = gunpackb(src, pos_ptr, 1);

          if (gunpackb(src, pos_ptr, 1))
          {
            dst->cell.gsm.container_flag = TRUE;
            dst->cell.gsm.container_id = gunpackb(src, pos_ptr, CSN_CONTAINER_ID_LEN);
          }

          if (gunpackb(src, pos_ptr, 1))
          {
            dst->cell.gsm.ccn_support_flag = TRUE;
            csn_ccn_support_description_extract(src, pos_ptr, &(dst->cell.gsm.ccn_support_desc));
          }

          if (gunpackb(src, pos_ptr, 1))
          {
            /* Additions for Release 5 - extracted, but discarded */
            dst->cell.gsm.r5_additions_flag = FALSE;

            if (gunpackb(src, pos_ptr, 1))
            {
              /* discard G-RNTI extension */
              (void)gunpackb(src, pos_ptr, 4);
            }

            if (gunpackb(src, pos_ptr, 1))
            {
              /* discard Iu Mode Neighbour Cell Parameters  */
              while (gunpackb(src, pos_ptr, 1))
              {
                csn_discard_iu_mode_ncell_params(src, pos_ptr);
              }
            }

            if (gunpackb(src, pos_ptr, 1))
            {
              /* discard NC Iu Mode Only Cell List */
              csn_discard_iu_mode_only_cell_list(src, pos_ptr);
            }

            if (gunpackb(src, pos_ptr, 1))
            {
              /* discard GPRS 3G Additional Measurement Parameters Description */
              if (gunpackb(src, pos_ptr, 1))
              {
                /* FDD_REPORTING_THRESHOLD_2 */
                (void)gunpackb(src, pos_ptr, 6);
              }
            }

            if (gunpackb(src, pos_ptr, 1))
            {
              dst->cell.gsm.r6_additions_flag  = TRUE;
              dst->cell.gsm.three_g_ccn_active = gunpackb(src, pos_ptr, 1);
            } /* end of Release 6 additions */
          } /* end of Release 5 additions */
        } /* end of Release 4 additions */
      } /* end of Release 99 additions < ENH Measurement parameters struct > */
    } /* end of Release 98 additions */
  }
  else
  {
    dst->gsm_3g_cell_type = CSN_PCCO_3G_TARGET_CELL;
#ifdef FEATURE_INTERRAT_PCCO_GTOW
    /* message escape */
    (void)gunpackb(src, pos_ptr, 2);

    dst->immediate_rel = gunpackb(src, pos_ptr, 1);
    if (gunpackb(src, pos_ptr, 1))
    {
      dst->cell._3g.fdd_flag = TRUE;
      dst->cell._3g.fdd_arfcn = gunpackw(src, pos_ptr, 14);
      dst->cell._3g.fdd_diversity = gunpackb(src, pos_ptr, 1);
      if (gunpackb(src, pos_ptr, 1))
      {
        dst->cell._3g.fdd_bandwidth_flag = TRUE;
        dst->cell._3g.fdd_bandwidth = gunpackb(src, pos_ptr, 3);
      }
      dst->cell._3g.fdd_scrambling_code = gunpackw(src, pos_ptr, 9);
    }
#endif /* FEATURE_INTERRAT_PCCO_GTOW */
  }

  return retval;
}

/**
 * End of decoders for PACKET MEASUREMENT ORDER/PACKET CELL CHANGE ORDER
 */


/*===========================================================================
===
===  FUNCTION      csn_pkt_page_req
===
===  DESCRIPTION
===
===    This function extracts the information from the PACKET PAGING REQUEST
===    message.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/

static csn_decoder_error_t csn_pkt_page_req(
  csn_decode_memory_instance_t *mem,
  uint8 src[], uint16 *pos_ptr,
  packet_paging_request_t *msg_ptr,
  const gas_id_t gas_id
)
{
  csn_decoder_error_t return_status = CSN_DECODE_NO_ERROR;
  uint8 trap_count=MAX_REPEATED_PAGE;
  uint8 list_size=0;
  int8  persistence_count=0;

  CSN_DECODE_MEMORY_IN_USE(mem);

  /*  Clear persistence flag */
  msg_ptr->persistence_flag = FALSE;

  /* First information field is optional, may be Persistance Level */
  if (gunpackb(src, pos_ptr, (int16)1))
  {
    while (persistence_count < MAX_PERSIST_VALUES)
    {
      /*  Persistence levels present. */
      msg_ptr->persistence[persistence_count++] =
                                gunpackb(src, pos_ptr, CSN_PERSISTENCE_LEN);
    };

    /*  Set persistence flag */
    msg_ptr->persistence_flag = TRUE;
  }

  /*  Clear nln flag */
  msg_ptr->nln_flag = FALSE;

  /*  Next field is the optional nln bits */
  if (gunpackb(src, pos_ptr, (int16)1))
  {
    /*  NLN bits present */
    msg_ptr->nln = gunpackb(src, pos_ptr, CSN_NLN_LEN);

    /*  Set nln flag */
    msg_ptr->nln_flag = TRUE;
  }

  /*  Extract the repeated page infos, if present (may be several of them). */

  while (gunpackb(src, pos_ptr, (int16)1) && trap_count)
  {
    /*  Option and trap_count TRUE so extract freq list */
    csn_repeated_page_extract(src, pos_ptr,
                                          &(msg_ptr->repeated_page[list_size]));

    /*  Decrement trap_count so code cannot get 'stuck' */
    trap_count--;

    /*  Keep count */
    list_size++;
  };

  /*  Update number of frequency lists */
  msg_ptr->number_of_pages = list_size;

  return(return_status);

}

/*===========================================================================
===
===  FUNCTION      csn_pkt_pdch_release
===
===  DESCRIPTION
===
===    This function extracts the information from the PACKET PDCH RELEASE
===    message.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/

static csn_decoder_error_t csn_pkt_pdch_release(
  csn_decode_memory_instance_t *mem,
  uint8 *src,
  uint16 *pos_ptr,
  packet_pdch_release_t *dst,
  const gas_id_t gas_id
)
{
  csn_decoder_error_t return_status = CSN_DECODE_NO_ERROR;        /*  Sets error code to PASS */
  packet_pdch_release_t *temp_ptr;

  CSN_DECODE_MEMORY_IN_USE(mem);

  temp_ptr = (packet_pdch_release_t *)dst;

  /*  Only one bit to check and one element to extract */
  if (gunpackb(src, pos_ptr, (int16)1))
  {
    /*  Extract timeslots available */
    temp_ptr->timeslots_available =
                  gunpackb(src, pos_ptr, CSN_TIMESLOTS_AVAILABLE_LEN);
  }
  else
  {
    /*  Error occurred, set return status accordingly */
    return_status = DISTRIBUTION_ERROR;
  }
  return return_status;
}

/*===========================================================================
===
===  FUNCTION      csn_pkt_dwnlk_dummy_cntrl_blk
===
===  DESCRIPTION
===
===    This function extracts the information from the PACKET DOWNLINK DUMMY
===    CNTRL BLK message.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/

static csn_decoder_error_t csn_pkt_dwnlk_dummy_cntrl_blk(
  csn_decode_memory_instance_t *mem,
  uint8 *src,
  uint16 *pos_ptr,
  packet_dwnlk_dummy_cntrl_blk_t *dst,
  const gas_id_t gas_id
)
{
  packet_dwnlk_dummy_cntrl_blk_t *dmy_msg_ptr;
  uint8               persistence_count=0;

  CSN_DECODE_MEMORY_IN_USE(mem);

  /*  Set-up pointer within rlc_mac_hdr to correct message. */
  dmy_msg_ptr = (packet_dwnlk_dummy_cntrl_blk_t *)dst;

  /*  Clear flag to indicate persistence levels present. */
  dmy_msg_ptr->persistence_flag = FALSE;

  /* First information field is optional, may be Persistance Level */
  if (gunpackb(src, pos_ptr, (int16)1))
  {
    while (persistence_count < MAX_PERSIST_VALUES)
    {
      /*  Persistence levels present. */
      dmy_msg_ptr->persistence[persistence_count++] =
                gunpackb(src, pos_ptr, CSN_PERSISTENCE_LEN);
    };

    /*  Set flag to indicate persistence levels present. */
    dmy_msg_ptr->persistence_flag = TRUE;
  }
  return CSN_DECODE_NO_ERROR;
}


/*===========================================================================
===
===  FUNCTION      csn_pkt_poll_req
===
===  DESCRIPTION
===
===    This function extracts the information from the PACKET POLLING REQUEST
===    message.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/

static csn_decoder_error_t csn_pkt_poll_req(
  csn_decode_memory_instance_t *mem,
  uint8 src[], uint16 *pos_ptr,
  packet_polling_request_t *msg_ptr,
  const gas_id_t gas_id
)
{
  csn_decoder_error_t return_status = CSN_DECODE_NO_ERROR;

  CSN_DECODE_MEMORY_IN_USE(mem);

  /*  Check if address field is global tfi */
  if (!(gunpackb(src, pos_ptr, (int16)1)))
  {
    /*  Extract global tfi */
    csn_global_tfi_ie(src, pos_ptr, &(msg_ptr->global_tfi));

    /*  Set option enum to indicate that the global tfi is TRUE */
    msg_ptr->tlli_gtfi_pkt_ref_valid = global_tfi_valid;
  }
  else
  {
    /*  Check for either tlli or tqi */
    if (!(gunpackb(src, pos_ptr, (int16)1)))
    {
      /*  Extract TLLI */
      msg_ptr->tlli = gunpackd(src, pos_ptr, CSN_TLLI_LEN);

      /*  Set option enum to indicate that the tlli is TRUE */
      msg_ptr->tlli_gtfi_pkt_ref_valid = tlli_valid;
    }
    else
    {
      /*  Should be tqi but check for error case */
      if (!(gunpackb(src, pos_ptr, (int16)1)))
      {
        /*  Extract tqi */
        msg_ptr->tqi = gunpackw(src, pos_ptr, CSN_TQI_LEN);

        /*  Set option enum to indicate that the pkt requset is TRUE */
        msg_ptr->tlli_gtfi_pkt_ref_valid = tqi_valid;
      }
      else
      {
        /*  Address error has occurred. */
        return_status = ADDRESS_INFO_ERROR;
      }
    }
  }

  /*  Extract type_of_ack field */
  msg_ptr->type_of_ack = gunpackb(src, pos_ptr, CSN_TYPE_OF_ACK_LEN);

  return(return_status);

}



/*===========================================================================
===
===  FUNCTION     csn_pkt_pwrctrl_ta
===
===  DESCRIPTION
===
===    This function extracts the information from the PACKET POLLING REQUEST
===    message.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/

static csn_decoder_error_t csn_pkt_pwrctrl_ta(
  csn_decode_memory_instance_t *mem,
  uint8 src[], uint16 *pos_ptr,
  packet_pwr_ctrl_tmng_adv_t *msg_ptr,
  const gas_id_t gas_id
)
{
  csn_decoder_error_t return_status = CSN_DECODE_NO_ERROR;

  CSN_DECODE_MEMORY_IN_USE(mem);

  /*  Check if address field is global tfi */
  if (!(gunpackb(src, pos_ptr, (int16)1)))
  {
    /*  Extract global tfi */
    csn_global_tfi_ie(src, pos_ptr, &(msg_ptr->global_tfi));

    /*  Set option enum to indicate that the global tfi is TRUE */
    msg_ptr->tlli_gtfi_pkt_ref_valid = global_tfi_valid;
  }
  else
  {
    /*  Check for either tqi or Packet Reference Request */
    switch (gunpackb(src, pos_ptr, (int16)2))
    {
      case TQI_FIELD_PRESENT:
        /*  Extract tqi */
        msg_ptr->tqi = gunpackw(src, pos_ptr, CSN_TQI_LEN);

        /*  Set option enum to indicate that the tqi is TRUE */
        msg_ptr->tlli_gtfi_pkt_ref_valid = tqi_valid;
      break;

      default:
        /*  Address error has occurred. */
        return_status = ADDRESS_INFO_ERROR;
    }
  }

  /*  If ADDRESS_INFO_ERROR has occurred stop processing */
  if (!return_status)
  {
    /*  Check message escape bit */
    if (!(gunpackb(src, pos_ptr, (int16)1)))
    {
      /*  Clear global pwr ctrl flag prior to checking */
      msg_ptr->global_pwr_ctrl_flag = FALSE;

      /*  Clear global timing advance flag prior to checking */
      msg_ptr->global_timing_adv_flag = FALSE;

      /*  Clear power control flag prior to checking */
      msg_ptr->power_control_flag = FALSE;

      /*  Next field is optional global power control parameters */
      if (gunpackb(src, pos_ptr, (int16)1))
      {
        /*  Extract global power control parameters */
        csn_global_pwr_ctrl_ie(src, pos_ptr, &(msg_ptr->global_pwr_ctrl));

        /*  Set global pwr ctrl flag indicating option TRUE */
        msg_ptr->global_pwr_ctrl_flag = TRUE;
      }

      /*  Next field is conditional */
      if (!gunpackb(src, pos_ptr, (int16)1))
      {
        /*  Extract Global packet timing AND Power control parameters */
        csn_global_pkt_timing_ie(src, pos_ptr, &(msg_ptr->global_pkt_timing));

        csn_pwr_cntrl_params_ie(src, pos_ptr, &(msg_ptr->pwr_ctrl_paramtrs));

        /*  Set global timing advance flag */
        msg_ptr->global_timing_adv_flag = TRUE;

        /*  Set power control flag */
        msg_ptr->power_control_flag = TRUE;
      }
      else
      {
        /*  Extract Global packet timing OR Power control parameters */
        /*  Next field is conditional */
        if (!gunpackb(src, pos_ptr, (int16)1))
        {
          /*  Extract Global packet timing */
          csn_global_pkt_timing_ie(src,
                                 pos_ptr, &(msg_ptr->global_pkt_timing));

          /*  Set global timing advance flag */
          msg_ptr->global_timing_adv_flag = TRUE;
        }
        else
        {
          /*  Extract Power control parameters */
          csn_pwr_cntrl_params_ie(src,
                                 pos_ptr, &(msg_ptr->pwr_ctrl_paramtrs));

          /*  Set power control flag */
          msg_ptr->power_control_flag = TRUE;
        }
      }

      /*  Clear option flag before checking if optional field present */
      msg_ptr->r99_additions_flag = FALSE;

      /*  Add Additions for R99 */
      if (gunpackb(src, pos_ptr, (int16)1))
      {
        /*  Clear option flag before checking if optional field present */
        msg_ptr->pkt_ext_ta_flag = FALSE;

        if (gunpackb(src, pos_ptr, (int16)1))
        {
          /*  Packet Extended Timing Advance present */
          msg_ptr->pkt_ext_ta = gunpackb(src, pos_ptr, CSN_PKT_EXT_TA_LEN);

          /* Set conditional flag */
          msg_ptr->pkt_ext_ta_flag = TRUE;
        }

        /* Set conditional flag */
        msg_ptr->r99_additions_flag = TRUE;
      }
    }
    else
    {
      /*  A Message Escape error has occurred */
      return_status = MESSAGE_ESCAPE_ERROR;
    }

  }

  return(return_status);

}


/*===========================================================================
===
===  FUNCTION     csn_pkt_timeslot_reconf
===
===  DESCRIPTION
===
===    This function extracts the information from the PACKET TIMESLOT
===    RECONFIGURATION message.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/

static csn_decoder_error_t csn_pkt_timeslot_reconf(
  csn_decode_memory_instance_t *mem,
  uint8 src[], uint16 *pos_ptr,
  packet_timeslot_reconfigure_t *msg_ptr,
  const gas_id_t gas_id
)
{
  csn_decoder_error_t return_status = CSN_DECODE_NO_ERROR;

  CSN_DECODE_MEMORY_IN_USE(mem);

  /*  PTR does not carry EGPRS params */
  msg_ptr->egprs_ptr = FALSE;

  /*  First bit should be 0, if not an address error has occurred */
  if (!(gunpackb(src, pos_ptr, (int16)1)))
  {
    /*  Extract global tfi */
    csn_global_tfi_ie(src, pos_ptr, &(msg_ptr->global_tfi));

    /*  Check message escape bit */
    if (!(gunpackb(src, pos_ptr, (int16)1)))
    {
      /*  Extract channel coding command */
      msg_ptr->chan_coding_cmd =
                            gunpackb(src, pos_ptr, CSN_CHAN_CODING_CMD_LEN);

      /*  Extract global packet timing advance */
      csn_global_pkt_timing_ie(src, pos_ptr, &(msg_ptr->global_pkt_timing));

      /*  Extract downlink_rlc_mode */
      msg_ptr->dwnlk_rlc_mode =
                              gunpackb(src, pos_ptr, CSN_DWNLK_RLC_MODE_LEN);

      /*  Extract control ack bit */
      msg_ptr->control_ack = gunpackb(src, pos_ptr, CSN_CONTROL_ACK_LEN);

      /*  Next field is optional */
      /*  Clear option flag before checking if optional field present */
      msg_ptr->dwnlk_tfi_assignment_flag = FALSE;

      if (gunpackb(src, pos_ptr, (int16)1))
      {
        /*  Downlink tfi assignment present */
        msg_ptr->dwnlk_tfi_assignment = gunpackb(src,
                                    pos_ptr, CSN_DWNLK_TFI_ASSIGNMENT_LEN);

        /*  Set option flag before checking if optional field present */
        msg_ptr->dwnlk_tfi_assignment_flag = TRUE;
      }

      /*  Next field is optional */
      /*  Clear option flag before checking if optional field present */
      msg_ptr->uplk_tfi_assignment_flag = FALSE;

      if (gunpackb(src, pos_ptr, (int16)1))
      {
        /*  Uplink tfi assignment present */
        msg_ptr->uplk_tfi_assignment = gunpackb(src,
                                      pos_ptr, CSN_UPLINK_TFI_ASSIGNMENT_LEN);

        /*  Set option flag before checking if optional field present */
        msg_ptr->uplk_tfi_assignment_flag = TRUE;
      }

      /*  Extract downlink timeslot allocation */
      msg_ptr->dwnlk_ts_alloc =
                              gunpackb(src, pos_ptr, CSN_DWNLK_TS_ALLOC_LEN);

      /*  Next field is optional */
      /*  Clear option flag before checking if optional field present */
      msg_ptr->freq_param_flag = FALSE;

      if (gunpackb(src, pos_ptr, (int16)1))
      {
        /*  Frequency Parameters present */
        return_status = csn_frequency_params_ie(src, pos_ptr, &(msg_ptr->freq_params), gas_id);

        if (return_status != CSN_DECODE_NO_ERROR)
        {
          return return_status;
        }

        /*  Set option flag before checking if optional field present */
        msg_ptr->freq_param_flag = TRUE;
      }

      /*  Next field is conditional */

      if (!gunpackb(src, pos_ptr, (int16)1))
      {
        /*  Dynamic allocation struct present, last parameter indicates */
        /*  the presents of the uplink_tfi_assignment field in the */
        /*  dynamic allocation structure. */
        csn_dynamic_alloc_extract(src, pos_ptr,
                                            &(msg_ptr->dynamic_alloc), FALSE);

        /*  Set option flag before checking if optional field present */
        msg_ptr->alloc_flag = dynamic;
      }
      else
      {
        /*  Fixed allocation struct present - not supported */
        return NON_DISTRIBUTION_ERROR;
      }

      /*  Clear option flag before checking if optional field present */
      msg_ptr->r99_additions_flag = FALSE;

      /*  Add Additions for R99 */
      if (gunpackb(src, pos_ptr, (int16)1))
      {
        /*  Clear option flag before checking if optional field present */
        msg_ptr->pkt_ext_ta_flag = FALSE;

        if (gunpackb(src, pos_ptr, (int16)1))
        {
          /*  Packet Extended Timing Advance present */
          msg_ptr->pkt_ext_ta = gunpackb(src, pos_ptr, CSN_PKT_EXT_TA_LEN);

          /* Set conditional flag */
          msg_ptr->pkt_ext_ta_flag = TRUE;
        }

        /* Set conditional flag */
        msg_ptr->r99_additions_flag = TRUE;

      }
    }
    else
    {

        if (!(gunpackb(src, pos_ptr, (int16)2)))
        {

          msg_ptr->egprs_ptr = TRUE;

          /* { 0 | 1 < COMPACT reduced MA : < COMPACT reduced MA IE >> }  */
          /*  Check next bit */
          if ((gunpackb(src, pos_ptr, (int16)1)))
          {
              /*  Extract COMPACT reduced MA */
              csn_compact_reduced_ma_ie(src, pos_ptr, &(msg_ptr->compact_reduced_ma));
              msg_ptr->compact_reduced_ma_flag = TRUE;
          }
          else
          {
              msg_ptr->compact_reduced_ma_flag = FALSE;
          }

          /*/ < EGPRS Channel Coding Command : < EGPRS Modulation and Coding IE >>  */
          msg_ptr->egprs_chan_coding_cmd = gunpackb(src, pos_ptr, 4);

          /* < RESEGMENT : bit (1) >*/
          msg_ptr->resegment = gunpackb(src, pos_ptr, 1);

          /* {0 | 1 < DOWNLINK EGPRS Window Size : < EGPRS Window Size IE >}*/
          if ((gunpackb(src, pos_ptr, (int16)1)))
          {
            /* < EGPRS DL Window Size : < EGPRS Window Size IE >  */
            msg_ptr->dl_egprs_window_size = gunpackb(src, pos_ptr, 5);
            msg_ptr->dl_egprs_window_size_flag = TRUE;
          }
          else
          {
            msg_ptr->dl_egprs_window_size_flag = FALSE;
          }

          /*{ 0 | 1 < UPLINK EGPRS Window Size : < EGPRS Window Size IE >}*/
          if ((gunpackb(src, pos_ptr, (int16)1)))
          {
            /* < EGPRS UL Window Size : < EGPRS Window Size IE >  */
            msg_ptr->ul_egprs_window_size = gunpackb(src, pos_ptr,5);
            msg_ptr->ul_egprs_window_size_flag = TRUE;
          }
          else
          {
            msg_ptr->ul_egprs_window_size_flag = FALSE;
          }

          /*< LINK_QUALITY_MEASUREMENT_MODE : bit (2) >*/
          msg_ptr->link_qual_meas_mode = gunpackb(src, pos_ptr,2);

          /*  Extract global packet timing advance */
          csn_global_pkt_timing_ie(src, pos_ptr, &(msg_ptr->global_pkt_timing));

          /*{ 0 | 1 <Packet Extended Timing Advance : bit (2)> }*/
          if ((gunpackb(src, pos_ptr, (int16)1)))
          {
             msg_ptr->pkt_ext_ta = gunpackb(src, pos_ptr, CSN_PKT_EXT_TA_LEN);
             msg_ptr->pkt_ext_ta_flag = TRUE;
          }
          else
          {
            msg_ptr->pkt_ext_ta_flag = FALSE;
          }

          /*  Extract downlink_rlc_mode */
          msg_ptr->dwnlk_rlc_mode =
                                  gunpackb(src, pos_ptr, CSN_DWNLK_RLC_MODE_LEN);

          /*  Extract control ack bit */
          msg_ptr->control_ack = gunpackb(src, pos_ptr, CSN_CONTROL_ACK_LEN);

          /*  Next field is optional */
          /*  Clear option flag before checking if optional field present */
          msg_ptr->dwnlk_tfi_assignment_flag = FALSE;

          if (gunpackb(src, pos_ptr, (int16)1))
          {
            /*  Downlink tfi assignment present */
            msg_ptr->dwnlk_tfi_assignment = gunpackb(src,
                                        pos_ptr, CSN_DWNLK_TFI_ASSIGNMENT_LEN);

            /*  Set option flag before checking if optional field present */
            msg_ptr->dwnlk_tfi_assignment_flag = TRUE;
          }

          /*  Next field is optional */
          /*  Clear option flag before checking if optional field present */
          msg_ptr->uplk_tfi_assignment_flag = FALSE;

          if (gunpackb(src, pos_ptr, (int16)1))
          {
            /*  Uplink tfi assignment present */
            msg_ptr->uplk_tfi_assignment = gunpackb(src,
                                          pos_ptr, CSN_UPLINK_TFI_ASSIGNMENT_LEN);

            /*  Set option flag before checking if optional field present */
            msg_ptr->uplk_tfi_assignment_flag = TRUE;
          }

          /*  Extract downlink timeslot allocation */
          msg_ptr->dwnlk_ts_alloc =
                                  gunpackb(src, pos_ptr, CSN_DWNLK_TS_ALLOC_LEN);

          /*  Next field is optional */
          /*  Clear option flag before checking if optional field present */
          msg_ptr->freq_param_flag = FALSE;

          if (gunpackb(src, pos_ptr, (int16)1))
          {
            /*  Frequency Parameters present */
            return_status = csn_frequency_params_ie(src, pos_ptr, &(msg_ptr->freq_params), gas_id);

            if (return_status != CSN_DECODE_NO_ERROR)
            {
              return return_status;
            }

            /*  Set option flag before checking if optional field present */
            msg_ptr->freq_param_flag = TRUE;
          }

          /*  Next field is conditional */

          if (!gunpackb(src, pos_ptr, (int16)1))
          {
            /*  Dynamic allocation struct present, last parameter indicates */
            /*  the presents of the uplink_tfi_assignment field in the */
            /*  dynamic allocation structure. */
            csn_dynamic_alloc_extract(src, pos_ptr,
                                                &(msg_ptr->dynamic_alloc), FALSE);

            /*  Set option flag before checking if optional field present */
            msg_ptr->alloc_flag = dynamic;
          }
          else
          {
            /*  Fixed allocation struct present - not supported */
            return NON_DISTRIBUTION_ERROR;
          }

        }
        else
        {
          /*  A Message Escape error has occurred */
          msg_ptr->egprs_ptr = FALSE;
          return_status = MESSAGE_ESCAPE_ERROR;
        }

     }

  }
  else
  {
    /*  Address error has occurred. */
    return_status = ADDRESS_INFO_ERROR;
  }

  return(return_status);

}


/*===========================================================================
===
===  FUNCTION     csn_pkt_tbf_release
===
===  DESCRIPTION
===
===    This function extracts the information from the PACKET TBF RELEASE
===    message.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/

static csn_decoder_error_t csn_pkt_tbf_release(
  csn_decode_memory_instance_t *mem,
  uint8 src[], uint16 *pos_ptr,
  packet_tbf_release_t *msg_ptr,
  const gas_id_t gas_id
)
{
  csn_decoder_error_t return_status = CSN_DECODE_NO_ERROR;

  CSN_DECODE_MEMORY_IN_USE(mem);

  /*  First bit should be 0, if not an address error has occurred */
  if (!(gunpackb(src, pos_ptr, (int16)1)))
  {
    /*  Extract global tfi */
    csn_global_tfi_ie(src, pos_ptr, &(msg_ptr->global_tfi));

    /*  Extract uplink release */
    msg_ptr->uplink_release = gunpackb(src, pos_ptr, CSN_UPLINK_RELEASE_LEN);

    /*  Extract downlink release */
    msg_ptr->dwnlink_release =
                            gunpackb(src, pos_ptr, CSN_DWNLINK_RELEASE_LEN);

    /*  Extract tbf release cause */
    msg_ptr->tbf_release_cause =
                          gunpackb(src, pos_ptr, CSN_TBF_RELEASE_CAUSE_LEN);
  }
  else
  {
    /*  Address error has occurred. */
    return_status = ADDRESS_INFO_ERROR;
  }

  return(return_status);

}



/*===========================================================================
===
===  FUNCTION     csn_pkt_uplk_ack_nack
===
===  DESCRIPTION
===
===    This function extracts the information from the PACKET TBF RELEASE
===    message.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/

static csn_decoder_error_t csn_pkt_uplk_ack_nack(
  csn_decode_memory_instance_t *mem,
  uint8 src[], uint16 *pos_ptr,
  packet_uplink_ack_nack_t *msg_ptr,
  const gas_id_t gas_id
)
{
  csn_decoder_error_t return_status = CSN_DECODE_NO_ERROR;

  CSN_DECODE_MEMORY_IN_USE(mem);

  /*  PUAN does not carry EGPRS params */
  msg_ptr->egprs_puan = FALSE;

  /*  First two bits should be 0, if not an address error has occurred */
  if (!(gunpackb(src, pos_ptr, (int16)2)))
  {
    /*  Extract uplink tfi */
    msg_ptr->uplink_tfi = gunpackb(src, pos_ptr, CSN_UPLINK_TFI_LEN);

    /*  Check message escape bit */
    if (!(gunpackb(src, pos_ptr, (int16)1)))
    {
      /*  Extract channel coding command */
      msg_ptr->chan_coding_cmd = gunpackb(src,
                                          pos_ptr, CSN_CHAN_CODING_CMD_LEN);

      /*  Extract ack/nack description IE */
      csn_ack_nack_desc_ie(src, pos_ptr, &(msg_ptr->ack_nack_desc));

      /*  Next field is optional */
      /*  Clear option flag before checking if optional field present */
      msg_ptr->contention_res_flag = FALSE;

      if (gunpackb(src, pos_ptr, (int16)1))
      {
        /*  Contention resolution tlli present */
        msg_ptr->contention_res_tlli = gunpackd(src,
                                      pos_ptr, CSN_CONTENTION_RES_TLLI_LEN);

        /*  Set option flag before checking if optional field present */
        msg_ptr->contention_res_flag = TRUE;
      }

      /*  Next field is optional */
      /*  Clear option flag before checking if optional field present */
      msg_ptr->pkt_timing_adv_flag = FALSE;

      if (gunpackb(src, pos_ptr, (int16)1))
      {
        /*  Packet timing advance ie present */
        csn_pkt_timing_advance_ie(src, pos_ptr, &(msg_ptr->pkt_timing_adv));

        /*  Set option flag before checking if optional field present */
        msg_ptr->pkt_timing_adv_flag = TRUE;
      }

      /*  Clear power control parameters present flag. */
      msg_ptr->pwr_ctrl_flag = FALSE;

      /*  Next field is an optional field */
      if (gunpackb(src, pos_ptr, (int16)1))
      {
        /*  Extract power control parameters */
        csn_pwr_cntrl_params_ie(src, pos_ptr, &(msg_ptr->pwr_ctrl_paramtrs));

        /*  Set power control parameters present flag. */
        msg_ptr->pwr_ctrl_flag = TRUE;
      }

      /*  Next field is optional */
      /*  Clear option flag before checking if optional field present */
      msg_ptr->extension_bits_flag = FALSE;

      if (gunpackb(src, pos_ptr, (int16)1))
      {
        /*  Packet timing advance ie present */
        csn_extension_bits_ie(src, pos_ptr, &(msg_ptr->extension_bits));

        /*  Set option flag before checking if optional field present */
        msg_ptr->extension_bits_flag = TRUE;
      }

      /*  Next field is optional */
      /*  Clear option flag before checking if optional field present */
      msg_ptr->fixed_alloc_flag = FALSE;

      if (gunpackb(src, pos_ptr, (int16)1))
      {
        /* Fixed Allocation not supported */
        return NON_DISTRIBUTION_ERROR;
      }

      /*  Clear option flag before checking if optional field present */
      msg_ptr->r99_additions_flag = FALSE;

      /*  Add Additions for R99 */
      if (gunpackb(src, pos_ptr, (int16)1))
      {
        /*  Clear option flag before checking if optional field present */
        msg_ptr->pkt_ext_ta_flag = FALSE;

        if (gunpackb(src, pos_ptr, (int16)1))
        {
          /*  Packet Extended Timing Advance present */
          msg_ptr->pkt_ext_ta = gunpackb(src, pos_ptr, CSN_PKT_EXT_TA_LEN);

          /* Set conditional flag */
          msg_ptr->pkt_ext_ta_flag = TRUE;
        }

        /*  Extract TBF_EST bit which will be present if R99 additionsi */
        /*  present. */
        msg_ptr->tbf_est = gunpackb(src, pos_ptr, CSN_TBF_EST_LEN);

        /* Set conditional flag */
        msg_ptr->r99_additions_flag = TRUE;

      }

    }
    else
    {

        /*EDGE PUAN PARAMS START HERE*/

        if (!(gunpackb(src, pos_ptr, (int16)2)))
        {
          msg_ptr->egprs_puan = TRUE;

          /*< EGPRS Channel Coding Command : < EGPRS Modulation and Coding IE >>*/
          msg_ptr->egprs_chan_coding_cmd = gunpackb(src, pos_ptr,4);

          /* < RESEGMENT : bit (1) >*/
          msg_ptr->resegment = gunpackb(src, pos_ptr,1);


          /*< PRE_EMPTIVE_TRANSMISSION : bit (1) >*/
          msg_ptr->pre_emptive_tran = gunpackb(src, pos_ptr, 1);

          /*< PRR RETRANSMISSION REQUEST : bit (1) >*/
          msg_ptr->prr_retran_req = gunpackb(src, pos_ptr, 1);

          /*< ARAC RETRANSMISSION REQUEST : bit (1) >*/
          msg_ptr->arac_retran_req = gunpackb(src, pos_ptr, 1);

          /*{ 0 | 1 < CONTENTION_RESOLUTION_TLLI : bit (32) > }*/
          if ((gunpackb(src, pos_ptr, (int16)1)))
          {
            /*  Extract CONTENTION_RESOLUTION_TLLI */
            msg_ptr->contention_res_tlli = gunpackd(src, pos_ptr, 32);
            msg_ptr->contention_res_flag = TRUE;
          }
          else
          {
            msg_ptr->contention_res_flag = FALSE;
          }

          /*<TBF_EST : bit (1)>*/
          msg_ptr->tbf_est = gunpackb(src, pos_ptr, 1);

          /*{ 0 | 1 < Packet Timing Advance : < Packet Timing Advance IE > > } */
          if (gunpackb(src, pos_ptr, (int16)1))
          {
            /*  Packet timing advance ie present */
            csn_pkt_timing_advance_ie(src, pos_ptr, &(msg_ptr->pkt_timing_adv));
            msg_ptr->pkt_timing_adv_flag = TRUE;
          }
          else
          {
            msg_ptr->pkt_timing_adv_flag = FALSE;
          }

          /*{ 0 | 1 <Packet Extended Timing Advance : bit (2)> }               */
          if (gunpackb(src, pos_ptr, (int16)1))
          {
            /*  Packet Extended Timing Advance present */
            msg_ptr->pkt_ext_ta = gunpackb(src, pos_ptr, 2);

            /* Set conditional flag */
            msg_ptr->pkt_ext_ta_flag = TRUE;
          }
          else
          {
             msg_ptr->pkt_ext_ta_flag = FALSE;
          }

          /*{ 0 | 1 < Power Control Parameters : < Power Control Parameters IE > > }  */
          if (gunpackb(src, pos_ptr, (int16)1))
          {
            /*  Extract power control parameters */
            csn_pwr_cntrl_params_ie(src, pos_ptr, &(msg_ptr->pwr_ctrl_paramtrs));

            /*  Set power control parameters present flag. */
            msg_ptr->pwr_ctrl_flag = TRUE;
          }
          else
          {
            msg_ptr->pwr_ctrl_flag = FALSE;
          }

          /*{ 0 | 1 < Extension Bits : Extension Bits IE > } -- sub-clause 12.26   */
          if (gunpackb(src, pos_ptr, (int16)1))
          {
            /*  Packet timing advance ie present */
            csn_extension_bits_ie(src, pos_ptr, &(msg_ptr->extension_bits));

            /*  Set option flag before checking if optional field present */
            msg_ptr->extension_bits_flag = TRUE;
          }
          else
          {
            msg_ptr->extension_bits_flag = FALSE;
          }

          /* < EGPRS Ack/Nack Description : < EGPRS Ack/Nack Description IE > > */
          csn_egprs_puan_desc_ie(src, pos_ptr, &(msg_ptr->egprs_ack_nack_des));
          if(*pos_ptr < (dl_control_msg_size * 8))
          {
            /*{ 0 | 1 < Fixed Allocation Parameters : < Fixed Allocation struct > > } }*/
            if (gunpackb(src, pos_ptr, (int16)1))
            {
              /* Fixed Allocation not supported */
              return NON_DISTRIBUTION_ERROR;
            }
            else
            {
              msg_ptr->fixed_alloc_flag = FALSE;
            }
          }
          else
          {
            msg_ptr->fixed_alloc_flag = FALSE;
          }
        }
        else
        {
           /*EDGE Message Escape error has occurred */
           return_status = MESSAGE_ESCAPE_ERROR;
        }

     }

  }
  else
  {
    /*  Address error has occurred. */
    return_status = ADDRESS_INFO_ERROR;
  }

  return(return_status);
}



/*===========================================================================
===
===  FUNCTION    csn_pkt_uplk_ass
===
===  DESCRIPTION
===
===    This function extracts the information from the PACKET UPLINK ASSIGNMENT
===    message.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/

static csn_decoder_error_t csn_pkt_uplk_ass(
  csn_decode_memory_instance_t *mem,
  uint8 src[], uint16 *pos_ptr,
  packet_uplink_assignment_t *msg_ptr,
  const gas_id_t gas_id
)
{
  csn_decoder_error_t return_status = CSN_DECODE_NO_ERROR;
  int8  persistence_count=0;

  CSN_DECODE_MEMORY_IN_USE(mem);

  /*  PUA does not carry EGPRS params */
  msg_ptr->egprs_pua = FALSE;

  /*  Clear flag before checking if persistence levels present. */
  msg_ptr->persistence_flag = FALSE;

  /* This information field is optional */
  if (gunpackb(src, pos_ptr, (int16)1))
  {
    /* Persistance Level present */
    while (persistence_count < MAX_PERSIST_VALUES)
    {
      /*  Persistence levels present. */
      msg_ptr->persistence[persistence_count++] =
                                gunpackb(src, pos_ptr, CSN_PERSISTENCE_LEN);
    };

    /*  Set flag to indicate persistence levels present. */
    msg_ptr->persistence_flag = TRUE;
  }

  /*  Next field is an address field, check which type */
  if (!(gunpackb(src, pos_ptr, (int16)1)))
  {
    /*  Extract global tfi */
    csn_global_tfi_ie(src, pos_ptr, &(msg_ptr->global_tfi));

    /*  Set option enum to indicate that the global tfi is TRUE */
    msg_ptr->tlli_gtfi_pkt_ref_valid = global_tfi_valid;
  }
  else
  {
    /*  Check next bit */
    if (!(gunpackb(src, pos_ptr, (int16)1)))
    {
      /*  Extract TLLI */
      msg_ptr->tlli = gunpackd(src, pos_ptr, CSN_TLLI_LEN);

      /*  Set option enum to indicate that the tlli is TRUE */
      msg_ptr->tlli_gtfi_pkt_ref_valid = tlli_valid;
    }
    else
    {
      /*  Check next bit */
      if (!(gunpackb(src, pos_ptr, (int16)1)))
      {
        /*  Extract TQI */
        msg_ptr->tqi = gunpackw(src, pos_ptr, CSN_TQI_LEN);

        /*  Set option enum to indicate that tqi is TRUE */
        msg_ptr->tlli_gtfi_pkt_ref_valid = tqi_valid;
      }
      else
      {
        return ADDRESS_INFO_ERROR;
      }
    }
  }

  /*  Check message escape bit */
  if (!(gunpackb(src, pos_ptr, (int16)1)))
  {
    /*  Extract channel coding command */
    msg_ptr->chan_coding_cmd = gunpackb(src,
                                          pos_ptr, CSN_CHAN_CODING_CMD_LEN);

    /*  Extract tlli block channel coding */
    msg_ptr->tlli_blk_chan_coding = gunpackb(src,
                                      pos_ptr, CSN_TLLI_BLK_CHAN_CODING_LEN);

    /*  Extract Packet timing advance ie */
    csn_pkt_timing_advance_ie(src, pos_ptr, &(msg_ptr->pkt_timing_adv));

    /*  Clear frequency parameters present flag. */
    msg_ptr->freq_param_flag = FALSE;

    /*  Next field is an optional field */
    if (gunpackb(src, pos_ptr, (int16)1))
    {
      /*  Frequency parameters are present */
      return_status = csn_frequency_params_ie(src, pos_ptr, &(msg_ptr->freq_params), gas_id);

      if (return_status != CSN_DECODE_NO_ERROR)
      {
        return return_status;
      }

      /*  Set frequency parameters present flag. */
      msg_ptr->freq_param_flag = TRUE;
    }

    /*  The following fields are conditional */
    switch (gunpackb(src, pos_ptr, (int16)2))
    {
      case EXTENSION:
        /*  Set allocation type flag */
        msg_ptr->allocation_flag = extension_alloc;

      break;

      case DYNAMIC_ALLOC:
        /*  Dynamic allocation structure present */
        csn_dynamic_alloc_extract(src,
                                    pos_ptr, &(msg_ptr->dynamic_alloc), TRUE);

        /*  Set allocation type flag */
        msg_ptr->allocation_flag = dynamic_alloc_e;

      break;

      case SINGLE_BLK_ALLOC:
        /*  Single block allocation structure present */
        csn_sngl_blk_alloc_extract(src, pos_ptr, &(msg_ptr->sngl_blk_alloc));

        /*  Set allocation type flag */
        msg_ptr->allocation_flag = single_blk_alloc;

      break;

      case FIXED_ALLOC:
        /* See 3GPP 44.018 Table 10.5.2.25c.1 */
        /* allocated in an earlier version of the protocol and shall not be used */
        return NON_DISTRIBUTION_ERROR;

      default:
      break;
    }

    /*  Clear option flag before checking if optional field present */
    msg_ptr->r99_additions_flag = FALSE;

    /*  Add Additions for R99 */
    if (gunpackb(src, pos_ptr, (int16)1))
    {
      /*  Clear option flag before checking if optional field present */
      msg_ptr->pkt_ext_ta_flag = FALSE;

      if (gunpackb(src, pos_ptr, (int16)1))
      {
        /*  Packet Extended Timing Advance present */
        msg_ptr->pkt_ext_ta = gunpackb(src, pos_ptr, CSN_PKT_EXT_TA_LEN);

        /* Set conditional flag */
        msg_ptr->pkt_ext_ta_flag = TRUE;
      }

      /* Set conditional flag */
      msg_ptr->r99_additions_flag = TRUE;
    }
  }
  else
  {
      /*EDGE PARAMS START HERE*/
      if (!(gunpackb(src, pos_ptr, (int16)2)))
      {
        msg_ptr->egprs_pua = TRUE;

        /* { 0 | 1 <CONTENTION_RESOLUTION_TLLI : bit(32) > }  */
        /*  Check next bit */
        if ((gunpackb(src, pos_ptr, (int16)1)))
        {
          /*  Extract CONTENTION_RESOLUTION_TLLI */
          msg_ptr->cont_res_tlli = gunpackd(src, pos_ptr, CSN_TLLI_LEN);
          msg_ptr->cont_res_tlli_flag = TRUE;
        }
        else
        {
          msg_ptr->cont_res_tlli_flag = FALSE;
        }
        /* { 0 | 1 < COMPACT reduced MA : < COMPACT reduced MA IE >> }  */
        /*  Check next bit */
        if ((gunpackb(src, pos_ptr, (int16)1)))
        {
            /*  Extract COMPACT reduced MA */
            csn_compact_reduced_ma_ie(src, pos_ptr, &(msg_ptr->compact_reduced_ma));
            msg_ptr->compact_reduced_ma_flag = TRUE;
        }
        else
        {
            msg_ptr->compact_reduced_ma_flag = FALSE;
        }

        /*/ < EGPRS Channel Coding Command : < EGPRS Modulation and Coding IE >>  */
        msg_ptr->egprs_chan_coding_cmd =
          gunpackb(src, pos_ptr, CSN_EGPRS_CHANNEL_CODING_COMMAND_LEN);

        /* < RESEGMENT : bit (1) >*/
        msg_ptr->resegment = gunpackb(src, pos_ptr, CSN_RESEGMENT_LEN);

        /* < EGPRS Window Size : < EGPRS Window Size IE > */
        msg_ptr->egprs_window_size = gunpackb(src, pos_ptr, CSN_EGPRS_WINDOW_SIZE_LEN);

        /* { 0 | 1 < Access Technologies Request : Access Technologies Request struct >}*/
        /*  Check next bit */
        if ((gunpackb(src, pos_ptr, (int16)1)))
        {
            /*  Extract Access Technologies Request */
            memset(msg_ptr->access_technologies_req,0,MAX_ACCESS_TECH_TYPE);
            msg_ptr->access_technologies_req[0] = gunpackb(src, pos_ptr, CSN_ACCESS_TECHNOLOGIES_REQ_LEN);
            msg_ptr->access_tech_reg_length = 1;

            while (gunpackb(src, pos_ptr, (word) 1) && (msg_ptr->access_tech_reg_length < MAX_ACCESS_TECH_TYPE))
            {
              msg_ptr->access_technologies_req[msg_ptr->access_tech_reg_length] = gunpackb(src, pos_ptr, CSN_ACCESS_TECHNOLOGIES_REQ_LEN);
              msg_ptr->access_tech_reg_length++;
            }

            msg_ptr->access_technologies_req_flag = TRUE;
        }
        else
        {
            msg_ptr->access_technologies_req_flag = FALSE;
        }

        /* < ARAC RETRANSMISSION REQUEST : bit (1) >  */
        msg_ptr->arac_retran_req = gunpackb(src, pos_ptr, CSN_ARAC_RETRAN_REQ_LEN);

        /* < TLLI_BLOCK_CHANNEL_CODING : bit (1) >  */

        msg_ptr->tlli_blk_chan_coding = gunpackb(src,pos_ptr, CSN_TLLI_BLK_CHAN_CODING_LEN);

         /* { 0 | 1 < BEP_PERIOD2 : bit(4) > }    */
        /*  Check next bit */
        if ((gunpackb(src, pos_ptr, (int16)1)))
        {
            /*  Extract BEP_PERIOD2 */
            msg_ptr->bep_period2 = gunpackb(src,pos_ptr, CSN_BEP_PERIOD2_LEN);
            msg_ptr->bep_period2_flag = TRUE;
        }
        else
        {
            msg_ptr->bep_period2_flag = FALSE;
        }

       /* < Packet Timing Advance : < Packet Timing Advance IE > >    */
        csn_pkt_timing_advance_ie(src, pos_ptr, &(msg_ptr->pkt_timing_adv));

        /* { 0 | 1 <Packet Extended Timing Advance : bit (2)> }       */
        /*  Check next bit */
        if (gunpackb(src, pos_ptr, (int16)1))
        {
           /*  Packet Extended Timing Advance present */
           msg_ptr->pkt_ext_ta = gunpackb(src, pos_ptr, CSN_PKT_EXT_TA_LEN);
           msg_ptr->pkt_ext_ta_flag = TRUE;
        }
        else
        {
          msg_ptr->pkt_ext_ta_flag = FALSE;
        }

        /* { 0 | 1  < Frequency Parameters : < Frequency Parameters IE > > } */
        /*  Check next bit */
        if (gunpackb(src, pos_ptr, (int16)1))
        {
          /*  Frequency parameters are present */
          return_status = csn_frequency_params_ie(src, pos_ptr, &(msg_ptr->freq_params), gas_id);

          if (return_status != CSN_DECODE_NO_ERROR)
          {
            return return_status;
          }

          msg_ptr->freq_param_flag = TRUE;
        }
        else
        {
          msg_ptr->freq_param_flag = FALSE;
        }

        /*check the TBF allocation type */
        switch (gunpackb(src, pos_ptr, (int16)2))
        {
          case EXTENSION:
            /*  Set allocation type flag */
            msg_ptr->allocation_flag = extension_alloc;

          break;

          case DYNAMIC_ALLOC:
            /*  Dynamic allocation structure present */
          csn_dynamic_alloc_extract(src,pos_ptr, &(msg_ptr->dynamic_alloc), TRUE);

            /*  Set allocation type flag */
            msg_ptr->allocation_flag = dynamic_alloc_e;

          break;

          case MULTI_BLOCK_ALLOC:
            /*  Single block allocation structure present */
            csn_multi_blk_alloc_extract(src, pos_ptr, &(msg_ptr->multi_blk_alloc));

            /*  Set allocation type flag */
            msg_ptr->allocation_flag = multi_blk_allocation;

          break;

          case FIXED_ALLOC:
            /* See 3GPP 44.018 Table 10.5.2.25c.1 */
            /* allocated in an earlier version of the protocol and shall not be used */
            return NON_DISTRIBUTION_ERROR;

          default:
          break;
        }

      }
      else
      {
        /*EDGE Message Escape error has occurred */
        return_status = MESSAGE_ESCAPE_ERROR;
      }

  }

  return(return_status);
}


/*===========================================================================
===
===  FUNCTION    csn_pkt_psi13
===
===  DESCRIPTION
===
===    This function extracts the information from the PACKET SYSTEM
===    INFORMATION Type 13 message.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/

static csn_decoder_error_t csn_pkt_psi13(
  csn_decode_memory_instance_t *mem,
  uint8 src[], uint16 *pos_ptr,
  packet_psi13_t *msg_ptr,
  const gas_id_t gas_id
)
{
  csn_decoder_error_t return_status = CSN_DECODE_NO_ERROR;
  msg_ptr->lb_ms_txpwr_max_cch = INVALID_LB_MS_TXPWR_MAX_CCH;

  CSN_DECODE_MEMORY_IN_USE(mem);

  /*  Extract BCCH change mark */
  msg_ptr->bcch_chng_mrk = gunpackb(src, pos_ptr, CSN_BCCH_CHNG_MRK_LEN);

  /*  Extract SI Change Field */
  msg_ptr->si_chng_field = gunpackb(src, pos_ptr, CSN_SI_CHNG_FIELD_LEN);

  /*  Clear SI13 Change mark / GPRS Mobile Alloc flag before checking */
  msg_ptr->si13_cm_gprs_ma_flag = FALSE;

  /*  Next field is optional */
  if (gunpackb(src, pos_ptr, (int16)1))
  {
    /*  SI13 change mark present */
    msg_ptr->psi13_chng_mrk =
                            gunpackb(src, pos_ptr, CSN_PSI13_CHNG_MRK_LEN);

    /*  Next field is gprs mobile allocation */
    return_status = csn_gprs_mobile_alloc_ie( src, pos_ptr, &(msg_ptr->gprs_mobile_alloc));

    if (return_status != CSN_DECODE_NO_ERROR)
    {
      return return_status;
    }

    /*  Set SI13 Change mark / GPRS Mobile Alloc flag */
    msg_ptr->si13_cm_gprs_ma_flag = TRUE;
  }

  /*  Next field is conditional */
  if (!gunpackb(src, pos_ptr, (int16)1))
  {
    /*  PBCCH not present in cell */
    /*  Extract RAC bits */
    msg_ptr->rac = gunpackb(src, pos_ptr, CSN_RAC_LEN);

    /*  Extract SPGC_CCCH_SUP bit */
    msg_ptr->spgc_ccch_sup =
                             gunpackb(src, pos_ptr, CSN_SPGC_CCCH_SUP_LEN);

    /*  Extract Priority access thr bits */
    msg_ptr->priority_access_thr =
                       gunpackb(src, pos_ptr, CSN_PRIORITY_ACCESS_THR_LEN);

    /*  Extract the network control order */
    msg_ptr->network_ctrl_ordr =
                         gunpackb(src, pos_ptr, 2);

    /*  Extract GPRS cell options */
    csn_gprs_cell_options_ie(src, pos_ptr, &(msg_ptr->gprs_cell_options));

    /*  The remaining fields form the Power Control Parameters */
    /*  Extract alpha */
    msg_ptr->alpha = gunpackb(src, pos_ptr, CSN_ALPHA_LEN);

    /*  Extract t_avg_w */
    msg_ptr->t_avg_w = gunpackb(src, pos_ptr, CSN_T_AVG_W_LEN);

    /*  Extract t_avg_t */
    msg_ptr->t_avg_t = gunpackb(src, pos_ptr, CSN_T_AVG_T_LEN);

    /*  Extract pc_meas_chan */
    msg_ptr->pc_meas_chan = gunpackb(src, pos_ptr, CSN_PC_MEAS_CHAN_LEN);

    /*  Extract n_avg_i */
    msg_ptr->n_avg_i = gunpackb(src, pos_ptr, CSN_N_AVG_I_LEN);

    /*  Indicate PBCCH is not present */
    msg_ptr->pbcch_present_flag = FALSE;
  }
  else
  {
    /* PBCCH presence should never be advertised - ignore the message */
    return NON_DISTRIBUTION_ERROR;
  }

  /*  Clear flags before checking if optional fields are present */
  msg_ptr->r99_additions_flag  = FALSE;
  msg_ptr->r4_additions_flag   = FALSE;
  msg_ptr->r6_additions_flag   = FALSE;
  msg_ptr->si_status_ind       = 0;
  msg_ptr->lb_ms_txpwr_max_cch = INVALID_LB_MS_TXPWR_MAX_CCH;
  msg_ptr->si2n_support        = SI2n_NOT_SUPPORTED;

  /*  Additions for R99 */
  if (gunpackb(src, pos_ptr, (int16)1))
  {
    /*  Set optional flag */
    msg_ptr->r99_additions_flag = TRUE;

    /*  Extract the SGSN release bit */
    msg_ptr->sgsnr = gunpackb(src, pos_ptr, CSN_SGSNR_LEN);

    /*  Additions for R4 */
    if (gunpackb(src, pos_ptr, (int16)1))
    {
      /*  Set optional flag */
      msg_ptr->r4_additions_flag = TRUE;

      /*  Extract the SI_STATUS_IND bit */
      msg_ptr->si_status_ind = gunpackb(src, pos_ptr, 1);

      /*  Additions for R6 */
      if (gunpackb(src, pos_ptr, (int16)1))
      {
        /*  Set optional flag */
        msg_ptr->r6_additions_flag = TRUE;

        if (gunpackb(src, pos_ptr, (int16)1))
        {
          /* Extract the LB_MS_TXPWR_MAX_CCH value */
          msg_ptr->lb_ms_txpwr_max_cch = gunpackb(src, pos_ptr, CSN_LB_MS_TXPWR_MAX_CCH_LEN);
        }

        /* Extract the SI2n_SUPPORT value */
        msg_ptr->si2n_support = gunpackb(src, pos_ptr, CSN_SI2N_SUPPORT_LEN);
      }
    }
  }

  return(return_status);
}

/*===========================================================================
===
===  FUNCTION    csn_pkt_psi14
===
===  DESCRIPTION
===
===    This function extracts the information from the PACKET SYSTEM
===    INFORMATION Type 14 message.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/
static csn_decoder_error_t csn_pkt_psi14(
  csn_decode_memory_instance_t *mem,
  uint8 src[],
  uint16 *pos_ptr,
  packet_psi14_t *msg_ptr,
  const gas_id_t gas_id
)
{
  csn_decoder_error_t return_status = CSN_DECODE_NO_ERROR;

  CSN_DECODE_MEMORY_IN_USE(mem);

  msg_ptr->ccch_pbcch_flag = gunpackb(src, pos_ptr, 1);
  msg_ptr->dtm_additions_flag = FALSE;
  msg_ptr->r6_additions_flag  = FALSE;
  msg_ptr->ccch_or_pbcch.ccch_access_info.lb_ms_txpwr_max_cch = INVALID_LB_MS_TXPWR_MAX_CCH;

  if (msg_ptr->ccch_pbcch_flag == 0)
  {
    /* CCCH Access Information */

    /*  Extract BCCH change mark */
    msg_ptr->ccch_or_pbcch.ccch_access_info.bcch_chng_mrk = gunpackb(src, pos_ptr, CSN_BCCH_CHNG_MRK_LEN);

    /*  Clear SI13 Change mark / GPRS Mobile Alloc flag before checking */
    msg_ptr->ccch_or_pbcch.ccch_access_info.si13_cm_gprs_ma_flag = FALSE;

    /*  Next field is optional */
    if (gunpackb(src, pos_ptr, (int16)1))
    {
      /*  SI13 change mark present */
      msg_ptr->ccch_or_pbcch.ccch_access_info.si13_chng_mrk = gunpackb(src, pos_ptr, CSN_PSI13_CHNG_MRK_LEN);

      /*  Next field is gprs mobile allocation */
      return_status = csn_gprs_mobile_alloc_ie( src, pos_ptr, &(msg_ptr->ccch_or_pbcch.ccch_access_info.gprs_mobile_alloc));

      if (return_status != CSN_DECODE_NO_ERROR)
      {
        return return_status;
      }

      /*  Set SI13 Change mark / GPRS Mobile Alloc flag */
      msg_ptr->ccch_or_pbcch.ccch_access_info.si13_cm_gprs_ma_flag = TRUE;
    }

    /*  Extract SPGC_CCCH_SUP bit */
    msg_ptr->ccch_or_pbcch.ccch_access_info.spgc_ccch_sup = gunpackb(src, pos_ptr, CSN_SPGC_CCCH_SUP_LEN);

    /*  Extract Priority access thr bits */
    msg_ptr->ccch_or_pbcch.ccch_access_info.priority_access_thr = gunpackb(src, pos_ptr, CSN_PRIORITY_ACCESS_THR_LEN);

    /*  Extract the network control order */
    msg_ptr->ccch_or_pbcch.ccch_access_info.network_ctrl_ordr = gunpackb(src, pos_ptr, 2);

    /*  Extract GPRS cell options */
    csn_gprs_cell_options_ie(src, pos_ptr, &(msg_ptr->ccch_or_pbcch.ccch_access_info.gprs_cell_options));

    /*  The remaining fields form the Power Control Parameters */
    /*  Extract alpha */
    msg_ptr->ccch_or_pbcch.ccch_access_info.alpha = gunpackb(src, pos_ptr, CSN_ALPHA_LEN);

    /*  Extract t_avg_w */
    msg_ptr->ccch_or_pbcch.ccch_access_info.t_avg_w = gunpackb(src, pos_ptr, CSN_T_AVG_W_LEN);

    /*  Extract t_avg_t */
    msg_ptr->ccch_or_pbcch.ccch_access_info.t_avg_t = gunpackb(src, pos_ptr, CSN_T_AVG_T_LEN);

    /*  Extract pc_meas_chan */
    msg_ptr->ccch_or_pbcch.ccch_access_info.pc_meas_chan = gunpackb(src, pos_ptr, CSN_PC_MEAS_CHAN_LEN);

    /*  Extract n_avg_i */
    msg_ptr->ccch_or_pbcch.ccch_access_info.n_avg_i = gunpackb(src, pos_ptr, CSN_N_AVG_I_LEN);

    /* SGSNR bit */
    msg_ptr->ccch_or_pbcch.ccch_access_info.sgsnr = gunpackb(src, pos_ptr, CSN_SGSNR_LEN);

    /* check for DTM extension bits */
    if (gunpackb(src, pos_ptr, 1) == 1)
    {
      if (gunpackb(src, pos_ptr, 1) == 0)
      {
        msg_ptr->dtm_additions_flag = TRUE;
        msg_ptr->ccch_or_pbcch.ccch_access_info.rac = gunpackb(src, pos_ptr, CSN_RAC_LEN);
        msg_ptr->ccch_or_pbcch.ccch_access_info.si_status_ind = gunpackb(src, pos_ptr, 1);

        /* check for Rel6 extension bits */
        if (gunpackb(src, pos_ptr, 1) == 1)
        {
          msg_ptr->r6_additions_flag = TRUE;
          if (gunpackb(src, pos_ptr, 1) == 1)
          {
            msg_ptr->ccch_or_pbcch.ccch_access_info.lb_ms_txpwr_max_cch = gunpackb(src, pos_ptr, CSN_LB_MS_TXPWR_MAX_CCH_LEN);
          }
          else
          {
            msg_ptr->ccch_or_pbcch.ccch_access_info.lb_ms_txpwr_max_cch = INVALID_LB_MS_TXPWR_MAX_CCH;
          }
          msg_ptr->ccch_or_pbcch.ccch_access_info.si2n_support = gunpackb(src, pos_ptr, CSN_SI2N_SUPPORT_LEN);
        }
      }
    }
  }
  else
  {
    /* PBCCH presence should never be advertised - ignore the message */
    return NON_DISTRIBUTION_ERROR;
  }

  return(return_status);
}


/*****************************************************************************
***
***     Message Extract Sub-functions to follow
***
*****************************************************************************/


/*===========================================================================
===
===  FUNCTION      csn_reject_struct_extract
===
===  DESCRIPTION
===
===    This function extracts the information from the PACKET ACCESS REJECT
===    message.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/

static csn_decoder_error_t csn_reject_struct_extract(

  uint8 src[], uint16 *pos_ptr,
  const packet_access_reject_t *msg_ptr
  )
{
  csn_decoder_error_t return_status = CSN_DECODE_NO_ERROR;

  /* Reject Struct */
  /*  First bit determines presence of TLLI or Packet Request reference */

  /*  Check if address field is TLLI */
  if (!(gunpackb(src, pos_ptr, (int16)1)))
  {
    /*  Extract TLLI */
    msg_ptr->rej_ptr->tlli = gunpackd(src, pos_ptr, CSN_TLLI_LEN);

    /*  Set option enum to indicate that the tlli is TRUE */
    msg_ptr->rej_ptr->tlli_gtfi_pkt_ref_valid = tlli_valid;
  }
  else
  {
    /*  Could be either packet random request or global tfi */
    /*  Check if address field is packet reference */
    if (!(gunpackb(src, pos_ptr, (int16)1)))
    {
      // A Request Ref is not expected, as PBCCH is not supported so PRACHing would not have occurred
      return ADDRESS_INFO_ERROR;
    }
    else
    {
      /*  Extract global tfi */
      csn_global_tfi_ie(src, pos_ptr, &(msg_ptr->rej_ptr->global_tfi));

      /*  Set option enum to indicate that the global tfi is TRUE */
      msg_ptr->rej_ptr->tlli_gtfi_pkt_ref_valid = global_tfi_valid;
    }
  }

  /*  Clear wait_valid_flag prior to testing */
  msg_ptr->rej_ptr->wait_valid = wait_ind_not_valid;

  /*  Check if wait_indication present else end. */
  if (gunpackb(src, pos_ptr, (int16)1))
  {
    /*  Extract wait_indication information */
    msg_ptr->rej_ptr->wait_indication =
    gunpackb(src, pos_ptr, CSN_WAIT_IND_LEN);

    /*  Extract wait_indication size information */
    msg_ptr->rej_ptr->wait_indication_size =
    gunpackb(src, pos_ptr, CSN_WAIT_IND_SZ_LEN);

    /*  Set flag to indicate presence of wait_indication */
    msg_ptr->rej_ptr->wait_valid = wait_ind_valid;
  }

  return(return_status);

}


/*===========================================================================
===
===  FUNCTION      csn_cell_sel_params_extract
===
===  DESCRIPTION
===
===    This function extracts the information from the cell parameters
===    struct which is part of the Packet Cell change order message.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/

static csn_decoder_error_t csn_cell_sel_params_extract(
  uint8 src[], uint16 *pos_ptr,
  cell_sel_params_t *msg_ptr
  )
{
  /*  First field cell_bar_access2 */
  msg_ptr->cell_bar_access2 =
                            gunpackb(src, pos_ptr, CSN_CELL_BAR_ACCESS_2_LEN);

  /*  Next field exc_acc */
  msg_ptr->exc_acc = gunpackb(src, pos_ptr, CSN_EXC_ACC_LEN);

  /*  Next field same_ra_as_serv_cell */
  msg_ptr->same_ra_as_serv_cell = gunpackb(src, pos_ptr, CSN_SAME_RA_LEN);

  /*  Reset option_bitmap before use */
  msg_ptr->option_bitmap = 0;

  /*  Next fields are optional */
  if (gunpackb(src, pos_ptr, (int16)1))
  {
    /*  Rxlev and ms maxpwr present. */
    msg_ptr->gprs_rxlev_access_min =
                                gunpackb(src, pos_ptr, CSN_RXLEV_ACCESS_LEN);

    msg_ptr->gprs_ms_txpwr_max_cch =
                                gunpackb(src, pos_ptr, CSN_MS_TXPWR_MAX_LEN);

    /*  Set appropriate bit in option_bitmap to indicate TRUE data */
    msg_ptr->option_bitmap |= GPRS_RXLEV_VALID;
  }

  /*  Next fields are optional */
  if (gunpackb(src, pos_ptr, (int16)1))
  {
    /*  gprs temp offset and gprs penalty time present */
    msg_ptr->gprs_temp_offset =
                            gunpackb(src, pos_ptr, CSN_GPRS_TEMP_OFFSET_LEN);

    msg_ptr->gprs_penalty_time =
                          gunpackb(src, pos_ptr, CSN_GPRS_PENALTY_TIME_LEN);

    /*  Set appropriate bit in option_bitmap to indicate TRUE data */
    msg_ptr->option_bitmap |= GPRS_TEMP_OFFSET_VALID;
  }

  /*  Next fields are optional */
  if (gunpackb(src, pos_ptr, (int16)1))
  {
    /*  gprs reselect offset field present */
    msg_ptr->gprs_reselect_offset =
                        gunpackb(src, pos_ptr, CSN_GPRS_RESELECT_OFFSET_LEN);

    /*  Set appropriate bit in option_bitmap to indicate TRUE data */
    msg_ptr->option_bitmap |= GPRS_RESELECT_OFFEST_VALID;
  }

  /*  Next fields are optional */
  if (gunpackb(src, pos_ptr, (int16)1))
  {
    /*  hcs struct present */
    msg_ptr->hcs_params.priority_class =
                              gunpackb(src, pos_ptr, CSN_PRIORITY_CLASS_LEN);

    msg_ptr->hcs_params.hcs_thr = gunpackb(src, pos_ptr, CSN_HCS_THR_LEN);

    /*  Set appropriate bit in option_bitmap to indicate TRUE data */
    msg_ptr->option_bitmap |= HCS_VALID;
  }

  /*  Next fields are optional */
  if (gunpackb(src, pos_ptr, (int16)1))
  {
    /*  si13 pbcch location struct present */
    /*  Next fields are optional */
    if (!(gunpackb(src, pos_ptr, (int16)1)))
    {
      /*  si13 pbcch_loc field present */
      msg_ptr->si13_pbcch_loc.si13_location =
                                    gunpackb(src, pos_ptr, CSN_SI13_LOC_LEN);

      /*  Set flag to determine pbcch/si13 present */
      msg_ptr->si13_pbcch_loc.si13_loc_flag = si13_loc_valid;
    }
    else
    {
      /* PBCCH presence should never be advertised - ignore the message */
      return NON_DISTRIBUTION_ERROR;
    }

    /*  Set appropriate bit in option_bitmap to indicate si13 data TRUE */
    msg_ptr->option_bitmap |= SI13_PBCCH_VALID;
  }

  return CSN_DECODE_NO_ERROR;
}


/*===========================================================================
===
===  FUNCTION      csn_repeated_page_extract
===
===  DESCRIPTION
===
===    This function extracts the information from the repeated page struct.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/

static void csn_repeated_page_extract(
  uint8 src[], uint16 *pos_ptr,
  repeated_page_t *msg_ptr
  )
{
  uint8 mi_index;

  /*  First bit determines if TBF or RR establishment */
  if (!(gunpackb(src, pos_ptr, (int16)1)))
  {
    /*  Page request for TBF */
    /*  Determine if ptmsi or mobile identity present */
    if (!(gunpackb(src, pos_ptr, (int16)1)))
    {
      /*  Extract ptmsi */
      msg_ptr->ptmsi = gunpackd(src, pos_ptr, CSN_PTMSI_LEN);

      /*  Set ptmsi_tmsi_mi_flag to ptmsi */
      msg_ptr->ptmsi_tmsi_mi_flag = ptmsi_valid;
    }
    else
    {
      /*  Extract the length of the mobile identity */
      msg_ptr->length_of_mi = gunpackb(src, pos_ptr, CSN_LENGTH_OF_MI_LEN);

      /*  Now extract the appropriate number of mobile identity octets */
      for(mi_index = 0; mi_index < msg_ptr->length_of_mi; mi_index++)
        msg_ptr->mobile_identity[mi_index] =
                            gunpackb(src, pos_ptr, CSN_MOBILE_IDENTITY_LEN);
      /* Fill in the unused bytes with 0xff */
      for(; mi_index < REPEATED_PAGE_MOBILE_IDENTITY_SIZE; mi_index++)
        msg_ptr->mobile_identity[mi_index] = 0xff;

      /*  Set ptmsi_tmsi_mi_flag to mi */
      msg_ptr->ptmsi_tmsi_mi_flag = ptmsi_mi_valid;
    }

    /*  Set establishment flag to TBF establishment */
    msg_ptr->establishment = tbf_establishment;

  }
  else
  {
    /*  Page request for RR */
    /*  Determine if tmsi or mobile identity present */
    if (!(gunpackb(src, pos_ptr, (int16)1)))
    {
      /*  Extract tmsi */
      msg_ptr->tmsi = gunpackd(src, pos_ptr, CSN_TMSI_LEN);

      /*  Set ptmsi_tmsi_mi_flag to tmsi */
      msg_ptr->ptmsi_tmsi_mi_flag = tmsi_valid;
    }
    else
    {
      /*  Extract the length of the mobile identity */
      msg_ptr->length_of_mi = gunpackb(src, pos_ptr, CSN_LENGTH_OF_MI_LEN);

      /*  Now extract the appropriate number of mobile identity octets */
      for(mi_index = 0; mi_index < msg_ptr->length_of_mi; mi_index++)
        msg_ptr->mobile_identity[mi_index] =
                            gunpackb(src, pos_ptr, CSN_MOBILE_IDENTITY_LEN);
      /* Fill in the unused bytes with 0xff */
      for(; mi_index < REPEATED_PAGE_MOBILE_IDENTITY_SIZE; mi_index++)
        msg_ptr->mobile_identity[mi_index] = 0xff;

      /*  Set ptmsi_tmsi_mi_flag to mi */
      msg_ptr->ptmsi_tmsi_mi_flag = tmsi_mi_valid;
    }

    /*  Extract channel_needed */
    msg_ptr->channel_needed = gunpackb(src, pos_ptr, CSN_CHANNEL_NEEDED_LEN);

    /*  Clear emlpp_priority flag */
    msg_ptr->emlpp_flag = FALSE;

    /*  See if emlpp_priority present and if so extract */
    if (gunpackb(src, pos_ptr, (int16)1))
    {
      /*  emlpp_priority present */
      msg_ptr->emlpp_priority =
                              gunpackb(src, pos_ptr, CSN_EMLPP_PRIORITY_LEN);

      /*  Set emlpp_priority_valid */
      msg_ptr->emlpp_flag = TRUE;
    }

    /*  Set establishment flag to RR establishment */
    msg_ptr->establishment = rr_establishment;
  }

}


/*===========================================================================
===
===  FUNCTION      csn_dynamic_alloc_extract
===
===  DESCRIPTION
===
===    This function extracts the information from the dynamic allocation
===    struct
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/

static void csn_dynamic_alloc_extract(
  uint8 src[], uint16 *pos_ptr,
  dynamic_alloc_t *msg_ptr,
  uint8 uplk_tfi_ass_present
  )
{
  uint8   power_flag;
  uint8   *usf_ptr;
  uint8   *gamma_ptr;
  uint8   loop_count;
  uint8   flag_value=1;

  /*  Extract the extended dynamic allocation bit */
  msg_ptr->ext_dynamic_alloc = gunpackb(src,
                                        pos_ptr, CSN_EXT_DYNAMIC_ALLOC_LEN);

  /*  Next field is optional */
  /*  Clear option flag before checking if optional field present */
  msg_ptr->po_prmode_flag = FALSE;

  if (gunpackb(src, pos_ptr, (int16)1))
  {
    /*  po bits present */
    msg_ptr->po = gunpackb(src, pos_ptr, CSN_PO_LEN);

    /*  pr_mode bit present */
    msg_ptr->pr_mode = gunpackb(src, pos_ptr, CSN_PR_MODE_LEN);

    /*  Set option flag before checking if optional field present */
    msg_ptr->po_prmode_flag = TRUE;
  }

  /*  Extract usf granularity bit */
  msg_ptr->usf_granularity = gunpackb(src, pos_ptr, CSN_USF_GRANULARITY_LEN);

  /*  Depending on which message the dynamic allocation structure is in, */
  /*  the uplink_tfi_assignment field may not be expected. The parameter */
  /*  uplk_tfi_ass_present determines if this field should be expected or not */
  if (uplk_tfi_ass_present)
  {
    /*  Uplink tfi assignment field may be present */
    /*  Next field is optional */
    /*  Clear option flag before checking if optional field present */
    msg_ptr->uplk_tfi_assignment_flag = FALSE;

    if (gunpackb(src, pos_ptr, (int16)1))
    {
      /*  uplink tfi assignment present */
      msg_ptr->uplk_tfi_assignment = gunpackb(src,
                                      pos_ptr, CSN_UPLINK_TFI_ASSIGNMENT_LEN);

      /*  Set option flag */
      msg_ptr->uplk_tfi_assignment_flag = TRUE;
    }
  }

  /*  Next field is optional */
  /*  Clear option flag before checking if optional field present */
  msg_ptr->rlc_data_blks_granted_flag = FALSE;

  if (gunpackb(src, pos_ptr, (int16)1))
  {
    /*  rlc data blocks granted bits present */
    msg_ptr->rlc_data_blks_granted = gunpackb(src,
                                    pos_ptr, CSN_RLC_DATA_BLKS_GRANTED_LEN);

    /*  Set option flag */
    msg_ptr->rlc_data_blks_granted_flag = TRUE;
  }

  /*  Next field is optional */
  /*  Clear option flag before checking if optional field present */
  msg_ptr->tbf_start_time_flag = FALSE;

  if (gunpackb(src, pos_ptr, (int16)1))
  {
    /*  tbf starting time present */
    csn_start_frm_num_ie(src, pos_ptr, &(msg_ptr->tbf_start_frm_num));

    /*  Set option flag */
    msg_ptr->tbf_start_time_flag = TRUE;
  }

  /*  Next field is conditional */
  if (!gunpackb(src, pos_ptr, (int16)1))
  {
    /*  USF Timeslot allocation present. */
    /*  Set power flag */
    power_flag = FALSE;
  }
  else
  {
    /*  Timeslot allocation with power control present. */
    /*  Set power flag */
    power_flag = TRUE;
  }

  /*  Based on the power flag extract the appropriate data */
  /*  Setup usf pointer */
  usf_ptr = &(msg_ptr->usf_tn0);

  /*  Clear usf bitmap */
  msg_ptr->usf_bitmap = 0;

  /*  Setup gamma pointer */
  gamma_ptr = &(msg_ptr->gamma_tn0);

  /*  Clear gamma bitmap */
  msg_ptr->gamma_bitmap = 0;

  /*  If power flag is set extract alpha value */
  if (power_flag)
  {
    msg_ptr->alpha = gunpackb(src, pos_ptr, CSN_ALPHA_LEN);
  }

  /*  Execute usf and possible gamma extracts using a loop */
  for(loop_count=0;loop_count<8;loop_count++)
  {
    /*  Next field is optional */
    if (gunpackb(src, pos_ptr, (int16)1))
    {
      /*  Extract usf value */
      *usf_ptr = gunpackb(src, pos_ptr, CSN_USF_LEN);

      /*  Update usf bitmap */
      msg_ptr->usf_bitmap |= flag_value;

      if (power_flag)
      {
        /*  Extract gamma value */
        *gamma_ptr = gunpackb(src, pos_ptr, CSN_GAMMA_LEN);

        /*  Update gamma bitmap */
        msg_ptr->gamma_bitmap |= flag_value;
      }

    }

    /*  Update usf pointer and usf_flag_value */
    usf_ptr++;
    flag_value <<= 1;

    if(power_flag)
      gamma_ptr++;
  }

  if(power_flag)
  {
    /*  Set flag to indicate usf and power control */
    msg_ptr->timeslot_alloc_flag = usf_and_power_control;
  }
  else
  {
    /*  Set flag to indicate usf only */
    msg_ptr->timeslot_alloc_flag = usf_only;
  }

}


/*===========================================================================
===
===  FUNCTION      csn_sngl_blk_alloc_extract
===
===  DESCRIPTION
===
===    This function extracts the information from the dynamic allocation
===    struct
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/

static void csn_sngl_blk_alloc_extract(
  uint8 src[], uint16 *pos_ptr,
  sngl_blk_alloc_t *msg_ptr
  )
{
  /*  Extract timeslot number */
  msg_ptr->timeslot_num = gunpackb(src, pos_ptr, CSN_TIMESLOT_NUMBER_LEN);

  /*  Next field is optional */
  /*  Clear option flag before checking if optional field present */
  msg_ptr->alpha_gamma_flag = FALSE;

  if (gunpackb(src, pos_ptr, (int16)1))
  {
    /*  alpha bits present */
    msg_ptr->alpha = gunpackb(src, pos_ptr, CSN_ALPHA_LEN);

    /*  Extract gamma_tn */
    msg_ptr->gamma_tn = gunpackb(src, pos_ptr, CSN_GAMMA_TN_LEN);

    /*  Set option flag */
    msg_ptr->alpha_gamma_flag = TRUE;
  }

  /*  Next field is optional */
  /*  Clear option flag before checking if optional field present */
  msg_ptr->po_prmode_flag = FALSE;

  if (gunpackb(src, pos_ptr, (int16)1))
  {
    /*  alpha bits present */
    msg_ptr->po = gunpackb(src, pos_ptr, CSN_PO_LEN);

    msg_ptr->bts_pwr_ctrl_mode =
                          gunpackb(src, pos_ptr, CSN_BTS_PWR_CTRL_MODE_LEN);

    /*  pr_mode bit present */
    msg_ptr->pr_mode = gunpackb(src, pos_ptr, CSN_PR_MODE_LEN);

    /*  Set option flag before checking if optional field present */
    msg_ptr->po_prmode_flag = TRUE;
  }

  /*  Extract tbf starting time */
  csn_start_frm_num_ie(src, pos_ptr, &(msg_ptr->tbf_start_frm_num));

}


/*===========================================================================
===
===  FUNCTION   csn_discard_lsa_information
===
===  DESCRIPTION
===
===    This function extracts the information from the LSA ID information struct.
===
===    Note that the decode is limited to length MAX_DECODE_BITS, which does not
===    account for segmented messages.  Currently this decoder function is only
===    used for PSI3 (non segmented msg).
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/
#ifdef PERLUTF
#error code not present
#else
static void csn_discard_lsa_information(uint8 src[], uint16 *pos_ptr)
#endif /*PERLUTF*/
{
  while (gunpackb(src, pos_ptr, 1))
  {
    if (gunpackb(src, pos_ptr, 1))
    {
      (void)gunpackw(src, pos_ptr, 10); /* discard the ShortLSA_ID */
    }
    else
    {
      (void)gunpackd(src, pos_ptr, 24); /* discard the LSA_ID */
    }

    if (*pos_ptr >= MAX_DECODE_BITS)
    {
      MSG_GERAN_ERROR_1("LSA Info error CSN msg, overrun msg pos_ptr: %d",*pos_ptr);
      break;
    }
  }
}

static void csn_discard_lsa_parameters(uint8 src[], uint16 *pos_ptr)
{
  uint8 nr_of_freq_or_cells = gunpackb(src, pos_ptr, 5);
  uint8 i;

  for (i = 0; i < nr_of_freq_or_cells; i++)
  {
    csn_discard_lsa_information(src, pos_ptr);
  }
}

static void csn_ccn_support_description_extract(uint8 src[], uint16 *pos_ptr, csn_ccn_support_desc_t *msg_ptr)
{
  uint8 bit_index = 0;
  uint8 bit_mask  = 1;
  uint8 octet     = 0;

  msg_ptr->number_cells = gunpackb(src, pos_ptr, CSN_NUMBER_CELLS_LEN);
  memset(msg_ptr->ccn_supported, 0, CCN_SUPPORT_BITMAP_LENGTH);

  for (bit_index = 0; bit_index < msg_ptr->number_cells; bit_index++)
  {
    if (gunpackb(src,pos_ptr,1))
    {
      /* set this bit to 1 */
      msg_ptr->ccn_supported[octet] |= bit_mask;
    }
    bit_mask <<= 1;
    if (bit_mask == 0)
    {
      bit_mask = 1;
      octet++;
    }
  }
}

/*****************************************************************************
***
***     12.xx - Information Element Extract functions to follow
***
*****************************************************************************/


/*****************************************************************************
***
***     12.03 - Information Element - Ack/Nack description
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION    csn_ack_nack_desc_ie
===
===  DESCRIPTION
===
===    This function extracts the information from the ACK/NACK description
===    IE.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/

static void csn_ack_nack_desc_ie(
  uint8 src[], uint16 *pos_ptr,
  ack_nack_desc_t *msg_ptr)
{
  int8  i;

  /*  Extract final ack indication */
  msg_ptr->final_ack_ind = gunpackb(src, pos_ptr, CSN_FINAL_ACK_IND_LEN);

  /*  Extract starting sequence number */
  msg_ptr->strt_seq_num = gunpackb(src, pos_ptr, CSN_STRT_SEQ_NUM_LEN);

  /*  Extract received block bitmap */
  for (i=0; i<8; i++)
    msg_ptr->received_blk_btmp[i] =
                          gunpackb(src, pos_ptr, CSN_RECEIVED_BLK_BTMP_LEN);

}

/*****************************************************************************
***
***     12.08 - Information Element - Frequency Parameters
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      csn_frequency_params_ie
===
===  DESCRIPTION
===
===    This function extracts the information from the Frequency Parameters
===    IE.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/

static csn_decoder_error_t csn_frequency_params_ie(
  uint8 src[],
  uint16 *pos_ptr,
  freq_params_t *msg_ptr,
  const gas_id_t gas_id
)
{
  uint8 local_ma_len;
  uint8 ma_index;
  uint8 encoding;

  /*  Extract tsc */
  msg_ptr->tsc = gunpackb(src, pos_ptr, CSN_TSC_LEN);

  /*  Next 2 bits determine the encoding type */
  encoding = gunpackb(src, pos_ptr, (int16)2);
  switch (encoding)
  {
    case FP_ARFCN:
      /*  Arfcn parameter present */
      CSN_DECODE_ARFCN(msg_ptr, pos_ptr);

      /* Set encoding type flag to indicate appropriate type */
      msg_ptr->encoding_type = arfcn_valid;

    break;

    case FP_IND_ENC:
      /*  Indirect encoding structure present */
      msg_ptr->indir_enc.maio = gunpackb(src, pos_ptr, CSN_MAIO_LEN);

      msg_ptr->indir_enc.ma_number =
                                  gunpackb(src, pos_ptr, CSN_MA_NUMBER_LEN);

      /*  Clear change mark flag before checking. */
      msg_ptr->indir_enc.change_mark_flag = no_change_mark_valid;

      /*  Next field is optional change_mark1 */
      if (gunpackb(src, pos_ptr, (int16)1))
      {
        /*  Change Mark 1 present */
        msg_ptr->indir_enc.change_mark_1 =
                              gunpackb(src, pos_ptr, CSN_CHANGE_MARK_1_LEN);

        /*  Set change mark flag before leaving. */
        msg_ptr->indir_enc.change_mark_flag = change_mark_1_valid;

        /*  Now see if change mark 2 is also present. */
        /*  Next field is optional change_mark2 */
        if (gunpackb(src, pos_ptr, (int16)1))
        {
          /*  Change Mark 2 present */
          msg_ptr->indir_enc.change_mark_2 =
                              gunpackb(src, pos_ptr, CSN_CHANGE_MARK_2_LEN);

          /*  Set change mark flag before leaving. If change_mark 1 set then */
          /*  set to both, else set to 2. */
          if (msg_ptr->indir_enc.change_mark_flag == change_mark_1_valid)
          {
            msg_ptr->indir_enc.change_mark_flag = both_change_marks_valid;

          }
          else
          {
            msg_ptr->indir_enc.change_mark_flag = change_mark_2_valid;
          }
        }
      }

      /* Set encoding type flag to indicate appropriate type */
      msg_ptr->encoding_type = indirect_valid;

    break;

    case FP_DIR_ENC1:
    {
      csn_decoder_error_t return_status;

      /*  Direct encoding 1 structure present */
      msg_ptr->dir_enc1.maio = gunpackb(src, pos_ptr, CSN_MAIO_LEN);

      /*  Next field is gprs mobile allocation */
      return_status = csn_gprs_mobile_alloc_ie(src, pos_ptr, &(msg_ptr->dir_enc1.gprs_mobile_alloc));

      if (return_status != CSN_DECODE_NO_ERROR)
      {
        return return_status;
      }

      /* Set encoding type flag to indicate appropriate type */
      msg_ptr->encoding_type = direct1_valid;

      break;
    }

    case FP_DIR_ENC2:
      /*  Direct encoding 2 structure present */
      msg_ptr->dir_enc2.maio = gunpackb(src, pos_ptr, CSN_MAIO_LEN);

      /*  Now extract hsn */
      msg_ptr->dir_enc2.hsn = gunpackb(src, pos_ptr, CSN_HSN_LEN);

      /*  Now extract length of ma frequency list */
      local_ma_len = gunpackb(src, pos_ptr, CSN_LEN_MA_FREQ_LIST_LEN);

      msg_ptr->dir_enc2.len_ma_freq_list = local_ma_len;

      /* Extract the ma_frequency list elements. */
      for(ma_index = 0; ma_index <= (local_ma_len + 2); ma_index++)
      {
         msg_ptr->dir_enc2.ma_freq_list[ma_index] =
                        (uint8)gunpackw(src, pos_ptr, CSN_MA_FREQ_LIST_LEN);
      }

      /* Set encoding type flag to indicate appropriate type */
      msg_ptr->encoding_type = direct2_valid;

    break;

    default:
    break;
  }

  return CSN_DECODE_NO_ERROR;
}


/*****************************************************************************
***
***     12.09 - Information Element - Global Power Control Parameters
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      csn_global_pwr_ctrl_ie
===
===  DESCRIPTION
===
===    This function extracts the information from the Global power ctrl
===    IE.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/

static void csn_global_pwr_ctrl_ie(
  uint8 src[], uint16 *pos_ptr,
  global_pwr_ctrl_t *msg_ptr)
{
  /*  Extract alpha */
  msg_ptr->alpha = gunpackb(src, pos_ptr, CSN_ALPHA_LEN);

  /*  Extract t_avg_w */
  msg_ptr->t_avg_w = gunpackb(src, pos_ptr, CSN_T_AVG_W_LEN);

  /*  Extract t_avg_t */
  msg_ptr->t_avg_t = gunpackb(src, pos_ptr, CSN_T_AVG_T_LEN);

  /*  Extract pb */
  msg_ptr->pb = gunpackb(src, pos_ptr, CSN_PB_LEN);

  /*  Extract pc_meas_chan */
  msg_ptr->pc_meas_chan = gunpackb(src, pos_ptr, CSN_PC_MEAS_CHAN_LEN);

  /*  Extract int_meas_chn_list */
  msg_ptr->int_meas_chn_list = gunpackb(src, pos_ptr,
                                                  CSN_INT_MEAS_CHN_LIST_LEN);

  /*  Extract n_avg_i */
  msg_ptr->n_avg_i = gunpackb(src, pos_ptr, CSN_N_AVG_I_LEN);

}


/*****************************************************************************
***
***     12.10 - Information Element - Global TFI
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      csn_global_tfi_ie
===
===  DESCRIPTION
===
===    This function extracts the information from the Global TFI IE
===    message.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/

static void csn_global_tfi_ie (
  uint8 src[], uint16 *pos_ptr,
  global_tfi_t *msg_ptr)
{
  /*  Extract global tfi */
  /*  Check if uplink or downlink tfi */
  if (!(gunpackb(src, pos_ptr, (int16)1)))
  {
    /*  Extract uplink tfi */
    msg_ptr->uplink_tfi = gunpackb(src, pos_ptr, CSN_UPLINK_TFI_LEN);

    /*  Set uplink/downlink enum */
    msg_ptr->direction = uplink_valid;
  }
  else
  {
    /*  Extract downlink tfi */
    msg_ptr->dwnlink_tfi = gunpackb(src, pos_ptr, CSN_DWNLINK_TFI_LEN);

    /*  Set uplink/downlink enum */
    msg_ptr->direction = dwnlink_valid;
  }

}


/*****************************************************************************
***
***     12.10a - Information Element - GPRS Mobile Allocation
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      csn_gprs_mobile_alloc_ie
===
===  DESCRIPTION
===
===    This function extracts the information from the GPRS Mobile Allocation
===    IE.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/

static csn_decoder_error_t csn_gprs_mobile_alloc_ie(
  uint8 src[], uint16 *pos_ptr,
  gprs_mobile_alloc_t *msg_ptr)
{
  /*  Extract hsn */
  msg_ptr->hsn = gunpackb(src, pos_ptr, CSN_HSN_LEN);

  /*  Next field is optional, rfl number list */
  if (gunpackb(src, pos_ptr, (int16)1))
  {
    /* RFL inclusion is not supported (requires PBCCH support) */
    return NON_DISTRIBUTION_ERROR;
  };

  /* RFL list size is always zero (indicates to reference Cell Allocation) */
  msg_ptr->size_of_rfl_list = 0;

  /*  Next fields are conditional on next option bit. */
  if (!(gunpackb(src, pos_ptr, (int16)1)))
  {
    /*  ma fields present */
    msg_ptr->ma_length = gunpackb(src, pos_ptr, CSN_MA_LENGTH_LEN);

    msg_ptr->ma_bitmap =
                      gunpackq(src, pos_ptr, (int16)((msg_ptr->ma_length) + 1));

    /*  Set flag indicating ma fields TRUE */
    msg_ptr->ma_arfcn_flag = ma_list_valid;
  }
  else
  {
    uint8 trap_count = MAX_NO_ARFCN_ELEMENTS;
    uint8 list_size = 0;

    /*  arfcn index may be present */
    while (gunpackb(src, pos_ptr, (int16)1) && trap_count)
    {
      /*  arfcn index present */
      msg_ptr->arfcn_index_list[list_size++] =
                            gunpackb(src, pos_ptr, CSN_ARFCN_INDEX_LIST_LEN);

      /*  Decrement error trap */
      trap_count--;
    };

    /*  Copy the size of the rfl list */
    msg_ptr->size_of_arfcn_list = list_size;

    /*  Set flag indicating arfcn fields TRUE */
    msg_ptr->ma_arfcn_flag = arfcn_list_valid;
  }

  return CSN_DECODE_NO_ERROR;
}


/*****************************************************************************
***
***     12.12 - Information Element - Packet Timing Advance
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      csn_pkt_timing_advance_ie
===
===  DESCRIPTION
===
===    This function extracts the information from the Packet Timing Advance
===    IE.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/

static void csn_pkt_timing_advance_ie(
  uint8 src[], uint16 *pos_ptr,
  pkt_timing_adv_t *msg_ptr)
{
  /*  Clear options flag before any checks made */
  msg_ptr->valid_options = no_timing_params_valid;

  if (gunpackb(src, pos_ptr, (int16)1))
  {
    /* Timing advance value present */
    /*  Extract value */
    msg_ptr->timing_adv_value =
                            gunpackb(src, pos_ptr, CSN_TIMING_ADV_VALUE_LEN);

    /*  Set flag to indicate field present */
    msg_ptr->valid_options = timing_value_valid;
  }

  if (gunpackb(src, pos_ptr, (int16)1))
  {
    /* Timing index and timeslot TRUE */
    /* Extract index first, followed by timeslot number */
    msg_ptr->timing_adv_index =
                            gunpackb(src, pos_ptr, CSN_TIMING_ADV_INDEX_LEN);

    msg_ptr->timing_adv_timeslot =
                        gunpackb(src, pos_ptr, CSN_TIMING_ADV_TIMESLOT_LEN);

    /*  Set flag to indicate field present, */
    /*  value set depends on current value */
    msg_ptr->valid_options ?
                        (msg_ptr->valid_options = all_timing_params_valid) :
                                (msg_ptr->valid_options = timing_indx_ts_valid);
  }

}


/*****************************************************************************
***
***     12.12a - Information Element - Global Packet Timing Advance
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION     csn_global_pkt_timing_ie
===
===  DESCRIPTION
===
===    This function extracts the information from the Packet Timing Advance
===    IE.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/

static void csn_global_pkt_timing_ie(
  uint8 src[], uint16 *pos_ptr,
  global_pkt_timing_t *msg_ptr)
{
  /*  Clear ta_flag */
  msg_ptr->ta_flag = FALSE;

  /*  Clear uplink_ta_flag */
  msg_ptr->uplink_ta_flag = FALSE;

  /*  Clear dwnlink_ta_flag */
  msg_ptr->dwnlink_ta_flag = FALSE;

  /*  Next field is optional */
  if (gunpackb(src, pos_ptr, (int16)1))
  {
    /* Timing Advance present, extract */
    msg_ptr->ta_value = gunpackb(src, pos_ptr, CSN_TA_VALUE_LEN);

    /*  Set ta_flag */
    msg_ptr->ta_flag = TRUE;
  }

  /*  Next field is optional */
  if (gunpackb(src, pos_ptr, (int16)1))
  {
    /* Uplink Timing Advance present, extract */
    msg_ptr->uplink_ta_index = gunpackb(src, pos_ptr,
                                                    CSN_UPLINK_TA_INDEX_LEN);

    msg_ptr->uplink_ta_ts_num = gunpackb(src, pos_ptr,
                                                  CSN_UPLINK_TA_TS_NUM_LEN);

    /*  Set ta_flag */
    msg_ptr->uplink_ta_flag = TRUE;
  }

  /*  Next field is optional */
  if (gunpackb(src, pos_ptr, (int16)1))
  {
    /* Downlink Timing Advance present, extract */
    msg_ptr->dwnlink_ta_index = gunpackb(src, pos_ptr,
                                              CSN_DWNLINK_TA_INDEX_LEN);

    msg_ptr->dwnlink_ta_ts_num = gunpackb(src, pos_ptr,
                                              CSN_DWNLINK_TA_TS_NUM_LEN);

    /*  Set ta_flag */
    msg_ptr->dwnlink_ta_flag = TRUE;
  }

}


/*****************************************************************************
***
***     12.13 - Information Element - Power Control Parameters
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      csn_pwr_cntrl_params_ie
===
===  DESCRIPTION
===
===    This function extracts the information from the Power Control Parameters
===    IE.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/

static void csn_pwr_cntrl_params_ie(
  uint8 src[], uint16 *pos_ptr,
  pwr_ctrl_paramtrs_t *msg_ptr)
{
  uint8 loop_count;
  uint8 gamma_flag_value = 1;
  uint8 *gamma_ptr;

  /*  Extract alpha value */
  msg_ptr->alpha = gunpackb(src, pos_ptr, CSN_ALPHA_LEN);

  /*  Setup gamma_ptr */
  gamma_ptr = &(msg_ptr->gamma_tn0);

  /*  Clear gamma_bitmap */
  msg_ptr->gamma_bitmap = 0;

  /*  Execute gamma extracts using a loop */
  for(loop_count=0;loop_count<8;loop_count++)
  {
    /*  Next field is optional */
    if (gunpackb(src, pos_ptr, (int16)1))
    {
      /*  Extract gamma value */
      *gamma_ptr = gunpackb(src, pos_ptr, CSN_GAMMA_TN_LEN);

      /*  Update gamma bitmap */
      msg_ptr->gamma_bitmap |= gamma_flag_value;
    }
    /*  Update gamma pointer and gamma_flag_value */
    gamma_ptr++;
    gamma_flag_value <<= 1;
  }
}


/*****************************************************************************
***
***     12.21 - Information Element - Starting Frame Number
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      csn_start_frm_num_ie
===
===  DESCRIPTION
===
===    This function extracts the information from the Power Control Parameters
===    IE.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/

static void csn_start_frm_num_ie(
  uint8 src[], uint16 *pos_ptr,
  start_frame_number_t *msg_ptr)
{
  /*  Extract tbf starting time. */
  /*  Determine if absolute or relative frame number encoding */
  if (gunpackb(src, pos_ptr, (int16)1))
  {
    /*  Relative frame number encoding */
    msg_ptr->relative = gunpackw(src, pos_ptr, CSN_RELATIVE_LEN);

    /*  Set flag to idicate relative */
    msg_ptr->abs_rel_flag = relative_valid;
  }
  else
  {
    /*  Absolute frame number encoding */
    msg_ptr->absolute = gunpackw(src, pos_ptr, CSN_ABSOLUTE_LEN);

    /*  Set flag to idicate absolute */
    msg_ptr->abs_rel_flag = absolute_valid;
  }

}


/*****************************************************************************
***
***     12.24 - Information Element - GPRS Cell Options
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      csn_decode_optional_extension_info
===
===  DESCRIPTION
===
===  This function decodes the optional extension information field from the
===  GPRS Cell Options IE.
===  Things to note: the length of the extension information field is given
===  by the bit(6) (extension_length) + 1.  We should extract exactly this many
===  bits from the incoming data.  It is possible that this many bits are not
===  actually sufficient to contain the complete extension information part.
===  If this is the case, we should stop extracting, but retain any extracted
===  data.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/
static void csn_decode_optional_extension_info(uint8 src[], uint16 *pos_ptr, optional_extension_info_t *msg_ptr)
{
  uint16 padding = 0;
  uint16 length  =
           gunpackb(src, pos_ptr,
                    CSN_GPRS_CELL_OPTIONS_OPTIONAL_EXTENSION_INFO_LENGTH_LEN) + 1;

  #define EXTRACT_OPT_EXT_INFO_PARAM(param,NUM_BITS)        \
  if (length >= NUM_BITS)                                   \
  {                                                         \
    msg_ptr->param =                                        \
      gunpackb(src, pos_ptr, NUM_BITS);                     \
                                                            \
    length -= NUM_BITS;                                     \
  }                                                         \
  else                                                      \
  {                                                         \
    msg_ptr->param = 0;                                     \
    if (length > 0)                                         \
    {                                                       \
      padding = length;                                     \
      length = 0;                                           \
    }                                                       \
  }

  /* R99 extension */

  EXTRACT_OPT_EXT_INFO_PARAM(egprs_supported_flag,1);

  if (msg_ptr->egprs_supported_flag)
  {
    EXTRACT_OPT_EXT_INFO_PARAM(egprs_packet_channel_request,CSN_EGPRS_PACKET_CHANNEL_REQUEST_LEN);
    EXTRACT_OPT_EXT_INFO_PARAM(bep_period,CSN_BEP_PERIOD_LEN);
  }

  EXTRACT_OPT_EXT_INFO_PARAM(pfc_feature_mode,CSN_PFC_FEATURE_MODE_LEN);
  EXTRACT_OPT_EXT_INFO_PARAM(dtm_support,CSN_DTM_SUPPORT_LEN);
  EXTRACT_OPT_EXT_INFO_PARAM(bss_paging_coordination,CSN_BSS_PAGING_COORDINATION_LEN);

  /* REL 4 extension */

  EXTRACT_OPT_EXT_INFO_PARAM(ccn_active,CSN_CCN_ACTIVE_LEN);
  EXTRACT_OPT_EXT_INFO_PARAM(nw_ext_tbf,CSN_NW_EXT_TBF_LEN);

  /* REL 6 extension */

  EXTRACT_OPT_EXT_INFO_PARAM(multiple_tbf_capability,CSN_MULTIPLE_TBF_CAPABILITY_LEN);
  EXTRACT_OPT_EXT_INFO_PARAM(ext_utbf_no_data,CSN_EXT_UTBF_NO_DATA_LEN);
  EXTRACT_OPT_EXT_INFO_PARAM(dtm_enhancements_capability,CSN_DTM_ENHANCEMENTS_CAPABILITY_LEN);
  MSG_GERAN_MED_3("Rel6 flags MTBF:%1d EXTULTBF:%1d EDTM:%1d",
          (int)msg_ptr->multiple_tbf_capability,
          (int)msg_ptr->ext_utbf_no_data,
          (int)msg_ptr->dtm_enhancements_capability);

  /*
   * Swallow any remaining bits
   * If the message was truncated, padding will be non-zero
   * If the message was over-long, length will be non-zero
   */
  if ( (padding + length) > 0)
  {
    (void) gunpackb(src, pos_ptr, (uint16)(padding + length));
  }
}

/*===========================================================================
===
===  FUNCTION      csn_gprs_cell_options_ie
===
===  DESCRIPTION
===
===    This function extracts the information from the GPRS Cell Options
===    IE.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/

static void csn_gprs_cell_options_ie(
  uint8 src[], uint16 *pos_ptr,
  gprs_cell_options_t *msg_ptr)
{

  /*  Extract NMO */
  msg_ptr->nmo = gunpackb(src, pos_ptr, CSN_NMO_LEN);

  /*  Extract T3168 */
  msg_ptr->t3168 = gunpackb(src, pos_ptr, CSN_T3168_LEN);

  /*  Extract T3192 */
  msg_ptr->t3192 = gunpackb(src, pos_ptr, CSN_T3192_LEN);

  /*  Extract DRX Timer max */
  msg_ptr->drx_timer_max = gunpackb(src, pos_ptr, CSN_DRX_TIMER_MAX_LEN);

  /*  Extract Access burst type */
  msg_ptr->access_burst_type =
                          gunpackb(src, pos_ptr, CSN_ACCESS_BURST_TYPE_LEN);

  /*  Extract Control Ack type */
  msg_ptr->ctrl_ack_type = gunpackb(src, pos_ptr, CSN_CTRL_ACK_TYPE_LEN);

  /*  Extract BS_CV_MAX */
  msg_ptr->bs_cv_max = gunpackb(src, pos_ptr, CSN_BS_CV_MAX_LEN);

  /*  Clear pan_flag */
  msg_ptr->pan_flag = FALSE;

  /*  Next field is optional */
  if (gunpackb(src, pos_ptr, (int16)1))
  {
    /* Pan_dec present */
    msg_ptr->pan_dec = gunpackb(src, pos_ptr, CSN_PAN_DEC_LEN);

    /* Pan_inc present */
    msg_ptr->pan_inc = gunpackb(src, pos_ptr, CSN_PAN_INC_LEN);

    /* Pan_dec present */
    msg_ptr->pan_max = gunpackb(src, pos_ptr, CSN_PAN_MAX_LEN);

    /*  Set pan_flag */
    msg_ptr->pan_flag = TRUE;
  }

  msg_ptr->optional_extension_info_flag = GUNPACKS(src, pos_ptr, (int16) 1);
  if (msg_ptr->optional_extension_info_flag)
  {
    csn_decode_optional_extension_info(src, pos_ptr, &msg_ptr->optional_extension_info);
  }
  else
  {
    /* ensure that no other optional extension flags are set */
    memset(&(msg_ptr->optional_extension_info),0,sizeof(optional_extension_info_t));
  }
}


/*****************************************************************************
***
***     12.26 - Information Element - Extension Bits IE
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION     csn_extension_bits_ie
===
===  DESCRIPTION
===
===    This function extracts the information from the Extension Bits
===    IE.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/

static void csn_extension_bits_ie(
  uint8 src[], uint16 *pos_ptr,
  extension_bits_t *msg_ptr)
{
  /*  Extract extension length */
  msg_ptr->ext_length = gunpackb(src, pos_ptr, CSN_EXT_LENGTH_LEN);

  /*  Extract extension */
  /*  Num of bits = ext_length+1 See GSM 4.60 12.26 */
  msg_ptr->extension = gunpackq(src, pos_ptr, (int16)(msg_ptr->ext_length+1));

}


/*****************************************************************************
***
***     EDGE Alterations
***
*****************************************************************************/


/*****************************************************************************
***
***     12.29 Information Element COMPACT reduced MA
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION    csn_compact_reduced_ma_ie
===
===  DESCRIPTION
===
===    This function extracts the information from the Element COMPACT reduced MA
===    IE.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/
static void csn_compact_reduced_ma_ie(uint8 src[], uint16 *pos_ptr, compact_reduced_ma_t *msg_ptr)
{
 #define ma_bit_1_to_64  64


  msg_ptr->reduced_ma_bit_length = gunpackb(src, pos_ptr, CSN_REDUCED_MA_BIT_LENGTH_LEN);

  if(msg_ptr->reduced_ma_bit_length < ma_bit_1_to_64)
  {
     msg_ptr->reduced_ma_bit_1_to_64 = gunpackq(src, pos_ptr, (int16)(msg_ptr->reduced_ma_bit_length));
     msg_ptr->reduced_ma_bits_65_to_127 = 0;
  }
  else
  {
    msg_ptr->reduced_ma_bit_1_to_64 = gunpackq(src, pos_ptr, (int16)(ma_bit_1_to_64));
    msg_ptr->reduced_ma_bits_65_to_127 = gunpackq(src, pos_ptr, (int16)(msg_ptr->reduced_ma_bit_length - ma_bit_1_to_64));
  }

  /*{ 0 | 1 <MAIO_2 : bit(6)} */
  if ((gunpackb(src, pos_ptr, (int16)1)))
  {
     msg_ptr->maio_2 = gunpackb(src, pos_ptr, CSN_MAIO_2_LEN);
     msg_ptr->maio_2_flag = TRUE;
  }
  else
  {
     msg_ptr->maio_2_flag = FALSE;
  }

}

/*****************************************************************************
***
***     12.5.3 EGPRS Multi Block Allocation struct
***
*****************************************************************************/
static void csn_multi_blk_alloc_extract(uint8 src[], uint16 *pos_ptr, multi_blk_alloc_t *msg_ptr)
{
  msg_ptr->timeslot_num = gunpackb(src, pos_ptr, CSN_TIMESLOT_NUMBER_LEN);

  /*{ 0 | 1 < ALPHA : bit (4) > < GAMMA_TN : bit (5) >} */

  if ((gunpackb(src, pos_ptr, (int16)1)))
  {
     msg_ptr->alpha = gunpackb(src, pos_ptr, CSN_ALPHA_LEN);
     msg_ptr->gamma_tn = gunpackb(src, pos_ptr, CSN_GAMMA_TN_LEN);

     msg_ptr->alpha_gamma_flag = TRUE;
  }
  else
  {
     msg_ptr->alpha_gamma_flag = FALSE;
  }

  /*{ 0 | 1 < P0 : bit (4) > < BTS_PWR_CTRL_MODE : bit (1) > < PR_MODE : bit (1) > } */
  if ((gunpackb(src, pos_ptr, (int16)1)))
  {
     msg_ptr->po = gunpackb(src, pos_ptr, CSN_PO_LEN);
     msg_ptr->bts_pwr_ctrl_mode = gunpackb(src, pos_ptr, CSN_BTS_PWR_CTRL_MODE_LEN);
     msg_ptr->pr_mode = gunpackb(src, pos_ptr, CSN_PR_MODE_LEN);
     msg_ptr->po_prmode_flag = TRUE;
  }
  else
  {
     msg_ptr->po_prmode_flag = FALSE;
  }

   /* < TBF Starting Time : < Starting Frame Number Description IE > */
  csn_start_frm_num_ie(src, pos_ptr, &(msg_ptr->tbf_start_frm_num));

  /* < NUMBER OF RADIO BLOCKS ALLOCATED: bit (2)>; */
  msg_ptr->number_of_blks_alloc = gunpackb(src, pos_ptr, CSN_NUMBER_OF_BLKS_ALLOC_LEN);


}

/*****************************************************************************
***
***     12.5.3 EGPRS Ack/Nack Description IE
***
*****************************************************************************/

static void csn_egprs_puan_desc_ie(uint8 src[], uint16 *pos_ptr, egprs_ack_nack_des_t *msg_ptr)
{
    uint8    array_cnt = 0;
    uint8    ack_nack_desc_length = 0;
    uint16    bitmap_length = 0;
    uint8    leftover_bits = 0;
    boolean  acknack_des_fills_rest_of_msg = FALSE;

    if (gunpackb(src, pos_ptr, (int16)1))
    {
      /*1 < Length L : bit (8) > -- Value part of this IE is of length L
      < bit (val(Length L)) > & < EGPRS Ack/Nack Description struct > ;
      Range 15 to 255*/
      acknack_des_fills_rest_of_msg = FALSE;
      ack_nack_desc_length = gunpackb(src, pos_ptr, 8);
    }
    else
    {
      /*0 < EGPRS Ack/Nack Description struct > -- This IE fills rest of message*/
      acknack_des_fills_rest_of_msg = TRUE;
    }

    /*< FINAL_ACK_INDICATION : bit (1) >  */
    msg_ptr->fai = gunpackb(src, pos_ptr, 1);
    /*< BEGINNING_OF_WINDOW : bit (1) >   */
    msg_ptr->bow = gunpackb(src, pos_ptr, 1);
    /*< END_OF_WINDOW : bit (1) >*/
    msg_ptr->eow = gunpackb(src, pos_ptr, 1);
    /*< STARTING_SEQUENCE_NUMBER : bit (11) >  */
    msg_ptr->ssn = gunpackw(src, pos_ptr, 11);

    /*{ 0 | 1 < COMPRESSED_BITMAP >  */
    if ((gunpackb(src, pos_ptr, (int16)1)))
    {
      /* < COMPRESSED_BITMAP_LENGTH: bit (7) >*/
      msg_ptr->crbb_len = gunpackb(src, pos_ptr, 7);
      /*< COMPRESSED_BITMAP_STARTING_COLOR_CODE: bit (1) >*/
      msg_ptr->scc = gunpackb(src, pos_ptr, 1);

      /*  Extract crbb bitmap. range from 0 - 127 bits.  extract as array of uint32.*/
      bitmap_length = msg_ptr->crbb_len;

      /* calculate the number of elements in the array*/
      array_cnt = (uint8)(bitmap_length / 32);

      if (array_cnt < sizeof(msg_ptr->crbb)/sizeof(msg_ptr->crbb[0])) /* belt and braces - check for invalid bitmap_length */
      {
        /* calculate the number of remaining bits not part of a complete 32-bit word*/
        leftover_bits = bitmap_length % 32;

        /* Unpack the leftover bits first */
        if (leftover_bits)
        {
          msg_ptr->crbb[array_cnt] =  gunpackd(src, pos_ptr, leftover_bits);
        }

        bitmap_length = bitmap_length - leftover_bits;
        array_cnt = array_cnt - 1;

        /* pack the remaining whole words*/
        while (bitmap_length >= 32)
        {
          /*  Extract octet */
          msg_ptr->crbb[array_cnt] =  gunpackd(src, pos_ptr, 32);

          /*  Decrement length */
          bitmap_length -= 32;
          array_cnt -= 1;
        };

        msg_ptr->comp_present = TRUE;
      }
      else /* invalid bitmap_length */
      {
        msg_ptr->comp_present = FALSE;
      }
    }
    else
    {
      msg_ptr->comp_present = FALSE;
    }

    /********   URBB element is ALWAYS present in ACK/NACK description   *********/

    /* Determine URBB bit len*/
    if(TRUE == acknack_des_fills_rest_of_msg)
    {
     /* bitmap_length = (max_length_of MAC control block in bits  -  current position)
      * i.e. pos_ptr depends on number of optional octets
      */

      bitmap_length = (dl_control_msg_size * 8) - *pos_ptr;

    }
    else
    {
      if(TRUE == msg_ptr->comp_present)
      {
         /* Lu =L - Lc - 23 */
         bitmap_length = (ack_nack_desc_length - msg_ptr->crbb_len) - 23;
      }
      else
      {
         /*Lu = L - 15*/
         bitmap_length = ack_nack_desc_length - 15;
      }
    }

    /* URBB bit len*/
    msg_ptr->urbb_len = (uint8)bitmap_length;

    /* calculate the number of elements in the array*/
    array_cnt = (uint8)(bitmap_length / 32);

    if (array_cnt < sizeof(msg_ptr->urbb)/sizeof(msg_ptr->urbb[0])) /* belt and braces - check for invalid bitmap_length */
    {
      /* calculate the number of remaining bits not part of a complete 32-bit word*/
      leftover_bits = bitmap_length % 32;

      /* Unpack the leftover bits first */
      if ((leftover_bits > 0) && (leftover_bits < 32))
      {
        msg_ptr->urbb[array_cnt] = gunpackd(src, pos_ptr, leftover_bits);
      }

      bitmap_length = bitmap_length - leftover_bits;
      array_cnt = array_cnt - 1;

      /* Extract urbb  bitmap. range from 0 - 127 bits. extract as array of uint32.*/
      while (bitmap_length >= 32)
      {
        /*  Extract octet */
        msg_ptr->urbb[array_cnt] =  gunpackd(src, pos_ptr, 32);

        /*  Decrement length */
        bitmap_length -= 32;
        array_cnt -= 1;
      };
    }
    /* else bitmap_length is invalid but there's no error return */
}


/*=============================================================================
===
===  RR CSN1 Decoder Public Functions
===
=============================================================================*/


/*===========================================================================
===
===  FUNCTION      gcsn1_rr_decoder
===
===  DESCRIPTION
===
===    Translate RR Sys Info Rest Octets from compressed format into
===    internal decompressed format
===
===    Ref 44.018 section
9.1.31    si 1    size = 1
9.1.35    si 3    size = 4
9.1.36    si 4    size = 0 to 10
9.1.40    si 6    size = 7
9.1.41    si 7    size = 20
9.1.42    si 8    size = 20
9.1.43    si 9    size = 17
9.1.43a   si 13   size = 20

9.1.22     page request 1   size = 0 to 17
9.1.23     page request 2   size = 0 to 11
9.1.24     page request 3   size = 3

9.1.18.0c
10.5.2.25a pkt channel discription (rcvd in IA only) size = 4

9.1.18  IA   size = 0 to 11
9.1.19  IAR  size = 3

9.1.12e DTM ass command
10.5.2.25c  rr pkt ul assignment size = 3 to n
10.5.2.25d  rr pkt dl assignment size = 3 to n
10.5.2.14d  gprs broadcast info  size = 7 to n

9.1.12g DTM information
10.5.2.11a  DTM information details size = 4 to n

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

/*public*/


csn_decoder_error_t gcsn1_rr_decoder(
  csn_rr_decoder_msg_id_t message_type,
  uint8 *csn1_ie_src,
  void *dst,
  uint16 len_to_end_of_buf,
  const gas_id_t gas_id
)
{
  /*use local buffer of max size to protect against over reads of OTA buffer*/
  uint8 src[MAX_CSN_OTA_MSG_IN_OCTETS];

  if(message_type == P1_REST_OCTETS || message_type == P2_REST_OCTETS)
  {
   /*Special case for P1 and P2 rest octets where truncation is allowed.*/
   /*Requires SRC to be padded with 2B's                                */
    memset(src,0x2b,MAX_CSN_OTA_MSG_IN_OCTETS);
  }
  else
  {
    memset(src,0,MAX_CSN_OTA_MSG_IN_OCTETS);
  }

  if(len_to_end_of_buf > MAX_CSN_OTA_MSG_IN_OCTETS)
  {
    memscpy(src,MAX_CSN_OTA_MSG_IN_OCTETS,csn1_ie_src,MAX_CSN_OTA_MSG_IN_OCTETS);
  }
  else
  {
    memscpy(src,MAX_CSN_OTA_MSG_IN_OCTETS,csn1_ie_src,len_to_end_of_buf);
  }

  switch (message_type)
  {
    case SI1_REST_OCTETS:
      return csn_decode_si1_rest_octets(src, (si1_rest_octets_t *) dst);
    case SI3_REST_OCTETS:
      return csn_decode_si3_rest_octets(src, (si3_rest_octets_t *) dst);
    case SI4_REST_OCTETS:
      return csn_decode_si4_rest_octets(src, (si4_rest_octets_t *) dst);
    case SI6_REST_OCTETS:
      return csn_decode_si6_rest_octets(src, (si6_rest_octets_t *) dst);
    case SI7_REST_OCTETS:
      return csn_decode_si7_si8_rest_octets(src, (struct si7_8_rest_octets_t *) dst);
    case SI8_REST_OCTETS:
      return csn_decode_si7_si8_rest_octets(src, (struct si7_8_rest_octets_t *) dst);
    case SI9_REST_OCTETS:
      return csn_decode_si9_rest_octets(src, (si9_rest_octets_t *) dst);
    case SI13_REST_OCTETS:
      return csn_decode_si13_rest_octets(src, (si13_rest_octets_t *) dst);
    case P1_REST_OCTETS:
      return csn_decode_p1_rest_octets(src, (p1_rest_octets_t *) dst, gas_id);
    case P2_REST_OCTETS:
      return csn_decode_p2_rest_octets(src, (p2_rest_octets_t *) dst);
    case P3_REST_OCTETS:
      return csn_decode_p3_rest_octets(src, (p3_rest_octets_t *) dst);
    case PACKET_CHANNEL_DESCRIPTION:
      return csn_decode_packet_channel_description(src, (packet_channel_description_t *) dst, gas_id);
    case IA_REST_OCTETS:
      return csn_decode_ia_rest_octets(src, (ia_rest_octets_t *) dst);
    case IAR_REST_OCTETS:
      return csn_decode_iar_rest_octets(src, (iar_rest_octets_t *) dst);
#ifdef FEATURE_GSM_DTM
    case RR_PACKET_UPLINK_ASSIGNMENT:
      return csn_decode_rr_packet_uplink_assignment(src, (rr_packet_uplink_assignment_t *) dst);
    case RR_PACKET_DOWNLINK_ASSIGNMENT:
      return csn_decode_rr_packet_downlink_assignment(src, (rr_packet_downlink_assignment_t *) dst);
    case GPRS_BROADCAST_INFORMATION:
      return csn_decode_gprs_broadcast_information(src, (gprs_broadcast_information_t *) dst);
    case DTM_INFORMATION_DETAILS:
      return csn_decode_dtm_information_details(src, (dtm_information_details_t *) dst);
#endif /* FEATURE_GSM_DTM */
    default:
      return UNKNOWN_MESSAGE_TYPE;
  }
}


/*=============================================================================
===
===  RR CSN1 Decoder Private Functions
===
=============================================================================*/

static csn_decoder_error_t csn_decode_si1_rest_octets(uint8 *src, si1_rest_octets_t *dst)
{
  uint16 msg_pos = 0;

  memset(dst, 0, sizeof(si1_rest_octets_t));
  dst->nch_position_flag = GUNPACKS_LH(src, &msg_pos);
  if (dst->nch_position_flag)
  {
    dst->nch_position = gunpackb(src, &msg_pos, CSN_NCH_POSITION_LEN);
  }

  dst->band_indicator = GUNPACKS_LH(src, &msg_pos);

  return CSN_DECODE_NO_ERROR;
}


/*===========================================================================
===
===  FUNCTION     csn_decode_si3_rest_octets
===
===  DESCRIPTION
===
===    Decode CSN1 encoded SI3 Rest Octets.
===
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/
static csn_decoder_error_t csn_decode_si3_rest_octets(uint8 *src, si3_rest_octets_t *dst)
{
 uint16 msg_pos = 0;

 memset(dst, 0, sizeof(si3_rest_octets_t));
 dst->selection_params_flag = GUNPACKS_LH(src, &msg_pos);
 if (dst->selection_params_flag)
 {
  dst->cbq = gunpackb(src, &msg_pos, CSN_CBQ_LEN);
  dst->cell_reselect_offset = gunpackb(src, &msg_pos, CSN_CELL_RESELECT_OFFSET_LEN);
  dst->temporary_offset = gunpackb(src, &msg_pos, CSN_TEMPORARY_OFFSET_LEN);
  dst->penalty_time = gunpackb(src, &msg_pos, CSN_PENALTY_TIME_LEN);
 }

 dst->power_offset_flag = GUNPACKS_LH(src, &msg_pos);
 if (dst->power_offset_flag)
 {
  dst->power_offset = gunpackb(src, &msg_pos, CSN_POWER_OFFSET_LEN);
 }

 dst->si2ter_indicator = gunpackb_lh(src, &msg_pos);

 dst->ecsc = gunpackb_lh(src, &msg_pos);

 dst->siaw_flag = GUNPACKS_LH(src, &msg_pos);
 if (dst->siaw_flag)
 {
  dst->where = gunpackb(src, &msg_pos, CSN_WHERE_LEN);
 }

 dst->gprs_indicator_flag = GUNPACKS_LH(src, &msg_pos);
 if (dst->gprs_indicator_flag)
 {
  dst->ra_colour = gunpackb(src, &msg_pos, CSN_RA_COLOUR_LEN);
  dst->si13_position = gunpackb(src, &msg_pos, CSN_SI13_POSITION_LEN);
 }

 dst->three_g_ecsr = gunpackb_lh(src, &msg_pos);

 dst->si2quater_indicator_flag = GUNPACKS_LH(src, &msg_pos);
 if (dst->si2quater_indicator_flag)
 {
  dst->si2quater_position = gunpackb(src, &msg_pos, CSN_SI2QUATER_POSITION_LEN);
 }

 return CSN_DECODE_NO_ERROR;
}

/*===========================================================================
===
===  FUNCTION     csn_decode_si4_rest_octets
===
===  DESCRIPTION
===
===    Decode CSN1 encoded SI 4 Rest Octets.
===
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/
static csn_decoder_error_t csn_decode_si4_rest_octets(uint8 *src, si4_rest_octets_t *dst)
{
 lsa_identity_t *lsa_ptr;
 uint16 msg_pos = 0;

 memset(dst, 0, sizeof(si4_rest_octets_t));
 dst->selection_params_flag = GUNPACKS_LH(src, &msg_pos);
 if (dst->selection_params_flag)
 {
  dst->cbq = gunpackb(src, &msg_pos, CSN_CBQ_LEN);
  dst->cell_reselect_offset = gunpackb(src, &msg_pos, CSN_CELL_RESELECT_OFFSET_LEN);
  dst->temporary_offset = gunpackb(src, &msg_pos, CSN_TEMPORARY_OFFSET_LEN);
  dst->penalty_time = gunpackb(src, &msg_pos, CSN_PENALTY_TIME_LEN);
 }
 dst->power_offset_flag = GUNPACKS_LH(src, &msg_pos);
 if (dst->power_offset_flag)
 {
  dst->power_offset = gunpackb(src, &msg_pos, CSN_POWER_OFFSET_LEN);
 }
 dst->gprs_indicator_flag = GUNPACKS_LH(src, &msg_pos);
 if (dst->gprs_indicator_flag)
 {
  dst->ra_colour = gunpackb(src, &msg_pos, CSN_RA_COLOUR_LEN);
  dst->si13_position = gunpackb(src, &msg_pos, CSN_SI13_POSITION_LEN);
 }
 dst->si4_rest_octets_s_flag = GUNPACKS_LH(src, &msg_pos);
 if (dst->si4_rest_octets_s_flag)
 {
  dst->lsa_parameters_flag = GUNPACKS_LH(src, &msg_pos);
  if (dst->lsa_parameters_flag)
  {
   dst->prio_thr = gunpackb(src, &msg_pos, CSN_PRIO_THR_LEN);
   dst->lsa_offset = gunpackb(src, &msg_pos, CSN_LSA_OFFSET_LEN);
   dst->mcc_mnc_flag = GUNPACKS(src, &msg_pos, (word) 1);
   if (dst->mcc_mnc_flag)
   {
    dst->mcc = gunpackw(src, &msg_pos, CSN_MCC_LEN);
    dst->mnc = gunpackw(src, &msg_pos, CSN_MNC_LEN);
   }
  }
  dst->cell_identity_flag = GUNPACKS_LH(src, &msg_pos);
  if (dst->cell_identity_flag)
  {
   dst->cell_identity = gunpackw(src, &msg_pos, CSN_CELL_IDENTITY_LEN);
  }
  dst->lsa_id_info_count = 0;
  lsa_ptr = &(dst->lsa_id_info[0]);
  if (gunpackb_lh(src, &msg_pos))
  {
   do {
    dst->lsa_id_info_count++;
    lsa_ptr->short_lsa_id_flag = GUNPACKS(src, &msg_pos, (word) 1);
    if (lsa_ptr->short_lsa_id_flag)
    {
     lsa_ptr->short_lsa_id = gunpackw(src, &msg_pos, CSN_SHORT_LSA_ID_LEN);
    }
    else
    {
     lsa_ptr->lsa_id = gunpackd(src, &msg_pos, CSN_LSA_ID_LEN);
    }
    lsa_ptr->lsa_id_type = gunpackb(src, &msg_pos, (word) 1) == 0 ? plmn_significant_number : universal_lsa;
    lsa_ptr++;
   } while (gunpackb(src, &msg_pos, 1) && (dst->lsa_id_info_count < MAX_LSA_ID_INFO));
  }
 }
 return CSN_DECODE_NO_ERROR;
}



/*===========================================================================
 ===
 ===  FUNCTION     csn_decode_si6_rest_octets
 ===
 ===  DESCRIPTION
 ===
 ===    Decode CSN1 encoded SI6 Rest Octets.
 ===
 ===
 ===  DEPENDENCIES
 ===
 ===    xxx.
 ===
 ===  RETURN VALUE
 ===
 ===    xxx.
 ===
 ===  SIDE EFFECTS
 ===
 ===    xxx.
 ===
 ===========================================================================*/
static csn_decoder_error_t csn_decode_si6_rest_octets(uint8 *src, si6_rest_octets_t *dst)
{
  uint16 msg_pos = 0;

  memset(dst, 0, sizeof(si6_rest_octets_t));
  dst->pch_and_nch_flag = GUNPACKS_LH(src, &msg_pos);
  if (dst->pch_and_nch_flag == TRUE)
  {
    dst->page_ch_restruct = gunpackb(src, &msg_pos, 1);
    dst->nln_sacch = gunpackb(src, &msg_pos, CSN_NLN_SACCH_LEN);

    dst->call_priority_flag = GUNPACKS(src, &msg_pos, 1);
    if (dst->call_priority_flag == TRUE)
    {
      dst->call_priority = gunpackb(src, &msg_pos, CSN_CALL_PRIORITY_LEN);
    }
    dst->nln_status = gunpackb(src, &msg_pos, 1);
  }

  dst->vbs_vgcs_flag = GUNPACKS_LH(src, &msg_pos);
  if (dst->vbs_vgcs_flag == TRUE)
  {
    dst->vbs_vgcs_inband_notif = gunpackb(src, &msg_pos, 1);
    dst->vbs_vgcs_inband_pagings = gunpackb(src, &msg_pos, 1);
  }

  dst->dtm_supp = gunpackb_lh(src, &msg_pos);
  if (dst->dtm_supp)
  {
    dst->rac = gunpackb(src, &msg_pos, CSN_RAC_LEN);
    dst->max_lapdm = gunpackb(src, &msg_pos, CSN_MAX_LAPDM_LEN);
  }

  dst->band_ind = gunpackb_lh(src, &msg_pos);

  dst->gprs_ms_tmc_flag = GUNPACKS_LH(src, &msg_pos);
  if (dst->gprs_ms_tmc_flag == TRUE)
  {
    dst->gprs_ms_tmc = gunpackb(src, &msg_pos, CSN_GPRS_MS_TMC_LEN);
  }

  return(CSN_DECODE_NO_ERROR);
}



static csn_decoder_error_t csn_decode_si7_8_rest_octets(uint8 *src, struct si7_8_rest_octets_t *dst)
{
 lsa_identity_t *lsa_ptr;
 uint16 msg_pos = 0;

 if (dst->si4_acs_flag)
 {
  dst->selection_params_flag = GUNPACKS_LH(src, &msg_pos);
  if (dst->selection_params_flag)
  {
   dst->cbq = gunpackb(src, &msg_pos, CSN_CBQ_LEN);
   dst->cell_reselect_offset = gunpackb(src, &msg_pos, CSN_CELL_RESELECT_OFFSET_LEN);
   dst->temporary_offset = gunpackb(src, &msg_pos, CSN_TEMPORARY_OFFSET_LEN);
   dst->penalty_time = gunpackb(src, &msg_pos, CSN_PENALTY_TIME_LEN);
  }
  dst->power_offset_flag = GUNPACKS_LH(src, &msg_pos);
  if (dst->power_offset_flag)
  {
   dst->power_offset = gunpackb(src, &msg_pos, CSN_POWER_OFFSET_LEN);
  }
  dst->gprs_indicator_flag = GUNPACKS_LH(src, &msg_pos);
  if (dst->gprs_indicator_flag)
  {
   dst->ra_colour = gunpackb(src, &msg_pos, CSN_RA_COLOUR_LEN);
   dst->si13_position = gunpackb(src, &msg_pos, CSN_SI13_POSITION_LEN);
  }
 }
 dst->lsa_parameters_flag = GUNPACKS_LH(src, &msg_pos);
 if (dst->lsa_parameters_flag)
 {
  dst->prio_thr = gunpackb(src, &msg_pos, CSN_PRIO_THR_LEN);
  dst->lsa_offset = gunpackb(src, &msg_pos, CSN_LSA_OFFSET_LEN);
  dst->mcc_mnc_flag = GUNPACKS(src, &msg_pos, (word) 1);
  if (dst->mcc_mnc_flag)
  {
   dst->mcc = gunpackw(src, &msg_pos, CSN_MCC_LEN);
   dst->mnc = gunpackw(src, &msg_pos, CSN_MNC_LEN);
  }
 }
 dst->cell_identity_flag = GUNPACKS_LH(src, &msg_pos);
 if (dst->cell_identity_flag)
 {
  dst->cell_identity = gunpackw(src, &msg_pos, CSN_CELL_IDENTITY_LEN);
 }
 dst->lsa_id_info_count = 0;
 lsa_ptr = &(dst->lsa_id_info[0]);
 if (gunpackb_lh(src, &msg_pos))
 {
  do {
   dst->lsa_id_info_count++;
   lsa_ptr->short_lsa_id_flag = GUNPACKS(src, &msg_pos, (word) 1);
   if (lsa_ptr->short_lsa_id_flag)
   {
    lsa_ptr->short_lsa_id = gunpackw(src, &msg_pos, CSN_SHORT_LSA_ID_LEN);
   }
   else
   {
    lsa_ptr->lsa_id = gunpackd(src, &msg_pos, CSN_LSA_ID_LEN);
   }
   lsa_ptr->lsa_id_type = gunpackb(src, &msg_pos, (word) 1) == 0 ? plmn_significant_number : universal_lsa;
   lsa_ptr++;
  } while (gunpackb(src, &msg_pos, (word) 1) && (dst->lsa_id_info_count < MAX_LSA_ID_INFO));
 }
 return CSN_DECODE_NO_ERROR;
}

/*
04.18 v8a0: 10.5.3.35

If "ACS" in the System information type 4 message is set to "1" then the
SI 7 and SI 8 rest octets consists of "SI4 Rest Octets_O" and "SI4 Rest Octets_S",
otherwise of only "SI4 Rest Octets_S".

BEFORE csn_decode_si8... is called, (via gcsn1_rr_decoder), the si8_rest_octets_t
struct MUST have the si4_acs_flag set to the appropriate value (decoded from SI4 message).

*/

static csn_decoder_error_t csn_decode_si7_si8_rest_octets(uint8 *src, struct si7_8_rest_octets_t *dst)
{
  /*Save ACS flag setting and set dst again after the memset*/
  boolean acs_flag = dst->si4_acs_flag;
  memset(dst, 0, sizeof(si8_rest_octets_t));
  dst->si4_acs_flag = acs_flag;

 return csn_decode_si7_8_rest_octets(src, dst);
}


/*===========================================================================
===
===  FUNCTION     csn_decode_si13_rest_octets
===
===  DESCRIPTION
===
===    Decode CSN1 encoded SI 13 Rest Octets.
===
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/
static csn_decoder_error_t csn_decode_si13_rest_octets(uint8 *src, si13_rest_octets_t *dst)
{
  uint16 msg_pos = 0;

  memset(dst, 0, sizeof(si13_rest_octets_t));
  dst->lb_ms_txpwr_max_cch = INVALID_LB_MS_TXPWR_MAX_CCH;
  dst->si13_info_included = gunpackb_lh(src, &msg_pos);

  if (dst->si13_info_included)
  {
    dst->bcch_chng_mrk = gunpackb(src, &msg_pos, CSN_BCCH_CHNG_MRK_LEN);
    dst->si_chng_field = gunpackb(src, &msg_pos, CSN_SI_CHNG_FIELD_LEN);
    dst->si13_cm_gprs_ma_flag = gunpackb(src, &msg_pos, 1);
    if (dst->si13_cm_gprs_ma_flag)
    {
      csn_decoder_error_t return_status;

      dst->si13_change_mark = gunpackb(src, &msg_pos, CSN_SI13_CHANGE_MARK_LEN);

      return_status = csn_gprs_mobile_alloc_ie(src, &msg_pos, &(dst->gprs_mobile_alloc));

      if (return_status != CSN_DECODE_NO_ERROR)
      {
        return return_status;
      }
    }
    dst->pbcch_present_flag = gunpackb(src, &msg_pos, 1);
    if (dst->pbcch_present_flag)
    {
      /* PBCCH presence should never be advertised - ignore the message */
      return NON_DISTRIBUTION_ERROR;
    }
    else /* pbcch not present */
    {
      dst->rac = gunpackb(src, &msg_pos, CSN_RAC_LEN);
      dst->spgc_ccch_sup = gunpackb(src, &msg_pos, CSN_SPGC_CCCH_SUP_LEN);
      dst->priority_access_thr = gunpackb(src, &msg_pos, CSN_PRIORITY_ACCESS_THR_LEN);
      dst->network_ctrl_ordr = gunpackb(src, &msg_pos, 2);
      csn_gprs_cell_options_ie(src, &msg_pos, &(dst->gprs_cell_options));
      /* the remaining fields are the Power Control Parameters */
      dst->alpha = gunpackb(src, &msg_pos, CSN_ALPHA_LEN);
      dst->t_avg_w = gunpackb(src, &msg_pos, CSN_T_AVG_W_LEN);
      dst->t_avg_t = gunpackb(src, &msg_pos, CSN_T_AVG_T_LEN);
      dst->pc_meas_chan = gunpackb(src, &msg_pos, CSN_PC_MEAS_CHAN_LEN);
      dst->n_avg_i = gunpackb(src, &msg_pos, CSN_N_AVG_I_LEN);
    }

    /* RELEASE 99 additions: SGSNR */
    dst->sgsnr_flag = GUNPACKS_LH(src, &msg_pos);
    dst->r4_additions_flag = FALSE;
    dst->r6_additions_flag = FALSE;

    if (dst->sgsnr_flag == TRUE)
    {
      dst->sgsnr = gunpackb(src, &msg_pos, CSN_SGSNR_LEN);

      /* RELEASE 4 additions: SI_STATUS_IND */
      dst->r4_additions_flag = GUNPACKS_LH(src, &msg_pos);
      if (dst->r4_additions_flag == TRUE)
      {
        dst->si_status_ind = gunpackb(src, &msg_pos, 1);

        /* RELEASE 6 additions: LB_MS_TXPWR_MAX_CCH and SI2n_SUPPORT */
        dst->r6_additions_flag = GUNPACKS_LH(src, &msg_pos);
        if (dst->r6_additions_flag == TRUE)
        {
          if (gunpackb(src, &msg_pos, 1) == 1)
          {
            dst->lb_ms_txpwr_max_cch = gunpackb(src, &msg_pos, CSN_LB_MS_TXPWR_MAX_CCH_LEN);
          }

          dst->si2n_support = gunpackb(src, &msg_pos, CSN_SI2N_SUPPORT_LEN);
        }
      }
      else
      {
        /* Rel4 additions not present, default SI_STATUS_IND to zero */
        dst->si_status_ind = 0;
      }
    }
  }

 return CSN_DECODE_NO_ERROR;
}


#ifdef FEATURE_GSM_DTM

static void csn_decode_rr_pua_dynamic_alloc(uint8 *src, uint16 *msg_pos, rr_pua_dynamic_alloc_t *dst)
{
 uint8 power_flag = 0;
 uint8 *usf_ptr;
 uint8 *gamma_ptr;
 uint8 loop_count;
 uint8 flag_value = 1;

 dst->ext_dynamic_alloc = gunpackb(src, msg_pos, CSN_EXT_DYNAMIC_ALLOC_LEN);
 dst->po_prmode_flag = GUNPACKS(src, msg_pos, (word) 1);
 if (dst->po_prmode_flag)
 {
  dst->po = gunpackb(src, msg_pos, CSN_P0_LEN);
  dst->pr_mode = gunpackb(src, msg_pos, CSN_PR_MODE_LEN);
 }
 dst->usf_granularity = gunpackb(src, msg_pos, CSN_USF_GRANULARITY_LEN);
 dst->uplk_tfi_assignment_flag = GUNPACKS(src, msg_pos, (word) 1);
 if (dst->uplk_tfi_assignment_flag)
 {
  dst->uplk_tfi_assignment = gunpackb(src, msg_pos, CSN_UPLINK_TFI_ASSIGNMENT_LEN);
 }
 dst->rlc_data_blks_granted_flag = GUNPACKS(src, msg_pos, (word) 1);
 if (dst->rlc_data_blks_granted_flag)
 {
  dst->rlc_data_blks_granted = gunpackb(src, msg_pos, CSN_RLC_DATA_BLKS_GRANTED_LEN);
 }
 dst->timeslot_alloc_flag = gunpackb(src, msg_pos, (word) 1) == 0 ? usf_only : usf_and_power_control;
 if (dst->timeslot_alloc_flag == usf_and_power_control)
 {
  power_flag = 1;
  dst->alpha = gunpackb(src, msg_pos, CSN_ALPHA_LEN);
 }
 /* Based on the power flag extract the appropriate data */
 /* Setup usf pointer */
 usf_ptr = &(dst->usf_tn[0]);

 /* Clear usf bitmap */
 dst->usf_bitmap = 0;

 /*  Setup gamma pointer */
 gamma_ptr = &(dst->gamma_tn[0]);

 /* Clear gamma bitmap */
 dst->gamma_bitmap = 0;

 /* Execute usf and possible gamma extracts using a loop */
 for(loop_count=0;loop_count<8;loop_count++)
 {
  /* Next field is optional */
  if (gunpackb(src, msg_pos, (int16)1))
  {
   /* Extract usf value */
   *usf_ptr = gunpackb(src, msg_pos, CSN_USF_LEN);

   /* Update usf bitmap */
   dst->usf_bitmap |= flag_value;

   if (power_flag)
   {
    /* Extract gamma value */
    *gamma_ptr = gunpackb(src, msg_pos, CSN_GAMMA_LEN);
    /* Update gamma bitmap */
    dst->gamma_bitmap |= flag_value;
   }
  }

  /* Update usf pointer and usf_flag_value */
  usf_ptr++;
  flag_value <<= 1;

  if (power_flag) gamma_ptr++;
 }
}

static void csn_gprs_power_control_params_ie(
  uint8 *src_ptr,
  uint16 *pos_ptr,
  gprs_power_control_params_t *dst_ptr
)
{
  /*  Extract alpha */
  dst_ptr->alpha = gunpackb(src_ptr, pos_ptr, CSN_ALPHA_LEN);

  /*  Extract t_avg_w */
  dst_ptr->t_avg_w = gunpackb(src_ptr, pos_ptr, CSN_T_AVG_W_LEN);

  /*  Extract t_avg_t */
  dst_ptr->t_avg_t = gunpackb(src_ptr, pos_ptr, CSN_T_AVG_T_LEN);

  /*  Extract pc_meas_chan */
  dst_ptr->pc_meas_chan = gunpackb(src_ptr, pos_ptr, CSN_PC_MEAS_CHAN_LEN);

  /*  Extract n_avg_i */
  dst_ptr->n_avg_i = gunpackb(src_ptr, pos_ptr, CSN_N_AVG_I_LEN);

} /* end csn_gprs_power_control_params_ie() */

static csn_decoder_error_t csn_decode_rr_packet_uplink_assignment(uint8 *src, rr_packet_uplink_assignment_t *dst)
{
  uint16 msg_pos = 0;

  memset(dst, 0, sizeof(rr_packet_uplink_assignment_t));
  dst->length_in_octets          = gunpackb(src, &msg_pos, CSN_LENGTH_IN_OCTETS_LEN);
  dst->channel_coding_command    = gunpackb(src, &msg_pos, CSN_CHANNEL_CODING_COMMAND_LEN);
  dst->tlli_block_channel_coding = gunpackb(src, &msg_pos, CSN_TLLI_BLOCK_CHANNEL_CODING_LEN);
  csn_pkt_timing_advance_ie(src, &msg_pos, &(dst->pkt_timing_adv));
  dst->allocation_type           = gunpackb(src, &msg_pos, CSN_ALLOCATION_TYPE_LEN);
  switch (dst->allocation_type)
  {
    case DYNAMIC_ALLOC:
      csn_decode_rr_pua_dynamic_alloc(src, &msg_pos, &(dst->allocation.dynamic_alloc));
      break;

    case SINGLE_BLK_ALLOC:
    case FIXED_ALLOC:
      /* See 3GPP 44.018 Table 10.5.2.25c.1 */
      /* allocated in an earlier version of the protocol and shall not be used */
      MSG_GERAN_ERROR_1("RR packet UL assignment type 0x%02X not supported",
                (int)dst->allocation_type);
      return(NON_DISTRIBUTION_ERROR);

    case EXTENSION:
      break;

    default:
      /* do nothing */
      break;
  }
  /* if we get this far, we may have some more message to decode... */
  if ((dst->length_in_octets * 8) == msg_pos)
  {
    dst->egprs_flag = FALSE;
    dst->packet_ext_timing_adv_flag = FALSE;
    return CSN_DECODE_NO_ERROR;
  }
  else
  {
    dst->egprs_flag = FALSE;
    dst->packet_ext_timing_adv_flag = FALSE;

    /* When calculating how many bits decoded so far, remember to subtract 8 from msg_pos */
    /* due to the initial length octet at the start of the IE */
    #define MAX_NUM_OF_BITS_REMAINING(curr_bit_pos)  ((dst->length_in_octets * 8) - (curr_bit_pos - 8))

    /* Check that there is at least enough room for the two indicator bits to say that the */
    /* R99 additions are not present */
    if ( MAX_NUM_OF_BITS_REMAINING(msg_pos) >= 2 )
    {
      boolean flag;

      /* Next bit indicates whether EGPRS MCS Mode, Resegment and EGPRS Window */
      /* Size are present or not */
      flag = GUNPACKS(src, &msg_pos, (word) 1);

      if ( flag )
      {
        /* Check whether there are enough bits left in the message to encode these */
        /* elements */
        if ( MAX_NUM_OF_BITS_REMAINING(msg_pos) >= 11 )
        {
          dst->egprs_flag = TRUE;
          dst->egprs_mcs_mode = gunpackb(src, &msg_pos, CSN_EGPRS_MCS_MODE_LEN);
          dst->resegment = gunpackb(src, &msg_pos, CSN_RESEGMENT_LEN);
          dst->epgrs_win_sz = gunpackb(src, &msg_pos, CSN_EGPRS_WINDOW_SIZE_LEN);
        }
        else
        {
          return RR_TRUNCATED_MSG;
        }
      }

      /* There must be at least one bit left to complete the decoding */
      /* (for the Packet Extended Timing Advance presence bit */
      if ( MAX_NUM_OF_BITS_REMAINING(msg_pos) >= 1 )
      {
        /* The next bit indicates whether Packet Extended Timing Advance params */
        /* are present */
        flag = GUNPACKS(src, &msg_pos, (word) 1);

        if ( flag )
        {
          /* Check there are enough bits remaining to encode the Packet Extended */
          /* Timing Advance params */
          if ( MAX_NUM_OF_BITS_REMAINING(msg_pos)>= CSN_PACKET_EXT_TIMING_ADV_LEN )
          {
            dst->packet_ext_timing_adv_flag = TRUE;
            dst->packet_ext_timing_adv = gunpackb(src, &msg_pos, CSN_PACKET_EXT_TIMING_ADV_LEN);
          }
          else
          {
            return RR_TRUNCATED_MSG;
          }
        }
      }
    }
  }

#ifdef FEATURE_EDGE_CAPABILITY_NOT_SUPPORTED
  if (dst->egprs_flag == TRUE)
  {
    /* RR Uplink Assignment contains unsupported EGPRS parameters */
    return NON_DISTRIBUTION_ERROR;
  }
#endif /* FEATURE_EDGE_CAPABILITY_NOT_SUPPORTED */

  return CSN_DECODE_NO_ERROR;
}

static csn_decoder_error_t csn_decode_rr_packet_downlink_assignment(uint8 *src, rr_packet_downlink_assignment_t *dst)
{
  uint16 msg_pos = 0;

  memset(dst, 0, sizeof(rr_packet_downlink_assignment_t));
  dst->length_in_octets                     = gunpackb(src, &msg_pos, CSN_LENGTH_IN_OCTETS_LEN);
  dst->mac_mode                             = gunpackb(src, &msg_pos, CSN_MAC_MODE_LEN);
  dst->rlc_mode                             = gunpackb(src, &msg_pos, CSN_RLC_MODE_LEN);
  dst->timeslot_allocation                  = gunpackb(src, &msg_pos, CSN_TIMESLOT_ALLOC_LEN);
  csn_pkt_timing_advance_ie(src, &msg_pos, &(dst->packet_timing_advance));
  dst->dwnlk_pwr_ctrl_flag                  = GUNPACKS(src, &msg_pos, (word) 1);
  if (dst->dwnlk_pwr_ctrl_flag)
  {
    dst->dwnlk_pwr_ctrl.po                = gunpackb(src, &msg_pos, CSN_PO_LEN);
    dst->dwnlk_pwr_ctrl.bts_pwr_ctrl_mode = gunpackb(src, &msg_pos, CSN_BTS_PWR_CTRL_MODE_LEN);
    dst->dwnlk_pwr_ctrl.pr_mode           = gunpackb(src, &msg_pos, CSN_PR_MODE_LEN);
  }
  dst->pwr_ctrl_paramtrs_flag               = GUNPACKS(src, &msg_pos, (word) 1);
  if (dst->pwr_ctrl_paramtrs_flag)
  {
    csn_pwr_cntrl_params_ie(src, &msg_pos, &(dst->pwr_ctrl_paramtrs));
  }
  dst->downlink_tfi_assignment_flag         = GUNPACKS(src, &msg_pos, (word) 1);
  if (dst->downlink_tfi_assignment_flag)
  {
    dst->downlink_tfi_assignment          = gunpackb(src, &msg_pos, CSN_DOWNLINK_TFI_ASSIGNMENT_LEN);
  }
  dst->measurement_flag                     = GUNPACKS(src, &msg_pos, (word) 1);
  if (dst->measurement_flag)
  {
    dst->measurement_starting_time        = gunpackw(src, &msg_pos, CSN_MEASUREMENT_STARTING_TIME_LEN);
    dst->measurement_interval             = gunpackb(src, &msg_pos, CSN_MEASUREMENT_INTERVAL_LEN);
    dst->measurement_bitmap               = gunpackb(src, &msg_pos, CSN_MEASUREMENT_BITMAP_LEN);
  }
 /*
 R99 additions:
 There is no indicator to tell us if the R99 additions are included in the message.
 We have to check the message length to determine whether or not there is sufficient
 space left to potentially contain the R99 additions. The minimum length of the R99
 additions is 2 bits (in the case that the egprs tbf flag is 0, and the packet
 extended timing advance flag is 0, so this is the minimum amount of space we need.
 For each following field, we check that there is sufficient remaining space before
 unpacking it. These check can be eliminated if we can be sure that the incoming message is
 in a (large enough) statically assigned chunk of memory, that is cleared before
 the incoming message is written to it.
 */

  dst->egprs_tbf_mode_flag = FALSE;
  dst->packet_ext_timing_adv_flag = FALSE;

  if ( MAX_NUM_OF_BITS_REMAINING(msg_pos) >= 2 )
  {
    boolean flag;

    /* The next bit indicates 'EGPRS TBF Mode' if set */
    flag = GUNPACKS(src, &msg_pos, (word) 1);

    if ( flag )
    {
      /* Now check there is enough bits remaining in the message to contain both */
      /* the EGPRS Window Size and Link Quality Measurement Mode */
      if ( MAX_NUM_OF_BITS_REMAINING(msg_pos) >=
           CSN_EGPRS_WINDOW_SIZE_LEN + CSN_LINK_QUALITY_MEASUREMENT_MODE_LEN )
      {
        dst->egprs_tbf_mode_flag = TRUE;
        dst->egprs_win_sz = gunpackb(src, &msg_pos, CSN_EGPRS_WINDOW_SIZE_LEN);
        dst->link_quality_measurement_mode =
          gunpackb(src, &msg_pos, CSN_LINK_QUALITY_MEASUREMENT_MODE_LEN);
      }
      else
      {
        return RR_TRUNCATED_MSG;
      }
    }

    /* The next bit indicates that Packet Extended Timing Advance is included */
    flag = GUNPACKS(src, &msg_pos, (word) 1);

    if ( flag )
    {
      /* Now check there is enough bits remaining in the message to contain the */
      /* Packet Extended Timing Advance */
      if ( MAX_NUM_OF_BITS_REMAINING(msg_pos) >= CSN_PACKET_EXT_TIMING_ADV_LEN )
      {
        dst->packet_ext_timing_adv_flag = TRUE;
        dst->packet_ext_timing_adv = gunpackb(src, &msg_pos, CSN_PACKET_EXT_TIMING_ADV_LEN);
      }
      else
      {
        return RR_TRUNCATED_MSG;
      }
    }
  }

#ifdef FEATURE_EDGE_CAPABILITY_NOT_SUPPORTED
  if (dst->egprs_tbf_mode_flag == TRUE)
  {
    /* RR Downlink Assignment contains unsupported EGPRS parameters */
    return NON_DISTRIBUTION_ERROR;
  }
#endif /* FEATURE_EDGE_CAPABILITY_NOT_SUPPORTED */

  return CSN_DECODE_NO_ERROR;
}

#endif /* FEATURE_GSM_DTM */

static void csn_decode_chan_desc(uint8 *src, uint16 *msg_pos, chan_desc_t *dst,
  const gas_id_t gas_id
)
{
 dst->chan_type_tdma_offset = gunpackb(src, msg_pos, CSN_CHAN_TYPE_TDMA_OFFSET_LEN);
 dst->tn = gunpackb(src, msg_pos, CSN_TN_LEN);
 dst->tsc = gunpackb(src, msg_pos, CSN_TSC_LEN);
 dst->hopping_flag = GUNPACKS(src, msg_pos, 1);
 if (dst->hopping_flag)
 {
  dst->maio = gunpackb(src, msg_pos, CSN_MAIO_LEN);
  dst->hsn = gunpackb(src, msg_pos, CSN_HSN_LEN);
 }
 else
 {
  (void)gunpackb(src, msg_pos, 1);
  CSN_DECODE_ARFCN(dst, msg_pos);
 }
}

static void csn_decode_group_call_info(
  uint8 *src,
  uint16 *msg_pos,
  group_call_info_t *dst,
  const gas_id_t gas_id
)
{
 int32 i;
 dst->group_call_ref = gunpackq(src, msg_pos, CSN_GROUP_CALL_REF_LEN);
 dst->group_chan_desc_flag = GUNPACKS(src, msg_pos, (word) 1);
 if (dst->group_chan_desc_flag)
 {
  csn_decode_chan_desc(src, msg_pos, &(dst->group_chan_desc.chan_desc), gas_id);
  dst->group_chan_desc.hopping_flag = GUNPACKS(src, msg_pos, (word) 1);
  if (dst->group_chan_desc.hopping_flag)
  {
   dst->group_chan_desc.ma_or_fsl = gunpackb(src, msg_pos, (word) 1) == 0 ? ma : fsl;
   if (dst->group_chan_desc.ma_or_fsl == ma)
   {
    dst->group_chan_desc.ma_length = gunpackb(src, msg_pos, 8);
    for (i=0; i<dst->group_chan_desc.ma_length; i++)
    {
     dst->group_chan_desc.ma[i] = gunpackb(src, msg_pos, 8);
    }
   }
   else /* dst->group_chan_desc.ma_or_fsl == fsl */
   {
    for (i=0; i<FSL_LENGTH_BYTES; i++)
    {
     dst->group_chan_desc.fsl[i] = gunpackb(src, msg_pos, 8);
    }
   }
  }
 }
}

static csn_decoder_error_t csn_decode_p1_rest_octets(
  uint8 *src,
  p1_rest_octets_t *dst,
  const gas_id_t gas_id
)
{
 uint16 msg_pos = 0;

 memset(dst, 0, sizeof(p1_rest_octets_t));
 dst->nln_flag = GUNPACKS_LH(src, &msg_pos);
 if (dst->nln_flag)
 {
  dst->nln_pch = gunpackb(src, &msg_pos, CSN_NLN_PCH_LEN);
  dst->nln_status = gunpackb(src, &msg_pos, CSN_NLN_STATUS_LEN);
 }
 dst->priority_1_flag = GUNPACKS_LH(src, &msg_pos);
 if (dst->priority_1_flag)
 {
  dst->priority_1 = gunpackb(src, &msg_pos, CSN_PRIORITY_LEN);
 }
 dst->priority_2_flag = GUNPACKS_LH(src, &msg_pos);
 if (dst->priority_2_flag)
 {
  dst->priority_2 = gunpackb(src, &msg_pos, CSN_PRIORITY_LEN);
 }
 dst->group_call_info_flag = GUNPACKS_LH(src, &msg_pos);
 if (dst->group_call_info_flag)
 {
  csn_decode_group_call_info(src, &msg_pos, &(dst->group_call_info), gas_id);
 }
 dst->packet_page_ind_1 = gunpackb_lh(src, &msg_pos) == 0 ? rr_paging : packet_paging;
 dst->packet_page_ind_2 = gunpackb_lh(src, &msg_pos) == 0 ? rr_paging : packet_paging;
 return CSN_DECODE_NO_ERROR;
}

static csn_decoder_error_t csn_decode_p2_rest_octets(uint8 *src, p2_rest_octets_t *dst)
{
 uint16 msg_pos = 0;

 memset(dst, 0, sizeof(p2_rest_octets_t));
 dst->cn3_flag = GUNPACKS_LH(src, &msg_pos);
 if (dst->cn3_flag)
 {
  dst->cn3 = gunpackb(src, &msg_pos, CSN_CN3_LEN);
 }
 dst->nln_flag = GUNPACKS_LH(src, &msg_pos);
 if (dst->nln_flag)
 {
  dst->nln = gunpackb(src, &msg_pos, CSN_NLN_LEN);
  dst->nln_status = gunpackb(src, &msg_pos, CSN_NLN_STATUS_LEN);
 }
 dst->priority_1_flag = GUNPACKS_LH(src, &msg_pos);
 if (dst->priority_1_flag)
 {
  dst->priority_1 = gunpackb(src, &msg_pos, CSN_PRIORITY_LEN);
 }
 dst->priority_2_flag = GUNPACKS_LH(src, &msg_pos);
 if (dst->priority_2_flag)
 {
  dst->priority_2 = gunpackb(src, &msg_pos, CSN_PRIORITY_LEN);
 }
 dst->priority_3_flag = GUNPACKS_LH(src, &msg_pos);
 if (dst->priority_3_flag)
 {
  dst->priority_3 = gunpackb(src, &msg_pos, CSN_PRIORITY_LEN);
 }
 dst->packet_page_ind_3 = gunpackb_lh(src, &msg_pos) == 0 ? rr_paging : packet_paging;
 return CSN_DECODE_NO_ERROR;
}

/*
Function: csn_decode_p3_rest_octets
*/
static csn_decoder_error_t csn_decode_p3_rest_octets(uint8 *src, p3_rest_octets_t *dst)
{
 uint16 msg_pos = 0;

 memset(dst, 0, sizeof(p3_rest_octets_t));
 dst->cn3_cn4_flag = GUNPACKS_LH(src, &msg_pos);
 if (dst->cn3_cn4_flag)
 {
  dst->cn3 = gunpackb(src, &msg_pos, CSN_CN3_LEN);
  dst->cn4 = gunpackb(src, &msg_pos, CSN_CN4_LEN);
 }
 dst->nln_flag = GUNPACKS_LH(src, &msg_pos);
 if (dst->nln_flag)
 {
  dst->nln = gunpackb(src, &msg_pos, CSN_NLN_LEN);
  dst->nln_status = gunpackb(src, &msg_pos, CSN_NLN_STATUS_LEN);
 }
 dst->priority_1_flag = GUNPACKS_LH(src, &msg_pos);
 if (dst->priority_1_flag)
 {
  dst->priority_1 = gunpackb(src, &msg_pos, CSN_PRIORITY_LEN);
 }
 dst->priority_2_flag = GUNPACKS_LH(src, &msg_pos);
 if (dst->priority_2_flag)
 {
  dst->priority_2 = gunpackb(src, &msg_pos, CSN_PRIORITY_LEN);
 }
 dst->priority_3_flag = GUNPACKS_LH(src, &msg_pos);
 if (dst->priority_3_flag)
 {
  dst->priority_3 = gunpackb(src, &msg_pos, CSN_PRIORITY_LEN);
 }
 dst->priority_4_flag = GUNPACKS_LH(src, &msg_pos);
 if (dst->priority_4_flag)
 {
  dst->priority_4 = gunpackb(src, &msg_pos, CSN_PRIORITY_LEN);
 }

 return CSN_DECODE_NO_ERROR;
}

static csn_decoder_error_t csn_decode_packet_channel_description(
  uint8 *src,
  packet_channel_description_t *dst,
  const gas_id_t gas_id
)
{
  uint16 msg_pos = 0;

  memset(dst, 0, sizeof(packet_channel_description_t));
  dst->channel_type = gunpackb(src, &msg_pos, CSN_CHANNEL_TYPE_LEN);
  dst->tn = gunpackb(src, &msg_pos, CSN_TN_LEN);
  dst->tsc = gunpackb(src, &msg_pos, CSN_TSC_LEN);
  if (gunpackb(src, &msg_pos, (word) 1))
  {
    dst->rf_channel_config = CONFIG_DIRECT_ENCODING;
    dst->maio = gunpackb(src, &msg_pos, CSN_MAIO_LEN);
    dst->hsn = gunpackb(src, &msg_pos, CSN_HSN_LEN);
  }
  else
  {
    if (gunpackb(src, &msg_pos, (word) 1))
    {
      dst->rf_channel_config = CONFIG_INDIRECT_ENCODING;
      msg_pos++;
      dst->maio = gunpackb(src, &msg_pos, CSN_MAIO_LEN);
      dst->ma_number_ind = gunpackb(src, &msg_pos, CSN_MA_NUMBER_IND_LEN);
      dst->change_mark_1_flag = GUNPACKS(src, &msg_pos, (word) 1);
      if (dst->change_mark_1_flag)
      {
       dst->change_mark_1 = gunpackb(src, &msg_pos, CSN_CHANGE_MARK_1_LEN);
      }
    }
    else
    {
      dst->rf_channel_config = CONFIG_NON_HOPPING;
      msg_pos++;
      CSN_DECODE_ARFCN(dst, &msg_pos);
    }
  }
  return CSN_DECODE_NO_ERROR;
}

/*===========================================================================
===
===  FUNCTION      csn_decode_ia_rest_octets (and friends)
===
===  DESCRIPTION
===
===    Decode CSN1 encoded IA Rest Octets.
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

static void csn_decode_ia_rest_octets_dynamic_alloc(uint8 *src, uint16 *msg_pos, ia_rest_octets_dynamic_alloc_t *dst)
{
 dst->usf = gunpackb(src, msg_pos, CSN_USF_LEN);
 dst->usf_granularity = gunpackb(src, msg_pos, CSN_USF_GRANULARITY_LEN);
 dst->po_pr_mode_flag = GUNPACKS(src, msg_pos, (word) 1);
 if (dst->po_pr_mode_flag)
 {
  dst->po = gunpackb(src, msg_pos, CSN_PO_LEN);
  dst->pr_mode = gunpackb(src, msg_pos, CSN_PR_MODE_LEN);
 }
}

static csn_decoder_error_t csn_decode_egprs_packet_uplink_ass(uint8 *src, uint16 *msg_pos, ia_rest_octets_egprs_packet_uplink_ass_t *dst)
{
 dst->extended_ra                                      = gunpackb(src, msg_pos, CSN_EXTENDED_RA_LEN);
 dst->access_tech_type_count                           = 0;
 while (gunpackb(src, msg_pos, (word) 1) && (dst->access_tech_type_count < MAX_ACCESS_TECH_TYPE))
 {
  dst->access_tech_type[dst->access_tech_type_count] = gunpackb(src, msg_pos, CSN_ACCESS_TECH_TYPE_LEN);
  dst->access_tech_type_count++;
 }
 dst->block_alloc_flag = gunpackb(src, msg_pos, (word) 1) == 1 ? BLOCK_ALLOC_FLAG_SINGLE : BLOCK_ALLOC_FLAG_MULTI;
 switch (dst->block_alloc_flag)
 {
 case BLOCK_ALLOC_FLAG_SINGLE:
  {
   ia_ro_egprs_pua_single_block_alloc_t *block_alloc = &(dst->block_alloc.single_block_alloc);
   block_alloc->tfi_assignment                       = gunpackb(src, msg_pos, CSN_TFI_ASSIGNMENT_LEN);
   block_alloc->polling                              = gunpackb(src, msg_pos, CSN_POLLING_LEN);
   block_alloc->alloc_flag                           = gunpackb(src, msg_pos, (word) 1) == 0 ? dynamic : fixed;
   if (block_alloc->alloc_flag == dynamic)
   {
    csn_decode_ia_rest_octets_dynamic_alloc(src, msg_pos, &(block_alloc->alloc.dynamic_alloc));
   }
   else /* dst->alloc_flag == fixed */
   {
     /* Fixed Alloc not suppported */
     return NON_DISTRIBUTION_ERROR;
   }
   block_alloc->egprs_channel_coding_command         = gunpackb(src, msg_pos, CSN_EGPRS_CHANNEL_CODING_COMMAND_LEN);
   block_alloc->tlli_block_channel_coding            = gunpackb(src, msg_pos, CSN_TLLI_BLOCK_CHANNEL_CODING_LEN);
   block_alloc->bep_period2_flag                     = GUNPACKS(src, msg_pos, (word) 1);
   if (block_alloc->bep_period2_flag)
   {
    block_alloc->bep_period2                      = gunpackb(src, msg_pos, CSN_BEP_PERIOD2_LEN);
   }
   block_alloc->resegment                            = gunpackb(src, msg_pos, CSN_RESEGMENT_LEN);
   block_alloc->egprs_window_size                    = gunpackb(src, msg_pos, CSN_EGPRS_WINDOW_SIZE_LEN);
   block_alloc->alpha_flag                           = GUNPACKS(src, msg_pos, (word) 1);
   if (block_alloc->alpha_flag)
   {
    block_alloc->alpha                            = gunpackb(src, msg_pos, CSN_ALPHA_LEN);
   }
   block_alloc->gamma                                = gunpackb(src, msg_pos, CSN_GAMMA_LEN);
   block_alloc->timing_advance_index_flag            = GUNPACKS(src, msg_pos, (word) 1);
   if (block_alloc->timing_advance_index_flag)
   {
    block_alloc->timing_advance_index             = gunpackb(src, msg_pos, CSN_TIMING_ADVANCE_INDEX_LEN);
   }
   block_alloc->tbf_starting_time_flag               = GUNPACKS(src, msg_pos, (word) 1);
   if (block_alloc->tbf_starting_time_flag)
   {
    block_alloc->tbf_starting_time                = gunpackw(src, msg_pos, CSN_TBF_STARTING_TIME_LEN);
   }
   break;
  }
 case BLOCK_ALLOC_FLAG_MULTI:
  {
   ia_ro_egprs_pua_multi_block_alloc_t *block_alloc  = &(dst->block_alloc.multi_block_alloc);
   block_alloc->alpha_flag                           = GUNPACKS(src, msg_pos, (word) 1);
   if (block_alloc->alpha_flag)
   {
    block_alloc->alpha                            = gunpackb(src, msg_pos, CSN_ALPHA_LEN);
   }
   block_alloc->gamma                                = gunpackb(src, msg_pos, CSN_GAMMA_LEN);
   block_alloc->tbf_starting_time                    = gunpackw(src, msg_pos, CSN_TBF_STARTING_TIME_LEN);
   block_alloc->num_of_radio_blocks_allocated        = gunpackb(src, msg_pos, CSN_NUM_OF_RADIO_BLOCKS_ALLOCATED_LEN);
   block_alloc->dwnlk_pwr_ctrl_flag                  = GUNPACKS(src, msg_pos, (word) 1);
   if (block_alloc->dwnlk_pwr_ctrl_flag)
   {
    block_alloc->dwnlk_pwr_ctrl.po                = gunpackb(src, msg_pos, CSN_PO_LEN);
    block_alloc->dwnlk_pwr_ctrl.bts_pwr_ctrl_mode = gunpackb(src, msg_pos, CSN_BTS_PWR_CTRL_MODE_LEN);
    block_alloc->dwnlk_pwr_ctrl.pr_mode           = gunpackb(src, msg_pos, CSN_PR_MODE_LEN);
   }
   break;
  }
 }
 return CSN_DECODE_NO_ERROR;
}

static csn_decoder_error_t csn_decode_packet_uplink_ass(uint8 *src, uint16 *msg_pos, ia_rest_octets_packet_uplink_ass_t *dst)
{
 dst->block_alloc_flag = gunpackb(src, msg_pos, (word) 1) == 1 ? BLOCK_ALLOC_FLAG_MULTI : BLOCK_ALLOC_FLAG_SINGLE;
 if (dst->block_alloc_flag == BLOCK_ALLOC_FLAG_MULTI)
 {
  ia_multi_block_alloc_t * block_alloc = &(dst->block_alloc.multi_block_alloc);
  block_alloc->tfi_assignment = gunpackb(src, msg_pos, CSN_TFI_ASSIGNMENT_LEN);
  block_alloc->polling = gunpackb(src, msg_pos, CSN_POLLING_LEN);
  block_alloc->alloc_flag = gunpackb(src, msg_pos, (word) 1) == 0 ? dynamic : fixed;
  if (block_alloc->alloc_flag == dynamic)
  {
   csn_decode_ia_rest_octets_dynamic_alloc(src, msg_pos, &(block_alloc->alloc.dynamic_alloc));
  }
  else
  {
    /* Fixed Allocation not supported */
    return NON_DISTRIBUTION_ERROR;
  }
  block_alloc->channel_coding_command = gunpackb(src, msg_pos, CSN_CHANNEL_CODING_COMMAND_LEN);
  block_alloc->tlli_block_channel_coding = gunpackb(src, msg_pos, CSN_TLLI_BLOCK_CHANNEL_CODING_LEN);
  block_alloc->alpha_flag = GUNPACKS(src, msg_pos, (word) 1);
  if (block_alloc->alpha_flag)
  {
   block_alloc->alpha = gunpackb(src, msg_pos, CSN_ALPHA_LEN);
  }
  block_alloc->gamma = gunpackb(src, msg_pos, CSN_GAMMA_LEN);
  block_alloc->timing_advance_index_flag = GUNPACKS(src, msg_pos, (word) 1);
  if (block_alloc->timing_advance_index_flag)
  {
   block_alloc->timing_advance_index = gunpackb(src, msg_pos, CSN_TIMING_ADVANCE_INDEX_LEN);
  }
  block_alloc->tbf_starting_time_flag = GUNPACKS(src, msg_pos, (word) 1);
  if (block_alloc->tbf_starting_time_flag)
  {
   block_alloc->tbf_starting_time = gunpackw(src, msg_pos, CSN_TBF_STARTING_TIME_LEN);
  }
 }
 else /* dst->block_alloc == BLOCK_ALLOC_FLAG_SINGLE */
 {
  ia_single_block_alloc_t *block_alloc = &(dst->block_alloc.single_block_alloc);
  block_alloc->alpha_flag = GUNPACKS(src, msg_pos, (word) 1);
  if (block_alloc->alpha_flag)
  {
   block_alloc->alpha = gunpackb(src, msg_pos, CSN_ALPHA_LEN);
  }
  block_alloc->gamma = gunpackb(src, msg_pos, CSN_GAMMA_LEN);
  (void)gunpackb(src, msg_pos, (word) 1);
  (void)gunpackb(src, msg_pos, (word) 1);
  block_alloc->tbf_starting_time = gunpackw(src, msg_pos, CSN_TBF_STARTING_TIME_LEN);
  block_alloc->dwnlk_pwr_ctrl_flag = GUNPACKS_LH(src, msg_pos);
  if (block_alloc->dwnlk_pwr_ctrl_flag)
  {
   dwnlk_pwr_ctrl_t *dwnlk_pwr_ctrl = &(block_alloc->dwnlk_pwr_ctrl);
   dwnlk_pwr_ctrl->po = gunpackb(src, msg_pos, CSN_PO_LEN);
   dwnlk_pwr_ctrl->bts_pwr_ctrl_mode = gunpackb(src, msg_pos, CSN_BTS_PWR_CTRL_MODE_LEN);
   dwnlk_pwr_ctrl->pr_mode = gunpackb(src, msg_pos, CSN_PR_MODE_LEN);
  }
 }
 /*
 R97 messages end here. We have to take this into account. There's no indication
 in the message as to whether or not R99 info is included so, from here on, we have to check
 that there is sufficient room left in the IE BEFORE we attempt to read any more info (there
 is an 11 byte upper limit on the length of the IE). We just have to hope that the networks
 correctly pad out the messages with zeros.
 */
 if (((MAX_IA_RO_LENGTH_BITS) - *msg_pos) >= MIN_IA_RO_PUA_R99_ADDITIONS_LENGTH)
 {
  if (gunpackb_lh(src, msg_pos))
  {
   dst->extended_ra_flag = GUNPACKS(src, msg_pos, (word) 1);
   if (dst->extended_ra_flag)
   {
    if ((MAX_IA_RO_LENGTH_BITS - *msg_pos) >= CSN_EXTENDED_RA_LEN)
    {
     dst->extended_ra = gunpackb(src, msg_pos, CSN_EXTENDED_RA_LEN);
    }
    else
    {
     dst->extended_ra_flag = FALSE;
     return RR_TRUNCATED_MSG;
    }
   }
  }
 }
 else
 {
  dst->extended_ra_flag = FALSE;
 }
 return CSN_DECODE_NO_ERROR;
}

static csn_decoder_error_t csn_decode_packet_downlink_ass(uint8 *src, uint16 *msg_pos, ia_rest_octets_packet_downlink_ass_t *dst)
{
 dst->tlli = gunpackd(src, msg_pos, CSN_TLLI_LEN);
 dst->tfi_assignment_flag = GUNPACKS(src, msg_pos, (word) 1);
 if (dst->tfi_assignment_flag == TRUE)
 {
  dst->tfi_assignment = gunpackb(src, msg_pos, CSN_TFI_ASSIGNMENT_LEN);
  dst->rlc_mode = gunpackb(src, msg_pos, CSN_RLC_MODE_LEN);
  dst->alpha_flag = GUNPACKS(src, msg_pos, (word) 1);
  if (dst->alpha_flag == TRUE)
  {
   dst->alpha = gunpackb(src, msg_pos, CSN_ALPHA_LEN);
  }
  dst->gamma = gunpackb(src, msg_pos, CSN_GAMMA_LEN);
  dst->polling = gunpackb(src, msg_pos, CSN_POLLING_LEN);
  dst->ta_valid = gunpackb(src, msg_pos, CSN_TA_VALID_LEN);
 }
 dst->timing_advance_index_flag = GUNPACKS(src, msg_pos, (word) 1);
 if (dst->timing_advance_index_flag)
 {
  dst->timing_advance_index = gunpackb(src, msg_pos, CSN_TIMING_ADVANCE_INDEX_LEN);
 }
 dst->tbf_starting_time_flag = GUNPACKS(src, msg_pos, (word) 1);
 if (dst->tbf_starting_time_flag)
 {
  dst->tbf_starting_time = gunpackw(src, msg_pos, CSN_TBF_STARTING_TIME_LEN);
 }
 dst->dwnlk_pwr_ctrl_flag = GUNPACKS(src, msg_pos, (word) 1);
 if (dst->dwnlk_pwr_ctrl_flag)
 {
  dst->dwnlk_pwr_ctrl.po = gunpackb(src, msg_pos, CSN_PO_LEN);
  dst->dwnlk_pwr_ctrl.bts_pwr_ctrl_mode = gunpackb(src, msg_pos, CSN_BTS_PWR_CTRL_MODE_LEN);
  dst->dwnlk_pwr_ctrl.pr_mode = gunpackb(src, msg_pos, CSN_PR_MODE_LEN);
 }
 /*
 It's the same issue with R97/99 messages here again (see above for details)
 */
 if ((MAX_IA_RO_LENGTH_BITS - *msg_pos) >= MIN_IA_RO_PDA_R99_ADDITIONS_LENGTH)
 {
  dst->egprs_tbf_mode_flag = GUNPACKS_LH(src, msg_pos);
  if (dst->egprs_tbf_mode_flag)
  {
   dst->egprs_window_size = gunpackb(src, msg_pos, CSN_EGPRS_WINDOW_SIZE_LEN);
   dst->link_quality_measurement_mode = gunpackb(src, msg_pos, CSN_LINK_QUALITY_MEASUREMENT_MODE_LEN);
   dst->bep_period2_flag = GUNPACKS(src, msg_pos, (word) 1);
   if (dst->bep_period2_flag)
   {
    if ((MAX_IA_RO_LENGTH_BITS - *msg_pos) >= CSN_BEP_PERIOD2_LEN)
    {
     dst->bep_period2 = gunpackb(src, msg_pos, CSN_BEP_PERIOD2_LEN);
    }
    else
    {
     dst->bep_period2_flag = FALSE;
     return RR_TRUNCATED_MSG;
    }
   }
  }
 }
 else
 {
  dst->egprs_tbf_mode_flag = FALSE;
 }
 return CSN_DECODE_NO_ERROR;
}

static csn_decoder_error_t csn_decode_second_part_packet_ass(uint8 *src, uint16 *msg_pos, ia_rest_octets_second_part_packet_ass_t *dst)
{
 dst->additions_for_r99 = GUNPACKS_LH(src, msg_pos);
 if (dst->additions_for_r99)
 {
  dst->extended_ra_flag = GUNPACKS(src, msg_pos, (word) 1);
  if (dst->extended_ra_flag)
  {
   dst->extended_ra = gunpackb(src, msg_pos, CSN_EXTENDED_RA_LEN);
  }
 }
 return CSN_DECODE_NO_ERROR;
}

static csn_decoder_error_t csn_decode_iar_rest_octets(uint8 *src, iar_rest_octets_t *dst)
{
  uint8  i;
  uint16 msg_pos = 0;

  memset(dst, 0, sizeof(iar_rest_octets_t));
  for ( i=0; i<NUM_OF_EXT_RA; i++ )
  {
    if ( gunpackb( src, &msg_pos, (word) 1) == 1 )
    {
      dst->ext_ra[i].ext_ra_present = TRUE;
      dst->ext_ra[i].ext_ra = gunpackb(src,&msg_pos, CSN_EXT_RA_LEN);
    }
    else
    {
      dst->ext_ra[i].ext_ra_present = FALSE;
    }
  }

  return CSN_DECODE_NO_ERROR;
}

/* See 3GPP 44.018 section 10.5.2.16 */
static csn_decoder_error_t csn_decode_ia_rest_octets(uint8 *src, ia_rest_octets_t *dst)
{
#define LL 0
#define LH 1
#define HL 2
#define HH 3

 csn_decoder_error_t retval;
 uint16              msg_pos = 0;
 uint8               lh_type;
 uint8               i;

 /* initialise the output structure */
 memset(dst, 0, sizeof(ia_rest_octets_t));
 dst->contents_type = ia_rest_octets_unknown;

 lh_type = ((gunpackb_lh(src, &msg_pos) << 1) & 0xFF) | gunpackb_lh(src, &msg_pos);

 switch(lh_type)
 {
 case LL:
  /* handler for inter-RAT compressed mode handover field should go here */
  dst->contents_type = compressed_interRAT_ho_info;
  /*
   * L A compressed version of the INTER RAT HANDOVER INFO message shall not be used;
   * H A compressed version of the INTER RAT HANDOVER INFO message shall be used.
   */
  dst->contents.compressed_irat_ho_info = GUNPACKS_LH(src, &msg_pos);
  retval = CSN_DECODE_NO_ERROR;
  break;

 case LH:
  if (gunpackb(src, &msg_pos, (word) 1) == 0)
  {
   if (gunpackb(src, &msg_pos, (word) 1) == 0)
   {
    dst->contents_type = egprs_packet_uplink_ass;
    retval = csn_decode_egprs_packet_uplink_ass(src, &msg_pos, &(dst->contents.egprs_packet_uplink_ass));
   }
   else
   {
     /* Multiple Blocks Packet Downlink Assignment - not handled at present */
    retval = UNKNOWN_MESSAGE_TYPE;
   }
  }
  else
  {
   /* Reserved for future use */
   retval = IGNORE_ERROR;
  }
  break;

 case HL:
  dst->contents_type = freq_params_before_time;
  dst->length_of_freq_params = gunpackb(src, &msg_pos, CSN_LENGTH_OF_FREQ_PARAMS_LEN);
  if (dst->length_of_freq_params)
  {
   msg_pos++;
   msg_pos++;
   dst->contents.freq_params_before_time.maio = gunpackb(src, &msg_pos, CSN_MAIO_LEN);
   dst->contents.freq_params_before_time.mobile_allocation_length = dst->length_of_freq_params - 1;
   for (i=0; i<dst->contents.freq_params_before_time.mobile_allocation_length; i++)
   {
    dst->contents.freq_params_before_time.mobile_allocation[i] = gunpackb(src, &msg_pos, sizeof(uint8));
   }
  }
  /* inter-RAT compressed mode handover field  */
  dst->contents.freq_params_before_time.compressed_irat_ho_info = GUNPACKS_LH(src, &msg_pos);
  retval  = CSN_DECODE_NO_ERROR;
  break;

 case HH:
  if (gunpackb(src, &msg_pos, (word) 1))
  {
   dst->contents_type = second_part_packet_ass;
   retval = csn_decode_second_part_packet_ass(src, &msg_pos, &(dst->contents.second_part_packet_ass));
  }
  else
  {
   if (gunpackb(src, &msg_pos, (word) 1))
   {
    dst->contents_type = packet_downlink_ass;
    retval = csn_decode_packet_downlink_ass(src, &msg_pos, &(dst->contents.packet_downlink_ass));
   }
   else
   {
    dst->contents_type = packet_uplink_ass;
    retval = csn_decode_packet_uplink_ass(src, &msg_pos, &(dst->contents.packet_uplink_ass));
   }
  }
  break;

 default:
  /* this code should be unreachable */
  retval = RR_TRUNCATED_MSG;
  break;
 }

 return retval;
}

static csn_decoder_error_t csn_decode_si9_rest_octets(uint8 *src, si9_rest_octets_t *dst)
{
 uint16 msg_pos = 0;
 position_t *position_ptr;
 scheduling_info_t *sched_ptr;

 memset(dst, 0, sizeof(si9_rest_octets_t));
 dst->scheduling_info_flag = GUNPACKS(src, &msg_pos, (word) 1);
 if (dst->scheduling_info_flag)
 {
  dst->scheduling_info_count = 0;
  do {
   sched_ptr = &dst->scheduling_info[dst->scheduling_info_count];
   /* get the info type */
   if (gunpackb(src, &msg_pos, (word) 1))
   {
    if (gunpackb(src, &msg_pos, (word) 1))
    {
     sched_ptr->info_type_flag = info_type_6;
     sched_ptr->info_type = gunpackb(src, &msg_pos, CSN_INFO_TYPE_6_LEN);
    }
    else
    {
     sched_ptr->info_type_flag = info_type_5;
     sched_ptr->info_type = gunpackb(src, &msg_pos, CSN_INFO_TYPE_5_LEN);
    }
   }
   else
   {
    sched_ptr->info_type_flag = info_type_4;
    sched_ptr->info_type = gunpackb(src, &msg_pos, CSN_INFO_TYPE_4_LEN);
   }
   /* get the positions */
   sched_ptr->position_count = 0;
   do {
    position_ptr = &sched_ptr->positions[sched_ptr->position_count];
    position_ptr->modulus = gunpackb(src, &msg_pos, CSN_MODULUS_LEN);
    if (position_ptr->modulus)
    {
     position_ptr->relative_position = gunpackw(src, &msg_pos, (word) (position_ptr->modulus + 1));
    }
    position_ptr->bcch_type = gunpackb(src, &msg_pos, CSN_BCCH_TYPE_LEN);
    sched_ptr->position_count++;
   } while (gunpackb(src, &msg_pos, (word) 1) && (sched_ptr->position_count < MAX_POSITIONS));
   dst->scheduling_info_count++;
  } while (gunpackb(src, &msg_pos, (word) 1) && dst->scheduling_info_count < MAX_SCHEDULING_INFO);
 }
 return CSN_DECODE_NO_ERROR;
}

static csn_decoder_error_t csn_decode_container_repetition_struct(uint8 src[], uint16 *pos_ptr, container_repetition_struct_t *msg_ptr)
{
  uint8 repeat_struct = 1;
  uint8 num_bits_left;
  uint8 next_octet;
  uint8 cd_length;
  uint8 pd;

  msg_ptr->num_data_structs = 0;
  msg_ptr->last_struct      = FALSE;

  /* container repetition structures */
  while (repeat_struct)
  {
    container_data_t *data_ptr;

    pd = gunpackb(src, pos_ptr, CSN_PD_LEN);
    cd_length = gunpackb(src, pos_ptr, CSN_CD_LENGTH_LEN);

    /* only process repetition structs with at least one octet */
    if (cd_length)
    {
      if (msg_ptr->num_data_structs >= ARR_SIZE(msg_ptr->data_structs) )
        break;
      data_ptr = &(msg_ptr->data_structs[msg_ptr->num_data_structs]);
    }
    else
    {
      /* CD LENGTH of zero indicates the last repeat structure */
      data_ptr = NULL;
      repeat_struct = 0;
      msg_ptr->last_struct = TRUE;
    }

    if (data_ptr)
    {
      data_ptr->pd        = pd;
      data_ptr->cd_length = cd_length; /* always store the decoded CD LENGTH */
      memset(data_ptr->data, 0x2B, MAX_CONTAINER_DATA_OCTETS);

      if (cd_length == CONTAINER_DATA_FILLS_MSG)
      {
        uint8 num_octets_left;

        /* rest of this msg contains one repeat struct */
        /* determine number of whole octets left */
        num_bits_left   = (uint8)(MAX_DECODE_BITS - (*pos_ptr));
        num_octets_left = (num_bits_left >> 3);

        /* extract the whole octets first, up to a maximum amount */
        if (num_octets_left > MAX_CONTAINER_DATA_OCTETS)
        {
          num_octets_left = MAX_CONTAINER_DATA_OCTETS;
        }

        /* "exact length" is number of data octets physically stored */
        data_ptr->ex_length = num_octets_left;

        next_octet = 0;
        while (num_octets_left)
        {
          data_ptr->data[next_octet] = gunpackb(src, pos_ptr, 8);
          next_octet++;
          num_octets_left--;
        }

        /* set the flag to indicate that this is the last structure, any */
        /* subsequent zero length structure will be absorbed into this one */
        repeat_struct = 0;
      }
      else
      {
        /* normal case, we have a length indicator and a whole number of octets left */
        if (cd_length > MAX_CONTAINER_DATA_OCTETS)
        {
          cd_length = MAX_CONTAINER_DATA_OCTETS;
        }

        /* exact length is number of data octets physically stored */
        data_ptr->ex_length = 0;

        /* double check that decoder doesn't run off the end of the message */
        num_bits_left = (uint8)(MAX_DECODE_BITS - (*pos_ptr));

        for (next_octet = 0; next_octet < cd_length; next_octet++)
        {
          if (num_bits_left >= 8)
          {
            data_ptr->data[next_octet] = gunpackb(src, pos_ptr, 8);
            num_bits_left-=8;
            data_ptr->ex_length++;
          }
        }

        if (cd_length > data_ptr->ex_length)
        {
          /* declared length is too long */
          /* try and error correct */
          data_ptr->cd_length = CONTAINER_DATA_FILLS_MSG;
        }

        /* check for sufficient bits for next loop iteration */
        if (num_bits_left < (CSN_PD_LEN + CSN_CD_LENGTH_LEN))
        {
          repeat_struct = 0;
        }
      }

      /* only store containers for TRUE protocol discriminators */
      if (pd == BCCH_PD)
      {
        /* increment the count of octets and structs stored from this message  */
        /* note that separate counts are maintained per protocol discriminator */
        msg_ptr->octet_counts[pd]  += data_ptr->ex_length;
        if (data_ptr->cd_length != CONTAINER_DATA_FILLS_MSG)
        {
          msg_ptr->struct_counts[pd]++;
        }

        /* increment count of TRUE data structures in the decoder output */
        msg_ptr->num_data_structs++;
      }
    }
  }

  return(CSN_DECODE_NO_ERROR);
}


static csn_decoder_error_t csn_decode_packet_ncell_data(
  csn_decode_memory_instance_t *mem,
  uint8 src[],
  uint16 *pos_ptr,
  packet_ncell_data_t *msg_ptr,
  const gas_id_t gas_id
)
{
  CSN_DECODE_MEMORY_IN_USE(mem);

  memset(msg_ptr, 0, sizeof(packet_ncell_data_t));

  (void)gunpackb(src, pos_ptr, 1); /* spare bit, should be 0 */

  csn_global_tfi_ie(src, pos_ptr, &(msg_ptr->global_tfi));

  msg_ptr->container_id = gunpackb(src, pos_ptr, CSN_CONTAINER_ID_LEN);

  (void)gunpackb(src, pos_ptr, 1); /* spare bit */

  msg_ptr->container_index = gunpackb(src, pos_ptr, CSN_CONTAINER_INDEX_LEN);

  msg_ptr->arfcn_bsic_included = gunpackb(src, pos_ptr, 1);

  if (msg_ptr->arfcn_bsic_included)
  {
    CSN_DECODE_ARFCN(msg_ptr, pos_ptr);
    msg_ptr->bsic = gunpackb(src, pos_ptr, 6);
  }

  return(csn_decode_container_repetition_struct(src, pos_ptr, &(msg_ptr->container_repetition_struct)));
}

static csn_decoder_error_t csn_decode_packet_cell_change_continue(
  csn_decode_memory_instance_t *mem,
  uint8 src[],
  uint16 *pos_ptr,
  packet_cell_change_continue_t *msg_ptr,
  const gas_id_t gas_id
)
{
  CSN_DECODE_MEMORY_IN_USE(mem);

  memset(msg_ptr, 0, sizeof(packet_cell_change_continue_t));

  (void)gunpackb(src, pos_ptr, 1); /* should be 0 */

  csn_global_tfi_ie(src, pos_ptr, &(msg_ptr->global_tfi));

  msg_ptr->data_valid = (boolean)gunpackb(src, pos_ptr, 1);
  if (msg_ptr->data_valid)
  {
    CSN_DECODE_ARFCN(msg_ptr, pos_ptr);
    msg_ptr->bsic = gunpackb(src, pos_ptr, 6);
    msg_ptr->container_id = gunpackb(src, pos_ptr, CSN_CONTAINER_ID_LEN);
  }

  return CSN_DECODE_NO_ERROR;
}


static csn_decoder_error_t csn_decode_packet_scell_data(
  csn_decode_memory_instance_t *mem,
  uint8 src[],
  uint16 *pos_ptr,
  packet_scell_data_t *msg_ptr,
  const gas_id_t gas_id
)
{
  CSN_DECODE_MEMORY_IN_USE(mem);

  memset(msg_ptr, 0, sizeof(packet_scell_data_t));

  (void)gunpackb(src, pos_ptr, 1); /* spare bit, should be 0 */

  csn_global_tfi_ie(src, pos_ptr, &(msg_ptr->global_tfi));

  (void)gunpackb(src, pos_ptr, 4); /* spare bits */

  msg_ptr->container_index = gunpackb(src, pos_ptr, CSN_CONTAINER_INDEX_LEN);

  return(csn_decode_container_repetition_struct(src, pos_ptr, &(msg_ptr->container_repetition_struct)));
}

static csn_decoder_error_t csn_decode_packet_scell_si(
  csn_decode_memory_instance_t *mem,
  uint8 src[],
  uint16 *pos_ptr,
  packet_serving_cell_si_t *msg_ptr,
  const gas_id_t gas_id
)
{
  uint8 num_octets;
  uint8 next_octet;
  uint8 bits_left;
  uint8 *octet_ptr;

  CSN_DECODE_MEMORY_IN_USE(mem);

  bits_left  = (uint8)(MAX_DECODE_BITS - (*pos_ptr));
  num_octets = (bits_left >> 3);

  octet_ptr = msg_ptr->container_data;
  memset(octet_ptr, 0x2B, CSN_CONTAINER_DATA_LEN);

  if (num_octets > (CSN_CONTAINER_DATA_LEN - 2))
  {
    return NO_MEMORY;
  }

  *octet_ptr = (uint8)(num_octets << 2) | 0x01; /* L2 length */
  octet_ptr++;
  *octet_ptr = 0x06;                     /* RR PD and skip */
  octet_ptr++;

  for (next_octet = 0; next_octet < num_octets; next_octet++, octet_ptr++)
  {
    *octet_ptr = gunpackb(src, pos_ptr, 8);
  }

  return CSN_DECODE_NO_ERROR;
}

static csn_decoder_error_t csn_decode_pkt_cs_command(
  csn_decode_memory_instance_t *mem,
  uint8 *src,
  uint16 *pos_ptr,
  packet_cs_command_t *msg_ptr,
  const gas_id_t gas_id
)
{
  uint8 num_octets;

  CSN_DECODE_MEMORY_IN_USE(mem);

  memset(msg_ptr, 0, sizeof(packet_cs_command_t));

  /* Extract the Global TFI */
  csn_global_tfi_ie(src, pos_ptr, &(msg_ptr->global_tfi));

  /* Extract spare bits */
 (void)gunpackb(src, pos_ptr, 2);

  /* Extract the container length */
  num_octets = gunpackb(src, pos_ptr, CSN_CONTAINER_LENGTH_LEN);
  if (num_octets > CSN_PKT_CS_COMMAND_MAX_OCTETS)
  {
    return NON_DISTRIBUTION_ERROR;
  }

  /* Extract the encapsulated message and add a L3 header */
  if (num_octets != 0)
  {
    uint8 *data_ptr = msg_ptr->container_data;

    /* update the output structure with the number of octets */
    msg_ptr->container_length = num_octets + 1;

    /* insert the header octet at the start of the buffer */
    *data_ptr = 0x06; /* Radio Resources Protocol Discriminator and Skip Indicator */
    data_ptr++;

    /* now extract the remaining octets from the received message */
    while (num_octets)
    {
      *data_ptr = gunpackb(src, pos_ptr, 8);
      data_ptr++;
      num_octets--;
    }
  }

  return CSN_DECODE_NO_ERROR;
}

static csn_decoder_error_t csn_decode_packet_si_data(
  csn_decode_memory_instance_t *mem,
  uint8 src[],
  uint16 *pos_ptr,
  packet_si_data_t *msg_ptr,
  const gas_id_t gas_id
)
{
  uint8 num_octets;
  uint8 i;

  CSN_DECODE_MEMORY_IN_USE(mem);

  memset(msg_ptr, 0x2B, sizeof(packet_si_data_t));

  /* length should have been loaded into the 2nd octet (i.e. after the msg type) */
  num_octets = gunpackb(src, pos_ptr, 8);
  if (num_octets > CSN_CONTAINER_DATA_LEN)
  {
    num_octets = CSN_CONTAINER_DATA_LEN;
  }

  for (i = 0; i < num_octets; i++)
  {
    msg_ptr->container_data[i] = gunpackb(src, pos_ptr, 8);
  }

  return CSN_DECODE_NO_ERROR;
}

#ifdef FEATURE_GSM_DTM

static csn_decoder_error_t csn_decode_gprs_broadcast_information(uint8 *src, gprs_broadcast_information_t *dst)
{
  uint16 msg_pos = 0;

  /* Initialise the output structure */
  memset(dst, 0, sizeof(gprs_broadcast_information_t));

  /* Extract GPRS Cell Options */
  csn_gprs_cell_options_ie(src, &msg_pos, &(dst->gprs_cell_options));

  /* Extract GPRS Power Control Parameters */
  csn_gprs_power_control_params_ie(src, &msg_pos, &(dst->gprs_power_control_params));

  return CSN_DECODE_NO_ERROR;

} /* end csn_decode_gprs_broadcast_information() */

static csn_decoder_error_t csn_decode_dtm_information_details(uint8 *src, dtm_information_details_t *dst)
{
  uint16 msg_pos = 0;

  /* Initialise the output structure */
  memset(dst, 0, sizeof(dtm_information_details_t));

  /*  Extract MAX_LAPDm */
  dst->max_LAPDm = gunpackb(src, &msg_pos, CSN_MAX_LAPDM_LEN);

  /*  Extract GPRS_MS_TXPWR_MAX_CCH */
  dst->gprs_ms_txpwr_max_cch = gunpackb(src, &msg_pos, CSN_GPRS_MS_TXPWR_MAX_CCH_LEN);

  /*  Extract Cell Identity */
  dst->cell_identity = gunpackw(src, &msg_pos, CSN_CELL_IDENTITY_LEN);

  return CSN_DECODE_NO_ERROR;

} /* end csn_decode_dtm_information_details() */

#ifdef FEATURE_GSM_EDTM

static void csn_decode_dl_tbf_assignment(uint8 *src, uint16 *pos_ptr, dl_tbf_assign_t *msg_ptr)
{
  msg_ptr->pfi_present = (boolean)gunpackb(src, pos_ptr, 1);
  if (msg_ptr->pfi_present == TRUE)
  {
    msg_ptr->pfi = gunpackb(src, pos_ptr, CSN_PFI_LEN);
  }

  msg_ptr->downlink_rlc_mode = gunpackb(src, pos_ptr, 1);

  msg_ptr->uplink_control_timeslot_present = (boolean)gunpackb(src, pos_ptr, 1);
  if (msg_ptr->uplink_control_timeslot_present == TRUE)
  {
    msg_ptr->uplink_control_timeslot = gunpackb(src, pos_ptr, CSN_UPLINK_CONTROL_TIMESLOT_LEN);
  }

  msg_ptr->tfi_assignment = gunpackb(src, pos_ptr, CSN_TFI_ASSIGNMENT_LEN);

  msg_ptr->control_ack = gunpackb(src, pos_ptr, 1);

  msg_ptr->dl_egprs_window_size_present = (boolean)gunpackb(src, pos_ptr, 1);
  if (msg_ptr->dl_egprs_window_size_present == TRUE)
  {
    msg_ptr->dl_egprs_window_size = gunpackb(src, pos_ptr, CSN_EGPRS_WINDOW_SIZE_LEN);
  }
}

static void csn_decode_multiple_downlink_assignment(uint8 *src, uint16 *pos_ptr, multiple_dl_assign_t *msg_ptr)
{
  dl_tbf_assign_t *dl_assign_ptr;
  dl_tbf_assign_t  dummy_tbf;
  uint8            num_dl_tbfs = 0;

  msg_ptr->timeslot_allocation = gunpackb(src, pos_ptr, CSN_TIMESLOT_ALLOC_LEN);

  msg_ptr->uplink_control_timeslot_present = (boolean)gunpackb(src, pos_ptr, 1);
  if (msg_ptr->uplink_control_timeslot_present == TRUE)
  {
    msg_ptr->uplink_control_timeslot = gunpackb(src, pos_ptr, CSN_UPLINK_CONTROL_TIMESLOT_LEN);
  }

  while (gunpackb(src, pos_ptr, 1) == 1)
  {
    if (num_dl_tbfs < MAX_NUM_DL_TBF_ASSIGNMENTS)
    {
      dl_assign_ptr = &(msg_ptr->dl_tbf_assignments[num_dl_tbfs]);
      num_dl_tbfs++;
    }
    else
    {
      /* if there are further structures that exceed our internal */
      /* storage limits, just soak them up into a dummy variable  */
      dl_assign_ptr = &dummy_tbf;
      MSG_GERAN_ERROR_0("WARNING: ignoring subsequent DL TBF assignments");
    }

    csn_decode_dl_tbf_assignment(src, pos_ptr, dl_assign_ptr);
  }

  msg_ptr->num_dl_tbf_assignments = num_dl_tbfs;
}

static void csn_decode_ul_tbf_assignment(uint8 *src, uint16 *pos_ptr, ul_tbf_assign_t *msg_ptr, uint8 num_slots)
{
  uint8 m_slots;

  msg_ptr->num_timeslots = num_slots;

  msg_ptr->pfi_present = (boolean)gunpackb(src, pos_ptr, 1);
  if (msg_ptr->pfi_present == TRUE)
  {
    msg_ptr->pfi = gunpackb(src, pos_ptr, CSN_PFI_LEN);
  }

  msg_ptr->tfi_assignment = gunpackb(src, pos_ptr, CSN_TFI_ASSIGNMENT_LEN);

  msg_ptr->channel_coding_present = (boolean)gunpackb(src, pos_ptr, 1);
  if (msg_ptr->channel_coding_present == TRUE)
  {
    msg_ptr->channel_coding_command = gunpackb(src, pos_ptr, CSN_CHANNEL_CODING_COMMAND_LEN);
  }

  msg_ptr->egprs_channel_coding_present = (boolean)gunpackb(src, pos_ptr, 1);
  if (msg_ptr->egprs_channel_coding_present == TRUE)
  {
    msg_ptr->egprs_channel_coding_command = gunpackb(src, pos_ptr, CSN_EGPRS_CHANNEL_CODING_COMMAND_LEN);
  }

  msg_ptr->ul_egprs_window_size_present = (boolean)gunpackb(src, pos_ptr, 1);
  if (msg_ptr->ul_egprs_window_size_present == TRUE)
  {
    msg_ptr->ul_egprs_window_size = gunpackb(src, pos_ptr, CSN_EGPRS_WINDOW_SIZE_LEN);
  }

  msg_ptr->usf_granularity = gunpackb(src, pos_ptr, 1);

  msg_ptr->timeslot_allocation_present = (boolean)gunpackb(src, pos_ptr, 1);
  if (msg_ptr->timeslot_allocation_present == TRUE)
  {
    uint8 slot_mask;

    msg_ptr->timeslot_allocation = gunpackb(src, pos_ptr, num_slots);
    m_slots = 0;

    for (slot_mask = 0x80; slot_mask > 0x00; slot_mask >>= 1)
    {
      if (msg_ptr->timeslot_allocation & slot_mask)
      {
        m_slots++;
      }
    }
  }
  else
  {
    /* no separate allocation in this structure, use the supplied value */
    m_slots = num_slots;
  }

  /* determine whether a single USF applies to all timeslots */
  msg_ptr->usf_allocation_per_slot = (boolean)gunpackb(src, pos_ptr, 1);
  msg_ptr->usf_allocation[0] = gunpackb(src, pos_ptr, CSN_USF_LEN);
  if (msg_ptr->usf_allocation_per_slot == TRUE)
  {
    uint8 index = 0;

    /* local variable m_slots refers to "M", the amount of timeslots assigned */
    msg_ptr->num_usf_allocations = m_slots;
    for (index = 1; index < m_slots; index++)
    {
      if (index >= ARR_SIZE(msg_ptr->usf_allocation))
        break;
      if (gunpackb(src,pos_ptr, 1) == 0)
      {
        /* same USF as previous slot */
        msg_ptr->usf_allocation[index] = msg_ptr->usf_allocation[index-1];
      }
      else
      {
        msg_ptr->usf_allocation[index] = gunpackb(src, pos_ptr, CSN_USF_LEN);
      }
    }
  }
  else
  {
    msg_ptr->num_usf_allocations = 1;
  }

}

static void csn_decode_multiple_uplink_assignment(uint8 *src, uint16 *pos_ptr, multiple_ul_assign_t *msg_ptr)
{
  msg_ptr->extended_dynamic_allocation = gunpackb(src, pos_ptr, 1);

  msg_ptr->p0_present = (boolean)gunpackb(src, pos_ptr, 1);
  if (msg_ptr->p0_present == TRUE)
  {
    msg_ptr->p0 = gunpackb(src, pos_ptr, CSN_P0_LEN);
    msg_ptr->pr_mode = gunpackb(src, pos_ptr, CSN_PR_MODE_LEN);
  }

  msg_ptr->timeslot_description_present = (boolean)gunpackb(src, pos_ptr, 1);
  if (msg_ptr->timeslot_description_present == TRUE)
  {
    ul_tbf_assign_t *ul_assign_ptr;
    ul_tbf_assign_t  dummy_tbf;
    uint8            num_ul_tbfs = 0;
    uint8            num_slots   = 0;
    uint8            slot_mask;

    /* the MS TIMESLOT ALLOCATION field is output in the same format,
     * regardless of whether the power control params are present or not.
     * The 8-bit value has the highest numbered timeslot represented in the
     * least significant bit. So 0x80 implies TN=0, 0x20 implies TN=2 etc.
     * (reading from left to right in the bitmap starts with timeslot zero)
     */
    msg_ptr->timeslot_description.power_control_included = (boolean)gunpackb(src, pos_ptr, 1);
    if (msg_ptr->timeslot_description.power_control_included == TRUE)
    {
      uint8 timeslot_bitmap;
      uint8 alloc_mask;

      csn_pwr_cntrl_params_ie(src, pos_ptr, &(msg_ptr->timeslot_description.power_control_params));
      timeslot_bitmap = msg_ptr->timeslot_description.power_control_params.gamma_bitmap;

      /* infer the timeslot allocation from the gamma bitmap */
      /* which uses the LEAST significant bit for timeslot 0 */
      msg_ptr->timeslot_description.ms_timeslot_allocation = 0x00;
      alloc_mask = 0x01;
      for (slot_mask = 0x80; slot_mask > 0x00; slot_mask >>= 1)
      {
        if (timeslot_bitmap & slot_mask)
        {
          msg_ptr->timeslot_description.ms_timeslot_allocation |= alloc_mask;
        }
        alloc_mask <<= 1;
      }
    }
    else
    {
      msg_ptr->timeslot_description.ms_timeslot_allocation = gunpackb(src, pos_ptr, CSN_TS_ALLOC_LEN);
    }

    /* count how many slots were assigned in the allocation */
    for (slot_mask = 0x80; slot_mask > 0x00; slot_mask >>= 1)
    {
      if (msg_ptr->timeslot_description.ms_timeslot_allocation & slot_mask)
      {
        num_slots++;
      }
    }

    /* determine whether the repeated Uplink TBF Assignment structuers are present */
    while (gunpackb(src, pos_ptr, 1) == 1)
    {
      if (num_ul_tbfs < MAX_NUM_UL_TBF_ASSIGNMENTS)
      {
        ul_assign_ptr = &(msg_ptr->ul_tbf_assignments[num_ul_tbfs]);
        num_ul_tbfs++;
      }
      else
      {
        /* if there are further structures that exceed our internal */
        /* storage limits, just soak them up into a dummy variable  */
        ul_assign_ptr = &dummy_tbf;
        MSG_GERAN_ERROR_0("WARNING: ignoring subsequent UL TBF assignments");
      }

      csn_decode_ul_tbf_assignment(src, pos_ptr, ul_assign_ptr, num_slots);
    }

    msg_ptr->num_ul_tbf_assignments = num_ul_tbfs;
  }
}

static csn_decoder_error_t csn_decode_packet_cs_release_ind(
  csn_decode_memory_instance_t *mem,
  uint8 *src,
  uint16 *pos_ptr,
  packet_cs_release_ind_t *msg_ptr,
  const gas_id_t gas_id
)
{
  CSN_DECODE_MEMORY_IN_USE(mem);

  /* Initialise everything to zero */
  memset(msg_ptr, 0, sizeof(packet_cs_release_ind_t));

  /* Extract the Global TFI */
  csn_global_tfi_ie(src, pos_ptr, &(msg_ptr->global_tfi));

  /* Extract the EDTM CS release ind flag (which should be 1) */
  msg_ptr->enhanced_dtm_cs_release_ind = gunpackb(src, pos_ptr, 1);

  /* Extract the Global Packet Timing Advance */
  csn_global_pkt_timing_ie(src, pos_ptr, &(msg_ptr->global_pkt_timing));

  /* extract the reallocation type */
  msg_ptr->edtm_realloc_type = (edtm_realloc_type_t)gunpackb(src, pos_ptr, CSN_EDTM_REALLOC_TYPE_LEN);

  if (msg_ptr->edtm_realloc_type == EDTM_USE_ALLOCATION_PROVIDED)
  {
    msg_ptr->egprs_mode_tbfs = gunpackb(src, pos_ptr, 1);

    if (msg_ptr->egprs_mode_tbfs == 0) /* Message escape for GPRS mode TBFS  */
    {
      /* determine whether Frequency Parameters are present */
      msg_ptr->freq_params_present = (boolean)gunpackb(src, pos_ptr, 1);
      if (msg_ptr->freq_params_present == TRUE)
      {
        csn_decoder_error_t return_status = csn_frequency_params_ie(
          src,
          pos_ptr,
          &(msg_ptr->freq_params),
          gas_id
        );

        if (return_status != CSN_DECODE_NO_ERROR)
        {
          return return_status;
        }
      }

      /* determine whether Packet Extended Timing advance is present */
      msg_ptr->extended_timing_advance_present = (boolean)gunpackb(src, pos_ptr, 1);
      if (msg_ptr->extended_timing_advance_present == TRUE)
      {
        msg_ptr->extended_timing_advance = gunpackb(src, pos_ptr, CSN_EXTENDED_TIMING_ADVANCE_LEN);
      }

      /* determine whether the Multiple Downlink Assignment structures are present */
      if (gunpackb(src, pos_ptr, 1) == 1)
      {
        multiple_dl_assign_t *mdl_assign_ptr;
        multiple_dl_assign_t  dummy_mdl;
        uint8                 num_dl_ass = 0;

        while (gunpackb(src, pos_ptr, 1) == 1)
        {
          if (num_dl_ass < MAX_NUM_MULTIPLE_DL_ASSIGNMENTS)
          {
            mdl_assign_ptr = &(msg_ptr->multiple_dl_assignments[num_dl_ass]);
            num_dl_ass++;
          }
          else
          {
            /* if there are further structures that exceed our internal */
            /* storage limits, just soak them up into a dummy variable  */
            mdl_assign_ptr = &dummy_mdl;
          }

          csn_decode_multiple_downlink_assignment(src, pos_ptr, mdl_assign_ptr);
        }

        msg_ptr->num_mult_dl_assignments = num_dl_ass;
      }

      /* determine whether the Multiple Uplink Assignment structure is present */
      msg_ptr->num_mult_ul_assignments = gunpackb(src, pos_ptr, 1);
      if (msg_ptr->num_mult_ul_assignments == 1)
      {
        msg_ptr->channel_coding_present = (boolean)gunpackb(src, pos_ptr, 1);
        if (msg_ptr->channel_coding_present == TRUE)
        {
          msg_ptr->channel_coding_command = gunpackb(src, pos_ptr, CSN_CHANNEL_CODING_COMMAND_LEN);
        }
        csn_decode_multiple_uplink_assignment(src, pos_ptr, &(msg_ptr->multiple_ul_assignment));
      }
    }
    else                               /* Message escape for EGPRS mode TBFS */
    {
      /* extract the following two zero bits */
      (void)gunpackb(src, pos_ptr, 2);

      /* determine whether Frequency Parameters are present */
      msg_ptr->freq_params_present = (boolean)gunpackb(src, pos_ptr, 1);
      if (msg_ptr->freq_params_present == TRUE)
      {
        csn_decoder_error_t return_status = csn_frequency_params_ie(
          src,
          pos_ptr,
          &(msg_ptr->freq_params),
          gas_id
        );

        if (return_status != CSN_DECODE_NO_ERROR)
        {
          return return_status;
        }
      }

      /* determine whether Packet Extended Timing advance is present */
      msg_ptr->extended_timing_advance_present = (boolean)gunpackb(src, pos_ptr, 1);
      if (msg_ptr->extended_timing_advance_present == TRUE)
      {
        msg_ptr->extended_timing_advance = gunpackb(src, pos_ptr, CSN_EXTENDED_TIMING_ADVANCE_LEN);
      }

      /* determine whether BEP_PERIOD2 is present */
      msg_ptr->bep_period2_present = (boolean)gunpackb(src, pos_ptr, 1);
      if (msg_ptr->bep_period2_present == TRUE)
      {
        msg_ptr->bep_period2 = gunpackb(src, pos_ptr, CSN_BEP_PERIOD2_LEN);
      }

      /* determine whether downlink TBF structures are present */
      if (gunpackb(src, pos_ptr, 1) == 1)
      {
        multiple_dl_assign_t *mdl_assign_ptr;
        multiple_dl_assign_t  dummy_mdl;
        uint8                 num_dl_ass = 0;

        msg_ptr->dl_egprs_window_size_present = (boolean)gunpackb(src, pos_ptr, 1);
        if (msg_ptr->dl_egprs_window_size_present == TRUE)
        {
          msg_ptr->dl_egprs_window_size = gunpackb(src, pos_ptr, CSN_EGPRS_WINDOW_SIZE_LEN);
        }

        msg_ptr->link_quality_measurement_mode = gunpackb(src, pos_ptr, 2);

        /* determine whether the Multiple Downlink Assignment structures are present */
        while (gunpackb(src, pos_ptr, 1) == 1)
        {
          if (num_dl_ass < MAX_NUM_MULTIPLE_DL_ASSIGNMENTS)
          {
            mdl_assign_ptr = &(msg_ptr->multiple_dl_assignments[num_dl_ass]);
            num_dl_ass++;
          }
          else
          {
            /* if there are further structures that exceed our internal */
            /* storage limits, just soak them up into a dummy variable  */
            mdl_assign_ptr = &dummy_mdl;
          }

          csn_decode_multiple_downlink_assignment(src, pos_ptr, mdl_assign_ptr);
        }

        msg_ptr->num_mult_dl_assignments = num_dl_ass;
      }

      /* determine whether the Multiple Uplink Assignment structure is present */
      msg_ptr->num_mult_ul_assignments = gunpackb(src, pos_ptr, 1);
      if (msg_ptr->num_mult_ul_assignments == 1)
      {
        msg_ptr->egprs_channel_coding_present = (boolean)gunpackb(src, pos_ptr, 1);
        if (msg_ptr->egprs_channel_coding_present == TRUE)
        {
          msg_ptr->egprs_channel_coding_command = gunpackb(src, pos_ptr, CSN_EGPRS_CHANNEL_CODING_COMMAND_LEN);
        }

        msg_ptr->resegment = gunpackb(src, pos_ptr, CSN_RESEGMENT_LEN);

        msg_ptr->ul_egprs_window_size_present = (boolean)gunpackb(src, pos_ptr, 1);
        if (msg_ptr->ul_egprs_window_size_present == TRUE)
        {
          msg_ptr->ul_egprs_window_size = gunpackb(src, pos_ptr, CSN_EGPRS_WINDOW_SIZE_LEN);
        }

        csn_decode_multiple_uplink_assignment(src, pos_ptr, &(msg_ptr->multiple_ul_assignment));
      }
    }
  }

  return CSN_DECODE_NO_ERROR;
}

#endif /* FEATURE_GSM_EDTM */

#endif /* FEATURE_GSM_DTM */


#ifdef FEATURE_GPRS_PS_HANDOVER
static void csn_decode_downlink_assign(uint8 src[], uint16 *pos_ptr,
  dl_assignment_t *msg_ptr)
{
  /* Extract timeslot allocation*/
  msg_ptr->timeslot_allocation = gunpackb(src, pos_ptr, CSN_TIMESLOT_ALLOC_LEN);
  msg_ptr->dl_tbf_assignment.pfi_present = gunpackb(src, pos_ptr, 1);
  if(msg_ptr->dl_tbf_assignment.pfi_present == TRUE)
  {
    /* Extract PFI*/
    msg_ptr->dl_tbf_assignment.pfi = gunpackb(src, pos_ptr, CSN_PFI_LEN);
  }
  /* Extract downlink rlc mode*/
  msg_ptr->dl_tbf_assignment.downlink_rlc_mode = gunpackb(src, pos_ptr, 1);
  /* Extract tfi assignment*/
  msg_ptr->dl_tbf_assignment.tfi_assignment = gunpackb(src, pos_ptr, CSN_TFI_ASSIGNMENT_LEN);
  /* Extract control ack*/
  msg_ptr->dl_tbf_assignment.control_ack = gunpackb(src, pos_ptr, 1);
  /* Extract dl egprs window size present flag*/
  msg_ptr->dl_tbf_assignment.dl_egprs_window_size_present = (boolean)gunpackb(src, pos_ptr, 1);
  if (msg_ptr->dl_tbf_assignment.dl_egprs_window_size_present == TRUE)
  {
    /* Extract dl egprs window size*/
    msg_ptr->dl_tbf_assignment.dl_egprs_window_size = gunpackb(src, pos_ptr, CSN_EGPRS_WINDOW_SIZE_LEN);
  }
}

static void csn_decode_ul_tbf_assign(uint8 src[], uint16 *pos_ptr,
  ul_tbf_assignment_t *msg_ptr,uint8 num_slots)
{
  uint8 m_slots;
  msg_ptr->pfi_present = gunpackb(src, pos_ptr, 1);
  if(msg_ptr->pfi_present == TRUE)
  {
    /* Extract PFI*/
    msg_ptr->pfi = gunpackb(src, pos_ptr, CSN_PFI_LEN);
  }
  /* Extract rlc mode*/
  msg_ptr->rlc_mode = gunpackb(src, pos_ptr, 1);
  /* Extract tfi assignment*/
  msg_ptr->tfi_assignment = gunpackb(src, pos_ptr, CSN_TFI_ASSIGNMENT_LEN);
  /* Extract channel coding present flag*/
  msg_ptr->channel_coding_present = (boolean)gunpackb(src, pos_ptr, 1);
  if (msg_ptr->channel_coding_present == TRUE)
  {
    /* Extract channel coding command*/
    msg_ptr->channel_coding_command = gunpackb(src, pos_ptr, CSN_CHANNEL_CODING_COMMAND_LEN);
  }
  /* Extract egprs channel coding present flag*/
  msg_ptr->egprs_channel_coding_present = (boolean)gunpackb(src, pos_ptr, 1);
  if (msg_ptr->egprs_channel_coding_present == TRUE)
  {
    /* Extract egprs channel coding command*/
    msg_ptr->egprs_channel_coding_command = gunpackb(src, pos_ptr, CSN_EGPRS_CHANNEL_CODING_COMMAND_LEN);
  }
  /* Extract ul egprs window size present flag*/
  msg_ptr->ul_egprs_window_size_present = (boolean)gunpackb(src, pos_ptr, 1);
  if (msg_ptr->ul_egprs_window_size_present == TRUE)
  {
    /* Extract ul egprs window size*/
    msg_ptr->ul_egprs_window_size = gunpackb(src, pos_ptr, CSN_EGPRS_WINDOW_SIZE_LEN);
  }
  /* Extract usf granularity flag*/
  msg_ptr->usf_granularity = gunpackb(src, pos_ptr, 1);
  /* Extract timeslot allocation present flag*/
  msg_ptr->timeslot_allocation_present = (boolean)gunpackb(src, pos_ptr, 1);
  if (msg_ptr->timeslot_allocation_present == TRUE)
  {
    uint8 slot_mask;

    msg_ptr->timeslot_allocation = gunpackb(src, pos_ptr, num_slots);
    m_slots = 0;

    for (slot_mask = 0x80; slot_mask > 0x00; slot_mask >>= 1)
    {
      if (msg_ptr->timeslot_allocation & slot_mask)
      {
        m_slots++;
      }
    }
  }
  else
  {
    /* no separate allocation in this structure, use the supplied value */
    m_slots = num_slots;
  }
  msg_ptr->num_timeslots = m_slots;

  /* determine whether a single USF applies to all timeslots */
  msg_ptr->usf_allocation_per_slot = (boolean)gunpackb(src, pos_ptr, 1);
  msg_ptr->usf_allocation[0] = gunpackb(src, pos_ptr, CSN_USF_LEN);
  if (msg_ptr->usf_allocation_per_slot == TRUE)
  {
    uint8 index = 0;

    /* local variable m_slots refers to "M", the amount of timeslots assigned */
    msg_ptr->num_usf_allocations = m_slots;
    for (index = 1; index < m_slots; index++)
    {
      if (index >= ARR_SIZE(msg_ptr->usf_allocation))
        break;
      if (gunpackb(src,pos_ptr, 1) == 0)
      {
        /* same USF as previous slot */
        msg_ptr->usf_allocation[index] = msg_ptr->usf_allocation[index-1];
      }
      else
      {
        msg_ptr->usf_allocation[index] = gunpackb(src, pos_ptr, CSN_USF_LEN);
      }
    }
  }
  else
  {
    msg_ptr->num_usf_allocations = 1;
  }
}
static void csn_decode_egprs_uplink_assignments(uint8 src[], uint16 *pos_ptr,
  egprs_ul_tbf_t *msg_ptr)
{
  msg_ptr->ul_egprs_window_size_present = (boolean)gunpackb(src, pos_ptr, 1);
  if (msg_ptr->ul_egprs_window_size_present == TRUE)
  {
    /* Extract ul egprs window size*/
    msg_ptr->ul_egprs_window_size = gunpackb(src, pos_ptr, CSN_EGPRS_WINDOW_SIZE_LEN);
  }

  msg_ptr->egprs_channel_coding_present = (boolean)gunpackb(src, pos_ptr, 1);
  if (msg_ptr->egprs_channel_coding_present == TRUE)
  {
    /* Extract egprs channel coding*/
    msg_ptr->egprs_channel_coding_command = gunpackb(src, pos_ptr, CSN_EGPRS_CHANNEL_CODING_COMMAND_LEN);
  }

  msg_ptr->bep_period2_present_flag = (boolean)gunpackb(src, pos_ptr, 1);
  if (msg_ptr->bep_period2_present_flag == TRUE)
  {
    /* Extract bep period2*/
    msg_ptr->bep_period2 = gunpackb(src, pos_ptr, 4);
  }

  msg_ptr->timeslot_description_present = (boolean)gunpackb(src, pos_ptr, 1);
  if (msg_ptr->timeslot_description_present == TRUE)
  {
    /* Extract timeslot description*/
    ul_tbf_assignment_t *ul_assignment_ptr;
    ul_tbf_assignment_t  dummy_tbf;
    uint8            num_ul_tbfs = 0;
    uint8            num_slots   = 0;
    uint8            slot_mask;

    /* the MS TIMESLOT ALLOCATION field is output in the same format,
     * regardless of whether the power control params are present or not.
     * The 8-bit value has the highest numbered timeslot represented in the
     * least significant bit. So 0x80 implies TN=0, 0x20 implies TN=2 etc.
     * (reading from left to right in the bitmap starts with timeslot zero)
     */
    msg_ptr->timeslot_description.power_control_included = (boolean)gunpackb(src, pos_ptr, 1);
    if (msg_ptr->timeslot_description.power_control_included == TRUE)
    {
      uint8 timeslot_bitmap;
      uint8 alloc_mask;

      csn_pwr_cntrl_params_ie(src, pos_ptr, &(msg_ptr->timeslot_description.power_control_params));
      timeslot_bitmap = msg_ptr->timeslot_description.power_control_params.gamma_bitmap;

      /* infer the timeslot allocation from the gamma bitmap */
      /* which uses the LEAST significant bit for timeslot 0 */
      msg_ptr->timeslot_description.ms_timeslot_allocation = 0x00;
      alloc_mask = 0x01;
      for (slot_mask = 0x80; slot_mask > 0x00; slot_mask >>= 1)
      {
        if (timeslot_bitmap & slot_mask)
        {
          msg_ptr->timeslot_description.ms_timeslot_allocation |= alloc_mask;
        }
        alloc_mask <<= 1;
      }
    }
    else
    {
      msg_ptr->timeslot_description.ms_timeslot_allocation = gunpackb(src, pos_ptr, CSN_TS_ALLOC_LEN);
    }

    /* count how many slots were assigned in the allocation */
    for (slot_mask = 0x80; slot_mask > 0x00; slot_mask >>= 1)
    {
      if (msg_ptr->timeslot_description.ms_timeslot_allocation & slot_mask)
      {
        num_slots++;
      }
    }

    /* determine whether the repeated Uplink TBF Assignment structuers are present */
    while (gunpackb(src, pos_ptr, 1) == 1)
    {
      if (num_ul_tbfs < MAX_NUM_UL_TBF_ASSIGNMENTS)
      {
        ul_assignment_ptr = &(msg_ptr->ul_tbf_assignments[num_ul_tbfs]);
        num_ul_tbfs++;
      }
      else
      {
        /* if there are further structures that exceed our internal */
        /* storage limits, just soak them up into a dummy variable  */
        ul_assignment_ptr = &dummy_tbf;
        MSG_GERAN_ERROR_0("WARNING: ignoring subsequent UL TBF assignments");
      }

      csn_decode_ul_tbf_assign(src, pos_ptr, ul_assignment_ptr, num_slots);
    }

    msg_ptr->num_ul_tbf_assignments = num_ul_tbfs;
  }
}
static void csn_decode_egprs_downlink_assignments(uint8 src[], uint16 *pos_ptr,
  egprs_dl_tbf_t *msg_ptr)
{
  uint8                 num_dl_ass = 0;
  if(gunpackb(src, pos_ptr, 1))
  {
    if(gunpackb(src, pos_ptr, 1))
    {
      /* Extract dl egprs window size*/
      msg_ptr->dl_egprs_window_size_present = TRUE;
      msg_ptr->dl_egprs_window_size = gunpackb(src, pos_ptr, CSN_EGPRS_WINDOW_SIZE_LEN);
    }
    msg_ptr->link_quality_meas_mode = gunpackb(src, pos_ptr, 2);
    if (gunpackb(src, pos_ptr, 1))
    {
      /* Extract bep period2*/
      msg_ptr->bep_period2_present_flag = TRUE;
      msg_ptr->bep_period2 = gunpackb(src, pos_ptr, 4);
    }
  }
  while (gunpackb(src, pos_ptr, 1) == 1)
  {
    dl_assignment_t *dl_assignment_ptr;
    dl_assignment_t  dummy_mdl;
    if (num_dl_ass < MAX_NUM_MULTIPLE_DL_ASSIGNMENTS)
    {
      dl_assignment_ptr = &(msg_ptr->dl_assignments[num_dl_ass]);
      num_dl_ass++;
    }
    else
    {
      /* if there are further structures that exceed our internal */
      /* storage limits, just soak them up into a dummy variable  */
      dl_assignment_ptr = &dummy_mdl;
    }
    csn_decode_downlink_assign(src, pos_ptr, dl_assignment_ptr);
  }
  msg_ptr->num_dl_assignments = num_dl_ass;
}
static void csn_decode_egprs_mode_extract(uint8 src[], uint16 *pos_ptr,
  egprs_mode_t *msg_ptr)
{
  csn_decode_egprs_uplink_assignments(src,pos_ptr,&(msg_ptr->egprs_ul_tbf));

  if(gunpackb(src, pos_ptr, 1))
  {
    /*Extract downlink tbf parameters*/
    msg_ptr->egprs_dl_tbf_present = TRUE;
    csn_decode_egprs_downlink_assignments(src,pos_ptr,&(msg_ptr->egprs_dl_tbf));
  }
}
static void csn_decode_gprs_mode_extract(uint8 src[], uint16 *pos_ptr,
  gprs_mode_t *msg_ptr)
{
  uint8                 num_dl_ass = 0;

  if(gunpackb(src, pos_ptr, 1))
  {
    /*Extract channel coding scheme*/
    msg_ptr->channel_coding_command_present = TRUE;
    msg_ptr->channel_coding_command = gunpackb(src, pos_ptr, 2);
  }
  msg_ptr->timeslot_description_present = (boolean)gunpackb(src, pos_ptr, 1);
  if (msg_ptr->timeslot_description_present == TRUE)
  {
    ul_tbf_assignment_t *ul_assignment_ptr;
    ul_tbf_assignment_t  dummy_tbf;
    uint8            num_ul_tbfs = 0;
    uint8            num_slots   = 0;
    uint8            slot_mask;

    /* the MS TIMESLOT ALLOCATION field is output in the same format,
     * regardless of whether the power control params are present or not.
     * The 8-bit value has the highest numbered timeslot represented in the
     * least significant bit. So 0x80 implies TN=0, 0x20 implies TN=2 etc.
     * (reading from left to right in the bitmap starts with timeslot zero)
     */
    msg_ptr->timeslot_description.power_control_included = (boolean)gunpackb(src, pos_ptr, 1);
    if (msg_ptr->timeslot_description.power_control_included == TRUE)
    {
      uint8 timeslot_bitmap;
      uint8 alloc_mask;

      csn_pwr_cntrl_params_ie(src, pos_ptr, &(msg_ptr->timeslot_description.power_control_params));
      timeslot_bitmap = msg_ptr->timeslot_description.power_control_params.gamma_bitmap;

      /* infer the timeslot allocation from the gamma bitmap */
      /* which uses the LEAST significant bit for timeslot 0 */
      msg_ptr->timeslot_description.ms_timeslot_allocation = 0x00;
      alloc_mask = 0x01;
      for (slot_mask = 0x80; slot_mask > 0x00; slot_mask >>= 1)
      {
        if (timeslot_bitmap & slot_mask)
        {
          msg_ptr->timeslot_description.ms_timeslot_allocation |= alloc_mask;
        }
        alloc_mask <<= 1;
      }
    }
    else
    {
      msg_ptr->timeslot_description.ms_timeslot_allocation = gunpackb(src, pos_ptr, CSN_TS_ALLOC_LEN);
    }

    /* count how many slots were assigned in the allocation */
    for (slot_mask = 0x80; slot_mask > 0x00; slot_mask >>= 1)
    {
      if (msg_ptr->timeslot_description.ms_timeslot_allocation & slot_mask)
      {
        num_slots++;
      }
    }

    /* determine whether the repeated Uplink TBF Assignment structuers are present */
    while (gunpackb(src, pos_ptr, 1) == 1)
    {
      if (num_ul_tbfs < MAX_NUM_UL_TBF_ASSIGNMENTS)
      {
        ul_assignment_ptr = &(msg_ptr->ul_tbf_assignments[num_ul_tbfs]);
        num_ul_tbfs++;
      }
      else
      {
        /* if there are further structures that exceed our internal */
        /* storage limits, just soak them up into a dummy variable  */
        ul_assignment_ptr = &dummy_tbf;
        MSG_GERAN_ERROR_0("WARNING: ignoring subsequent UL TBF assignments");
      }

      csn_decode_ul_tbf_assign(src, pos_ptr, ul_assignment_ptr, num_slots);
    }

    msg_ptr->num_ul_tbf_assignments = num_ul_tbfs;
  }

  /* determine whether the Multiple Downlink Assignment structures are present */
  while (gunpackb(src, pos_ptr, 1) == 1)
  {
    dl_assignment_t *dl_assignment_ptr;
    dl_assignment_t  dummy_mdl;
    if (num_dl_ass < MAX_NUM_MULTIPLE_DL_ASSIGNMENTS)
    {
      dl_assignment_ptr = &(msg_ptr->dl_assignments[num_dl_ass]);
      num_dl_ass++;
    }
    else
    {
      /* if there are further structures that exceed our internal */
      /* storage limits, just soak them up into a dummy variable  */
      dl_assignment_ptr = &dummy_mdl;
    }
    csn_decode_downlink_assign(src, pos_ptr, dl_assignment_ptr);
  }
  msg_ptr->num_dl_assignments = num_dl_ass;
}

static csn_decoder_error_t csn_decode_ps_handover_radio_resources(
  uint8 src[],
  uint16 *pos_ptr,
  ps_handover_radio_resources_t *msg_ptr,
  const gas_id_t gas_id
)
{
  csn_decoder_error_t return_status;

  if(gunpackb(src, pos_ptr, 1))
  {
    /*Extract HANDOVER REFERENCE*/
    msg_ptr->handover_reference_present = TRUE;
    msg_ptr->handover_reference = gunpackb(src, pos_ptr, 8);
  }
  /*Extract ARFCN*/
  CSN_DECODE_ARFCN(msg_ptr, pos_ptr);
  /*Extract SI*/
  msg_ptr->si = gunpackb(src, pos_ptr, 2);
  /*Extract NCI*/
  msg_ptr->nci = gunpackb(src, pos_ptr, 1);
  /*Extract BSIC*/
  msg_ptr->bsic = gunpackb(src, pos_ptr, 6);

  if(gunpackb(src, pos_ptr, 1))
  {
    /*Extract CCN ACTIVE FLAG*/
    msg_ptr->ccn_active_present = TRUE;
    msg_ptr->ccn_active = gunpackb(src, pos_ptr, 1);
  }
  if(gunpackb(src, pos_ptr, 1))
  {
    /*Extract 3g CCN ACTIVE FLAG*/
    msg_ptr->ccn_3g_active_present = TRUE;
    msg_ptr->ccn_3g_active = gunpackb(src, pos_ptr, 1);
  }
  if(gunpackb(src, pos_ptr, 1))
  {
    /*Extract ccn support desc structure*/
    msg_ptr->ccn_support_desc_present = TRUE;
    csn_ccn_support_description_extract(src,pos_ptr,&(msg_ptr->ccn_support_desc));
  }

  /*Extract frequency parameters*/
  return_status = csn_frequency_params_ie(
    src,
    pos_ptr,
    &(msg_ptr->freq_params),
    gas_id
  );

  if (return_status != CSN_DECODE_NO_ERROR)
  {
    return return_status;
  }

  /*Extract network control order*/
  msg_ptr->network_control_order = gunpackb(src, pos_ptr, 2);

  if(gunpackb(src, pos_ptr, 1))
  {
    /*Extract global pkt timing advance structure*/
    msg_ptr->global_pkt_timing_present = TRUE;
    csn_global_pkt_timing_ie(src,pos_ptr,&(msg_ptr->global_pkt_timing));
    if(gunpackb(src, pos_ptr, 1))
    {
      /*Extract extended timing advance*/
      msg_ptr->pkt_ext_timing_advance_present = TRUE;
      msg_ptr->pkt_ext_timing_advance = gunpackb(src, pos_ptr, 2);
    }
  }

  /*Extract exteneded dynamic allocation*/
  msg_ptr->ext_dynamic_allocation = gunpackb(src, pos_ptr, 1);

  /*Extract RLC RESET flag*/
  msg_ptr->rlc_reset = gunpackb(src, pos_ptr, 1);

  if(gunpackb(src, pos_ptr, 1))
  {
    /*Extract power control paramerters*/
    msg_ptr->pwr_control_present = TRUE;
    msg_ptr->po = gunpackb(src, pos_ptr, 4);
    msg_ptr->pr_mode = gunpackb(src, pos_ptr, 1);
  }

  if(gunpackb(src, pos_ptr, 1))
  {
    /*Extract uplink control timeslot*/
    msg_ptr->ul_ctrl_timeslot_present = TRUE;
    msg_ptr->ul_ctrl_timeslot = gunpackb(src, pos_ptr, 3);
  }

  /*Extract type of tbf*/
  msg_ptr->type_of_mode = gunpackb(src, pos_ptr, 1);

  if(msg_ptr->type_of_mode == 0)
  {
    /*Extract GPRS tbf parameters*/
    csn_decode_gprs_mode_extract(src,pos_ptr,&(msg_ptr->tbf_mode.gprs_mode));
  }
  else
  {
    /*Extract EGPRS tbf parameters*/
    csn_decode_egprs_mode_extract(src,pos_ptr,&(msg_ptr->tbf_mode.egprs_mode));
  }

  return CSN_DECODE_NO_ERROR;
}

static csn_decoder_error_t csn_decode_ps_handover_to_a_gb_mode_payload(
  uint8 src[],
  uint16 *pos_ptr,
  ps_handover_to_a_gb_mode_payload_t *msg_ptr,
  const gas_id_t gas_id
)
{
  uint8 index;
  csn_decoder_error_t return_status;

  (void) gunpackb(src, pos_ptr, 2);

  /* Extract PS HANDOVER radio resources*/
  return_status = csn_decode_ps_handover_radio_resources(
    src,
    pos_ptr,
    &(msg_ptr->ps_handover_radio_resources),
    gas_id
  );

  if (return_status != CSN_DECODE_NO_ERROR)
  {
    return return_status;
  }

  msg_ptr->nas_container_present = gunpackb(src, pos_ptr, 1);
  /* Extract nas container present flag*/
  if(msg_ptr->nas_container_present == TRUE)
  {
    /* Extract nas container ie*/
    msg_ptr->nas_container.nas_container_length = gunpackb(src, pos_ptr, 7);
    for (index = 0; index < msg_ptr->nas_container.nas_container_length && index <= 8; index++)
    {
      msg_ptr->nas_container.nas_container_data[index] = gunpackb(src, pos_ptr, 8);
    }
  }

  return CSN_DECODE_NO_ERROR;
}

static csn_decoder_error_t csn_decode_ps_handover_command(
  csn_decode_memory_instance_t *mem,
  uint8 *src,
  uint16 *pos_ptr,
  ps_handover_command_t *msg_ptr,
  const gas_id_t gas_id
)
{
  uint8 index;
  csn_decoder_error_t return_status;

  CSN_DECODE_MEMORY_IN_USE(mem);

  /* Initialise everything to zero */
  memset(msg_ptr, 0, sizeof(ps_handover_command_t));

  msg_ptr->global_tfi_present = gunpackb(src, pos_ptr, 1);

  if(msg_ptr->global_tfi_present == 0)
  {
    /* Extract the Global TFI */
    csn_global_tfi_ie(src, pos_ptr, &(msg_ptr->global_tfi));
    /* Extract container ID */
    msg_ptr->container_id = gunpackb(src, pos_ptr, 2);
    /* Extract type of payload present in PSHO COMMAND*/
    msg_ptr->target_type = (psho_target_type_e)gunpackb(src, pos_ptr, 2);
    if(msg_ptr->target_type == CSN_PSHO_GSM_TARGET_CELL)
    {
      /* Extract A/GB PS HANDOVER PAYLOAD*/
      return_status = csn_decode_ps_handover_to_a_gb_mode_payload(
        src,
        pos_ptr,
        &(msg_ptr->payload.a_gb),
        gas_id
      );

      if (return_status != CSN_DECODE_NO_ERROR)
      {
        return return_status;
      }
    }
    else if(msg_ptr->target_type == CSN_PSHO_3G_TARGET_CELL)
    {
      /* Extract UTRAN PS HANDOVER PAYLOAD*/
      msg_ptr->payload.utran.rrc_container.rrc_container_length = gunpackb(src, pos_ptr, 8);
      for (index = 0; index < msg_ptr->payload.utran.rrc_container.rrc_container_length; index++)
      {
        msg_ptr->payload.utran.rrc_container.rrc_container_data[index] = gunpackb(src, pos_ptr, 8);
      }
    }
    else
    {
      return (NON_DISTRIBUTION_ERROR);
    }
  }
  else
  {
    return (ADDRESS_INFO_ERROR);
  }

  return CSN_DECODE_NO_ERROR;
}

static csn_decoder_error_t csn_decode_packet_physical_information(
  csn_decode_memory_instance_t *mem,
  uint8 *src,
  uint16 *pos_ptr,
  packet_physical_information_t *msg_ptr,
  const gas_id_t gas_id
)
{
  CSN_DECODE_MEMORY_IN_USE(mem);

  /* Initialise everything to zero */
  memset(msg_ptr, 0, sizeof(packet_physical_information_t));
  /* Extract the Global TFI */
  csn_global_tfi_ie(src, pos_ptr, &(msg_ptr->global_tfi));
  /* Extract timing advance value*/
  msg_ptr->timing_advance_value = gunpackb(src, pos_ptr, 8);

  return CSN_DECODE_NO_ERROR;
}
#endif /*FEATURE_GPRS_PS_HANDOVER*/


/*****************************************************************************

            Memory allocation and de-allocation

*****************************************************************************/

/* csn_decode allocates memory in various places, then the calling routine uses
   gcsn1_decoder_memory_free() to free the memory when it's finished with it.

   To manage this a list of the pointers to allocated memory is built up,
   gcsn1_decoder_memory_free() simply has to free all the pointers in that list.

   To make csn_decode re-entrant a separate instance of the memory allocation
   is created each time. The first of these is statically allocated for
   efficiency, the remainder are malloc'd.
*/
static csn_decode_memory_instance_t csn_decode_static_memory;
static boolean csn_decode_static_memory_in_use = FALSE;

static uint8 csn_decode_malloc_list_index_max = 0;

static csn_decode_memory_instance_t* csn_decode_malloc_start(void)
{
  csn_decode_memory_instance_t *result;

#ifndef FEATURE_DUAL_DATA
  if (csn_decode_static_memory_in_use == FALSE)
  { /* use the statically allocated memory */
    csn_decode_static_memory_in_use = TRUE;
    result = &csn_decode_static_memory;
  }
  else
#endif /* !FEATURE_DUAL_DATA */

  {
    result = (csn_decode_memory_instance_t *)GPRS_MEM_MALLOC(
      sizeof(csn_decode_memory_instance_t));
    if (result == NULL)
    {
      ERR_GERAN_FATAL_0("Heap exhausted ");
    }
  }

  result->csn_decode_malloc_list_index = 0;
  result->csn_decode_reserved_memory_used = FALSE;
  return result;
}

static void *csn_decode_malloc(csn_decode_memory_instance_t *mem, size_t size)
{
  void *result_ptr;

  if (!mem->csn_decode_reserved_memory_used &&
       size <= CSN_DECODE_RESERVED_MEMORY_SIZE_IN_BYTES)
  {
    mem->csn_decode_reserved_memory_used = TRUE;
#ifdef PERLUTF
    #error code not present
#endif /* PERLUTF */
    return mem->csn_decode_reserved_memory;
  }

  if (mem->csn_decode_malloc_list_index >= CSN_MAX_MALLOCS)
  {
    MSG_GERAN_ERROR_0("malloc list FULL");
    return NULL;
  }

  result_ptr = GPRS_MEM_MALLOC(size);
  if (result_ptr != NULL)
  {
    mem->csn_decode_malloc_list[mem->csn_decode_malloc_list_index++] = result_ptr;
    if (mem->csn_decode_malloc_list_index > csn_decode_malloc_list_index_max)
    {
      csn_decode_malloc_list_index_max = mem->csn_decode_malloc_list_index;
    }
  }

  return result_ptr;
}

/*public*/void gcsn1_decoder_memory_free(csn_decoder_ptrs_t *item)
{
  csn_decode_memory_instance_t *mem;
  uint8 i;

  if (item == NULL)
  {
    return;
  }
  mem = item->allocated_memory_ptr;
  if (mem == NULL)
  {
    return;
  }
  mem->csn_decode_reserved_memory_used = FALSE;

  for (i = 0; i < mem->csn_decode_malloc_list_index; i++)
  {
    GPRS_MEM_FREE(mem->csn_decode_malloc_list[i]);
  }
  mem->csn_decode_malloc_list_index = 0;  /* To signal (internally) that the memory
                                          has been freed */

  if (mem == &csn_decode_static_memory)
  {
    csn_decode_static_memory_in_use = FALSE;
  }
  else
  {
    GPRS_MEM_FREE(mem);
  }
  item->allocated_memory_ptr = NULL;  /* to make sure the caller does not
                                         free this memory twice */

}

