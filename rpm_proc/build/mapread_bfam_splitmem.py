# vim: set ts=4 sw=4 noet smarttab ai :

import os, re, sys, subprocess


# dictionary of dictionaries representing image sizes
# first dictionary is keyed on reference build, e.g. 30, 31, 32
# second dictionary is keyed on "driver" e.g. sleep, kernel, etc.
library_sizes = { }

library_map = {
	'main.o'                            : 'misc',
	'rpm.o'                             : 'misc',
	'rpm_error_handler.o'               : 'misc',
	'rpm_mc.o'                          : 'misc',
	'rpm_ram_data.o'                    : 'misc',
	'rpm_ram_init.o'                    : 'misc',
	'xpu_init.o'                        : 'misc',
	'anon*'                             : 'misc',
	'coredump.o'                        : 'misc',
	'exceptions.o'                      : 'misc',
	'retarget.o'                        : 'misc',
	'time_service.o'                    : 'misc',
	'timetick.*'                        : 'misc',
	'image_layout.o'                    : 'misc',
	'datetime.o'                        : 'misc',
	'chipinfo.lib'                      : 'misc',
	'system_db.lib'                     : 'misc',
     'platforminfo.lib'                  : 'misc',
     'version.lib'                       : 'misc',
	'sleep.*'							: 'sleep',
	'DALInterruptController.*'          : 'intctrl',
	'HALqgic.lib'						: 'intctrl',
	'tramp.lib'							: 'intctrl',
	'DALRPMFW.lib'						: 'rpm',
	'RPMAlgorithm.lib'                  : 'rpm',
	'rpm.lib'                           : 'rpm',
	'rpm_c.lib'                         : 'rpm',
	'rpm_cpp.lib'                       : 'rpm',
	'rpm_config.lib'                    : 'rpm',
	'stack_protect.o'                   : 'rpm',
	'Prng*'                             : 'rpm',
	'rbcpr_c.lib'                       : 'rpm',
	'rbcpr_cpp.lib'                     : 'rpm',
	'DALFwk.lib'                        : 'kernel',
	'DALFwk_reduced.lib'				: 'kernel',
	'DALFwk_rex.lib'     				: 'kernel',
	'DALSys.lib'                        : 'kernel',
	'DALSys_native.lib'					: 'kernel',
	'DALConfig.lib'						: 'kernel',
	'DALTimer.lib'						: 'daltimer',
	'DALTimer_native.lib'				: 'daltimer',
	'HALsbi.lib'						: 'sbi',
	'DALsbi.lib'						: 'sbi',
	'sbi_mini.lib'						: 'sbi',
	'Spmi.lib'    						: 'spmi',
	'mpm.lib'							: 'mpm',
	'[^m]+mpm.*'						: 'mpm',
	'MPM_.*'                            : 'mpm',
	'src.lib'                           : 'mpm', # for a while the MPM code built as 'src.lib'
	'npa.lib'							: 'npa',
	'ulog.lib'                          : 'ulog',
	'DALulog.lib'                       : 'ulog',
	'DALulogSm.lib'                     : 'ulog',
	'ulog_frontend.lib'                 : 'ulog',
	'ulogSm.lib'						: 'ulog',
	'fakedrivers.lib'					: 'misc',
	'utils.lib'							: 'misc',
	'fz_t_p.l'							: 'floating point',
	'fz_ts.l'							: 'floating point',
	'h_t__un.l'							: 'libc',
	'c_t__un.l'							: 'libc',
	'c_t.l'                             : 'libc',
	'c_w.l'                             : 'libc',
	'm_t_pu.l'                          : 'libc',
	'cpprt_t.l'                         : 'libc',
	'cpprt_w.l'                         : 'libc',
	'pmic_.*'                           : 'pmic',
	'(DalA|HALa)xi.*'                   : 'buses',
	'Icb.*'                             : 'buses',
	'icb.*'                             : 'buses',
	'HAL(m2vmt|xpu).*'                  : 'xpu',
	'busywait.*'                        : 'clock',
	'HALclk_rpm.lib'                    : 'clock',
	'clkregim_rpm.lib'                  : 'clock',
	'clkrgm.*'                          : 'clock',
	'Clock*'                            : 'clock',
	'HALclk*'                           : 'clock',
	'DDR_.*.lib'                        : 'boot',
	'DDR_HAL.lib'                       : 'boot',
	'ddr_*'                             : 'boot',
    'railway.*'                         : 'railway',
    'svs.lib'                           : 'svs',
    'rpm_sched.lib'                     : 'scheduler',
    'rpmutils.lib'                      : 'scheduler',
	'lite.lib'                          : 'qdss',
	'qdss_common.lib'                   : 'qdss',
	'smem.*'                            : 'smd',
	'smd.*'                             : 'smd',
	'services.*'                        : 'smd',
}

reference_builds = r'\\gigantic\corebsp\power\rpm\refbuilds\8960'

def get_lib_line_data(m):
	code_size = int(m.group('code_size'))
	data_size = int(m.group('ro_size')) + int(m.group('rw_data')) + int(m.group('zi_data'))
	total_size = code_size + data_size
	module = m.group('module').strip()
	return module, code_size, data_size, total_size

def get_exec_line_data(m):
	size = -1 * int(m.group('size'), 16) # size is negative because we're parsing the heap region that doesn't count
	module = m.group('object').strip()
	module = module.split('(')
	if len(module) > 1:
		module = module[1].strip('()')
	else:
		module = module[0]
	return module, 0, size, size

