#ifndef FC_WM_H__
#define FC_WM_H__
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              F L O W    C O N T R O L L E R     H E A D E R    F I L E

DESCRIPTION
  Public Header file for the Flow Contoller. Contains types and externalized
  functions of the Flow Controller module.

Copyright (c) 2000-2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header:

  when       who      what, where, why
--------   -------- --------------------------------------------------------
02/01/10    kvk      Added enums for SNDCP<-->PS watermarks
02/08/10    kvk      Added support for QXDM based DSM and WM tracing
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "utils_variation.h"
#include "comdef.h"

#ifdef CUST_H
#include "customer.h"
#endif

#ifdef FEATURE_QXDM_WM_TRACING
#include "dsm.h"
#endif

#ifdef FEATURE_QXDM_WM_TRACING

#define MAX_WM_CNT 20


typedef enum
{
  UMTS_TECH,
  C2K_TECH,
  GSM_TECH,
  LTE_TECH,
  UNKNOWN_TECH,
  MAX_TECH
}fc_wm_tech_id_enum_type;

typedef enum
{
  WM_DIR_RLC_PDCP_DL,
  WM_DIR_PDCP_PS_DL,
  WM_DIR_RLC_PS_DL,
  WM_DIR_PS_SIO_DL,
  WM_DIR_RLC_PDCP_UL,
  WM_DIR_PDCP_PS_UL,
  WM_DIR_RLC_PS_UL,
  WM_DIR_PS_SIO_UL,
  WM_DIR_RM_RX,
  WM_DIR_RM_TX,
  WM_DIR_PRIMARY_UM_TX,
  WM_DIR_SECONDARY_UM_TX,
  WM_DIR_UM_RX,
  WM_DIR_SHIM_TX,
  WM_DIR_SHIM_RX,
  WM_DIR_RMNET_SIO_DL,
  WM_DIR_RMNET_SIO_UL,
  WM_DIR_SNDCP_PS_DL,
  WM_DIR_SNDCP_PS_UL,
  WM_DIR_A2_PS_DL,
   WM_DIR_MAX
}fc_wm_id_enum_type;

typedef enum
{
  UMTS_MODE_HANDLER_CLIENT,
  CDMA_1x_MODE_HANDLER_CLIENT,
  PDCP_CLIENT,
  RLP_CLIENT,
  RMSM_CLIENT,
  SMD_CLIENT,
  USB_CLIENT,
  RMNET_CLIENT,
  UNIFIED_3GPP_MODE_HANDLER_CLIENT,
  MAX_CLIENT
}fc_wm_client_id_enum_type;

typedef struct
{
  fc_wm_tech_id_enum_type   tech_id;
  uint32                        call_id;
  fc_wm_id_enum_type  wm_id;
  uint8                        mux_flag;
  dsm_wm_counts_type wm_counts;
}fc_wm_info_type;

typedef struct 
{
  uint8 num_wms;
  fc_wm_info_type wm_info[MAX_WM_CNT];
}fc_wm_stat_type;



typedef void (*fc_get_wm_trace_cb_type)(uint32 cb_data, fc_wm_stat_type *wm_stat_ptr);

/*===========================================================================
FUNCTION: fc_post_reg_cmd_wm_trace

DESCRIPTION:
  This function register with FC when ever PS is established

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/

void fc_post_reg_cmd_wm_trace(fc_wm_tech_id_enum_type tech_id,
                                                             uint32 call_id,
                                                             fc_wm_client_id_enum_type client_id,
                                                             fc_get_wm_trace_cb_type wm_trace_cb_ptr, 
                                                             uint32 cb_data);

/*===========================================================================
FUNCTION: fc_post_modify_cmd_wm_trace

DESCRIPTION:
  This function modify with FC when ever PS is released

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/

void fc_post_modify_cmd_wm_trace(uint32 call_id,
                                                                   fc_wm_client_id_enum_type client_id,
                                                                   fc_wm_tech_id_enum_type tech_id);

/*===========================================================================
FUNCTION: fc_post_deregister_cmd_wm_trace

DESCRIPTION:
  This function denregister with FC when ever PS is released

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/

void fc_post_deregister_cmd_wm_trace(uint32 call_id,
                                                                         fc_wm_client_id_enum_type client_id);

#endif/*FEATURE_QXDM_WM_TRACING*/
#endif/*FC_WM_H__*/

