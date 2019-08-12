#ifndef WMSSIM_H
#define WMSSIM_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           wmssim.h -- SIM Interface Definitions

  The WMS module which implements the User API for SMS. This source file
  defines the SIM Interface functions.

  -------------

    Copyright (c) 1998-2008,2010-2013 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/WMSE/wmssim.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "mmcp_variation.h"
#include "comdef.h"
#include "wmsi.h"

#ifdef FEATURE_GWSMS
#ifdef FEATURE_GWSMS_BROADCAST

boolean wms_sim_write_cbmi(
  wms_prov_app_info_s_type  *appInfo_ptr,
  uint8                     *data
);

boolean wms_sim_write_cbmir(
  wms_prov_app_info_s_type  *appInfo_ptr,
  uint8                     *data
);

wms_cmd_err_e_type wms_sim_mmgsdi_read_lang_files(
  wms_prov_app_info_s_type  *appInfo_ptr,
  uint16                    *lang_list_ptr,
  int                        lang_list_size,
  int                       *num_languages_read
);

wms_cmd_err_e_type wms_sim_init_lang_files(
  wms_prov_app_info_s_type  *appInfo_ptr
);

uint16 wms_sim_xlate_iso639_to_lang_filter(
  byte                       iso_lang_msb,
  byte                       iso_lang_lsb
);

uint16 wms_sim_xlate_dcs_to_lang_filter(
  wms_gw_cb_language_e_type dcs
);

#endif /* FEATURE_GWSMS_BROADCAST */
#endif /* FEATURE_GWSMS */


void wms_sim_mmgsdi_init(void);

void wms_sim_mmgsdi_event_cb(
  const mmgsdi_event_data_type *event
);

void wms_sim_mmgsdi_response_cb(
  mmgsdi_return_enum_type   status,
  mmgsdi_cnf_enum_type      cnf,
  const mmgsdi_cnf_type    *cnf_ptr
);

void wms_sim_mmgsdi_client_id_reg_cb (
  mmgsdi_return_enum_type   status,
  mmgsdi_cnf_enum_type      cnf,
  const mmgsdi_cnf_type    *cnf_ptr
);

void wms_sim_mmgsdi_event_cb_proc(
  wms_cmd_type             *cmd_ptr
);

boolean asid_is_valid(
  sys_modem_as_id_e_type asid
);

wms_prov_app_info_s_type *wms_get_lte_appinfo(
  void
);

boolean wms_check_srv_avail_by_asid_and_message_mode( 
   sys_modem_as_id_e_type   asid,
   wms_message_mode_e_type  message_mode,
   sys_sys_mode_e_type      sys_mode 
);

wms_prov_app_info_s_type * wms_get_appinfo_by_session_id(
  mmgsdi_session_id_type session_id
);

wms_prov_app_info_s_type * wms_get_appinfo_by_session_type(
  mmgsdi_session_type_enum_type session_type
);

wms_prov_app_info_s_type * wms_get_appinfo_by_asid_and_message_mode(
  sys_modem_as_id_e_type   asid,
  wms_message_mode_e_type  message_mode
);

wms_prov_app_info_s_type * wms_get_appinfo_by_asid_and_mem_store(
  sys_modem_as_id_e_type    asid,
  wms_memory_store_e_type   mem_store
);

boolean wms_check_appinfo_is_valid(
  wms_prov_app_info_s_type   *appInfo_ptr
);

wms_prov_app_info_s_type * wms_get_appinfo_1x(
  void
);

sys_modem_as_id_e_type wms_get_appinfo_1x_asid(
  void
);

wms_prov_app_info_s_type * wms_get_gw_appinfo_by_asid(
  sys_modem_as_id_e_type   asid
);

mmgsdi_session_type_enum_type wms_get_session_type_by_as_id_and_app_type(
  sys_modem_as_id_e_type as_id,
  mmgsdi_app_enum_type app_type
);

wms_prov_mmgsdi_app_info_s_type * wms_get_mmgsdi_appInfo_by_session_type(
  mmgsdi_session_type_enum_type session_type
);

wms_prov_app_info_s_type * wms_sim_alloc_appInfo_ptr(
  wms_prov_mmgsdi_app_info_s_type *wms_mmsgdi_appInfo
);


void wms_sim_read_app_files(
  wms_prov_app_info_s_type *appInfo_ptr
);

void wms_read_gw_template_from_nv(
  wms_gw_template_s_type   *template_ptr,
  sys_modem_as_id_e_type   as_id
);

void wms_read_cdma_template_from_nv(
  wms_cdma_template_s_type *template_ptr
);

void wms_sim_mmgsdi_response_cb_proc(
  wms_cmd_type             *cmd_ptr
);

void wms_sim_mmgsdi_seek_cb_proc(
  wms_cmd_type             *cmd_ptr
);

