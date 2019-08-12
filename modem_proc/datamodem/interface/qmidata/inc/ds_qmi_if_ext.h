#ifndef DS_QMI_IF_EXT_H
#define DS_QMI_IF_EXT_H
/*===========================================================================

                            D S _ Q M I _ I F _ E X T . H

DESCRIPTION

  The Data Services QMI external interface file.

Copyright (c) 2006-2009,2011,2012,2013,2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmidata/inc/ds_qmi_if_ext.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/08/15    vrk    Added support for LTE-U 256 bands.
03/10/14    vrk    CM SS events handling for SGLTE+G CM redesign
11/27/13    sah    Fix to update PRL info in case of missing prl init event
06/11/12    sb     Fix to move to correct UIM state in case of CARD ERROR
                   event.
02/28/12    sj     Cache TDSCDMA band capability value 
01/25/12    sj     Cache mode_capability from CM 
11/02/11    wc     Add qmi_dms_get_current_prl_info
10/10/11    sj     Add support for caching LTE band capability value 
03/08/11    ua     Retreiving WCDMA/HSDPA/HSUPA default data rates from RRC.
02/15/11    kk     Added support for PRL_ONLY TLV in PRL_INFO message.
10/22/09    kk     Windows7 compliance - WDS changes and BER/PER/FER support.
10/16/09    kk     Windows7 compliance - new messages and ATCoP dependencies.
10/09/09    kk     Windows7 compliance - NAS changes.
10/08/09    kk     Windows7 compliance - DMS changes.
09/05/06    ks     Featurized pin support code, under FEATURE_MMGSDI.
09/05/06    ks     Added qmi_if_mmgsdi_evt_info_type struct. 
06/13/06    ks     Created file
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "cm.h"
#ifdef FEATURE_MMGSDI
  #include "mmgsdilib.h"
#endif /* FEATURE_MMGSDI*/

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                            EXPORTED DEFINITIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*---------------------------------------------------------------------------
  Definitions
---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
  Error rate is supposed to be on a scale of 0 to 10000.
  We use the special value 0xFFFF to signify an unknown error rate.
---------------------------------------------------------------------------*/
#define QMI_IF_UNKNOWN_ERROR_RATE (0xFFFF)

enum current_hs_ind_e
{
  QMI_IF_CURRENT_HS_IND_UNK = 0,        // don't know
  QMI_IF_CURRENT_HS_IND_HS_IND,         // hs_ind
  QMI_IF_CURRENT_HS_IND_HS_CALL_STATUS, // hs_call_status
};

typedef enum
{
  QMI_IF_AS_ID_MIN = 0,
  QMI_IF_AS_ID_1 = QMI_IF_AS_ID_MIN,
  QMI_IF_AS_ID_2,
#if defined(FEATURE_TRIPLE_SIM)
  QMI_IF_AS_ID_3,
#endif /*defined(FEATURE_TRIPLE_SIM)*/
  QMI_IF_AS_ID_MAX
} qmi_if_asubs_id_type;

/*---------------------------------------------------------------------------
      Subset of the CM SS info structure needed by QMI
    ---------------------------------------------------------------------------*/
    typedef struct
    {
      uint64                  changed_fields;
      sys_srv_status_e_type   srv_status;
      sys_srv_domain_e_type   srv_capability;
      sys_sys_mode_e_type     sys_mode;
      boolean                 hdr_hybrid;
      cm_cell_srv_ind_s_type  cell_srv_ind;
      sys_active_prot_e_type  hdr_active_prot;
      boolean                 is_hybr_stack_operational;
    } qmi_if_cm_ss_info_type;

    /*---------------------------------------------------------------------------
      Subset of the CM PH info structure needed by QMI
    ---------------------------------------------------------------------------*/
    typedef struct
    {
      cm_mode_pref_e_type              mode_pref; // TODO: review removal
      sys_oprt_mode_e_type             oprt_mode;
      boolean                          prl_only;
      uint16                           prl_id;
      sys_band_mask_e_type             band_capability;
#ifdef LTE_BAND_NUM
      sys_lte_band_mask_e_type         lte_band_capability;
#else
      sys_band_mask_e_type             lte_band_capability;
#endif /* LTE_BAND_NUM */
      sys_band_mask_e_type             tds_band_capability;
      sys_sys_mode_mask_e_type         mode_capability;
      boolean                          wd_switch_on;
      boolean                          cdma_lock_mode;
      boolean                          is_prl_load_complete;
    } qmi_if_cm_ph_info_type;

/*---------------------------------------------------------------------------
  Subset of the CM Call info structure needed by QMI
---------------------------------------------------------------------------*/
typedef struct
{
  cm_call_event_e_type                 call_event;
  cm_call_type_e_type                  call_type;
  cm_call_id_type                      call_id;
  uint8                                phone_num_len;
  uint8                                phone_num_buf[CM_MAX_NUMBER_CHARS];
  cm_otasp_status_e_type otasp_state;
} qmi_if_cm_call_info_type;

typedef struct
{
  mmgsdi_slot_id_enum_type  slot;
  mmgsdi_pin_info_type      pin;
  mmgsdi_events_enum_type   event;
  mmgsdi_app_enum_type      app_type;
  mmgsdi_card_err_info_enum_type  card_err_info;
  boolean                   activated;
} qmi_if_mmgsdi_evt_info_type;

/*---------------------------------------------------------------------------
  Set of info on the current GSM/WCDMA configuration needed by QMI
---------------------------------------------------------------------------*/
typedef struct
{
  uint8                   gprs_multislot_class;
  uint8                   edge_multislot_class;
} qmi_if_gw_info_type;

/*---------------------------------------------------------------------------
  Set of info on max TX and RX TDSCDMA rate needed by QMI
---------------------------------------------------------------------------*/

typedef struct
{
  uint32                  tx_rate;
  uint32                  rx_rate;
} qmi_if_tds_info_type;

#endif /* DS_QMI_IF_EXT_H */
