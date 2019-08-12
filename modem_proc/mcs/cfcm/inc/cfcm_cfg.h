/*!
  @file
  cfcm_cfg.h

  @brief
  REQUIRED brief one-sentence description of this C header file.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

  @author
  rohitj

*/

/*==============================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cfcm/inc/cfcm_cfg.h#1 $

==============================================================================*/

#ifndef CFCM_CFG_H
#define CFCM_CFG_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <customer.h>
#include "cfcm.h"
#include "cfcm_cpu_monitor.h"


/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! @brief define CFCM_CFG_DEBUG_MSG if debug F3 is desired. undef by default
*/
#undef CFCM_CFG_DEBUG_MSG

#ifndef TEST_FRAMEWORK
#define CFCM_ON_TARGET
#endif

/*! @brief CFCM Disabled Mask
*/
#define CFCM_DISABLE_MASK 0x1

/*! @brief CFCM Diag Test Enable Mask
*/
#define CFCM_DIAG_TEST_ENABLE_MASK 0x2

/*! @brief Define Mailbox priority for CFCM Task
*/
#define CFCM_MAILBOX_PRIO 1

/*! @brief Define stack size for CFCM Task
*/
#define CFCM_TASK_STACK_SIZE 4096

/*! @brief Define the maximum message queue size for CFCM Task
*/
#define CFCM_CFG_MAILBOX_SIZE 32

/*! @brief default client flow control enable mask in the absense of efs config
*/
#define CFCM_CFG_DEFAULT_CLIENT_FC_ENABLED_MASK                    0xFFFFFFFF

/*! @brief default monitor flow control enable mask in the absense of efs config
*/
#define CFCM_CFG_DEFAULT_MONITOR_FC_ENABLED_MASK                   0xFFFFFFFF

/*! @brief monitor flow control enable mask in the absense of efs config with 
     PA thermal monitor disabled
*/
#define CFCM_CFG_NO_PA_MONITOR_FC_ENABLED_MASK  (~CFCM_MONITOR_MASK_THERMAL_PA) &\
(~CFCM_MONITOR_MASK_THERMAL_PA_EX) & \
(~CFCM_MONITOR_MASK_THERMAL_PA_EM)

/*! @brief step timer in second for thermal monitor
*/
#define CFCM_MONITOR_THERMAL_STEP_TIMER                          15

/*! @brief step timer in ms for memory (dsm pool) monitor
*/
#define CFCM_MONITOR_DSM_POOL_STEP_TIMER                          5

/*! @brief step timer in ms for default monitor
*/
#define CFCM_MONITOR_DEFAULT_STEP_TIMER                     0xffffffff

/*! @brief freeing over this threshold will trigger CFCM events
*/
#if ( defined( FEATURE_BOLT_MODEM ) && !defined( FEATURE_THOR_MODEM ) )
#define CFCM_MONITOR_DSM_LARGE_ITEM_LTE_DL_DNE_MARK     200
#define CFCM_MONITOR_DSM_LARGE_ITEM_LTE_DL_FEW_MARK     800
#define CFCM_MONITOR_DSM_LARGE_ITEM_LTE_DL_MANY_MARK    1000
#elif (defined (FEATURE_MCS_CFCM_IOE ))
#define CFCM_MONITOR_DSM_LARGE_ITEM_LTE_DL_DNE_MARK     25
#define CFCM_MONITOR_DSM_LARGE_ITEM_LTE_DL_FEW_MARK     75
#define CFCM_MONITOR_DSM_LARGE_ITEM_LTE_DL_MANY_MARK    100
#else 
#define CFCM_MONITOR_DSM_LARGE_ITEM_LTE_DL_DNE_MARK     175
#define CFCM_MONITOR_DSM_LARGE_ITEM_LTE_DL_FEW_MARK     500
#define CFCM_MONITOR_DSM_LARGE_ITEM_LTE_DL_MANY_MARK    650
#endif /* FEATURE_BOLT_MODEM && not-FEATURE_THOR_MODEM */

