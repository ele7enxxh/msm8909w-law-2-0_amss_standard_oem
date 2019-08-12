#===============================================================================
#
# CBSP Buils system
#
# General Description
#    build shell script file.
#
# Copyright (c) 2009,2013 - 2014,2015 by QUALCOMM, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
# $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/build/ms/build.sh#2 $
# $DateTime: 2015/09/13 23:33:33 $
# $Author: pwbldsvc $
# $Change: 9017835 $
#                      EDIT HISTORY FOR FILE
#
# This section contains comments describing changes made to the module.
# Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     -----------------------------------------------------------
# 04/21/15   yps     Add support pm8916 for DDI
# 03/20/15   sve     Added compilation support for Nand sbl and flash tools
# 01/22/15   sk  	 Code changes for merging firehose DeviceProgrammer into SBL
# 01/08/15   sc      Changes to generate two Elfs for RUMI and asic
# 01/02/15   moh     Appended deviceprogrammer flags for external build flavor
# 12/17/14   sve     Code changes for merging firehose DeviceProgrammer into SBL
# 12/15/14   yps     Fixed 8909 DDI prod compilation problem
# 11/25/14   yps     Added 8909 DDI build cmd
# 11/21/14   sk      Done changes for creating two elfs for RUMI and ASIC
# 11/05/14   bs      Added for 8956 build command
# 11/19/14   sy      Adding emmc
# 10/30/14   aus     Remove emmcbld compilation and NAND build support
# 09/29/14   lm      Added for 8952 build command
# 09/26/14   sk      Added flashtools for compiling nand tools on linux machine for 8909 Target
# 08/05/14   jz      Added emmc build support for 9x45
# 08/07/14   mrb     removed 8916/8936 compilation
# 03/07/14   jz      Added for 9x45 build command
# 02/13/14   sve     Added 8936 build command
# 10/08/13   ck      Fixed 8916 default build id to match .builds file
# 10/07/13   ck      Removed targets that are not in Bear family
# 09/18/13   svelic  Added support for jsdcc.elf compilation for 8962 target
# 09/09/13   jz      Added emmcbld jtagprogrammer deviceprogrammer for 8084
# 09/03/13   svelic  Added support for jsdcc.elf compilation for 8084 target
# 09/12/13   ck      Add support for MSM8916
# 07/23/13   plc     Add support for MSM8994
# 07/11/13    wg     Updated Build_ID for 8x62 and 8092
# 07/03/13   ajs     Added deviceprogrammer to 8974, 8x10 and 8x26 targets
# 06/26/13   plc     Adding support for 8092
# 06/13/13   kedara  Support 9x35 source compilation.
# 05/10/13   sy      Fixing 9x25 builds  and 8084 mistakes
# 05/06/13   plc     Adding support for 8x62
# 04/23/13   sy      Adding 8084 production build  
# 03/27/13   dh      Add 8084 build id GAAAANAZ, only compile boot for 8084
# 04/08/13   sg      adding emmcbld for 8x10 and 8x26
# 03/04/13   sy      Fixing 9x25 and 8974 Pack issue 
# 12/12/12   sy      Adding emmcbld to 8974
# 10/31/12   sy      Fix 8974 Pack 
# 09/18/12   sy      Enable 8x10 ,8x26 compilation 
# 09/09/12   sy      Adding Linux pack 
# 08/08/12   sy      Enable warning=error at the top level build script
# 08/01/12   sy      Adding emmcbld 
# 07/20/2012 sy      Adding 9x25 compilation
#===============================================================================
#===============================================================================
# Set up default paths
#===============================================================================
#!/bin/bash
export BUILD_ROOT=../..
export CORE_BSP_ROOT=$BUILD_ROOT/core/bsp
export TOOLS_SCONS_ROOT=$BUILD_ROOT/tools/build/scons

