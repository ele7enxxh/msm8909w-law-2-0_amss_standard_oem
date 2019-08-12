#line 1 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/dal/config/build/ehostdl/sparrow/WAATANAZ/DevCfg_master.xml"
#line 1 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/dal/DALPropDef.h"






 









#line 24 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/dal/DALPropDef.h"
























 
 
 























 

 









#line 97 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/dal/DALPropDef.h"



#line 110 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/dal/DALPropDef.h"

#line 2 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/dal/config/build/ehostdl/sparrow/WAATANAZ/DevCfg_master.xml"
#line 1 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/dal/DALDeviceId.h"






 


































#line 56 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/dal/DALDeviceId.h"

#line 66 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/dal/DALDeviceId.h"








































































































#line 176 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/dal/DALDeviceId.h"


















#line 202 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/dal/DALDeviceId.h"











#line 219 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/dal/DALDeviceId.h"

#line 226 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/dal/DALDeviceId.h"

#line 233 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/dal/DALDeviceId.h"

 
#line 267 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/dal/DALDeviceId.h"







#line 296 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/dal/DALDeviceId.h"






#line 317 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/dal/DALDeviceId.h"

















#line 341 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/dal/DALDeviceId.h"



#line 351 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/api/dal/DALDeviceId.h"




































#line 3 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/dal/config/build/ehostdl/sparrow/WAATANAZ/DevCfg_master.xml"
#line 1 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/dal/config/dalconfig.h"

 





#line 9 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/dal/config/dalconfig.h"





 

 


 






#line 4 "/local/mnt/workspace/CRMBuilds/BOOT.BF.3.1.2.c3-00012-M8909WAAAANAZB-1_20161208_043404/b/boot_images/core/dal/config/build/ehostdl/sparrow/WAATANAZ/DevCfg_master.xml"

<?xml version="1.0"?>
<dal>
<module name="boot">
<driver name="System">
    <global_def> 
        <var_seq name="g_target_cfg" type=0x00000003> 
            0x8909, 0x2070, end
        </var_seq>
    </global_def> 
    <device id="0x0">
        <props name="DalEnv" type=0x00000002>
            0x1001
        </props>
        <props name= "TargetCfg" type=0x00000014> 
          g_target_cfg
        </props>
    </device>
</driver>

 <driver name="Flash">
   <device id=0x02000050>
   </device>
   <device id=0x0200013F>
   </device>
 </driver>

</module>
</dal>