/*! @brief freeing over this threshold will trigger CFCM events
*/
#ifndef CFCM_MONITOR_DSM_LARGE_ITEM_A2_UL_PER_FEW_MARK
#if (defined (FEATURE_MCS_CFCM_IOE ))
#define CFCM_MONITOR_DSM_LARGE_ITEM_A2_UL_PER_FEW_MARK  80
#else
#define CFCM_MONITOR_DSM_LARGE_ITEM_A2_UL_PER_FEW_MARK  475
#endif
#endif
 
/*! @brief freeing over this threshold will trigger CFCM events
*/
#ifndef CFCM_MONITOR_DSM_LARGE_ITEM_A2_UL_PER_MANY_MARK
#if (defined (FEATURE_MCS_CFCM_IOE ))
#define CFCM_MONITOR_DSM_LARGE_ITEM_A2_UL_PER_MANY_MARK 110
#else
#define CFCM_MONITOR_DSM_LARGE_ITEM_A2_UL_PER_MANY_MARK 575
#endif
#endif

/*! @brief freeing over this threshold will trigger CFCM events
*/
#if (defined (FEATURE_MCS_CFCM_IOE ))
#define CFCM_MONITOR_DSM_DUP_ITEM_LTE_DL_DNE_MARK     350
#else
#define CFCM_MONITOR_DSM_DUP_ITEM_LTE_DL_DNE_MARK     800
#endif

/*! @brief freeing over this threshold will trigger CFCM events
*/
#if (defined (FEATURE_MCS_CFCM_IOE ))
#define CFCM_MONITOR_DSM_DUP_ITEM_LTE_DL_FEW_MARK     500
#else 
#define CFCM_MONITOR_DSM_DUP_ITEM_LTE_DL_FEW_MARK     1300
#endif

/*! @brief grabbing under this threshold will trigger CFCM events
*/
#if (defined (FEATURE_MCS_CFCM_IOE ))
#define CFCM_MONITOR_DSM_DUP_ITEM_LTE_DL_MANY_MARK    600
#else
#define CFCM_MONITOR_DSM_DUP_ITEM_LTE_DL_MANY_MARK    1500
#endif

/*! @brief freeing over this threshold will trigger CFCM events
*/
#define CFCM_MONITOR_DSM_DUP_ITEM_A2_UL_PER_FEW_MARK  1000

/*! @brief grabbing under this threshold will trigger CFCM events
*/
#define CFCM_MONITOR_DSM_DUP_ITEM_A2_UL_PER_MANY_MARK 1200

/*! @brief freeing over this threshold will trigger CFCM events
*/
#if (defined (FEATURE_MCS_CFCM_IOE ))
#define CFCM_MONITOR_DSM_LARGE_ITEM_RLC_DL_DNE_MARK     35
#else
#define CFCM_MONITOR_DSM_LARGE_ITEM_RLC_DL_DNE_MARK     6
#endif

/*! @brief freeing over this threshold will trigger CFCM events
*/
#if (defined (FEATURE_MCS_CFCM_IOE ))
#define CFCM_MONITOR_DSM_LARGE_ITEM_RLC_DL_FEW_MARK     85
#else
#define CFCM_MONITOR_DSM_LARGE_ITEM_RLC_DL_FEW_MARK     50
#endif

/*! @brief grabbing under this threshold will trigger CFCM events
*/
#if (defined (FEATURE_MCS_CFCM_IOE ))
#define CFCM_MONITOR_DSM_LARGE_ITEM_RLC_DL_MANY_MARK    140
#else
#define CFCM_MONITOR_DSM_LARGE_ITEM_RLC_DL_MANY_MARK    100
#endif

/*! @brief freeing over this threshold will trigger CFCM events
*/
#if (defined (FEATURE_MCS_CFCM_IOE ))
#define CFCM_MONITOR_DSM_DUP_ITEM_RLC_DL_FEW_MARK     1000
#else
#define CFCM_MONITOR_DSM_DUP_ITEM_RLC_DL_FEW_MARK     50
#endif

