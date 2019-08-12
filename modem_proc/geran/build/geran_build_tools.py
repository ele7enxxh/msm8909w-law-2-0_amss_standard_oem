#===============================================================================
#
#       G E R A N    C O M M O N   S C O N S   C O N F I G U R A T I O N
#
# GENERAL DESCRIPTION
#    General SCons magic shared by all GERAN components
#
# Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/build/geran_build_tools.py#1 $
#  $DateTime: 2016/12/13 08:00:21 $
#
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 20130204   tjw     New file factored out of component SCons file
#
#===============================================================================

from glob import glob
from os.path import join, basename, exists
import os, sys

buildTool_o = None # failed attempt to only create one object

class buildTool:
  def __init__(self, env):
    class dummyDevObject:
      """
      Replacement for class used by internal development builds
      """
      def warningsErrors(self, env, component):
        pass
        return env
      def setup_static_analysis(self, unit):
        pass
      def devFolder(self):
        return False
      def apiCheck(self, env):
        pass
      def externsCheck(self):
        pass
      def setup_show_headers_used(self):
        pass
      def internal_build_variation(self, variation):
        return False

    # this may be imported from anywhere so work out THIS file's path
    _path = os.path.abspath(__file__)
    dir_path = os.path.dirname(_path)

    _internal_build_tools_folder = os.path.join(dir_path, '_qualcomm')
    self.devObj = dummyDevObject()  # use the dummy unless...
    if os.path.exists(_internal_build_tools_folder):
      sys.path.append(_internal_build_tools_folder)
      try:
        from internal_build_tools import devObject, geranBuild
        if geranBuild(env): # (the internal one is only available to GERAN development builds)
          self.devObj = devObject(env)  #...the internal one is available
      except ImportError: # internal_build_tools.py is not present
        pass
      except: #  or other error
        raise

  def apiCheck(self, env):
    self.devObj.apiCheck(env)
  def devFolder(self):
    return self.devObj.devFolder()
  def setup_static_analysis(self, unit):
    self.devObj.setup_static_analysis(unit)
  def externsCheck(self):
    self.devObj.externsCheck()
  def setup_show_headers_used(self):
    self.devObj.setup_show_headers_used()
  def internal_build_variation(self, variation):
    return self.devObj.internal_build_variation(variation)
  def warningsErrors(self, env, component):
    return self.devObj.warningsErrors(env, component)

#-------------------------------------------------------------------------------

