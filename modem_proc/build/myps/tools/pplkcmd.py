#!/usr/bin/env python
# vim: set ts=2 sts=0 sw=2 et fen fdm=marker:

'''Post-process link command. Call post-link optimization routines.

This script's original purpose was to selectively relocate object files into a
hot zone within the modem image, exploiting the hot zone's locality to limit
the amount of veneers (long jumps, trampolines) within the neighbouring 8 MiB.

Over time it has evolved as a starting point to call other optimization
routines, losing its original purpose in the process.

Current post-link operations include:
  - ERR_FATAL zlib compression
  - code and data q6zip compression
  - firmware overlay section tweaks
  - QShrink (F3 message hashing and externalization)
  - TLB MMU generation
  - default physpool reduction
  - memory usage summary
  - memory report generation

Copyright (c) 2014 by QUALCOMM Technologies, Inc. All Rights Reserved.
'''

__author__ = ('adambre', 'alexh')

# imports {{{
import os
import re
import sys
import time
import base64
import shutil
import inspect
import tempfile
import threading
import subprocess

from os.path import join as pj
from os.path import split as ps
from os.path import exists as pe
from os.path import abspath as ap


file_path = inspect.getframeinfo(inspect.currentframe()).filename
file_dir, file_name = os.path.split(file_path)

# import elfManipualtor {{{
elfManipulator_path = os.path.abspath(
  os.path.join(
    file_dir,
    '..',
    'elfManipulator',
    'include'))

sys.path.insert(0, elfManipulator_path)

import elfFileClass
# }}}
# }}}

debug = False

# 'private' globals  {{{
if sys.platform.lower().startswith('win'):
  timer = time.clock

else:
  timer = time.time
# }}}


# helper functions {{{
def t_str(seconds):  # {{{
  hours = int(seconds / 3600)
  minutes = int((seconds % 3600) / 60)
  seconds = seconds % 60

  s = []

  if hours > 0:
    if hours == 1:
      s.append('%d hour' % hours)

    else:
      s.append('%d hours' % hours)

  if minutes > 0:
    if minutes == 1:
      s.append('%d minute' % minutes)

    else:
      s.append('%d minutes' % minutes)

  if seconds > 0.0:
    if seconds == 1.0:
      s.append('%.2f second' % seconds)

    else:
      s.append('%.2f seconds' % seconds)

  return ' '.join(s)
# }}}
# }}}


