#include "DALPropDef.h"
#include "DALPropDef.h"
#include "DALPropDef.h"
#include "DALDeviceId.h"
#include "DALDeviceId.h"
#include "DALDeviceId.h"
#include "DALDeviceId.h"
#include "DALDeviceId.h"
#include "DALDeviceId.h"
#include "DALDeviceId.h"
#include "DALDeviceId.h"
#include "DALDeviceId.h"
#include "DALDeviceId.h"
#include "DALDeviceId.h"
#include "DALDeviceId.h"
#include "DALDeviceId.h"
#include "dalconfig.h"
#include "dalconfig.h"
#include "dalconfig.h"
#include "dalconfig.h"
#include "dalconfig.h"
#include "dalconfig.h"
#include "pm_dal_prop_ids.h"
#include "DALStdDef.h" 
#include "DALSysTypes.h" 
#include "dalconfig.h" 

#ifndef DAL_CONFIG_IMAGE_RPM 
#define DAL_CONFIG_IMAGE_RPM 
#endif 
extern void * icb_rpm_system;
extern void * SourceConfig;
extern void * RPMClockConfig;
extern void * SystemNOCClockConfig;
extern void * PCNOClockConfig;
extern void * BIMCClockConfig;
extern void * BIMCGPUClockConfig;
extern void * APSSTCUASYNCClockConfig;
extern void * QDSSATClockConfig;
extern void * QDSSTraceClockConfig;
extern void * QDSSSTMClockConfig;
extern void * QDSSTSCTRDiv2ClockConfig;
extern void * RBCPRClockConfig;
extern void * SPMIAHBClockConfig;
extern void * SPMISERClockConfig;
extern void * QPICClockConfig;
extern void * ClockLogDefaultConfig;
extern void * ClockVregRailMapConfig;
extern void * clk_reg;
extern void * smps_reg;
extern void * ldo_reg;
extern void * vs_reg;
extern void * boost_reg;
extern void * boost_byp_reg;
extern void * fts_volt;
extern void * fts2p5_volt;
extern void * hfs_volt;
extern void * boost_volt;
extern void * boost_byp_volt;
extern void * nmos_volt;
extern void * n600_volt;
extern void * n1200_volt;
extern void * pmos_volt;
extern void * ln_volt;
extern void * fts_range;
extern void * fts2p5_range;
extern void * hfs_range;
extern void * boost_range;
extern void * boost_byp_range;
extern void * nmos_range;
extern void * n600_range;
extern void * n1200_range;
extern void * pmos_range;
extern void * ln_range;
extern void * ult_nmos_volt;
extern void * ult_pmos_volt;
extern void * ult_nmos_range;
extern void * ult_pmos_range;
extern void * ult_buck_range_1;
extern void * ult_buck_volt_1;
extern void * ult_buck_range_2;
extern void * ult_buck_volt_2;
extern void * ult_ldo_settling_time;
extern void * ult_smps_settling_time;
extern void * ldo_settling_time;
extern void * smps_settling_time;
extern void * vs_settling_time;
extern void * boost_settling_time;
extern void * clk_sleep_reg;
extern void * clk_xo_reg;
extern void * clk_common;
extern void * num_of_smps;
extern void * num_of_ldo;
extern void * ldo_rail;
extern void * smps_rail;
extern void * clk_info;
extern void * smps_dep;
extern void * ldo_dep;
extern void * clk_dep;
extern void * pm_mpm_active_cfg;
extern void * pm_mpm_sleep_cfg;
extern void * pm_mpm_cmd_index;
extern void * pm_mpm_rails_info;
extern void * sleep_enter_info;
extern void * sleep_exit_info;