def build_variation(env, variation):
  """
  This function handles all build variations found in different baselines and
  build environments (compilers).  This is the list of variations:
  'FEATURE_GSM_CXM_L3_STATE'
  'FEATURE_IDLE_DRX_SCALING'
  'MSGRIF_SCONS_USES_CSV'
  'MVS_REMOVED'
  'GERAN_INTERNAL_APIS'
  'MSDEV_COMPILER'
  'LLVM_COMPILER'
  'LLVM_COMPILER_WARNING_LEVEL'
  'COMPILER_WARNINGS_AS_ERRORS'
  'NUMBER_OF_SUBS'
  """
  #-------------------------------------------------------------------------------
  def _PL_check(env, PL_list):
    """
    Return True if environment variable 'PRODUCT_LINE' matches any of 'PL_list'
    """
    try:
      for PL in PL_list:
        if env['PRODUCT_LINE'].find(PL) != -1:
          if debug:
            env.PrintDebugInfo('geran', 'GeRaN %s matched %s ' % (env['PRODUCT_LINE'], PL))
          return True
      if env['PRODUCT_LINE'] == '':
        env.PrintError('!!!! ERROR: Environment variable PRODUCT_LINE is blank. GERAN build errors probable !!!!')
        env.PrintError('!!!! ERROR: QUITTING BUILD !!!!')
        sys.exit(98)
      if debug:
        env.PrintDebugInfo('geran', 'GeRaN %s did not match %s ' % (env['PRODUCT_LINE'], PL))
    except:
      env.PrintError('!!!! ERROR: Environment variable PRODUCT_LINE is not defined. GERAN build errors probable !!!!')
      env.PrintError('!!!! ERROR: QUITTING BUILD !!!!')
      sys.exit(99)
    return False
  #-------------------------------------------------------------------------------
  def _version_tuple(v):
    """
    convert version number x.y.z into a tuple so it can be compared
    """
    try:  # To cope with 5.1_hw_errata.2
      result = tuple(map(int, (v.split('.'))))
    except:
      result = [5, 1, 2]
    return result
  #-------------------------------------------------------------------------------

  # If the build command line USES switch SCONS_DEBUG_GERAN is defined
  debug = ('SCONS_DEBUG_GERAN' in env )

  if _PL_check(env, ['MPSS.NI.', \
                     'MPSS.DI.1.0', 'MPSS.DI.2.0', 'MPSS.DI.2.1', \
                     'MPSS.TR.1', 'MPSS.TR.2', 'MPSS.TR.3']):
    env.PrintError('!!!! ERROR: This Product Line is not supported by this version of the GERAN build files. !!!!')
    env.PrintError('!!!! ERROR: GERAN build errors probable !!!!')
    env.PrintError('!!!! ERROR: QUITTING BUILD !!!!')
    sys.exit(99)

  elif variation == 'FEATURE_GSM_CXM_L3_STATE':
    result = False  # It's defined in the command line for TH.1.1
    # _PL_check(env, ['MPSS.TH.1.1'])  # Specifically: not required for TH.1.0.1
  elif variation == 'FEATURE_IDLE_DRX_SCALING':
    result = env.get('CHIPSET') == "mdm9607"

  elif variation == 'MSGRIF_SCONS_USES_CSV':
    result = _PL_check(env, ['MPSS.TH.', 'MPSS.AT.'])

  elif variation == 'MVS_REMOVED':  # mvs API no longer in include path
    result = False # ghdi_exp_v.h and wl2api.h still include it
    # _PL_check(env, ['MPSS.TH.2', 'MPSS.TA.2', 'MPSS.AT.'])  

  elif variation == 'GERAN_INTERNAL_APIS':
    # Protocol and L1 have APIs for each other
    result = env.PathExists('${GERAN_ROOT}/gprotocol')

  elif variation == 'MSDEV_COMPILER':
    # Building with the Visual C compiler vs. some flavour of gcc / LLVM
    result = (env['PROC'] in ['msvc'] )

  elif variation == 'LLVM_COMPILER':
    # Building with the LLVM compiler
    if build_variation(env, 'MSDEV_COMPILER') or \
      env['PROC'] in ['linux']: # It's a Linux MOB build
      result = False
    else:
      result = (os.environ.get('HEXAGON_RTOS_RELEASE')[0] >= '6' )

  elif variation == 'COMPILER_WARNINGS_AS_ERRORS':
    # Report warnings as errors
    # When using the GNU compiler
    if build_variation(env, 'MSDEV_COMPILER'):
      result = False
    # DPM which is using GNU compiler on files written under LLVM
    elif _PL_check(env, ['MPSS.DPM.']):
      result = False
    elif build_variation(env, 'LLVM_COMPILER'):
      # (except LLVM toolchain (6.x.x) during migration)
      result = False
    else:
      # Ensure that continuous build processes will reject code with warnings
      result = True

  elif variation == 'NUMBER_OF_SUBS':
    # Return the number of subscriptions (Stds_rrc) supported
    subs = 1
    if 'USES_TSTS' in env:
      subs = 3
    elif 'USES_DUAL_SIM' in env:
      subs = 2
    result = subs

  else:
    # see if it's an internal build variation
    global buildTool_o
    if buildTool_o == None:
      buildTool_o = buildTool(env)
    return buildTool_o.internal_build_variation(variation)

  if debug:
    env.PrintDebugInfo('geran', 'GeRaN %s: %s' % (variation, str(result)))
  return result

#-------------------------------------------------------------------------------