class Pplk(object):  # {{{
  '''Class to wrap all of our goodies around'''
  log_version = 3
  savings_searchstr = 'Memory Savings Report: '

  def __init__(self, target, source, env, debug=False, keeplog=True):  # {{{
    '''Take everything we need from 'env' and prep it for our subroutines.

    Note: to find env items:
    print [i for i in env._dict.keys() if 'search_string' in i.lower()]
    '''
    self.elfpath = ap(str(source[0]))
    self.outdir = ap(ps(self.elfpath)[0])
    if target is not None:
      self.outpath = ap(str(target[0]))
      if len(target) == 1:
        self.qdbpath = ""
      elif len(target) == 2:
        self.qdbpath = ap(str(target[1]))
    else:
      self.outpath = None
      self.qdbpath = None
    self.buildms = ap(env.subst('${BUILD_MS_ROOT}'))
    self.memreport = (env.subst('${MEMREPORT}') == '1' or
                      ('USES_INTERNAL_BUILD' in env and
                       # last dir of $TARGET_ROOT is 'b'
                       ps(env.subst('${TARGET_ROOT}'))[-1] == 'b'))
    self.buildroot = env.subst('${TARGET_ROOT}')
    self.objcopy = env.subst('${OBJCOPY}')
    self.threads = []
    self.python = env.subst('${PYTHONCMD}')
    self.short_buildpath = env.subst('${SHORT_BUILDPATH}')
    self.cust_config = env.FindConfigFiles('cust_config.xml')[0]
    self.core_root = env.subst('${COREBSP_ROOT}')
    self.debug = debug

    if len(source) > 1 and source[1] is not None:
      self.coproc_path = ap(str(source[1]))

    else:
      self.coproc_path = None

    # BYO OrderedDict implementation: [0]: order list, [1]: dict
    self.savings_report = [[], {}]

    self.mypspath = ap(
      pj(
        ps(inspect.getfile(inspect.currentframe()))[0],
        '..'))

    if env.subst('${FAKE_ENV}') == '1':
      self.replay = True

    else:
      self.replay = False

    self.executed_manips = []
    self.finalized = False

    if keeplog:
      self.log_handle = open(
        ap(pj(self.buildms, 'pplk-%s.log' % self.short_buildpath)),
        'wb',
      )

    else:
      self.log_handle = tempfile.TemporaryFile(
        prefix='pplk-tmp-%s-' % (self.short_buildpath,),
        suffix='.log',
        dir=self.buildms,
      )

    self.log('pplkcmd log version: %i' % (self.log_version,))
    self.log('pplkcmd __dict__: %s' % (base64.b64encode(repr(self.__dict__)),))
  # }}}

  @staticmethod
  def save(k, v):  # {{{
    global myps_data
    myps_data = myps_data if 'myps_data' in locals() else {}  # NOQA
    myps_data[k] = v
  # }}}

  @staticmethod
  def load(k):  # {{{
    return myps_data[k]  # NOQA
  # }}}

  def p(self, string=''):  # {{{
    self.log_handle.write('%s\n' % string)
    print string
  # }}}

  def dp(self, string=''):  # {{{
    if debug:
      self.log_handle.write('Debug: %s\n' % string)
      sys.stderr.write('Debug: %s\n' % string)
  # }}}

  def log(self, string=''):  # {{{
    self.log_handle.write('%s\n' % string)
  # }}}

  def call(self,  # {{{
           args,
           input=None,
           merge_stderr=False,
           cwd=None,
           env=None):
    '''Execute an external command

Optionally pass input into its stdin.

Optionally merge stderr into stdout.

Optionally specify a different cwd and environment.

Return its return code, stdout, and stderr.
'''
    self.log('call(): %s' % repr(args))

    # http://docs.python.org/2/library/subprocess.html#popen-constructor
    if not merge_stderr:
      process = subprocess.Popen(args, stdin=subprocess.PIPE,
                                 stdout=subprocess.PIPE,
                                 stderr=subprocess.PIPE,
                                 cwd=cwd, env=env, universal_newlines=True)

    else:
      process = subprocess.Popen(args, stdin=subprocess.PIPE,
                                 stdout=subprocess.PIPE,
                                 stderr=subprocess.STDOUT,
                                 cwd=cwd, env=env, universal_newlines=True)

# http://docs.python.org/2/library/subprocess.html#subprocess.Popen.communicate
    (stdoutdata, stderrdata) = process.communicate(input=input)

    for line in stdoutdata.split('\n'):
      line = line.strip('\r\n')

      if line.startswith(self.savings_searchstr):
        (
          feature_name,
          sep,
          rest,
        ) = line[len(self.savings_searchstr):].partition(': ')

        (title, sep, rest) = rest.partition(': ')

        if feature_name not in self.savings_report[0]:
          self.savings_report[0].append(feature_name)
          self.savings_report[1][feature_name] = [[], {}]

        if title not in self.savings_report[1][feature_name][0]:
          self.savings_report[1][feature_name][0].append(title)

          try:
            self.savings_report[1][feature_name][1][title] = int(rest)

          except ValueError:
            sys.stderr.write(
              'Malformed value in memory savings line "%s"\n' % (line,)
            )
            raise

    return (process.returncode, stdoutdata, stderrdata)
  # }}}

  def parallel_call(self, function, parameter, chunksize=-1):  # {{{
    '''Perform a parallel call

Call function once per parameter, in parallel.

The smaller the chunksize, the more things will be processed in parallel.'''
    if not debug:
      from multiprocessing import Pool