cd `dirname $0`
#-------------Usage --------------
function usage()
{
    echo "==============================================================="
    echo "# This script compiles boot binaries for ONE of the Bear family"
    echo "# i.e.  9x45. "
    echo "# Target must be specific by TARGET_FAMILY"
    echo "# -------------------------------------------------------------"
    echo ""
    echo " ./build_all.sh                        # compile all available targets"
    echo " ./build.sh  --usage                   # display this menu"
    echo " ./build.sh  TARGET_FAMILY=9x45 --prod # create customer build for 9x45"
    echo " ./build.sh  TARGET_FAMILY=9x45       # compiling 9x45"
    echo " ./build.sh  -c TARGET_FAMILY=9x45     # clean 9x45 binaries"
    echo " ./build.sh  TARGET_FAMILY=9x45      # compile 9x45 binaries"
    echo " ./build.sh  TARGET_FAMILY=8909      # compile 8909  binaries"
    echo " ./build.sh                            # Error!! target name is missing"
    echo ""
    echo " # To build individual module , use the target specific build comand\n"
    echo " ./build_8909.sh sbl1                       # compiling 8909 sbl1\n"
    echo " ./build_8909.sh -c sbl1                    # cleaning 8909 sbl1\n"
    echo " ./build_9x45.sh boot                       # compiling 9x45 boot\n"
    echo " ./build_9x45.sh flashtools                 # compiling 9x45 flash tool\n"
    echo "==============================================================="
    sleep 2   
}
#-------- set the build dir and  reset the command line parameter ----- 
build_dir=`dirname $0`
cd $build_dir
#===============================================================================
#Set up  Environment 
#===============================================================================
usage


# if setenv.sh exist, run it.
[ -f $BUILD_ROOT/build/ms/setenv.sh ] && source setenv.sh || echo "Warning: SetEnv file not existed" 
#----- Parse target enviroment----
unset TARGET_FAMILY 
unset  BUILD_ID
unset cmds
isProd=0 

while [ $# -gt 0 ]
do
    case "$1" in
	BUILD_ID=*) 
		BUILD_ID=${1#BUILD_ID=}
		;;
	TARGET_FAMILY=*) 
		TARGET_FAMILY=${1#TARGET_FAMILY=}
		;;
	--usage) 
		usage
		exit 0
		;;	
	--prod) 
		isProd=1	
		cmds=${cmds//--prod/}
		if [ -e setenv.sh ] ; then #this is integrator build
			cmds="$cmds USES_FLAGS=USES_NO_STRIP_NO_ODM,USES_NO_DEBUG" 
		fi 
		;;
	*)
		cmds="$cmds $1"
    esac
    shift
done

source $TOOLS_SCONS_ROOT/build/rename-log.sh buildlog

#Set Default to 8916 
#export TARGET_FAMILY=${TARGET_FAMILY:-8916}

#Enable warning=error 
cmds="CFLAGS=--diag_error=warning $cmds"

