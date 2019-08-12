#line 1 "Z:\\b\\rpm_proc\\core\\dal\\config\\build\\rpm\\rpm\\AAAAANAAR\\DevCfg_master.xml"
#line 1 "Z:\\b\\rpm_proc\\core\\api\\dal\\DALPropDef.h"









#line 17 "Z:\\b\\rpm_proc\\core\\api\\dal\\DALPropDef.h"




























 
 
 























 

 









#line 91 "Z:\\b\\rpm_proc\\core\\api\\dal\\DALPropDef.h"

#line 2 "Z:\\b\\rpm_proc\\core\\dal\\config\\build\\rpm\\rpm\\AAAAANAAR\\DevCfg_master.xml"
#line 1 "Z:\\b\\rpm_proc\\core\\api\\dal\\DALDeviceId.h"

































#line 48 "Z:\\b\\rpm_proc\\core\\api\\dal\\DALDeviceId.h"

#line 58 "Z:\\b\\rpm_proc\\core\\api\\dal\\DALDeviceId.h"








































































































#line 168 "Z:\\b\\rpm_proc\\core\\api\\dal\\DALDeviceId.h"


















#line 194 "Z:\\b\\rpm_proc\\core\\api\\dal\\DALDeviceId.h"











#line 211 "Z:\\b\\rpm_proc\\core\\api\\dal\\DALDeviceId.h"

#line 218 "Z:\\b\\rpm_proc\\core\\api\\dal\\DALDeviceId.h"

#line 225 "Z:\\b\\rpm_proc\\core\\api\\dal\\DALDeviceId.h"

 
#line 259 "Z:\\b\\rpm_proc\\core\\api\\dal\\DALDeviceId.h"







#line 288 "Z:\\b\\rpm_proc\\core\\api\\dal\\DALDeviceId.h"






#line 309 "Z:\\b\\rpm_proc\\core\\api\\dal\\DALDeviceId.h"

















#line 333 "Z:\\b\\rpm_proc\\core\\api\\dal\\DALDeviceId.h"



#line 343 "Z:\\b\\rpm_proc\\core\\api\\dal\\DALDeviceId.h"



































#line 3 "Z:\\b\\rpm_proc\\core\\dal\\config\\build\\rpm\\rpm\\AAAAANAAR\\DevCfg_master.xml"
#line 1 "Z:\\b\\rpm_proc\\core\\dal\\config\\8909\\dalconfig.h"










#line 20 "Z:\\b\\rpm_proc\\core\\dal\\config\\8909\\dalconfig.h"
   
   
#line 29 "Z:\\b\\rpm_proc\\core\\dal\\config\\8909\\dalconfig.h"

#line 31 "Z:\\b\\rpm_proc\\core\\dal\\config\\8909\\dalconfig.h"

#line 43 "Z:\\b\\rpm_proc\\core\\dal\\config\\8909\\dalconfig.h"

 
#line 61 "Z:\\b\\rpm_proc\\core\\dal\\config\\8909\\dalconfig.h"



#line 4 "Z:\\b\\rpm_proc\\core\\dal\\config\\build\\rpm\\rpm\\AAAAANAAR\\DevCfg_master.xml"

<?xml version="1.0"?>
<dal>
<module name="rpm">
<driver name="icb_rpm">

   <global_def>

   </global_def>

   <device id="/dev/icb/rpm">

     <props name="system" type=0x00000012>

        icb_rpm_system

     </props> 

   </device>   

</driver>


<!--

 * @brief: Configuration file for the SPMI driver

 * 

 * Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.

 * 

 * $DateTime:$

 * $Header:$

 * $Change:$

-->

<driver name="NULL">

    <device id="DALDEVICEID_SPMI_DEVICE">

<!-- System props -->

<props name="pmic_arb_base_addr" type=0x00000002>

    0x02000000

</props>

        <!-- User configurable props -->

        <props name="owner" type=0x00000002>

            4

        </props>

        <props name="interrupt" type=0x00000002>

            48

        </props>

    </device>

</driver>



