
#ifndef _TM_LPP_UP_H
#define _TM_LPP_UP_H


/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         TM LPP Utility

GENERAL DESCRIPTION
  This file contains TM's LPP protocol utilities, which convert decoded LPP
  messages for the consumption of TmCore.  

  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* ^L<EJECT> */
/*===========================================================================
  
                         EDIT HISTORY FOR MODULE
  
   This section contains comments describing changes made to the module.
   Notice that changes are listed in reverse chronological order.
  
   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/lpp_up/inc/tm_lpp_up.h#1 $ $DateTime: 2016/12/13 07:59:45 $ $Author: mplcsds1 $
  
   when       who     what, where, why
   --------   ---     ----------------------------------------------------------
   05/22/13   rk      Use a common data structure to store gnss time from network.
   02/27/13   rk      Enhance supl_pos_enc_lpp_error_tx to take current transation details.
                      This solution should address LPP Error with new transaction id issue [CR 456984].
===========================================================================*/
  

#include "gps_variation.h"
#include "tm_lpp_tx_mgr.h"
#include "tm_umts_up_supl.h"

#include "tm_supl_asn1.h"


extern boolean b_lte_ota_otdoa_reset_flag;      /* TRUE if an LTE OTA reset is received, indicates test mode */

boolean supl_lpp_gnss_otdoa_assist_proc(tm_lpp_transaction_info_s_type *p_lpp_session_status);


boolean supl_lpp_req_loc_info_proc(tm_lpp_transaction_info_s_type *p_lpp_session_status);


tm_lpp_provide_cap_s_type *tm_umts_up_supl_lpp_get_cap(tm_lpp_transaction_info_s_type *p_lpp_tx_info);

void tm_umts_up_supl_lpp_get_ad_list(tm_lpp_transaction_info_s_type *p_lpp_tx_info);
/* this function encodes the supl pos in suplposinit for LPP*/
boolean supl_enc_lpp_suplpos_suplposinit(
  supl_status_s_type   *p_supl_session_status,
  supl_SUPLPOSINIT*   p_Suplposinit,
  supl_Ver2_PosPayLoad_extension *p_lpp_payload
);

boolean supl_SuplPos_lpp_proc(const supl_Ver2_PosPayLoad_extension_lPPPayload *p_payload); 

boolean supl_SuplPos_lpp_tx (const supl_status_s_type*        p_supl_session_status,
                             const supl_Velocity *p_velocity );

boolean tm_umts_up_supl_lpp_cb_func ( tm_lpp_tx_mgr_client_cmd_e_type  e_cmd_type,
                                      tm_lpp_transaction_info_s_type *p_lpp_tx_info);

boolean supl_enc_lpp_prov_loc(supl_status_s_type *p_supl_session_status, uint32 req_loc_type);

boolean tm_lpp_prov_loc_gnss_meas_build ( const SmGnssMeasReportStructType *p_meas_reports, 
                                       A_GNSS_ProvideLocationInformation *p_prov_loc_info_ie, 
                                       boolean b_status_check_needed,
                                        uint8 b_ls_gnss_method_bmap );
  
void tm_umts_up_supl_lpp_prov_loc_info(tm_lpp_transaction_info_s_type *p_lpp_tx_info);

boolean supl_SuplPos_lpp_error_tx(const supl_status_s_type *p_supl_session_status,
                                  const tm_lpp_transaction_info_s_type *p_lpp_tx_info_type);


/*
 ******************************************************************************
 * Function description:
 *  Encodes LPP SUPL POS with LPP ERROR
 *
 * Parameters:
 *
 *
 * Return value:
 *
 *    TRUE/FALSE
 *    
 *
 ******************************************************************************
*/
boolean supl_pos_enc_lpp_error_tx(
  tm_lpp_transaction_id_s_type *p_lpp_tx_id,
  const supl_status_s_type   *p_supl_session_status,
  tm_lpp_transaction_info_s_type **p_lpp_tx_info_type
);



#endif