const void * DALPROP_StructPtrs[76] =  {
	&icb_rpm_system,
	&SourceConfig,
	&RPMClockConfig,
	&SystemNOCClockConfig,
	&PCNOClockConfig,
	&BIMCClockConfig,
	&BIMCGPUClockConfig,
	&APSSTCUASYNCClockConfig,
	&QDSSATClockConfig,
	&QDSSTraceClockConfig,
	&QDSSSTMClockConfig,
	&QDSSTSCTRDiv2ClockConfig,
	&RBCPRClockConfig,
	&SPMIAHBClockConfig,
	&SPMISERClockConfig,
	&QPICClockConfig,
	&ClockLogDefaultConfig,
	&ClockVregRailMapConfig,
	&clk_reg,
	&smps_reg,
	&ldo_reg,
	&vs_reg,
	&boost_reg,
	&boost_byp_reg,
	&fts_volt,
	&fts2p5_volt,
	&hfs_volt,
	&boost_volt,
	&boost_byp_volt,
	&nmos_volt,
	&n600_volt,
	&n1200_volt,
	&pmos_volt,
	&ln_volt,
	&fts_range,
	&fts2p5_range,
	&hfs_range,
	&boost_range,
	&boost_byp_range,
	&nmos_range,
	&n600_range,
	&n1200_range,
	&pmos_range,
	&ln_range,
	&ult_nmos_volt,
	&ult_pmos_volt,
	&ult_nmos_range,
	&ult_pmos_range,
	&ult_buck_range_1,
	&ult_buck_volt_1,
	&ult_buck_range_2,
	&ult_buck_volt_2,
	&ult_ldo_settling_time,
	&ult_smps_settling_time,
	&ldo_settling_time,
	&smps_settling_time,
	&vs_settling_time,
	&boost_settling_time,
	&clk_sleep_reg,
	&clk_xo_reg,
	&clk_common,
	&num_of_smps,
	&num_of_ldo,
	&ldo_rail,
	&smps_rail,
	&clk_info,
	&smps_dep,
	&ldo_dep,
	&clk_dep,
	&pm_mpm_active_cfg,
	&pm_mpm_sleep_cfg,
	&pm_mpm_cmd_index,
	&pm_mpm_rails_info,
	&sleep_enter_info,
	&sleep_exit_info,
	 NULL }; 

