#ifndef MSG_I_H
#define MSG_I_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diagnostics Packet Processing

General Description
  Diagnostic packet processing internal declarations.

Copyright (c) 2007-2014 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/services/diag/LSM/common/src/msgi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/14   rs      Added support for build mask centralization. 
10/01/13   sa      listeners clean up function
11/15/12   is      Support for preset masks
08/31/12   sr      Support for mask retrieval command  
08/14/12   ra      Added prototype for msg_mask_binary_search
08/10/12   sa      Mainlined DIAG_CENTRAL_ROUTING
07/05/12   ra      Features for F3 Mask Check Optimization 
06/21/12   sa      Added prototype for diag_f3_searchlist_ext
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
12/23/10   mad     Added prototype for msg_get_ssid_masks_adv 
05/10/10   sg      moved msg_mask type from msg.c to msgi.h
01/15/09   mad     defined MSG_I_H, updated copyright
12/02/08   mad     Created file.
===========================================================================*/

#include "diagcomm_ctrl.h" /* For diag_ctrl_msg_type */


typedef struct{
  uint32 args[10];
}msg_large_args;

#define MSG_LARGE_ARGS(X) (((msg_large_args *)X)->args)

typedef enum
{
  MSG_MASK_BT,          /* Build-time mask */
  MSG_MASK_RT           /* Run-time mask */
} msg_mask_type;
typedef enum
{
	F3_LISTENER,      /* Single Listeners */
	F3_LISTENER_EXT   /* Extended Listeners */
}f3_listener_type;
void msg_set_all_listener_masks(uint32 mask);
boolean msg_listeners_masks_enabled(uint32 ssid, uint32 ss_mask, f3_listener_type type);
void diag_clean_f3_listener_ext (void *param);

uint8 msg_mask_binary_search (uint16 curr_ssid);
boolean msg_get_ssid_masks_adv (msg_mask_type mask_type, uint16 ssid_start,
                                uint16 ssid_end, uint32 * mask, byte stream_id, uint8 preset_id);

void msg_set_all_rt_masks_adv (uint32 mask, byte stream_id, uint8 preset_id);
                                
void diag_switch_msg_preset_mask( void );
                                
PACK(void *) msg_pkt_get_local_masks(PACK(void *) req_pkt, uint16 pkt_len);
 
PACK(void *) msg_pkt_get_local_ssid_range(PACK(void *) req_pkt, uint16 pkt_len);

boolean diag_f3_searchlist_ext(unsigned int ssid, unsigned int ss_mask, diag_msg_type *msg);

/* On MP Master only */
#if defined(DIAG_MP_MASTER)
void diagcomm_ctrl_send_msg_mask( diagcomm_port_type port_type, 
                                  diagcomm_enum_port_type port_num, 
                                  uint8 stream_id,
                                  uint8 preset_id,
                                  boolean send_now );
#endif /*DIAG_MP_MASTER */

/* On MP Slave only */
#if defined(DIAG_MP) && !defined(DIAG_MP_MASTER)
void diag_ctrl_update_msg_mask( diag_ctrl_msg_type * ctrl_msg );
void diag_ctrl_update_msg_preset_mask( diag_ctrl_msg_type * ctrl_msg );
void diag_ctrl_send_msg_ssid_range_ctrl_pkt(void);
void diag_ctrl_send_msg_bt_mask_ctrl_pkt(void);
#endif /* DIAG_MP && !DIAG_MP_MASTER */

/* For F3 Mask Check Optimization - Used to set
   corresponding bits in msg_enabled_mask.
   These denote if a msg mask / listener is turned
   "on" or "off"
*/
#define DIAG_MSG_NONE (0x00)
#define DIAG_MSG_STREAM_1 (0x01)
#define DIAG_MSG_STREAM_2 (0x02)
#define DIAG_MSG_LISTENER (0x04)
#define DIAG_MSG_EXT_LISTENER (0x08)

#endif /* MSG_I_H */
