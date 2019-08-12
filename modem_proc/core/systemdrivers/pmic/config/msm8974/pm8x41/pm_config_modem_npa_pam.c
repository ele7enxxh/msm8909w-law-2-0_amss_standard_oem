/*! \file  pm_config_modem_npa_pam.c
 *  
 *  \brief  This file contains PAM information for NPA
 *  \details This file contains PAM information for NPA for all NPA Clients and Modes
 *  
 *    PMIC code generation Version: 1.0.0.0
 *    PMIC code generation NPA Client Version: MSM8974 branch 1.0 ver 06_04_2013 - Approved
 *    PMIC code generation NPA Device Setting Value Version: MSM8974 branch 1.0 ver 06_11_2013 - Approved
 *    This file contains code for Target specific settings and modes.
 *  
 *  &copy; Copyright 2013 Qualcomm Technologies, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/config/msm8974/pm8x41/pm_config_modem_npa_pam.c#1 $ 

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

/* =======================================================================
 For more information regarding these settings please see document: 
Qualcomm PMIC NPA (QPN) Node
System Software Specification
80-VN657-xxx
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when        who      what, where, why
--------    ---      ----------------------------------------------------------
6/11/2013	NA\henryw	MSM8974 branch 1.0 ver 06_04_2013 - Approved 
Comments: 1.Version Notes:
No PMIC System data change

- move test client to test processor. 
Note test client is not used on target. It is used for testing only

- add "NOT_GENERATED" for PMIC_NPA_MODE_ID_RF_MODE_10 to MODE_39
- add new client Vdd Mem ( copy of mx rail )

2.Verification(How/Who will test):
Verified through diffing
	

5/28/2013	NA\crosolow	MSM8974 branch 1.0 ver 05_09_2013 - Approved 
Comments: 1.Version Notes:
CR#: 491994 - Add quiet mode support for LTE
	
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

5/8/2013	NA\crosolow	MSM8974 branch 1.0 ver 05_07_2013 - Approved 
Comments: 1.Version Notes:
CR#: 484941 - Increase HV LDO headroom to help with LTE SNR issues in some bands
This affects RF1_TECH_LTE modes.
	
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

5/7/2013	NA\henryw	MSM8974 branch 1.0 ver 05_03_2013 - Approved 
Comments: 1.Version Notes:
Fix 1. Corner mode - Corner mode should only be generated for clk regime
Feature 1. Added support for the ability to remove corner mode for individual clients.
Feature 2. Add active floor support

SYSTEM SETTINGS SHOULD BE EXACTLY THE SAME AS APRIL 25TH VERSION
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
Verified through diffs

4/25/2013	NA\crosolow	MSM8974 branch 1.0 ver 04_25_2013 - Approved 
Comments: 1.Version Notes:
Clean up release:
CR#: 480164 - Remove 5V boost from USB PAM clients
CR#: 480161 - Remove Unused RPM clients
PMIC_NPA_CLIENT_NODE_CLK_REGIME_DIG

PMIC_NPA_CLIENT_NODE_HF_PLL1

PMIC_NPA_CLIENT_NODE_PLL_HF

PMIC_NPA_CLIENT_NODE_PLL_SR2

PMIC_NPA_CLIENT_NODE_RAIL_CX

PMIC_NPA_CLIENT_NODE_RAIL_MX
	
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

4/17/2013	NA\crosolow	MSM8974 branch 1.0 ver 04_17_2013 - Approved 
Comments: 1.Version Notes:
Increase headroom for 933 MHz LPDDR to 75 mV
CR#: 476591 - Increase LDO1 headroom to support 933 MHz LPDDR max current
	
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

4/16/2013	NA\crosolow	MSM8974 branch 1.0 ver 04_16_2013 - NOT Approved 1.Version Notes:
	
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

Comments: 1.Version Notes:
	
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

4/16/2013	NA\crosolow	MSM8974 branch 1.0 ver 03_21_2013 - Approved 
Comments: 1.Version Notes:
CR#: 474843 - Add thermal client to PAM table
CR#: 474870 - Add LPDDR client to PAM
CR#: 474867 - Change HV RF LDO set point in PAM.
CR#: 474877 - Add USB clients to PAM
	
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

3/20/2013	NA\crosolow	MSM8974 branch 1.0 ver 03_20_2013 - Approved 
Comments: 1.Version Notes:
New WCN clients
Removed MX from RAIL_CX and RAIL_GFX
Updated RPM_INIT
	
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

3/19/2013	NA\crosolow	MSM8974 branch 1.0 ver 03_19_2013 - NOT Approved 1.Version Notes:
	
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

Comments: 1.Version Notes:
	
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

3/12/2013	NA\crosolow	MSM8974 branch 1.0 ver 02_28_2013 - Approved 
Comments: 1.Version Notes:
Updated MSS client to not use corners, and deleted MX vote
Changed vote in RPM_INIT from NPM to Auto for MSS
	
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

2/28/2013	NA\crosolow	MSM8974 branch 1.0 ver 02_06_2013 - Approved 
Comments: 1.Version Notes:
Added WCN2 client w/ RFCLK3 for WCN3620
	
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

2/5/2013	NA\crosolow	MSM8974 branch 1.0 ver 02_05_2013 - Approved 
Comments: 1.Version Notes:
based on 01_13_2013
Adds new standby mode for WLAN where LDOs are left on in LPM.
	
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

1/31/2013	NA\crosolow	MSM8974 branch 1.0 ver 01_31_2013 - Approved 
Comments: 1.Version Notes:
Based off of 01_24_2013
removed RPM client from core rail clients
Add QFPROM client
	
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

1/25/2013	NA\crosolow	MSM8974 branch 1.0 ver 01_24_2013 - Approved 
Comments: 1.Version Notes:
Adds new sensor clients.  Independent control of I/O and Ana (VDD) rails.
	
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

1/21/2013	NA\crosolow	MSM8974 branch 1.0 ver 01_21_2013 - NOT Approved 1.Version Notes:
	
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

Comments: 1.Version Notes:
	
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

1/21/2013	NA\crosolow	MSM8974 branch 1.0 ver 01_18_2013 - Approved 
Comments: 1.Version Notes:
Created 3 new sensor clients
	
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

1/17/2013	NA\crosolow	MSM8974 branch 1.0 ver 12_13_2012 - Approved 
Comments: 1.Version Notes:
Adding new generic RF client.
	
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

12/13/2012	NA\crosolow	MSM8974 branch 1.0 ver 12_11_2012b - Approved 
Comments: 1.Version Notes:
Sensors was incorrectly swapped back to LDO17.  Fix.  Again.
	
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

12/11/2012	NA\crosolow	MSM8974 branch 1.0 ver 12_11_2012 - Approved 
Comments: 1.Version Notes:
Enable LDO15 for GPS *Temporary Workaround*
	
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

12/10/2012	NA\crosolow	MSM8974 branch 1.0 ver 12_10_2012 (DON'T USE) - NOT Approved 1.Version Notes:
	
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

Comments: 1.Version Notes:
	
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

12/10/2012	NA\crosolow	MSM8974 branch 1.0 ver 12_10_2012 (DON'T USE) - NOT Approved 1.Version Notes:
	
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

Comments: 1.Version Notes:
	
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

12/10/2012	NA\crosolow	MSM8974 branch 1.0 ver 12_05_2012 - Approved 
Comments: 1.Version Notes:
Changed WLAN PA from 2.9V to 3.3V
For GFX client.  Force MX to NPM when in nominal or above.
Set WLAN PA supply to 3.3V http://prism/CR/419780
	
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

12/5/2012	NA\crosolow	MSM8974 branch 1.0 ver 11_20_2012 - Approved 
Comments: 1.Version Notes:
Updated Generic Clients... Includes LOW_MINUS
	
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

11/19/2012	NA\crosolow	MSM8974 branch 1.0 ver 11_19_2012 - Approved 
Comments: 1.Version Notes:
Removed MX and CX from RPM_INIT client
	
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

11/19/2012	NA\crosolow	MSM8974 branch 1.0 ver 11_05_2012 - Approved 
Comments: 1.Version Notes:
Corrected bug in sensors client:  LDO18 changed to LDO17
	
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

11/5/2012	NA\crosolow	MSM8974 branch 1.0 ver 10_30_2012 - Approved 
Comments: 1.Version Notes:
Corrected Sensor LDO from LDO17 to LDO18
	
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

10/25/2012	NA\crosolow	MSM8974 branch 1.0 ver 10_12_2012 - Approved 
Comments: 1.Version Notes:
Updated with generic clients
Added new PLLs clients	

2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

10/12/2012	NA\crosolow	MSM8974 branch 1.0 ver 10_01_2012 - Approved 
Comments: 1.Version Notes:
Corrected Several bugs in the Audio Client (wrong voltage for S3A, boost)
	
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

10/1/2012	NA\crosolow	MSM8974 branch 1.0 ver 8_28a_2012 - Approved 
Comments: 1.Version Notes:
Added missing processors to generic clients
	
2.Applicability:
Needed specifically for QNX
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

9/28/2012	NA\crosolow	MSM8974 branch 1.0 ver 8_28_2012 - NOT Approved 1.Version Notes:
Adding Other Processors for Generic CX/MX clients
	
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

Comments: 1.Version Notes:
Adding Other Processors for Generic CX/MX clients
	
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

9/24/2012	NA\crosolow	MSM8974 branch 1.0 ver 8_21_2012 - Approved 
Comments: 1.Version Notes:
- Adding DIFF_CLK and SLEEP_CLK test client modes for off target testing.
- Added Generic clients (RAIL) for CX, GFX, MX.  Old clients are still included for backwards compatibility.
	
2.Applicability:
	
3.Release Priority(Urgent/Medium/Low/Not for release):
	
4.Verification(How/Who will test):
	

8/16/2012	NA\crosolow	MSM8974 branch 1.0 ver 7_16_2012 - Approved 
Comments: 1.Version Notes:
Changed Camera Processor from Modem to Apps
Changed MHL Processor from Modem to Apps
	
2.Applicability:
Camera, MHL
	
3.Release Priority(Urgent/Medium/Low/Not for release):
Medium
	
4.Verification(How/Who will test):
Camera, MHL	

7/16/2012	NA\agnerw	MSM8974 branch 1.0 ver 7_3_2012 - Approved 
Comments: Converting to Version 4.0 of PAM.
Introduced NPA Test Clients
7/3/2012	NA\agnerw	MSM8974 branch 1.0 ver 5_25_2012 - Approved 
Comments: removing parent resources from PAM (handled by dependencies)
5/16/2012	NA\crosolow	MSM8974 branch 1.0 ver 3_29_2012 - Approved 
Comments: Locking after updating UIM to 2 clients per UIM interface (ENABLE and ACTIVITY)
3/29/2012	NA\agnerw	MSM8974 branch 1.0 ver 3_29_2012 - FTS Replace - Approved 
Comments: 

3/29/2012	NA\agnerw	MSM8974 branch 1.0 ver 3_14_2012 - Approved 
Comments: 

3/14/2012	NA\agnerw	B-Family Settings 2_29_2012 - Approved 
Comments: Locking for 3/15/2012 Release.
- Note WCN and MCPM client resources are still 
pointing to PM8921 component.
2/29/2012	NA\agnerw	Depecated A-Family Settings 2_29_2012 - Approved 
Comments: 

3/27/2011	NA\agnerw	MSM8974 branch 1.0 ver 3_27_2012 - DO NOT USE - NOT Approved 
Comments: 

3/26/2011	NA\agnerw	MSM8974 branch 1.0 ver 3_26_2011 - DO NOT USE - NOT Approved 
Comments: 

3/29/2012	NA\agnerw	MSM8974 branch 1.0 ver 3_26_2012 - FTS Replace - Approved 
Comments: 

========================================================================== */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "pm_npa_device_clk_buff.h"
#include "pm_npa_device_ldo.h"
#include "pm_npa_device_smps.h"
#include "pm_npa_device_vs.h"
#include "pm_npa.h"
#include "pm_modem_proc_npa.h"
#include "pm_modem_proc_npa_device.h"
#include "DDIPlatformInfo.h"

