
/*!
  @file
  ds_umts_bearer_context.h

  @brief
  Lower layer/Bearer Context interface

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2008-2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/umtsps/inc/ds_umts_bearer_context.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/07/13   nd      Added support for filling PCO of MT calls(primary/secondary)
                   from incoming call request and not from profile(See 23.060).
08/10/12   ss      Delegating RAT specific handling during call_end.
09/08/11   ttv     Changes to add src_stat_desc as profile parameter.
02/23/11   ttv     Added support for watermark tracing.
01/31/11   ss      Added support for reporting bearer tech type in WCDMA 
                   and GPRS.

===========================================================================*/

#ifndef DSUMTSBEARERCONTEXT_H
#define DSUMTSBEARERCONTEXT_H

#include "datamodem_variation.h"
#if defined (FEATURE_DATA_WCDMA_PS) || defined (FEATURE_GSM_GPRS)
#include "ps_iface_defs.h"
#include "ds_3gpp_bearer_context.h"
#include "ps_comp_logging_helper.h"
#include "dsumtspdpreg.h"
#include "ps_crit_sect.h"
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*---------------------------------------------------------------------------
  This is another function table maintained for carrying out WCDMA and GPRS 
  functions. The handlers are registered appropriately while power up and the
  invocation of the same happens through the function pointers.
---------------------------------------------------------------------------*/

