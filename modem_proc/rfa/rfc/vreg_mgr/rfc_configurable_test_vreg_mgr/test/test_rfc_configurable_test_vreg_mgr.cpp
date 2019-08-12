/*!
   @file
   test_rfc_vreg_mgr_generic.cpp

   @brief
   This file contains implementation the rfc_generic_card_cmn_data class, which can be used to
   configure the rf-card related data as required.

*/

/*===========================================================================

Copyright (c) 2011-12 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:52 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rfc/vreg_mgr/rfc_configurable_test_vreg_mgr/test/test_rfc_configurable_test_vreg_mgr.cpp#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
11/07/12   sr      Initial version.

============================================================================*/
/*=============================================================================
                           INCLUDE FILES
=============================================================================*/

#include "TestFramework.h"
#include "comdef.h"
#include "rf_unit_test_env.h"
#include "rfc_class.h"
#include "rfc_configurable_test_vreg_mgr.h"


/*----------------------------------------------------------------------------*/
/*                              Test Cases                                    */
/*----------------------------------------------------------------------------*/

TF_DEFINE_BASIC_TEST_CASE( FUNCTIONAL, rfc_configurable_test_vreg_mgr_construction_test );
TF_DEFINE_BASIC_TEST_CASE( FUNCTIONAL, rfc_configurable_test_vreg_mgr_virtual_fn_test );
/*----------------------------------------------------------------------------*/



void rfc_configurable_test_vreg_mgr_construction_test::Test()
{
  rfc_vreg_mgr *temp_p = NULL;

  //rf_unit_test_setup( 0 );

  /* ----- create vreg mgr ----- */
  temp_p = rfc_configurable_test_vreg_mgr::get_instance();
  TF_ASSERT (temp_p != NULL);
  
  delete temp_p;

  //rf_unit_test_teardown();
}


void rfc_configurable_test_vreg_mgr_virtual_fn_test::Test()
{
  rfc_vreg_mgr *temp_p = NULL;
  rfm_resource_info *resource_info = new rfm_resource_info;
  rfm_tech_info *tech_A = new rfm_tech_info;
  rfm_tech_info *tech_B = new rfm_tech_info;
  tech_A->device_num = RFM_DEVICE_0;
  tech_A->mode = RFM_PARKED_MODE;
  tech_A->path_state = RF_PATH_SLEEP_STATE;
  tech_B->device_num = RFM_DEVICE_1;
  tech_B->mode = RFM_PARKED_MODE;
  tech_B->path_state = RF_PATH_SLEEP_STATE;

  resource_info->tech_A = *tech_A;
  resource_info->tech_B = *tech_B;

  //rf_unit_test_setup( 0 );

  /* ----- create vreg mgr ----- */
  temp_p = rfc_configurable_test_vreg_mgr::get_instance();
  TF_ASSERT (temp_p != NULL);

  //Check to see that calling virtual functions does not cause a crash
  temp_p->enable_autocal_vregs(TRUE);
  temp_p->enable_autocal_vregs(FALSE);
  
  temp_p->cdma_manage_power(rfc_vreg_param());
  temp_p->gsm_manage_power(rfc_vreg_param());
  temp_p->wcdma_manage_power(rfc_vreg_param());
  temp_p->lte_manage_power(rfc_vreg_param());
  temp_p->tdscdma_manage_power(rfc_vreg_param());
  temp_p->gps_manage_power(rfc_vreg_param());

  temp_p->get_awake_resource_request(resource_info); 

  temp_p->set_quiet_mode(RFM_PARKED_MODE, TRUE);
  temp_p->set_quiet_mode(RFM_1X_MODE, FALSE);

  delete temp_p;
  delete tech_A;
  delete tech_B;
  delete resource_info;

  //rf_unit_test_teardown();
}