/*! @brief grabbing under this threshold will trigger CFCM events
*/
#if (defined (FEATURE_MCS_CFCM_IOE ))
#define CFCM_MONITOR_DSM_DUP_ITEM_RLC_DL_MANY_MARK    1600
#else
#define CFCM_MONITOR_DSM_DUP_ITEM_RLC_DL_MANY_MARK    100
#endif

/*! @brief freeing over this threshold will trigger CFCM events
*/
#if (defined (FEATURE_MCS_CFCM_IOE ))
#define CFCM_MONITOR_DSM_SMALL_ITEM_RLC_DL_DNE_MARK     150
#else
#define CFCM_MONITOR_DSM_SMALL_ITEM_RLC_DL_DNE_MARK     6
#endif

/*! @brief freeing over this threshold will trigger CFCM events
*/
#if (defined (FEATURE_MCS_CFCM_IOE ))
#define CFCM_MONITOR_DSM_SMALL_ITEM_RLC_DL_FEW_MARK     375
#else
#define CFCM_MONITOR_DSM_SMALL_ITEM_RLC_DL_FEW_MARK     50
#endif

/*! @brief grabbing under this threshold will trigger CFCM events
*/
#if (defined (FEATURE_MCS_CFCM_IOE ))
#define CFCM_MONITOR_DSM_SMALL_ITEM_RLC_DL_MANY_MARK    600
#else
#define CFCM_MONITOR_DSM_SMALL_ITEM_RLC_DL_MANY_MARK    100
#endif

/*! @brief grabbing under this threshold will trigger CFCM events
*/
#define CFCM_MONITOR_DSM_LARGE_ITEM_IPA_DL_MANY_MARK   150

/*! @brief grabbing under this threshold will trigger CFCM events
*/
#define CFCM_MONITOR_DSM_LARGE_ITEM_IPA_DL_FEW_MARK   50

#if ( defined( FEATURE_BOLT_MODEM ) && !defined( FEATURE_THOR_MODEM ) )
#define CFCM_MONITOR_DSM_LARGE_ITEM_LTE_DL_LEVEL1_FEW_MARK    500
#define CFCM_MONITOR_DSM_LARGE_ITEM_LTE_DL_LEVEL1_MANY_MARK   600
#elif (defined (FEATURE_MCS_CFCM_IOE ))
#define CFCM_MONITOR_DSM_LARGE_ITEM_LTE_DL_LEVEL1_FEW_MARK    40
#define CFCM_MONITOR_DSM_LARGE_ITEM_LTE_DL_LEVEL1_MANY_MARK   50
#else
#define CFCM_MONITOR_DSM_LARGE_ITEM_LTE_DL_LEVEL1_FEW_MARK    350
#define CFCM_MONITOR_DSM_LARGE_ITEM_LTE_DL_LEVEL1_MANY_MARK   400
#endif /* FEATURE_BOLT_MODEM && not-FEATURE_THOR_MODEM */

#if (defined (FEATURE_MCS_CFCM_IOE ))
#define CFCM_MONITOR_DSM_DUP_ITEM_LTE_DL_LEVEL1_FEW_MARK      450
#define CFCM_MONITOR_DSM_DUP_ITEM_LTE_DL_LEVEL1_MANY_MARK     550
#else
#define CFCM_MONITOR_DSM_DUP_ITEM_LTE_DL_LEVEL1_FEW_MARK      1100
#define CFCM_MONITOR_DSM_DUP_ITEM_LTE_DL_LEVEL1_MANY_MARK     1400
#endif

#define CFCM_MONITOR_DSM_LARGE_ITEM_LTE_PDCP_COMP_DNE_MARK    300
#define CFCM_MONITOR_DSM_LARGE_ITEM_LTE_PDCP_COMP_FEW_MARK    600
#define CFCM_MONITOR_DSM_LARGE_ITEM_LTE_PDCP_COMP_MANY_MARK   700


