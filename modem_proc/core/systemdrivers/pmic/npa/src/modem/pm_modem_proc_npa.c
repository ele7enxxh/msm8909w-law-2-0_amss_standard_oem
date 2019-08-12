/*! \file
 *
 *  \brief  pm_modem_proc_npa.c ----This file contains initialization functions for NPA
 *  \details This file contains initialization functions for NPA
 *          and node definitions.
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

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/npa/src/modem/pm_modem_proc_npa.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/26/15   mr      NPA SINGLE clients should also use the MAX plugin (CR-796926)
02/18/13   rk      PMIC typcasting issue while reading device config PAM data for num_of_nodes (CR - 533203)

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "DDIPlatformInfo.h"
#include "comdef.h"
#include <err.h>
#include "npa.h"
#include "npa_resource.h"

#include "pm_qc_pmic.h"
#include "pmapp_npa.h"
#include "pm_npa.h"
#include "pm_modem_proc_npa_device.h"
#include "pm_modem_proc_npa.h"
#include "pm_npa_device.h"
#include "pm_target_information.h"
#include "pm_malloc.h"
#include "pm_sw_event.h"

#ifdef PM_IMAGE_MODEM_PROC

/*===========================================================================

                 LOCAL CONSTANT AND MACRO DEFINITIONS

===========================================================================*/

/*===========================================================================

                LOCAL FUNCTION PROTOTYPES

===========================================================================*/
uint32                             num_of_nodes = 0;
npa_node_definition                *pmic_npa_modem_nodes = NULL;
npa_resource_definition            *pmic_npa_modem_resources = NULL;
extern boolean                      bPMICStub;

void pm_modem_proc_npa_register_resources (void);

/*===========================================================================

                VARIABLES DEFINITIONS

===========================================================================*/

/*===========================================================================

                         FUNCTION DEFINITIONS

===========================================================================*/
/* Surf, FFA, FLUID platform info*/
DalPlatformInfoPlatformInfoType pm_npa_platform_info;


/*===========================================================================

                FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION pm_modem_proc_npa_platform_compensate

DESCRIPTION
    This function performes the changes needed for surf ffa on PAM data.

INPUT PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  Platform detection should be avaliable.

SIDE EFFECTS
  NONE.

===========================================================================*/
static void
pm_modem_proc_npa_platform_compensate (void)
{
   DalDeviceHandle      *phPFormInfo = NULL;
   DALResult             eResult;

   eResult = DAL_DeviceAttach(DALDEVICEID_PLATFORMINFO, &phPFormInfo);

   if (DAL_SUCCESS == eResult)
{
      DalDevice_Open (phPFormInfo, DAL_OPEN_SHARED);
      DalPlatformInfo_GetPlatformInfo (phPFormInfo, &pm_npa_platform_info );
      DalDevice_Close (phPFormInfo);
   }
}


/*===========================================================================

FUNCTION pm_modem_proc_pam_init                                EXTERNAL
FUNCTION

DESCRIPTION
This function initializes the PAM for PMIC.

It does the following:
* Initializes the NPA PMIC Nodes and resources.

INPUT PARAMETERS
None.

RETURN VALUE
None.

DEPENDENCIES
NPA Framework should be available in the build being compiled for

SIDE EFFECTS
NONE.

===========================================================================*/
static void
pm_modem_proc_pam_init (void)
{
   pm_modem_proc_npa_platform_compensate ();
} /* end of pm_modem_proc_pam_init() */

/*===========================================================================

FUNCTION pm_modem_proc_npa_init                                EXTERNAL FUNCTION

DESCRIPTION
This function initializes the NPA for PMIC.

It does the following:
* It initializes the PMIC NPA Node and resources.

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
pm_modem_proc_npa_init (void)
{
    unsigned i;

    pm_sw_event(PMIC_MPSS_PAM_INIT, 0);

    pm_npa_node_resource_info          *nodeRscArray = NULL;
    pm_npa_node_resource_info          *nodeRsc = NULL;
    uint32                             *node_num_ptr = NULL;

    /* Init PAM */
    pm_modem_proc_pam_init ();

    /* Init Modem device trans-apply */
    pm_modem_proc_npa_register_resources ();

    // Get the PAM info
    nodeRscArray = (pm_npa_node_resource_info*)pm_target_information_get_specific_info(PM_PROP_PAM_NODE_RSRCS);
    node_num_ptr = (uint32*)pm_target_information_get_specific_info(PM_PROP_PAM_NODE_NUM);

    if (node_num_ptr != NULL)
    {
       num_of_nodes = *node_num_ptr;
    }

    if(num_of_nodes && nodeRscArray)
    {
        pm_malloc(sizeof(npa_node_definition)*num_of_nodes, (void**)&pmic_npa_modem_nodes);
        pm_malloc(sizeof(npa_resource_definition)*num_of_nodes, (void**)&pmic_npa_modem_resources);

        // construct the PMIC RPM resources
        for(i = 0; i<num_of_nodes; i++)
        {
            nodeRsc = &nodeRscArray[i];

            // construct the PMIC RPM resources
            pmic_npa_modem_resources[i].name = nodeRsc->group_name;
            pmic_npa_modem_resources[i].units = "ModeID";
            pmic_npa_modem_resources[i].max = nodeRsc->max_mode;
            pmic_npa_modem_resources[i].attributes = nodeRsc->resource_attributes;

            pmic_npa_modem_resources[i].plugin = &npa_max_plugin;

            pmic_npa_modem_resources[i].data = nodeRsc->resource_data;

            // construct the PMIC RPM Nodes
            pmic_npa_modem_nodes[i].name = nodeRsc->node_name;

            //Set appropriate callback function in case of MSS Node
            if(strcmp(pmic_npa_modem_resources[i].name, PMIC_NPA_GROUP_ID_MEM_UVOL) == 0)
            {
                pmic_npa_modem_nodes[i].driver_fcn = pm_npa_resource_set_vdd_uvol;
            }
            else
            {
                pmic_npa_modem_nodes[i].driver_fcn = pm_npa_resource_set;
            }

            pmic_npa_modem_nodes[i].attributes = nodeRsc->node_attributes;
            pmic_npa_modem_nodes[i].data = nodeRsc->node_data;
            pmic_npa_modem_nodes[i].dependencies = nodeRsc->node_dependencies;
            pmic_npa_modem_nodes[i].dependency_count = nodeRsc->dependency_count;
            pmic_npa_modem_nodes[i].resource_count = 1;
            pmic_npa_modem_nodes[i].resources = NULL;
        }
    }
    else
    {
        ERR_FATAL( "PMIC-MODEM PAM initialization failed! ", 0, 0, 0 ); // we should never get here
    }

    for (i = 0; i < num_of_nodes; i++)
    {
        pmic_npa_modem_nodes[i].resources = &pmic_npa_modem_resources[i];

        if (bPMICStub == TRUE)
        {
            npa_stub_resource(pmic_npa_modem_resources[i].name);
        }
        else
        {
            npa_define_node (pmic_npa_modem_nodes + i, NULL, NULL);
        }
    }

} /* end of pm_modem_proc_npa_init() */

#endif // PM_IMAGE_MODEM_PROC
