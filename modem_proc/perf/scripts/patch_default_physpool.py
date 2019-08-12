#!/usr/bin/env python
#
# image_patch.py
#
# GENERAL DESCRIPTION
#   Used to add/patch an elf file with an additional program header segment to
#   reflect the full memory usage based on a DEFAULT_PHYSPOOL specified in
#   the provided xml configuration file.
#
# Copyright (c) 2012 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
# =============================================================================

import os
import sys
import struct
import inspect
from xml.dom.minidom import parse


def fix_physpool_size(input_elf, dummy_segment_size):  # {{{
  script_dir = os.path.split(
    inspect.getframeinfo(inspect.currentframe()).filename)[0]

  # cwd() is supposedly modem_proc/build/ms
  # could also be launched from our master location
  elfmanip_paths = (
    ('elfManipulator', 'include'),
  )

  for elfmanip_path in elfmanip_paths:
    sys.path.insert(0, os.path.join(*[script_dir] + list(elfmanip_path)))

  import elfFileClass
  import elfConstants

  elf = elfFileClass.elfFile(input_elf)
  symtab = elf.getSectionByName('.symtab')
  strtab = elf.getSectionByName('.strtab')
  found_pool_configs = False

  ph_table = sorted(
    [
      i
      for i in elf.programHeaderTable
      if i.p_paddr > 0
    ],
    key=lambda x: x.p_paddr,
  )

  ph = ph_table[-1]

  print 'Program segment before dummy segment:'
  print '  Starting address       : 0x%08x' % ph.p_paddr
  print '  Size                   : 0x%08x' % ph.p_memsz
  print '  Alignment              : 0x%08x' % ph.p_align

  dummy_seg = {
    'start': (ph.p_paddr + ph.p_memsz + ph.p_align - 1) & ~(ph.p_align - 1),
    'size': dummy_segment_size,
    'align': 0x100000}

  print 'Dummy segment information:'
  print '  Starting address       : 0x%08x' % dummy_seg['start']
  print '  Unaligned size         : 0x%08x' % dummy_seg['size']

  physpool = {'size_nodummy': dummy_seg['start'] - ph_table[0].p_paddr}

  physpool['end_aligned'] = (
    ph_table[0].p_paddr +
    physpool['size_nodummy'] +
    dummy_seg['size'] +
    dummy_seg['align'] - 0x1) & ~(dummy_seg['align'] - 0x1)

  physpool['size_new'] = physpool['end_aligned'] - \
      ph_table[0].p_paddr

  if (symtab != elfConstants.RC_ERROR and strtab != elfConstants.RC_ERROR):
    for symbol in elfFileClass.Elf32_SymGenerator(symtab, strtab):
      if symbol.st_nameStr == 'pool_configs':
        found_pool_configs = True

        # For executable files, st_value holds a virtual address
        sect_offset = symbol.st_value - \
            elf.sectionHeaderTable[symbol.st_shndx].sh_addr

        # struct definition of pool_configs
        # modem_proc/core/kernel/config/build/mba/qdsp6/.../qurt_config.c#286
        s = struct.Struct('<32c' + 'II' * 16)
        entry_size = s.size

        found_default_physpool = False

        for struct_offset in xrange(0, symbol.st_size, entry_size):
          unpacked_data = list(
            s.unpack(
              elf.sectionHeaderTable[symbol.st_shndx].contents[
                sect_offset +
                struct_offset:sect_offset +
                struct_offset +
                entry_size]))

          pool_entry_name = ''.join(unpacked_data[0:32]).rstrip('\x00')
          pool_entry_addr_pairs = unpacked_data[32:]

          if pool_entry_name == 'DEFAULT_PHYSPOOL':
            found_default_physpool = True

            for i in xrange(0, len(pool_entry_addr_pairs), 2):
              # the first pair holds the (start, size) tuple
              old_size = pool_entry_addr_pairs[i + 1]

              print 'Default physical pool information:'
              print '  Old size               : 0x%08x' % old_size
              print '  Aligned adjusted size  : 0x%08x' % physpool['size_new']
              print '  Aligned ending address : 0x%08x' % physpool['end_aligned']  # NOQA

              if physpool['size_new'] > old_size:
                raise Exception(' '.join([
                  'Adjusted budget of 0x%08x cannot' % (physpool['size_new'],),
                  'be greater than the original budget of',
                  '0x%08x' % (old_size,)
                ]))

              pool_entry_addr_pairs[i + 1] = physpool['size_new']
              break

            # roll it all back into the ELF
            unpacked_data[32:] = pool_entry_addr_pairs
            packed_data = s.pack(*unpacked_data)
            elf.sectionHeaderTable[symbol.st_shndx].contents = ''.join([
              elf.sectionHeaderTable[symbol.st_shndx].contents[:sect_offset],
              packed_data,
              elf.sectionHeaderTable[symbol.st_shndx].contents[
                sect_offset +
                len(packed_data):]])
            elf.writeOutELF(input_elf)

            print 'Successfully wrote new DEFAULT_PHYSPOOL size 0x%08x' % \
                physpool['size_new']

            return

        if not found_default_physpool:
          print 'Error: Could not find DEFAULT_PHYSPOOL within pool_configs'
          sys.exit(2)

    if not found_pool_configs:
      print 'Error: Could not find symbol pool_configs to update'
      sys.exit(2)

  else:
    print 'Error: Missing .symtab and/or .strtab section'
    sys.exit(2)
# }}}


def main(argv):  # {{{
  try:
    elf_path, xml_path = argv[1:]

  except ValueError:
    print 'Usage: python %s elf_path xml_path' % argv[0]
    sys.exit(3)

  dom = parse(xml_path)

  pgp_size = 0
  for node in dom.getElementsByTagName('program'):
    for node1 in node.getElementsByTagName('dynamic_default_pool_size'):
      pgp_size = int(node1.getAttribute('value'), 16)

  if pgp_size > 0:
    fix_physpool_size(elf_path, pgp_size)

  else:
    print 'dynamic_default_pool_size not found, nothing to patch'
# }}}

if __name__ == "__main__":
  main(sys.argv)