# http://docs.python.org/2/library/multiprocessing.html#using-a-pool-of-workers
      pool = Pool(processes=len(parameter))

      if chunksize < 0:
        # by default, split it into 2 chunks
        n_chunks = 2

        chunksize = (len(parameter) + n_chunks - 1) / n_chunks

      # multiprocessing with ctrl+c: http://stackoverflow.com/a/1408476
      results = pool.map_async(function, parameter, chunksize).get(1e100)

    else:
      results = [function(i) for i in parameter]

    return results
  # }}}

  def test_executable(self, exe_name):  # {{{
    ok = True

    try:
      self.call([exe_name])

    except OSError:
      ok = False

    return ok
  # }}}

  def gen_memreport(self):  # {{{
    '''Call mpssmem.py and QMAT'''
    t1 = timer()
    base_outdir = ap(pj(self.buildms, 'reports'))

    self.p('\nGenerating MPSS memory reports in %s' % base_outdir)

    memreport = {
      'mpssmem': {
        'paths': [
          pj('reports', 'mpssmem-v3', 'mpssmem.py')],

        'arguments': [
          '-b',
          ap(self.buildroot),
          '-f',
          '-v',
          self.short_buildpath],

        'pyscript': True},

      'qcmat': {
        'paths': [
          pj('reports', 'QMAT', 'rel', 'QMemoryAnalyzer_unix_cmd'),
          pj('reports', 'QMAT', 'rel', 'QMemoryAnalyser_cmd.exe')],

        'arguments': [
          '-b',
          ap(self.buildroot)],

        'pyscript': False}}

    for report_tool, items in memreport.iteritems():
      self.p('  Generating the "%s" memory report' % report_tool)

      command = None

      for path in items['paths']:
        if items['pyscript']:
          potential_command = self.python

        else:
          potential_command = ap(pj(self.mypspath, path))

        if self.test_executable(potential_command):
          command = [potential_command]
          break

      if command:
        outdir = ap(pj(base_outdir, report_tool))

        if not os.path.exists(outdir):
          os.makedirs(outdir)

        if memreport[report_tool]['pyscript']:
          retcode, stdout, stderr = self.call(
            command +
            [ap(pj(self.mypspath, memreport[report_tool]['paths'][0]))] +
            memreport[report_tool]['arguments'] +
            ['-o', outdir])

        else:
          retcode, stdout, stderr = self.call(
            command +
            memreport[report_tool]['arguments'] +
            ['-o', outdir])

        self.log('memreport - %s:' % report_tool)
        self.log('return code: %s' % repr(retcode))
        self.log('stdout:\n%s' % stdout)
        self.log('stderr:\n%s' % stderr)
        self.log()

      else:
        self.p('  Could not execute the "%s" tool' % report_tool)

    t2 = timer()
    t_diff = t2 - t1

    self.p('  Memory report execution time: %s' % t_str(t_diff))
  # }}}

  def threaded(self, function, args=(), kwargs={}):  # {{{
    self.dp('Preparing a thread to execute %s' % repr(function))

    self.threads.append(
      threading.Thread(
        target=function,
        args=args,
        kwargs=kwargs))

    self.dp('Thread %s created. Starting.' % repr(self.threads[-1]))

    self.threads[-1].start()
  # }}}

  def qshrink(self, inpath, outpath):  # {{{
    t1 = timer()
    sh_table = list(elfFileClass.elfFile(inpath).sectionHeaderTable)
    retcode = 0

    if 'QSR_STRING' in [i.sh_nameStr for i in sh_table]:
      # qshrink section exists, run qshrink
      self.p('\nRunning QShrink')

      qspath = pj(self.mypspath, 'qshrink')

      retcode, stdout, stderr = self.call([
        self.python,
        '-O',
        pj(qspath, 'Qshrink20.py'),
        inpath,
        '--image_end_from_elfheader',
        ''.join(['--output=', outpath]),
        ''.join(['--hashfile=', pj(qspath, 'msg_hash.txt')]),
        ''.join(['--qdbFile=',self.qdbpath]),
        ''.join(['--log=', pj(self.buildms, ''.join([
          'qshrink-',
          self.short_buildpath]))])])

      if retcode != 0:
        self.p('QShrink failed. Logs below:')
        self.p('stdout: %s' % stdout)
        self.p('stderr: %s' % stderr)

      else:
        self.log('QShrink logs:')
        self.log('stdout: %s' % stdout)
        self.log('stderr: %s' % stderr)

    else:
      self.p('  Skipping QShrink: no QSR_STRING section found')

    t2 = timer()
    t_diff = t2 - t1

    self.p('  QShrink execution time: %s' % t_str(t_diff))

    return retcode
  # }}}

  def elfmanip(self, inpath, outpath):  # {{{
    t1 = timer()
    retcode = 0

    self.p('\nRunning ELF Manipulator')

    flags = [
      '-R',
      '-M',
      '-r',                 # remove
      '.rodata.trace',      # firmware F3 trace section
      '-r',                 #
      '.rodata.fw_trace',   # new name for firmware F3 trace section
      '-c',
      self.core_root,
    ]

    if self.coproc_path is not None and pe(self.coproc_path):
      flags.extend(['-s', self.coproc_path])

    retcode, stdout, stderr = self.call(
      [
        self.python,
        '-O',
        pj(self.mypspath, 'elfManipulator', 'elfManipulator.py'),
      ]
      + flags
      + [
        inpath,
        outpath,
      ]
    )

    if retcode != 0:
      self.p('ELF Manipulator failed. Logs below:')
      self.p('stdout: %s' % stdout)
      self.p('stderr: %s' % stderr)

    else:
      self.log('ELF Manipulator logs:')
      self.log('stdout: %s' % stdout)
      self.log('stderr: %s' % stderr)

    t2 = timer()
    t_diff = t2 - t1

    self.p('  ELF Manipulator execution time: %s' % t_str(t_diff))

    return retcode
  # }}}


  # supposed to be called at the end to move up section offset with
  # elfManipulator API.
  def move_up_offset(self, inpath, outpath):  # {{{
    t1 = timer()
    retcode = 0

    self.p('\nMoving up section offset after QShrink operation...')

    shutil.copyfile(inpath, outpath)

    elf = elfFileClass.elfFile(inpath)
    # TODO: add check for existence of target sections.
    elf.moveupElfOffsetSectionByName(".dynamicReclaim")
    elf.moveupElfOffsetSectionByName("QSR_STRING")
    elf.writeOutELF(outpath)

