/*! \file
 *
 *  \brief  pm_modem_proc_npa_device.c ----This file contains initialization functions for NPA Device layer
 *  \details This file contains initialization functions for NPA Device layer and
 *          the node and resource definition implementations
 *
 *    PMIC code generation Version: 1.0.0.0
 *    PMIC code generation NPA Client Version: MSM8974 branch 1.0 ver 01_31_2013 - Approved
 *    PMIC code generation NPA Device Setting Value Version: MSM8974 branch 1.0 ver 01_31_2013 - Approved
 *    This file contains code for Target specific settings and modes.
 *
 *  &copy; Copyright 2013 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/npa/src/modem/pm_modem_proc_npa_device.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/26/13   rk      Remove the code for VS

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "npa.h"
#include "npa_resource.h"
#include "npa_remote_resource.h"
#include "npa_log.h"

#include "pm_qc_pmic.h"
#include "pm_modem_proc_npa.h"
#include "pm_modem_proc_npa_device.h"
#include "pm_npa.h"
#include "pm_npa_device_clk_buff.h"
#include "pm_npa_device_ldo.h"
#include "pm_npa_device_smps.h"
#include "pm_npa_device_vs.h"
#include "pm_malloc.h"
#include "pm_target_information.h"


/*===========================================================================

                 LOCAL CONSTANT AND MACRO DEFINITIONS

===========================================================================*/

/*===========================================================================

                LOCAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

                VARIABLES DEFINITIONS

===========================================================================*/
npa_remote_resource_definition            *pmic_npa_ldo_remote_rsrc = NULL;
npa_remote_resource_definition            *pmic_npa_clk_remote_rsrc = NULL;
npa_remote_resource_definition            *pmic_npa_smps_remote_rsrc = NULL;
npa_remote_resource_definition            *pmic_npa_vs_remote_rsrc = NULL;


/*===========================================================================

                FUNCTION DEFINITIONS

===========================================================================*/

static npa_status
pm_npa_remove_resource_log_by_handle( char *log_name, struct npa_resource *resource )
{
#ifndef PMIC_OFFTARGET_TESTING
    if (resource)
    {
        return(npa_remove_resource_log_by_handle(log_name, resource));
    }
    else
    {
        return NPA_ERROR;
    }
#else
    (void)log_name;
    (void)resource;

    return NPA_SUCCESS;
#endif
}