map_line_types = [
	{'parse_func' : get_lib_line_data,  'regex' : re.compile(r'(?P<code_size>\d+)\s+(?P<inc_data>\d+)\s+(?P<ro_size>\d+)\s+(?P<rw_data>\d+)\s+(?P<zi_data>\d+)\s+(?P<debug>\d+)\s+(?P<module>.+)')},
	{'parse_func' : get_exec_line_data, 'regex' : re.compile(r'(?P<base_addr>0x[0-9a-fA-F]+)\s+(?P<size>0x[0-9a-fA-F]+)\s+(?P<type>[^\s]+)\s+(?P<attr>[^\s]+)\s+(?P<idx>\d+)\s+(?P<section_name>[^\s]+)\s+(?P<object>.+)')},
]

at_mapped_symbol_regex = re.compile(r'\s*0x[0-9a-fA-F]{8}\s+(?P<size>0x[0-9a-fA-F]{8}).+\.ARM\.__AT_.+\s+(?P<module>.+)')

def get_module_pretty_name(refbuild, module):
	# figure out if we should attribute this to a renamed module, or to it's own name
	for pattern in library_map:
		m = pattern.match(module)
		if m:
			return library_map[pattern]

	print >> sys.stderr, "*** WARNING: no pretty renaming for refbuild %s module %s" % (refbuild, module)
	return module

def parse_map_line(refbuild, line):
	# match this against the interesting line pattern
	line = line.strip()
	module = None
	for map_line_type in map_line_types:
		m = map_line_type['regex'].match(line)
		if m:
			module, code_size, data_size, total_size = map_line_type['parse_func'](m)
			break
	
	if not module:
		return

	module = get_module_pretty_name(refbuild, module)

	if module not in library_sizes[refbuild]:
		library_sizes[refbuild][module] = [code_size, data_size, total_size]
	else:
		library_sizes[refbuild][module] = [existing + new for existing, new in zip(library_sizes[refbuild][module], [code_size, data_size, total_size])]

def parse_map(refbuild, map_lines):
	library_sizes[refbuild] = {}

	# now we're interested in capturing data from the following sections
	#	  1 - misc build objects
	#	  5 - library descriptions
	# section boundaries are denoted with '-----' lines and the end of
	# the interesting section is denoted by a '======' line
	region = ''
	section = 0
	for line in map_lines:
		# check for start of interesting data or section boundary
		if 'Execution Region' in line:
			region = line.strip().split(' ')[2]
			continue

		if '.ARM.__AT_' in line:
			m = at_mapped_symbol_regex.match(line)
			if m:
				module = get_module_pretty_name(refbuild, m.group('module'))
				at_mapped_size = int(m.group('size'), 16)
				module_code_size, module_data_size, module_total_size = library_sizes[refbuild].get(module, [0, 0, 0])
				library_sizes[refbuild][module] = [module_code_size, module_data_size - at_mapped_size, module_total_size - at_mapped_size]

		if line.find('=====') >= 0:
			region = ''
			continue

		if line.startswith('Image component sizes') or line.find('-----') >= 0:
			section += 1
			continue

		# check for end of interesting data
		if section > 0 and line.find('=====') >= 0:
			break

		# skip other boring sections
		if not (region == 'DAL_HEAP' or section == 1 or section == 5):
			continue

		# do the actual parsing
		parse_map_line(refbuild, line)

def get_column_data(column_name):

	module_data = library_sizes[refbuild].get(module, [0, 0, 0])


def dump_csv():
	# determine columns our file should have (columns are modules, rows are builds)
	modules = set()
	for refbuild in library_sizes:
		for module in library_sizes[refbuild]:
			modules.add(module)
	
	header = 'build'
	for module in modules:
		header += ',%s code' % module
		header += ',%s data' % module
		header += ',%s total' % module
	print header

	# now data
	refbuilds = library_sizes.keys()
	refbuilds.sort()
	for refbuild in refbuilds:
		data = refbuild
		for module in modules:
			data += ',%s' % library_sizes[refbuild][module][0]
			data += ',%s' % library_sizes[refbuild][module][1]
			data += ',%s' % library_sizes[refbuild][module][2]
		print data


def get_map_lines(build_root, map_file):
	fname = os.path.join(build_root, map_file)
	if os.path.exists(fname):
		# unzipped build, can read map file directly
		with open(fname, 'r') as f:
			map_lines = f.readlines()
		return map_lines

	build_name = os.path.basename(build_root)
	zip_file = os.path.join(build_root, build_name + '.7z')
	if os.path.exists(zip_file):
		# zipped build, must read from archive
		read_cmd = '7z -so e %s %s' % (zip_file, os.path.join(build_name, map_file))
		unzipper = subprocess.Popen(read_cmd, stdout = subprocess.PIPE, stderr = subprocess.PIPE)
		stdout, stderr = unzipper.communicate()
		if 'Everything is Ok' in stderr:
			return stdout.split('\n')
	
	return None


if __name__ == '__main__':
	# regenerate the pretty library renaming map with actual compiled regexs
	library_map_compiled = {}
	for regex in library_map:
		library_map_compiled[re.compile(regex)] = library_map[regex]
	library_map = library_map_compiled

	if len(sys.argv) > 1:
		builds = sys.argv[1:]
	else:
		print 'Usage:', sys.argv[0], ' <build paths>'
		sys.exit(1)

	for build_root in builds:
		refbuild = os.path.basename(build_root)
		map_file_locations = [
             r'rpm_proc/core/bsp/rpm/build/RPM_AAAAANAAR.map',
             r'rpm_proc/core/bsp/rpm/build/'+os.environ['MSM_ID']+'/RPM_AAAAANAAR.map',
 		]
		for map_file in map_file_locations:
			map_lines = get_map_lines(build_root, map_file)
			if map_lines:
				print >> sys.stderr, "Evaluating %s" % refbuild
				parse_map(refbuild, map_lines)
				break

	dump_csv()