<driver name="NULL">

  <global_def></global_def>

  <device id="/core/mproc/smd">

    <props name="smd_intr_enabled" type= 0x00000008>

      0x1, 

      0x1, 

      0x0, 

      0x0, 

      0x1, 

      0x0, 

      0x1, 

      end

    </props>

  </device>

</driver>


<!-- NOTE: DAL config has trouble parsing multi-line comment, so please put -->

<!--       proper comment tags on each line                                 -->

<!--                                                                        -->

<!-- Clock Driver Properties file                                           -->

<!-- Note we depend on the include path for the following directories to be -->

<!-- available to the DAL config compiler:                                  -->

<!-- "${INC_ROOT}/core/systemdrivers/hal/clk/inc",                          -->

<!-- "${INC_ROOT}/core/systemdrivers/clock/src",                            -->

<!-- "${INC_ROOT}/core/systemdrivers/clock/config"                          -->

<!-- "${INC_ROOT}/core/systemdrivers/clock/config/${CHIPSET}"               -->

<driver name="Clock">

  <device id=0x02000145>

<!-- Clock sources -->

<props name="ClockSources" type=0x00000012>

  SourceConfig

</props>

<!-- RPM Clocks -->

<props name="gcc_rpm_proc_fclk" type=0x00000012>

  RPMClockConfig

</props>

<!-- System NOC Clocks -->

<props name="gcc_sys_noc_axi_clk" type=0x00000012>

  SystemNOCClockConfig

</props>

<!-- Peripheral Config NOC Clocks -->

<props name="gcc_pcnoc_ahb_clk" type=0x00000012>

  PCNOClockConfig

</props>

<!-- BIMC Clocks -->

<props name="gcc_bimc_clk" type=0x00000012>

  BIMCClockConfig

</props>

<!-- BIMC GPU Clock -->

<props name="gcc_bimc_gpu_clk" type=0x00000012>

  BIMCGPUClockConfig

</props>

<!-- APSS_TCU_ASYNC Clock -->

<props name="gcc_apss_tcu_async_clk" type=0x00000012>

  APSSTCUASYNCClockConfig

</props>

<!-- QDSS AT Clocks -->

<props name="gcc_qdss_at_clk" type=0x00000012>

  QDSSATClockConfig

</props>

<!-- QDSS Trace Clocks -->

<props name="gcc_qdss_traceclkin_clk" type=0x00000012>

  QDSSTraceClockConfig

</props>

<!-- QDSS STM Clocks -->

<props name="gcc_qdss_stm_clk" type=0x00000012>

  QDSSSTMClockConfig

</props>

<!-- QDSS TSCTR Div2 Clocks -->

<props name="gcc_qdss_tsctr_div2_clk" type=0x00000012>

  QDSSTSCTRDiv2ClockConfig

</props>

<!-- RBCPR Clocks -->

<props name="gcc_rbcpr_clk" type=0x00000012>

  RBCPRClockConfig

</props>

<!-- SPMI AHB Clocks -->

<props name="gcc_spmi_ahb_clk" type=0x00000012>

  SPMIAHBClockConfig

</props>

<!-- SPMI SER Clocks -->

<props name="gcc_spmi_ser_clk" type=0x00000012>

  SPMISERClockConfig

</props>

<!-- QPIC Clocks -->

<props name="gcc_qpic_clk" type=0x00000012>

  QPICClockConfig

</props>

<!-- Clock Log Defaults -->

<props name="ClockLogDefaults" type=0x00000012>

  ClockLogDefaultConfig

</props>

<!-- Clock Vreg level railway level mapping -->

<props name="ClockVregRailMap" type=0x00000012>

  ClockVregRailMapConfig

</props>

  </device>

</driver>


enum_header_path "pm_dal_prop_ids.h"
<driver name=NULL>

<device id="/rpm/pmic/common">

<props id=1 type=0x00000012>

clk_reg

</props>

<props id=2 type=0x00000012>

smps_reg 

</props>

<props id=3 type=0x00000012>

ldo_reg

</props>

<props id=4 type=0x00000012>

vs_reg 

</props>

<props id=5 type=0x00000012>

boost_reg 

</props>

<props id=41 type=0x00000012>

