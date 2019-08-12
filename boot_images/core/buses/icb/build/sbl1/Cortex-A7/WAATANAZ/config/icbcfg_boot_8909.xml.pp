#line 1 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/buses/icb/config/icbcfg_boot_8909.xml"





























<driver name="NULL">
   <global_def>
        
   </global_def>
   <device id="/dev/icbcfg/boot">
     <props name="icb_config_data" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
        icbcfg_boot_prop
     </props> 
     <props name="num_channels" type=DALPROP_ATTR_TYPE_UINT32>
        1
     </props> 
     <props name="num_segments" type=DALPROP_ATTR_TYPE_UINT32>
        3
     </props> 
     <props name="map_ddr_region_count" type=DALPROP_ATTR_TYPE_UINT32>
        1
     </props> 
     <props name="map_ddr_regions" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
        map_ddr_regions
     </props> 
     <props name="bimc_hal_info" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
        bimc_hal_info
     </props> 
     <props name="channel_map" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
        channel_map
     </props> 
     <props name="safe_reset_seg" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
        safe_reset_seg
     </props> 
     <props name="icb_lock_data" type=DALPROP_ATTR_TYPE_STRUCT_PTR>
        icbcfg_lock_prop
     </props> 
   </device>   
</driver>