#define CFCM_MONITOR_DSM_DUP_ITEM_LTE_PDCP_COMP_DNE_MARK    1100
#define CFCM_MONITOR_DSM_DUP_ITEM_LTE_PDCP_COMP_FEW_MARK    1400
#define CFCM_MONITOR_DSM_DUP_ITEM_LTE_PDCP_COMP_MANY_MARK   1700

#define CFCM_MAX_CPU_THRSHLD_VALUE 100

#define CFCM_IS_CPU_THRSHLD_VALID(thrshld) ((thrshld > 0) && (thrshld < CFCM_MAX_CPU_THRSHLD_VALUE))



/*==============================================================================
                   START EFS files DEFINITIONS AND TYPES
==============================================================================*/

#define CFCM_CONF_EFS_FILE  "/nv/item_files/conf/mcs_cfcm.conf"

/*! @brief EFS file to store CFCM Specific Mask for disabling different features
*/
#define CFCM_CONFIG_CFCM_FC_MASK                          "/nv/item_files/mcs/cfcm/cfcm_fc_masks"

//#define CFCM_CONFIG_LTE_SPECIFIC_EFS_DATA                  "/nv/item_files/mcs/cfcm/cfcm_config_lte_specific"

/*! @brief EFS file to store CPU monitor threshold limits for each clients
*/
#define CFCM_CONFIG_CPU_BASED_FC_THRESHOLDS                "/nv/item_files/mcs/cfcm/cfcm_cpu_threshold_limits"

/*! @brief file name/location of client FC Enabled masks EFS file
*/
#define CFCM_CFG_CLIENT_FC_ENABLED_MASKS_EFS_FILE          "/nv/item_files/mcs/cfcm/cfcm_client_enabled_masks"

/*! @brief file name/location of monitor FC Enabled masks EFS file
*/
#define CFCM_CFG_MONITOR_FC_ENABLED_MASKS_EFS_FILE         "/nv/item_files/mcs/cfcm/cfcm_monitor_enabled_masks"

/*! @brief file name/location of thermal step timer EFS file
*/
#define CFCM_CFG_STEP_TIMER_EFS_FILE                       "/nv/item_files/mcs/cfcm/cfcm_config_step_timer_efs"

#define CFCM_CONFIG_CPU_DNE_EFS_FILE                       "/nv/item_files/mcs/cfcm/cfcm_config_cpu_dne_efs"



 /* buffer for the list of file paths to be be written to the conf file */
#define CFCM_UPDATE_CONF_FILE_WITH_EFS_NAMES       CFCM_CONFIG_CFCM_FC_MASK "\n"\
                                                   CFCM_CFG_CLIENT_FC_ENABLED_MASKS_EFS_FILE "\n"\
                                                   CFCM_CFG_MONITOR_FC_ENABLED_MASKS_EFS_FILE "\n"\
                                                   CFCM_CFG_STEP_TIMER_EFS_FILE "\n"\
                                                   CFCM_CONFIG_CPU_BASED_FC_THRESHOLDS "\n"\
												   CFCM_CONFIG_CPU_DNE_EFS_FILE "\n"

/*==============================================================================
                   END EFS files DEFINITIONS AND TYPES
==============================================================================*/

/*----------------------------------------------------------------------------
  CFCM Config data structure for CPU FC Threshold
----------------------------------------------------------------------------*/
typedef struct 
{
  /* Thresholds Valid */
  boolean is_valid;

  /* Tier High Threshold */
  uint8 tier_high;

  /* Tier Medium Threshold */
  uint8 tier_medium;

  /* Tier Low Threshold */
  uint8 tier_low;
}cfcm_config_cpu_thrshld_type_s;

/*----------------------------------------------------------------------------
  CFCM Config data structure for Step Timers
----------------------------------------------------------------------------*/