def set_up_component_build_config(
  env,
  SU_NAME,
  UNIT_NAME,
  SRCPATH,
  SRC_FILES,
  PRIVATE_SRC_FILES,
  REQUIRED_SU_APIS,
  IMAGES,
  MSG_BT_SSID_DFLT = None,                # default is 'MSG_SSID_GSM_GPRS_' + UNIT_NAME
  DELIVER_SOURCE = True,                  # False => deliver as a binary to some customers
  PUBLISHED_HDR_FILES = [],               # List of the files not to be cleaned by cleanpack
  GL1_TDRRC_SRC_FILES = [],               # List of the L1 files to be delivered to some customers
  UNIT_TEST = False,
  CMI4_VIOLATIONS = [],                   # Work round CMI4 rules where necessary for unit test
  debug = False
):
  if not debug:
    # if it's not set locally, see if it's set globally
    debug =  build_variation(env, 'SCONS_DEBUG')

  if debug:
    env.PrintDebugInfo('geran', 'GeRaN SU_NAME %s' %                  SU_NAME)
    env.PrintDebugInfo('geran', 'GeRaN UNIT_NAME %s' %                UNIT_NAME)
    env.PrintDebugInfo('geran', 'GeRaN SRCPATH %s' %                  SRCPATH)
    env.PrintDebugInfo('geran', 'GeRaN SRC_FILES %s' %                SRC_FILES)
    env.PrintDebugInfo('geran', 'GeRaN PRIVATE_SRC_FILES %s' %        PRIVATE_SRC_FILES)
    env.PrintDebugInfo('geran', 'GeRaN REQUIRED_SU_APIS %s' %         REQUIRED_SU_APIS)
    env.PrintDebugInfo('geran', 'GeRaN IMAGES %s' %                   IMAGES)
    env.PrintDebugInfo('geran', 'GeRaN MSG_BT_SSID_DFLT %s' %         MSG_BT_SSID_DFLT)
    env.PrintDebugInfo('geran', 'GeRaN DELIVER_SOURCE %s' %           DELIVER_SOURCE)
    env.PrintDebugInfo('geran', 'GeRaN PUBLISHED_HDR_FILES %s' %      PUBLISHED_HDR_FILES)
    env.PrintDebugInfo('geran', 'GeRaN GL1_TDRRC_SRC_FILES %s' %      GL1_TDRRC_SRC_FILES)
    env.PrintDebugInfo('geran', 'GeRaN UNIT_TEST %s' %                UNIT_TEST)
    env.PrintDebugInfo('geran', 'GeRaN CMI4_VIOLATIONS %s' %          CMI4_VIOLATIONS)

  #-------------------------------------------------------------------------------
  # Setup source PATH
  #-------------------------------------------------------------------------------
  env.VariantDir('${BUILDPATH}', SRCPATH, duplicate=0)

  #-------------------------------------------------------------------------------
  # Set MSG_BT_SSID_DFLT for legacy MSG macros
  #-------------------------------------------------------------------------------
  if MSG_BT_SSID_DFLT == None:  # Not specified => follows the naming convention
    env.Append(CPPDEFINES = [
       'MSG_BT_SSID_DFLT=MSG_SSID_GSM_GPRS_' + UNIT_NAME,
    ])
  else:
    env.Append(CPPDEFINES = [
       'MSG_BT_SSID_DFLT=' + MSG_BT_SSID_DFLT,
    ])

  """
  API terminology:
  Public        visible to other AUs
  Restricted    visible to other SUs
  Protected     visible only within the SU
  """

  #-------------------------------------------------------------------------------
  # Necessary Public APIs
  #-------------------------------------------------------------------------------
  if UNIT_TEST:
    ################################################################
    # Work round CMI4 rules where necessary for unit test
    ################################################################
    # Handle source trees that have modem/ and those that don't
    if env.PathExists('${INC_ROOT}/modem/geran'):
      modemRoot = '${INC_ROOT}/modem'
    else:
      modemRoot = '${INC_ROOT}'
    for incpath in CMI4_VIOLATIONS:
      env.PublishPrivateApi('VIOLATIONS', [join(modemRoot, incpath)])


  if debug or 'MOB_GERAN' in env:
    env.RequirePublicApi(['QTF'], area='MOB') # QTF comes from MOB area in no-modem-dir
    env.RequireRestrictedApi(['QTF', 'MCS_OFFTARGET']) # older MOBs
    env.RequireProtectedApi(['GL1TEST'])
    env.RequireProtectedApi(['GMDSP6TEST'])
    env.RequireProtectedApi(['GTEST'])

  # Need to get access to SU's own Public headers
  env.RequireRestrictedApi(SU_NAME)

  if UNIT_NAME != 'API':
    # The API must not access the SU's internal cust files
    # Require these for all components within the SU as they all have compile time
    # variation.  In the future this can be pushed down to the component .scons
    # files only for components that have compile time variation.
    env.RequireProtectedApi(['GERAN'])

    # The API must not access the SU's internal inc files either
    if build_variation(env, 'GERAN_INTERNAL_APIS'):
      # separate API for L1 and Protocol
      L1_UNITS =       ['GDRIVERS', 'GL1', 'GMDSP', 'GPLT']
      PROTOCOL_UNITS = ['GCOMMON', 'GDIAG', 'GL2', 'GLLC', 'GMAC', 'GRLC', 'GRR', 'GSNDCP']

      if UNIT_NAME in L1_UNITS:
        env.RequireProtectedApi([
        'GDRIVERS',
        'GTRACER_IDS',
        'GL1',
        'GMDSP',
        'GPLT',
        'GMSGRIF',
        'GERAN_COMMON',
        'GERAN_PROTOCOL',
        'GERAN_L1'
        ]
        )

      if UNIT_NAME in PROTOCOL_UNITS :
        env.RequireProtectedApi([
        'GCOMMON',
        'GDIAG',
        'GL2',
        'GLLC',
        'GMAC',
        'GRLC',
        'GRR',
        'GSNDCP',
        'GPLT',
        'GERAN_COMMON',
        'GERAN_PROTOCOL',
        'GERAN_L1'
        ]
        )
    else:
        # All GERAN units access all internal header files
        env.RequireProtectedApi([
        'GCOMMON',
        'GDIAG',
        'GDRIVERS',
        'GTRACER_IDS',
        'GL1',
        'GL2',
        'GLLC',
        'GMAC',
        'GMDSP',
        'GRLC',
        'GRR',
        'GSNDCP',
        'GPLT',
        'GMSGRIF']
        )

  # Now add the include paths outside GERAN
  # The order of APIs in the geran_public_api_list determines the order the paths
  # are listed on the build command line.
  # To optimize build times the API list is sorted in descending order of use.

  geran_public_api_list = [
        # api_area              api_name
        ('CORE',                'KERNEL'),
        ('GERAN',               'GERAN'),
        ('MMCP',                'MMCP'),
        ('CORE',                'DAL'),
        ('MCS',                 'MCS'),
        ('RFA',                 'COMMON'),
        ('CORE',                'SYSTEMDRIVERS'),
        ('LTE',                 'LTE'),
        ('MMCP',                'PUBLIC'),
        ('CORE',                'MEMORY'),
        ('CORE',                'DEBUGTOOLS'),
        ('RFA',                 'TDSCDMA'),
        ('RFA',                 'CDMA'),
        ('RFA',                 'MEAS'),
        ('RFA',                 'GSM'),
        #('AVS',                 'MVS'),
        ('RFA',                 'WCDMA'),
        ('TDSCDMA',             'TDSCDMA'),
        ('CORE',                'MPROC'),
        ('WCDMA',               'WCDMA'),
        ('CORE',                'POWER'),
        ('ONEX',                'PUBLIC'),
        ('CORE',                'BUSES'),
        ('UIM',                 'UIM'),
        ('UIM',                 'PUBLIC'),
        ('RFA',                 'GNSS'),
        ('CORE',                'SERVICES'),
        ('GERAN',               'PUBLIC'),
        ('RFA',                 'LTE'),
        ('WCDMA',               'PUBLIC'),
        ('FW',                  'RF'),
        ('RFA',                 'LM'),
        ('RFLM',                'DTR'),
        ('FW',                  'GERAN'),
        ('FW',                  'TARGET'),
        ('FW_CCS',              'FW_CCS'),
        ('FW',                  'COMMON'),
        ('FW_GERAN',            'FW_GERAN'),
        ('MPOWER',              'MPOWER'),
        ('MCS',                 'PUBLIC'),
        ('DATAMODEM',           'PUBLIC'),
        ('GPS',                 'GPS'),
        ('FW',                  'WCDMA'),
        ('FW_WCDMA',            'FW_WCDMA'),
        ('UTILS',               'OSYS'),
        ('DATAMODEM',           'DATAMODEM'),
        ('CORE',                'STORAGE'),
        ('UTILS',               'A2'),
        ('AVS',                 'VS'),
        ('AVS',                 'MMUTILS'),
        ('CORE',                'SECUREMSM'),
        ('CORE',                'DEBUGTRACE'),
        ('MCFG',                'MCFG'),
        ('RFLM',                'RFLM'),
        ('UTILS',               'QSH'),
      ]

  for api_area,api_name in geran_public_api_list:
      env.RequirePublicApi([api_name], area=api_area)
  if not build_variation(env, 'MVS_REMOVED'):
      env.RequirePublicApi(['MVS'], area='AVS')
      if debug:
        env.PrintDebugInfo('geran', 'MVS API added')
      else:
        env.PrintDebugInfo('geran', 'MVS API omitted')
  #-------------------------------------------------------------------------------
  # Generate the library and add to an image
  #-------------------------------------------------------------------------------

  no_assembler_files = True

  if SRC_FILES == '*.c' or SRC_FILES == '*.c*':
    # Construct the list of source files by looking for *.c
    SOURCES = ['${BUILDPATH}/' + basename(fname)
      for fname in glob(join(env.subst(SRCPATH), SRC_FILES))]
    if debug:
      env.PrintDebugInfo('geran', 'GeRaN SOURCES')
      env.PrintDebugInfo('geran',  SOURCES)
  else:
    # Construct the list of source files from the given list
    SOURCES = []
    for fname in SRC_FILES:
      if fname[-2:] != '.c':
        no_assembler_files = False
      if exists(join(env.subst(SRCPATH), fname)):
        SOURCES.append('${BUILDPATH}/' + basename(fname))

  #---------------------------------------------------------------------------
  # rfm_types.h is included indirectly 2700 times by GERAN but contains nothing
  # of use. Avoid it by forcing the include of our local pre-compiled header
  if build_variation(env, 'LLVM_COMPILER') and \
     no_assembler_files:
    # this may be imported from anywhere so work out THIS file's path
    _path = os.path.abspath(__file__)
    dir_path = os.path.dirname(_path)
    _rfm_types_h = os.path.join(dir_path, '..', 'gdrivers', 'inc', 'rfm_types.h')

    if os.path.exists(_rfm_types_h):
      env.Append(CFLAGS = '-include rfm_types.h')
      env.PrintError('>>>>> BYPASSING RFM_TYPES.h >>>>>')

  global buildTool_o
  if buildTool_o == None:
    buildTool_o = buildTool(env)

  env = buildTool_o.warningsErrors(env, UNIT_NAME.lower())

  if UNIT_TEST:
    # UT's must be added as objects
    env.AddObject(['MOB_GERAN_UT'], SOURCES)
  else:
    # Add our library to the ModemApps image
    """
    All the source files in this unit can be divided up into sets when
    considering what is shipped to customers:
    S: all C (CPP) and assembler files
    I: all header files in the inc/ folder
    L: all (local) header files in the src/ folder
    P: private source files (not usually shipped)
    H: header files (in inc/) that are always shipped
    T: GL1 / TDRRC files (shipped in some cases)

    In addition DELIVER_SOURCE controls whether S is shipped, also
    exceptions can override the decision not to ship.

    Restricted Source Packages

    The hierarchy of the USES flags exceptions is
    CUSTOMER_GENERATE_LIBS                     Build binaries to copy to pack builds
                                                 Also used to deliver all files
                                                 (even PRIVATE) as source
    COMPILE_L1_OPT_AC_PROTECTED_LIBS           Deliver all files (even PRIVATE) as source
    COMPILE_GL1_TDRRC_CUSTOM_PROTECTED_LIBS    Deliver just the GL1_TDRRC_SRC_FILES L1 files
    COMPILE_TDSCDMA_AND_ALL_L1_PROTECTED_LIBS  No L1 files

    The process is:
    S = S - P - T
    if H
      I = I - H           # I is now the headers NOT shipped

    if DELIVER_SOURCE
      AddLibrary(S)
    else
      AddBinaryLibrary(S) # do not ship, except COMPILE_TDSCDMA_AND_ALL_L1_PROTECTED_LIBS
                                             or COMPILE_L1_OPT_AC_PROTECTED_LIBS
      CleanPack(L)        # do not ship, except COMPILE_TDSCDMA_AND_ALL_L1_PROTECTED_LIBS
                          #                  or COMPILE_L1_OPT_AC_PROTECTED_LIBS
    if P
      AddBinaryLibrary(P) # do not ship, except CUSTOMER_GENERATE_LIBS
                                             or COMPILE_L1_OPT_AC_PROTECTED_LIBS
    if T
      AddBinaryLibrary(T) # do not ship, except COMPILE_TDS_RRC_EXT_PROTECTED_LIBS
                                             or COMPILE_TDSCDMA_AND_ALL_L1_PROTECTED_LIBS
                                             or COMPILE_L1_OPT_AC_PROTECTED_LIBS
    if I
      CleanPack(I)        # do not ship, except COMPILE_TDSCDMA_AND_ALL_L1_PROTECTED_LIBS
                                             or COMPILE_GL1_TDRRC_CUSTOM_PROTECTED_LIBS
    """

    PRIVATE_SOURCES = []
    TDS_RRC_SOURCES = []
    HEADERS = []

    # S = S - P - T
    # P
    if PRIVATE_SRC_FILES != []:
      # PRIVATE_SRC_FILES *are* delivered in some cases
      for private_file in PRIVATE_SRC_FILES:
        private_fname = '${BUILDPATH}/' + basename(private_file)
        if SOURCES.count(private_fname) > 0:    # the private source file is in the list
          SOURCES.remove(private_fname)

      # Construct the list of private source files from the given list
      for fname in PRIVATE_SRC_FILES:
        PRIVATE_SOURCES.append('${BUILDPATH}/' + basename(fname))

    # T
    if GL1_TDRRC_SRC_FILES != []:
      # GL1_TDRRC_SRC_FILES *are* delivered in some cases
      for tds_rrc_file in GL1_TDRRC_SRC_FILES:
        tds_rrc_fname = '${BUILDPATH}/' + basename(tds_rrc_file)
        if SOURCES.count(tds_rrc_fname) > 0:    # the tds_rrc source file is in the list
          SOURCES.remove(tds_rrc_fname)

      # Construct the list of TDC_RRC source files from the given list
      for fname in GL1_TDRRC_SRC_FILES:
        TDS_RRC_SOURCES.append('${BUILDPATH}/' + basename(fname))

    # L
    # Construct the list of local header files by looking for *.h - are delivered sometimes
    LOCAL_HEADERS = ['${BUILDPATH}/' + basename(fname)
      for fname in glob(join(env.subst(SRCPATH), '*.h'))]
    if debug:
      env.PrintDebugInfo('geran', 'GeRaN LOCAL_HEADERS')
      env.PrintDebugInfo('geran', LOCAL_HEADERS)

    # if H
    if PUBLISHED_HDR_FILES != []:
      # Construct the list of inc header files by looking for *.h
      HEADERS = ['${BUILDPATH}/' + basename(fname)
        for fname in glob('../inc/*.h')]
      # and removing the "white list" files that *are* always published
      # (using set subtraction)
      PUBLISHED_HEADERS = ['${BUILDPATH}/' + basename(fname)
        for fname in PUBLISHED_HDR_FILES]
      # I  = I - H
      HDR_SET = set(HEADERS) - set(PUBLISHED_HEADERS)
      HEADERS = list(HDR_SET)
      if debug:
        env.PrintDebugInfo('geran', 'GeRaN HEADERS')
        env.PrintDebugInfo('geran', HEADERS)

    if DELIVER_SOURCE:
      # AddLibrary(S)
      # Source for this component is delivered to customer
      # except for PRIVATE files
      if len(SOURCES):  # if there is any
        env.AddLibrary( IMAGES, join('${BUILDPATH}', '%s_%s' % (SU_NAME, UNIT_NAME)), [SOURCES] )
    else:
      # This component is delivered as a binary by default, with exceptions.
      # AddBinaryLibrary(S) # do not ship, except COMPILE_TDSCDMA_AND_ALL_L1_PROTECTED_LIBS
      #                                        or COMPILE_L1_OPT_AC_PROTECTED_LIBS
      # CleanPack(L)        # do not ship, except COMPILE_TDSCDMA_AND_ALL_L1_PROTECTED_LIBS
      #                                        or COMPILE_L1_OPT_AC_PROTECTED_LIBS
      env.AddBinaryLibrary( IMAGES, join('${BUILDPATH}', '%s_%s' % (SU_NAME, UNIT_NAME)), [SOURCES],
                            pack_exception=['USES_COMPILE_TDSCDMA_AND_ALL_L1_PROTECTED_LIBS',
                                            'USES_COMPILE_L1_OPT_AC_PROTECTED_LIBS'] )

      env.CleanPack("CLEANPACK_TARGET", LOCAL_HEADERS,
                    pack_exception=['USES_COMPILE_TDSCDMA_AND_ALL_L1_PROTECTED_LIBS',
                                    'USES_COMPILE_L1_OPT_AC_PROTECTED_LIBS'])


    # if P
    if PRIVATE_SOURCES != []:
      #   AddBinaryLibrary(P) # do not ship, except CUSTOMER_GENERATE_LIBS
      #                                          or COMPILE_L1_OPT_AC_PROTECTED_LIBS
      env_private = env.Clone()
      env_private.Append(CPPDEFINES = [
        'FEATURE_LIBRARY_ONLY'
      ])
      env_private.AddBinaryLibrary( IMAGES, join('${BUILDPATH}', 'lib_%s_%s' %
                                    (SU_NAME, UNIT_NAME)), [PRIVATE_SOURCES],
                                    pack_exception=['USES_CUSTOMER_GENERATE_LIBS',
                                                    'USES_COMPILE_L1_OPT_AC_PROTECTED_LIBS'] )

    # if T
    if TDS_RRC_SOURCES != []:
      #   AddBinaryLibrary(T) # do not ship, except COMPILE_TDS_RRC_EXT_PROTECTED_LIBS
      #                                          or COMPILE_TDSCDMA_AND_ALL_L1_PROTECTED_LIBS
      #                                          or COMPILE_L1_OPT_AC_PROTECTED_LIBS
      env.AddBinaryLibrary( IMAGES, join('${BUILDPATH}', 'tds_rrc_%s_%s' % (SU_NAME, UNIT_NAME)), [TDS_RRC_SOURCES],
                            pack_exception=['USES_COMPILE_GL1_TDRRC_CUSTOM_PROTECTED_LIBS',
                                            'USES_COMPILE_TDSCDMA_AND_ALL_L1_PROTECTED_LIBS',
                                            'USES_COMPILE_L1_OPT_AC_PROTECTED_LIBS'] )

    # if H
    if HEADERS != []:
      env.CleanPack("CLEANPACK_TARGET", HEADERS,
                    pack_exception=['USES_COMPILE_TDSCDMA_AND_ALL_L1_PROTECTED_LIBS',
                                    'USES_COMPILE_GL1_TDRRC_CUSTOM_PROTECTED_LIBS'])