typedef struct
{

  /*---------------------------------------------------------------------------
    This is the function that is called from the UMTS handler function when
    there is a call connected indication from CM.
  ---------------------------------------------------------------------------*/
  boolean (*DS_BEARER_CTXT_VF_FPN(activateCnf))(
                           ds_bearer_context_s             *bearer_context_p,
                           const cm_call_mode_info_s_type  *mode_info_p,
                           cm_end_params_s_type            *end_parms_p
                          );

  /*---------------------------------------------------------------------------
   Called to indicate deactivate of Bearer Context. Tears down the data path
   including deregistration with lower layers if needed, and changes state to
   DOWN. Notifies PdnContext of state change. 
  ---------------------------------------------------------------------------*/
  void (*DS_BEARER_CTXT_VF_FPN(deactivateCnf))(
                                     ds_bearer_context_s  *bearer_context_p);

  /*---------------------------------------------------------------------------
    Dormancy reorig actions happen here. This could be any mode specific 
    functions that have to be done. Note that this fn pointer is NULL for GPRS.
  ---------------------------------------------------------------------------*/
  void (*DS_BEARER_CTXT_VF_FPN(dormancyReorig))(
                            ds_bearer_context_s        *bearer_context_p,
                            dsm_item_type*              reorig_pkt_ptr
                          );

  /*---------------------------------------------------------------------------
    Virtual function to configure PdpPPP - PPP is not supported by GPRS
  ---------------------------------------------------------------------------*/
  void (*DS_BEARER_CTXT_VF_FPN(configurePdpPpp))(
		                      ds_bearer_context_s  *bearer_context_p,
                              ppp_dev_opts_type    *ppp_config_info_p
		                   );
  /*--------------------------------------------------------------------------
    This function is to perform any mode specific operations on Rab re-estab
    indication.
  ---------------------------------------------------------------------------*/
  boolean (*DS_BEARER_CTXT_VF_FPN(rab_re_estab_ind))(
                                  ds_bearer_context_s* bearer_context_p,
                                  cm_call_mode_info_s_type  *mode_info_ptr                                  
                               );
  /*--------------------------------------------------------------------------
    Mode specific Suspend indication handler
  ---------------------------------------------------------------------------*/
  void (*DS_BEARER_CTXT_VF_FPN(suspend_ind))(                   
                          ds_bearer_context_s* bearer_context_p  
                        );

  /*--------------------------------------------------------------------------
    Mode specific Resume indication handler
  ---------------------------------------------------------------------------*/
  void (*DS_BEARER_CTXT_VF_FPN(resume_ind))(
                        ds_bearer_context_s  *bearer_context_p
                      );

  /*--------------------------------------------------------------------------
    Mode specific Resume indication handler
  ---------------------------------------------------------------------------*/
  void (*DS_BEARER_CTXT_VF_FPN(empty_watermark))(
                        ds_bearer_context_s  *bearer_context_p
                      );

  /*--------------------------------------------------------------------------
    Mode specific context transfer indication handlers
    release hdlr is called in SRC RAT, setup hdlr in TARGET RAT.
  ---------------------------------------------------------------------------*/
  void (*DS_BEARER_CTXT_VF_FPN(cntxt_transfer_ind_setup))(
                        ds_bearer_context_s  *bearer_context_p
                      );

  void (*DS_BEARER_CTXT_VF_FPN(cntxt_transfer_ind_release))(
                        ds_bearer_context_s  *bearer_context_p
                      );

  int (*DS_BEARER_CTXT_VF_FPN(getBearerTechType)) (
                        ps_iface_bearer_technology_type    *bearer_tech_ptr,
                        sint15                             *ps_errno,
                        sys_modem_as_id_e_type              subs_id
                      );

#ifdef FEATURE_DATA_WLAN_MAPCON
  int (*DS_BEARER_CTXT_VF_FPN(getBearerTechTypeEx)) (
                        ds_sys_system_status_info_type    *bearer_tech_ex_ptr,
                        sys_modem_as_id_e_type             subs_id
                      );

  uint64 (*DS_BEARER_CTXT_VF_FPN(getSoMask)) (
                        sys_modem_as_id_e_type             subs_id
                      );
#endif /* FEATURE_DATA_WLAN_MAPCON */

  int (*DS_BEARER_CTXT_VF_FPN(getBearerRate)) (
                        uint32                   call_inst,
                        void                    *argval_ptr,
                        sint15                  *ps_errno,
                        sys_modem_as_id_e_type   subs_id
                      );
  
#ifdef FEATURE_QXDM_WM_TRACING
  void (*DS_BEARER_CTXT_VF_FPN(report_wm_stats)) (
                        uint32 call_instance,
                        uint32 call_id,
                        fc_wm_stat_type *wm_stat_ptr
                       );
#endif /* FEATURE_QXDM_WM_TRACING */

  uint32 (*DS_BEARER_CTXT_VF_FPN(getratMaskforHs))(
                         sys_hs_ind_e_type       hs_status,
                         sys_modem_as_id_e_type  subs_id);

  uint32 (*DS_BEARER_CTXT_VF_FPN(getratMaskforEgprsSupport))(
                         sys_egprs_support_e_type  egprs_support);

  cm_cell_srv_ind_s_type 
         (*DS_BEARER_CTXT_VF_FPN(getCellSrvInd))(
                         sys_modem_as_id_e_type  subs_id);

  void   (*DS_BEARER_CTXT_VF_FPN(setCellSrvInd))(
                         cm_cell_srv_ind_s_type  cell_srv_ind,
                         sys_modem_as_id_e_type  subs_id);

#ifdef FEATURE_WCDMA
  void   (*DS_BEARER_CTXT_VF_FPN(purgecmdcbhdr))(
	               ds_bearer_context_purge_cnf_ev_type *ds3gpp_purge_cnf_ev);
#endif /* FEATURE_WCDMA */
  
  ps_iface_type* (*DS_BEARER_CTXT_VF_FPN(complogfetchiface))(
                           ps_comp_logging_instance_config_s *logging_info_ptr,
                           ip_version_enum_type ip_version);

  void   (*DS_BEARER_CTXT_VF_FPN (report_ul_throughput_quality))
                    (ds_3gpp_throughput_quality_s* throughput_quality_info_p);

  boolean (*DS_BEARER_CTXT_VF_FPN (refresh_actual_throughput))
                    (ds_bearer_context_s        *bearer_context_p, 
                     sys_modem_as_id_e_type      cm_subs_id ,
                     uint32                      sampling_interval);
  void (*DS_BEARER_CTXT_VF_FPN (init_bearer_throughput_stats))
                    (ds_bearer_context_s* bearer_context_p);
  /*--------------------------------------------------------------------------
    This function is to perform any mode specific operations on mode_pref_change
  ---------------------------------------------------------------------------*/
  void (*DS_BEARER_CTXT_VF_FPN(mode_pref_change))(
                                  ds_bearer_context_s* bearer_context_p                               
                               );
} ds_umts_bearer_context_ftbl_s;

