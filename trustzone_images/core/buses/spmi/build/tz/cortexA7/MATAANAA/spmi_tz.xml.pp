#line 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/buses/spmi/src/platform/config/bear/default/spmi_tz.xml"
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

#line 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/buses/spmi/src/platform/config/bear/default/spmi_sys.cfg"
<!-- System props -->
<props name="pmic_arb_base_addr" type=DALPROP_ATTR_TYPE_UINT32>
    0x02000000
</props>
<props name="useDebugChan" type=DALPROP_ATTR_TYPE_UINT32>
    1
</props>
#line 14 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/buses/spmi/src/platform/config/bear/default/spmi_tz.xml"
        
        <!-- User configurable props -->
        <props name="owner" type=DALPROP_ATTR_TYPE_UINT32>
            1
        </props>
        <props name="useDebugChan" type=DALPROP_ATTR_TYPE_UINT32>
    	   1
	</props>
    </device>
</driver>