################################################################
# Set up the Run Control Initialisation (RCInit) data to define
# the task(s) that controls this unit (if there is one).
################################################################

def set_up_component_tasks(
  env,
  IMAGES,
  INITFUNC_PARAMETER_DICTIONARIES = None,   # old style explicit list of dictionaries
  INITTASK_PARAMETER_DICTIONARIES = None    # list of dictionaries
  ):
  #-------------------------------------------------------------------------------
  # Generate the RCInit tables
  #-------------------------------------------------------------------------------

  if 'USES_MODEM_RCINIT' in env:
    num_subs = build_variation(env, 'NUMBER_OF_SUBS')

    sub = 1
    for INITFUNC_PARAMETER_DICTIONARY in INITFUNC_PARAMETER_DICTIONARIES:
      # init function descriptor : add to specific build products
      env.AddRCInitFunc(IMAGES, INITFUNC_PARAMETER_DICTIONARY)
      sub += 1
      if sub > num_subs:  # Only do as many as there are subscriptions in this build.
        break

    sub = 1
    for INITTASK_PARAMETER_DICTIONARY in INITTASK_PARAMETER_DICTIONARIES:
      # task descriptor : add to specific build products
      env.AddRCInitTask(IMAGES, INITTASK_PARAMETER_DICTIONARY)
      sub += 1
      if sub > num_subs:
        break