/*-----------------------------------------------------------------------------
  This structure contains all info needed in the UMTS bearer context.
-----------------------------------------------------------------------------*/
typedef struct
{    
  /*! Cookie to ensure dyn mem is valid */
  uint32                           cookie;  
  ds_umts_umts_qos_params_type umts_qos_val; /*!< UMTS specific QOS values */
  ds_umts_gprs_qos_params_type gprs_qos_val; /*!< GPRS specific QOS values */
  uint32                       src_stat_desc;
  protocol_cfg_options_T       pco;          /*!< UMTS specific Incoming call PCO */
  int                          index;
  uint8                        u_lc_id;      /*!< uplink logical channel id */
  uint8                        d_lc_id;      /*!< downlink logical channel id */
} ds_umts_bearer_context_dyn_s;



typedef struct
{    
  ds_umts_bearer_context_dyn_s* ds_umts_bearer_context_dyn_p; 
} ds_umts_bearer_context_s;

/*-----------------------------------------------------------------------------
  UMTS bearer context table exposed outside through header 
-----------------------------------------------------------------------------*/
extern ds_umts_bearer_context_s umts_bearer_context_tbl[DS_3GPP_MAX_BEARER_CONTEXT];

/*-----------------------------------------------------------------------------
  wcdma/gprs specific mode handler exposed outside through header
-----------------------------------------------------------------------------*/
extern ds_umts_bearer_context_ftbl_s umts_bearer_context_ftbl[DS_3GPPI_SYS_MODE_MAX];

/*-------------------------------------------------------------------------
Cookie to ensure that dynamic memory allocated is valid
---------------------------------------------------------------------------*/
#define UMTS_BEARER_CNTXT_VALID_COOKIE  0x1bea1bea


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
#define DS_UMTS_BEARER_CTXT_VF_FPN(func) DS_3GPPI_VF_FPN(func)

#define DS_UMTS_BEARER_CTXT_VF_REG(mode, func, fp) \
          DS_3GPPI_VF_REG(umts_bearer_context_ftbl, mode, func, fp)       

            
/*===========================================================================
FUNCTION DSUMTSBEARERCNTXT_INIT

DESCRIPTION
  This function is used to initialize the UMTS part of the bearer context. 

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None.

===========================================================================*/
void dsUmtsBearerCntxt_init( void );


boolean dsUmtsBearerCntxt_free_umts_bearer(
   ds_bearer_context_s*        bearer_context_p
);


boolean dsUmtsBearerCntxt_allocate_umts_bearer(
   int loop_index
);

/*===========================================================================
FUNCTION dsUmtsBearerCntxt_examine_if_throt_needed

DESCRIPTION
  This function is used to examine the cause code of rejection and determine
  if we need to perform throttling.

PARAMETERS
  call_info_ptr  - Pointer to the call info to find out rejection cause

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if throttling is needed
  FALSE otherwise. 
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean dsUmtsBearerCntxt_examine_if_throt_needed
(
  dsgwl_ps_call_info_u_type *call_info_ptr,
  sys_modem_as_id_e_type   subs_id
);

/*===========================================================================
FUNCTION DSUMTSBEARERCNTXT_GETANSWERPARAMS

DESCRIPTION
  This function is used to fill the answer parameters for an MT call.

PARAMETERS
  IN
    bearer_context_p - The bearer context we are working with
    answer_params_p  - The structure from CM that has to be filled in with
                       answer parameters.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean - True on Success. False on any failure.
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean dsUmtsBearerCntxt_getAnswerParams
(
  ds_bearer_context_s  *bearer_context_p,
  cm_ans_params_s_type *answer_params_p
);

/*===========================================================================
FUNCTION DSUMTSBEARERCNTXT_CONFIGUREPDPPPP

DESCRIPTION
  Add function header: This is used to fill up the PPP options with tx and rx
  function call backs. This function is called from UMTS PDN context as a part
  of configuring PPP. RMSM PPP calls PDN context to configure PPP which in turn
  results in this call.

PARAMETERS
  IN
    bearer_context_p  - This is the bearer that is used for this PPP call
    ppp_config_info_p - This is the pointer to PPP configuration block.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void dsUmtsBearerCntxt_configurePdpPpp
(
  ds_bearer_context_s* bearer_context_p,
  ppp_dev_opts_type* ppp_config_info_p
);

#ifdef FEATURE_NAS_REL7
/*===========================================================================
FUNCTION DSUMTSPS_CONVERT_DATA_RATE_TO_R7_EXT_ENCODE_VAL

DESCRIPTION
  This function converts the given REL7 data rate from an unsigned integer
  to a one byte value as specified in TS24.008 (10.5.6.5)

  NOTE: The return value is a 'byte' so that it can be used as-is to
  encode the QOS parameters in the message.

PARAMETERS
  data_rate : data rate in kbps

DEPENDENCIES
  None.

RETURN VALUE
  one byte value indicating the data rate as per encoding rules

SIDE EFFECTS
  None.

===========================================================================*/
uint8 dsUmtsBearerCntxt_convert_data_rate_to_r7_ext_encode_val
(
  uint32  data_rate
);
#endif /*FEATURE_NAS_REL7*/