/*===========================================================================

                VARIABLES DEFINITIONS

===========================================================================*/
/* ANT1 Client */
pm_npa_ldo_kvps
pm_pam_ant1_a_ldo16 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_ANT_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_ANT_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_ANT_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_ANT_ACTIVE
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2700000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 20,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_ANT_MODE_2
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
};

pm_npa_pam_client_cfg_type
pm_pam_ant1_rails_info []=
{
   {
      (void*)pm_pam_ant1_a_ldo16,
      PM_NPA_VREG_LDO
   },
};
/* CLK_REGIME_DIG Client */
pm_npa_ldo_kvps
pm_pam_clk_regime_dig_a_ldo12 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_CLK_REGIME_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_CLK_REGIME_DIG_NO_VOTE_DIG
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CLK_REGIME_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_CLK_REGIME_DIG_RETENTION_DIG
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CLK_REGIME_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_CLK_REGIME_DIG_LOW_DIG
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1800000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 100,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 200, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CLK_REGIME_MODE_3
    // Comments: 	PMIC_NPA_MODE_ID_CLK_REGIME_DIG_NORMAL_DIG
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1800000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 100,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 200, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CLK_REGIME_MODE_4
    // Comments: 	PMIC_NPA_MODE_ID_CLK_REGIME_DIG_HIGH_DIG
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1800000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 100,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 200, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CLK_REGIME_MODE_5
    // Comments: 	PMIC_NPA_MODE_ID_CLK_REGIME_DIG_SUPER_HIGH_DIG
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1800000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 100,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 200, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CLK_REGIME_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
};

