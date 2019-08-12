#!/usr/bin/env python
'''mc.py - memory checker

Give an estimate of the memory consumed by the MPSS image stored in an ELF

Designed to work with Q6 ELF files.
'''

# imports {{{
import os
import re
import sys
import struct
import inspect
import subprocess

from os.path import split as ps
from os.path import abspath as ap

file_path = inspect.getframeinfo(inspect.currentframe()).filename
file_dir, file_name = os.path.split(file_path)

# import elfManipualtor {{{
elfManipulator_path = os.path.abspath(
  os.path.join(
    file_dir,
    'elfManipulator',
    'include'))

sys.path.insert(0, elfManipulator_path)

import elfConstants
import elfFileClass
import elfStructs
# }}}
# }}}

last_seg_alignment = 0.25 * (1 << 20)

verbose = False


class SuperSymbolTable(dict):  # {{{
  def get(self, sym_name):  # {{{
    '''Return the first strong symbol out of symbol_list

    Override weak entries with strong ones

    Caveat: will not validate weak/strong symbols (e.g. only 2 weak symbols w/
    same name, 2 strong symbols w/ same name)'''
    strongest_symbol = None

    for i in self[sym_name]:
      if not strongest_symbol:
        strongest_symbol = i

      elif not i.isWeak():
        strongest_symbol = i
        break

    return strongest_symbol
  # }}}

  def get_list(self, sym_list):  # {{{
    '''Return all symbols specified by sym_list. Calls get() in the
    background, ensuring that the strongest symbol is returned'''
    return dict(zip(sym_list, [self.get(sym_name) for sym_name in sym_list]))
  # }}}

  def re_get(self, re_sym):  # {{{
    '''Return the strongest symbol matching re_sym. Calls get() in the
    background.'''
    sym = None

    for sym_name in self.iterkeys():
      if re.search(re_sym, sym_name):
        sym = self.get(sym_name)
        break

    return sym
  # }}}

  def re_get_list(self, re_sym_list):  # {{{
    '''Return all symbols specified by re_sym_list. Calls re_get() in the
    background.'''
    return [self.re_get(re_sym) for re_sym in re_sym_list]
  # }}}

  def re_get_all(self, re_sym):  # {{{
    '''Return all symbols matching re_sym. Calls get() in the background.'''
    matching_syms = {}

    for sym_name in self.iterkeys():
      if re.search(re_sym, sym_name):
        matching_syms[sym_name] = self.get(sym_name)

    return matching_syms
  # }}}
# }}}


def dp(string):  # {{{
  if verbose:
    sys.stderr.write('%s\n' % string)
# }}}


