#==============================================================================
# SCONS Tool for Target-Specific Build Parameters.
#
# Copyright (c) 2009-2010 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#==============================================================================
#==============================================================================
#
#                        EDIT HISTORY FOR MODULE
#
# $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/build/ms/tbc_modem.py#1 $
#
# when       who    what, where, why
# -------    ---    -------------------------------------------
# 09/03/10   dlb'   Initial Revision
#==============================================================================


#=====================================================================
# Global data
#=====================================================================

image_env = None


#=====================================================================
# exists()
#=====================================================================
def exists(env):
    return env.Detect('tbc_modem')
# End of exists()


#=====================================================================
# generate()
#=====================================================================
def generate(env):

    #-------------------------------------------
    # Save our image-level environment.
    #-------------------------------------------
    global image_env
    image_env = env

    #-------------------------------------------
    # Add required APIs
    #-------------------------------------------
    if env.GetUsesFlag('USES_MODEM_RCINIT'):
        env.Replace(MODEM_DRIVER = 'RCINIT_GROUP_3')
        env.Replace(MODEM_PROTOCOL = 'RCINIT_GROUP_4')
        env.Replace(MODEM_UPPERLAYER = 'RCINIT_GROUP_5')
        env.Replace( MODEM_CPU_AFFINITY = 'REX_ANY_CPU_AFFINITY_MASK' )
        env.Replace( MODEM_NAS_CPU_AFFINITY = 'REX_ANY_CPU_AFFINITY_MASK' )
        env.Replace( MODEM_TDSCDMA_CPU_AFFINITY = 'REX_ANY_CPU_AFFINITY_MASK' )	   
      
    #-------------------------------------------
    # Define global APIs
    #-------------------------------------------

    env.AddMethod(append_linker_path, "AppendLinkerPath")

# End of generate()


#-------------------------------------------------------------------------------
# Return a list of search paths in which config files might be located
#
def append_linker_path(env, new_linker_path):

    image_env.Append(LFLAGS = "-L " + new_linker_path + ' ')

# End of append_linker_path()
