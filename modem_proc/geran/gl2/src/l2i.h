#ifndef L2I_H
#define L2I_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  L 2   T Y P E S   H E A D E R   F I L E

DESCRIPTION
   Layer 2 type declarations.


Copyright (c) 2001-2014 Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/L2/vcs/l2i.h_v   1.5   17 Jul 2002 16:09:24   jachan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl2/src/l2i.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who      what, where, why
--------   -------- ---------------------------------------------
01/13/12   SJW       Avoid timer API calls during task startup
07/22/09   SJW       Added GL2_ALLOC and GL2_FREE macros for new heap manager
02/11/03   JAC       Added short header support
10/17/02   JAC       Added TO_BE_SET_MAX_LATENCY in timer state
08/04/02   JAC       Added forced suspend
10/19/01   JAC       Initial revision.

===========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_l2.h"
#include "msg.h"
#include "l2_l1.h"
#include "l2_l1_g.h"
#include "modem_mem.h"
#include "geran_multi_sim.h"

/* -----------------------------------------------------------------------
** Constant / Macro Declarations
** ----------------------------------------------------------------------- */ 

#define L2_HEADER_LENGTH 3

#define GL2_ALLOC(siz) modem_mem_alloc((siz), MODEM_MEM_CLIENT_GERAN_CRIT)
#define GL2_FREE(ptr) modem_mem_free((ptr), MODEM_MEM_CLIENT_GERAN_CRIT)

