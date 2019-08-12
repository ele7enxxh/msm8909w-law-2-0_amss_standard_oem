
export BUILDSPEC=KLOCWORK
#===============================================================================
#Set up  Environment
#===============================================================================
unamestr=`uname`
if [ "$unamestr" = "Linux" ]; then

   # set up local environment
   export_armlmd_license()
   {
     # know where the host is located
     mdb $(echo `hostname`) return site > __temp.out

     # in boulder?
     grep -q "site: boulder" __temp.out
     if [ $? -eq 0 ]
     then
       echo "in boulder"
       export ARMLMD_LICENSE_FILE=8224@redcloud:8224@swiftly:7117@license-wan-arm1
     else
       # in rtp?
       grep -q "site: rtp" __temp.out
       if [ $? -eq 0 ]
       then
         echo "in rtp"
         export ARMLMD_LICENSE_FILE=8224@license-wan-rtp1
       else
         # in hyderabad?
         grep -q "site: hyderabad" __temp.out
         if [ $? -eq 0 ]
         then
           echo "in hyderabad"
           export ARMLMD_LICENSE_FILE=7117@license-hyd1:7117@license-hyd2:7117@license-hyd3
         else
           # in sandiego and others
           echo "in sandiego"
           export ARMLMD_LICENSE_FILE=7117@license-wan-arm1
         fi
       fi
     fi

     rm -f __temp.out
   }
# Set up compiler path
   ARM_COMPILER_PATH=/pkg/qct/software/arm/RVDS/5.01bld94/sw/debugger/configdb/Boards/ARM/Cortex-A8_RTSM/linux-pentium
   PYTHON_PATH=/pkg/qct/software/python/2.6.6/bin
   MAKE_PATH=/pkg/gnu/make/3.81/bin
   export ARMTOOLS=ARMCT5.01
   export ARMROOT=/pkg/qct/software/arm/RVDS/5.01bld94
   export ARMLIB=$ARMROOT/lib
   export ARMINCLUDE=$ARMROOT/include
   export ARMINC=$ARMINCLUDE
#   export ARMCONF=$ARMROOT/sw/Debugger/configdb/Boards/ARM/Cortex-A8_RTSM/linux-pentium
#   export ARMDLL=$ARMROOT/sw/Debugger/configdb/Boards/ARM/Cortex-A8_RTSM/linux-pentium
   export ARMBIN=$ARMROOT/bin64

   # PYTHONBIN specifies version that will be added to PATH
   if [ -z ${PYTHONBIN} ]; then
      # Set default version for legacy builds
      PYTHONBIN=${PYTHON_PATH}
   fi

   export PATH=$MAKE_PATH:$PYTHONBIN:$ARM_COMPILER_PATH:$PATH
   export ARMHOME=$ARMROOT
   export_armlmd_license

fi

#==============================================================================
# Dump environment to stdout so that calling scripts can read it.
#==============================================================================
env
