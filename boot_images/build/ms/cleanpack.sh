#!/bin/bash

cd `dirname $0`
#First build to ensure no error

if [ $1 = "9x45" ]
then
       echo  "Building for Target 9x45 "
       sh build.sh TARGET_FAMILY=9x45  --prod
 
       sh build.sh TARGET_FAMILY=9x45 BUILD_ID=AAATANAZ USES_FLAGS=USES_CLEAN_PACK -c --implicit-deps-unchanged 
       sh build.sh TARGET_FAMILY=9x45 BUILD_ID=AAAHANAZ USES_FLAGS=USES_CLEAN_PACK -c --implicit-deps-unchanged 

       # Remove unship files
       sh build.sh TARGET_FAMILY=9x45 BUILD_ID=AAATANAZ -c --implicit-deps-unchanged --cleanpack
       sh build.sh TARGET_FAMILY=9x45 BUILD_ID=AAAHANAZ -c --implicit-deps-unchanged --cleanpack

       #recommpile
       sh build.sh TARGET_FAMILY=9x45 BUILD_ID=AAATANAZ
       sh build.sh TARGET_FAMILY=9x45 BUILD_ID=AAAHANAZ
       
fi


if [ $1 = "8909" ]
then
       echo  " Building for Target 8909 "
       sh build.sh TARGET_FAMILY=8909 --prod

       sh build.sh TARGET_FAMILY=8909 BUILD_ID=DAATANAZ USES_FLAGS=USES_CLEAN_PACK -c --implicit-deps-unchanged
	   sh build.sh TARGET_FAMILY=8909 BUILD_ID=DAASANAZ USES_FLAGS=USES_CLEAN_PACK -c --implicit-deps-unchanged
	   sh build.sh TARGET_FAMILY=8909 BUILD_ID=DAADANAZ -c --implicit-deps-unchanged USES_FLAGS=USES_CLEAN_PACK

       # Remove unship files
	   sh build.sh TARGET_FAMILY=8909 BUILD_ID=DAATANAZ -c --implicit-deps-unchanged --cleanpack 
       sh build.sh TARGET_FAMILY=8909 BUILD_ID=DAASANAZ -c --implicit-deps-unchanged --cleanpack 
	   sh build.sh TARGET_FAMILY=8909 BUILD_ID=DAADANAZ -c --implicit-deps-unchanged --cleanpack USES_FLAGS=USES_DEVPRO_DDR,USES_DEVPRO_DDR_SEC,USES_DEVPRO_LITE,USES_DEVPRO_LITE_SEC

       #recommpile
       sh build.sh TARGET_FAMILY=8909 --prod
              
fi

if [ $1 = "8909w" ]
then
       echo  " Building for Target 8909w "
       sh build.sh TARGET_FAMILY=8909w --prod

       sh build.sh TARGET_FAMILY=8909w BUILD_ID=WAATANAZ USES_FLAGS=USES_CLEAN_PACK -c --implicit-deps-unchanged
	   sh build.sh TARGET_FAMILY=8909w BUILD_ID=WAASANAZ USES_FLAGS=USES_CLEAN_PACK -c --implicit-deps-unchanged
	   sh build.sh TARGET_FAMILY=8909w BUILD_ID=WAADANAZ -c --implicit-deps-unchanged USES_FLAGS=USES_CLEAN_PACK
	  
	  # Remove unship files
	   sh build.sh TARGET_FAMILY=8909w BUILD_ID=WAATANAZ -c --implicit-deps-unchanged --cleanpack 
       sh build.sh TARGET_FAMILY=8909w BUILD_ID=WAASANAZ -c --implicit-deps-unchanged --cleanpack 
	   sh build.sh TARGET_FAMILY=8909w BUILD_ID=WAADANAZ -c --implicit-deps-unchanged --cleanpack USES_FLAGS=USES_DEVPRO_DDR,USES_DEVPRO_DDR_SEC,USES_DEVPRO_LITE,USES_DEVPRO_LITE_SEC

       #recommpile
       sh build.sh TARGET_FAMILY=8909w --prod
              
fi

if [ $1 = "8952" ]
then
       echo  " Building for Target 8952 "
       sh build.sh TARGET_FAMILY=8952  --prod

       sh build.sh TARGET_FAMILY=8952 BUILD_ID=SAAAANAZ  USES_FLAGS=USES_CLEAN_PACK -c --implicit-deps-unchanged 

       # Remove unship files
       sh build.sh TARGET_FAMILY=8952 BUILD_ID=SAAAANAZ  -c --implicit-deps-unchanged --cleanpack

       #recommpile
       sh build.sh TARGET_FAMILY=8952 BUILD_ID=SAAAANAZ 
              
fi

if [ $1 = "8956" ]
then
       echo  " Building for Target 8956 "
       sh build.sh TARGET_FAMILY=8956  --prod

       sh build.sh TARGET_FAMILY=8956 BUILD_ID=EAAAANAZ  USES_FLAGS=USES_CLEAN_PACK -c --implicit-deps-unchanged 

       # Remove unship files
       sh build.sh TARGET_FAMILY=8956 BUILD_ID=EAAAANAZ  -c --implicit-deps-unchanged --cleanpack

       #recommpile
       sh build.sh TARGET_FAMILY=8956 BUILD_ID=EAAAANAZ 
              
fi

