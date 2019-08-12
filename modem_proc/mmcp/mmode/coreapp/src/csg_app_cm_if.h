
#ifndef _CSF_APP_CM_IF_H
#define _CSF_APP_CM_IF_H
/*===========================================================================

                         UI_BASE_ MSGR _ I F . H

DESCRIPTION

 The Data Services Qualcomm Voice service Messenger interface header file.


EXTERNALIZED FUNCTIONS



Copyright (c) 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/coreapp/src/csg_app_cm_if.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/23/13    vs     Initial revision.
===========================================================================*/
#include <stdio.h>
#include <stringl/stringl.h>

#include "cm.h"
#include "sys.h"
#include "ui_base_msgr_msg.h"
#include "qmi_mmode_msgr_msg.h"
#include "cm_msgr_msg.h"
#include "ui_base_timer.h"

#define CSG_APP_CM_IF_ARR_SIZE( a ) ( sizeof( (a) ) / sizeof( (a[0]) ) )

#define CSG_SEARCH_SHORTER_TIMER 1
#define CSG_SEARCH_DEFAULT_TIMER 120
//Default 120 min timer.

#define TUI_MSG_FATAL_4(x_fmt, a, b, c, d) \
    MSG_4 (MSG_SSID_DFLT, MSG_LEGACY_FATAL, x_fmt, a, b, c, d)

#define TUI_MSG_ERROR_4(x_fmt, a, b, c, d) \
    MSG_4 (MSG_SSID_DFLT, MSG_LEGACY_ERROR, x_fmt, a, b, c, d)

#define TUI_MSG_HIGH_4(x_fmt, a, b, c, d) \
    MSG_4 (MSG_SSID_DFLT, MSG_LEGACY_HIGH, x_fmt, a, b, c, d)

#define TUI_MSG_MED_4(x_fmt, a, b, c, d) \
    MSG_4 (MSG_SSID_DFLT, MSG_LEGACY_MED, x_fmt, a, b, c, d)

#define TUI_MSG_LOW_4(x_fmt, a, b, c, d) \
    MSG_4 (MSG_SSID_DFLT, MSG_LEGACY_LOW, x_fmt, a, b, c, d)
    
#define TUI_MSG_FATAL_5(x_fmt, a, b, c, d, e) \
    MSG_5 (MSG_SSID_DFLT, MSG_LEGACY_FATAL, x_fmt, a, b, c, d, e)

#define TUI_MSG_ERROR_5(x_fmt, a, b, c, d, e) \
    MSG_5 (MSG_SSID_DFLT, MSG_LEGACY_ERROR, x_fmt, a, b, c, d, e)

#define TUI_MSG_HIGH_5(x_fmt, a, b, c, d, e) \
    MSG_5 (MSG_SSID_DFLT, MSG_LEGACY_HIGH, x_fmt, a, b, c, d, e)

#define TUI_MSG_MED_5(x_fmt, a, b, c, d, e) \
    MSG_5 (MSG_SSID_DFLT, MSG_LEGACY_MED, x_fmt, a, b, c, d, e)

#define TUI_MSG_LOW_5(x_fmt, a, b, c, d, e) \
    MSG_5 (MSG_SSID_DFLT, MSG_LEGACY_LOW, x_fmt, a, b, c, d, e)

#define TUI_MSG_FATAL_6(x_fmt, a, b, c, d, e, f) \
    MSG_6 (MSG_SSID_DFLT, MSG_LEGACY_FATAL, x_fmt, a, b, c, d, e, f)

#define TUI_MSG_ERROR_6(x_fmt, a, b, c, d, e, f) \
    MSG_6 (MSG_SSID_DFLT, MSG_LEGACY_ERROR, x_fmt, a, b, c, d, e, f)

#define TUI_MSG_HIGH_6(x_fmt, a, b, c, d, e, f) \
    MSG_6 (MSG_SSID_DFLT, MSG_LEGACY_HIGH, x_fmt, a, b, c, d, e, f)

