/*
===========================================================================

FILE:         rpm_resources_config.c

DESCRIPTION:
  Per target resource configurations

===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/power/rpm/server/rpm_resources_config.c#4 $
$Date: 2016/01/13 $

when       who     what, where, why
--------   ---     --------------------------------------------------------

===========================================================================
             Copyright (c) 2015 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
              QUALCOMM Proprietary and Confidential
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include <stddef.h>
#include "Chipinfo.h"
#include "rpm_resources_config.h"
#include "railway.h"

/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */

#if (DAL_CONFIG_TARGET_ID == 0x9645)

static const rpm_resource_config_t temp_config_data[] =
{
    { RPM_LDO_A_REQ,        9 },  // VDDMX
    { RPM_SMPS_A_REQ,       5 },  // VDDCX
    { (rpm_resource_type)0, 0 }
};

static const rpm_resource_config_t bimc_preprocess_dep[] =
{
    { RPM_BUS_SLAVE_REQ,          0 },
    { RPM_BUS_SLAVE_REQ,          2 },
    { RPM_BUS_MASTER_REQ,         3 },
    { (rpm_resource_type)0,       0 }
};

#elif (DAL_CONFIG_TARGET_ID == 0x8909)
// pmic is 8909
static const rpm_resource_config_t temp_config_data_combined[] =
{
    { RPM_LDO_A_REQ,        3 },  // VDDMX
    { RPM_SMPS_A_REQ,       1 },  // VDDCX
    { RPM_SMPS_A_REQ,       2 },  // S2
    { (rpm_resource_type)0, 0 }
};
//pmic is 8916
static const rpm_resource_config_t temp_config_data[] =
{
    { RPM_LDO_A_REQ,        3 },  // VDDMX
    { RPM_SMPS_A_REQ,       1 },  // VDDCX
    { RPM_SMPS_A_REQ,       3 },  // S3
    { (rpm_resource_type)0, 0 }
};

static const rpm_resource_config_t bimc_preprocess_dep[] =
{
    { RPM_BUS_SLAVE_REQ,          0 },
    { RPM_BUS_SLAVE_REQ,          2 },
    { RPM_BUS_MASTER_REQ,         3 },
    { (rpm_resource_type)0,       0 }
};

#elif (DAL_CONFIG_TARGET_ID == 0x9607)

static const rpm_resource_config_t temp_config_data[] =
{
    { RPM_LDO_A_REQ,        12 },  // VDDMX
    { RPM_SMPS_A_REQ,       3 },  // VDDCX
    { RPM_SMPS_A_REQ,       2 },  // S2
    { (rpm_resource_type)0, 0 }
};
// Active->Sleep
static const rpm_resource_config_t bimc_preprocess_dep[] = 
{
    { RPM_BUS_SLAVE_REQ,          0 },
	{ RPM_BUS_SLAVE_REQ,        202 },
	{ RPM_BUS_MASTER_REQ,         0 },
	{ RPM_BUS_MASTER_REQ,         1 },
	{ RPM_BUS_MASTER_REQ,         26 },
    { RPM_BUS_MASTER_REQ,         27 },
    { RPM_BUS_MASTER_REQ,       102 },
	{ RPM_BUS_MASTER_REQ,       139 },
    { RPM_BUS_MASTER_LATENCY_REQ, 1 },
	{ (rpm_resource_type)0,       0 }
};

// Sleep->Active
static const rpm_resource_config_t bimc_postprocess_dep[] = 
{
    { RPM_BUS_SLAVE_REQ,          0 },
	{ RPM_BUS_SLAVE_REQ,        202 },
	{ RPM_BUS_MASTER_REQ,         0 },
	{ RPM_BUS_MASTER_REQ,         1 },
	{ RPM_BUS_MASTER_REQ,         26 },
    { RPM_BUS_MASTER_REQ,         27 },
    { RPM_BUS_MASTER_REQ,       102 },
	{ RPM_BUS_MASTER_REQ,       139 },
    { RPM_BUS_MASTER_LATENCY_REQ, 1 },
	{ (rpm_resource_type)0,       0 }
};


#else
static const rpm_resource_config_t * temp_config_data = NULL;
// Active->Sleep
static const rpm_resource_config_t bimc_preprocess_dep[] =
{
    { RPM_BUS_SLAVE_REQ,          0 },
    { RPM_BUS_SLAVE_REQ,          2 },
    { RPM_BUS_MASTER_REQ,         3 },
    { (rpm_resource_type)0,       0 }
};

// Sleep->Active
static const rpm_resource_config_t *const bimc_postprocess_dep = NULL;

#endif

const rpm_resource_config_t * RESOURCE_CONFIG_DATA;
const rpm_resource_config_t * BIMC_PREPROCESS_DEP;
const rpm_resource_config_t * BIMC_POSTPROCESS_DEP;

extern uint32 railway_target_type;

void rpm_resources_config_init(void) 
{
#if (DAL_CONFIG_TARGET_ID == 0x8909)
    if(railway_target_type == RAILWAY_CX_APC_SPLIT) 
    {
        // pmic is 8916
        RESOURCE_CONFIG_DATA = temp_config_data_combined;
		BIMC_PREPROCESS_DEP  = bimc_preprocess_dep;
    }
    else
    {
        // pmic is 8909
        RESOURCE_CONFIG_DATA = temp_config_data;
		BIMC_PREPROCESS_DEP  = bimc_preprocess_dep;
    }
#elif (DAL_CONFIG_TARGET_ID == 0x9607)
	RESOURCE_CONFIG_DATA = temp_config_data;
	BIMC_PREPROCESS_DEP  = bimc_preprocess_dep;
	BIMC_POSTPROCESS_DEP = bimc_postprocess_dep;
#else
    RESOURCE_CONFIG_DATA = temp_config_data;
#endif
}