pm_npa_smps_kvps
pm_pam_clk_regime_dig_b_smps1 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_CLK_REGIME_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_CLK_REGIME_DIG_NO_VOTE_DIG
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 670000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CLK_REGIME_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_CLK_REGIME_DIG_RETENTION_DIG
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 670000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CLK_REGIME_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_CLK_REGIME_DIG_LOW_DIG
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 950000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 100,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CLK_REGIME_MODE_3
    // Comments: 	PMIC_NPA_MODE_ID_CLK_REGIME_DIG_NORMAL_DIG
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 950000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 100,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CLK_REGIME_MODE_4
    // Comments: 	PMIC_NPA_MODE_ID_CLK_REGIME_DIG_HIGH_DIG
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1050000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 100,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CLK_REGIME_MODE_5
    // Comments: 	PMIC_NPA_MODE_ID_CLK_REGIME_DIG_SUPER_HIGH_DIG
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1050000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 100,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CLK_REGIME_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
};

pm_npa_smps_kvps
pm_pam_clk_regime_dig_b_smps2 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_CLK_REGIME_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_CLK_REGIME_DIG_NO_VOTE_DIG
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__1,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CLK_REGIME_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_CLK_REGIME_DIG_RETENTION_DIG
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__1,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CLK_REGIME_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_CLK_REGIME_DIG_LOW_DIG
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 100,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__3,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CLK_REGIME_MODE_3
    // Comments: 	PMIC_NPA_MODE_ID_CLK_REGIME_DIG_NORMAL_DIG
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 100,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__4,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CLK_REGIME_MODE_4
    // Comments: 	PMIC_NPA_MODE_ID_CLK_REGIME_DIG_HIGH_DIG
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 100,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__5,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CLK_REGIME_MODE_5
    // Comments: 	PMIC_NPA_MODE_ID_CLK_REGIME_DIG_SUPER_HIGH_DIG
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 100,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__6,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CLK_REGIME_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
};

pm_npa_pam_client_cfg_type
pm_pam_clk_regime_dig_rails_info []=
{
   {
      (void*)pm_pam_clk_regime_dig_a_ldo12,
      PM_NPA_VREG_LDO
   },
   {
      (void*)pm_pam_clk_regime_dig_b_smps1,
      PM_NPA_VREG_SMPS
   },
   {
      (void*)pm_pam_clk_regime_dig_b_smps2,
      PM_NPA_VREG_SMPS
   },
};
/* HFPLL1 Client */
pm_npa_ldo_kvps
pm_pam_hfpll1_a_ldo12 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_GENERIC_STANDBY
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GENERIC_ACTIVE
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1800000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 50,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 50, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
};

pm_npa_pam_client_cfg_type
pm_pam_hfpll1_rails_info []=
{
   {
      (void*)pm_pam_hfpll1_a_ldo12,
      PM_NPA_VREG_LDO
   },
};
/* MCPM_DIG Client */
pm_npa_smps_kvps
pm_pam_mcpm_dig_b_smps1 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_CORNER_LEVEL_NO_VOTE
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CORNER_LEVEL_1
    // Comments: 	Retention
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CORNER_LEVEL_2
    // Comments: 	SVS
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 950000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CORNER_LEVEL_3
    // Comments: 	Normal
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 950000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CORNER_LEVEL_4
    // Comments: 	Turbo
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1050000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CORNER_LEVEL_5
    // Comments: 	Super Turbo
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1050000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CORNER_LEVEL_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CORNER_LEVEL_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CORNER_LEVEL_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
};

pm_npa_smps_kvps
pm_pam_mcpm_dig_b_smps2 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_CORNER_LEVEL_NO_VOTE
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CORNER_LEVEL_1
    // Comments: 	Retention
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CORNER_LEVEL_2
    // Comments: 	SVS
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__3,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CORNER_LEVEL_3
    // Comments: 	Normal
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__4,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CORNER_LEVEL_4
    // Comments: 	Turbo
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__5,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CORNER_LEVEL_5
    // Comments: 	Super Turbo
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__6,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CORNER_LEVEL_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CORNER_LEVEL_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CORNER_LEVEL_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
};

pm_npa_pam_client_cfg_type
pm_pam_mcpm_dig_rails_info []=
{
   {
      (void*)pm_pam_mcpm_dig_b_smps1,
      PM_NPA_VREG_SMPS
   },
   {
      (void*)pm_pam_mcpm_dig_b_smps2,
      PM_NPA_VREG_SMPS
   },
};
/* MCPM_MEM Client */
pm_npa_smps_kvps
pm_pam_mcpm_mem_b_smps1 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_MCPM_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_MCPM_ NO_VOTE
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_MCPM_MODE_1
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_MCPM_MODE_2
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_MCPM_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_MCPM_MODE_4
    // Comments: 	PMIC_NPA_MODE_ID_MCPM_MEM_HIGH
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1050000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 100,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_MCPM_MODE_5
    // Comments: 	PMIC_NPA_MODE_ID_MCPM_MEM_SUPER_HIGH
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1050000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 100,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_MCPM_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_MCPM_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_MCPM_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
};

pm_npa_pam_client_cfg_type
pm_pam_mcpm_mem_rails_info []=
{
   {
      (void*)pm_pam_mcpm_mem_b_smps1,
      PM_NPA_VREG_SMPS
   },
};
/* PA1 Client */
pm_npa_clk_buff_kvps
pm_pam_pa1_a_clk_buff13 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_PA_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_PA_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_PA_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_PA_ACTIVE
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_PA_MODE_2
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
};

pm_npa_ldo_kvps
pm_pam_pa1_a_ldo16 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_PA_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_PA_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_PA_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_PA_ACTIVE
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2700000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 5,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_PA_MODE_2
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
};

pm_npa_pam_client_cfg_type
pm_pam_pa1_rails_info []=
{
   {
      (void*)pm_pam_pa1_a_clk_buff13,
      PM_NPA_CLK_BUFF
   },
   {
      (void*)pm_pam_pa1_a_ldo16,
      PM_NPA_VREG_LDO
   },
};
/* PLL_HF Client */
pm_npa_ldo_kvps
pm_pam_pll_hf_a_ldo12 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_GENERIC_STANDBY
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GENERIC_ACTIVE
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1800000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 50,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 50, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
};