/*===========================================================================
FUNCTION DSUMTSBEARERCNTXT_CONVERT_DATA_RATE_TO_R5_EXT_ENCODE_VAL

DESCRIPTION
  This function converts the given REL5 data rate from an unsigned integer
  to a one byte value as specified in TS24.008 (10.5.6.5)

  NOTE: The return value is a 'byte' so that it can be used as-is to
  encode the QOS parameters in the message.

PARAMETERS
  data_rate : data rate in kbps

DEPENDENCIES
  None.

RETURN VALUE
  one byte value indicating the data rate as per encoding rules

SIDE EFFECTS
  None.

===========================================================================*/
uint8 dsUmtsBearerCntxt_convert_data_rate_to_r5_ext_encode_val
(
  uint32  data_rate
);

#ifdef FEATURE_DATA_RATE_MAPPING
/*===========================================================================
FUNCTION DSUMTSBEARERCNTXT_MAP_MAX_BIT_RATE_TO_ALLOWED_RATE

DESCRIPTION
  This function maps the max bit rate passed in by the application to
  one of the allowed bit rates.
  Match the request for UL/DL Max data rates to the nearest rate from the
  set {16, 32, 64, 128, 256 and 384} kbps

PARAMETERS
  data_rate : data rate in kbps

DEPENDENCIES
  None.

RETURN VALUE
  maps max bit rate to allowed rate

SIDE EFFECTS
  None.

===========================================================================*/
uint32 dsUmtsBearerCntxt_map_max_bit_rate_to_allowed_rate
(
  uint32  data_rate
);
#endif /*FEATURE_DATA_RATE_MAPPING*/

/*===========================================================================
FUNCTION DSUMTS_BEARERCNTXT_CONVERT_DATA_RATE_TO_R99_ENCODE_VAL

DESCRIPTION
  This function converts the given data rate from an unsigned integer
  to a one byte value as specified in TS24.008 (10.5.6.5)

  NOTE: The return value is a 'byte' so that it can be used as-is to
  encode the QOS parameters in the message.

PARAMETERS
  data_rate : data rate in kbps

DEPENDENCIES
  None.

RETURN VALUE
  one byte value indicating the data rate as per encoding rules

SIDE EFFECTS
  None.

===========================================================================*/
uint8 dsUmtsBearerCntxt_convert_data_rate_to_r99_encode_val
(
  uint32  data_rate
);

/*===========================================================================
FUNCTION DS_TDS_W_BEARER_CNTXT_CALCULATE_ACTUAL_THROUGHPUT

DESCRIPTION
  This function calculates the actual throughput for all active UMTS bearers 

PARAMETERS
  ds_bearer_context_s*    -  Pointer to the bearer context
  sys_modem_as_id_e_type  -  Subscription ID
  uint32                  -  Sampling interval in msec

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - If the actual throughput values have been calculated successfully
  FALSE - Otherwise
 
SIDE EFFECTS
  None.

===========================================================================*/