#     retcode, stdout, stderr = self.call([
#       self.python,
#       '-O',
#       pj(self.mypspath, 'tools', 'move_up_offset.py'),
#       outpath,
#       outpath])

    if retcode != 0:
      self.p('Failed moving up offset')
#     self.p('stdout: %s' % stdout)
#     self.p('stderr: %s' % stderr)
#
    else:
      self.log('Passed moving up offset')
#     self.log('stdout: %s' % stdout)
#     self.log('stderr: %s' % stderr)

    t2 = timer()
    t_diff = t2 - t1

    self.p('  Move up offset execution time: %s' % t_str(t_diff))

    return retcode
  # }}}


  #}}}


  def dynrec(self, inpath, outpath):  # {{{
    t1 = timer()
    retcode = 0

    self.p('\nRunning Dynamic Reclaim')

    # dynRec_manip.py aborts if there is nothing to do, failing to generate
    # outpath. Copy here to force outpath generation
    shutil.copyfile(inpath, outpath)

    retcode, stdout, stderr = self.call([
      self.python,
      '-O',
      pj(self.mypspath, 'tools', 'dynRec_manip.py'),
      outpath,
      outpath])

    if retcode != 0:
      self.p('Failed dynRecManip')
      self.p('stdout: %s' % stdout)
      self.p('stderr: %s' % stderr)

    else:
      self.log('Passed dynRecManip')
      self.log('stdout: %s' % stdout)
      self.log('stderr: %s' % stderr)

    t2 = timer()
    t_diff = t2 - t1

    self.p('  Dynamic Reclaim execution time: %s' % t_str(t_diff))

    return retcode
  # }}}

  def tlbupdt(self, inpath, outpath):  # {{{
    t1 = timer()
    retcode = 0

    self.p('\nRunning TLB update')

    # note the trailing slash
    tempdir = pj(*[
      self.outdir,
      ''.join(['tlbupdt-', self.short_buildpath]),
      ''])

    shutil.rmtree(tempdir, ignore_errors=True)
    os.makedirs(tempdir)

    retcode, stdout, stderr = self.call([
      self.python,
      '-O',
      pj(self.mypspath, 'tools', 'tlbupdt.py'),
      inpath,
      '-t',
      tempdir,
      '-q'])

    if retcode != 0:
      self.p('TLB update failed. Logs below:')
      self.p('stdout: %s' % stdout)
      self.p('stderr: %s' % stderr)

      return retcode

    else:
      self.log('TLB update logs:')
      self.log('stdout: %s' % stdout)
      self.log('stderr: %s' % stderr)

      locked_tlb_entries = '(unknown)'

      m = re.search(r'(\d+) locked entries', stdout)
      if m:
        locked_tlb_entries = m.groups()[0]

      self.p('  %s locked TLB entries' % locked_tlb_entries)

    _elfpath = ps(inpath)
    _elfpath = pj(_elfpath[0], ''.join(['_', _elfpath[1]]))

    shutil.move(_elfpath, outpath)

    if retcode != 0:
      self.p('Post-TLB update objcopy failed. Logs below:')
      self.p('stdout: %s' % stdout)
      self.p('stderr: %s' % stderr)

    else:
      self.log('Post-TLB update objcopy logs:')
      self.log('stdout: %s' % stdout)
      self.log('stderr: %s' % stderr)

      shutil.rmtree(tempdir, ignore_errors=True)

    t2 = timer()
    t_diff = t2 - t1

    self.p('  TLB update execution time: %s' % t_str(t_diff))

    return retcode
  # }}}

  def fwoverlay(self, inpath, outpath):  # {{{
    t1 = timer()
    retcode = 0

    self.p('\nRunning firmware overlay patch')

    shutil.copyfile(inpath, outpath)

    retcode, stdout, stderr = self.call([
      self.python,
      '-O',
      pj(self.mypspath, 'tools', 'PatchFWOverlay.py'),
      outpath])

    if retcode != 0:
      self.p('Firmware overlay patch failed. Logs below:')
      self.p('stdout: %s' % stdout)
      self.p('stderr: %s' % stderr)

    else:
      self.log('Firmware overlay patch logs:')
      self.log('stdout: %s' % stdout)
      self.log('stderr: %s' % stderr)

    t2 = timer()
    t_diff = t2 - t1

    self.p('  Firmware overlay patch execution time: %s' % t_str(t_diff))

    return retcode
  # }}}

  def default_physpool(self, inpath, outpath):  # {{{
    t1 = timer()
    retcode = 0

    self.p('\nRunning default physpool patch')

    shutil.copyfile(inpath, outpath)

    retcode, stdout, stderr = self.call([
      self.python,
      '-O',
      pj(self.mypspath, 'tools', 'patch_default_physpool.py'),
      outpath,
      self.cust_config])

    # TODO: capture 'Unaligned size' after 'Dummy segment information:'

    if retcode != 0:
      self.p('Default physpool patch failed. Logs below:')
      self.p('stdout: %s' % stdout)
      self.p('stderr: %s' % stderr)

    else:
      self.log('Default physpool patch logs:')
      self.log('stdout: %s' % stdout)
      self.log('stderr: %s' % stderr)

    t2 = timer()
    t_diff = t2 - t1

    self.p('  Default physpool patch execution time: %s' % t_str(t_diff))

    return retcode
  # }}}

  def subtract_savings_costs(self, mc_output):  # {{{
    '''
    Filter mc.py output for:

        X, Y: Z MiB

    Where:
      - X is the name of a memory saving feature
      - Y is the title of the cost
      - Z is the value on the cost
    '''
    for line in mc_output.replace('\r', '').split('\n'):
      line = line.rstrip('\n')

      if len(line) < 1 or ':' not in line or ', ' not in line:
        continue

      pre, _, post = line.partition(':')
      name, _, title = pre.partition(', ')

      try:
        value = float(post.strip().split()[0]) * (1 << 20)

      except ValueError:
        sys.stderr.write(
          'Malformed value in mc.py memory savings line "%s"' % (line,)
        )
        raise

      if (
        name in self.savings_report[0]
        and title not in self.savings_report[1][name][0]
      ):
        self.savings_report[1][name][0].append(title)
        self.savings_report[1][name][1][title] = -1 * value
  # }}}

  def print_savings_report(self):  # {{{
    sys.path.insert(0, pj(self.mypspath, 'tools'))
    import mc

    if len(self.savings_report[0]) == 0:
      return

    self.p()
    self.p('=' * 80)
    self.p()
    self.p('Memory Savings Report')
    self.p()

    for name in self.savings_report[0]:
      total = 0

      for title in self.savings_report[1][name][0]:
        value = self.savings_report[1][name][1][title]
        total += value
        self.p(mc.MemoryChecker.format('%s, %s' % (name, title), value))

      self.p(mc.MemoryChecker.format('%s, %s' % (name, 'Net Savings'), total))
      self.p()

    self.p('=' * 80)
  # }}}

  def mc(  # {{{
    self,
    inpath,
    outpath=None,
    do_print=True,
    store_dyn=False,
    print_dyn=False
  ):
    t1 = timer()
    retcode = 0

    self.p('\nRunning memory checker')

    objdump = self.objcopy
    objdump = ps(objdump)
    objdump = (objdump[0], objdump[1].lower().replace('objcopy', 'objdump'))
    objdump = pj(*objdump)

    if print_dyn:
      try:
        extra = self.load('mc_extra')

      except (NameError, KeyError):
        extra = []

    else:
      extra = []

    retcode, stdout, stderr = self.call([
      self.python,
      '-O',
      pj(self.mypspath, 'tools', 'mc.py'),
      '--hexagon-path',
      objdump,
      inpath])

    if retcode != 0:
      self.p('Memory checker failed. Logs below:')
      self.p('stdout: %s' % stdout)
      self.p('stderr: %s' % stderr)

    elif do_print:  # {{{
      if len(extra) > 0:  # splice in extra values {{{
        extra_lines = []
        extra_sizes = 0

        sys.path.insert(0, pj(self.mypspath, 'tools'))
        import mc

        # append extra lines
        for l in extra:
          m = re.search(r'^([^:]+):\s+([0-9.]{4,}) .iB', l)

          if not m:
            continue

          k, v = m.groups()
          v = float(v) * (1 << 20)

          if ('%.2f' % (mc.meg(v),)) != '0.00':
            extra_lines.append(
              '%s (dynamic)' % (mc.MemoryChecker.format(k, v),)
            )

          extra_sizes += v

        stdout = stdout.split('\n')

        total_offset = [
          i
          for i, s in enumerate(stdout)
          if s.startswith('Total:')
        ][0]

        for i, s in enumerate(extra_lines):
          stdout.insert(total_offset - 1 + i, s)

        # update Total
        total_offset = [
          i
          for i, s in enumerate(stdout)
          if s.startswith('Total:')
        ][0]

        total_val = float(stdout[total_offset].split()[1]) * (1 << 20)
        total_val += extra_sizes

        stdout[total_offset] = mc.MemoryChecker.format('Total', total_val)

        # update Available, if it exists
        if stdout[total_offset + 1].startswith('Available:'):
          avail_val = float(stdout[total_offset + 1].split()[1]) * (1 << 20)
          avail_val -= extra_sizes

          stdout[total_offset + 1] = mc.MemoryChecker.format(
            'Available',
            avail_val
          )

        stdout = '\n'.join(stdout)
      # }}}

      self.p()
      self.p('=' * 80)
      self.p()
      self.p('%s' % stdout)
      self.p('=' * 80)
      self.p()

      self.subtract_savings_costs(stdout)
    # }}}

    if store_dyn:
      try:
        extra = self.load('mc_extra')

      except (NameError, KeyError):
        extra = []

      extra.extend([i for i in stdout.split('\n') if 'dynamic' in i])

      self.save('mc_extra', extra)

    t2 = timer()
    t_diff = t2 - t1

    self.p('  Memory checker execution time: %s' % t_str(t_diff))

    return retcode
  # }}}

  def wait_for_threads(self):  # {{{
    self.dp('Waiting for all threads to finish execution')
    for thread in self.threads:
      if thread.is_alive():
        thread.join()
  # }}}

  def manip(self, method):  # {{{
    if self.finalized:
      raise Exception(
        'Already finalized the ELF, will not perform additional manipulation'
      )

    inpath = '.'.join([self.elfpath] + self.executed_manips)
    outpath = '.'.join([self.elfpath] + self.executed_manips + [method])

    return_value = self._run(method, inpath, outpath)

    self.executed_manips.append(method)

    return return_value
  # }}}

  def run(self, method, *args, **kw):  # {{{
    inpath = '.'.join([self.elfpath] + self.executed_manips)
    return self._run(method, inpath, outpath=None, *args, **kw)
  # }}}

  def _run(self, method, inpath, outpath=None, *args, **kw):  # {{{
    try:
      getattr(self.__class__, method)
      getattr(getattr(self.__class__, method), '__call__')

    except AttributeError:
      sys.stderr.write(
        'Method %s does not exist within %r\n' % (
          method,
          self.__class__
        )
      )
      raise

    # Strap yourself in, we're going deep. http://stackoverflow.com/a/1015405
    return getattr(self.__class__, method).__get__(self, self.__class__)(
      inpath,
      outpath,
      *args,
      **kw
    )
  # }}}

  def finalize_dst(self):  # {{{
    if self.outpath is not None:  # {{{
      if pe(self.outpath):
        self.p(
          'Warning: output ELF already exists at %s, overwriting' % (
            self.outpath
          )
        )

        os.unlink(self.outpath)

      if not self.debug:
        # Remove all manipulated ELF files, except the last one. Move that to
        # the final destination
        path = [self.elfpath]

        for manip in self.executed_manips[0:-1]:
          path.append(manip)
          os.unlink('.'.join(path))

        shutil.move(
          '.'.join([self.elfpath] + self.executed_manips),
          self.outpath,
        )

      else:
        shutil.copyfile(
          '.'.join([self.elfpath] + self.executed_manips),
          self.outpath,
        )

      self.elfpath = self.outpath
      self.executed_manips = []
    # }}}
    else:
      self.p('Warning: nothing to finalize, since there is no output')

    self.finalized = True
  # }}}