#8909 compilation on nand and emmc
if [ "$TARGET_FAMILY" = "8909" ]
then
    if [ -z $BUILD_ID ] 
    then
	if [ $isProd -gt 0 ]
	then 
	  sh build_8909.sh boot flashtools BUILD_ID=DAATANAZ $cmds  
	  sh build_8909.sh boot jsdcc  BUILD_ID=DAASANAZ  $cmds
      sh build_8909.sh deviceprogrammer_ddr BUILD_ID=DAADANAZ $cmds,USES_DEVPRO_DDR
      sh build_8909.sh deviceprogrammer_ddr BUILD_ID=DAADANAZ $cmds,USES_DEVPRO_DDR_SEC
      sh build_8909.sh deviceprogrammer_ddr BUILD_ID=DAADANAZ $cmds,USES_DEVPRO_LITE
      sh build_8909.sh deviceprogrammer_ddr BUILD_ID=DAADANAZ $cmds,USES_DEVPRO_LITE_SEC
	  sh build_8909.sh ddr_debug BUILD_ID=DAASANAZ $cmds
      sh build_8909.sh ddr_debug BUILD_ID=DAASANAZ USES_FLAGS=USES_BOOT_DDR_DEBUG_USE_PM8916 $cmds
	else
	  sh build_8909.sh boot flashtools BUILD_ID=DAATANAA $cmds  
	  sh build_8909.sh boot jsdcc BUILD_ID=DAASANAA $cmds
      sh build_8909.sh deviceprogrammer_ddr BUILD_ID=DAADANAA USES_FLAGS=USES_DEVPRO_DDR $cmds
      sh build_8909.sh deviceprogrammer_ddr BUILD_ID=DAADANAA USES_FLAGS=USES_DEVPRO_DDR_SEC $cmds
      sh build_8909.sh deviceprogrammer_ddr BUILD_ID=DAADANAA USES_FLAGS=USES_DEVPRO_LITE $cmds
      sh build_8909.sh deviceprogrammer_ddr BUILD_ID=DAADANAA USES_FLAGS=USES_DEVPRO_LITE_SEC $cmds
	  sh build_8909.sh ddr_debug BUILD_ID=DAASANAA $cmds
	  sh build_8909.sh ddr_debug BUILD_ID=DAASANAA USES_FLAGS=USES_BOOT_DDR_DEBUG_USE_PM8916 $cmds
	fi
   else
   if [[ $BUILD_ID =~ DAATANAZ ]] 
	then
		sh build_8909.sh boot flashtools BUILD_ID=$BUILD_ID $cmds
	elif [[ $BUILD_ID =~ DAASANAZ ]]
	then
		sh build_8909.sh boot jsdcc BUILD_ID=$BUILD_ID $cmds
		sh build_8909.sh ddr_debug BUILD_ID=$BUILD_ID $cmds
		sh build_8909.sh ddr_debug BUILD_ID=$BUILD_ID USES_FLAGS=USES_BOOT_DDR_DEBUG_USE_PM8916 $cmds
	elif [[ $BUILD_ID =~ DAADANAZ ]]
	then
	    sh build_8909.sh deviceprogrammer_ddr BUILD_ID=$BUILD_ID $cmds,USES_DEVPRO_DDR
        sh build_8909.sh deviceprogrammer_ddr BUILD_ID=$BUILD_ID $cmds,USES_DEVPRO_DDR_SEC
        sh build_8909.sh deviceprogrammer_ddr BUILD_ID=$BUILD_ID $cmds,USES_DEVPRO_LITE
        sh build_8909.sh deviceprogrammer_ddr BUILD_ID=$BUILD_ID $cmds,USES_DEVPRO_LITE_SEC
    elif [[ $BUILD_ID =~ DAATANAA ]] 
	then
		sh build_8909.sh boot flashtools BUILD_ID=$BUILD_ID $cmds
	elif [[ $BUILD_ID =~ DAASANAA ]]
	then
		sh build_8909.sh  boot jsdcc BUILD_ID=$BUILD_ID $cmds
		sh build_8909.sh ddr_debug BUILD_ID=$BUILD_ID $cmds
		sh build_8909.sh ddr_debug BUILD_ID=$BUILD_ID USES_FLAGS=USES_BOOT_DDR_DEBUG_USE_PM8916 $cmds
	elif [[ $BUILD_ID =~ DAADANAA ]]
	then
		sh build_8909.sh deviceprogrammer_ddr BUILD_ID=$BUILD_ID USES_FLAGS=USES_DEVPRO_DDR $cmds
		sh build_8909.sh deviceprogrammer_ddr BUILD_ID=$BUILD_ID USES_FLAGS=USES_DEVPRO_DDR_SEC $cmds
		sh build_8909.sh deviceprogrammer_ddr BUILD_ID=$BUILD_ID USES_FLAGS=USES_DEVPRO_LITE $cmds
		sh build_8909.sh deviceprogrammer_ddr BUILD_ID=$BUILD_ID USES_FLAGS=USES_DEVPRO_LITE_SEC $cmds
	fi
  fi
fi