boolean ds_tds_w_bearer_cntxt_calculate_actual_throughput
(
  ds_bearer_context_s       *bearer_context_p,
  sys_modem_as_id_e_type     cm_subs_id,
  uint32                     sampling_interval
);

#ifdef FEATURE_QXDM_WM_TRACING
/*===========================================================================
FUNCTION DS_UMTS_BEARER_CNTXT_REPORT_WM_STATS_CB

DESCRIPTION
  This function internally call wcdma or gsm related function.

PARAMETERS
  bearer_instance - bearer context instance
  wm_stat_ptr     - pointer where we store the wm values

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void dsUmtsBearerCntxt_reportWmStats
(
  uint32          bearer_instance,
  fc_wm_stat_type *wm_stat_ptr
);

#endif /* FEATURE_QXDM_WM_TRACING */

/*===========================================================================
MACRO DSUMTSBEARERCNTXT_IS_CM_PROV_NETW_QOS_VALID

DESCRIPTION   
  Checks validilty of Cm provided Netw Allocated QoS 

PARAMETERS
  qos_T *netw_qos_ptr;

DEPENDENCIES  
  None.

RETURN VALUE  
  boolean
  
LIMITATIONS

===========================================================================*/
#define DSUMTSBEARERCNTXT_IS_CM_PROV_NETW_QOS_VALID( netw_qos_ptr ) \
                  ( ( (netw_qos_ptr) == NULL ) \
                    ?FALSE \
                    :(netw_qos_ptr)->valid \
                   )