void wms_sim_mmgsdi_session_open_cb(
  mmgsdi_return_enum_type   status,
  mmgsdi_cnf_enum_type      cnf,
  const mmgsdi_cnf_type    *cnf_ptr
);

void wms_mmgsdi_wait_for_write_complete_cb(
  mmgsdi_return_enum_type   status,
  mmgsdi_cnf_enum_type      cnf,
  const mmgsdi_cnf_type    *cnf_ptr
);

mmgsdi_return_enum_type wms_sim_mmgsdi_write_data(
  wms_prov_app_info_s_type *appInfo_ptr,
  mmgsdi_file_enum_type     file_enum,
  mmgsdi_rec_num_type       record_number,
  uint8*                    write_data_ptr,
  uint32                    write_data_len
);

mmgsdi_return_enum_type wms_sim_mmgsdi_read_data(
  wms_prov_app_info_s_type *appInfo_ptr,
  mmgsdi_file_enum_type     file_enum,
  mmgsdi_rec_num_type       record_number,
  mmgsdi_len_type           request_length,
  uint8                    *data_ptr,
  uint32                    data_len
);

mmgsdi_return_enum_type wms_sim_mmgsdi_delete_data(
  wms_prov_app_info_s_type *appInfo_ptr,
  mmgsdi_file_enum_type     file_enum,
  mmgsdi_rec_num_type       record_number,
  uint32                    record_len
);

mmgsdi_return_enum_type wms_sim_mmgsdi_read_transparent(
  wms_prov_app_info_s_type *appInfo_ptr,
  mmgsdi_file_enum_type     file_enum,
  mmgsdi_len_type           req_len
);

mmgsdi_return_enum_type wms_sim_mmgsdi_get_file_attr(
  wms_prov_app_info_s_type *appInfo_ptr,
  mmgsdi_file_enum_type     file_enum
);

void wms_sim_mmgsdi_file_attr_cb_smsp_proc(
  const mmgsdi_cnf_type    *cnf_ptr
);

void wms_sim_mmgsdi_file_attr_cb_sms_proc(
  const mmgsdi_cnf_type    *cnf_ptr
);

void wms_sim_mmgsdi_file_attr_cb_smsr_proc(
  const mmgsdi_cnf_type    *cnf_ptr
);

void wms_sim_mmgsdi_file_attr_cb_mwis_proc(
  const mmgsdi_cnf_type    *cnf_ptr
);

void wms_sim_mmgsdi_read_cb_smsp_gw_proc(
  const mmgsdi_cnf_type    *cnf_ptr
);

void wms_sim_mmgsdi_read_cb_smss_gw_proc(
  const mmgsdi_cnf_type    *cnf_ptr
);

void wms_sim_mmgsdi_read_cb_sms_gw_proc(
  const mmgsdi_cnf_type    *cnf_ptr
);

void wms_sim_mmgsdi_read_cb_smsp_cdma_proc(
  const mmgsdi_cnf_type    *cnf_ptr
);

void wms_sim_mmgsdi_read_cb_smss_cdma_proc(
  const mmgsdi_cnf_type    *cnf_ptr
);

void wms_sim_mmgsdi_read_cb_sms_cdma_proc(
  const mmgsdi_cnf_type    *cnf_ptr
);

void wms_sim_mmgsdi_read_cb_smsr_proc(
  const mmgsdi_cnf_type    *cnf_ptr
);

void wms_sim_mmgsdi_read_cb_mwis_proc(
  const mmgsdi_cnf_type *cnf_ptr
);

void wms_sim_mmgsdi_read_cb_cbmi_proc(
  const mmgsdi_cnf_type    *cnf_ptr
);

void wms_sim_mmgsdi_read_cb_cbmir_proc(
  const mmgsdi_cnf_type    *cnf_ptr
);

void wms_sim_mmgsdi_read_cb_cbmid_proc(
  const mmgsdi_cnf_type    *cnf_ptr
);

wms_memory_store_e_type wms_sim_mmgsdi_get_supproted_mem_store(
  mmgsdi_app_enum_type      app_type
);

boolean wms_sim_mmgsdi_mem_store_supported(
  mmgsdi_app_enum_type      app_type,
  wms_memory_store_e_type       mem_store
);

boolean wms_sim_mmgsdi_init_done(
  wms_prov_app_info_s_type *appInfo_ptr
);

void wms_sim_mmgsdi_register_for_refresh(
  wms_prov_app_info_s_type *appInfo_ptr
);

void wms_sim_do_subs_ready(
  wms_prov_app_info_s_type *appInfo_ptr
);

void wms_sim_set_sim_files_init_states(
  wms_prov_app_info_s_type *appInfo_ptr, 
  boolean                   inited
);


#endif /* WMSSIM_H */