pm_npa_pam_client_cfg_type
pm_pam_pll_hf_rails_info []=
{
   {
      (void*)pm_pam_pll_hf_a_ldo12,
      PM_NPA_VREG_LDO
   },
};
/* PLL_SR2 Client */
pm_npa_ldo_kvps
pm_pam_pll_sr2_a_ldo12 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_GENERIC_STANDBY
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GENERIC_ACTIVE
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1800000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 50,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 50, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
};

pm_npa_pam_client_cfg_type
pm_pam_pll_sr2_rails_info []=
{
   {
      (void*)pm_pam_pll_sr2_a_ldo12,
      PM_NPA_VREG_LDO
   },
};
/* QFPROM Client */
pm_npa_ldo_kvps
pm_pam_qfprom_a_ldo12 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_GENERIC_STANDBY
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GENERIC_ACTIVE
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1900000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
};

pm_npa_pam_client_cfg_type
pm_pam_qfprom_rails_info []=
{
   {
      (void*)pm_pam_qfprom_a_ldo12,
      PM_NPA_VREG_LDO
   },
};
/* RAIL_CX Client */
pm_npa_smps_kvps
pm_pam_rail_cx_b_smps2 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_CORE_RAIL_OFF
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CORE_RAIL_RETENTION
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__1,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CORE_RAIL_LOW_MINUS
    // Comments: 	Do not use
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__2,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CORE_RAIL_LOW
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__3,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CORE_RAIL_NOMINAL
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__4,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CORE_RAIL_NOMINAL_PLUS
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__5,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CORE_RAIL_TURBO
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__6,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
};

pm_npa_pam_client_cfg_type
pm_pam_rail_cx_rails_info []=
{
   {
      (void*)pm_pam_rail_cx_b_smps2,
      PM_NPA_VREG_SMPS
   },
};
/* RAIL_MSS Client */
pm_npa_smps_kvps
pm_pam_rail_mss_b_smps3 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_CORE_RAIL_OFF
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CORE_RAIL_RETENTION
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 500000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CORE_RAIL_LOW_MINUS
    // Comments: 	Do not use
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 700000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CORE_RAIL_LOW
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 812500,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CORE_RAIL_NOMINAL
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 900000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CORE_RAIL_NOMINAL_PLUS
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 987500,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CORE_RAIL_TURBO
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1050000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
};

pm_npa_pam_client_cfg_type
pm_pam_rail_mss_rails_info []=
{
   {
      (void*)pm_pam_rail_mss_b_smps3,
      PM_NPA_VREG_SMPS
   },
};
/* RAIL_MX Client */
pm_npa_smps_kvps
pm_pam_rail_mx_b_smps1 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_CORE_RAIL_OFF
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 670000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CORE_RAIL_RETENTION
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 670000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CORE_RAIL_LOW_MINUS
    // Comments: 	Do not use
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 950000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CORE_RAIL_LOW
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 950000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CORE_RAIL_NOMINAL
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 950000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CORE_RAIL_NOMINAL_PLUS
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__NPM,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1050000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_CORE_RAIL_TURBO
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__NPM,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1050000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
};

pm_npa_pam_client_cfg_type
pm_pam_rail_mx_rails_info []=
{
   {
      (void*)pm_pam_rail_mx_b_smps1,
      PM_NPA_VREG_SMPS
   },
};
/* RF1 Client */
pm_npa_clk_buff_kvps
pm_pam_rf1_a_clk_buff4 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	RF_SLEEP:    LV_WAN=OFF     ; HV_WAN=OFF   ; BBRx_LDO=OFF  ; RF_2P7_LDO=OFF ; RFCLK1=OFF
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	RF_MODE1: LV_WAN=1.225V; HV_WAN=2.05V; BBRx_LDO=1.8V  ; RF_2P7_LDO=2.7V; RFCLK1=ON
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	RF_MODE2: LV_WAN=1.300V; HV_WAN=2.05V; BBRx_LDO=1.8V  ; RF_2P7_LDO=2.7V; RFCLK1=ON
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
    // Comments: 	RF_MODE3: LV_WAN=1.225V; HV_WAN=2.10V; BBRx_LDO=1.8V  ; RF_2P7_LDO=2.7V; RFCLK1=ON
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
    // Comments: 	RF_MODE4: LV_WAN=1.300V; HV_WAN=2.10V; BBRx_LDO=1.8V  ; RF_2P7_LDO=2.7V; RFCLK1=ON
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
};

pm_npa_clk_buff_kvps
pm_pam_rf1_a_clk_buff5 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	RF_SLEEP:    LV_WAN=OFF     ; HV_WAN=OFF   ; BBRx_LDO=OFF  ; RF_2P7_LDO=OFF ; RFCLK1=OFF
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	RF_MODE1: LV_WAN=1.225V; HV_WAN=2.05V; BBRx_LDO=1.8V  ; RF_2P7_LDO=2.7V; RFCLK1=ON
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	RF_MODE2: LV_WAN=1.300V; HV_WAN=2.05V; BBRx_LDO=1.8V  ; RF_2P7_LDO=2.7V; RFCLK1=ON
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
    // Comments: 	RF_MODE3: LV_WAN=1.225V; HV_WAN=2.10V; BBRx_LDO=1.8V  ; RF_2P7_LDO=2.7V; RFCLK1=ON
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
    // Comments: 	RF_MODE4: LV_WAN=1.300V; HV_WAN=2.10V; BBRx_LDO=1.8V  ; RF_2P7_LDO=2.7V; RFCLK1=ON
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
};

pm_npa_ldo_kvps
pm_pam_rf1_a_ldo4 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	RF_SLEEP:    LV_WAN=OFF     ; HV_WAN=OFF   ; BBRx_LDO=OFF  ; RF_2P7_LDO=OFF ; RFCLK1=OFF
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	RF_MODE1: LV_WAN=1.225V; HV_WAN=2.05V; BBRx_LDO=1.8V  ; RF_2P7_LDO=2.7V; RFCLK1=ON
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1225000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	RF_MODE2: LV_WAN=1.300V; HV_WAN=2.05V; BBRx_LDO=1.8V  ; RF_2P7_LDO=2.7V; RFCLK1=ON
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1300000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
    // Comments: 	RF_MODE3: LV_WAN=1.225V; HV_WAN=2.10V; BBRx_LDO=1.8V  ; RF_2P7_LDO=2.7V; RFCLK1=ON
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1225000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
    // Comments: 	RF_MODE4: LV_WAN=1.300V; HV_WAN=2.10V; BBRx_LDO=1.8V  ; RF_2P7_LDO=2.7V; RFCLK1=ON
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1300000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
};