boost_byp_reg 

</props>

<props id=6 type=0x00000012>

fts_volt 

</props>

<props id=30 type=0x00000012>

fts2p5_volt 

</props>

<props id=7 type=0x00000012>

hfs_volt 

</props>

<props id=8 type=0x00000012>

boost_volt

</props>

<props id=42 type=0x00000012>

boost_byp_volt

</props>

<props id=9 type=0x00000012>

nmos_volt

</props>

<props id=10 type=0x00000012>

n600_volt

</props>

<props id=11 type=0x00000012>

n1200_volt 

</props>

<props id=12 type=0x00000012>

pmos_volt 

</props>

<props id=13 type=0x00000012>

ln_volt 

</props>

<props id=14 type=0x00000012>

fts_range 

</props>

<props id=31 type=0x00000012>

fts2p5_range 

</props>

<props id=15 type=0x00000012>

hfs_range 

</props>

<props id=16 type=0x00000012>

boost_range 

</props>

<props id=40 type=0x00000012>

boost_byp_range 

</props>

<props id=17 type=0x00000012>

nmos_range 

</props>

<props id=18 type=0x00000012>

n600_range 

</props>

<props id=19 type=0x00000012>

n1200_range 

</props>

<props id=20 type=0x00000012>

pmos_range 

</props>

<props id=21 type=0x00000012>

ln_range 

</props>

<props id=39 type=0x00000012>

ult_nmos_volt

</props>

<props id=37 type=0x00000012>

ult_pmos_volt

</props>

<props id=38 type=0x00000012>

ult_nmos_range

</props>

<props id=36 type=0x00000012>

ult_pmos_range

</props>

<props id=32 type=0x00000012>

ult_buck_range_1 

</props>

<props id=33 type=0x00000012>

ult_buck_volt_1 

</props>

<props id=34 type=0x00000012>

ult_buck_range_2 

</props>

<props id=35 type=0x00000012>

ult_buck_volt_2 

</props>

<props id=43 type=0x00000012>

ult_ldo_settling_time 

</props> 

<props id=44 type=0x00000012>

ult_smps_settling_time 

</props>

<props id=22 type=0x00000012>

ldo_settling_time 

</props> 

<props id=23 type=0x00000012>

smps_settling_time 

</props> 

<props id=24 type=0x00000012>

vs_settling_time 

</props> 

<props id=25 type=0x00000012>

boost_settling_time

</props>

<props id=26 type=0x00000012>

clk_sleep_reg 

</props>

<props id=27 type=0x00000012>

clk_xo_reg 

</props> 

<props id=28 type=0x00000012>

clk_common

</props>

</device> 

</driver>


<driver name=NULL>

<device id="/rpm/pmic/target">

<props id=101 type=0x00000012 >

num_of_smps 

</props>

<props id=103 type=0x00000012 >

num_of_ldo 

</props>

<props id=134 type=0x00000012>

ldo_rail

</props>

<props id=135 type=0x00000012>

smps_rail

</props>

<props id=116 type=0x00000012>

clk_info

</props> 

<props id=110 type=0x00000012>

smps_dep

</props>

<props id=109 type=0x00000012>

ldo_dep

</props>

<props id=113 type=0x00000012>

clk_dep

</props>

<props id=301 type=0x00000012>

pm_mpm_active_cfg

</props>

<props id=302 type=0x00000012>

pm_mpm_sleep_cfg

</props>

<props id=303 type=0x00000012>

pm_mpm_cmd_index

</props>

<props id=304 type=0x00000012>

pm_mpm_rails_info

</props>

<props id=117 type=0x00000012>

sleep_enter_info

</props>

<props id=118 type=0x00000012>

sleep_exit_info

</props>

</device>

</driver>


<driver name="Timetick">

  <device id=0x0200009B>

     <props name="DEFAULT_FREQUENCY" type=0x00000002>

       19200000

     </props>

     <props name="QTIMER_AC_BASE" type=0x00000002>

       0x00082000

     </props>

     <props name="QTIMER_BASE" type=0x00000002>

       0x00084000

     </props>

  </device>

</driver>


</module>
</dal>
