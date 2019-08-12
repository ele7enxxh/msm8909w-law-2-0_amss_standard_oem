#ifndef PS_IPDC_CONFIG_H
#define PS_IPDC_CONFIG_H

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
#include "comdef.h"
#include "ps_ipdc.h"

/*===========================================================================

                          DATA TYPES

===========================================================================*/
typedef enum 
{
  PS_IPDC_CONFIG_PARAM_LOG_INVALID = 0,
  PS_IPDC_CONFIG_PARAM_LOG_COMP_PKTS = 1,
  PS_IPDC_CONFIG_PARAM_PKT_BETWEEN_COMP_MEM_DUMP = 2,
  PS_IPDC_CONFIG_PARAM_PKT_IN_METRICS_LOG_PKT = 3,
  PS_IPDC_CONFIG_PARAM_NUM_PKTS_TO_TRIGGER_STATS_LOG_PKT = 4,
  PS_IPDC_CONFIG_PARAM_HB_SIZE = 5,
  PS_IPDC_CONFIG_PARAM_HK_ECM_PREALLOC_SIZE = 6,
  PS_IPDC_CONFIG_PARAM_DOUBLE_COMP_MEM_FOR_HK_ECM_PREALLOC_SIZE = 7,
  PS_IPDC_CONFIG_PARAM_IPDC_LIB_MAX_SEARCHES_PER_MATCH = 8
  
}ps_ipdc_config_param_type_e;

typedef union
{

  boolean ps_ipdc_config_param_log_comp_pkts;
  uint16  ps_ipdc_config_param_pkt_between_comp_mem_dump;
  uint16  ps_ipdc_config_param_pkt_in_metrics_log_pkt;
  uint16  ps_ipdc_config_param_num_pkts_to_trigger_stats_log_pkt;
  ipdc_hb_size_e_type              ps_ipdc_config_param_hb_size;
  ipdc_hk_ecm_prealloc_size_e_type ps_ipdc_config_param_hk_ecm_prealloc_size;
  boolean ps_ipdc_config_param_double_comp_mem_for_hk_ecm_prealloc_size;
  uint16  ps_ipdc_config_param_ipdc_lib_max_searches_per_match;
  
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
void ps_ipdc_config_init
( 
  void
);

/*===========================================================================
FUNCTION ps_ipdc_config_init()

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
ps_ipdc_config_param_value_u ps_ipdc_config_get
( 
  ps_ipdc_config_param_type_e config_param
);

#endif  // PS_IPDC_CONFIG_H
