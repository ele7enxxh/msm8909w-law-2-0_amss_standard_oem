#ifndef GPL1_GPRS_TASK_H
#define GPL1_GPRS_TASK_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  L1   T A S K   P R O C E S S I N G
                       H E A D E R   F I L E

GENERAL DESCRIPTION
   This header file contains declarations relevant to L1 task processing.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001-2013 Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl1/inc/gpl1_gprs_task.h#1 $

when       who      what, where, why
--------   -------- ---------------------------------------------
30/05/14    ws      CR657674 Data + MMS Phase 1
22/08/08    nt      Removed compiler warnings
14/03/08    ip      Added PSHO chnages in feature FEATURE_GPRS_PS_HANDOVER
21/11/07    og      Ensure L1 drops back to IDLE when the single block start
                    time elapses. Resolves CR115529.
21/06/07    og      Ensure the afc information ise set correctly when doing G2G
                    non-blind handover. CR122525 (CR120843).
06/02/07    cs      Use common sig defs in l1_os.h
22/02/06    nt      added gl1_complete_channel_reconnect for DTM use
22/12/05    rm      Added gpl1_gprs_handle_reselection_bcch_mode()
25/11/05    nt      Added handling of ASSIGNMENT and HANDOVER commands in DTM
02/11/05    nt      Added reset gprs data/flags functions
02/11/05    nt      Added gpl1_dtm_set_current_rr_mac_l1_msg_id function prototype
13/1/05     og      MS timebase adjustements confined to radio block gap.
15/10/04    og      Removed Fixed TBF allocation related source code.
23/03/04    ws      Modified prototype forL1_send_l1_mac_freq_ass_conf()
14/07/03    bk      Changed the prototype for gpl1_gprs_handle_tbf_release()
23/06/03    gfr     Compiler warning cleanup.
14/02/03    pjr     Added signal define GPL1_WAIT_FOR_ISR_TO_GET_FN for sleep.
01/11/02    pjr     Added serving cell measurements under feature switch
09/09/02    ws      Added SINGLE_DL_BLOCK_ALLOC_COMP bitmap
8/08/02     ws      Corrected Lint error's and Warning's
15/07/02    ws      Added gpl1_gprs_handle_tbf_release() prototype
02/07/02    ws      Added prototype for gpl1_gprs_lowest_tn_changed(void)
05/09/02    pjr     Added defines for single block allocation
03/08/02    pjr     Added externs and prototypes for Buffer for L1 - MAC
                    message interface.
12/14/01    pjr     Added function prototype gpl1_gprs_handle_idle_mode
09/05/01    ws      Initial revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Alphabetical Order */
#include "geran_variation.h"
#include "cmd.h"
#include "comdef.h"
#include "l1i.h"
#include "l2_l1.h"
#include "l2_l1_g.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "gpl1_gprs_if.h"
#include "l1_os.h"

/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/

/* macro's to handle config_bitmap
 *   ---------------------------------------------------------------------
 *  | UPLINK DATA | DUMMY | FREQ CONFIG | DOWN | DYNAMIC  | FIXED   | SB |
 *  ---------------------------------------------------------------------
 *
 *   MSB                                                              LSB
 */

/* used to mark messages as received */
#define SET_SINGLE_BLOCK_RECV(gas_id)  (l1_transfer_data[(gas_id)].config_bitmap |= 0x1)
#define SET_DYNAMIC_TBF_RECV(gas_id)   (l1_transfer_data[(gas_id)].config_bitmap |= 0x4)
#define SET_DOWNLINK_TBF_RECV(gas_id)  (l1_transfer_data[(gas_id)].config_bitmap |= 0x8)
#define SET_FREQ_CONFIG_RECV(gas_id)   (l1_transfer_data[(gas_id)].config_bitmap |= 0x10)
#define SET_DUMMY_CTRL_RECV(gas_id)    (l1_transfer_data[(gas_id)].config_bitmap |= 0x20)
#define SET_UPLINK_DATA_VALID(gas_id)  (l1_transfer_data[(gas_id)].config_bitmap |= 0x40)
#define RESET_BITMAP(gas_id)           (l1_transfer_data[(gas_id)].config_bitmap = 0)

/*  Check to see if the above bits have been set. */
#define SINGLE_BLOCK_RECV(gas_id)          ((l1_transfer_data[(gas_id)].config_bitmap & 0x1) ? TRUE : FALSE)
#define FREQ_CONFIG_RECV(gas_id)           ((l1_transfer_data[(gas_id)].config_bitmap & 0x10) ? TRUE : FALSE)
#define UPLINK_DATA_VALID(gas_id)          ((l1_transfer_data[(gas_id)].config_bitmap & 0x40) ? TRUE : FALSE)
#define DUMMY_CTRL_RECV(gas_id)            ((l1_transfer_data[(gas_id)].config_bitmap & 0x20) ? TRUE : FALSE)

/*  Clear specific bits. Only valid if less than 8 bits used */
#define CLEAR_SINGLE_BLOCK_RECV(gas_id)    (l1_transfer_data[(gas_id)].config_bitmap &= 0xfe) 
#define CLEAR_FREQ_CONFIG_RECV(gas_id)     (l1_transfer_data[(gas_id)].config_bitmap &= 0xef) 
#define CLEAR_UPLINK_DATA_VALID(gas_id)    (l1_transfer_data[(gas_id)].config_bitmap &= 0xbf) 

/* used to check if allocations are complete
 * so that L1_TRANSFER state can be entered
 */