/*===========================================================================

FUNCTION pm_modem_proc_npa_device_init

DESCRIPTION
    This function initializes the NPA Device layer for PMIC.

    Does the following:
    * Initializes the PMIC NPA Device Node and resources.

INPUT PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  NPA Framework should be available in the build being compiled for

SIDE EFFECTS
  NONE.

===========================================================================*/
void
pm_modem_proc_npa_register_resources (void)
{
  uint32 i = 0;
  pm_npa_remote_resource_type* pPmRemoteRsrc = NULL;

  /*
   * Get the PMIC remote node resources for LDO
   */
  pPmRemoteRsrc = (pm_npa_remote_resource_type*)
                           pm_target_information_get_specific_info(PM_PROP_REMOTE_LDO);

 /* Initialize all Remote Resources - LDO */
  if ((pPmRemoteRsrc != NULL) && (pPmRemoteRsrc->num_resources > 0))
  {
    pm_malloc(pPmRemoteRsrc->num_resources * sizeof(npa_remote_resource_definition), (void**)&pmic_npa_ldo_remote_rsrc);

    for (i = 0; i < pPmRemoteRsrc->num_resources; i++)
    {
      npa_resource *resource = NULL;
      npa_query_handle query_handle = NULL;
      pmic_npa_ldo_remote_rsrc[i].local_resource_name = (char*)pPmRemoteRsrc->remote_resource[i].rsrc_name;
      pmic_npa_ldo_remote_rsrc[i].remote_resource_name = (char*)pPmRemoteRsrc->remote_resource[i].rmt_rsrc_name;
      pmic_npa_ldo_remote_rsrc[i].protocol_type = PMIC_NPA_RMT_PROTOCOL_RPM_TYPE;
      pmic_npa_ldo_remote_rsrc[i].plugin = pm_npa_get_ldo_kvps_plugin();
#ifdef PMIC_OFFTARGET_TESTING
      pmic_npa_ldo_remote_rsrc[i].driver_fcn = npa_remote_resource_local_aggregation_driver_fcn;
#else
      pmic_npa_ldo_remote_rsrc[i].driver_fcn = npa_remote_resource_local_aggregation_no_initial_request_driver_fcn;
#endif
      pmic_npa_ldo_remote_rsrc[i].units = "kvps";
      pmic_npa_ldo_remote_rsrc[i].max = sizeof(pm_npa_ldo_kvps)/sizeof(npa_resource_state);
      pmic_npa_ldo_remote_rsrc[i].attributes = NPA_RESOURCE_VECTOR_STATE | NPA_RESOURCE_SUPPORTS_SUPPRESSIBLE ;

      npa_remote_define_resource (&pmic_npa_ldo_remote_rsrc[i], (npa_resource_state)NULL, NULL);

      /* Disable Logging */

      query_handle = npa_create_query_handle(pmic_npa_ldo_remote_rsrc[i].local_resource_name);
      if (query_handle != NULL)
      {
         resource = npa_query_get_resource(query_handle);
      }

      pm_npa_remove_resource_log_by_handle (NPA_DEFAULT_LOG_NAME, resource);
      if ((resource != NULL) && (resource->required_state_vector))
      {
        pm_npa_ldo_kvps_init ((pm_npa_ldo_kvps*)resource->required_state_vector);
      }
     }
  }

  pPmRemoteRsrc = NULL;

  /*
   * Get the PMIC remote node resources for SMPS
   */
  pPmRemoteRsrc = (pm_npa_remote_resource_type*)
                           pm_target_information_get_specific_info(PM_PROP_REMOTE_SMPS);

  /* Initialize all Remote Resources - SMPS */
  if ((pPmRemoteRsrc != NULL) && (pPmRemoteRsrc->num_resources > 0))
  {
    pm_malloc(pPmRemoteRsrc->num_resources * sizeof(npa_remote_resource_definition), (void**)&pmic_npa_smps_remote_rsrc);

    for (i = 0; i < pPmRemoteRsrc->num_resources; i++)
    {
      npa_resource *resource = NULL;
      npa_query_handle query_handle = NULL;
      pmic_npa_smps_remote_rsrc[i].local_resource_name = (char*)pPmRemoteRsrc->remote_resource[i].rsrc_name;
      pmic_npa_smps_remote_rsrc[i].remote_resource_name = (char*)pPmRemoteRsrc->remote_resource[i].rmt_rsrc_name;
      pmic_npa_smps_remote_rsrc[i].protocol_type = PMIC_NPA_RMT_PROTOCOL_RPM_TYPE;
      pmic_npa_smps_remote_rsrc[i].plugin = pm_npa_get_smps_kvps_plugin();
#ifdef PMIC_OFFTARGET_TESTING
      pmic_npa_smps_remote_rsrc[i].driver_fcn = npa_remote_resource_local_aggregation_driver_fcn;
#else
      pmic_npa_smps_remote_rsrc[i].driver_fcn = npa_remote_resource_local_aggregation_no_initial_request_driver_fcn;
#endif
      pmic_npa_smps_remote_rsrc[i].units = "kvps";
      pmic_npa_smps_remote_rsrc[i].max =  sizeof(pm_npa_smps_kvps)/sizeof(npa_resource_state);
      pmic_npa_smps_remote_rsrc[i].attributes = NPA_RESOURCE_VECTOR_STATE | NPA_RESOURCE_SUPPORTS_SUPPRESSIBLE ;

      npa_remote_define_resource (&pmic_npa_smps_remote_rsrc[i], (npa_resource_state)NULL, NULL);

      /* Disable Logging */
      query_handle = npa_create_query_handle(pmic_npa_smps_remote_rsrc[i].local_resource_name);
      if (query_handle != NULL)
      {
         resource = npa_query_get_resource(query_handle);
      }

      pm_npa_remove_resource_log_by_handle (NPA_DEFAULT_LOG_NAME, resource);
      if ((resource != NULL) && (resource->required_state_vector))
      {
        pm_npa_smps_kvps_init ((pm_npa_smps_kvps*)resource->required_state_vector);
      }
    }
  }

  pPmRemoteRsrc = NULL;

  /*
   * Get the PMIC remote node resources for CLK
   */
  pPmRemoteRsrc = (pm_npa_remote_resource_type*)
                           pm_target_information_get_specific_info(PM_PROP_REMOTE_CLK);

  if ((pPmRemoteRsrc != NULL) && (pPmRemoteRsrc->num_resources > 0))
  {
    pm_malloc(pPmRemoteRsrc->num_resources * sizeof(npa_remote_resource_definition), (void**)&pmic_npa_clk_remote_rsrc);

    for (i = 0; i < pPmRemoteRsrc->num_resources; i++)
    {
      npa_resource *resource = NULL;
      npa_query_handle query_handle = NULL;
      pmic_npa_clk_remote_rsrc[i].local_resource_name = (char*)pPmRemoteRsrc->remote_resource[i].rsrc_name;
      pmic_npa_clk_remote_rsrc[i].remote_resource_name = (char*)pPmRemoteRsrc->remote_resource[i].rmt_rsrc_name;
      pmic_npa_clk_remote_rsrc[i].protocol_type = PMIC_NPA_RMT_PROTOCOL_RPM_TYPE;
      pmic_npa_clk_remote_rsrc[i].plugin = pm_npa_get_clk_buff_kvps_plugin();
#ifdef PMIC_OFFTARGET_TESTING
      pmic_npa_clk_remote_rsrc[i].driver_fcn = npa_remote_resource_local_aggregation_driver_fcn;
#else
      pmic_npa_clk_remote_rsrc[i].driver_fcn = npa_remote_resource_local_aggregation_no_initial_request_driver_fcn;
#endif
      pmic_npa_clk_remote_rsrc[i].units = "kvps";
      pmic_npa_clk_remote_rsrc[i].max =  sizeof(pm_npa_clk_buff_kvps)/sizeof(npa_resource_state);
      pmic_npa_clk_remote_rsrc[i].attributes = NPA_RESOURCE_VECTOR_STATE | NPA_RESOURCE_SUPPORTS_SUPPRESSIBLE ;

      npa_remote_define_resource (&pmic_npa_clk_remote_rsrc[i], (npa_resource_state)NULL, NULL);

      /* Disable Logging */
      query_handle = npa_create_query_handle(pmic_npa_clk_remote_rsrc[i].local_resource_name);
      if (query_handle != NULL)
      {
         resource = npa_query_get_resource(query_handle);
      }

      pm_npa_remove_resource_log_by_handle (NPA_DEFAULT_LOG_NAME, resource);
      if ((resource != NULL) && (resource->required_state_vector))
      {
        pm_npa_clk_buff_kvps_init ((pm_npa_clk_buff_kvps*)resource->required_state_vector);
      }
    }
  }

  pPmRemoteRsrc = NULL;

  /*
   * Get the PMIC remote node resources for VS
   */
  pPmRemoteRsrc = (pm_npa_remote_resource_type*)
                           pm_target_information_get_specific_info(PM_PROP_REMOTE_VS);

  if ((pPmRemoteRsrc != NULL) && (pPmRemoteRsrc->num_resources > 0))
  {
    pm_malloc(pPmRemoteRsrc->num_resources * sizeof(npa_remote_resource_definition), (void**)&pmic_npa_vs_remote_rsrc);

    for (i = 0; i < pPmRemoteRsrc->num_resources; i++)
    {
      npa_resource *resource = NULL;
      npa_query_handle query_handle = NULL;
      pmic_npa_vs_remote_rsrc[i].local_resource_name = (char*)pPmRemoteRsrc->remote_resource[i].rsrc_name;
      pmic_npa_vs_remote_rsrc[i].remote_resource_name = (char*)pPmRemoteRsrc->remote_resource[i].rmt_rsrc_name;
      pmic_npa_vs_remote_rsrc[i].protocol_type = PMIC_NPA_RMT_PROTOCOL_RPM_TYPE;
      pmic_npa_vs_remote_rsrc[i].plugin = pm_npa_get_vs_kvps_plugin();
#ifdef PMIC_OFFTARGET_TESTING
      pmic_npa_vs_remote_rsrc[i].driver_fcn = npa_remote_resource_local_aggregation_driver_fcn;
#else
      pmic_npa_vs_remote_rsrc[i].driver_fcn = npa_remote_resource_local_aggregation_no_initial_request_driver_fcn;
#endif
      pmic_npa_vs_remote_rsrc[i].units = "kvps";
      pmic_npa_vs_remote_rsrc[i].max =  sizeof(pm_npa_vs_kvps)/sizeof(npa_resource_state);
      pmic_npa_vs_remote_rsrc[i].attributes = NPA_RESOURCE_VECTOR_STATE | NPA_RESOURCE_SUPPORTS_SUPPRESSIBLE ;

      npa_remote_define_resource (&pmic_npa_vs_remote_rsrc[i], (npa_resource_state)NULL, NULL);

      /* Disable Logging */
      query_handle = npa_create_query_handle(pmic_npa_vs_remote_rsrc[i].local_resource_name);

      if (query_handle != NULL)
      {
         resource = npa_query_get_resource(query_handle);
      }

      pm_npa_remove_resource_log_by_handle (NPA_DEFAULT_LOG_NAME, resource);
      if ((resource != NULL) && (resource->required_state_vector))
      {
        pm_npa_vs_kvps_init ((pm_npa_vs_kvps*)resource->required_state_vector);
      }
    }
  }

} /* end of pm_modem_proc_npa_device_init() */