#define L2_ERR_FATAL(xx_exp) \
  if (!(xx_exp)) \
  { \
    ERR_FATAL("L2 Assertion " #xx_exp " failed", 0, 0, 0); \
  }

#define NUM_SEED_VALUES 4

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

typedef enum
{
  SABM,
  UA,
  DM,
  DISC,
  UI,
  I,
  RR,
  REJ,
  RNR,
  UI_SHORT_HEADER
} frame_type_T;

typedef enum
{
  EV_NO_EVENT,
  EV_CONNECT_RECEIVED,
  EV_ESTABLISH_REQUEST,
  EV_RELEASE_REQUEST,
  EV_SUSPEND_REQUEST,
  EV_RESUME_REQUEST,
  EV_RECONNECT_REQUEST,
  EV_DATA_REQUEST,
  EV_MDL_RELEASE_REQUEST,
  EV_UA_RECEIVED,
  EV_DM_RECEIVED,
  EV_DISC_RECEIVED,
  EV_SABM_RECEIVED,
  EV_I_RECEIVED,
  EV_UI_RECEIVED,
  EV_RR_RECEIVED,
  EV_REJ_RECEIVED,
  EV_T200_TIMEOUT,
  EV_CONTENTION_FAILED,
  EV_ABORT_ESTABLISHMENT,
  EV_LINK_ESTABLISHED,
  EV_RELEASE_CONFIRMED,
  EV_CLEAR_RECOVERY_CONDITION,
  EV_OPTIONAL_SEND,
  EV_RESET_L2,
  EV_UA_SENT,
  EV_FORCED_SUSPEND_REQ
} l2_event_T;

typedef enum
{
  SAPI0_NULL,
  SAPI0_CONNECT_PEND,
  SAPI0_IDLE,
  SAPI0_ESTABLISHMENT_PENDING,
  SAPI0_RELEASE_PENDING,
  SAPI0_LINK_ESTABLISHED,
  SAPI0_TIMER_RECOVERY,
  LINK_SUSPENDED,
  SAPI0_UA_PENDING
} SAPI0_state_T;

typedef enum
{
  SAPI3_NULL,
  SAPI3_CONNECT_PEND,
  SAPI3_IDLE,
  SAPI3_ESTABLISHMENT_PENDING,
  SAPI3_RELEASE_PENDING,
  SAPI3_LINK_ESTABLISHED,
  SAPI3_TIMER_RECOVERY,
  SAPI3_UA_PENDING
} SAPI3_state_T;

typedef struct message_entry
{
  struct message_entry *next_message;
  byte                 message_length;
  byte                 message_number;
  byte                 *message;
} message_entry_T;

typedef enum
{
  RESUMPTION,
  RECONNECTION
} reest_type_T;

typedef enum
{
  STOPPED,
  RUNNING,
  EXPIRED,
  TO_BE_SET,
  TO_BE_SET_ASAP,
  TO_BE_SET_MAX_LATENCY,
  GS_TMR_UNINITIALISED
} L2_timer_state_T;

typedef struct
{
  byte            no_of_entries;
  message_entry_T *first_message;
} send_queue_T;

typedef struct segment_descriptor
{
  byte   segment_length;
  byte   *segment;
  struct segment_descriptor *next;
} segment_descriptor_T;

typedef struct
{
  boolean valid_data_flag;
  byte    length;
  byte    layer3_message[N201_DCCH];
} contention_msg_T;

typedef struct
{
  byte    no_of_entries;
  segment_descriptor_T *first_segment;
  segment_descriptor_T *next_segment;
} send_buffer_T;

typedef struct
{
  boolean      valid_data_flag;
  frame_type_T frame_type;
  boolean      retransmission;
  byte         layer2_message[N201_MAX + L2_HEADER_LENGTH];
} transmit_buffer_T;

typedef struct
{
  byte length;
  byte layer3_message[MAX_OCTETS_L3_MESSAGE];
} receive_buffer_T;

typedef struct channel_info_T
{
  l2_channel_type_T  channel;
  transmit_buffer_T  transmit_buffer;
  send_queue_T       send_queue;
  send_buffer_T      send_buffer;
  boolean            outstanding_i_frame;
  boolean            outstanding_u_frame;
  byte               N201;
} channel_info_T;

typedef struct sapi_info_T
{
  SAPI_T            sapi;

  byte              VS;
  byte              VR;
  byte              VA;
  byte              sequence_error;
  byte              ns_dup_counter;
  byte              retransmission_counter;
  receive_buffer_T  receive_buffer;
  channel_info_T    DCCH_channel;
  channel_info_T    SACCH_channel;
  
  byte                 T200;
  L2_timer_state_T     T200_state;
  l2_channel_type_T    T200_channel_type;
  boolean              T200_start_request;
  byte                 T200_index;

  byte                 T_ack;
  L2_timer_state_T     T_ack_state;
  byte                 T_ack_seq_number;
  byte                 T_ack_index;
  gas_id_t             gas_id;
} sapi_info_T;


typedef struct l2_store_T
{
  sapi_info_T            sapi0;
  sapi_info_T            sapi3;
  SAPI3_state_T          SAPI3_state;
  SAPI3_state_T          old_SAPI3_state;
  SAPI0_state_T          SAPI0_state;
  SAPI0_state_T          old_SAPI0_state;
  SAPI0_state_T          previous_SAPI0_state;
  reest_type_T           reestablishment_type;
  boolean                dcch_ready;
  boolean                sacch_ready;
  boolean                suspend_pending_transmit;
  byte                   l3_resume_message[N201_MAX];
  byte                   l3_resume_message_length;
  byte                   received_CR;
  byte                   received_M;
  byte                   received_PF;
  byte                   received_NS;
  byte                   received_NR;
  byte                   l2_segment_length;
  byte                  *received_l2_segment;
  byte                   l3_message_length;
  byte                   l3_message_number;
  byte                   l3_message_to_send[MAX_OCTETS_L3_MESSAGE];
  establish_mode_T       establish_mode;
  contention_msg_T       contention_message;
  dl_random_access_req_T pending_ra_request;
  SAPI_T                 sacch_last_sapi_served;
  int                    pseudo_sapi3_receive;
  SAPI_T                 current_SAPI;
  l2_channel_type_T      current_l2_channel_type;
  l2_channel_type_T      last_dcch_type;
  channel_type_T         N200_dcch_type;
  release_mode_T         release_mode;
  ph_data_req_T          sapi3_cache;
  byte                   sacch_repetition_order;
  boolean                l1_has_candidate_buffer;
  ARFCN_T                last_channel_number;
  unsigned long          xor_seed[NUM_SEED_VALUES];
  boolean                random_pad;
  gas_id_t               gas_id;
} l2_store_T;

/* -----------------------------------------------------------------------
** Function Definitions
** ----------------------------------------------------------------------- */

extern channel_info_T * l2_get_channel_info(sapi_info_T *, l2_channel_type_T);
extern sapi_info_T *    l2_get_sapi_info(l2_store_T *, SAPI_T);
extern boolean          l2_check_frame_optional(channel_info_T *);
extern void             l2_clear_sapi(l2_store_T *, SAPI_T, boolean);
extern void             l2_clear_T200_timer(sapi_info_T *);
extern void             l2_clear_T_ack_timer(sapi_info_T *);
extern void             l2_SAPI0_control(l2_store_T *, l2_event_T); /* in l2_sapi0.c */
extern void             l2_SAPI3_control(l2_store_T *, l2_event_T); /* in l2_sapi3.c */
extern void             l2_init_sapi(sapi_info_T *, boolean);          /* in l2_transfer.c */
extern l2_store_T      *l2_get_store(const gas_id_t);
#endif
