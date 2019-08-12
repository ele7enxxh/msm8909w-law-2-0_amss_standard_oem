#ifndef FC_DSM_TRACE_H__
#define FC_DSM_TRACE_H__
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
01/09/2010  kvk     Initial version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "utils_variation.h"
#include "comdef.h"

#ifdef CUST_H
#include "customer.h"
#endif

#ifdef FEATURE_QXDM_DSM_TRACING
#include "dsm.h"
#endif

#ifdef FEATURE_QXDM_DSM_TRACING

#define MAX_DSM_CLNT_CNT 5

#define MAX_DSM_POOL_CNT 3


typedef enum
{
  DSM_UMTS_TECH,
  DSM_C2K_TECH,
  DSM_GSM_TECH,
  DSM_LTE_TECH,
  DSM_UNKNOWN_TECH,
  MAX_DSM_TECH
}fc_dsm_trace_tech_id_enum_type;



typedef enum
{
  UMTS_RLC_CLIENT,
  LTE_RLC_CLIENT,
  DSM_MAX_CLIENT
}fc_dsm_trace_client_id_enum_type;

typedef enum
{
  FC_LARGE_POOL,
  FC_SMALL_POOL,
  FC_DUP_POOL,
  FC_MAX_POOL_ID
}fc_dsm_pool_id_type;


typedef struct 
{
  uint16   few_event_cnt;
  uint16   many_event_cnt;
  uint16   dne_event_cnt;
}fc_dsm_event_hits_stat_type;


typedef struct
{
  uint32                                 few_threshold;
  uint32                                 many_threshold;
  uint32                                 dne_threshold;
}fc_dsm_threshold_stat_type;

typedef struct 
{
  fc_dsm_pool_id_type              dsm_pool;
  dsm_pool_counts_type           dsm_pool_counts;
  fc_dsm_event_hits_stat_type  dsm_event_hits;
  fc_dsm_threshold_stat_type    dsm_threshold_stats;
}fc_dsm_counts_type;

typedef struct
{
  fc_dsm_trace_tech_id_enum_type    tech_id;
  fc_dsm_trace_client_id_enum_type   client_id;
  uint8                                           num_pools;
  fc_dsm_counts_type                     dsm_counts[MAX_DSM_POOL_CNT];
}fc_dsm_trace_info_type;

typedef struct 
{
  uint8                            num_clnt;
  uint8                            mux_flag;
  fc_dsm_trace_info_type  dsm_info[MAX_DSM_CLNT_CNT];
}fc_dsm_stat_type;



typedef void (*fc_get_dsm_trace_cb_type)(uint32 cb_data, fc_dsm_trace_info_type *wm_stat_ptr);

/*===========================================================================
FUNCTION: fc_post_reg_cmd_dsm_trace

DESCRIPTION:
  This function register with FC whenever DSM trace needs to be monitored.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/

void fc_post_reg_cmd_dsm_trace(fc_dsm_trace_tech_id_enum_type tech_id,
                                                              fc_dsm_trace_client_id_enum_type client_id,
                                                              fc_get_dsm_trace_cb_type  wm_trace_cb_ptr, 
                                                              uint32 cb_data);



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

void fc_post_deregister_cmd_dsm_trace(fc_dsm_trace_tech_id_enum_type tech_id,
                                                                           fc_dsm_trace_client_id_enum_type client_id);

#endif/*FEATURE_QXDM_DSM_TRACING*/
#endif/*FC_WM_H__*/

