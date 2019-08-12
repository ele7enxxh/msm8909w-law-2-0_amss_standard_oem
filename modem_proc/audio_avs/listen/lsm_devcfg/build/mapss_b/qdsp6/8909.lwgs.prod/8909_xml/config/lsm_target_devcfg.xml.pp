# 1 "/local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/audio_avs/listen/lsm_devcfg/target/config/lsm_target_devcfg.xml"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/audio_avs/listen/lsm_devcfg/target/config/lsm_target_devcfg.xml" 1
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 136 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/audio_avs/listen/lsm_devcfg/target/config/lsm_target_devcfg.xml" 2
<!--
  This file contains LSM device configuration parsing information

  Copyright (c) 2009-2015 Qualcomm Technologies, Incorporated. All Rights Reserved.
  QUALCOMM Proprietary. Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 -->

<!-- NULL Driver does not require Dal Driver Interface APIs, since none of LSM devcfg uses DAL Device framework -->
<driver name="NULL">
   <device id="LSM">
      <props name="lsm_mmpm_prop_struct_ptr" type=DALPROP_ATTR_TYPE_STRUCT_PTR>lsm_mmpm_prop</props>
   </device>
</driver>