pm_npa_ldo_kvps
pm_pam_rf1_a_ldo14 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	RF_SLEEP:    LV_WAN=OFF     ; HV_WAN=OFF   ; BBRx_LDO=OFF  ; RF_2P7_LDO=OFF ; RFCLK1=OFF
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	RF_MODE1: LV_WAN=1.225V; HV_WAN=2.05V; BBRx_LDO=1.8V  ; RF_2P7_LDO=2.7V; RFCLK1=ON
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1800000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	RF_MODE2: LV_WAN=1.300V; HV_WAN=2.05V; BBRx_LDO=1.8V  ; RF_2P7_LDO=2.7V; RFCLK1=ON
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1800000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
    // Comments: 	RF_MODE3: LV_WAN=1.225V; HV_WAN=2.10V; BBRx_LDO=1.8V  ; RF_2P7_LDO=2.7V; RFCLK1=ON
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1800000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
    // Comments: 	RF_MODE4: LV_WAN=1.300V; HV_WAN=2.10V; BBRx_LDO=1.8V  ; RF_2P7_LDO=2.7V; RFCLK1=ON
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1800000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
};

pm_npa_ldo_kvps
pm_pam_rf1_a_ldo15 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	RF_SLEEP:    LV_WAN=OFF     ; HV_WAN=OFF   ; BBRx_LDO=OFF  ; RF_2P7_LDO=OFF ; RFCLK1=OFF
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	RF_MODE1: LV_WAN=1.225V; HV_WAN=2.05V; BBRx_LDO=1.8V  ; RF_2P7_LDO=2.7V; RFCLK1=ON
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2050000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	RF_MODE2: LV_WAN=1.300V; HV_WAN=2.05V; BBRx_LDO=1.8V  ; RF_2P7_LDO=2.7V; RFCLK1=ON
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2050000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
    // Comments: 	RF_MODE3: LV_WAN=1.225V; HV_WAN=2.10V; BBRx_LDO=1.8V  ; RF_2P7_LDO=2.7V; RFCLK1=ON
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2100000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
    // Comments: 	RF_MODE4: LV_WAN=1.300V; HV_WAN=2.10V; BBRx_LDO=1.8V  ; RF_2P7_LDO=2.7V; RFCLK1=ON
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2100000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
};

pm_npa_ldo_kvps
pm_pam_rf1_a_ldo16 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	RF_SLEEP:    LV_WAN=OFF     ; HV_WAN=OFF   ; BBRx_LDO=OFF  ; RF_2P7_LDO=OFF ; RFCLK1=OFF
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	RF_MODE1: LV_WAN=1.225V; HV_WAN=2.05V; BBRx_LDO=1.8V  ; RF_2P7_LDO=2.7V; RFCLK1=ON
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2700000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	RF_MODE2: LV_WAN=1.300V; HV_WAN=2.05V; BBRx_LDO=1.8V  ; RF_2P7_LDO=2.7V; RFCLK1=ON
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2700000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
    // Comments: 	RF_MODE3: LV_WAN=1.225V; HV_WAN=2.10V; BBRx_LDO=1.8V  ; RF_2P7_LDO=2.7V; RFCLK1=ON
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2700000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
    // Comments: 	RF_MODE4: LV_WAN=1.300V; HV_WAN=2.10V; BBRx_LDO=1.8V  ; RF_2P7_LDO=2.7V; RFCLK1=ON
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2700000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
   },
};

pm_npa_pam_client_cfg_type
pm_pam_rf1_rails_info []=
{
   {
      (void*)pm_pam_rf1_a_clk_buff4,
      PM_NPA_CLK_BUFF
   },
   {
      (void*)pm_pam_rf1_a_clk_buff5,
      PM_NPA_CLK_BUFF
   },
   {
      (void*)pm_pam_rf1_a_ldo4,
      PM_NPA_VREG_LDO
   },
   {
      (void*)pm_pam_rf1_a_ldo14,
      PM_NPA_VREG_LDO
   },
   {
      (void*)pm_pam_rf1_a_ldo15,
      PM_NPA_VREG_LDO
   },
   {
      (void*)pm_pam_rf1_a_ldo16,
      PM_NPA_VREG_LDO
   },
};
/* RF1_TECH_1X Client */
pm_npa_clk_buff_kvps
pm_pam_rf1_tech_1x_a_clk_buff4 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
};

pm_npa_clk_buff_kvps
pm_pam_rf1_tech_1x_a_clk_buff5 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
};

pm_npa_ldo_kvps
pm_pam_rf1_tech_1x_a_ldo4 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1225000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 260,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1300000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 260,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
};

pm_npa_ldo_kvps
pm_pam_rf1_tech_1x_a_ldo14 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1800000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 8,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1800000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 8,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
};

pm_npa_ldo_kvps
pm_pam_rf1_tech_1x_a_ldo15 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2050000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 105,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2050000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 105,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
};

pm_npa_ldo_kvps
pm_pam_rf1_tech_1x_a_ldo16 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2700000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 40,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2700000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 40,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
};

pm_npa_smps_kvps
pm_pam_rf1_tech_1x_a_smps1 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_CLK_1p6_MHz,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
};

pm_npa_smps_kvps
pm_pam_rf1_tech_1x_a_smps2 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
};

pm_npa_pam_client_cfg_type
pm_pam_rf1_tech_1x_rails_info []=
{
   {
      (void*)pm_pam_rf1_tech_1x_a_clk_buff4,
      PM_NPA_CLK_BUFF
   },
   {
      (void*)pm_pam_rf1_tech_1x_a_clk_buff5,
      PM_NPA_CLK_BUFF
   },
   {
      (void*)pm_pam_rf1_tech_1x_a_ldo4,
      PM_NPA_VREG_LDO
   },
   {
      (void*)pm_pam_rf1_tech_1x_a_ldo14,
      PM_NPA_VREG_LDO
   },
   {
      (void*)pm_pam_rf1_tech_1x_a_ldo15,
      PM_NPA_VREG_LDO
   },
   {
      (void*)pm_pam_rf1_tech_1x_a_ldo16,
      PM_NPA_VREG_LDO
   },
   {
      (void*)pm_pam_rf1_tech_1x_a_smps1,
      PM_NPA_VREG_SMPS
   },
   {
      (void*)pm_pam_rf1_tech_1x_a_smps2,
      PM_NPA_VREG_SMPS
   },
};
/* RF1_TECH_DO Client */
pm_npa_clk_buff_kvps
pm_pam_rf1_tech_do_a_clk_buff4 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
};

