#===============================================================================
#
# Utility script for the AVS module
#
# GENERAL DESCRIPTION
#    A generic python script for handling any miscellenous AVS requirements
#     
#
# Copyright (c) 2014 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/build/avs_utils.py#1 $
#  $DateTime: 2016/12/13 07:58:24 $
#  $Author: mplcsds1 $
#  $Change: 11985135 $
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 05/26/14   rr      Obtain the max_threads value from the cust_config.xml
#
#===============================================================================

import re
import string
import os
import sys
from xml.dom import minidom
def avs_utils(env):

#To support building multiple variants in parallel, we need to leave the config files in the config component, and not copy them to the build\ms directory.
    if env.PathExists('${BUILD_ROOT}/build/scripts/build_utils.py'):
        env.LoadToolScript('build_utils', toolpath = ['${BUILD_ROOT}/build/scripts'])
        CUST_CONFIG_PATH = env.FindConfigFiles('cust_config.xml')[0]
              
    else:
        CUST_CONFIG_PATH="../../build/ms/cust_config.xml"
       
    xmldoc = minidom.parse(CUST_CONFIG_PATH)
    itemlist = xmldoc.getElementsByTagName('max_threads') 
    for s in itemlist :
        os.environ['MAX_THREADS_FROM_XML']=str(s.attributes['value'].value)
        print "From avs_utils.py MAX_THREADS value imported is %s " %s.attributes['value'].value
    return
