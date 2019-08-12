/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "railway.h"

#include "railway_internal.h"
#include "railway_config.h"

#include "railway_adapter.h"
#include "kvp.h"
#include "rpmserver.h"
#include "coredump.h"
#include "pmapp_npa.h"


railway_internals_t railway;
int mx_rail_id = RAIL_NOT_SUPPORTED_BY_RAILWAY;
int ebi_rail_id = RAIL_NOT_SUPPORTED_BY_RAILWAY;

void railway_init_v2(void)
{
    uint32 num_rails = RAILWAY_CONFIG_DATA->num_rails;

    uint32 rail_state_size = num_rails * sizeof(railway_rail_state_t);
    railway.rail_state = malloc(rail_state_size);
    CORE_VERIFY(railway.rail_state);
    memset(railway.rail_state, 0, rail_state_size);

    ebi_rail_id = rail_id("vdda_ebi");

    mx_rail_id = rail_id("vddmx");
    CORE_VERIFY(RAIL_NOT_SUPPORTED_BY_RAILWAY!=mx_rail_id);

    railway_target_init();
    railway_adapter_init();
    railway_transitioner_init();
    
    rpm_core_dump.railway_state  = &railway;
    rpm_core_dump.railway_config = RAILWAY_CONFIG_DATA;

    const int vddcx_rail_id = rail_id("vddcx");
    CORE_VERIFY(RAIL_NOT_SUPPORTED_BY_RAILWAY!=vddcx_rail_id);

    //init time request for lowest active level on CX internal
    railway_voter_t rpm_vdd_cx_voter = railway_create_voter(vddcx_rail_id, true, RAILWAY_RPM_CX_VOTER_ID);
    railway_corner_vote(rpm_vdd_cx_voter, RAILWAY_SVS_SOC);

    //init time request for sleep settings on CX internal
    railway_voter_t rpm_vdd_cx_sleep_voter = railway_create_voter(vddcx_rail_id, false, RAILWAY_RPM_CX_SLEEP_VOTER_ID);
    railway_corner_vote(rpm_vdd_cx_sleep_voter, RAILWAY_RETENTION);
    railway_sw_enable_vote(rpm_vdd_cx_sleep_voter, true);

    //init time request for lowest active level on MX internal
    railway_voter_t rpm_vdd_mx_voter = railway_create_voter(mx_rail_id, true, RAILWAY_RPM_MX_VOTER_ID);
    railway_corner_vote(rpm_vdd_mx_voter, RAILWAY_SVS_SOC);
    
    //init time request for sleep settings on MX internal
    railway_voter_t rpm_vdd_mx_sleep_voter = railway_create_voter(mx_rail_id, false, RAILWAY_RPM_MX_SLEEP_VOTER_ID);
    railway_corner_vote(rpm_vdd_mx_sleep_voter, RAILWAY_RETENTION);
    railway_sw_enable_vote(rpm_vdd_mx_sleep_voter, true);

    if(ebi_rail_id != RAIL_NOT_SUPPORTED_BY_RAILWAY)
    {
        //init time request for lowest active level on EBI internal
        railway_voter_t rpm_vdda_ebi_voter = railway_create_voter(ebi_rail_id, true, RAILWAY_RPM_EBI_VOTER_ID);
        railway_corner_vote(rpm_vdda_ebi_voter, RAILWAY_RETENTION);

        //init time request for sleep settings on EBI internal
        railway_voter_t rpm_vdda_ebi_sleep_voter = railway_create_voter(ebi_rail_id, false, RAILWAY_RPM_EBI_SLEEP_VOTER_ID);
        railway_corner_vote(rpm_vdda_ebi_sleep_voter, RAILWAY_RETENTION);
        railway_sw_enable_vote(rpm_vdda_ebi_sleep_voter, true);

#ifdef PIN_VDDA_EBI_VOLTAGE
    // CR 674763 - set VDDA_EBI to 1.125V during bringup for DDR folks
    railway_set_pmic_voltage(ebi_rail_id, 1125000);
#endif /* PIN_VDDA_EBI_VOLTAGE */
    }
}

void railway_finalize_init(void)
{
    CORE_VERIFY(mx_rail_id!=RAIL_NOT_SUPPORTED_BY_RAILWAY);
    
    const railway_rail_config_t *mx_config = &RAILWAY_CONFIG_DATA->rails[mx_rail_id];
    
    rpm_set_last_transitioning_resource(mx_config->vreg_type, mx_config->vreg_num);
}
