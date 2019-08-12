#line 1 "Z:\\b\\rpm_proc\\core\\buses\\spmi\\src\\platform\\config\\bear\\8909\\spmi_rpm.xml"
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

#line 1 "Z:\\b\\rpm_proc\\core\\buses\\spmi\\src\\platform\\config\\bear\\8909\\spmi_sys.cfg"
<!-- System props -->
<props name="pmic_arb_base_addr" type=DALPROP_ATTR_TYPE_UINT32>
    0x02000000
</props>
#line 14 "Z:\\b\\rpm_proc\\core\\buses\\spmi\\src\\platform\\config\\bear\\8909\\spmi_rpm.xml"
        
        <!-- User configurable props -->
        <props name="owner" type=DALPROP_ATTR_TYPE_UINT32>
            4
        </props>
        <props name="interrupt" type=DALPROP_ATTR_TYPE_UINT32>
            48
        </props>
        
    </device>
</driver>