pm_npa_clk_buff_kvps
pm_pam_rf1_tech_do_a_clk_buff5 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
};

pm_npa_ldo_kvps
pm_pam_rf1_tech_do_a_ldo4 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1225000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 260,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1300000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 260,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
};

pm_npa_ldo_kvps
pm_pam_rf1_tech_do_a_ldo14 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1800000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 8,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1800000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 8,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
};

pm_npa_ldo_kvps
pm_pam_rf1_tech_do_a_ldo15 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2050000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 105,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2050000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 105,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
};

pm_npa_ldo_kvps
pm_pam_rf1_tech_do_a_ldo16 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2700000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 40,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2700000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 40,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
};

pm_npa_pam_client_cfg_type
pm_pam_rf1_tech_do_rails_info []=
{
   {
      (void*)pm_pam_rf1_tech_do_a_clk_buff4,
      PM_NPA_CLK_BUFF
   },
   {
      (void*)pm_pam_rf1_tech_do_a_clk_buff5,
      PM_NPA_CLK_BUFF
   },
   {
      (void*)pm_pam_rf1_tech_do_a_ldo4,
      PM_NPA_VREG_LDO
   },
   {
      (void*)pm_pam_rf1_tech_do_a_ldo14,
      PM_NPA_VREG_LDO
   },
   {
      (void*)pm_pam_rf1_tech_do_a_ldo15,
      PM_NPA_VREG_LDO
   },
   {
      (void*)pm_pam_rf1_tech_do_a_ldo16,
      PM_NPA_VREG_LDO
   },
};
/* RF1_TECH_GPS Client */
pm_npa_clk_buff_kvps
pm_pam_rf1_tech_gps_a_clk_buff4 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_GPS_SLEEP, PMIC_NPA_MODE_ID_GPS_STANDBY
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_GPS_DVS_LOW_RX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_GPS_DVS_HIGH_RX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_3
    // Comments: 	PMIC_NPA_MODE_ID_GPS_DVS_LOW_RX_CRIT_DEMOD, PMIC_NPA_MODE_ID_GPS_DVS_HIGH_RX_CRIT_DEMOD
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
};

pm_npa_clk_buff_kvps
pm_pam_rf1_tech_gps_a_clk_buff5 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_GPS_SLEEP, PMIC_NPA_MODE_ID_GPS_STANDBY
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_GPS_DVS_LOW_RX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_GPS_DVS_HIGH_RX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_3
    // Comments: 	PMIC_NPA_MODE_ID_GPS_DVS_LOW_RX_CRIT_DEMOD, PMIC_NPA_MODE_ID_GPS_DVS_HIGH_RX_CRIT_DEMOD
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
};

pm_npa_ldo_kvps
pm_pam_rf1_tech_gps_a_ldo4 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_GPS_SLEEP, PMIC_NPA_MODE_ID_GPS_STANDBY
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_GPS_DVS_LOW_RX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1225000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 30,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_GPS_DVS_HIGH_RX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1300000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 30,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_3
    // Comments: 	PMIC_NPA_MODE_ID_GPS_DVS_LOW_RX_CRIT_DEMOD, PMIC_NPA_MODE_ID_GPS_DVS_HIGH_RX_CRIT_DEMOD
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1300000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 30,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
};

pm_npa_ldo_kvps
pm_pam_rf1_tech_gps_a_ldo15 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_GPS_SLEEP, PMIC_NPA_MODE_ID_GPS_STANDBY
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_GPS_DVS_LOW_RX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2050000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_GPS_DVS_HIGH_RX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2050000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_3
    // Comments: 	PMIC_NPA_MODE_ID_GPS_DVS_LOW_RX_CRIT_DEMOD, PMIC_NPA_MODE_ID_GPS_DVS_HIGH_RX_CRIT_DEMOD
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2100000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 10,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
};

pm_npa_ldo_kvps
pm_pam_rf1_tech_gps_a_ldo16 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_GPS_SLEEP, PMIC_NPA_MODE_ID_GPS_STANDBY
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_GPS_DVS_LOW_RX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2700000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 5,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_GPS_DVS_HIGH_RX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2700000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 5,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_3
    // Comments: 	PMIC_NPA_MODE_ID_GPS_DVS_LOW_RX_CRIT_DEMOD, PMIC_NPA_MODE_ID_GPS_DVS_HIGH_RX_CRIT_DEMOD
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2700000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 5,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_GPS_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
};

pm_npa_pam_client_cfg_type
pm_pam_rf1_tech_gps_rails_info []=
{
   {
      (void*)pm_pam_rf1_tech_gps_a_clk_buff4,
      PM_NPA_CLK_BUFF
   },
   {
      (void*)pm_pam_rf1_tech_gps_a_clk_buff5,
      PM_NPA_CLK_BUFF
   },
   {
      (void*)pm_pam_rf1_tech_gps_a_ldo4,
      PM_NPA_VREG_LDO
   },
   {
      (void*)pm_pam_rf1_tech_gps_a_ldo15,
      PM_NPA_VREG_LDO
   },
   {
      (void*)pm_pam_rf1_tech_gps_a_ldo16,
      PM_NPA_VREG_LDO
   },
};
/* RF1_TECH_GSM Client */
pm_npa_clk_buff_kvps
pm_pam_rf1_tech_gsm_a_clk_buff4 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
};

pm_npa_clk_buff_kvps
pm_pam_rf1_tech_gsm_a_clk_buff5 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
};

pm_npa_ldo_kvps
pm_pam_rf1_tech_gsm_a_ldo4 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1225000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 260,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1300000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 260,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
};

pm_npa_ldo_kvps
pm_pam_rf1_tech_gsm_a_ldo14 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1800000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 8,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1800000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 8,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
};

pm_npa_ldo_kvps
pm_pam_rf1_tech_gsm_a_ldo15 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2050000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 105,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2050000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 105,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
};

pm_npa_ldo_kvps
pm_pam_rf1_tech_gsm_a_ldo16 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2700000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 40,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2700000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 40,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
};

pm_npa_pam_client_cfg_type
pm_pam_rf1_tech_gsm_rails_info []=
{
   {
      (void*)pm_pam_rf1_tech_gsm_a_clk_buff4,
      PM_NPA_CLK_BUFF
   },
   {
      (void*)pm_pam_rf1_tech_gsm_a_clk_buff5,
      PM_NPA_CLK_BUFF
   },
   {
      (void*)pm_pam_rf1_tech_gsm_a_ldo4,
      PM_NPA_VREG_LDO
   },
   {
      (void*)pm_pam_rf1_tech_gsm_a_ldo14,
      PM_NPA_VREG_LDO
   },
   {
      (void*)pm_pam_rf1_tech_gsm_a_ldo15,
      PM_NPA_VREG_LDO
   },
   {
      (void*)pm_pam_rf1_tech_gsm_a_ldo16,
      PM_NPA_VREG_LDO
   },
};
/* RF1_TECH_LTE Client */
pm_npa_clk_buff_kvps
pm_pam_rf1_tech_lte_a_clk_buff4 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX (2.1V)
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX (2.1V)
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX2 (2.05V)
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX2 (2.05V)
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
};

