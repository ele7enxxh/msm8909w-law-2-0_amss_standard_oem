#==========================================================================
#
#  QC SCons overrides file for hexagon builder:
#
#  This file is used to temporarily override Hexagon compiler options 
#  during the transition to LLVM for MPSS SI's.
#
#  Name this file hexagon_product.py and place it in config/default.  The
#  environment variable SCONS_BUILD_SCRIPTS_PRODUCT_ROOT must be set to 
#  config/default.
#
#==========================================================================

#--------------------------------------------------------------------------
# Hooks for SCons
#--------------------------------------------------------------------------

def exists(env):
   return env.Detect('hexagon_product')

def generate(env):

   ENDFLAGS = ''
   env.Prepend(HEXAGON_COMPILE_CMD = ENDFLAGS)

