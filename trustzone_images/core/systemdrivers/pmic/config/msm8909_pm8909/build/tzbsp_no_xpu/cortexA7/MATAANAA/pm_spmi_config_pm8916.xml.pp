#line 1 "/local/mnt/workspace/CRMBuilds/TZ.BF.2.5.c10-00004-M8909AAAAANAZT-2_20161130_222454/b/trustzone_images/core/systemdrivers/pmic/config/msm8909_pm8909/src/pm_spmi_config_pm8916.xml"







 











 

<driver name="NULL">
   <device id="/pmic/periph/pm8916">
      <props name="pm_arb_prio_cfg" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
         pm8916_pm_arb_priorities
      </props>
      <props name="number_priorities" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
         pm8916_pm_num_prio
      </props>
      <props name="pm_arb_pvc_cfg" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
         pm_arb_pvc_cfg_8916
      </props>      
      <props name="number_pvc_port" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
         pm8916_pm_num_pvc
      </props>
      <props name="xpu_cfg" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
         pm8916_pm_spmi_mpu_cfg
      </props>
      <props name="xpu_cfg_size" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
          pm8916_pm_spmi_mpu_cfg_size
      </props>
   </device>
</driver>