#define SINGLE_BLOCK_ALLOC_COMP(gas_id)    ((l1_transfer_data[(gas_id)].config_bitmap & 0x51) == 0x51 ? TRUE : FALSE)
#define SINGLE_DL_BLOCK_ALLOC_COMP(gas_id) ((l1_transfer_data[(gas_id)].config_bitmap & 0x11) == 0x11 ? TRUE : FALSE)
#define DYNAMIC_TBF_ALLOC_COMP(gas_id)     ((l1_transfer_data[(gas_id)].config_bitmap & 0x34) == 0x34 ? TRUE : FALSE)
#define DOWNLINK_TBF_ALLOC_COMP(gas_id)    ((l1_transfer_data[(gas_id)].config_bitmap & 0x18) == 0x18 ? TRUE : FALSE)

/* only used if num_alloc_block = 2 as we need to wait for ul dummy block before
 * multiblock allocation is complete
 * so need FREQUENCY_CONFIG + SINGLE_BLOCK_CONFIG + UPLINK_DATA + DUMMY_BLOCK
 */
#define MULTI_BLOCK_ALLOC_COMP(gas_id)     ((l1_transfer_data[(gas_id)].config_bitmap & 0x71) == 0x71 ? TRUE : FALSE)



/*  Signal definitions now in l1_os.h  */

#ifdef FEATURE_GPRS_PS_HANDOVER
/*
   This data structure will contain target cell information
   and source cell information specifically for use during
   PSHO, its here because of inclusion order from
   other header files, l1i.h and gpl1_gprs_if.h
*/

typedef struct
{
  uint8               pmax;
  uint8               alpha;
  sys_band_T          ul_power_band;
  uint8               gamma_tn[ GL1_DEFS_SLOTS_IN_FRAME ];
  uint32              pch[ GL1_DEFS_SLOTS_IN_FRAME ];
  uint16              gamma_gsm;
} psho_gamma_alpha_backup_T;

typedef struct
{
  transfer_data_T   psho_backup_l1_transfer_data;
  gpl1_gprs_ta_T    psho_backup_gpl1_gprs_ta_info;
  psho_gamma_alpha_backup_T psho_backup_gamma_alpha;
  /* Target information */
  l1_sc_bs_T        target_cell_details;
  l1_sc_bs_T        current_cell_details;
  l1_sc_bs_T        previous_cell_details;

  psho_sync_type_t  psho_sync_type;
  boolean           handover_reference_present;
  uint8             handover_reference;
  uint8             psho_access_burst_type;
} psho_data_T;
#endif

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/* PROTOTYPES */
boolean gpl1_gprs_handle_tbf_release(release_tbf_t   release_tbf, gas_id_t gas_id);
boolean gpl1_gprs_check_frequency_params(frequency_list_T *freq_list, boolean hopping_flag, gas_id_t gas_id);
void L1_send_l1_mac_freq_ass_conf(byte error,tbf_freq_cnf_t tbf_type, gas_id_t gas_id);
uint8   gpl1_gprs_reorder_tn(uint8 *mac_tn, uint8 *pl1_tn);

void    gpl1_gprs_handle_transfer_mode(IMH_T *msg_header);
void    gpl1_gprs_handle_mac_idle_mode(IMH_T *msg_header);
void    gpl1_gprs_handle_grr_idle_mode(IMH_T *msg_header);
void    gpl1_gprs_handle_idle_mode(IMH_T *msg_header);
void    gpl1_gprs_handle_random_access_mode(IMH_T *msg_header);
void    gpl1_gprs_report_period_done_trans(gas_id_t gas_id);
void    gpl1_gprs_serving_cell_meas_if(gas_id_t gas_id);
void    gpl1_gprs_handle_reselection_bcch_mode(mph_gprs_reselection_req_t *msg_ptr);

void gpl1_gprs_send_mac_51_sb_failure(void *dummy_param, gas_id_t gas_id);
gprs_l1_idle_data_t* gl1_ms_switch_gprs_idle_data(gas_id_t gas_id);

/*  Buffer for L1 - MAC message interface */
#define L1_MAC_MAX_QUEUE  10
extern l1_mac_sig_t              l1_mac_interface_buffer[L1_MAC_MAX_QUEUE];
extern l1_mac_sig_t              *l1_mac_interface_ptr;

#ifdef FEATURE_GPRS_PS_HANDOVER
extern psho_data_T  gpl1_psho_data;

void  gpl1_gprs_sync_to_target (byte new_ts,
                                l1_sc_bs_T *current_cell,
                                l1_sc_bs_T *target_cell,
                                l1_sc_bs_T *previous_cell,
                                gas_id_t gas_id );

void gpl1_gprs_send_l1_mac_psho_cnf(psho_status_t psho_status, gas_id_t gas_id);
#endif

/*  Buffer for L1 - MAC message interface function prototypes */
void gl1_gprs_l1_mac_interface (l1_mac_sig_t *common_msg_ptr, uint8 *pacch_data);
l1_mac_sig_t *gl1_gprs_get_buf_l1_mac (void);

#ifdef FEATURE_GSM_DTM
void gpl1_dtm_set_current_rr_mac_l1_msg_id( IMH_T message, gas_id_t gas_id );
void gpl1_dtm_data_init( gas_id_t gas_id );
void gl1_complete_channel_assignment ( gas_id_t gas_id );
void gl1_complete_channel_handover ( boolean  apply_afc, gas_id_t gas_id );
void gl1_complete_channel_reconnect( gas_id_t gas_id );
#endif

/* Functions used to reset critical data variables and flags */
void gpl1_dynamic_data_init( gas_id_t gas_id );
void gpl1_single_block_data_init( gas_id_t gas_id );
void gpl1_prach_data_init( void );

#endif /* GPL1_GPRS_TASK_H */