pm_npa_clk_buff_kvps
pm_pam_rf1_tech_lte_a_clk_buff5 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX (2.1V)
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX (2.1V)
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX2 (2.05V)
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX2 (2.05V)
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
};

pm_npa_ldo_kvps
pm_pam_rf1_tech_lte_a_ldo4 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX (2.1V)
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1225000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 260,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX (2.1V)
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1300000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 260,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX2 (2.05V)
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1225000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 260,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX2 (2.05V)
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1300000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 260,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
};

pm_npa_ldo_kvps
pm_pam_rf1_tech_lte_a_ldo14 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX (2.1V)
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1800000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 8,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX (2.1V)
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1800000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 8,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX2 (2.05V)
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1800000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 8,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX2 (2.05V)
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1800000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 8,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
};

pm_npa_ldo_kvps
pm_pam_rf1_tech_lte_a_ldo15 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX (2.1V)
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2100000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 105,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 50, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX (2.1V)
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2100000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 105,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 50, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX2 (2.05V)
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2050000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 105,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 50, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX2 (2.05V)
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2050000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 105,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 50, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
};

pm_npa_ldo_kvps
pm_pam_rf1_tech_lte_a_ldo16 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX (2.1V)
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2700000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 40,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX (2.1V)
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2700000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 40,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX2 (2.05V)
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2700000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 40,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX2 (2.05V)
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2700000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 40,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
};

pm_npa_smps_kvps
pm_pam_rf1_tech_lte_a_smps1 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX (2.1V)
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__NPM,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__QUIET,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX (2.1V)
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__NPM,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__QUIET,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX2 (2.05V)
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__NPM,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__QUIET,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX2 (2.05V)
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__NPM,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__QUIET,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__AUTO,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
};

pm_npa_pam_client_cfg_type
pm_pam_rf1_tech_lte_rails_info []=
{
   {
      (void*)pm_pam_rf1_tech_lte_a_clk_buff4,
      PM_NPA_CLK_BUFF
   },
   {
      (void*)pm_pam_rf1_tech_lte_a_clk_buff5,
      PM_NPA_CLK_BUFF
   },
   {
      (void*)pm_pam_rf1_tech_lte_a_ldo4,
      PM_NPA_VREG_LDO
   },
   {
      (void*)pm_pam_rf1_tech_lte_a_ldo14,
      PM_NPA_VREG_LDO
   },
   {
      (void*)pm_pam_rf1_tech_lte_a_ldo15,
      PM_NPA_VREG_LDO
   },
   {
      (void*)pm_pam_rf1_tech_lte_a_ldo16,
      PM_NPA_VREG_LDO
   },
   {
      (void*)pm_pam_rf1_tech_lte_a_smps1,
      PM_NPA_VREG_SMPS
   },
};
/* RF1_TECH_TDSCDMA Client */
pm_npa_clk_buff_kvps
pm_pam_rf1_tech_tdscdma_a_clk_buff4 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
};

pm_npa_clk_buff_kvps
pm_pam_rf1_tech_tdscdma_a_clk_buff5 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
};

pm_npa_ldo_kvps
pm_pam_rf1_tech_tdscdma_a_ldo4 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1225000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 260,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1300000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 260,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
};

pm_npa_ldo_kvps
pm_pam_rf1_tech_tdscdma_a_ldo14 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1800000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 8,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1800000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 8,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
};

pm_npa_ldo_kvps
pm_pam_rf1_tech_tdscdma_a_ldo15 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2050000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 105,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2050000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 105,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
};

pm_npa_ldo_kvps
pm_pam_rf1_tech_tdscdma_a_ldo16 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2700000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 40,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2700000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 40,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
};

pm_npa_pam_client_cfg_type
pm_pam_rf1_tech_tdscdma_rails_info []=
{
   {
      (void*)pm_pam_rf1_tech_tdscdma_a_clk_buff4,
      PM_NPA_CLK_BUFF
   },
   {
      (void*)pm_pam_rf1_tech_tdscdma_a_clk_buff5,
      PM_NPA_CLK_BUFF
   },
   {
      (void*)pm_pam_rf1_tech_tdscdma_a_ldo4,
      PM_NPA_VREG_LDO
   },
   {
      (void*)pm_pam_rf1_tech_tdscdma_a_ldo14,
      PM_NPA_VREG_LDO
   },
   {
      (void*)pm_pam_rf1_tech_tdscdma_a_ldo15,
      PM_NPA_VREG_LDO
   },
   {
      (void*)pm_pam_rf1_tech_tdscdma_a_ldo16,
      PM_NPA_VREG_LDO
   },
};
/* RF1_TECH_WCDMA Client */
pm_npa_clk_buff_kvps
pm_pam_rf1_tech_wcdma_a_clk_buff4 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
};

pm_npa_clk_buff_kvps
pm_pam_rf1_tech_wcdma_a_clk_buff5 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY, 4, PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE, /**< pin_ctrl_clk_buffer_enable_value */
   },
};

pm_npa_ldo_kvps
pm_pam_rf1_tech_wcdma_a_ldo4 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1225000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 260,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1300000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 260,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
};

pm_npa_ldo_kvps
pm_pam_rf1_tech_wcdma_a_ldo14 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1800000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 8,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1800000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 8,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
};

pm_npa_ldo_kvps
pm_pam_rf1_tech_wcdma_a_ldo15 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2050000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 105,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2050000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 105,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
};

pm_npa_ldo_kvps
pm_pam_rf1_tech_wcdma_a_ldo16 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_0
    // Comments: 	PMIC_NPA_MODE_ID_RF_SLEEP
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_LOW_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2700000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 40,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_RF_DVS_HIGH_RXTX
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__NPM,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2700000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 40,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 25, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_DISALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_RF_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
};

