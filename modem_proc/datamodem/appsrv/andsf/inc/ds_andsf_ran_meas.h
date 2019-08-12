/*===========================================================================
                      DS_ANDSF_RAN_MEAS.H

DESCRIPTION
 signal quality measurement header file for ANDSF.

EXTERNALIZED FUNCTIONS

 Copyright (c) 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who                            what, where, why
--------    -------                ----------------------------------------
03/03/15    Youjunf                          First version of file
===========================================================================*/
#ifndef DS_ANDSF_RAN_MEAS_H
#define DS_ANDSF_RAN_MEAS_H

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "datamodem_variation.h"
#ifdef FEATURE_DATA_ANDSF_RULE_MGR

#include "ds_sys.h"
#include "cm.h"
/*===========================================================================
                   EXTERNAL DEFINITIONS AND TYPES
===========================================================================*/
#define LTE_MEAS_REQUIRED   0x1<<0
#define WLAN_MEAS_REQUIRED  0x1<<1
#define CDMA_MEAS_REQUIRED  0x1<<2
#define ANDSF_MEAS_ID       1
typedef struct
{
  ds_sys_subscription_enum_type subs_id;
  char                          apn_name[DS_SYS_MAX_APN_LEN];
  uint8                         call_type;
} ds_andsf_call_info_s_type;


/*------------------------------------------------------------------------------
  Structure of lte signal quality measurement
------------------------------------------------------------------------------*/
typedef struct
{
  ds_sys_subscription_enum_type  subs_id;
  /* Indicates whether LTE service is available. The remaining parameter values
      are only valid when service is available; otherwise, do not use. */
  boolean                        is_lte_srv_avail;
  int16                          rsrp;         //dbm
  int16                          rsrq;         //dbm
  boolean                        rsrp_avail;
  boolean                        rsrq_avail;
} ds_andsf_lte_meas_info_s_type;


/*------------------------------------------------------------------------------
  Structure of wifi signal quality measurement
------------------------------------------------------------------------------*/
typedef struct
{
  /* Indicates whether wlan service is available. The remaining parameter values
      are only valid when service is available; otherwise, do not use. */
  boolean                        is_wlan_srv_avail;
  int16                          rssi;         //dbm
  int16                          sinr;         //dbm
  uint16                         bss_load;
  boolean                        rssi_avail;
  boolean                        sinr_avail;
  boolean                        bss_load_avail;
} ds_andsf_wlan_meas_info_s_type;


/*------------------------------------------------------------------------------
  Structure of ehrpd signal quality measurement
------------------------------------------------------------------------------*/
typedef struct
{
  ds_sys_subscription_enum_type  subs_id;
  /* Indicates whether ehrpd service is available */
  boolean                        is_ehrpd_srv_avail;
} ds_andsf_ehrpd_meas_info_s_type;


/*------------------------------------------------------------------------------
  Structure of 1x signal quality measurement
------------------------------------------------------------------------------*/
typedef struct
{
  ds_sys_subscription_enum_type  subs_id;
/*Indicates whether cs_1x service available. The remaining parameter values 
  are only valid when service is available; otherwise, do not use.  */
  boolean                        is_1x_avail;
  int16                          ecio;              //dB
  boolean                        ecio_avail;
}ds_andsf_1x_meas_info_s_type;




/*===========================================================================
                           FUNCTION DECLARATIONS
===========================================================================*/
#ifdef __cplusplus
extern "C"
{
#endif


/*===========================================================================
FUNCTION      DS_ANDSF_RAN_MEAS_INIT

DESCRIPTION   This function initializes the andsf ran meas module. This is during 
              power up. 
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/

void ds_andsf_ran_meas_init(void);


void ds_andsf_ran_meas_nv_refresh(uint32 subs_idx);


/*=========================================================================  
FUNCTION DS_ANDSF_SYS_IOCTL_SET_APN_CALL_TYPE

DESCRIPTION
  This function handles andsf set apn type IOCTL from PS

PARAMETERS
  arg_val_ptr[in]- carrys required information
  ps_errno   [in]   - type of error

DEPENDENCIES
  None.

RETURN VALUE
 0 - Success
 -1- Failure

SIDE EFFECTS
  None
================================================================================*/
int ds_andsf_sys_ioctl_set_apn_call_type(void *arg_val_ptr,  int16 *ps_errno);


/*==============================================================================
FUNCTION DS_ANDSF_SYS_IOCTL_WIFI_MEAS_REPORT_CONFIG_STATUS

DESCRIPTION
  This function handles andsf wifi meas report config status IOCTL from PS

PARAMETERS
  arg_val_ptr[in]- carrys required information
  ps_errno   [in]   - type of error

DEPENDENCIES
  None.

RETURN VALUE
 0 - Success
 -1- Failure

SIDE EFFECTS
  None
===============================================================================================*/
int  ds_andsf_sys_ioctl_wifi_meas_report_config_status(void *arg_val_ptr,  int16 *ps_errno);



/*===============================================================================================
FUNCTION DS_SYS_IOCTL_WIFI_MEAS_REPORT

DESCRIPTION
  This function handles andsf wifi meas report IOCTL from PS

PARAMETERS
  arg_val_ptr[in]- carrys required information
  ps_errno   [in]   - type of error

DEPENDENCIES
  None.

RETURN VALUE
 0 - Success
 -1- Failure

SIDE EFFECTS
  None
=============================================================================================*/
int  ds_andsf_sys_ioctl_wifi_meas_report(void *arg_val_ptr,  int16 *ps_errno);


void ds_andsf_process_cm_meas_chg(cm_ss_event_e_type evt, cm_mm_msim_ss_info_s_type *ss_info_ptr);


/*-----------------------------------------------------------------------------
  Post the apps call type with APN information to ANDSF rule manager
------------------------------------------------------------------------------*/
void ds_andsf_notify_update_call_type(const ds_andsf_call_info_s_type *call_info_ptr);


/*-----------------------------------------------------------------------------
  Post the lte signal quality measurement information to ANDSF rule manager
------------------------------------------------------------------------------*/
void ds_andsf_notify_update_lte_meas(const ds_andsf_lte_meas_info_s_type *lte_meas_info_ptr);


/*post the ehrpd signal quality measurement information to ANDSF rule manager*/
void ds_andsf_notify_update_ehrpd_meas(const ds_andsf_ehrpd_meas_info_s_type *ehrpd_meas_info_ptr);


/*------------------------------------------------------------------------------
  Post the wlan signal quality measurement information to ANDSF rule manager
------------------------------------------------------------------------------*/
void ds_andsf_notify_update_wlan_meas(const ds_andsf_wlan_meas_info_s_type *wlan_meas_info_ptr);

/*------------------------------------------------------------------------------
  Post the 1x signal quality measurement information to ANDSF rule manager
------------------------------------------------------------------------------*/
void ds_andsf_notify_update_1x_meas(const ds_andsf_1x_meas_info_s_type *cdma_1x_meas_info_ptr);

/*============================================================================
FUNCTION DS_ANDSF_RAN_MEAS_SET_MEAS_REQUIRED

DESCRIPTION 
  This function sets if measurements are required for each RAT

PARAMETERS
  meas_reqd_bitmask[in]- Bitmask for whether measurement is required

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void ds_andsf_ran_meas_set_meas_required(uint8 meas_reqd_bitmask);

#ifdef __cplusplus
}
#endif

#endif /* FEATURE_DATA_ANDSF_RULE_MGR */
#endif /* DS_ANDSF_RAN_MEAS_H */