#8909w compilation on nand and emmc
if [ "$TARGET_FAMILY" = "8909w" ]
then
    if [ -z $BUILD_ID ] 
    then
	if [ $isProd -gt 0 ]
	then 
	  sh build_8909w.sh boot flashtools BUILD_ID=WAATANAZ $cmds  
	  sh build_8909w.sh boot jsdcc  BUILD_ID=WAASANAZ  $cmds
      sh build_8909w.sh deviceprogrammer_ddr BUILD_ID=WAADANAZ $cmds,USES_DEVPRO_DDR
      sh build_8909w.sh deviceprogrammer_ddr BUILD_ID=WAADANAZ $cmds,USES_DEVPRO_DDR_SEC
      sh build_8909w.sh deviceprogrammer_ddr BUILD_ID=WAADANAZ $cmds,USES_DEVPRO_LITE
      sh build_8909w.sh deviceprogrammer_ddr BUILD_ID=WAADANAZ $cmds,USES_DEVPRO_LITE_SEC
	  sh build_8909w.sh ddr_debug BUILD_ID=WAASANAZ $cmds
      sh build_8909w.sh ddr_debug BUILD_ID=WAASANAZ USES_FLAGS=USES_BOOT_DDR_DEBUG_USE_PM8916 $cmds
	else
	  sh build_8909w.sh boot flashtools BUILD_ID=WAATANAA $cmds  
	  sh build_8909w.sh boot jsdcc BUILD_ID=WAASANAA $cmds
      sh build_8909w.sh deviceprogrammer_ddr BUILD_ID=WAADANAA USES_FLAGS=USES_DEVPRO_DDR $cmds
      sh build_8909w.sh deviceprogrammer_ddr BUILD_ID=WAADANAA USES_FLAGS=USES_DEVPRO_DDR_SEC $cmds
      sh build_8909w.sh deviceprogrammer_ddr BUILD_ID=WAADANAA USES_FLAGS=USES_DEVPRO_LITE $cmds
      sh build_8909w.sh deviceprogrammer_ddr BUILD_ID=WAADANAA USES_FLAGS=USES_DEVPRO_LITE_SEC $cmds
	  sh build_8909w.sh ddr_debug BUILD_ID=WAASANAA $cmds
	  sh build_8909w.sh ddr_debug BUILD_ID=WAASANAA USES_FLAGS=USES_BOOT_DDR_DEBUG_USE_PM8916 $cmds
	fi
   else
   if [[ $BUILD_ID =~ WAATANAZ ]] 
	then
		sh build_8909w.sh boot flashtools BUILD_ID=$BUILD_ID $cmds
	elif [[ $BUILD_ID =~ WAASANAZ ]]
	then
		sh build_8909w.sh boot jsdcc BUILD_ID=$BUILD_ID $cmds
		sh build_8909w.sh ddr_debug BUILD_ID=$BUILD_ID $cmds
		sh build_8909w.sh ddr_debug BUILD_ID=$BUILD_ID USES_FLAGS=USES_BOOT_DDR_DEBUG_USE_PM8916 $cmds
	elif [[ $BUILD_ID =~ WAADANAZ ]]
	then
	    sh build_8909w.sh deviceprogrammer_ddr BUILD_ID=$BUILD_ID $cmds,USES_DEVPRO_DDR
        sh build_8909w.sh deviceprogrammer_ddr BUILD_ID=$BUILD_ID $cmds,USES_DEVPRO_DDR_SEC
        sh build_8909w.sh deviceprogrammer_ddr BUILD_ID=$BUILD_ID $cmds,USES_DEVPRO_LITE
        sh build_8909w.sh deviceprogrammer_ddr BUILD_ID=$BUILD_ID $cmds,USES_DEVPRO_LITE_SEC
    elif [[ $BUILD_ID =~ WAATANAA ]] 
	then
		sh build_8909w.sh boot flashtools BUILD_ID=$BUILD_ID $cmds
	elif [[ $BUILD_ID =~ WAASANAA ]]
	then
		sh build_8909w.sh  boot jsdcc BUILD_ID=$BUILD_ID $cmds
		sh build_8909w.sh ddr_debug BUILD_ID=$BUILD_ID $cmds
		sh build_8909w.sh ddr_debug BUILD_ID=$BUILD_ID USES_FLAGS=USES_BOOT_DDR_DEBUG_USE_PM8916 $cmds
	elif [[ $BUILD_ID =~ WAADANAA ]]
	then
		sh build_8909w.sh deviceprogrammer_ddr BUILD_ID=$BUILD_ID USES_FLAGS=USES_DEVPRO_DDR $cmds
		sh build_8909w.sh deviceprogrammer_ddr BUILD_ID=$BUILD_ID USES_FLAGS=USES_DEVPRO_DDR_SEC $cmds
		sh build_8909w.sh deviceprogrammer_ddr BUILD_ID=$BUILD_ID USES_FLAGS=USES_DEVPRO_LITE $cmds
		sh build_8909w.sh deviceprogrammer_ddr BUILD_ID=$BUILD_ID USES_FLAGS=USES_DEVPRO_LITE_SEC $cmds
	fi
  fi
fi