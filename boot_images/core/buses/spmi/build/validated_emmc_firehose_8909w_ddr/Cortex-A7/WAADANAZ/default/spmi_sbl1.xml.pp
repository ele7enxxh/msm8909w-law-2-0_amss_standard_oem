#line 1 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/buses/spmi/src/platform/config/bear/default/spmi_sbl1.xml"
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

#line 1 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/buses/spmi/src/platform/config/bear/default/spmi_sys.cfg"
<!-- System props -->
<props name="pmic_arb_base_addr" type=DALPROP_ATTR_TYPE_UINT32>
    0x02000000
</props>
#line 14 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/buses/spmi/src/platform/config/bear/default/spmi_sbl1.xml"
        
        <!-- User configurable props -->
        <props name="owner" type=DALPROP_ATTR_TYPE_UINT32>
            0
        </props>
        
    </device>
</driver>