typedef struct 
{
  /* Thresholds Valid, this is used to check if data present in EFS else load from default */
  boolean is_valid;

  /* CPU FC step timer in msecs */
  uint32  cpu_step_timer;

  /* DSM FC step timer in msecs */
  uint32  dsm_step_timer;

  /*!<  thermal step timer in unit of seconds */
  uint8   thermal_step_timer;

  /*! Bus BW step timer in msecs */
  uint32   bw_step_timer;

}cfcm_config_step_timer_type_s;


/*----------------------------------------------------------------------------
  CFCM Config global data structure
----------------------------------------------------------------------------*/

typedef struct 
{
  /* CPU Threshold array */
  cfcm_config_cpu_thrshld_type_s  cpu_thrshld;

  /* Step timer EFS items */
  cfcm_config_step_timer_type_s  step_timer;
}cfcm_config_efs_type_s;

/*! @brief enable mask that controls which monitors/clients are enabled, 
     definition of the mask is in cfcm.h
*/
typedef struct
{
  uint32  client_fc_enable_mask;  /*!< enable mask that dicates which clients
                                      are flow control enabled */
  uint32  monitor_fc_enable_mask; /*!< enable mask that dicates which monitors
                                      are flow control enabled */

  cfcm_config_efs_type_s config_efs[CFCM_CLIENT_MAX]; /*!<  Specific threshold values for each clients */

  /*! Flow Control mask  
       Bit 0 - CFCM FLow Control DISABLED (1 - Disable)
       Bit 1 - CFCM FLow Control via Diag Test enabled  (1 - Enable) */
  uint32  cfcm_fc_mask;
} cfcm_cfg_s;

/*==============================================================================

                   PACK Structures for EFS items 

==============================================================================*/

typedef PACK (struct)
{
  boolean is_valid;
  uint8   client_id;
  uint8   tier_high;
  uint8   tier_medium;
  uint8   tier_low;
}cfcm_config_cpu_thrshld_type;

typedef PACK (struct)
{
  uint32    version;
  uint32    fc_mask;
  uint32    reserved[4]; /* reserved for future use */
}cfcm_config_fc_mask_type;

typedef PACK (struct)
{
  boolean is_valid;
  uint8   client_id;
  uint32  cpu_step_timer;
  uint32  dsm_step_timer;
  uint8   thermal_step_timer;
  uint8   bw_step_timer;
}cfcm_config_step_timer_type;

typedef PACK (struct)
{
  uint32    version;
  uint32    cpu_dne_limit;
  uint32    cpu_max_successive_dne_hits;
}cfcm_config_cpu_dne_type;

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

  FUNCTION:  cfcm_cfg_init

==============================================================================*/
/*!
    @brief
    initialize cfcm_cfg structure

    @return
    None
*/
/*============================================================================*/
extern void cfcm_cfg_init(void);

/*==============================================================================

  FUNCTION:  cfcm_cfg_monitor_enabled

==============================================================================*/
/*!
    @brief
    Return whether or not the monitor is enabled for flow control.

    @return
    TRUE if flow control is enabled; FALSE otherwise
*/
/*============================================================================*/
extern boolean cfcm_cfg_monitor_enabled
(
  cfcm_monitor_e          monitor_id  /*!< the monitor in question */
);

/*==============================================================================

  FUNCTION:  cfcm_cfg_client_enabled

==============================================================================*/
/*!
    @brief
    Return whether or not the client is enabled for flow control.

    @return
    TRUE if flow control is enabled; FALSE otherwise
*/
/*============================================================================*/
extern boolean cfcm_cfg_client_enabled
(
  cfcm_client_e          client_id  /*!< the client in question */
);

/*==============================================================================

  FUNCTION:  cfcm_cfg_set_client_fc_enable_mask

==============================================================================*/
/*!
    @brief
    Sets the client flow control enabled mask.
*/
/*============================================================================*/
extern void cfcm_cfg_set_client_fc_enable_mask
(
  uint32 fc_enable_mask  /*!< new fc enable mask */
);

