/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include <stdlib.h>
#include <string.h>

#include "railway.h"

#include "railway_internal.h"
#include "railway_config.h"
#include "DALSys.h"
#include "CoreVerify.h"

void acc_init(void);

railway_internals_t railway;
int mx_rail_id = RAIL_NOT_SUPPORTED_BY_RAILWAY;

void railway_init(void)
{
    uint32 num_rails = RAILWAY_CONFIG_DATA->num_rails;

    uint32 rail_state_size = num_rails * sizeof(railway_rail_state_t);
    
    DALResult dalResult = DALSYS_Malloc(rail_state_size, (void**)&railway.rail_state);
    CORE_VERIFY_PTR(railway.rail_state);
    CORE_VERIFY(dalResult==DAL_SUCCESS);
    
    memset(railway.rail_state, 0, rail_state_size);

    railway_target_init();
    railway_transitioner_init();

    const int vddcx_rail_id = rail_id("vddcx");
    CORE_VERIFY(RAIL_NOT_SUPPORTED_BY_RAILWAY!=vddcx_rail_id);

    //init time request for lowest active level on CX internal
    railway_voter_t rpm_vdd_cx_voter = railway_create_voter(vddcx_rail_id, RAILWAY_RPM_CX_VOTER_ID);
    railway_corner_vote(rpm_vdd_cx_voter, RAILWAY_SVS_SOC);
    
    mx_rail_id = rail_id("vddmx");
    CORE_VERIFY(RAIL_NOT_SUPPORTED_BY_RAILWAY!=mx_rail_id);

    //init time request for lowest active level on MX internal
    railway_voter_t rpm_vdd_mx_voter = railway_create_voter(mx_rail_id, RAILWAY_RPM_MX_VOTER_ID);
    railway_corner_vote(rpm_vdd_mx_voter, RAILWAY_SVS_SOC);
    
    acc_init();
}

