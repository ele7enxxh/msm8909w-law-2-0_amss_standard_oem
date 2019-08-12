#!/bin/bash

cd `dirname $0`
sh build.sh CHIPSET=mdm9x07 tz tzbsp_no_xpu USES_FLAGS=USES_PLAYREADY_PACK
mkdir -p ./../../../temp/trustzone_images/core/securemsm/trustzone
\cp -f -r ./../../../trustzone_images/core/securemsm/trustzone/monitor ./../../../temp/trustzone_images/core/securemsm/trustzone
\cp -f -r ./../../../trustzone_images/core/securemsm/trustzone/qsee ./../../../temp/trustzone_images/core/securemsm/trustzone
sh build.sh CHIPSET=mdm9x07 tz  tzbsp_no_xpu USES_FLAGS=USES_CLEAN_PACK -c --implicit-deps-unchanged
sh build.sh CHIPSET=mdm9x07 tz  tzbsp_no_xpu -c --implicit-deps-unchanged --cleanpack
sh build.sh CHIPSET=mdm9x07 tz  tzbsp_no_xpu
