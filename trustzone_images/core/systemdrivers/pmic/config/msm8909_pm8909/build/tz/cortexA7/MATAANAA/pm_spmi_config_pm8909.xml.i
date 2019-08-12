<driver name="NULL">
   <device id="/pmic/periph">
      <props name="pm_arb_prio_cfg" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
         pm_arb_priorities
      </props>
      <props name="number_priorities" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
         pm_num_prio
      </props>
      <props name="pm_arb_pvc_cfg" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
         pm_arb_pvc_cfg_8909
      </props>      
      <props name="number_pvc_port" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
         pm_num_pvc
      </props>
      <props name="xpu_cfg" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
         pm_spmi_mpu_cfg
      </props>
      <props name="xpu_cfg_size" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
          pm_spmi_mpu_cfg_size
      </props>
   </device>
</driver>
