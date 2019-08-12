#line 1 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/mproc/smem/config/smem_toc.xml"

























<driver name="NULL">
  <global_def></global_def>
  <device id="/core/mproc/smem">
	<props name="smem_toc_vers" type=DALPROP_ATTR_TYPE_UINT32 >
	  0x00000001
	</props>
    <props name="smem_partitions" type=DALPROP_ATTR_TYPE_STRUCT_PTR >
      smem_partition_toc_entries
    </props>
  </device>
</driver>
