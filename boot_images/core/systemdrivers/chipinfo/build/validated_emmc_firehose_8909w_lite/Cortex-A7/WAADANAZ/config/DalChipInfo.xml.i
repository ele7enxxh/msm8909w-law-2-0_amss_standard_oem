<driver name="ChipInfo">
  <device id=DALDEVICEID_CHIPINFO>
     <props name="ChipIdOverride" type=DALPROP_ATTR_TYPE_UINT32>
       245
     </props>
     <props name="HWREVNUM_PHYS_ADDR" type=DALPROP_ATTR_TYPE_UINT32> 
       0x01000000
     </props>
     <props name="HWREVNUM_OFFSET" type=DALPROP_ATTR_TYPE_UINT32> 
       0x00110010
     </props> 
     <props name="PARTNUM_BMSK" type=DALPROP_ATTR_TYPE_UINT32>
       0xffff000
     </props> 
     <props name="PARTNUM_SHFT" type=DALPROP_ATTR_TYPE_UINT32>
       0xc
     </props> 
     <props name="VERSION_ID_BMSK" type=DALPROP_ATTR_TYPE_UINT32>
       0xf0000000
     </props> 
     <props name="VERSION_ID_SHFT" type=DALPROP_ATTR_TYPE_UINT32>
       0x1c
     </props> 
     <props name="QUALCOMM_MFG_ID_BMSK" type=DALPROP_ATTR_TYPE_UINT32>
       0xffe
     </props> 
     <props name="QUALCOMM_MFG_ID_SHFT" type=DALPROP_ATTR_TYPE_UINT32>
       0x1
     </props> 
     <props name="SOC_HW_VERSION_PHYS_ADDR" type=DALPROP_ATTR_TYPE_UINT32> 
       0x01900000 
     </props>
     <props name="SOC_HW_VERSION_OFFSET" type=DALPROP_ATTR_TYPE_UINT32> 
       0x0004d000 
     </props> 
     <props name="MAJOR_VERSION_BMSK" type=DALPROP_ATTR_TYPE_UINT32>
       0x0000FF00
     </props> 
     <props name="MAJOR_VERSION_SHFT" type=DALPROP_ATTR_TYPE_UINT32>
       0x8
     </props> 
     <props name="MINOR_VERSION_BMSK" type=DALPROP_ATTR_TYPE_UINT32>
       0x000000FF
     </props> 
     <props name="MINOR_VERSION_SHFT" type=DALPROP_ATTR_TYPE_UINT32>
       0x0
     </props> 
     <props name="FOUNDRY_PHYS_ADDR" type=DALPROP_ATTR_TYPE_UINT32>
       0x00058000
     </props>
     <props name="FOUNDRY_OFFSET" type=DALPROP_ATTR_TYPE_UINT32> 
       0x000C 
     </props> 
     <props name="FOUNDRY_BMSK" type=DALPROP_ATTR_TYPE_UINT32>
       0x00000700
     </props> 
     <props name="FOUNDRY_SHFT" type=DALPROP_ATTR_TYPE_UINT32>
       0x8
     </props>
     <props name="SERIALNUM_OFFSET" type=DALPROP_ATTR_TYPE_UINT32> 
       0x0008 
     </props> 
     <props name="SERIALNUM_BMSK" type=DALPROP_ATTR_TYPE_UINT32>
       0xFFFFFFFF
     </props> 
     <props name="SERIALNUM_SHFT" type=DALPROP_ATTR_TYPE_UINT32>
       0x0
     </props> 
  </device>
</driver>
