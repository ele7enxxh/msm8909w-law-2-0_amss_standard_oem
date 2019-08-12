import os

def normalize(rule):
  if(os.path.normcase(rule) != rule):
    # This is a dos filesystem, replace "/" with "\\", and lowercase the filenames
    rule = rule.lower()
    rule = rule.replace(r'/', r'\\')
  return rule
  
rules = [
  # keep build logs and launch scripts around, because they're pretty handy
  
  # save target build script
  r'build/build_'+MSM_ID+r'.+$',
  
  # save common build script files
  r'build/build[^0-9/]+$',
  r'build/packrat.+$',
  r'build/rpmpack.+$',
  
  # save build logs
  r'build/build-log.+$',
  
  # Need for signed image generation
  r'build/msm_jtag_mapping.txt',
  
  # store list of files stored by whitelist for debugging
  r'build/whitelist_log.+$',
  
  # tools that have bad build rules, so scons doesn't actually know about the dependency
  r'build/ms/target.builds$',
  r'core/dal/config/rpm/.*$',
  r'core/systemdrivers/clock/config/.*\.xml$',
  r'core/products/scripts/.*$',
  r'build/build/rpm/rpm/AAAAANAAR/qc_version.+$',
  r'build/build/rpm/rpm/AAAAANAAR/oem_version.+$',
  r'build/manifest.xml',

  # SCons can use different internal paths when in customer environments, so
  # preserve all of the SCons distribution internally.
  r'tools/build/scons/.*$',
  
  # FIXME: I think the devcfg build rules are broken
  #r'core/dal/config/.*$',
  
  # FIXME: #define'd header names don't scan right
  # Keep only the headers for the current target
  r'core/power/mpm/hal/source/'+MSM_ID+r'/HALmpmint.*.h$',
  # Keep common headers
  r'core/power/mpm/hal/source/common/HALmpmint.*.h$',
  r'core/power/mpm/hal/source/MPM_V4/HALmpmint.*.h$',
  
  # scripts for debugging
  r'core/bsp/rpm/scripts.*$',
  r'core/power/npa/scripts/NPADump.cmm',
  r'core/power/npa/scripts/rpm_npadump.cmm',
  r'core/power/ulog/scripts/ULogDump.cmm',
  r'core/power/ulog/scripts/rpm_ulogdump.cmm',
  r'core/power/rpm/debug/scripts/.*$',
  # Keep all pmic scripts as we don't know what target+pmic we have
  r'core/systemdrivers/pmic/scripts/[^/]+$',
  r'core/systemdrivers/pmic/scripts/.+/[^/]+$',
  # Keep only the debugtrace scripts for the current target
  r'core/debugtrace/tools/scripts/'+MSM_ID+r'/[^/]+$',
  r'core/debugtrace/tools/scripts/'+MSM_ID+r'/.+/[^/]+$',
  # Keep specific Clock debug scripts
  r'core/systemdrivers/clock/scripts/...'+MSM_ID+r'/ClockTestDict\.py$',
  
  # prevent storage/tools from being removed
  r'core/storage/tools/.*$',
  
]

# filename comparisons are done case-insensitively
rules = [normalize(r) for r in rules]

