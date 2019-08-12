#===============================================================================
# Copyright (c) 2015 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#===============================================================================

import sys
import os, errno
import string
import SCons.Action
import struct
import traceback
import hashlib
import ntpath
from SCons.Script import *

quiet = 1 # we use 0/1 instead of True/False
          # because python can optimize ints
          # but not bool objects when invoked with -O

# UTIL FUNCTIONs
# Our python version of VERIFY macro
def REQUIRE(boolval, error_msg):
    if not boolval:
        if not quiet: print "="*50, "\n\tERROR\n", "-"*50, "\n"
        if not quiet: print error_msg
        if not quiet: print "\n", "="*50
        raise Exception(error_msg)

DEVCFG_IMG = ['DAL_DEVCFG_IMG']

def hash_library(env, shared_objs):
    if 'USES_DEVCFG' not in env:
        return

    # Get the output path
    if 'HASHED_DYNAMIC_MODULE_DEST' not in env:
        output_path = os.path.join(env['BUILD_MS_ROOT'],
                        'bin',
                        env['SHORT_BUILDPATH'],
                        'dynamic_modules')
    else:
        output_path = env['HASHED_DYNAMIC_MODULE_DEST']

    # Create output folder if it doesn't exist
    if not os.path.exists(output_path):
        env.Execute(Mkdir(output_path))

    # Create build directory if it doesn't exist
    if not os.path.exists(env['BUILDPATH']):
        env.Execute(Mkdir(env['BUILDPATH']))

    #Get the hashed xml of the input files
    so_names = shared_objs
    if type(so_names) != list:
        so_names = [so_names]

    for shared_obj in so_names:
        filename = ntpath.basename(shared_obj)
        xml_file = os.path.join(env['BUILDPATH'], filename + '.xml')

        if env.GetOption('clean'):
            if os.path.exists(xml_file):
                env.Execute(Delete(xml_file))

        else:
            xml = build_devcfg_xml([shared_obj])

            with open(xml_file, "w") as f:
                f.write(xml)

            env.Depends(shared_obj, xml_file)


            # Add file to devcfg
            env.AddDevCfgInfo(
                DEVCFG_IMG,
                {
                    'soc_xml' : [xml_file]
                })

            env.Execute(Copy(output_path, shared_obj))

    # Create output folder if it doesn't exist
    if os.path.exists(output_path) and env.GetOption('clean'):
        env.Execute(Delete(output_path))


def exists(env):
    return env.Detect('hash_libs')

def generate(env):
    env.AddMethod(hash_library, "HashLib")

# Easily represent elf data
ELF_ADDR  = ('I', 4) #unsigned int 4byte
ELF_HALF  = ('H', 2) #unsigned short 2byte
ELF_OFF   = ('I', 4) #unsigned int 4byte
ELF_SWORD = ('i', 4) #signed integer word 4byte
ELF_WORD  = ('I', 4) #unsigned int 4byte
ELF_UC    = ('B', 1) #unsigned char 1byte

ELF_TYPES = ['None', 'REL', 'Excutable', 'Dynamic Module', 'CORE']

HASH_SEGMENT = 0x2000

# moar utils for cleaner code
def readelem(h, elem):
    return struct.unpack_from(elem[0], h.read(elem[1]))[0]

