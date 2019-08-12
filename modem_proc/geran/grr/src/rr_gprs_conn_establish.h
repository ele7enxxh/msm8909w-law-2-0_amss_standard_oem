#ifndef RR_GPRS_CONN_ESTABLISH_H
#define RR_GPRS_CONN_ESTABLISH_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             rr_gprs_conn_establish.h

GENERAL DESCRIPTION
   This module contains functions and typedefs for dealing with gprs rr
   connection establishment.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_gprs_conn_establish.h#1 $
  $History: rr_gprs_conn_establish.h $
 *
 * *****************  Version 2  *****************
 * User: Mel          Date: 22/10/02   Time: 16:02
 * Updated in $/GPRS_RR_Host_2_0/gprs/grr
 *
 * *****************  Version 1  *****************
 * User: Nick         Date: 2/10/02    Time: 12:44
 * Created in $/GPRS_RR_HOST_V2/gprs/grr
 * Added to newly created host archive Version2
 *
 * *****************  Version 4  *****************
 * User: Mel          Date: 20/08/02   Time: 11:11
 * Updated in $/GPRS_Integration_1_0/gprs/grr
 * Adding VSS History


when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/07/02   Mel     Initial Creation

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "geran_variation.h"
#include "comdef.h"
#include "sys_cnst.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "rr_defs.h"
#include "rr_l2.h"
#include "geran_multi_sim.h"


/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/
typedef struct
{
  /* Tbf or Dedicated mode - 0=>dedicated and 1=>tbf */
  byte td;

  /*Downlink assignment to mobile in packet idle mode - if 1, then message
  *assigns a resource to mobile identified in the IA rest octets */
  byte downlink;

  /*Two Message Assignment, indicates if there is another
  *Immediate Assignment message to follow */
  byte tma;

}tbf_info_T;

typedef enum
{
  SINGLE_BLOCK_UPLINK,
  SINGLE_BLOCK_DOWNLINK,
  TBF_UPLINK,
  TBF_DOWNLINK
}alloc_type_t;


typedef struct
{
  alloc_type_t allocation_type;
  byte alpha;
  byte gamma;
  word tbf_starting_time;
  byte P0;
  byte bts_pwr_ctrl_mode;
  byte pr_mode;
}ul_sb_allocation_t;


typedef struct
{
  byte type;
  byte alloc_bitmap_len;
  dword alloc_bitmap;
  byte bts_pwr_ctrl_mode;
}fixed_alloc_type;

typedef struct
{
  byte type;
  byte usf;
  byte usf_granularity;
}dynamic_alloc_type;

#define DYNAMIC_ALLOCATION 0
#define FIXED_ALLOCATION 1

typedef union
{
  byte type;
  fixed_alloc_type fixed;
  dynamic_alloc_type dynamic;
}dynamic_or_fixed_alloc_t ;

typedef struct
{
  alloc_type_t allocation_type;
  byte tfi_assignment;
  byte polling;
  dynamic_or_fixed_alloc_t allocation;
  byte P0;
  byte pr_mode;
  byte ch_coding_cmd;
  byte tlli_block_ch_coding;
  byte alpha;
  byte gamma;
  byte timing_advance_index;
  word tbf_starting_time;
}ul_tbf_allocation_t;

typedef struct
{
  alloc_type_t allocation_type;
  dword tlli;
  byte tfi_assignment;
  byte rlc_mode;
  byte alpha;
  byte gamma;
  byte polling;
  byte ta_valid;
  byte timing_advance_index;
  word tbf_starting_time;
  byte P0;
  byte bts_pwr_ctrl_mode;
  byte pr_mode;
}dl_tbf_allocation_t;

typedef union
{
  alloc_type_t allocation_type;
  ul_sb_allocation_t ul_sb_allocation;
  ul_tbf_allocation_t ul_tbf_allocation;
  dl_tbf_allocation_t dl_tbf_allocation;
}pkt_alloc_type;

#define EGPRS_EXTENDED_RA_INDICATOR 0x7f

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/* Functions defined in rr_gprs_conn_establish.c */
extern byte       grr_get_gprs_access_ref(byte, byte *);

extern word       grr_get_egprs_access_ref(byte, byte *, byte, const gas_id_t gas_id);

extern rr_event_T grr_process_mac_grr_ul_tbf_req_msg(
  mac_grr_ul_tbf_req_t *,
  const gas_id_t gas_id
);

/**
  @brief Sends a DL Imm Ass to MAC (either GRR_MAC_DL_SB_EST or GRR_MAC_DL_TBF_EST)
 */
extern void grr_send_gprs_dl_ia_to_mac(
  packet_channel_description_t *packet_ch_desc,
  ia_rest_octets_t             *ia_rest_octets,
  frequency_list_T             *frequency_list,
  byte                          timing_advance,
  const gas_id_t                gas_id
);

extern rr_event_T grr_respond_to_gprs_immediate_assignment(
  ie_descriptor_T *,
  byte,
  boolean *,
  byte,
  request_references_T *,
  const gas_id_t gas_id
);
extern void       grr_start_IA_timers(word, word, boolean, boolean, byte, const gas_id_t gas_id);
extern rr_event_T grr_conn_est_control(rr_event_T, void *, const gas_id_t gas_id);

/* Functions defined in rr_conn_establish.c */
extern rr_event_T rr_ce_process_immediate_assignment(boolean, boolean, uint8*, const gas_id_t gas_id);
extern rr_event_T rr_ce_process_immediate_assignment_extended(boolean, boolean, const gas_id_t gas_id);
extern rr_event_T rr_ce_process_assignment_reject(boolean, boolean, const gas_id_t gas_id);
extern boolean    rr_ce_get_waiting_for_mac_ul_tbf_cnf(const gas_id_t gas_id);

extern void rr_gprs_two_part_assignment_failure(const gas_id_t gas_id);
extern void rr_gprs_two_part_assignment_complete(const gas_id_t gas_id);

extern void grr_start_t3146(const gas_id_t gas_id);

extern boolean grr_return_tbf_data_structure_pointer(const gas_id_t gas_id);

/**
 * Called when this module is activated.
 * Note: The primary purpose of this function is to initialise
 * control/data structures, which may include allocating the storage for them. 
 * It can be assumed that the pointer to the control/data structure has been 
 * setup according to the data instance required. 
 */
extern void rr_tbf_est_activate_init(const gas_id_t gas_id);

/**
 * Called when this module is deactivated.
 * Note: The primary purpose of this function is to free any memory allocated 
 * for the control/data structure
 */
extern void rr_tbf_est_discard_data(const gas_id_t gas_id);

/**
 * Called once when RR task is started.
 * Note: The primary purpose of this function is to initialise
 * pointers to control/data structures. This should also take into 
 * account any second instance of data, as this is a one-off 
 * initialisation. 
 */
extern void rr_tbf_est_task_start_init(void);

#endif  /* RR_GPRS_CONN_ESTABLISH_H */

/* EOF */