pm_npa_pam_client_cfg_type
pm_pam_rf1_tech_wcdma_rails_info []=
{
   {
      (void*)pm_pam_rf1_tech_wcdma_a_clk_buff4,
      PM_NPA_CLK_BUFF
   },
   {
      (void*)pm_pam_rf1_tech_wcdma_a_clk_buff5,
      PM_NPA_CLK_BUFF
   },
   {
      (void*)pm_pam_rf1_tech_wcdma_a_ldo4,
      PM_NPA_VREG_LDO
   },
   {
      (void*)pm_pam_rf1_tech_wcdma_a_ldo14,
      PM_NPA_VREG_LDO
   },
   {
      (void*)pm_pam_rf1_tech_wcdma_a_ldo15,
      PM_NPA_VREG_LDO
   },
   {
      (void*)pm_pam_rf1_tech_wcdma_a_ldo16,
      PM_NPA_VREG_LDO
   },
};
/* UIM1_ACTIVITY Client */
pm_npa_ldo_kvps
pm_pam_uim1_activity_a_ldo9 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_UIM_STANDBY
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 1,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_UIM_ACTIVE
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 50,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_10
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_11
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_12
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_13
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
};

pm_npa_pam_client_cfg_type
pm_pam_uim1_activity_rails_info []=
{
   {
      (void*)pm_pam_uim1_activity_a_ldo9,
      PM_NPA_VREG_LDO
   },
};
/* UIM1_ENABLE Client */
pm_npa_ldo_kvps
pm_pam_uim1_enable_a_ldo9 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_UIM_NO_VOTE
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_UIM_CLASS_C_LOW
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1700000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_3
    // Comments: 	PMIC_NPA_MODE_ID_UIM_CLASS_C
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1800000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_4
    // Comments: 	PMIC_NPA_MODE_ID_UIM_CLASS_C_HIGH
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1900000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_5
    // Comments: 	PMIC_NPA_MODE_ID_UIM_CLASS_B_LOW
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2850000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_6
    // Comments: 	PMIC_NPA_MODE_ID_UIM_CLASS_B
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 3000000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_7
    // Comments: 	PMIC_NPA_MODE_ID_UIM_CLASS_B_HIGH
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 3050000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_10
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_11
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_12
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_13
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
};

pm_npa_pam_client_cfg_type
pm_pam_uim1_enable_rails_info []=
{
   {
      (void*)pm_pam_uim1_enable_a_ldo9,
      PM_NPA_VREG_LDO
   },
};
/* UIM2_ACTIVITY Client */
pm_npa_ldo_kvps
pm_pam_uim2_activity_a_ldo10 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_UIM_STANDBY
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 1,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_UIM_ACTIVE
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 50,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_3
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_4
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_5
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_6
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_7
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_10
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_11
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_12
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_13
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
};

pm_npa_pam_client_cfg_type
pm_pam_uim2_activity_rails_info []=
{
   {
      (void*)pm_pam_uim2_activity_a_ldo10,
      PM_NPA_VREG_LDO
   },
};
/* UIM2_ENABLE Client */
pm_npa_ldo_kvps
pm_pam_uim2_enable_a_ldo10 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_1
    // Comments: 	PMIC_NPA_MODE_ID_UIM_NO_VOTE
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_2
    // Comments: 	PMIC_NPA_MODE_ID_UIM_CLASS_C_LOW
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1700000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_3
    // Comments: 	PMIC_NPA_MODE_ID_UIM_CLASS_C
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1800000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_4
    // Comments: 	PMIC_NPA_MODE_ID_UIM_CLASS_C_HIGH
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1900000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_5
    // Comments: 	PMIC_NPA_MODE_ID_UIM_CLASS_B_LOW
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 2850000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_6
    // Comments: 	PMIC_NPA_MODE_ID_UIM_CLASS_B
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 3000000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_7
    // Comments: 	PMIC_NPA_MODE_ID_UIM_CLASS_B_HIGH
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 3050000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_8
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_9
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_10
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_11
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_12
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
    // Mode: 		PMIC_NPA_MODE_ID_UIM_MODE_13
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_DISABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_LDO_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_LDO__IPEAK,  /**< [BYPASS, IPEAK (default), NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 0,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
   },
};

pm_npa_pam_client_cfg_type
pm_pam_uim2_enable_rails_info []=
{
   {
      (void*)pm_pam_uim2_enable_a_ldo10,
      PM_NPA_VREG_LDO
   },
};
/* VDD_MEM_UVOL Client */
pm_npa_smps_kvps
pm_pam_vdd_mem_uvol_b_smps1 [] =
{
    // Mode: 		PMIC_NPA_MODE_ID_CORE_RAIL_TURBO
   {
      PM_NPA_KEY_SOFTWARE_ENABLE, 4, PM_NPA_GENERIC_ENABLE,  /**< [Disable (default), Enable] -> max aggregation (left to right) */
      PM_NPA_KEY_SMPS_SOFTWARE_MODE, 4, PM_NPA_SW_MODE_SMPS__NPM,  /**< [AUTO (default), IPEAK, NPM] -> max aggregation (left to right) */
      PM_NPA_KEY_PIN_CTRL_ENABLE, 4, PM_NPA_PIN_CONTROL_ENABLE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4] -> ORed value of list */
      PM_NPA_KEY_PIN_CTRL_POWER_MODE, 4, PM_NPA_PIN_CONTROL_POWER_MODE__NONE,  /**< [NONE, EN1, EN2, EN3, EN4, SLEEPB] -> ORed value of list */
      PM_NPA_KEY_MICRO_VOLT, 4, 1050000,  /**< [X uV] -> max aggregation */
      PM_NPA_KEY_CURRENT, 4, 0,  /**< [X mA] -> max aggregation */
      PM_NPA_KEY_FREQUENCY, 4, PM_SWITCHING_FREQ_FREQ_NONE,  /**< [xx MHz] -> max within a priority group */
      PM_NPA_KEY_FREQUENCY_REASON, 4, PM_NPA_FREQ_REASON_NONE,  /**< Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority) */
      PM_NPA_KEY_FOLLOW_QUIET_MODE, 4, PM_NPA_QUIET_MODE__DISABLE,  /**< [None, Quiet, Super Quiet] -> max aggregation (left to right)  */
      PM_NPA_KEY_BYPASS_ALLOWED_KEY, 4, PM_NPA_BYPASS_ALLOWED,  /**< [Allowed (default), Disallowed] */
      PM_NPA_KEY_CORNER_LEVEL_KEY, 4, PM_NPA_CORNER_MODE__NONE,  /**< [None, Level1 (Retention), Level2, Level3, Level4, Level5, Level6 (SuperTurbo), Not Used] */
      PM_NPA_KEY_HEAD_ROOM, 4, 0, /**< head_room_voltage_value */
   },
};

pm_npa_pam_client_cfg_type
pm_pam_vdd_mem_uvol_rails_info []=
{
   {
      (void*)pm_pam_vdd_mem_uvol_b_smps1,
      PM_NPA_VREG_SMPS
   },
};



