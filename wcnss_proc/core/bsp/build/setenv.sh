
#!/bin/bash

#===============================================================================
#Set up  Environment 
#===============================================================================
unamestr=`uname`
if [ "$unamestr" = "Linux" ] ; then

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

   ARM_COMPILER_PATH=/pkg/qct/software/arm/RVDS/5.01bld43
   PYTHON_PATH=/pkg/qct/software/python/2.6.5/bin
   MAKE_PATH=/pkg/gnu/make/3.81/bin
   export ARMTOOLS=ARMCT501
   export ARMROOT=/pkg/qct/software/arm/RVDS/5.01bld43
   export ARMCC5LIB=$ARMROOT/lib
   export ARMCC5INC=$ARMROOT/include
   export ARMCC5BIN=$ARMROOT/bin
   export PATH=$MAKE_PATH:$PYTHON_PATH:$ARM_COMPILER_PATH:$PATH
   export ARMHOME=$ARMROOT
   export_armlmd_license

fi

#===============================================================================
# Set target enviroment
#===============================================================================