def call(args,  # {{{
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

  return (process.returncode, stdoutdata, stderrdata)
# }}}


def in_range(outer_start, outer_size, inner_start, inner_size):  # {{{
  return (
    outer_start <= inner_start and
    outer_start + outer_size >= inner_start + inner_size)
# }}}


def align(number, alignment):  # {{{
  return (number + int(alignment) - 1) & ~(int(alignment) - 1)
# }}}


def meg(number):  # {{{
  return float(number) / float(1 << 20)
# }}}


def print_err(me, string):  # {{{
  sys.stderr.write('Error: %s\n' % string)
  sys.stderr.write(' '.join([
    'Usage: python %s [--hexagon-path <path_to_hexagon_tools>]' % me,
    '<elf_path>\n']))
  sys.stderr.write(' '.join([
    '\n  e.g. python %s --hexagon-path C:\\Qualcomm\\HEXAGON_Tools\\5.1' % me,
    'D:\\builds\\M8x10BAAAANWAQ201042.elf\n']))
  return 1
# }}}


class MemoryChecker(object):  # {{{
  mpss_syms = [
    'qurtos_app_heap_cfg',
    'qurtos_main_stack_size',
    'QURTK_trace_buffer_size',
  ]

  audio_syms = [
    'override_heap_Base',
    'override_heap_Limit',
    'heapSize',
  ]

  q6z_swap_pool_int_syms = [
    'q6zip_swap_pool_size',
    'q6zip_rw_swap_pool_size',
  ]

  q6z_swap_pool_abs_syms = [
    '__swapped_segments_start__',
    '__swapped_segments_bss_start__',
    '__swapped_segments_end__',
  ]

  q6z_dlpager_heap_abs_syms = [
    '__dlpager_heap_start__',
    '__dlpager_heap_end__',
  ]

  q6z_swap_pool_syms = q6z_swap_pool_int_syms + q6z_swap_pool_abs_syms

  dsm_symbol_re = r'^dsm_.+_array$'

  # categories to print to the user
  size_template = (
    # (
    #   'Print string',
    #   'store_string (will be merged with self.sym_sizes)',
    #   initial_value,
    # )
    ('Image', 'img_sz', 0),
    ('AMSS Heap', 'amss_heap', 0),
    ('MPSS Heap', 'mpss_heap', 0),
    ('sys_sbrk Heap', 'sbrk_heap', 0),
    ('DSM Pools', 'dsm', 0),
    ('Q6Zip RO, Swap Pool', 'q6zip_ro_swap_pool', 0),
    ('Q6Zip RW, Swap Pool', 'q6zip_rw_swap_pool', 0),
    ('Q6Zip RW, dlpager Heap', 'q6zip_dlpager_heap', 0),
    ('Kernel Stack', 'kernel_stack', 0),
    ('Extra', 'extra', 512 * (1 << 10)),  # was called 'QRegionReq'
    ('Padding', 'pad', 0),
    ('Segment Overlay', 'seg_overlay', 0),
    ('End Address Alignment', 'align_pad', 0),
    ('Trace Buffer', 'trace_buffer', 0),
    ('Total', 'total', 0),
  )

  def __init__(self, elf_path, hexagon_path=None):  # {{{
    self.dummy_segment = None  # get_image_and_pad_size may change this
    self.hex_path = hexagon_path
    self.elf = elfFileClass.elfFile(elf_path)

    self.symtab = self.get_reduced_symbol_table()

    self.sizes = [
      i[2]
      for i in self.size_template
    ]

    self.sz_idx = dict([
      (i[1], j)
      for j, i in enumerate(self.size_template)
    ])
  # }}}

  def print_info(self):  # {{{
    ok = True

    print 'Image loaded at virtual address 0x%08x' % self.start_address

    for i, v in enumerate(self.sizes):
      k = self.size_template[i][1]

      if ('%.2f' % (meg(v),)) != '0.00':
        if k == 'total':
          print

        print self.format(self.size_template[i][0], v),

        if (
            k in self.sym_sizes
            and 'type' in self.sym_sizes[k]
            and 'dynamic' in self.sym_sizes[k]['type'].lower()
        ):
          print '(%s)' % (self.sym_sizes[k]['type'],),

        print

    if self.budget >= 0:
      print self.format('Available', self.available)

    if self.available < 0 and self.budget >= 0:
      sys.stderr.write(''.join([
        '\nError: region ',
        '0x%08x' % self.start_address,
        ', size ',
        '%.2f' % meg(self.budget),
        ' MiB is too small (',
        '%i' % self.available,
        ' bytes)\n']))

      ok = False

    return ok
  # }}}

  def pull_info(self):  # {{{
    # please be careful when changing the order of operations
    self.sym_sizes = self.get_sym_sizes()

    (
      self.sizes[self.sz_idx['img_sz']],
      self.sizes[self.sz_idx['pad']],
      self.sizes[self.sz_idx['seg_overlay']],
      self.sizes[self.sz_idx['align_pad']]
    ) = self.get_image_and_pad_size()  # dependency on get_sym_sizes

    self.budget = self.get_budget()  # dependency on get_image_and_pad_size

    self.start_address = self.get_start_address()
    self.dynamic_or_static()
    self.distribute_sym_sizes()
  # }}}

  def dynamic_or_static(self):  # {{{
    if 'sbrk_heap' in self.sym_sizes:
      # audio PD image
      if self.sym_sizes['sbrk_heap']['type'] == 'static':
        self.sizes[self.sz_idx['img_sz']] -= \
          self.sym_sizes['sbrk_heap']['size']

    if in_range(self.start_address,
                self.budget,
                self.sym_sizes['amss_heap']['start'],
                self.sym_sizes['amss_heap']['size']):
      self.sizes[self.sz_idx['img_sz']] -= \
        self.sym_sizes['amss_heap']['size']
      self.sym_sizes['amss_heap']['type'] = 'static'

    else:
      self.sym_sizes['amss_heap']['type'] = 'dynamic'

    if ('mpss_heap' in self.sym_sizes and
        'start' in self.sym_sizes['mpss_heap'] and
            'size' in self.sym_sizes['mpss_heap']):
      if in_range(self.start_address,
                  self.budget,
                  self.sym_sizes['mpss_heap']['start'],
                  self.sym_sizes['mpss_heap']['size']):
        self.sizes[self.sz_idx['img_sz']] -= \
          self.sym_sizes['mpss_heap']['size']

        self.sym_sizes['mpss_heap']['type'] = 'static'

      else:
        self.sym_sizes['mpss_heap']['type'] = 'dynamic'

    else:
      self.sym_sizes['mpss_heap']['type'] = 'dynamic; not found'

    for dsm_sym_name in self.dsm_syms.iterkeys():
      if in_range(self.start_address,
                  self.budget,
                  self.sym_sizes[dsm_sym_name]['start'],
                  self.sym_sizes[dsm_sym_name]['size']):
        self.sizes[self.sz_idx['img_sz']] -= \
          self.sym_sizes[dsm_sym_name]['size']

    if in_range(
      self.start_address,
      self.budget,
      self.sym_sizes['q6zip_dlpager_heap']['start'],
      self.sym_sizes['q6zip_dlpager_heap']['size']
    ):
      self.sizes[self.sz_idx['img_sz']] -= \
        self.sym_sizes['q6zip_dlpager_heap']['size']

      self.sym_sizes['q6zip_dlpager_heap']['type'] = 'static'

    else:
      self.sym_sizes['q6zip_dlpager_heap']['type'] = 'dynamic'
  # }}}

  def get_start_address(self):  # {{{
    try:
      start_symbols = self.symtab['start']

      sections = dict([
        (i.idx, i.sh_nameStr)
        for i in self.elf.sectionHeaderTable
      ])

      # reduce start symbols to only those that exist in the .start section
      start_symbols = [
        i
        for i in start_symbols
        if sections[i.st_shndx] == '.start'
      ]

      if len(start_symbols) < 1:
        # fall back to .start section lookup
        raise KeyError

      if len(start_symbols) > 1:
        raise Exception(
          'Multiple "start" symbols found within the ".start" section!'
        )

      start_address = start_symbols[0].st_value

    except KeyError:
      # We could be run on a multi-pd image, in which case the symbol table
      # has been stripped out. Use alternate means of finding the start
      # address

      # use the beginning of section .start
      ok = False

      for section in self.elf.sectionHeaderTable:
        if section.sh_nameStr == '.start':
          start_address = section.sh_addr
          ok = True

      if not ok:
        sys.stderr.write('Error: could not find start address\n')
        raise

    return start_address
  # }}}

  def get_full_symbol_table(self):  # {{{
    symtab = self.elf.getSectionByName('.symtab')
    strtab = self.elf.getSectionByName('.strtab')

    symbol_table = []

    if (symtab != elfConstants.RC_ERROR and
            strtab != elfConstants.RC_ERROR):
      assert len(symtab.contents) % symtab.sh_entsize == 0

      for i in xrange(0, len(symtab.contents), symtab.sh_entsize):
        symbol_table.append(
          elfStructs.Elf32_Sym(
            symtab.contents[i:i + symtab.sh_entsize],
            strtab))

    return symbol_table  # list
  # }}}

  def get_reduced_symbol_table(self):  # {{{
    symbol_table = self.get_full_symbol_table()
    symbol_table = [i for i in symbol_table if i.st_nameStr != '']

    reduced_symbol_table = {}

    for i in symbol_table:
      if i.st_nameStr not in reduced_symbol_table:
        reduced_symbol_table[i.st_nameStr] = []

      reduced_symbol_table[i.st_nameStr].append(i)

    return SuperSymbolTable(reduced_symbol_table)  # dict, -ish
  # }}}

  def get_raw_symbol_value(self, symbol):  # {{{
    if (
      'self_generated' in symbol.__dict__
      or self.elf.sectionHeaderTable[symbol.st_shndx].sh_type ==
      elfConstants.sectionTypes.SHT_NOBITS
    ):
      return struct.pack('<I', 0)

    section_offset = (
      symbol.st_value - self.elf.sectionHeaderTable[symbol.st_shndx].sh_addr)

    return self.elf.sectionHeaderTable[symbol.st_shndx].contents[
      section_offset:
      section_offset + symbol.st_size]
  # }}}

  def get_little_endian_uint_sym(self, symbol, default_value=-1):  # {{{
    if not symbol:
      return default_value

    symbol_value = self.get_raw_symbol_value(symbol)

    if len(symbol_value) != 4:
      raise Exception(
        'Symbol %s cannot be interpreted as a uint' % (symbol.st_nameStr,)
      )

    return struct.unpack('<I', symbol_value)[0]
  # }}}

  def get_image_and_pad_size(self):  # {{{
    image_size = 0
    pad = 0
    seg_overlay = 0
    alignment_pad = 0
    jumped_to_q6zip_segment = False

    # clone the program header table, so we can manipulate it
    ph_table = list(self.elf.programHeaderTable)

    # only process loadable segments
    ph_table = [i for i in ph_table if i.p_type ==
                elfConstants.segmentTypes.PT_LOAD]

    # only process non-zero segments
    ph_table = [i for i in ph_table if i.p_memsz > 0]

    # sort by increasing address size
    ph_table.sort(key=lambda i: i.p_paddr)

    # drop the last segment if its file size is 0
    # (assume that it's the dummy segment)
    if ph_table[-1].p_filesz == 0:
      dp('Dropping dummy segment 0x%08x' % (ph_table[-1].p_paddr,))
      self.dummy_segment = ph_table.pop()

    prev_start = -1
    prev_size = -1

    for ph in ph_table:
      start = ph.p_paddr
      size = ph.p_memsz

      if prev_start > 0 and prev_size > 0:
        potential_pad = start - (prev_start + prev_size)

        if potential_pad >= 0:
          if (
            self.q6z_start_address is not None
            and start >= self.q6z_start_address
            and not jumped_to_q6zip_segment
          ):
            # ignore the mega jump to .candidate_compress_section
            jumped_to_q6zip_segment = True

          else:
            dp(''.join([
              'Padding of 0x%08x between segments 0x%08x and 0x%08x' % (
                potential_pad,
                prev_start,
                start)]))
            pad += potential_pad

        else:
          dp(''.join([
            'Overlay of 0x%08x between segments 0x%08x and 0x%08x' % (
              -1 * potential_pad,
              prev_start,
              start)]))
          seg_overlay += potential_pad

      image_size += size

      prev_start = start
      prev_size = size

    assert prev_start == start, 'Uh oh #1'
    assert prev_size == size, 'Uh oh #2'

    # we have to account for the last segment
    aligned_ending_address = align(prev_start + prev_size, last_seg_alignment)
    alignment_pad = aligned_ending_address - (prev_start + prev_size)
    dp('Alignment padding of 0x%08x added' % alignment_pad)

    return image_size, pad, seg_overlay, alignment_pad
  # }}}

  def calc_q6zip_swap_pool(self, sym_dict):  # {{{
    # for the logic behind this, see CR 695530
    return_vals = {
      'size': 0,
      'type': 'dynamic, disabled/not found',
    }

    defaults = {
      'q6zip_swap_pool_size': 2 * (1 << 20),  # 2 MiB
      'q6zip_rw_swap_pool_size': 1 * (1 << 20),  # 1 MiB
    }

    q6zip_values = dict(
      zip(
        self.q6z_swap_pool_syms,
        [0] * len(self.q6z_swap_pool_syms)
      )
    )

    ro = dict(return_vals)
    rw = dict(return_vals)

    for n in self.q6z_swap_pool_int_syms:
      if sym_dict[n].st_value > 0:
        # symbol exists, store its size
        q6zip_values[n] = self.get_little_endian_uint_sym(sym_dict[n])

      if q6zip_values[n] == 0 and n in defaults:
        # apply default values if we have 0 as the size
        q6zip_values[n] = defaults[n]

    for n in self.q6z_swap_pool_abs_syms:
       q6zip_values[n] = sym_dict[n].st_value

    if 'self_generated' in sym_dict['__swapped_segments_start__'].__dict__:
      self.q6z_start_address = None

    else:
      self.q6z_start_address = q6zip_values['__swapped_segments_start__']

    if (q6zip_values['__swapped_segments_start__'] ==
            q6zip_values['__swapped_segments_end__']):
      # q6zip compression disabled, no-op
      pass

    else:
      # q6zip compression enabled, add RO swap pool size
      ro['type'] = 'dynamic'
      ro['size'] += q6zip_values['q6zip_swap_pool_size']

      if (q6zip_values['__swapped_segments_bss_start__'] == 0 or
          (q6zip_values['__swapped_segments_bss_start__'] ==
           q6zip_values['__swapped_segments_end__'])):
        # q6zip RW compression disabled, no-op
        pass

      else:
        # q6zip RW compression enabled, add RW swap pool size
        rw['type'] = 'dynamic'
        rw['size'] += q6zip_values['q6zip_rw_swap_pool_size']

    return ro, rw
  # }}}

  def get_sym_sizes(self):  # {{{
    syms = {}
    sym_sizes = {}
    sym_list = (
      self.mpss_syms
      + self.q6z_swap_pool_syms
      + self.audio_syms
      + self.q6z_dlpager_heap_abs_syms
    )

    for i in sym_list:
      try:
        syms[i] = self.symtab.get(i)

      except KeyError:
        # any combination of these symbols may not be present, don't break if
        # we don't find them

        # avoid __init__ by calling __new__
        syms[i] = object.__new__(elfStructs.Elf32_Sym)

        # populate it with default values
        syms[i].__dict__ = dict(
          syms[i].__dict__.items() +
          {
            'st_name': 0,
            'st_value': 0,
            'st_size': 0,
            'st_info': elfConstants.symbolTypes.STT_NOTYPE,
            'st_other': elfConstants.symVisibility.STV_DEFAULT,
            'st_shndx': elfConstants.specialSectionsIndexes.SHN_ABS,
            'st_nameStr': '',
            'self_generated': True,  # out-of-band indicator
          }.items()
        )

    # modem_proc/qdsp6/*/qurt_config.c:
    # struct config_heap_info {
    #    enum config_heap_type type;
    #    void *vaddr;
    #    unsigned size;
    # };
    amss_heap_cfg = self.get_raw_symbol_value(syms['qurtos_app_heap_cfg'])

    if 'self_generated' in syms['qurtos_app_heap_cfg'].__dict__:
      sym_sizes['amss_heap'] = {
        'start': 0,
        'size': 0}

    else:
      # enum size may be chosen by the compiler
      # (http://stackoverflow.com/a/366026), so let's work backwards
      #
      # use I for void * vaddr, since P is unavailable for non-native byte
      # ordering
      #
      # use I for unsigned size
      enum_size = (syms['qurtos_app_heap_cfg'].st_size -
                   struct.calcsize('II'))

      s = struct.Struct(''.join(['<', str(enum_size), 'BII']))

      unpacked_data = list(s.unpack(amss_heap_cfg))

      sym_sizes['amss_heap'] = {
        'start': unpacked_data[-2],
        'size': unpacked_data[-1]}

    # audio PD heap symbols  {{{
    if (
      hasattr(syms['override_heap_Base'], 'self_generated')
      and hasattr(syms['override_heap_Limit'], 'self_generated')
    ):
      # override_heap_Base and override_heap_Limit don't exist, i.e. we're
      # using a dynamic heap implementation
      sym_sizes['sbrk_heap'] = {
        'size': self.get_little_endian_uint_sym(syms['heapSize']),
        'type': 'dynamic',
      }

    else:
      # static heap implementation
      try:
        sym_sizes['sbrk_heap'] = {
          'start': self.get_little_endian_uint_sym(syms['override_heap_Base']),
          'size': (
            self.get_little_endian_uint_sym(syms['override_heap_Limit'])
            - self.get_little_endian_uint_sym(syms['override_heap_Base'])
          ),
          'type': 'static',
        }

      except Exception:
        # could not extract static heap size, set it as 0
        sym_sizes['sbrk_heap'] = {
          'start': -1,
          'size': 0,
          'type': 'static',
        }
    # }}}

    self.dsm_syms = self.symtab.re_get_all(self.dsm_symbol_re)

    for dsm_sym_name, sym in self.dsm_syms.iteritems():
      sym_sizes[dsm_sym_name] = {
        'start': sym.st_value,
        'size': sym.st_size,
      }

    sym_sizes['kernel_stack'] = {
      'size': self.get_little_endian_uint_sym(syms['qurtos_main_stack_size'])}

    sym_sizes['trace_buffer'] = {
      'size': self.get_little_endian_uint_sym(syms['QURTK_trace_buffer_size'])}

    (
      sym_sizes['q6zip_ro_swap_pool'],
      sym_sizes['q6zip_rw_swap_pool'],
    ) = self.calc_q6zip_swap_pool(syms)

    try:
      sym_sizes['q6zip_dlpager_heap'] = {
        'start': syms[self.q6z_dlpager_heap_abs_syms[0]].st_value,
        'size': (
          syms[self.q6z_dlpager_heap_abs_syms[1]].st_value
          - syms[self.q6z_dlpager_heap_abs_syms[0]].st_value
        ),
      }

    except KeyError:
      sym_sizes['q6zip_dlpager_heap'] = {
        'start': -1,
        'size': 0,
      }

    # MPSS heap
    # The MPSS heap can be statically or dynamically allocated
    #
    # In static allocation, the symbol is called heap_buffer. The compiler may
    # change the symbol name into these permutations:
    # - modem_mem_init_heap.heap_buffer
    # - r'heap_buffer\.\d+'
    #
    # If that's the case we can simply search the symbol table for the address
    # and the size of the heap buffer
    #
    # In the case of dynamic allocation, a request was placed to store the
    # size of the heap into a symbol called modem_mem_heap_size. Search the
    # symbol table for that symbol, and use its value if found.
    #
    # If we dynamically allocate and modem_mem_heap_size can't be found, we
    # have to disassemble the ELF to look at how much we malloc for the MPSS
    # heap.
    mpss_heap_symbols = self.symtab.re_get_list(
      [r'^modem_mem_init_heap\.heap_buffer$',
       r'^heap_buffer\.[0-9]+$'])

    # clear blank entries
    mpss_heap_symbols = [i for i in mpss_heap_symbols if i]

    if len(mpss_heap_symbols) > 1:
      raise Exception(
        'Multiple MPSS heap symbols found: %r' % (mpss_heap_symbols,))

    if len(mpss_heap_symbols) > 0:
      sym_sizes['mpss_heap'] = {
        'start': mpss_heap_symbols[0].st_value,
        'size': mpss_heap_symbols[0].st_size,
        'found': True}

    elif 'modem_mem_heap_size' in self.symtab:
      sym_sizes['mpss_heap'] = {
        'start': -1,
        'size': self.get_little_endian_uint_sym(
          self.symtab.get('modem_mem_heap_size')),
        'found': True}

    else:
      sym_sizes['mpss_heap'] = self.disassemble_mpss_heap_size()

    return sym_sizes
  # }}}

  def disassemble_mpss_heap_size(self):  # {{{
    hexagon_path = self.hex_path

    values = {
      'found': False}

    elf_path = self.elf.elf

    if hexagon_path is not None:
      # not explicitly given to use, discover it for ourselves
      hexagon_paths = [
        r'\\QCTDFSRT\qctasw\Hexagon',
        '/pkg/qct/software/hexagon/releases/tools']

      hexagon_path = None

      # prerequisite checks {{{
      for path in hexagon_paths:
        if os.path.exists(path):
          hexagon_path = path
          break

      if not hexagon_path:
        # can't find hexagon tools, abort
        return values
      # }}}

      # get the highest numerical hexagon version; assume that it will work
      hexagon_versions = [i for i in os.listdir(hexagon_path)
                          if re.search(r'^[^.][0-9.]+$', i)]

      # versions 7 and up use hexagon-llvm-objdump, which doesn't support
      # --start-address and --stop-address; trim those
      hexagon_versions = [i for i in hexagon_versions
                          if int(i[0:i.find('.')], 10) < 7]

      hexagon_version = sorted(hexagon_versions)[-1]

      hexagon_path = os.path.join(hexagon_path, hexagon_version)

      hexagon_version = None
      hexagon_versions = None
      del hexagon_version
      del hexagon_versions

    functions_to_disassemble = (
      'modem_mem_init_heap',
      'modem_mem_check_heap_initialized')

    function = None

    for i in functions_to_disassemble:
      try:
        function = {
          'name': self.symtab.get(i).st_nameStr,
          'start': self.symtab.get(i).st_value,
          'end': self.symtab.get(i).st_value + self.symtab.get(i).st_size}

        break

      except KeyError:
        continue

    if not function:
      # don't know what function to disassemble, abort
      return values

    # we can allow hexagon_path to point to objdump itself, or the whole set of
    # GNU tools
    objdump_paths = (
      hexagon_path,
      os.path.join(hexagon_path, 'Tools', 'bin', 'hexagon-objdump'),
      os.path.join(hexagon_path, 'gnu', 'bin', 'hexagon-objdump'),
      'hexagon-objdump')

    objdump_path = None

    for path in objdump_paths:
      try:
        call([path])

        objdump_path = path
        break

      except OSError:
        continue

    if not objdump_path:
      # can't find hexagon-objdump, abort
      return values

    return_code, stdout, _ = call(
      [
        objdump_path,
        '-d',
        '--start-address=0x%08x' % function['start'],
        '--stop-address=0x%08x' % function['end'],
        elf_path],
      merge_stderr=True)

    if return_code != 0:
      # hexagon-objdump barfed, abort
      return values

    stdout = stdout.splitlines()

    alloc_line = -1

    for i, line in enumerate(stdout):
      if 'call' in line and 'mem_init_heap_alloc' in line:
        alloc_line = i
        break

    if alloc_line < 0:
      # the call to mem_init_heap_alloc doesn't exist, abort
      return values

    # work backwards from the designated line until we find r1 being set
    for line in reversed(stdout[0:alloc_line]):
      if re.search(r'\br1\b', line):
        # Looking for:
        #  974876c: 02 28 01 28   28012802       r1 = ##4194304 ; r2 = #0 }
        #
        # If the absolute number (##) isn't there, we'll need more advanced
        # logic to reconstruct r1. Save that for another time.
        m = re.search(r'r1 = ##([0-9]+)', line)

        if m:
          values = {
            'start': 0,
            'size': int(m.groups()[0]),
            'found': True}

        break

    return values
  # }}}

  def get_budget(self):  # {{{
    budget = -1

    pool_configs = None

    try:
      pool_configs = self.symtab.get('pool_configs')

    except KeyError:
      pass

    if pool_configs is not None:
      pool_configs_content = self.get_raw_symbol_value(pool_configs)

      # struct definition of pool_configs: struct phys_mem_pool_config
      # modem_proc/core/kernel/qurt/install/.../cust_config.c
      s = struct.Struct('<32c' + 'II' * 16)
      config_entry_size = s.size

      for i in xrange(0, pool_configs.st_size, config_entry_size):
        unpacked_data = list(
          s.unpack(
            pool_configs_content[
              i:
              i + config_entry_size]))

        pool_entry_name = ''.join(unpacked_data[0:32]).split('\x00')[0]
        pool_entry_addr_pairs = unpacked_data[32:]

        if pool_entry_name == 'DEFAULT_PHYSPOOL':
          # get even/odd entries of a list:
          # http://stackoverflow.com/a/11702449
          # see also:
          # http://docs.python.org/2/whatsnew/2.3.html#extended-slices
          budget = sum(pool_entry_addr_pairs[1::2])
          break

    if budget < 0 and self.dummy_segment is not None:
      budget = (
        self.dummy_segment.p_paddr
        + self.dummy_segment.p_memsz
        - min([
          i.p_paddr
          for i in self.elf.programHeaderTable
        ])
      )

    return budget
  # }}}

  @staticmethod
  def format(name, value):  # {{{
    # needs to be a static method, since it's referenced as a classmethod in
    # pplkcmd
    #
    # keep it in sync with how long 'name' gets
    return '%-39s %6s MiB' % (
      '%s:' % name,
      '%.2f' % meg(value))
  # }}}

  def distribute_sym_sizes(self):  # {{{
    '''Accumulate our symbol sizes into our general-purpose size dict'''
    sym_sizes = dict(self.sym_sizes)  # copy

    for src_key in sym_sizes.keys():
      for _, dest_key, _ in self.size_template:
        if dest_key in src_key:
          if 'size' in sym_sizes[src_key]:
            self.sizes[self.sz_idx[dest_key]] += sym_sizes[src_key]['size']

          del sym_sizes[src_key]
          break

    # add up everything we missed into 'extra'
    for src_key, src_value in sym_sizes.iteritems():
      if 'size' in src_value:
        self.sizes[self.sz_idx['extra']] += src_value['size']

    # merge some calculated items into 'Extra'
    self.sizes[self.sz_idx['extra']] += self.sizes[self.sz_idx['trace_buffer']]
    self.sizes[self.sz_idx['trace_buffer']] = 0

    self.sizes[self.sz_idx['extra']] += self.sizes[self.sz_idx['kernel_stack']]
    self.sizes[self.sz_idx['kernel_stack']] = 0

    self.sizes[self.sz_idx['total']] = sum(self.sizes)
    self.available = self.budget - self.sizes[self.sz_idx['total']]
  # }}}
# }}}


def main(argv):  # {{{
  hexagon_path = None
  elf_path = None

  i = 0
  while i < len(argv):
    arg = argv[i]

    if arg.lower() == '--hexagon-path':
      if not (i + 1 < len(argv)):
        return print_err(file_name, 'Expecting argument after --hexagon-path')

      hexagon_path = argv[i + 1]

      i += 2
      continue

    elif arg.lower().startswith('--hexagon-path='):
      path = '='.join(arg.split('=')[1:])

      if len(path) < 1:
        return print_err(file_name, 'Expecting argument after --hexagon-path=')

      hexagon_path = path

      i += 1
      continue

    elif arg.lower() == '-v' or arg.lower() == '--verbose':
      global verbose
      verbose = True

    else:
      elf_path = arg

    i += 1

  if not elf_path:
    return print_err(file_name, 'No ELF path given')

  if not os.path.exists(elf_path):
    return print_err(file_name, 'ELF does not exist: %s' % elf_path)

  print '%s\n' % (ps(ap(elf_path))[-1],)

  # get elf data
  mc = MemoryChecker(elf_path, hexagon_path=hexagon_path)

  mc.pull_info()

  dp('')  # separate verbose data from regular data
  ok = mc.print_info()

  if not ok:
    return 1

  else:
    return 0
# }}}

if __name__ == '__main__':
  sys.exit(main(sys.argv[1:]))

# vim: set ts=2 sw=2 et ai nosi fen fdm=marker:
