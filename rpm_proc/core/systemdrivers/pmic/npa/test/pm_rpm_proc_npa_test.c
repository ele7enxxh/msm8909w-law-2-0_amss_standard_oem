///*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
//
//             P M    NPA   S E R V I C E S
//
//GENERAL DESCRIPTION
//  This file contains initialization functions for NPA
//
//EXTERNALIZED FUNCTIONS
//  None.
//
//INITIALIZATION AND SEQUENCING REQUIREMENTS
//  None.
//
//Copyright (c) 2010           by Qualcomm Technologies, Inc.  All Rights Reserved.
//*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
//
///*===========================================================================
//
//                        EDIT HISTORY FOR MODULE
//
//  This section contains comments describing changes made to the module.
//  Notice that changes are listed in reverse chronological order.
//
//$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/npa/test/pm_rpm_proc_npa_test.c#1 $
//$DateTime: 2015/02/25 02:53:43 $ $Author: pwbldsvc $
//
//when       who     what, where, why
//--------   ---     ----------------------------------------------------------
//07/01/10   umr     Created.
//===========================================================================*/
///*===========================================================================
//
//                     INCLUDE FILES FOR MODULE
//
//===========================================================================*/
//#include "comdef.h"
//
//#include "npa.h"
//#include "npa_resource.h"
//#include "npa_init.h"
//#include "RPMTypes.h"
//
//#include "pm_dvt.h"
//
////#ifdef PMIC_OFFTARGET_TESTING
//
//extern void busywait (uint32 pause_time_us);
////#include "psdte_8058.h"
///*===========================================================================
//
//                 LOCAL CONSTANT AND MACRO DEFINITIONS
//
//===========================================================================*/
//#define PMIC_NPA_SMPS_A_RSRC_COUNT       3
//#define PMIC_NPA_SMPS_B_RSRC_COUNT       3
//
//#define PMIC_NPA_LDO_A_RSRC_COUNT       26
//#define PMIC_NPA_LDO_B_RSRC_COUNT        7
//
//#define PMIC_NPA_VS_A_RSRC_COUNT         2
//#define PMIC_NPA_VS_B_RSRC_COUNT         7
//
//#define PMIC_NPA_NCP_A_RSRC_COUNT        1
//
//#define PMIC_NPA_CXO_A_RSRC_COUNT        1
//
////#define PM_RPM_NPA_TEST_CXO
////#define PM_RPM_NPA_TEST_VDD_MEM_DIG
////#define PM_RPM_NPA_TEST_PINCTL
//
////#define PM_RPM_NPA_TEST_SMPS_A
////#define PM_RPM_NPA_TEST_LDO_A
////#define PM_RPM_NPA_TEST_VS_A
//
////#define PM_RPM_NPA_TEST_SMPS_B
////#define PM_RPM_NPA_TEST_LDO_B
////#define PM_RPM_NPA_TEST_VS_B
//
///*===========================================================================
//
//                LOCAL FUNCTION PROTOTYPES
//
//===========================================================================*/
//void*
//pm_rpm_device_rsrc_set_vreg_cb_offtarget ( void *usr_data, uint32 arg2,
//                                            void *req, uint32 req_len );
//
//pm_register_data_type read_data;
///*===========================================================================
//
//                VARIABLES DEFINITIONS
//
//===========================================================================*/
//typedef struct
//{
//  unsigned int mvol:8;
//  unsigned int on_off:1;
//  unsigned int reserved:23;
//}pm_rpmfw_vreg_read_test_type;
//
//typedef struct
//{
//  DAL_rpmfw_MasterType master;
//  DAL_rpm_ResourceType rpm_rsrc;
//  pm_npa_vreg_smps_type payload;
//  pm_rpmfw_vreg_read_test_type read;
//}pm_rpmfw_vreg_req_test_type;
//
//typedef struct
//{
//  DAL_rpmfw_MasterType master;
//  DAL_rpm_ResourceType rpm_rsrc;
//  pm_npa_vreg_vs_type payload;
//  pm_rpmfw_vreg_read_test_type read;
//}pm_rpmfw_vs_req_test_type;
//
//extern pm_npa_rpm_config_ctx_data_type
//pm_npa_rpm_smps_cfg_ctx_data_8058,
//pm_npa_rpm_ldo_cfg_ctx_data_8058,
//pm_npa_rpm_vs_cfg_ctx_data_8058,
//pm_npa_rpm_ncp_cfg_ctx_data_8058,
//pm_npa_rpm_smps_cfg_ctx_data_8901,
//pm_npa_rpm_ldo_cfg_ctx_data_8901,
//pm_npa_rpm_vs_cfg_ctx_data_8901;
//
//
//extern pm_npa_rpm_config_ctx_data_type
//pm_npa_rpm_cxo_cfg_ctx_data_8058;
//
//DAL_rpmfw_RequestType rpm_req;
//
//pm_switch_cmd_type state;
//
///*===========================================================================
//
//                FUNCTION DEFINITIONS
//
//===========================================================================*/
//#ifdef PM_RPM_NPA_TEST_CXO
//static pm_npa_cxo_type
//pm_npa_rpm_cxo_test_data [] = 
//{
//  {
//    PM_NPA_CXO_STATE_OFF,     //D0
//    0,
//    PM_NPA_CXO_STATE_ON,      //D1
//    0,
//    PM_NPA_CXO_STATE_OFF,      //A0
//    0,
//    PM_NPA_CXO_STATE_OFF,     //A1
//    0
//  },
//  {
//    PM_NPA_CXO_STATE_PIN_CTL,  //D0
//    0,
//    PM_NPA_CXO_STATE_ON,     //D1
//    0,
//    PM_NPA_CXO_STATE_ON,     //A0
//    0,
//    PM_NPA_CXO_STATE_ON,    //A1
//    0
//  },
//  {
//    PM_NPA_CXO_STATE_ON,       //D0
//    0,
//    PM_NPA_CXO_STATE_ON,       //D1
//    0,
//    PM_NPA_CXO_STATE_PIN_CTL,  //A0
//    0,
//    PM_NPA_CXO_STATE_PIN_CTL,  //A1
//    0
//  },
//  {
//    PM_NPA_CXO_STATE_OFF, 0,  //D0
//    PM_NPA_CXO_STATE_OFF, 0,  //D1
//    PM_NPA_CXO_STATE_OFF, 0, //A0
//    PM_NPA_CXO_STATE_OFF, 0, //A1
//  },
//  {
//    PM_NPA_CXO_STATE_ON, 0,  //D0
//    PM_NPA_CXO_STATE_MAX, 0, //D1
//    PM_NPA_CXO_STATE_ON, 0,  //A0
//    PM_NPA_CXO_STATE_ON, 0,  //A1
//  },
//  {
//    PM_NPA_CXO_STATE_OFF, 0,  //D0
//    PM_NPA_CXO_STATE_OFF, 0,  //D1
//    PM_NPA_CXO_STATE_OFF, 0,  //A0
//    PM_NPA_CXO_STATE_OFF, 0,  //A1
//  },
//  {
//    PM_NPA_CXO_STATE_ON, 0,  //D0
//    PM_NPA_CXO_STATE_ON, 0,  //D1
//    PM_NPA_CXO_STATE_ON, 0,  //A0
//    PM_NPA_CXO_STATE_ON, 0,  //A1
//  },
//  {
//    PM_NPA_CXO_STATE_OFF, 0,  //D0
//    PM_NPA_CXO_STATE_ON, 0,   //D1
//    PM_NPA_CXO_STATE_OFF, 0,  //A0
//    PM_NPA_CXO_STATE_OFF, 0,  //A1
//  },
//};
//
//void
//pm_rpm_proc_npa_test_cxo(void)
//{
//  int i;
//
//  rpm_test_handle = npa_create_sync_client ("/pmic/device/rpm/xo/A/cxo/vec", "Modem_test",
//                                    NPA_CLIENT_REQUIRED);
//
//  if (rpm_test_handle)
//  {
//    for (i = 0; i < sizeof(pm_npa_rpm_cxo_test_data) / sizeof (pm_npa_rpm_cxo_test_data[0]); i++)
//    {
//      npa_issue_required_request (rpm_test_handle, 
//                                  *((npa_resource_state *)&pm_npa_rpm_cxo_test_data[i]));
//      busywait(100000);
//    }
//  }
//  //npa_destroy_client (rpm_test_handle);
//}
//#endif
//
//void
//pm_rpm_proc_npa_test_scor_pwr_rail(void)
//{
//  /* 8901 SMPS 0 */
//  pm_vreg_ctrl(PM_ON_CMD, PM_SMPS_6);
//  pm_vreg_ctrl_status (&state, PM_SMPS_6);
//  busywait(100000);
//  pm_vreg_ctrl(PM_OFF_CMD, PM_SMPS_6);
//  pm_vreg_ctrl_status (&state, PM_SMPS_6);
//  busywait(100000);
//  pm_vreg_ctrl(PM_ON_CMD, PM_SMPS_6);
//  pm_vreg_ctrl_status (&state, PM_SMPS_6);
//  busywait(100000);
//
//  /* 8901 SMPS 1 */
//  pm_vreg_ctrl(PM_ON_CMD, PM_SMPS_7);
//  pm_vreg_ctrl_status (&state, PM_SMPS_7);
//  busywait(100000);
//  pm_vreg_ctrl(PM_OFF_CMD, PM_SMPS_7);
//  pm_vreg_ctrl_status (&state, PM_SMPS_7);
//  busywait(100000);
//  pm_vreg_ctrl(PM_ON_CMD, PM_SMPS_7);
//  pm_vreg_ctrl_status (&state, PM_SMPS_7);
//  busywait(100000);
//
//  /* 8901 SMPS 2 */
//  pm_vreg_ctrl(PM_ON_CMD, PM_SMPS_8);
//  pm_vreg_ctrl_status (&state, PM_SMPS_8);
//  busywait(100000);
//  pm_vreg_ctrl(PM_OFF_CMD, PM_SMPS_8);
//  pm_vreg_ctrl_status (&state, PM_SMPS_8);
//  busywait(100000);
//  pm_vreg_ctrl(PM_ON_CMD, PM_SMPS_8);
//  pm_vreg_ctrl_status (&state, PM_SMPS_8);
//  busywait(100000);
//
//  /* 8901 SMPS 3 */
//  pm_vreg_ctrl(PM_ON_CMD, PM_SMPS_9);
//  pm_vreg_ctrl_status (&state, PM_SMPS_9);
//  busywait(100000);
//  pm_vreg_ctrl(PM_OFF_CMD, PM_SMPS_9);
//  pm_vreg_ctrl_status (&state, PM_SMPS_9);
//  busywait(100000);
//  pm_vreg_ctrl(PM_ON_CMD, PM_SMPS_9);
//  pm_vreg_ctrl_status (&state, PM_SMPS_9);
//  busywait(100000);
//}
//
//#ifdef PM_RPM_NPA_TEST_SMPS_A
//static pm_rpmfw_vreg_req_test_type
//pm_rpm_tests_smps_a[] = 
//{
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_SMPS2,
//    {1300, 500, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE,
//      500, PM_NPA_FREQ_1P60
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_SMPS2,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE,
//      0, PM_NPA_FREQ_1P60
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_SMPS2,
//    {1237, 500, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE,
//      500, PM_NPA_FREQ_1P60
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_SMPS2,
//    {1300, 500, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE,
//      500, PM_NPA_FREQ_2P74
//    },
//  },
//  {  
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_SMPS2,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE,
//      0, PM_NPA_FREQ_1P60
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_SMPS4,
//    {2200, 500, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE,
//      500, PM_NPA_FREQ_1P60
//    },
//  },
//};
//
//void
//pm_rpm_proc_npa_test_smps_a(void)
//{
//  int i;
//
//  for (i = 0; i < sizeof (pm_rpm_tests_smps_a)/sizeof (pm_rpm_tests_smps_a[0]); i++)
//  {
//    rpm_req.Resource = pm_rpm_tests_smps_a[i].rpm_rsrc;
//    rpm_req.Master = pm_rpm_tests_smps_a[i].master;
//    rpm_req.DataLen = 8;
//    rpm_req.Data = &pm_rpm_tests_smps_a[i].payload;
//
//    pm_rpm_device_rsrc_set_vreg_cb_offtarget (&pm_npa_rpm_smps_cfg_ctx_data_8058, 0,
//                                               &rpm_req, sizeof(DAL_rpmfw_RequestType) );
//  }
//}
//#endif
//
//#ifdef PM_RPM_NPA_TEST_LDO_A
//static pm_rpmfw_vreg_req_test_type
//pm_rpm_tests_ldo_a[] = 
//{
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO0,
//    {1200, 500, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 500
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO1,
//    {0, 500, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 500
//    },
//  },
//  { /* Re-check 1.2*/
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO1,
//    {1800, 500, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 500
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO1,
//    {0, 500, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 500
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_LDO3,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 0
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_LDO3,
//    {1800, 100, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 100
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO3,
//    {1900, 100, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 100
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO3,
//    {0, 100, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE,100
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_LDO3,
//    {3050, 100, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE,100
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_LDO3,
//    {3000, 100, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 100
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO3,
//    {0, 100, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE,100
//    },
//  },
//
//  /* LDO 4 Sources CXO No Test */
//
//  /* LDO 5 ON on sleep */
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_LDO5,
//    {2850, 100, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 100
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_LDO6,
//    {3300, 100, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 100
//    },
//  },
//
//  /* ON During sleep */
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_LDO7,
//    {1800, 100, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 100
//    },
//  },
//
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_LDO8, /* DNW- low V 0.7*/
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE,0
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_LDO8,
//    {2900, 100, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 100
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_LDO8, /* DNW- low V 0.7*/
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE,0
//    },
//  },
//
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_LDO9,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE,0
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_LDO9,
//    {3050, 100, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 100
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_LDO9,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE,0
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_LDO13,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE,0
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_LDO13,
//    {2050, 100, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 100
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_LDO13,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE,0
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_LDO13,
//    {2050, 100, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 100
//    },
//  },
//
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO14, /* DNW 1.0 v ???*/
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE,0
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO14,
//    {2850, 100, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 100
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO14, /* DNW 1.0 v ???*/
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE,0
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO14,
//    {2850, 100, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 100
//    },
//  },
//
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO15,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE,0
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO15,
//    {2850, 100, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 100
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO15,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE,0
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO15,
//    {2850, 100, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 100
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO16,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE,0
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO16,
//    {1800, 100, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 100
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_LDO17,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE,0
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_LDO17,
//    {2600, 100, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 100
//    },
//  },
//
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_LDO18, /* DNW- crash seen some times.. not any more*/
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE,0
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_LDO18,
//    {2200, 100, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 100
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_LDO18,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE,0
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_LDO18,
//    {2200, 100, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 100
//    },
//  },
//  { /* DNW 0.811 leakage */
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_LDO20,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE,0
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_LDO20,
//    {1800, 100, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 100
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_LDO21,
//    {1200, 100, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 100
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_LDO25,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE,0
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_LDO25,
//    {1200, 100, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 100
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_LDO25,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE,0
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_LDO25,
//    {1200, 100, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 100
//    },
//  },
//};
//
//void
//pm_rpm_proc_npa_test_ldo_a(void)
//{
//  int i;
//
//  for (i = 0; i < sizeof (pm_rpm_tests_ldo_a)/sizeof (pm_rpm_tests_ldo_a[0]); i++)
//  {
//    rpm_req.Resource = pm_rpm_tests_ldo_a[i].rpm_rsrc;
//    rpm_req.Master = pm_rpm_tests_ldo_a[i].master;
//    rpm_req.DataLen = 8;
//    rpm_req.Data = &pm_rpm_tests_ldo_a[i].payload;
//
//    pm_rpm_device_rsrc_set_vreg_cb_offtarget (&pm_npa_rpm_ldo_cfg_ctx_data_8058, 0,
//                                              &rpm_req, sizeof(DAL_rpmfw_RequestType) );
//  }
//}
//#endif
//
//#ifdef PM_RPM_NPA_TEST_VS_A
//pm_rpmfw_vs_req_test_type
//pm_rpm_tests_vs_a[] = 
//{
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LVS0,
//    {PM_NPA_VREG_STATE_OFF, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LVS0,
//    {PM_NPA_VREG_STATE_ON, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LVS0,
//    {PM_NPA_VREG_STATE_OFF, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LVS0,
//    {PM_NPA_VREG_STATE_ON, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LVS1,
//    {PM_NPA_VREG_STATE_OFF, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LVS1,
//    {PM_NPA_VREG_STATE_ON, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LVS1,
//    {PM_NPA_VREG_STATE_OFF, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LVS1,
//    {PM_NPA_VREG_STATE_ON, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//};
//
//void
//pm_rpm_proc_npa_test_vs_a(void)
//{
//  int i;
//
//  for (i = 0; i < sizeof (pm_rpm_tests_vs_a)/sizeof (pm_rpm_tests_vs_a[0]); i++)
//  {
//    rpm_req.Resource = pm_rpm_tests_vs_a[i].rpm_rsrc;
//    rpm_req.Master = pm_rpm_tests_vs_a[i].master;
//    rpm_req.DataLen = 4;
//    rpm_req.Data = &pm_rpm_tests_vs_a[i].payload;
//
//    pm_rpm_device_rsrc_set_vreg_cb_offtarget (&pm_npa_rpm_vs_cfg_ctx_data_8058, 0,
//                                              &rpm_req, sizeof(DAL_rpmfw_RequestType) );
//    busywait(100000);
//  }
//
//
//  /*LVS 0 Off */
//  pm_dvt_in (0x12d, 0x00 ,0);
//  pm_dvt_in_mask (0x12d , 0x0 , 0x80 ,0);
//  pm_dvt_out (0x12d, &read_data, 0);
//  /*LVS 0 ON */
//  pm_dvt_in (0x12d, 0x80 , 0);
//  pm_dvt_out (0x12d, &read_data, 0);
//
//  /*LVS 0 OFF */
//  pm_dvt_in (0x12d, 0x10 , 0);
//  pm_dvt_out (0x12d, &read_data,0);
//
//  /*LVS 0 ON */
//  pm_dvt_in (0x12d, 0x90 , 0);
//  pm_dvt_out (0x12d, &read_data ,0);
//
//  /**************************************************************/
//
//  /*LVS 1 Off */
//  pm_dvt_in (0x12f, 0x10, 0);
//  pm_dvt_out ( 0x12f, &read_data, 0);
//
//    /*LVS 1 ON */
//  pm_dvt_in (0x12f, 0x90, 0);
//  pm_dvt_out ( 0x12f, &read_data, 0);
//
//  /*LVS 1 OFF */
//  pm_dvt_in (0x12f, 0x10, 0);
//  pm_dvt_out ( 0x12f, &read_data, 0);
//
//  /*LVS 1 ON */
//  pm_dvt_in (0x12f, 0x90, 0);
//  pm_dvt_out ( 0x12f, &read_data, 0);
//}
//#endif
//
//#ifdef PM_RPM_NPA_TEST_SMPS_B
//static pm_rpmfw_vreg_req_test_type
//pm_rpm_tests_smps_b[] = 
//{
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_SMPS2B,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE,
//      0, PM_NPA_FREQ_1P60
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_SMPS2B, /* 250 mv ripple. Not used*/
//    {1800, 500, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE,
//      500, PM_NPA_FREQ_1P60
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_SMPS2B, /* 260+ */
//    {2300, 500, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE,
//      500, PM_NPA_FREQ_1P60
//    },
//  },
//};
//
//void
//pm_rpm_proc_npa_test_smps_b(void)
//{
//  int i;
//
//  for (i = 0; i < sizeof (pm_rpm_tests_smps_b)/sizeof (pm_rpm_tests_smps_b[0]); i++)
//  {
//    rpm_req.Resource = pm_rpm_tests_smps_b[i].rpm_rsrc;
//    rpm_req.Master = pm_rpm_tests_smps_b[i].master;
//    rpm_req.DataLen = 8;
//    rpm_req.Data = &pm_rpm_tests_smps_b[i].payload;
//
//    pm_rpm_device_rsrc_set_vreg_cb_offtarget (&pm_npa_rpm_smps_cfg_ctx_data_8901, 0,
//                                               &rpm_req, sizeof(DAL_rpmfw_RequestType) );
//    busywait(100000);
//  }
//}
//#endif
//
//#ifdef PM_RPM_NPA_TEST_LDO_B
//static pm_rpmfw_vreg_req_test_type
//pm_rpm_tests_ldo_b[] = 
//{
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO0B,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 0
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO0B,
//    {1225, 100, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_A0 ,
//      PM_NPA_VREG_PIN_FUNCT_ON_OFF, PM_NPA_VREG_PULL_DOWN_ENABLE, 100
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO0B,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 0
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO1B,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 0
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO1B,
//    {3300, 100, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 100
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO1B,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 0
//    },
//  },
//
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO2B,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 0
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO2B, /* 200 mv low rip*/
//    {3300, 100, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 100
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO2B,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 0
//    },
//  },
//
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO3B,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 0
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO3B,/* 150 mv low rip*/
//    {3300, 100, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 100
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO3B,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 0
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO4B,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 0
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO4B,
//    {2600, 100, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 100
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO4B,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 0
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO5B,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 0
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO5B,
//    {2850, 100, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 100
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO5B,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 0
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0, /* 0.407 */
//    DAL_RPM_RESOURCE_LDO6B,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 0
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO6B,
//    {2200, 100, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 100
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO6B,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 0
//    },
//  },
//};
//
//void
//pm_rpm_proc_npa_test_ldo_b(void)
//{
//  int i;
//
//  for (i = 0; i < sizeof (pm_rpm_tests_ldo_b)/sizeof (pm_rpm_tests_ldo_b[0]); i++)
//  {
//    rpm_req.Resource = pm_rpm_tests_ldo_b[i].rpm_rsrc;
//    rpm_req.Master = pm_rpm_tests_ldo_b[i].master;
//    rpm_req.DataLen = 8;
//    rpm_req.Data = &pm_rpm_tests_ldo_b[i].payload;
//
//    pm_rpm_device_rsrc_set_vreg_cb_offtarget (&pm_npa_rpm_ldo_cfg_ctx_data_8901, 0,
//                                              &rpm_req, sizeof(DAL_rpmfw_RequestType) );
//    busywait(100000);
//  }
//}
//#endif
//
//#ifdef PM_RPM_NPA_TEST_VS_B
//pm_rpmfw_vs_req_test_type
//pm_rpm_tests_vs_b[] = 
//{
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LVS0B,
//    {PM_NPA_VREG_STATE_OFF, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LVS0B,
//    {PM_NPA_VREG_STATE_ON, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LVS0B,
//    {PM_NPA_VREG_STATE_OFF, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LVS0B,
//    {PM_NPA_VREG_STATE_ON, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LVS1B,
//    {PM_NPA_VREG_STATE_OFF, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LVS1B,
//    {PM_NPA_VREG_STATE_ON, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LVS1B,
//    {PM_NPA_VREG_STATE_OFF, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LVS1B,
//    {PM_NPA_VREG_STATE_ON, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LVS2B,
//    {PM_NPA_VREG_STATE_OFF, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LVS2B,
//    {PM_NPA_VREG_STATE_ON, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LVS2B,
//    {PM_NPA_VREG_STATE_OFF, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LVS2B,
//    {PM_NPA_VREG_STATE_ON, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LVS3B,
//    {PM_NPA_VREG_STATE_OFF, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LVS3B,
//    {PM_NPA_VREG_STATE_ON, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LVS3B,
//    {PM_NPA_VREG_STATE_OFF, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LVS3B,
//    {PM_NPA_VREG_STATE_ON, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_MVS, /* DNW- */
//    {PM_NPA_VREG_STATE_OFF, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_MVS,
//    {PM_NPA_VREG_STATE_ON, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_MVS,
//    {PM_NPA_VREG_STATE_OFF, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_MVS,
//    {PM_NPA_VREG_STATE_ON, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_USB_OTG_SWITCH, 
//    {PM_NPA_VREG_STATE_OFF, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_USB_OTG_SWITCH,
//    {PM_NPA_VREG_STATE_ON, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_USB_OTG_SWITCH,
//    {PM_NPA_VREG_STATE_OFF, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_USB_OTG_SWITCH,
//    {PM_NPA_VREG_STATE_ON, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_HDMI_SWITCH,
//    {PM_NPA_VREG_STATE_OFF, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_HDMI_SWITCH,
//    {PM_NPA_VREG_STATE_ON, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_HDMI_SWITCH,
//    {PM_NPA_VREG_STATE_OFF, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_HDMI_SWITCH,
//    {PM_NPA_VREG_STATE_ON, PM_NPA_VREG_PULL_DOWN_ENABLE,
//     PM_NPA_VREG_PIN_CTL_NONE, PM_NPA_VREG_PIN_FUNCT_MANUAL,
//    },
//  },
//};
//
//void
//pm_rpm_proc_npa_test_vs_b(void)
//{
//  int i;
//
//  for (i = 0; i < sizeof (pm_rpm_tests_vs_b)/sizeof (pm_rpm_tests_vs_b[0]); i++)
//  {
//    rpm_req.Resource = pm_rpm_tests_vs_b[i].rpm_rsrc;
//    rpm_req.Master = pm_rpm_tests_vs_b[i].master;
//    rpm_req.DataLen = 4;
//    rpm_req.Data = &pm_rpm_tests_vs_b[i].payload;
//
//    pm_rpm_device_rsrc_set_vreg_cb_offtarget (&pm_npa_rpm_vs_cfg_ctx_data_8901, 0,
//                                              &rpm_req, sizeof(DAL_rpmfw_RequestType) );
//    busywait(100000);
//  }
//}
//#endif
//
//#ifdef PM_RPM_NPA_TEST_VDD_MEM_DIG
//npa_client_handle rpm_test_handle;
//pm_npa_vreg_smps_type pm_npa_rpm_smps_test_data [5];
//
//npa_client_handle pm_test_vdd_mem_sc;
//npa_client_handle pm_test_vdd_mem_q6;
//npa_client_handle pm_test_vdd_mem_mo;
//npa_client_handle pm_test_vdd_mem_rpm;
//
//npa_client_handle pm_test_vdd_dig_sc;
//npa_client_handle pm_test_vdd_dig_q6;
//npa_client_handle pm_test_vdd_dig_mo;
//npa_client_handle pm_test_vdd_dig_rpm;
//
//void
//pm_rpm_proc_npa_test_vdd_mem_dig(void)
//{
//
//  pm_test_vdd_mem_q6 = npa_create_sync_client ("/pmic/device/rpm/smps/A/smps0/vec", "Q6_test",
//                                    NPA_CLIENT_VECTOR);
//  pm_test_vdd_mem_sc = npa_create_sync_client ("/pmic/device/rpm/smps/A/smps0/vec", "Apps_test",
//                                    NPA_CLIENT_VECTOR);
//  pm_test_vdd_mem_mo = npa_create_sync_client ("/pmic/device/rpm/smps/A/smps0/vec", "Modem_test",
//                                    NPA_CLIENT_VECTOR);
//  pm_test_vdd_mem_rpm = npa_create_sync_client ("/pmic/device/rpm/smps/A/smps0/vec", "/clk_test",
//                                    NPA_CLIENT_VECTOR);
//
//  pm_test_vdd_dig_q6 = npa_create_sync_client ("/pmic/device/rpm/smps/A/smps1/vec", "Q6_test",
//                                    NPA_CLIENT_VECTOR);
//  pm_test_vdd_dig_sc = npa_create_sync_client ("/pmic/device/rpm/smps/A/smps1/vec", "Apps_test",
//                                    NPA_CLIENT_VECTOR);
//  pm_test_vdd_dig_mo = npa_create_sync_client ("/pmic/device/rpm/smps/A/smps1/vec", "Modem_test",
//                                    NPA_CLIENT_VECTOR);
//  pm_test_vdd_dig_rpm = npa_create_sync_client ("/pmic/device/rpm/smps/A/smps1/vec", "/clk_test",
//                                    NPA_CLIENT_VECTOR);
//
//  if (pm_test_vdd_mem_sc)
//  {
//    pm_npa_rpm_smps_test_data [0].mvol = 1200;
//
//    npa_issue_vector_request( pm_test_vdd_mem_sc, 2, (npa_resource_state *)&pm_npa_rpm_smps_test_data[0] );
//    /* S0A Expected 1200 ; Seen  1197   */
//  }
//
//  if (pm_test_vdd_mem_q6)
//  {
//    pm_npa_rpm_smps_test_data [0].mvol = 1100;
//
//    npa_issue_vector_request( pm_test_vdd_mem_q6, 2, (npa_resource_state *)&pm_npa_rpm_smps_test_data [0] );
//    /* S0A Expected 1200 ; Seen  1197   */
//  }
//
//  if (pm_test_vdd_mem_sc)
//  {
//    pm_npa_rpm_smps_test_data [0].mvol = 1000;
//
//    npa_issue_vector_request( pm_test_vdd_mem_sc, 2, (npa_resource_state *)&pm_npa_rpm_smps_test_data[0] );
//    /* S0A Expected 1100 ; Seen  1101   */
//  }
//
//  if (pm_test_vdd_mem_sc)
//  {
//    pm_npa_rpm_smps_test_data [0].mvol = 1100;
//
//    npa_issue_vector_request( pm_test_vdd_mem_sc, 2, (npa_resource_state *)&pm_npa_rpm_smps_test_data[0] );
//    /* S0A Expected 1100 ; Seen  1101   */
//  }
//
//  if (pm_test_vdd_mem_sc)
//  {
//    pm_npa_rpm_smps_test_data [0].mvol = 1150;
//
//    npa_issue_vector_request( pm_test_vdd_mem_sc, 2, (npa_resource_state *)&pm_npa_rpm_smps_test_data[0] );
//    /* S0A Expected 1150 ; Seen  1149 */
//  }
//
//  if (pm_test_vdd_dig_q6)
//  {
//    pm_npa_rpm_smps_test_data [0].mvol = 1100;
//
//    npa_issue_vector_request( pm_test_vdd_dig_q6, 2, (npa_resource_state *)&pm_npa_rpm_smps_test_data [0] );
//    /* S1A Expected 1100 ; Seen  1098   */
//  }
//
//  if (pm_test_vdd_dig_sc)
//  {
//    pm_npa_rpm_smps_test_data [0].mvol = 1200;
//
//    npa_issue_vector_request( pm_test_vdd_dig_sc, 2, (npa_resource_state *)&pm_npa_rpm_smps_test_data[0] );
//    /* S0A Expected 1150 ; Seen 1147 */
//    /* S1A Expected 1200 ; Seen 1194 */
//  }
//
//  if (pm_test_vdd_dig_mo)
//  {
//    pm_npa_rpm_smps_test_data [0].mvol = 1000;
//
//    npa_issue_vector_request( pm_test_vdd_dig_mo, 2, (npa_resource_state *)&pm_npa_rpm_smps_test_data[0] );
//    /* S0A Expected 1200 ; Seen  1197   */
//    /* S1A Expected 1200 ; Seen  1194   */
//  }
//
//  if (pm_test_vdd_dig_rpm)
//  {
//    pm_npa_rpm_smps_test_data [0].mvol = 750;
//
//    npa_issue_vector_request( pm_test_vdd_dig_rpm, 2, (npa_resource_state *)&pm_npa_rpm_smps_test_data[0] );
//    /* Active */
//  }
//
//  if (pm_test_vdd_dig_q6)
//  {
//    pm_npa_rpm_smps_test_data [0].mvol = 750;
//
//    npa_issue_vector_request( pm_test_vdd_dig_q6, 2, (npa_resource_state *)&pm_npa_rpm_smps_test_data [0] );
//    /* Active */
//  }
//
//  if (pm_test_vdd_dig_sc)
//  {
//    pm_npa_rpm_smps_test_data [0].mvol = 750;
//
//    npa_issue_vector_request( pm_test_vdd_dig_sc, 2, (npa_resource_state *)&pm_npa_rpm_smps_test_data[0] );
//    /* Active */
//  }
//
//  if (pm_test_vdd_dig_mo)
//  {
//    pm_npa_rpm_smps_test_data [0].mvol = 750;
//
//    npa_issue_vector_request( pm_test_vdd_dig_mo, 2, (npa_resource_state *)&pm_npa_rpm_smps_test_data[0] );
//    /* Active 1150 and 999   */
//  }
//}
//#endif
//
//
//#ifdef PM_RPM_NPA_TEST_PINCTL
//static pm_rpmfw_vreg_req_test_type
//pm_rpm_tests_pinctl_ldo_b[] = 
//{
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO0B,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 0
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO0B,
//    {1200, 40, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_A0,
//      PM_NPA_VREG_PIN_FUNCT_ON_OFF, PM_NPA_VREG_PULL_DOWN_ENABLE, 0
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO0B,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE, 0
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_LDO0B,
//    {1200, 40, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_A0,
//      PM_NPA_VREG_PIN_FUNCT_ON_OFF, PM_NPA_VREG_PULL_DOWN_ENABLE, 0
//    },
//  },
//};
//
//static pm_rpmfw_vreg_req_test_type
//pm_rpm_tests_pinctl_smps_a[] = 
//{
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_SMPS2,
//    {1300, 500, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_A0,
//      PM_NPA_VREG_PIN_FUNCT_ON_OFF, PM_NPA_VREG_PULL_DOWN_ENABLE,
//      500, PM_NPA_FREQ_1P60
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_SMPS2,
//    {1300, 500, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE,
//      0, PM_NPA_FREQ_1P60
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_SMPS2,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE,
//      0, PM_NPA_FREQ_1P60
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_SMPS2,
//    {1300, 500, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE,
//      0, PM_NPA_FREQ_1P60
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_0,
//    DAL_RPM_RESOURCE_SMPS2,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_ON_OFF, PM_NPA_VREG_PULL_DOWN_ENABLE,
//      0, PM_NPA_FREQ_1P60
//    },
//  },
//  {
//    DAL_RPMFW_MASTER_1,
//    DAL_RPM_RESOURCE_SMPS2,
//    {0, 0, PM_NPA_VREG_FORCE_MODE_NONE, PM_NPA_VREG_PIN_CTL_NONE,
//      PM_NPA_VREG_PIN_FUNCT_MANUAL, PM_NPA_VREG_PULL_DOWN_ENABLE,
//      0, PM_NPA_FREQ_1P60
//    },
//  },
//};
//
//void
//pm_rpm_proc_npa_test_pin_ctl(void)
//{
//  int i;
//
//  pm_vreg_set_level (PM_SMPS_3, 1300); // Garantee we are in legacy mode.
//  pm_vreg_smps_config (PM_SMPS_3, PM_VREG_SMPS_MODE__PWM);
//
//  pm_vreg_pin_ctrl (PMAPP_CLOCK_D0_D1_A0_A1_ID, PM_OFF_CMD, PM_SMPS_3);
//  pm_vreg_pin_ctrl (PMAPP_CLOCK_A0_ID, PM_ON_CMD, PM_SMPS_3);
//  pm_vreg_ctrl (PM_OFF_CMD, PM_SMPS_3);
//
//  pm_vreg_pin_ctrl (PMAPP_CLOCK_D0_D1_A0_A1_ID, PM_OFF_CMD, PM_LDO_27);
//  pm_vreg_pin_ctrl (PMAPP_CLOCK_A0_ID, PM_ON_CMD, PM_LDO_27);
//  pm_vreg_ctrl (PM_OFF_CMD, PM_LDO_27);
//
//  pm_vreg_pin_ctrl (PMAPP_CLOCK_D0_D1_A0_A1_ID, PM_OFF_CMD, PM_SMPS_3);
//  pm_vreg_pin_ctrl (PMAPP_CLOCK_D0_D1_A0_A1_ID, PM_OFF_CMD, PM_LDO_27);
//
//
//  for (i = 0; i < sizeof (pm_rpm_tests_pinctl_ldo_b)/sizeof (pm_rpm_tests_pinctl_ldo_b[0]); i++)
//  {
//    rpm_req.Resource = pm_rpm_tests_pinctl_ldo_b[i].rpm_rsrc;
//    rpm_req.Master = pm_rpm_tests_pinctl_ldo_b[i].master;
//    rpm_req.DataLen = 4;
//    rpm_req.Data = &pm_rpm_tests_pinctl_ldo_b[i].payload;
//
//    pm_rpm_device_rsrc_set_vreg_cb_offtarget (&pm_npa_rpm_ldo_cfg_ctx_data_8901, 0,
//                                              &rpm_req, sizeof(DAL_rpmfw_RequestType));
//    busywait(100000);
//  }
//
//  for (i = 0; i < sizeof (pm_rpm_tests_pinctl_smps_a)/sizeof (pm_rpm_tests_pinctl_smps_a[0]); i++)
//  {
//    rpm_req.Resource = pm_rpm_tests_pinctl_smps_a[i].rpm_rsrc;
//    rpm_req.Master = pm_rpm_tests_pinctl_smps_a[i].master;
//    rpm_req.DataLen = 8;
//    rpm_req.Data = &pm_rpm_tests_pinctl_smps_a[i].payload;
//
//    pm_rpm_device_rsrc_set_vreg_cb_offtarget (&pm_npa_rpm_smps_cfg_ctx_data_8058, 0,
//                                              &rpm_req, sizeof(DAL_rpmfw_RequestType));
//    busywait(100000);
//  }
//}
//#endif
///*===========================================================================
//
//FUNCTION pm_rpm_proc_npa_test
//
//DESCRIPTION
//    This function tests the RPM NPA for PMIC.
//
//    It does the following:
//    * It initializes the NPA PMIC Node and resources.
//
//INPUT PARAMETERS
//  None.
//
//RETURN VALUE
//  None.
//
//DEPENDENCIES
//  NPA Framework should be available in the build being compiled for
//
//SIDE EFFECTS
//  NONE.
//
//===========================================================================*/
//void
//pm_rpm_proc_npa_test(void)
//{
//#ifdef PM_RPM_NPA_TEST_PINCTL
//  pm_rpm_proc_npa_test_pin_ctl ();
//#endif
//
//#ifdef PM_RPM_NPA_TEST_VDD_MEM_DIG
//  pm_rpm_proc_npa_test_vdd_mem_dig();
//#endif
//
//#ifdef PM_RPM_NPA_TEST_SMPS_A
//  pm_rpm_proc_npa_test_smps_a();
//#endif
//
//#ifdef PM_RPM_NPA_TEST_LDO_A
//  pm_rpm_proc_npa_test_ldo_a();
//#endif
//
//#ifdef PM_RPM_NPA_TEST_VS_A
// pm_rpm_proc_npa_test_vs_a();
//#endif
//
//#ifdef PM_RPM_NPA_TEST_SMPS_B
//  pm_rpm_proc_npa_test_smps_b();
//#endif
//
//#ifdef PM_RPM_NPA_TEST_LDO_B
//  pm_rpm_proc_npa_test_ldo_b();
//#endif
//
//#ifdef PM_RPM_NPA_TEST_VS_B
//  pm_rpm_proc_npa_test_vs_b();
//#endif
//
//  //pm_rpm_proc_npa_test_scor_pwr_rail ();
//#ifdef PM_RPM_NPA_TEST_CXO
//  pm_rpm_proc_npa_test_cxo ();
//#endif
//} /* end of pm_rpm_proc_npa_test() */
////#endif
//