#define TUI_MSG_MED_6(x_fmt, a, b, c, d, e, f) \
    MSG_6 (MSG_SSID_DFLT, MSG_LEGACY_MED, x_fmt, a, b, c, d, e, f)

#define TUI_MSG_LOW_6(x_fmt, a, b, c, d, e, f) \
    MSG_6 (MSG_SSID_DFLT, MSG_LEGACY_LOW, x_fmt, a, b, c, d, e, f)

/*===========================================================================

                    RESPONSES

===========================================================================*/




/*===========================================================================

                    INDICATIONS

===========================================================================*/



/*===========================================================================

                   CALLBACKS

===========================================================================*/



/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

struct csg_app_cm_if_ph_event_s
{
  cm_ph_event_e_type beg;
  cm_ph_event_e_type end;
};

struct csg_app_cm_if_cmss_event_s
{
  cm_ss_event_e_type beg;
  cm_ss_event_e_type end;
};

typedef struct {
  int event;
  cm_ph_info_s_type ph_info;
} csg_app_cm_if_ph_evt_cb_s;

typedef struct {
  int event;
  cm_mm_msim_ss_info_s_type ss_info[SYS_MODEM_AS_ID_3];
} csg_app_cm_if_ss_evt_cb_s;

typedef struct csg_search_select_config_s
{
  uint64  umts_band_pref;
  uint64  lte_band_pref;
  uint32  search_timer;
  uint32  list_type;
  uint32  network_type;
  uint8  search_mode;
  uint8  asubs_id;
  uint8  select_config;
} csg_search_select_config_s_type;


typedef struct
{
  boolean       is_valid;
  timer_type    timer;
  timetick_type interval;
} ui_base_timer_s_type;


typedef struct 
{
  boolean is_efs_read;

  ui_base_timer_s_type timer;

  csg_search_select_config_s_type csg_search_select_config;

}ui_base_csg_config_global_s_type;


void csg_app_cm_if_init(void);

void ui_call_event_cb( cm_call_event_e_type call_event, /* call event */
                       const cm_mm_call_info_s_type *call_info_ptr);

void csg_app_cm_if_ph_evt_cb (cm_ph_event_e_type ph_event, const cm_ph_info_s_type *ph_info_ptr);

void csg_app_cm_if_ss_evt_cb (  cm_ss_event_e_type ss_event,  const cm_mm_msim_ss_info_s_type *ss_info_ptr);

void csg_app_cm_trigger_search(boolean is_app_triggered, boolean send_config_update, boolean is_immediate_search);

void csg_app_cm_ph_cmd_cb( void *p_data, cm_ph_cmd_e_type cmd, cm_ph_cmd_err_e_type err);

extern boolean cm_ph_cmd_get_networks_per_subs_extn(cm_ph_cmd_cb_f_type cmd_cb_func,
                                       void * data_block_ptr,
                                       cm_client_id_type client_id,
                                       cm_network_list_type_e_type list_type,
                                       cm_mode_pref_e_type network_type,
                                       sys_modem_as_id_e_type asubs_id,
                                       cm_band_pref_e_type          band_pref_lmt,
                                       cm_band_pref_e_type          lte_band_pref_lmt,
                                       cm_band_pref_e_type          tds_band_pref_lmt);

boolean ui_base_read_csg_config( void );

boolean ui_base_write_csg_config( const csg_search_select_config_s_type *data_ptr,int size );

boolean csg_app_cm_send_config_update_ind(ui_base_msgr_msg_csg_select_config_cmd_msg_type *sel_config);

void csg_app_search_select_config_handler( mmode_qmi_nas_csg_search_select_config_cmd_msg_type *srch_sel_cfg );

void csg_app_search_trigger_handler( cm_csg_srch_trigger_ind_s_type *search_trigger_req );

void csg_app_immediate_search_handler( qmi_mmode_msgr_csg_immd_search_select_cmd_msg_type *immed_srch_sel_req );

void csg_app_cm_send_select_config_update_ind(void);

#endif /* #define _CSF_APP_CM_IF_H*/