const uint32 DALPROP_PropBin[] = {

			0x000005b4, 0x00000028, 0x000001bc, 0x000001bc, 0x000001bc, 
			0x00000002, 0x02000145, 0x00000208, 0x0200009b, 0x0000058c, 
			0x74737973, 0x70006d65, 0x5f63696d, 0x5f627261, 0x65736162, 
			0x6464615f, 0x776f0072, 0x0072656e, 0x65746e69, 0x70757272, 
			0x6d730074, 0x6e695f64, 0x655f7274, 0x6c62616e, 0x43006465, 
			0x6b636f6c, 0x72756f53, 0x00736563, 0x5f636367, 0x5f6d7072, 
			0x636f7270, 0x6c63665f, 0x6367006b, 0x79735f63, 0x6f6e5f73, 
			0x78615f63, 0x6c635f69, 0x6367006b, 0x63705f63, 0x5f636f6e, 
			0x5f626861, 0x006b6c63, 0x5f636367, 0x636d6962, 0x6b6c635f, 
			0x63636700, 0x6d69625f, 0x70675f63, 0x6c635f75, 0x6367006b, 
			0x70615f63, 0x745f7373, 0x615f7563, 0x636e7973, 0x6b6c635f, 
			0x63636700, 0x7364715f, 0x74615f73, 0x6b6c635f, 0x63636700, 
			0x7364715f, 0x72745f73, 0x63656361, 0x6e696b6c, 0x6b6c635f, 
			0x63636700, 0x7364715f, 0x74735f73, 0x6c635f6d, 0x6367006b, 
			0x64715f63, 0x745f7373, 0x72746373, 0x7669645f, 0x6c635f32, 
			0x6367006b, 0x62725f63, 0x5f727063, 0x006b6c63, 0x5f636367, 
			0x696d7073, 0x6268615f, 0x6b6c635f, 0x63636700, 0x6d70735f, 
			0x65735f69, 0x6c635f72, 0x6367006b, 0x70715f63, 0x635f6369, 
			0x43006b6c, 0x6b636f6c, 0x44676f4c, 0x75616665, 0x0073746c, 
			0x636f6c43, 0x6572566b, 0x69615267, 0x70614d6c, 0x46454400, 
			0x544c5541, 0x4552465f, 0x4e455551, 0x51005943, 0x454d4954, 
			0x43415f52, 0x5341425f, 0x54510045, 0x52454d49, 0x5341425f, 
			0x00000045, 0x00000012, 0x80000000, 0x00000000, 0xff00ff00, 
			0x00000002, 0x80000007, 0x02000000, 0x00000002, 0x8000001a, 
			0x00000004, 0x00000002, 0x80000020, 0x00000030, 0xff00ff00, 
			0x00000008, 0x8000002a, 0x00010106, 0x01000100, 0xff00ff00, 
			0x00000012, 0x8000003b, 0x00000001, 0x00000012, 0x80000048, 
			0x00000002, 0x00000012, 0x8000005a, 0x00000003, 0x00000012, 
			0x8000006e, 0x00000004, 0x00000012, 0x80000080, 0x00000005, 
			0x00000012, 0x8000008d, 0x00000006, 0x00000012, 0x8000009e, 
			0x00000007, 0x00000012, 0x800000b5, 0x00000008, 0x00000012, 
			0x800000c5, 0x00000009, 0x00000012, 0x800000dd, 0x0000000a, 
			0x00000012, 0x800000ee, 0x0000000b, 0x00000012, 0x80000106, 
			0x0000000c, 0x00000012, 0x80000114, 0x0000000d, 0x00000012, 
			0x80000125, 0x0000000e, 0x00000012, 0x80000136, 0x0000000f, 
			0x00000012, 0x80000143, 0x00000010, 0x00000012, 0x80000154, 
			0x00000011, 0xff00ff00, 0x00000012, 0x00000001, 0x00000012, 
			0x00000012, 0x00000002, 0x00000013, 0x00000012, 0x00000003, 
			0x00000014, 0x00000012, 0x00000004, 0x00000015, 0x00000012, 
			0x00000005, 0x00000016, 0x00000012, 0x00000029, 0x00000017, 
			0x00000012, 0x00000006, 0x00000018, 0x00000012, 0x0000001e, 
			0x00000019, 0x00000012, 0x00000007, 0x0000001a, 0x00000012, 
			0x00000008, 0x0000001b, 0x00000012, 0x0000002a, 0x0000001c, 
			0x00000012, 0x00000009, 0x0000001d, 0x00000012, 0x0000000a, 
			0x0000001e, 0x00000012, 0x0000000b, 0x0000001f, 0x00000012, 
			0x0000000c, 0x00000020, 0x00000012, 0x0000000d, 0x00000021, 
			0x00000012, 0x0000000e, 0x00000022, 0x00000012, 0x0000001f, 
			0x00000023, 0x00000012, 0x0000000f, 0x00000024, 0x00000012, 
			0x00000010, 0x00000025, 0x00000012, 0x00000028, 0x00000026, 
			0x00000012, 0x00000011, 0x00000027, 0x00000012, 0x00000012, 
			0x00000028, 0x00000012, 0x00000013, 0x00000029, 0x00000012, 
			0x00000014, 0x0000002a, 0x00000012, 0x00000015, 0x0000002b, 
			0x00000012, 0x00000027, 0x0000002c, 0x00000012, 0x00000025, 
			0x0000002d, 0x00000012, 0x00000026, 0x0000002e, 0x00000012, 
			0x00000024, 0x0000002f, 0x00000012, 0x00000020, 0x00000030, 
			0x00000012, 0x00000021, 0x00000031, 0x00000012, 0x00000022, 
			0x00000032, 0x00000012, 0x00000023, 0x00000033, 0x00000012, 
			0x0000002b, 0x00000034, 0x00000012, 0x0000002c, 0x00000035, 
			0x00000012, 0x00000016, 0x00000036, 0x00000012, 0x00000017, 
			0x00000037, 0x00000012, 0x00000018, 0x00000038, 0x00000012, 
			0x00000019, 0x00000039, 0x00000012, 0x0000001a, 0x0000003a, 
			0x00000012, 0x0000001b, 0x0000003b, 0x00000012, 0x0000001c, 
			0x0000003c, 0xff00ff00, 0x00000012, 0x00000065, 0x0000003d, 
			0x00000012, 0x00000067, 0x0000003e, 0x00000012, 0x00000086, 
			0x0000003f, 0x00000012, 0x00000087, 0x00000040, 0x00000012, 
			0x00000074, 0x00000041, 0x00000012, 0x0000006e, 0x00000042, 
			0x00000012, 0x0000006d, 0x00000043, 0x00000012, 0x00000071, 
			0x00000044, 0x00000012, 0x0000012d, 0x00000045, 0x00000012, 
			0x0000012e, 0x00000046, 0x00000012, 0x0000012f, 0x00000047, 
			0x00000012, 0x00000130, 0x00000048, 0x00000012, 0x00000075, 
			0x00000049, 0x00000012, 0x00000076, 0x0000004a, 0xff00ff00, 
			0x00000002, 0x80000165, 0x0124f800, 0x00000002, 0x80000177, 
			0x00082000, 0x00000002, 0x80000186, 0x00084000, 0xff00ff00 };



 const StringDevice driver_list[] = {
			{"/dev/icb/rpm",2545934574u, 444, NULL, 0, NULL },
			{"DALDEVICEID_SPMI_DEVICE",3290583706u, 460, NULL, 0, NULL },
			{"/core/mproc/smd",1450287328u, 500, NULL, 0, NULL },
			{"/rpm/pmic/common",2887753651u, 728, NULL, 0, NULL },
			{"/rpm/pmic/target",3536625265u, 1248, NULL, 0, NULL }
};


 const DALProps DALPROP_PropsInfo = {(const byte*)DALPROP_PropBin, DALPROP_StructPtrs , 5, driver_list};
