
/*!
  @file
  ds_3gpp_rpm.h

  @brief
  Internal utility functions and routines

  @detail
  OPTIONAL detailed description of this C file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/src/ds_3gpp_stats.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/24/11   ms      support power statistics FR 26957 

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "ds_3gpp_stats.h"
#include "ds_3gppi_utils.h"
#include "mcfg_seg_load.h"
#include "ITDSCDMA.h"
#include "IWCDMA.h"
#include "geran_gl1_api.h"
#include "wl1api.h"


/*===========================================================================

                           EXTERN DECLARTIONS

===========================================================================*/
extern interface_t *ds_3gpp_ps_tds_tbl;
extern interface_t *ds_3gpp_ps_wcdma_tbl;

/*===========================================================================

                           HEADER DECLARTIONS

===========================================================================*/


/*===========================================================================
FUNCTION DS_GPRS_CONFIG_MODEM_POWER_STATS

DESCRIPTION
  This function informs geran l1 to enable/disable modem power stats
 
PARAMETERS
  boolean - enable_modem_power_stats 
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
void ds_gprs_config_modem_power_stats
(
  sys_modem_as_id_e_type subs_id,
  boolean enable_modem_power_stats
)
{
  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
     return ;
  }

  DS_3GPP_MSG1_HIGH("enable_modem_power_stats: %d", enable_modem_power_stats);
  gpl1_config_modem_power_stats_tx(enable_modem_power_stats, subs_id);
}/*ds_gprs_config_modem_power_stats*/

/*===========================================================================
FUNCTION DS_GPRS_GET_MODEM_POWER_STATS

DESCRIPTION
  This function query geran l1 for tx power statistics
 
PARAMETERS
  subs_id - sys_modem_as_id_e_type
  tx_power_bin - array to hold the values
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
void ds_gprs_get_modem_power_stats
(
  sys_modem_as_id_e_type subs_id,
  uint32 tx_power_bin[DS3G_TX_POWER_BIN_MAX] 
)
{
  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
     return ;
  }

  gpl1_get_modem_power_stats_tx(tx_power_bin, subs_id);

} /*ds_gprs_get_modem_power_stats*/


/*===========================================================================
FUNCTION DS_WCDMA_CONFIG_MODEM_POWER_STATS

This function informs wcdma l1 to enable/disable modem power stats
 
PARAMETERS
  boolean - enable_modem_power_stats
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
void ds_wcdma_config_modem_power_stats
(
  sys_modem_as_id_e_type subs_id,
  boolean enable_modem_power_stats
)
{
  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
     return ;
  }

  DS_3GPP_MSG1_HIGH("enable_modem_power_stats: %d", enable_modem_power_stats);

#ifdef FEATURE_SEGMENT_LOADING
  if (ds_3gpp_ps_wcdma_tbl != NULL) 
  {
    IWCDMA_wl1_config_modem_power_stats_tx(ds_3gpp_ps_wcdma_tbl, enable_modem_power_stats);
  }
  else
  {
    DS_3GPP_MSG0_ERROR("WCDMA_segment is not loaded");
  }
#else
  wl1_config_modem_power_stats_tx(enable_modem_power_stats);
#endif

}/*ds_wcdma_config_modem_power_stats*/

/*===========================================================================
FUNCTION DS_WCDMA_GET_MODEM_POWER_STATS

DESCRIPTION
  This function query wcdma l1 for tx power statistics
 
PARAMETERS
  subs_id - sys_modem_as_id_e_type
  tx_power_bin - array to hold the values 
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
void ds_wcdma_get_modem_power_stats
(
  sys_modem_as_id_e_type subs_id,
  uint32 tx_power_bin[DS3G_TX_POWER_BIN_MAX] 
)
{
  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
     return ;
  }

#ifdef FEATURE_SEGMENT_LOADING
  if (ds_3gpp_ps_wcdma_tbl != NULL) 
  {
    IWCDMA_wl1_get_modem_power_stats(ds_3gpp_ps_wcdma_tbl, tx_power_bin);
  }
  else
  {
    DS_3GPP_MSG0_ERROR("WCDMA_segment is not loaded");
  }
  
#else
  wl1_get_modem_power_stats(tx_power_bin);
#endif
} /*ds_wcdma_get_modem_power_stats*/

/*===========================================================================
FUNCTION DS_TDSCDMA_CONFIG_MODEM_POWER_STATS

This function informs tdscdma l1 to enable/disable modem power stats
 
PARAMETERS
  boolean - enable_modem_power_stats
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
void ds_tdscdma_config_modem_power_stats
(
  sys_modem_as_id_e_type subs_id,
  boolean enable_modem_power_stats
)
{
  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
     return ;
  }

  DS_3GPP_MSG1_HIGH("enable_modem_power_stats: %d", enable_modem_power_stats);
#ifdef FEATURE_SEGMENT_LOADING
  if (ds_3gpp_ps_tds_tbl != NULL) 
  {
    ITDSCDMA_tdsl1ulcfg_config_modem_power_stats_tx(ds_3gpp_ps_tds_tbl, enable_modem_power_stats);
  }
  else
  {
    DS_3GPP_MSG0_ERROR("TDSCDMA_segment is not loaded");
  }
  
#else
  tdsl1ulcfg_config_modem_power_stats_tx(enable_modem_power_stats);
#endif
} /*ds_tdscdma_config_modem_power_stats*/

/*===========================================================================
FUNCTION DS_TDSCDMA_GET_MODEM_POWER_STATS

DESCRIPTION
  This function query tdscdma l1 for tx power statistics
 
PARAMETERS
  subs_id - sys_modem_as_id_e_type
  tx_power_bin - array to hold the values 
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
void ds_tdscdma_get_modem_power_stats
(
  sys_modem_as_id_e_type subs_id,
  uint32 tx_power_bin[DS3G_TX_POWER_BIN_MAX] 
)
{
  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
     return ;
  }
#ifdef FEATURE_SEGMENT_LOADING
  if (ds_3gpp_ps_tds_tbl != NULL) 
  {
    ITDSCDMA_tdsl1ulcfg_get_modem_power_stats_tx(ds_3gpp_ps_tds_tbl, tx_power_bin);
  }
  else
  {
    DS_3GPP_MSG0_ERROR("TDSCDMA_segment is not loaded");
  }
  
#else
  tdsl1ulcfg_get_modem_power_stats_tx(tx_power_bin);
#endif

} /*ds_tdscdma_get_modem_power_stats*/


