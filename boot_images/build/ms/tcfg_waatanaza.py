#==============================================================================
# Target Build ID Config Script.
#
# Copyright (c) 2010 Qualcomm Technologies Inc.  All Rights Reserved
#==============================================================================

def exists(env):
   return env.Detect('tcfg_WAATANAZA_data')

def generate(env):
   # Save the tcfg_bid_data in the env
   env['TCFG_BID_IMAGE'] = 'APPS_PROC'

   env.AddUsesFlags('USES_BOOT_EXTERN_APT_COMPLETED', from_builds_file = True)
   env.AddUsesFlags('USES_BOOT_EXTERN_BAM_COMPLETED', from_builds_file = True)
   env.AddUsesFlags('USES_BOOT_EXTERN_BUSYWAIT_COMPLETED', from_builds_file = True)
   env.AddUsesFlags('USES_BOOT_EXTERN_BUS_COMPLETED', from_builds_file = True)
   env.AddUsesFlags('USES_BOOT_EXTERN_CLK_COMPLETED', from_builds_file = True)
   env.AddUsesFlags('USES_BOOT_EXTERN_CRYPTO_COMPLETED', from_builds_file = True)
   env.AddUsesFlags('USES_BOOT_EXTERN_DAL_COMPLETED', from_builds_file = True)
   env.AddUsesFlags('USES_BOOT_EXTERN_DDR_COMPLETED', from_builds_file = True)
   env.AddUsesFlags('USES_BOOT_EXTERN_FLASH_COMPLETED', from_builds_file = True)
   env.AddUsesFlags('USES_BOOT_EXTERN_HOTPLUG_COMPLETED', from_builds_file = True)
   env.AddUsesFlags('USES_BOOT_EXTERN_HSUSB_COMPLETED', from_builds_file = True)
   env.AddUsesFlags('USES_BOOT_EXTERN_PLATFORMINFO_COMPLETED', from_builds_file = True)
   env.AddUsesFlags('USES_BOOT_EXTERN_PMIC_COMPLETED', from_builds_file = True)
   env.AddUsesFlags('USES_BOOT_EXTERN_SECBOOT_COMPLETED', from_builds_file = True)
   env.AddUsesFlags('USES_BOOT_EXTERN_SECCFG_COMPLETED', from_builds_file = True)
   env.AddUsesFlags('USES_BOOT_EXTERN_SMEM_COMPLETED', from_builds_file = True)
   env.AddUsesFlags('USES_BOOT_EXTERN_TLMM_COMPLETED', from_builds_file = True)
   env.AddUsesFlags('USES_BOOT_EXTERN_TSENSOR_COMPLETED', from_builds_file = True)
   env.AddUsesFlags('USES_BOOT_EXTERN_UART_COMPLETED', from_builds_file = True)
   env.AddUsesFlags('USES_BOOT_LOGGER_RAM', from_builds_file = True)
   env.AddUsesFlags('USES_BOOT_LOGGER_TIMER', from_builds_file = True)
   env.AddUsesFlags('USES_BOOT_LOGGER_UART', from_builds_file = True)
   env.AddUsesFlags('USES_BOOT_VERSION_ROLL_BACK', from_builds_file = True)
   env.AddUsesFlags('USES_CDT_ON_EEPROM', from_builds_file = True)
   env.AddUsesFlags('USES_EFS_READ_ONLY', from_builds_file = True)
   env.AddUsesFlags('USES_ELF_LOAD_BOOT', from_builds_file = True)
   env.AddUsesFlags('USES_MBNTOOLS', from_builds_file = True)
   env.AddUsesFlags('USES_NAND_BOOT', from_builds_file = True)
   env.AddUsesFlags('USES_SAHARA_BOOT', from_builds_file = True)
   env.AddUsesFlags('USES_SEC_POLICY_DEFAULT_SIGN', from_builds_file = True)
   env.AddUsesFlags('USES_STRIP_NO_ODM', from_builds_file = True)