/*===========================================================================
FUNCTION dsUmtsBearerCntxt_SendQOSIndication

DESCRIPTION
  This function sends the traffic class information of a particular bearer to interested
  parties.
  
PARAMETERS  
  bearer_context_p - Bearer Context
  system_rat - Current system information.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void dsUmtsBearerCntxt_SendQOSIndication
(
   ds_bearer_context_s *bearer_context_p,
   sys_radio_access_tech_e_type system_rat
);

/*===========================================================================
FUNCTION dsUmtsBearerCntxt_validatebearer

DESCRIPTION
  This function checks if the umts bearer context has been allocated dynamic memory

PARAMETERS
  umts_bearer_context_p  -umts_bearer_context_p to be validated
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/


boolean dsUmtsBearerCntxt_validatebearer
(
  ds_umts_bearer_context_s *umts_bearer_context_p
);

/*===========================================================================
FUNCTION dsUmtsBearerCntxtGetPsRatMaskForHsStatus

DESCRIPTION
  This function returns the RAT MASK corresponding to the passed HS status.

PARAMETERS
  hs_status  - the HS status
  subs_id    - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  RAT mask 

SIDE EFFECTS
  None.
  
===========================================================================*/
uint32
dsUmtsBearerCntxtGetPsRatMaskForHsStatus
(
  sys_hs_ind_e_type       hs_status,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION dsUmtsBearerCntxtGprsGetratMaskforEgprsSupport

DESCRIPTION
  This function returns the RAT MASK corresponding to the passed egprs support.

PARAMETERS
  egprs_support - egprs_supported or not
  subs_id       - Subscription Id
  
DEPENDENCIES
  None.

RETURN VALUE
  RAT mask 

SIDE EFFECTS
  None.

===========================================================================*/
uint32 dsUmtsBearerCntxtGprsGetratMaskforEgprsSupport
(
  sys_egprs_support_e_type  egprs_support,
  sys_modem_as_id_e_type    subs_id
);

/*===========================================================================
FUNCTION dsUmtsBearerCntxtGetCellSrvInd

DESCRIPTION
  This function returns the cached cell srv ind.

PARAMETERS
  subs_id    - Subscription Id
  
DEPENDENCIES
  None.

RETURN VALUE
  cell srv ind 

SIDE EFFECTS
  None.
  
===========================================================================*/
cm_cell_srv_ind_s_type dsUmtsBearerCntxtGetCellSrvInd
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION dsUmtsBearerCntxtSetCellSrvInd

DESCRIPTION
  This function sets the cached cell srv ind.

PARAMETERS
  cell_srv_ind - passed cell srv ind
  subs_id      - Subscription Id
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void dsUmtsBearerCntxtSetCellSrvInd
(
  cm_cell_srv_ind_s_type  cell_srv_ind,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION DS_UMTS_COMP_LOG_FETCH_IFACE

DESCRIPTION
  This Functions returns the iface corresponding to link control id.

PARAMETERS
  logging_info_ptr - the Log information. 
  ip_version       - ip version v4 or v6.

DEPENDENCIES
  None.

RETURN VALUE
  Iface.

SIDE EFFECTS
   None.

===========================================================================*/
ps_iface_type* ds_umts_bearer_cntxt_comp_log_fetch_iface
(
  ps_comp_logging_instance_config_s *logging_info_ptr,
  ip_version_enum_type               ip_version
);

/*===========================================================================
FUNCTION  dsUmtsBearerCntxt_teardown_all_calls

DESCRIPTION
  This function handles the system change from UMTS to CDMA 
  and tears down the calls existing in UMTS mode

PARAMETERS     
  subs_id    - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void dsUmtsBearerCntxt_teardown_all_calls
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DSUMTSBEARERCNTXT_ACTIVATION

DESCRIPTION
  This function is responsible for Posting Bearer Up Indication once
  registration with lower layers is incomplete

PARAMETERS
  bearer_context_p: Bearer context which has to be activated

DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS: If Successful
  DS3G_FAILURE: If Activation fails

SIDE EFFECTS
  None.

===========================================================================*/
boolean dsUmtsBearerCntxt_activation
(
  ds_bearer_context_s                          *bearer_context_p
);
/*==========================================================================
FUNCTION DSUMTSBEARERCNTXT_CONV_QOS_NETW_TO_UMTS

DESCRIPTION
  Given a set of network allocated QoS, converts them into UMTS
  specific QoS. 

PARAMETERS
  (->) *netw_qos_ptr    - Ptr to the network allocated QOS.
  (<-) *umts_qos_ptr    - Ptr to App QOS in Call table

DEPENDENCIES
  None.

RETURN VALUE
  TRUE on SUCCESS
  FALSE on FAILURE

SIDE EFFECTS
  None.

===========================================================================*/
boolean dsUmtsBearerCntxt_conv_qos_netw_to_umts
(
  const qos_T                  *netw_qos_ptr,
  ds_umts_umts_qos_params_type *umts_qos_ptr
);
/*==========================================================================
FUNCTION DSUMTSBEARERCNTXT_CONV_QOS_NETW_TO_GPRS

DESCRIPTION
  Given a set of network encoded QoS, decode GPRS QoS paramters

PARAMETERS
  (->) *netw_qos_ptr    - Ptr to the network allocated QOS.
  (<-) *gprs_qos_ptr    - Ptr to GPRS QoS Blob in DS Call table

DEPENDENCIES
  None.

RETURN VALUE
  0 on SUCCESS
 -1 on FAILURE

SIDE EFFECTS


===========================================================================*/

boolean dsUmtsBearerCntxt_conv_qos_netw_to_gprs
(
  const qos_T                  *netw_qos_ptr,
  ds_umts_gprs_qos_params_type *gprs_qos_ptr
);

/*===========================================================================
FUNCTION  DSUMTSBEARERCNTXT_GETENDPARAMS

DESCRIPTION
  This function is used to populate the end params structure passed to us by
  CM. This function is called as a part of the call ended handler implemented
  in the 3gpp_hdlr.

PARAMETERS   :
  IN
  *bearer_context_p         - Ptr to the bearer that has to be worked with.
  
  OUT
  *end_params_p             - Ptr to the end params structure passed to us.
  *ds3gi_capp_info_p        - Ptr to the call info structure.
  *delay_disconnect_p       - Ptr to the boolean that lets the caller know if
                              there is a need for a delayed disconnect.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dsUmtsBearerCntxt_getEndParams
(
  ds_bearer_context_s              *bearer_context_p,
  cm_end_params_s_type             *end_params,
  ds_3gpp_pdn_call_info_type       *ds3gi_call_info,
  boolean                          *delay_disconnect
);



#endif /* defined (FEATURE_DATA_WCDMA_PS) || defined (FEATURE_GSM_GPRS) */
#endif /* DSUMTSBEARERCONTEXT_H */