class ElfHasher(object):

    def __init__(self, hdl):
        self.h = hdl

    def hashsegs(self, format=None):
        hashes = []
        if not self.h:
            raise Exception("No handle set for ELF Parser")

        if not self.read_elf_header():
            raise Exception("Elf Header parse failed")

        if not self.read_program_headers():
            raise Exception("Program Header parse error")

        if not quiet: print '\n--Hashing segments--\n'
        for i in xrange(len(self.pheaders)):
            phent = self.pheaders[i]
            if not quiet:
                print 'Hashing segment %d at offset %d of size %d' %(i,phent['offset'], phent['memsz'])
            self.h.seek(phent['offset'])
            seghash = hashlib.sha256(self.h.read(phent['memsz']))
            if not quiet: print 'hashes :',  seghash.hexdigest()
            if format == 'devcfg':
                devcfg_seq = ', '.join('0x%x' % ord(x) for x in seghash.digest())
                devcfg_seq = devcfg_seq + ', '
                hashes.append(devcfg_seq)
            elif format in ['digest', 'raw', 'binary']:
                hashes.append(seghash.digest())
            else: #default is hexdigest
                hashes.append(seghash.hexdigest())

        return hashes



    def read_elf_header(s):
        h = s.h

        # 16 bytes is EIDENT starting with 0x7felf being the first 4 bytes
        s.ident =  struct.unpack_from("4s", h.read(16))[0]
        REQUIRE(s.ident == '\x7fELF', "Elf Identity mismatch %s" % repr(s.ident))
        if not quiet: print s.ident

        s.etype = readelem(h, ELF_HALF)
        if not quiet: print 'elf type ', s.etype, ELF_TYPES[s.etype]

        emachine = readelem(h, ELF_HALF)
        REQUIRE(emachine == 164, "Elf is not a Hexagon elf")
        if not quiet: print 'machine', emachine

        e = readelem(h, ELF_WORD)
        if not quiet: print 'version', e

        e = hex(readelem(h, ELF_ADDR))
        if not quiet: print 'entry', e

        # JS: ph offset is important
        s.phoff = readelem(h, ELF_OFF)
        if not quiet: print 'program header offset:', s.phoff, hex(s.phoff)

        e = hex(readelem(h, ELF_OFF))
        if not quiet: print 'setion header offset: ', e

        e = hex(readelem(h, ELF_WORD))
        if not quiet: print 'flags', e

        e = readelem(h,ELF_HALF)
        if not quiet: print 'elfheader_size', e

        s.phentsize = readelem(h, ELF_HALF)
        if not quiet: print 'ph entry size', s.phentsize

        s.phnum = readelem(h, ELF_HALF)
        if not quiet: print 'number of ph entries', s.phnum

        # Skip the rest as we don't need it for hashing segments
        return True

    def read_program_headers(s):
        # seek to the program header table (phoff)
        h = s.h
        h.seek(s.phoff)

        s.pheaders = []

        # read each entry
        for i in xrange(0, s.phnum):
            if not quiet: print '\n\tReading program header number', i
            phkeys = ['type', 'offset', 'vaddr', 'filesz', 'memsz', 'flags', 'align']
            phvals = struct.unpack_from('IIIIIIII', h.read(s.phentsize))

            phent = dict(zip(phkeys, phvals))
            if not quiet: print '\t', phent
            # Make sure we do not have a signed .so
            REQUIRE (phent['flags'] != HASH_SEGMENT,
                    "Elf must be unsigned. Hash segment found")

            s.pheaders.append(phent)

        return True


def elfhash(sopath):
    if not quiet: print '-'*50, '\nParsing', sopath, '\n'
    with open(sopath, 'rb') as filehandle:
        try:
            return ElfHasher(filehandle).hashsegs(format='devcfg')
        except:
            traceback.print_exc(file=sys.stdout)
    return None

def build_devcfg_xml(sopaths):
    xml = ['''<driver name="NULL">''']
    for sopath in sopaths:
        key = os.path.basename(sopath)
        hashes = elfhash(sopath)

        if not quiet: print "processing ", key
        xml.append('\n\t<device id="/statichashes/%s">' % key)
        xml.append('\n\t\t<props name="num_segments" type=DALPROP_ATTR_TYPE_UINT32>%d</props>' %len(hashes))
        xml.append('\n\t\t<props name="hash" type=DALPROP_ATTR_TYPE_BYTE_SEQ>')
        for h in hashes:
            xml.append(h)
        xml.append('end</props>')
        xml.append('\n\t</device>\n')

    xml.append('</driver>')
    return ''.join(xml)
