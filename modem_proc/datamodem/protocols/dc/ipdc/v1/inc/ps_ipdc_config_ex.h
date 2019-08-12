#ifndef PS_IPDC_CONFIG_EX_H
#define PS_IPDC_CONFIG_EX_H

#ifdef __cplusplus
extern "C" {
#endif

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    P S _ I P D C _ C O N F I G . H

GENERAL DESCRIPTION
  This file contains the logic to fetch IPDC configuration parameters via 
  QXDM command mechanism.
  The QXDM commands used would fall in the DIAG_SUBSYS_PS subsystem.
  Different commands to configure various IPDC configuration parameters would
  be differentiated via command codes listed in this header file.
 
Copyright (c) 2013 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary  

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "ps_ipdc_platform_defs.h"

#include "ps_ipdc_ex.h"

/*===========================================================================

                        MACRO DEFINITIONS

===========================================================================*/
#define PS_IPDC_CONFIG_DEF_VAL_PARAM_PER_PKT_LOGGING            FALSE
#define PS_IPDC_CONFIG_DEF_VAL_PARAM_LOG_COMP_PKTS              FALSE
#define PS_IPDC_CONFIG_DEF_VAL_PARAM_LOG_PER_PKT_METRICS        TRUE
#define PS_IPDC_CONFIG_DEF_VAL_PARAM_PKT_BETWEEN_COMP_MEM_DUMP  0
#define PS_IPDC_CONFIG_DEF_VAL_PARAM_PKT_BETWEEN_STATS_LOG_PKT  100
#define PS_IPDC_CONFIG_DEF_VAL_PARAM_STATS_F3_LOGGING           FALSE
#define PS_IPDC_CONFIG_DEF_VAL_PARAM_RESET_STATS_AFTER_LOGGING  FALSE


/*===========================================================================

                           DATA TYPES

===========================================================================*/
typedef enum 
{
  PS_IPDC_CONFIG_PARAM_LOG_INVALID = 0,

  PS_IPDC_CONFIG_PARAM_LOG_COMP_PKTS              = 1,
  PS_IPDC_CONFIG_PARAM_LOG_PER_PKT_METRICS        = 2,
  PS_IPDC_CONFIG_PARAM_PKT_BETWEEN_COMP_MEM_DUMP  = 3,
  PS_IPDC_CONFIG_PARAM_PKT_BETWEEN_STATS_LOG_PKT  = 4,
  PS_IPDC_CONFIG_PARAM_PER_PKT_LOGGING            = 5,
  PS_IPDC_CONFIG_PARAM_STATS_F3_LOGGING           = 6,
  PS_IPDC_CONFIG_PARAM_RESET_STATS_AFTER_LOGGING  = 7,
  
  PS_IPDC_CONFIG_PARAM_HB_SIZE                                  = 11,
  PS_IPDC_CONFIG_PARAM_HK_ECM_PREALLOC_SIZE                     = 12,
  PS_IPDC_CONFIG_PARAM_DOUBLE_COMP_MEM_FOR_HK_ECM_PREALLOC_SIZE = 13,
  PS_IPDC_CONFIG_PARAM_IPDC_LIB_MAX_SEARCHES_PER_MATCH          = 14,

  /*------------------------------------------------------------------------
    Below parameters are not configuration items, but commands instead.
    Adding these to config params enum as well to maintain centralized 
    processing.
  -------------------------------------------------------------------------*/
  /* Reset the compressor statistics */
  PS_IPDC_CONFIG_PARAM_CMD_RESET_COMP_STATS       = 101,

  /* Command to immediately log the compressor stats LOG packet.
     Also prints the statistics via F3 prints. */
  PS_IPDC_CONFIG_PARAM_CMD_LOG_COMP_STATS_NOW     = 102,

  PS_IPDC_CONFIG_PARAM_MAX
}ps_ipdc_config_param_type_e;

typedef union
{

  boolean  ps_ipdc_config_param_log_comp_pkts;
  boolean  ps_ipdc_config_param_log_per_pkt_metrics;
  uint16   ps_ipdc_config_param_pkt_between_comp_mem_dump;
  uint16   ps_ipdc_config_param_pkt_between_stats_log_pkt;
  boolean  ps_ipdc_config_param_per_pkt_logging;
  boolean  ps_ipdc_config_param_stats_f3_logging;
  boolean  ps_ipdc_config_param_reset_stats_after_logging;

  ipdc_hb_size_e_type               ps_ipdc_config_param_hb_size;
  ipdc_hk_ecm_prealloc_size_e_type  ps_ipdc_config_param_hk_ecm_prealloc_size;

  boolean  ps_ipdc_config_param_double_comp_mem_for_hk_ecm_prealloc_size;
  uint16   ps_ipdc_config_param_ipdc_lib_max_searches_per_match;
  
}ps_ipdc_config_param_value_u;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION ps_ipdc_config_init()

DESCRIPTION 
  This function will initialize the IPDC configuration module.
  It registers with the DIAG module for the DIAG_SUBSYS_PS subsystem along 
  with the command codes specified in ps_ipdc_config_diag_cmd_tbl

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS 
  None.

===========================================================================*/
void ps_ipdc_config_init_ex
( 
  void
);

/*===========================================================================
FUNCTION ps_ipdc_config_get_ex()

DESCRIPTION 
  This function will fetch the IPDC configuration parameter values.
  These values would be automatically updated when user keys in the specific
  QXDM command.

DEPENDENCIES
  The IPDC config module should have been initialized.

PARAMETERS
  config_param : IPDC configuration parameter whose vlaue is to be fetched.

RETURN VALUE
  Value of the specified configuration parameter.

SIDE EFFECTS 
  None.

===========================================================================*/
ps_ipdc_config_param_value_u ps_ipdc_config_get_ex
( 
  ps_ipdc_config_param_type_e config_param
);

/*===========================================================================
FUNCTION ps_ipdc_config_set()

DESCRIPTION 
  This function will set the IPDC configuration parameter values.

DEPENDENCIES
  The IPDC config module should have been initialized.

PARAMETERS
  config_param : IPDC configuration parameter whose vlaue is to be updated.
  config_value : Value to set for the configuration item.

RETURN VALUE
  None.

SIDE EFFECTS 
  None. 
===========================================================================*/
void ps_ipdc_config_set
( 
  ps_ipdc_config_param_type_e   config_param,
  ps_ipdc_config_param_value_u  config_value
);

/*===========================================================================
THIS API IS JUST EXPOSED FOR OFF TARGET VALIDATION

FUNCTION PS_IPDC_CONFIG_DIAG_CB()

DESCRIPTION
  This function is the registered subsystem command handler for updating IPDC
  configuration parameters.  It validates the incoming DIAG command, posts a
  command to the protocol stack, and dispatches a subsystem response packet.

  This runs in the DIAG context.
  
  Sample usage from QXDM to invoke this callback:
  send_data 75 42 51 0 <config-param-to-update> <config-param-value> 
  config-param-to-update : value from ps_ipdc_config_param_type_e enum
  config-param-value		 : value in number of bytes required

PARAMETERS
  *req_pkt: A pointer to the subsystem command packet.
  pkt_len:  Length of the subsystem command packet.

RETURN VALUE
  None

DEPENDENCIES
  This function must have been registered in the diag table in order to be
  invoked when the appropriate subsystem command is recognized.

SIDE EFFECTS
  Dispatches a subsystem command response packet.
===========================================================================*/
PACKED void* ps_ipdc_config_diag_cb_ex
(
  PACKED void*   req_pkt,
  uint16  pkt_len
);

#ifdef __cplusplus
}
#endif

#endif  // PS_IPDC_CONFIG_EX_H