/*==============================================================================

  FUNCTION:  cfcm_cfg_set_monitor_fc_enable_mask

==============================================================================*/
/*!
    @brief
    Sets the monitor flow control enabled mask.
*/
/*============================================================================*/
extern void cfcm_cfg_set_monitor_fc_enable_mask
(
  uint32 fc_enable_mask  /*!< new fc enable mask */
);

/*==============================================================================

  FUNCTION:  cfcm_cfg_get_thermal_step_timer

==============================================================================*/
/*!
    @brief
    this function returns the thermal step timer value in unit of ms.

    @return
    thermal step timer in unit of ms
*/
/*============================================================================*/
EXTERN uint32 cfcm_cfg_get_thermal_step_timer
( 
  cfcm_client_e          client_id  /*!< the client in question */
);

/*==============================================================================

  FUNCTION:  cfcm_cfg_get_dsm_step_timer

==============================================================================*/
/*!
    @brief
    this function returns the DSM step timer value in unit of ms.

    @return
    DSM step timer in unit of ms
*/
/*============================================================================*/
uint32 cfcm_cfg_get_dsm_step_timer
( 
  cfcm_client_e          client_id  /*!< the client in question */
);

/*==============================================================================

  FUNCTION:  cfcm_cfg_get_cpu_step_timer

==============================================================================*/
/*!
    @brief
    this function returns the CPU step timer value in unit of ms.

    @return
    CPU step timer in unit of ms
*/
/*============================================================================*/
uint32 cfcm_cfg_get_cpu_step_timer
( 
  cfcm_client_e          client_id  /*!< the client in question */
);

/*==============================================================================

  FUNCTION:  cfcm_cfg_get_bw_step_timer

==============================================================================*/
/*!
    @brief
    this function returns the Bus BW step timer value in unit of ms.

    @return
    Bus BW step timer in unit of ms
*/
/*============================================================================*/
uint32 cfcm_cfg_get_bw_step_timer
( 
  cfcm_client_e          client_id  /*!< the client in question */
);

/*==============================================================================

  FUNCTION:  cfcm_cfg_get_cfcm_disabled

==============================================================================*/
/*!
    @brief
    this function returns CFCM is disabled based on the EFS/NV read

    @return
    CPU Threshold values
*/
/*============================================================================*/
boolean cfcm_cfg_get_cfcm_disabled( void );

/*==============================================================================

  FUNCTION:  cfcm_cfg_cpu_monitor_client_cmd_status

==============================================================================*/
/*!
    @brief
    This function updated CFCM command status based on the new CPU busy percentage 

    @return
    None
*/
/*============================================================================*/
boolean cfcm_cfg_cpu_monitor_client_cmd_status
( 
  cfcm_client_e client_id,
  uint32 idlest_pct,
  cfcm_cpu_monitor_client_cmd_s*   status
);

/*==============================================================================

  FUNCTION:  cfcm_cfg_get_client_step_timers

==============================================================================*/
/*!
    @brief
    this function returns the ALL step timer values for a client 

    @return
    
*/
/*============================================================================*/
boolean cfcm_cfg_get_client_step_timers
( 
  cfcm_client_e                   client_id,
  cfcm_default_step_timer_type_s*  timers
);

/*==============================================================================

  FUNCTION:  cfcm_cfg_get_diag_test_enable_mask

==============================================================================*/
/*!
    @brief
    this function returns CFCM testing via Diag command enabled based on the EFS/NV read

    @return
    CPU Threshold values
*/
/*============================================================================*/
boolean cfcm_cfg_get_diag_test_enable_mask( void );

/*=============================================================================

  FUNCTION:  cfcm_read_cpu_dne_efs

=============================================================================*/
/*!
    @brief
    Reads the CPU DNE EFS file and return the value stored in it.
 
    @return
    None
*/
/*===========================================================================*/
cfcm_config_cpu_dne_type cfcm_read_cpu_dne_efs( void );

#endif /* CFCM_CFG_H */