# }}}


def main(target, source, env):  # {{{
  '''Success: return 0 or none; failure: return non-zero'''

  # Load needed tools
  env.LoadToolScript('build_utils', toolpath=['${BUILD_ROOT}/build/scripts'])

  t1 = timer()
  p = Pplk(target, source, env, debug=debug)

  p.p('Running pplkcmd on %s' % p.elfpath)

  r = p.manip('elfmanip')
  if r != 0:
    return r

  r = p.manip('dynrec')
  if r != 0:
    return r

  r = p.manip('qshrink')
  if r != 0:
    return r

  r = p.manip('move_up_offset')
  if r != 0:
    return r

  r = p.manip('tlbupdt')
  if r != 0:
    return r

  r = p.manip('fwoverlay')
  if r != 0:
    return r

  # run mc.py before adjusting the size of DEFAULT_PHYSPOOL in pool_configs,
  # so we can get an accurate representation of our available space
  r = p.run('mc', store_dyn=True)
  if r != 0:
    return r

  r = p.manip('default_physpool')
  if r != 0:
    return r

  p.finalize_dst()

  # run mc.py again to verify that the new DEFAULT_PHYSPOOL isn't exceeded
  r = p.run('mc', do_print=False)
  if r != 0:
    return r

  p.print_savings_report()

  if p.memreport:
    try:
      p.gen_memreport()

    except Exception:
      # don't break the build
      p.dp('An exception occured while generating the memory reports')

  p.wait_for_threads()

  t2 = timer()
  t_diff = t2 - t1

  p.p('\nTotal pplkcmd execution time: %s\n' % t_str(t_diff))

  return 0
# }}}


def mc_standalone_store(target, source, env):  # {{{
  p = Pplk(target, source, env, debug=debug, keeplog=False)

  r = p.run('mc', store_dyn=True)
  if r != 0:
    return r

  p.log_handle.close()
  return 0
# }}}


def mc_standalone_print(target, source, env):  # {{{
  p = Pplk(target, source, env, debug=debug, keeplog=False)

  r = p.run('mc', print_dyn=True)
  if r != 0:
    return r

  p.log_handle.close()
  return 0
# }}}


if __name__ == '__main__':
  raise Exception(' '.join([
    'pplkcmd.py is meant to be run from within SCons.',
    'Please retry your build with --linkonly to exclusively call pplkcmd.py',
  ]))
