/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.  
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef RPM_H
#define RPM_H

typedef enum
{
  RPM_MASTER_APPS     = 0, //APPS
  RPM_MASTER_MODEM    = 1, //MPSS
  RPM_MASTER_WCONNECT = 2, //PRONTO
  RPM_MASTER_TZ       = 3, //TZ
  RPM_MASTER_QDSP     = 4, //LPASS
  RPM_MASTER_MAX_COUNT 
} rpm_master_client;

typedef enum
{
    RPM_ACTIVE_SET       = 0,
    RPM_SLEEP_SET        = 1,
    RPM_NEXT_ACTIVE_SET  = 2,
    //RPM_NEXT_SLEEP_SET, -- not yet supported

    RPM_NUM_SETS
} rpm_set_type;

typedef enum
{
    RPM_TEST_REQ               = 0x74736574, // 'test' in little endiant

    RPM_CLOCK_0_REQ            = 0x306b6c63, // 'clk0' in little endian; misc clocks [CXO, QDSS, dcvs.ena, apcs.corner, gpll0]
    RPM_CLOCK_1_REQ            = 0x316b6c63, // 'clk1' in little endian; bus clocks [pcnoc, snoc, sysmmnoc]
    RPM_CLOCK_2_REQ            = 0x326b6c63, // 'clk2' in little endian; memory clocks [bimc]
    RPM_CLOCK_QPIC_REQ         = 0x63697071, // 'qpic' in little endian; clock [qpic]
    RPM_CLOCK_IPA_REQ          = 0x617069,   // 'ipa' in little endian; clock [ipa]
    RPM_CLOCK_CE_REQ           = 0x6563,     // 'ce' in little endian; clock [ce]

    RPM_BUS_SLAVE_REQ          = 0x766c7362, // 'bslv' in little endian
    RPM_BUS_MASTER_REQ         = 0x73616d62, // 'bmas' in little endian
    RPM_BUS_SPDM_CLK_REQ       = 0x63707362, // 'bspc' in little endian
    RPM_BUS_MASTER_LATENCY_REQ = 0x74616c62, // 'blat' in little endian

    RPM_SMPS_A_REQ       = 0x61706D73, // 'smpa' in little endian
    RPM_SMPS_A_S1_SH_REQ = 0x68733173, // 's1sh' in little endian - S1 shadow
    RPM_LDO_A_REQ        = 0x616F646C, // 'ldoa'  in little endian
    RPM_NCP_A_REQ        = 0x6170636E, // 'ncpa'  in little endian
    RPM_VS_A_REQ         = 0x617376,   // 'vsa'   in little endian
    RPM_CLK_BUFFER_A_REQ = 0x616B6C63, // 'clka' in little endian
    RPM_BOOST_A_REQ      = 0x61747362, // 'bsta' in little endian
    RPM_BOOST_BYP_A_REQ  = 0x61796262, // 'bbya' in little endian

    RPM_SMPS_B_REQ       = 0x62706D73, // 'smpb' in little endian
    RPM_LDO_B_REQ        = 0x626F646C, // 'ldob'  in little endian
    RPM_NCP_B_REQ        = 0x6270636E, // 'ncpb'  in little endian
    RPM_VS_B_REQ         = 0x627376,   // 'vsb'   in little endian
    RPM_CLK_BUFFER_B_REQ = 0x626B6C63, // 'clkb' in little endian
    RPM_BOOST_B_REQ      = 0x62747362, // 'bstb' in little endian
    RPM_BOOST_BYP_B_REQ  = 0x62796262, // 'bbyb' in little endian

    RPM_SMPS_C_REQ       = 0x63706D73, // 'smpc in little endian
    RPM_LDO_C_REQ        = 0x636F646C, // 'ldoc'  in little endian
    RPM_NCP_C_REQ        = 0x6370636E, // 'ncpc'  in little endian
    RPM_VS_C_REQ         = 0x637376,   // 'vsc'   in little endian
    RPM_CLK_BUFFER_C_REQ = 0x636B6C63, // 'clkc' in little endian
    RPM_BOOST_C_REQ      = 0x63747362, // 'bstc' in little endian
    RPM_BOOST_BYP_C_REQ  = 0x63796262, // 'bbyc' in little endian

    RPM_SMPS_D_REQ       = 0x64706D73, // 'smpd in little endian
    RPM_LDO_D_REQ        = 0x646F646C, // 'ldod'  in little endian
    RPM_NCP_D_REQ        = 0x6470636E, // 'ncpd'  in little endian
    RPM_VS_D_REQ         = 0x647376,   // 'vsd'   in little endian
    RPM_CLK_BUFFER_D_REQ = 0x646B6C63, // 'clkd' in little endian
    RPM_BOOST_D_REQ      = 0x64747362, // 'bstd' in little endian
    RPM_BOOST_BYP_D_REQ  = 0x64796262, // 'bbyd' in little endian

    RPM_SMPS_E_REQ       = 0x65706D73, // 'smpe in little endian
    RPM_LDO_E_REQ        = 0x656F646C, // 'ldoe'  in little endian
    RPM_NCP_E_REQ        = 0x6570636E, // 'ncpe'  in little endian
    RPM_VS_E_REQ         = 0x657376,   // 'vse'   in little endian
    RPM_CLK_BUFFER_E_REQ = 0x656B6C63, // 'clke' in little endian
    RPM_BOOST_E_REQ      = 0x65747362, // 'bste' in little endian
    RPM_BOOST_BYP_E_REQ  = 0x65796262, // 'bbye' in little endian

    RPM_SMPS_F_REQ       = 0x66706D73, // 'smpf in little endian
    RPM_LDO_F_REQ        = 0x666F646C, // 'ldof'  in little endian
    RPM_NCP_F_REQ        = 0x6670636E, // 'ncpf'  in little endian
    RPM_VS_F_REQ         = 0x667376,   // 'vsf'   in little endian
    RPM_CLK_BUFFER_F_REQ = 0x666B6C63, // 'clkf' in little endian
    RPM_BOOST_F_REQ      = 0x66747362, // 'bstf' in little endian
    RPM_BOOST_BYP_F_REQ  = 0x66796262, // 'bbyf' in little endian

    RPM_SMPS_G_REQ       = 0x67706D73, // 'smpg in little endian
    RPM_LDO_G_REQ        = 0x676F646C, // 'ldog'  in little endian
    RPM_NCP_G_REQ        = 0x6770636E, // 'ncpg'  in little endian
    RPM_VS_G_REQ         = 0x677376,   // 'vsg'   in little endian
    RPM_CLK_BUFFER_G_REQ = 0x676B6C63, // 'clkg' in little endian
    RPM_BOOST_G_REQ      = 0x67747362, // 'bstg' in little endian
    RPM_BOOST_BYP_G_REQ  = 0x67796262, // 'bbyg' in little endian
    
    RPM_SWEVENT_REQ            = 0x76657773, // 'swev' in little endian

    RPM_OCMEM_POWER_REQ        = 0x706d636f, // ‘ocmp’ in little endian
    
    RPM_RBCPR_REQ              = 0x727063,   // 'cpr'in little endian
    
    RPM_GPIO_TOGGLE_REQ        = 0x6F697067, // 'gpio' in little endian:[gpio0,gpio1,gpio2]
	RPM_SUBSYS_STATE_REQ       = 0x73737973, // 'syss' in little endian

} rpm_resource_type;

typedef enum
{
    RPM_REQUEST_SERVICE = 0x00716572, // 'req\0' in little endian
    RPM_SYSTEMDB_SERVICE = 0x00626473, // 'sdb\0' in little endian
    RPM_COMMAND_SERVICE = 0x00646d63, // 'cmd\0' in little endian
} rpm_service_type;

typedef enum
{
    RPM_TRANSITION_COMMAND = 0x736e7274, // 'trns' in little endian
} rpm_command_type;


#endif // RPM_H

