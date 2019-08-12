# 1 "/local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/audio_avs/elite/utils/devcfg/config/lpm.xml"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/audio_avs/elite/utils/devcfg/config/lpm.xml" 1
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 136 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/audio_avs/elite/utils/devcfg/config/lpm.xml" 2
<!--
  Copyright (c) 2013 Qualcomm Technologies Incorporated.
           All Rights Reserved.

   DESCRIPTION: LPM Device Configuration.
   $Header: $
 -->

<!-- NULL Driver does not require Dal Driver Interface APIs, since none of AVS HWD drivers uses DAL Device framework -->
<driver name="NULL">
   <!-- LPM -->
   <device id="LPM">
      <props name="LPMPropStructPtr" type=DALPROP_ATTR_TYPE_STRUCT_PTR>lpm_prop</props>
   </device>
</driver>
