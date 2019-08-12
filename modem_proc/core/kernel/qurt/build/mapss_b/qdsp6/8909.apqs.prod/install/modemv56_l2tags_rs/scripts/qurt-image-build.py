#! /usr/bin/python

import getopt
import os
import string
import struct
import sys

from lib.qurt import *

class Elf_File:
    def __init__(self, infile=None, outfile=None):
        if infile:
            self.appname = infile[0]
            self.filename = infile[1]
            self.file = open(self.filename, 'rb')
            self.restart = infile[2]
        elif outfile:
            self.appname = ''
            self.filename = outfile
            self.file = open(self.filename, 'w+b')

class parse_generic:
    def __init__(self, data, fmt):
        tmp = struct.unpack(string.join(['<']+[e[1] for e in fmt],''), data)
        for j in range(len(fmt)):
            setattr(self, fmt[j][0], tmp[j])
        self.fmt = fmt
        self.temp = False
    def copy(self):
        from copy import copy as docopy
        return docopy(self)
    def output(self):
        if getattr(self,'temp',False):
            return ''
        return struct.pack(string.join(['<']+[e[1] for e in self.fmt],''), *tuple([getattr(self,e[0]) for e in self.fmt]))

''' Define formats of ELF structures '''
ehdr_format = [['e_ident'    , '16s', '%r'  ],
               ['e_type'     , 'H'  , '%u'  ],
               ['e_machine'  , 'H'  , '%u'  ],
               ['e_version'  , 'L'  , '%u'  ],
               ['e_entry'    , 'L'  , '0x%X'],
               ['e_phoff'    , 'L'  , '%u'  ],
               ['e_shoff'    , 'L'  , '%u'  ],
               ['e_flags'    , 'L'  , '0x%X'],
               ['e_ehsize'   , 'H'  , '%u'  ],
               ['e_phentsize', 'H'  , '%u'  ],
               ['e_phnum'    , 'H'  , '%u'  ],
               ['e_shentsize', 'H'  , '%u'  ],
               ['e_shnum'    , 'H'  , '%u'  ],
               ['e_shstrndx' , 'H'  , '%u'  ]]

phdr_format = [['p_type'  , 'L', '0x%X'],
               ['p_offset', 'L', '0x%X'],
               ['p_vaddr' , 'L', '0x%X'],
               ['p_paddr' , 'L', '0x%X'],
               ['p_filesz', 'L', '0x%X'],
               ['p_memsz' , 'L', '0x%X'],
               ['p_flags' , 'L', '0x%X'],
               ['p_align' , 'L', '0x%X']]

shdr_format = [['sh_name'     , 'L', '0x%X'],
               ['sh_type'     , 'L', '0x%X'],
               ['sh_flags'    , 'L', '0x%X'],
               ['sh_addr'     , 'L', '0x%X'],
               ['sh_offset'   , 'L', '0x%X'],
               ['sh_size'     , 'L', '0x%X'],
               ['sh_link'     , 'L', '0x%X'],
               ['sh_info'     , 'L', '0x%X'],
               ['sh_addralign', 'L', '0x%X'],
               ['sh_entsize'  , 'L', '0x%X']]

no_adjust = None

# zstr = str(bytearray(0x10000))
# Can't use above line with Python 2.4, try this:
zstr = '\0' * 0x10000

def padzero(outfile, length):
    while length > len(zstr):
        outfile.write(zstr)
        length -= len(zstr)
    if length > 0:
        outfile.write(zstr[0:length])

def copydata(outfile, infile, seekloc, length):
    infile.seek(seekloc)
    while length > 0x10000:
        outfile.write(infile.read(0x10000))
        length -= 0x10000
    if length > 0:
        outfile.write(infile.read(length))

def new_strtab_entry(f, s):
    s = s + '\0'
    f.strtab = f.strtab + s
    return len(f.strtab)-len(s)

def aligndown(n, a):
    return n & ~(a-1)

def alignup(n, a):
    return (n+(a-1)) & ~(a-1)

def string_lookup(f, n):
    s = f.strtab
    e = s.index('\0', n)
    return s[n:e]

''' Look through the list for segments which overlap or touch '''
''' Elements are sorted by the start address '''
''' If they overlap, combine them and combine permissions '''
''' If they touch without overlapping, only combine them if permissions are identical '''
''' Edit the list and return True if we were able to optimize '''
''' Leave the list and return False if we were not able to optimize '''
def seg_optimize(segs):
    for n in range(len(segs)-1):
        if segs[n][1] > segs[n+1][0]:
            ''' n and n+1 overlap '''
            if segs[n][2] != segs[n+1][2]:
                raise Exception('Overlapping segments with different memory maps')
            ''' n and n+1 overlap with the same offset '''
            ''' Combine them, including permissions, and return '''
            combined = (segs[n][0],
                        max(segs[n][1], segs[n+1][1]),
                        segs[n][2],
                        segs[n][3] or segs[n+1][3],
                        segs[n][4] or segs[n+1][4],
                        segs[n][5] or segs[n+1][5])
            segs = segs[:n] + [combined] + segs[n+2:]
            return (False, segs)
        elif segs[n][1] == segs[n+1][0]:
            ''' n and n+1 touch '''
            if segs[n][2:] == segs[n+1][2:]:
                ''' n and n+1 touch and have the same offset and permissions '''
                ''' combine them and return '''
                segs = segs[:n] + [(segs[n][0], segs[n+1][1]) + segs[n][2:]] + segs[n+2:]
                return (False, segs)
        else:
            ''' n and n+1 have separation between them '''
            pass
    return (True, segs)

''' New version of write_eip_info, writes header, variable length records, and string table '''
'''
    Top level header (fixed length, one only):
      uint32_t magic_number (0x32504945)
      uint16_t version_number
      uint16_t number_of_apps
    Second level header (fixed length, one per app):
      uint16_t first_memsection_number
      uint16_t number_of_memsections
      uint16_t entry_memsection_number
      uint16_t string_table_offset_of_app_name
    Third level header (fixed length, one per memsection):
      uint32_t virtual_address_start
      uint32_t physical_address_start
      uint32_t length_in_bytes
      uint32_t permissions
    String table:
      Zero terminated strings for app names
      (App names which are restartable are prepended with '>' greater than sign)
      (App names which are not-restartable are not prepended)
'''
def build_eip_info(f, infiles, island_moves, opt_mappings):
    for i in infiles:
        segs = []
        for q in i.phdr:
            p = q.outcopy
            ''' Add the memory described in this program header '''
            virt_start = aligndown(p.p_vaddr, 0x1000)
            virt_end = alignup(p.p_vaddr + p.p_memsz, 0x1000)
            virt_offset = p.p_paddr - p.p_vaddr
            virt_exec = ((p.p_flags & 1) != 0)
            virt_write = ((p.p_flags & 2) != 0)
            if virt_start != virt_end and p.p_type == 1:
                segs += [(virt_start, virt_end, virt_offset, virt_exec, virt_write, False)]
        done, segs = (False, sorted(segs))
        while not done:
            done, segs = seg_optimize(segs)
        i.segs = segs
    if opt_mappings:
        # New style of doing things; use the optimized mappings we already computed
        for asid in range(1,len(infiles)):
            segs = []
            for M in opt_mappings:
                if M.ASID == asid:
                    segs += [(M.vpage << 12, (M.vpage+M.size) << 12, (M.ppage-M.vpage) << 12, (M.X != 0), (M.W != 0), M.island)]
            infiles[asid].segs = segs
    top_hdr = (0x32504945,1,len(infiles))
    eip_data = struct.pack('<LHH', *top_hdr)
    strtab = ''
    for m in island_moves:
        strtab += struct.pack('<LLL', *m)
    strtab += struct.pack('<L', 0)
    secno = 0
    for i in infiles:
        segs = i.segs
        second_hdr = (secno, len(segs), secno, len(strtab))
        eip_data += struct.pack('<HHHH', *second_hdr)
        secno += len(segs)
        if i.restart:
            strtab += '>'
        strtab += i.appname + '\0'
    for i in infiles:
        segs = i.segs
        segs = sorted(segs, key=lambda s: (s[5], s[0]+s[2]))
        for s in segs:
            perms = 1           # Read permission
            ''' Add in exec and write permissions '''
            if s[3]:
                perms += 4        # Execute permission
            if s[4]:
                perms += 2        # Write permission
            if s[5]:
                perms += 0x100    # Island status
            third_hdr = (s[0], s[0]+s[2], s[1]-s[0], perms)
            eip_data += struct.pack('<LLLL', *third_hdr)
    eip_data += strtab
    return eip_data

def parse_elf_header(f):
    try:
        f.file.seek(0)
        f.ehdr = parse_generic(f.file.read(52), ehdr_format)
        if f.ehdr.e_ehsize != 52:
            raise
        if f.ehdr.e_phentsize != 32:
            raise
        if f.ehdr.e_shentsize != 40:
            raise
    except:
        raise Exception('Error in ELF header for file %s' % (f.filename))

def parse_program_headers(f):
    try:
        f.file.seek(f.ehdr.e_phoff)
        f.phdr = []
        for j in range(f.ehdr.e_phnum):
            f.phdr += [parse_generic(f.file.read(f.ehdr.e_phentsize), phdr_format)]
    except:
        raise Exception('Error in ELF program headers for file %s' % (f.filename))

def parse_section_headers(f):
    try:
        f.file.seek(f.ehdr.e_shoff)
        f.shdr = []
        for j in range(f.ehdr.e_shnum):
            f.shdr += [parse_generic(f.file.read(f.ehdr.e_shentsize), shdr_format)]
    except:
        raise Exception('Error in ELF section headers for file %s' % (f.filename))

def read_string_table(f):
    try:
        f.file.seek(f.shdr[f.ehdr.e_shstrndx].sh_offset)
        f.strtab = f.file.read(f.shdr[f.ehdr.e_shstrndx].sh_size)
        if len(f.strtab) != f.shdr[f.ehdr.e_shstrndx].sh_size:
            raise
    except:
        raise Exception('Error in ELF string table for file %s' % (f.filename))

def read_elf_info(f):
    parse_elf_header(f)
    parse_program_headers(f)
    parse_section_headers(f)
    read_string_table(f)
    for s in f.shdr:
        if string_lookup(f, s.sh_name) == '.start':
            f.start_addr = s.sh_addr
            f.start_size = s.sh_size
            break
    if not hasattr(f, 'start_addr'):
        raise Exception('File %s: cannot find .start section' % f.filename)

def process_input_files(infiles, physaddr):
    curloc = infiles[0].ehdr.e_ehsize
    for f in infiles:
        for p in f.phdr:
            curloc += f.ehdr.e_phentsize
    for f in infiles:
        fbot = 0
        ftop = 0
        falign = 0
        for p in f.phdr:
            if p.p_type == 1 and p.p_filesz > 0:
                if ftop == 0:
                    fbot = p.p_offset
                    ftop = p.p_offset+p.p_filesz
                    falign = p.p_align
                else:
                    fbot = min(fbot, p.p_offset)
                    ftop = max(ftop, p.p_offset+p.p_filesz)
                    falign = max(falign, p.p_align)
        if falign == 0:
            falign = 1
        ''' Need to increase curloc until it is congruent to fbot modulo falign '''
        curloc += ((fbot%falign)+falign-(curloc%falign))%falign
        f.output_offset = curloc
        f.copy_bot = fbot
        f.copy_top = ftop
        f.filealign = falign
        curloc += (ftop-fbot)
    ''' Above figured out where we put all of the data in the output file '''
    ''' Now figure out where we put it in memory '''
    curloc = physaddr
    for f in infiles:
        mbot = 0
        mtop = 0
        malign = 0
        for p in f.phdr:
            ''' Only examining program headers of type LOAD and non-zero size '''
            if p.p_type == 1 and p.p_memsz > 0:
                if mtop == 0:
                    mbot = p.p_paddr
                    mtop = p.p_paddr+p.p_memsz
                    malign = p.p_align
                else:
                    mbot = min(mbot, p.p_paddr)
                    mtop = max(mtop, p.p_paddr+p.p_memsz)
                    malign = max(malign,p.p_align)
        if malign == 0:
            malign = 1
        ''' Need to increase curloc until it is congruent to mbot modulo malign '''
        curloc += ((mbot%malign)+malign-(curloc%malign))%malign
        f.output_physaddr = curloc
        f.vmem_bot = mbot
        f.vmem_top = mtop
        f.memalign = malign
        curloc += (mtop-mbot)
        ''' Round up to next 1MB boundary '''
        curloc = (curloc+0xFFFFF) & ~0xFFFFF

def populate_output_file(f, infiles):
    firstfile = infiles[0]
    lastfile = infiles[len(infiles)-1]
    f.phdr = []
    for i in infiles:
        for p in i.phdr:
            np = p.copy()
            p.outcopy = np
            if np.p_type == 0x6474E551:
                ''' GNU_STACK program header.  We strip these, they are useless to us '''
                continue
            if np.p_type == 6:
                ''' If the program header is of type PHDR, leave it alone '''
                pass
            else:
                if np.p_offset >= i.copy_bot and np.p_offset <= i.copy_top:
                    np.p_offset = i.output_offset + (np.p_offset - i.copy_bot)
                if np.p_paddr >= i.vmem_bot and np.p_paddr <= i.vmem_top:
                    np.p_paddr = i.output_physaddr + (np.p_paddr - i.vmem_bot)
            f.phdr = f.phdr + [np]
    f.shdr = [firstfile.shdr[0]]
    f.strtab = ''
    new_strtab_entry(f, '\0.shstrtab')
    cpad_loc = lastfile.output_offset + (lastfile.copy_top - lastfile.copy_bot)
    cpad = ''
    for i in infiles:
        for s in i.shdr:
            if s.sh_type != 0:
                if (s.sh_flags & 2) != 0 or string_lookup(i, s.sh_name) == '.comment':
                    sp = s.copy()
                    if sp.sh_offset >= i.copy_bot and sp.sh_offset <= i.copy_top:
                        sp.sh_offset = i.output_offset + (sp.sh_offset - i.copy_bot)
                    if i is firstfile:
                        ''' Don't rename sections in the OS image '''
                        sp.sh_name = new_strtab_entry(f, string_lookup(i, s.sh_name))
                    else:
                        sp.sh_name = new_strtab_entry(f, string_lookup(i, s.sh_name)+'.'+i.appname)
                    if (s.sh_flags & 2) == 0:
                        ''' Comment section '''
                        i.file.seek(s.sh_offset)                # Seek in input file to .comment data
                        tempstr = i.file.read(s.sh_size)        # Read .comment data from input file
                        if len(tempstr) != s.sh_size:           # Check for premature EOF
                            raise                                 #  and fail if so
                        sp.sh_offset = cpad_loc + len(cpad)     # Point output .comment at output loc
                        cpad += tempstr                         # Accumulate .comment sections
                    f.shdr = f.shdr + [sp]
                else:
                    pass
    while (cpad_loc + len(cpad)) % 4 != 0:
        cpad += '\0'
    new_strtab_entry(f, '\0')
    f.cpad = cpad
    f.shdr += [firstfile.shdr[firstfile.ehdr.e_shstrndx]]
    f.shdr[len(f.shdr)-1].sh_name = 1
    f.ehdr = firstfile.ehdr.copy()
    f.ehdr.e_phoff = 52
    f.ehdr.e_entry = firstfile.output_physaddr
    f.ehdr.e_shoff = cpad_loc + len(cpad)
    f.ehdr.e_phnum = len(f.phdr)
    f.ehdr.e_shnum = len(f.shdr)
    f.ehdr.e_shstrndx = len(f.shdr)-1
    # Begin temporary fix.  This should go away once this gets
    #  rearchitected better.
    # The effect here is to modify the build to strip the
    #  temp program and section headers.
    f.ehdr.e_phnum = len([p for p in f.phdr if not p.temp])
    shdr = [s for s in f.shdr if not s.temp]
    f.ehdr.e_shnum = len(shdr)
    f.ehdr.e_shstrndx = len(shdr) - 1
    f.shdr = shdr + [s for s in f.shdr if s.temp]

def dump_output_file(f, infiles):
    f.file.write(f.ehdr.output())
    for p in sorted(f.phdr, key=lambda p: p.p_paddr):
        f.file.write(p.output())
    for i in infiles:
        padzero(f.file, i.output_offset-f.file.tell())
        copydata(f.file, i.file, i.copy_bot, i.copy_top-i.copy_bot)
    f.file.write(f.cpad)
    f.shdr[f.ehdr.e_shstrndx].sh_offset = f.file.tell()+f.ehdr.e_shnum*f.ehdr.e_shentsize
    f.shdr[f.ehdr.e_shstrndx].sh_size = len(f.strtab)
    for s in f.shdr:
        f.file.write(s.output())
    f.file.write(f.strtab)
    f.file.flush()

def adjust_input_file(i, newnamefunc):
    outname = newnamefunc(i.filename)
    inplace = (outname == i.filename)
    # filemode = ('r+b' if inplace else 'wb')
    # Can't use above line with Python 2.4, try this:
    filemode = ('wb','r+b')[inplace]
    nfile = open(outname, filemode)
    i.ehdr.e_entry = i.output_physaddr
    nfile.write(i.ehdr.output())
    for p in i.phdr:
        p.p_paddr = p.outcopy.p_paddr
        nfile.write(p.output())
    if not inplace:
        ''' Copy rest of input file. '''
        # i.file.seek(0, os.SEEK_END)
        # Can't use above line with Python 2.4, try this:
        i.file.seek(0, 2)
        copydata(nfile, i.file, nfile.tell(), i.file.tell() - nfile.tell())
    nfile.flush()
    nfile.close()

def splitname(s, restart):
    names = s.split('=')
    if len(names) == 1:
        return (os.path.basename(s).replace('elf','pbn'), s, restart)
    if len(names) == 2:
        return (names[0], names[1], restart)
    raise Exception("input file argument cannot contain multiple '=' characters")

def parse_args(argv):
    global no_adjust
    opts, args = getopt.gnu_getopt(argv[1:], "eno:p:t:R:", ["assert_free_microimage="])
    optdict = dict()
    for opt, arg in opts:
        optdict[opt] = optdict.get(opt,[]) + [arg]
    no_adjust = '-n' in optdict
    outfilename = optdict.get('-o',[])
    physaddr = optdict.get('-p',[])
    restarts = optdict.get('-R', [])
    args = args+restarts
    if len(args) == 0:
        raise Exception('must provide at least one input file')
    if len(outfilename) != 1:
        raise Exception('must provide exactly one output file (-o)')
    if len(physaddr) != 1:
        raise Exception('must provide exactly one physical address (-p)')
    try:
        physaddr = long(physaddr[0], 0)
        if physaddr < 0 or physaddr >= pow(2,32):
            raise
        if physaddr & 0xFFF:
            raise
    except:
        raise Exception('physical address (-p) must be a 32-bit unsigned integer on a 4K boundary')
    free_uimg = 0
    for s in optdict.get('--assert_free_microimage', []):
        try:
            free_uimg = max(free_uimg, convert_size(s))
        except ValueError:
            raise Exception('Invalid size on command line: %s' % s)
    args = [splitname(arg, arg in restarts) for arg in args]
    return (args, outfilename[0], physaddr, free_uimg)

def rename_input_file(ifilename):
    ''' Replace the last dot (.) in the name with _reloc. '''
    ''' If no dot, this returns unchanged... '''
    return '_reloc.'.join(ifilename.rsplit('.',1))

class PhysPool:
    def __init__(self, tuple):
        self.ranges = []
        self.name = tuple[0].split('\0')[0]
        for base,size in zip(tuple[1::2],tuple[2::2]):
            if size == 0:
                break
            if (size & 0xFFF) != 0:
                raise Exception("Pool %s has size which is not 4K aligned" % self.name)
            self.ranges.append([base,size >> 12])
    def alloc_pages(self, size, align):
        # Allocate pages from this PhysPool
        # For now, don't get fancy.  We only allocate in order from large->small,
        #  and so we should always be able to take the first fit.
        # Returns the page number
        for r in self.ranges:
            if r[1] >= size:
                r[0] += size
                r[1] -= size
                return r[0]-size
        raise Exception("Unable to allocate %u pages from phys pool %s" % (size, self.name))

class HexagonMapping:
    """Class for representing a Hexagon TLB entry."""
    def __init__(self):
        pass
    def get_TLB_entry(self):
        """Return the 64-bit encoded TLB entry."""
        # Get the size in place first
        ret = 1
        while ret*ret < self.size:
            ret *= 2
        if ret*ret != self.size:
            raise Exception('Internal error -- locked TLB entry of illegal size')
        ret |= (1L << 63)       # Set the valid bit
        ret |= self.G << 62
        if self.ppage & 0x800000:
            ret |= (1L << 61)
        ret |= self.A1A0 << 59
        ret |= self.ASID << 52
        ret |= self.vpage << 32
        ret |= self.X << 31
        ret |= self.W << 30
        ret |= self.R << 29
        ret |= self.U << 28
        ret |= self.C << 24
        ret |= (self.ppage & 0x7FFFFF) << 1
        if self.ASID == 0 and self.X == 1 and self.W == 0 and self.C != 4 and self.C != 6:
            ret |= (1 << 28)
        return ret
    def set_from_TLB_entry(self, v, locked=True, atomic=True, reloc=False):
        """Set the fields of the mapping based on the 64-bit TLB entry (v)."""
        if (v & (1L << 63)) == 0:
            raise Exception('Tried to use a TLB entry not set to valid')
        tmp = (v & 0x7F)
        tmp = (tmp & -tmp)    # Sets to 0, 1, 2, 4, 8, 16, 32, or 64
        sz = tmp * tmp       # Sets to 0, 1, 4, 16, 64, 256, 1024, or 4096
        if sz == 0:
            raise Exception('Tried to use a TLB entry with invalid size')
        self.island = False
        self.reclaim = False
        self.atomic = atomic
        self.locked = locked
        self.reloc = reloc
        self.physpool = None
        self.overlay = 0
        self.G    = ((v >> 62) & 0x01)
        self.A1A0 = ((v >> 59) & 0x03)
        self.ASID = ((v >> 52) & 0x7F)
        self.X    = ((v >> 31) & 0x01)
        self.W    = ((v >> 30) & 0x01)
        self.R    = ((v >> 29) & 0x01)
        self.U    = ((v >> 28) & 0x01)
        self.C    = ((v >> 24) & 0x0F)
        self.vpage = ((v >> 32) & 0xFFFFF) & (~(sz-1))
        self.ppage = ((v >>  1) & 0x7FFFFF) & (~(sz-1))
        if ((v >> 61) & 0x01):
            # EP bit is set, ppage gets high bit set
            self.ppage += 0x800000
        self.size = sz
        return self
    def contains(self, other):
        """Return True if self is a proper superset of other."""
        my_attr = (self.atomic, self.locked, self.G, self.A1A0, self.ASID,
                   self.X, self.W, self.R, self.U, self.C, self.reloc)
        his_attr = (other.atomic, other.locked, other.G, other.A1A0, other.ASID,
                    other.X, other.W, other.R, other.U, other.C, other.reloc)
        if my_attr != his_attr:
            return False
        if self.vpage - self.ppage != other.vpage - other.ppage:
            return False
        if other.vpage < self.vpage:
            return False
        if other.vpage + other.size > self.vpage + self.size:
            return False
        return True
    def overlaps(self, other):
        """Return True if self and other overlap in virtual space."""
        if self.vpage + self.size <= other.vpage:
            return False
        if other.vpage + other.size <= self.vpage:
            return False
        return True
    def fix_to_TLB_entries(self, maxsize=0x1000):
        """Takes a mapping of any size and returns an equivalent list of mappings of legal sizes."""
        from copy import copy
        ret = []
        off = 0            # Offset from beginning
        sz = self.size     # Number of pages left
        while sz > 0:
            this_sz = 0x1000
            while this_sz > 1:
                if this_sz <= sz and this_sz <= maxsize:
                    if ((self.vpage+off) & (self.ppage+off) & (this_sz-1)) == 0:
                        break
                this_sz = this_sz >> 2
            tmp = copy(self)
            tmp.vpage += off
            tmp.ppage += off
            tmp.size = this_sz
            ret.append(tmp)
            sz -= this_sz
            off += this_sz
        return ret
    def __str__(self):
        return '<%05X->%06X [%04X] G:%u A:%u ASID:%u X:%u W:%u R:%u U:%u C:%X %s %s %s %s>' % (self.vpage,
                                                                                               self.ppage,
                                                                                               self.size,
                                                                                               self.G,
                                                                                               self.A1A0,
                                                                                               self.ASID,
                                                                                               self.X,
                                                                                               self.W,
                                                                                               self.R,
                                                                                               self.U,
                                                                                               self.C,
                                                                                               ('unlocked','locked')[self.locked],
                                                                                               ('non-atomic','atomic')[self.atomic],
                                                                                               ('non-island','island')[self.island],
                                                                                               ('non-reloc','reloc')[self.reloc])

class QurtElfPatch:
    """Class for doing ELF patch processing on the merged ELF file."""
    #
    # When we call start() on this class, the class becomes basically like
    #  a 4GB bytearray in the ELF's virtual address space.
    # Slices of the virtual address space can be accessed using standard
    #  slice notation.  For instance, to access the 4 bytes at address ADDR
    #  you can use self[ADDR:ADDR+4].
    # To read the same 4 bytes into X as an unsigned 32-bit quantity:
    #  X = struct.unpack('<L', self[ADDR:ADDR+4])
    # To write the same 4 bytes with X which is an unsigned 32-bit quantity:
    #  self[ADDR:ADDR+4] = struct.pack('<L', X)
    #
    # Note that accesses outside of the defined virtual image of the ELF cause
    #  an exception.  Portions of the virtual image which are not backed by
    #  file data (such as bss sections, etc.) can be read, but return zeroes;
    #  attempts to write non-zero data to those cause an exception.
    #
    def __init__(self):
        self.all_mappings = []
        self.opt_mappings = []
        self.island_moves = []
        self.load_offset = None
        pass
    def __getitem__(self, key):
        if isinstance(key,slice):
            sz = key.stop - key.start
            addr = key.start
        else:
            sz = key[1]
            addr = key[0]
        ret = ''
        while sz > 0:
            this_sz = 0
            for p in self.phdr:
                if p.p_type == 1 and addr >= p.p_vaddr:
                    if addr < p.p_vaddr + p.p_filesz:
                        # The next byte we need to read is in this segment
                        this_sz = min(sz, (p.p_vaddr + p.p_filesz) - addr)
                        this_offset = p.p_offset + (addr - p.p_vaddr)
                        self.file.seek(this_offset)
                        tmp = self.file.read(this_sz)
                        this_sz = len(tmp)
                        addr += this_sz
                        sz -= this_sz
                        ret += tmp
                        break
            if this_sz == 0:
                raise Exception('Cannot read %u bytes at vaddr %X' % (key.stop - key.start, key.start))
        return ret
    def __setitem__(self, key, value):
        sz = len(value)
        if isinstance(key,slice):
            addr = key.start
        else:
            addr = key[0]
        while sz > 0:
            this_sz = 0
            for p in self.phdr:
                if p.p_type == 1 and addr >= p.p_vaddr:
                    if addr < p.p_vaddr + p.p_filesz:
                        # The next byte we need to read is in this segment
                        this_sz = min(sz, (p.p_vaddr + p.p_filesz) - addr)
                        this_offset = p.p_offset + (addr - p.p_vaddr)
                        self.file.seek(this_offset)
                        self.file.write(value[:this_sz])
                        self.file.flush()
                        addr += this_sz
                        sz -= this_sz
                        value = value[this_sz:]
                        break
            if this_sz == 0:
                raise Exception('Cannot read %u bytes at vaddr %X' % (key.stop - key.start, key.start))
    def startr(self, file):
        self.file = file
        parse_elf_header(self)
        parse_program_headers(self)
        parse_section_headers(self)
        read_string_table(self)
    def start(self, filename):
        self.file = open(filename, 'r+b')
        parse_elf_header(self)
        parse_program_headers(self)
        parse_section_headers(self)
        read_string_table(self)
    def unpack_from(self, fmt, offset):
        return struct.unpack(fmt, self[(offset,struct.calcsize(fmt))])
    def finish(self):
        self.file.flush()
        self.file.close()
    def read_EIP(self, vaddr):
        self.eip_location = vaddr
        eip_header = self.unpack_from('<'+'L'*12, vaddr)
        self.eip_zero             = eip_header[0]     # See kernel/asm/elf_info_patch.inc for the format
        self.eip_version          = eip_header[1]     #  of the EIP area
        self.eip_size             = eip_header[2]
        self.eip_flags            = eip_header[3]
        self.eip_addr_phys_island = eip_header[4]
        self.eip_addr_tlb_dump    = eip_header[5]
        self.eip_size_tlb_dump    = eip_header[6]
        self.eip_addr_mmap_table  = eip_header[7]
        self.eip_addr_tlblock     = eip_header[8]
        self.eip_addr_tlb_relocs  = eip_header[9]
        self.eip_addr_boot_maps   = eip_header[10]
        self.eip_addr_quick_mmu   = eip_header[11]
    def check_EIP(self):
        # Raise an exception if the EIP looks wrong.
        if self.eip_zero != 0 or self.eip_version != 8:
            raise Exception('ELF Info Patch header is not valid.  Cannot complete image build.')
    def advanced_EIP(self):
        # Return True if advanced memory handling is requested in the EIP flags
        return ((self.eip_flags & 1) != 0)
    def write_EIP(self, data):
        if len(data) > self.eip_size:
            raise Exception('ELF Info Patch area overflow.  Cannot complete image build.')
        self[(self.eip_location,len(data))] = data
    def read_boot_mappings(self):
        # Get the boot mappings from the ELF file
        addr = self.eip_addr_boot_maps
        boot_maps = []
        while True:
            tmp = self.unpack_from('<QLL', addr)
            if tmp[0] == 0:
                break
            boot_maps.append(tmp)
            addr += 16
        self.boot_mappings = boot_maps
    def read_phys_island(self):
        # Get the phys island data from the ELF file
        addr = self.eip_addr_phys_island
        island_info = []
        while True:
            tmp = self.unpack_from('<L', addr)[0]
            if tmp == 0xFFFFFFFF:
                break
            addr += 4
            island_info.append(tmp)
        self.phys_island_regions = []
        while island_info:
            self.phys_island_regions.append([island_info[0],island_info[1],island_info[2]])
            island_info = island_info[3:]
    def read_tlb_dump(self):
        self.compare_tlbs = []
        if self.tlbdump_usage.value == 'ignore':
            return None
        addr = self.eip_addr_tlb_dump
        tlb_dump = []
        for i in range(self.eip_size_tlb_dump):
            tmp = self.unpack_from('<Q', addr)[0]
            if (tmp & (1L << 63)):
                if self.tlbdump_usage.value == 'compare':
                    self.compare_tlbs.append(tmp)
                else:
                    tlb_dump.append(HexagonMapping().set_from_TLB_entry(tmp))
            addr += 8
        self.tlb_dump = tlb_dump
        self.all_mappings.extend(tlb_dump)
    def do_tlb_compare(self, v, vn):
        if not self.compare_tlbs:
            return
        original = self.compare_tlbs
        updated = v+vn
        total = sorted(original+updated)
        for t in total:
            if not t in updated:
                print 'Original only: %s' % HexagonMapping().set_from_TLB_entry(t)
            if not t in original:
                print 'Updated  only: %s' % HexagonMapping().set_from_TLB_entry(t)
    def write_tlb_dump(self, v, vn):
        addr = self.eip_addr_tlb_dump
        if len(v)+len(vn) > self.eip_size_tlb_dump:
            raise Exception('Build generates %u static TLB entries -- only %u are allowed' %
                            (len(v)+len(vn), self.eip_size_tlb_dump))
        for tmp in v:
            self[(addr,8)] = struct.pack('<Q', tmp)
            addr += 8
        for tmp in vn:
            self[(addr,8)] = struct.pack('<Q', tmp)
            addr += 8
        self[(self.eip_addr_tlb_relocs,4)] = struct.pack('<L', len(v))
    def write_tlb_reclaim(self, mask):
        if self.qurtos_tlb_reclaim:
            self[(self.qurtos_tlb_reclaim,16)] = struct.pack('<QQ',
                                                            (mask >>  0) & ((1L << 64) - 1),
                                                            (mask >> 64) & ((1L << 64) - 1))
    def write_quick_mmu(self, i):
        addr = self.eip_addr_tlb_dump
        pg = (addr >> 12)
        for t in self.opt_mappings:
            if pg >= t.vpage and pg < t.vpage + t.size and t.locked:
                val = (addr - (t.vpage << 12)) + (t.ppage << 12)
                if t.reloc:
                    val += 1
                self[(self.eip_addr_quick_mmu,4)] = struct.pack('<L', val)
    def read_mmap_table(self):
        lock_count = self.unpack_from('<L', self.eip_addr_tlblock)[0]
        addr = self.eip_addr_mmap_table
        mmap_table_locked = []
        mmap_table_unlocked = []
        while True:
            tmp = self.unpack_from('<Q', addr)[0]
            if tmp == 0:
                break
            addr += 8
            if len(mmap_table_locked) < lock_count:
                if self.tlbdump_usage.value == 'compare':
                    self.compare_tlbs.append(tmp)
                mmap_table_locked.append(HexagonMapping().set_from_TLB_entry(tmp, locked=True))
            else:
                mmap_table_unlocked.append(HexagonMapping().set_from_TLB_entry(tmp, locked=False, atomic=False))
        self.mmap_table_locked = mmap_table_locked
        self.mmap_table_unlocked = mmap_table_unlocked
        self.all_mappings.extend(mmap_table_locked)
        self.all_mappings.extend(mmap_table_unlocked)
    def read_pool_configs(self):
        self.pools = []
        addr = self.pool_configs
        if addr == 0:
            return
        while True:
            tmp = self.unpack_from('<32s32L', addr)
            if tmp[0].startswith('\0'):
                break
            addr += 160
            self.pools.append(PhysPool(tmp))

    def do_section_overrides(self, infiles):
        for i in infiles:
            for s in i.shdr:
                s.root_elf = (i == infiles[0])

        for i in infiles:
            for s in i.shdr:
                for o in self.section_info:
                    if o.match(o,None):
                        for k,v in o.attrs.items():
                            v(s)
        for i in infiles:
            for s in i.shdr:
                for o in self.section_info:
                    if o.match(o,string_lookup(i,s.sh_name)):
                        for k,v in o.attrs.items():
                            v(s)

        for i in infiles:
            for s in i.shdr:
                s.need_map = (s.sh_flags & 2) != 0 and s.sh_size > 0
                mapattr = getattr(s,'mapping','normal')
                s.overlay = (0,1)[mapattr == 'overlay']

        if self.gap_reclaim.value == 'unrestricted':
            #
            #  We've been asked to do unrestricted gap reclaiming.
            #  For each of our ELF files, fill in the holes in
            #   the virtual address space with read-only gaps.
            #  Do this for both the program headers and the
            #   section headers.
            #

            for i in infiles:
                # Get the sorted list of phdrs which are of type 1 (LOAD)
                #  and of non-zero memory size
                zp = sorted([p for p in i.phdr if p.p_type == 1 and p.p_memsz > 0],
                            key=lambda p: p.p_vaddr)
                # Add a bogus entry that starts 1MB past the last entry; this
                #  will cause a 1MB gap to be added at the end.
                p = zp[-1].copy()
                p.p_vaddr = p.p_vaddr + p.p_memsz + 0x100000
                p.p_paddr = p.p_paddr + p.p_memsz + 0x100000
                zp.append(p)
                for p0,p1 in zip(zp[:-1],zp[1:]):
                    gap = p1.p_vaddr - (p0.p_vaddr + p0.p_memsz)
                    if gap < 0:
                        raise Exception("Overlapping LOAD program headers?")
                    if gap > 0:
                        # There is a gap between two LOAD program headers
                        #  in virtual memory.
                        # Add a new phdr to fill the gap, with zero file size
                        #  and with read-only permissions.  This is just our
                        #  internal representation -- it shouldn't survive
                        #  into the output ELF file.
                        # To make sure we can avoid propagating it to the
                        #  output ELF, set its 'temp' attribute to True.
                        p = p1.copy()
                        p.p_vaddr -= gap
                        p.p_paddr -= gap
                        p.p_memsz = gap
                        p.p_filesz = 0
                        p.p_flags = 0
                        p.temp = True
                        i.phdr.append(p)

                # Get the sorted list of shdrs which have the ALLOC flag set
                #  and which are of non-zero size
                zs = sorted([s for s in i.shdr if s.need_map],
                            key=lambda s: s.sh_addr)
                # Add a bogus entry that starts 1MB past the last entry; this
                #  will cause a 1MB gap to be added at the end.
                s = zs[-1].copy()
                s.sh_addr = s.sh_addr + s.sh_size + 0x100000
                zs.append(s)
                for s0,s1 in zip(zs[:-1],zs[1:]):
                    gap = s1.sh_addr - (s0.sh_addr + s0.sh_size)
                    if gap < 0:
                        raise Exception("Overlapping ALLOC section headers?")
                    if gap > 0:
                        # There is a gap between two ALLOC section headers
                        #  in virtual memory.
                        # Add a new shdr to fill the gap, with NOBITS/ALLOC
                        #  and with read-only permissions.  This is just our
                        #  internal representation -- it shouldn't survive
                        #  into the output ELF file.
                        # To make sure we can avoid propagating it to the
                        #  output ELF, set its 'temp' attribute to True.
                        s = s1.copy()
                        s.sh_addr -= gap
                        s.sh_size = gap
                        s.sh_type = 8        # NOBITS
                        s.sh_flags &= ~5     # No write/no execute
                        s.temp = True
                        s.sh_name = new_strtab_entry(i, '$GAP%08X' % s.sh_addr)
                        s.cache_policy = -1
                        s.overlay = -1
                        s.need_map = True
                        s.physpool = '*'
                        i.shdr.append(s)

    def synthesize_elf_mappings(self, infiles):
        asid = 0
        for i in infiles:
            for s in i.shdr:
                if (s.sh_flags & 2) != 0 and s.sh_size > 0:
                    sname = string_lookup(i, s.sh_name)
                    match = []
                    for p in self.phdr:
                        if p.p_type == 1 and s.sh_addr >= p.p_vaddr:
                            if s.sh_addr + (s.need_map * s.sh_size) <= p.p_vaddr + p.p_memsz:
                                match.append(p)
                    if len(match) != 1:
                        raise Exception('Cannot find unique PHDR for section %s' % sname)
                    p = match[0]
                    paddr = s.sh_addr + p.p_paddr - p.p_vaddr
                    perm_write = ((s.sh_flags & 1) != 0)
                    perm_exec = ((s.sh_flags & 4) != 0)
                    skip = False
                    for z in self.boot_mappings:
                        if s.sh_addr >= z[1] and s.sh_addr < z[2]:
                            skip = True
                    if not skip:
                        self.add_elf_mapping(s, s.sh_addr, s.sh_size, paddr, perm_write, perm_exec, asid, sname.endswith('.island'), i)
            asid += 1
    def add_elf_mapping(self, section, vaddr, size, paddr, perm_write, perm_exec, asid, island, infile):
        # Round beginning down to 4K boundary
        size += (vaddr & 0xFFF)
        paddr -= (vaddr & 0xFFF)
        vaddr -= (vaddr & 0xFFF)
        # Convert size to page count
        pgcnt = (size + 0xFFF) >> 12
        if (paddr & 0xFFF) != 0:
            raise Exception('Virtual/physical address mis-alignment')
        if self.load_offset == None:
            self.load_offset = paddr-vaddr
        vpage = vaddr >> 12
        ppage = paddr >> 12
        tmp = HexagonMapping()
        tmp.G = int(asid == 0)
        tmp.A1A0 = 0
        tmp.ASID = asid
        tmp.X = int(perm_exec)
        tmp.W = int(perm_write)
        tmp.R = 1
        tmp.U = int(getattr(section,'user_mode',asid != 0))
        tmp.C = int(getattr(section,'cache_policy','7'))    # Default L2 cacheable
        tmp.vpage = vpage
        tmp.ppage = ppage
        tmp.size = pgcnt
        tmp.locked = getattr(section,'locked',(asid==0))
        tmp.reclaim = getattr(section,'reclaim',False)
        tmp.atomic = False
        tmp.island = island
        tmp.reloc = self.relocatable_image.value
        tmp.infile = infile
        tmp.overlay = section.overlay
        tmp.physpool = getattr(section,'physpool',None)
        tmp = tmp.fix_to_TLB_entries(maxsize = self.max_elf_tlb.value/0x1000)
        self.all_mappings.extend(tmp)
    def sort_mappings(self):
        """Sort the all_mappings array."""
        for t in self.all_mappings:
            t.sortkey = (t.vpage << 32) + (t.size)
        self.all_mappings.sort(key=lambda x: x.sortkey)
    def get_ranges(self):
        """Compute the list of virtual address ranges to be mapped."""
        current_range = None
        all_ranges = []
        for t in self.all_mappings:
            if t.atomic:
                if current_range:
                    all_ranges.append(current_range)
                all_ranges.append([t.vpage, t.vpage+t.size, [t]])
                current_range = None
            elif current_range == None:
                current_range = [t.vpage, t.vpage+t.size, [t]]
            elif t.vpage <= current_range[1]:
                current_range[1] = max(current_range[1], t.vpage+t.size)
                current_range[2].append(t)
            else:
                all_ranges.append(current_range)
                current_range = [t.vpage, t.vpage+t.size, [t]]
        if current_range:
            all_ranges.append(current_range)
        self.all_ranges = all_ranges
    def get_map(self, vaddr, size, mappings):
        # Get the list of mappings that overlap the proposed (vaddr,size)
        overlap = []
        for t in mappings:
            if t.vpage + t.size <= vaddr:
                continue
            if t.vpage >= vaddr + size:
                continue
            overlap.append(t)
        if not overlap:
            raise Exception('Internal error in image builder -- hole found in range')
        from copy import copy
        proposed = copy(overlap[0])
        proposed.ppage += (vaddr - proposed.vpage)
        proposed.vpage = vaddr
        proposed.size = size
        for t in overlap[1:]:
            overs = sorted([proposed.overlay, t.overlay])
            if overs == [0,1]:
                return False
            if max(overs) == 1:
                continue                 # All overlays are compatible with each other
            if proposed.vpage - proposed.ppage != t.vpage - t.ppage:
                return False
            if proposed.physpool != t.physpool:
                if t.physpool == '*':
                    pass
                elif proposed.physpool == '*':
                    proposed.physpool = t.physpool
                else:
                    return False
            if proposed.G != t.G:
                return False
            if proposed.A1A0 != t.A1A0:
                return False
            if proposed.ASID != t.ASID:
                return False
            if proposed.R != t.R:
                return False
            if proposed.C >= 0 and t.C >= 0 and proposed.U != t.U:
                return False
            if proposed.C >= 0 and t.C >= 0 and proposed.C != t.C:
                return False
            if proposed.locked != t.locked:
                pass       # Allow mixing locked and unlocked for now
            if proposed.atomic or t.atomic:
                return False
            if proposed.island != t.island:
                return False
            if proposed.reloc != t.reloc:
                return False
            if self.allow_map_wx.value != 'true':
                if proposed.X and t.W:
                    return False
                if proposed.W and t.X:
                    return False
            if t.W:
                proposed.W = True
            if t.X:
                proposed.X = True
            if t.locked:
                proposed.locked = True
            if not t.reclaim:
                proposed.reclaim = False
            proposed.C = max(proposed.C, t.C)
            proposed.overlay = max(overs)
        return proposed
    def get_opt_mappings(self):
        for a in self.all_ranges:
            base = a[0]
            nsize = a[1]-a[0]
            while nsize:
                best_map = None
                sz_lo = 0
                sz_hi = nsize
                # Do binary search on the size to find the biggest size
                #  that actually works
                while sz_hi > sz_lo:
                    tmp = (sz_hi + sz_lo + 1) / 2
                    v = self.get_map(base, tmp, a[2])
                    if v:
                        best_map = v
                        sz_lo = tmp
                    else:
                        sz_hi = tmp - 1
                if best_map == None:
                    ex_str = 'Cannot find compatible mapping for 4K page at virtual address %X' % (base << 12)
                    raise Exception(ex_str)
                if best_map.locked or best_map.island:
                    # Locked mappings must be actually achievable in hardware
                    tmp = 0x1000
                    if best_map.reloc:
                        tmp = 0x100         # For reloc, don't use entries over 1 MB
                    while True:
                        if tmp <= sz_lo:
                            if (best_map.vpage & (tmp-1)) == 0:
                                if (best_map.ppage & (tmp-1)) == 0:
                                    break
                                if best_map.island:
                                    # If physical page is wrong on an island mapping now, it'll get fixed later
                                    #  when we actually allocate space for it in the island.
                                    break
                        tmp = tmp/4
                    sz_lo = tmp
                    best_map.size = tmp
                    best_map = self.get_map(best_map.vpage, best_map.size, a[2])
                base += sz_lo
                nsize -= sz_lo
                self.opt_mappings.append(best_map)
    def lookup_vma_to_lma(self, vaddr):
        for p in self.phdr:
            if vaddr >= p.p_vaddr and vaddr < p.p_vaddr+p.p_memsz:
                ret = p.p_paddr + (vaddr - p.p_vaddr)
                return ret
        raise Exception('Error finding LMA for VMA %X' % vaddr)
    def move_pool_mappings(self):
        w = []
        for t in self.opt_mappings:
            if t.physpool and t.physpool != '*':
                w.append(t)
        for t in w:
            tmp = (t.vpage | t.size)
            tmp = (tmp & -tmp)
            t.sortkey = (-tmp, t.vpage)
        w.sort(key=lambda x: x.sortkey)
        for t in w:
            pp = [p for p in self.pools if p.name == t.physpool]
            if len(pp) != 1:
                raise Exception('Error assigning mapping to pool %s' % t.physpool)
            p = pp[0]
            newpage = p.alloc_pages(t.size, t.size)
            t.ppage = newpage
            t.reloc = False
            orgaddr = self.lookup_vma_to_lma(t.vpage << 12) + (self.virt_base - self.phys_base)
            self.island_moves.append([orgaddr, newpage << 12, t.size << 12])
    def move_island_mappings(self):
        # Get the list of island mappings
        w = []
        self.island_pool_dynamic_usage = 0
        for t in self.opt_mappings:
            if t.island:
                if (t.ASID == 0 or t.W == 0 or t.infile.restart == 0):
                    w.append(t)
                else:
                    self.island_pool_dynamic_usage += t.size
        # Set a sort key for each island mapping which is the smallest
        #  power of two that divides both the base address and the size
        for t in w:
            tmp = (t.vpage | t.size)
            tmp = (tmp & -tmp)
            t.sortkey = tmp
        w.sort(key=lambda x: -x.sortkey)
        for t in w:
            moved = False
            for m in self.phys_island_regions:
                if not moved:
                    if (m[0] & (t.sortkey-1)) == 0:
                        # Alignment checks out
                        if m[0]+t.size <= m[1]:
                            orgaddr = self.lookup_vma_to_lma(t.vpage << 12) + (self.virt_base - self.phys_base)
                            # Size checks out
                            t.ppage = m[0]
                            t.C = m[2] & 0xF
                            t.reloc = ((m[2] & 0x10) != 0)
                            if t.reloc:
                                use_dest = (t.ppage << 12) + self.virt_base + 1
                                t.ppage += (self.phys_base >> 12)
                            else:
                                use_dest = t.ppage << 12
                            m[0] += t.size
                            m[2] += t.size << 16
                            moved = True
                            self.island_moves.append([orgaddr, use_dest, t.size << 12])
            if not moved:
                raise Exception('Problem with fitting all island sections into the island')
    def phys_island_report(self):
        total_bytes_copied = 0
        print 'BOOT island copies'
        for x in self.island_moves:
            #print 'Vaddr %08X -> Paddr %08X, size %X' % (x[0], x[1], x[2])
            total_bytes_copied += x[2]
        total_remaining_region = 0
        for x in self.phys_island_regions:
            total_remaining_region += x[1]-x[0]+1
        total_remaining_region -= self.island_pool_dynamic_usage
        print '*' * 60
        print 'Total island space copied at boot:      %4dK (%d bytes)' % (total_bytes_copied / 1024, total_bytes_copied)
        print 'Total island space used for spawn:      %4dK (%d bytes)' % (self.island_pool_dynamic_usage * 4, self.island_pool_dynamic_usage * 4096)
        print 'Total island space left in island pool: %4dK (%d bytes)' % (total_remaining_region * 4, total_remaining_region * 4096)
        print '*' * 60
        if total_remaining_region < 0:
            raise Exception('Problem with fitting all island sections into the island')
        if total_remaining_region*4096 < self.free_uimg:
            raise Exception('Failing build due to assert_free_microimage failure\nFree space is less than the requested %s bytes' % self.free_uimg)
    def can_be_reclaimed(self, map):
        # Return false if the map overlaps with any non-reclaim mappings
        # Return true otherwise
        for t in self.all_mappings:
            if t.overlaps(map):
                if not t.reclaim:
                    return False
        return True
    def ProcessFile(self, physaddr, filename, infiles):
        #self.start(filename)
        self.phys_base = physaddr
        # Find the lowest virtual address in the os image
        base_addr = infiles[0].start_addr
        self.virt_base = base_addr
        if self.CONFIG_ADVANCED_MEMORY:
            #self.read_phys_island()
            self.read_boot_mappings()
            self.read_tlb_dump()
            self.read_mmap_table()
            self.read_pool_configs()
            #self.do_section_overrides(infiles)
            self.synthesize_elf_mappings(infiles)
            self.sort_mappings()
            self.get_ranges()
            self.get_opt_mappings()
            self.move_pool_mappings()
            if self.phys_island_regions:
                self.move_island_mappings()
            reclaim_mask = 0L
            reclaim_index = 1L
            v = []
            vn = []
            print '*' * 60
            print 'BOOT TLB mappings'
            for t in self.opt_mappings:
                if t.locked and t.reloc and t.overlay != 1 and t.C >= 0:
                    reclaim = self.can_be_reclaimed(t)
                    reclaim_msg = ('',' unlock')[reclaim]
                    t.ppage -= (self.phys_base >> 12)
                    print '%05X000 -> BASE  +  %05X000, length %05X000, perms X:%u W:%u R:%u U:%u C:%X%s' % (t.vpage, t.ppage, t.size,
                                                                                                             t.X, t.W, t.R, t.U, t.C,
                                                                                                             reclaim_msg)
                    v.append(t.get_TLB_entry())
                    if reclaim:
                        reclaim_mask += reclaim_index
                    reclaim_index *= 2
            for t in self.opt_mappings:
                if t.locked and not t.reloc and t.overlay != 1 and t.C >= 0:
                    reclaim = self.can_be_reclaimed(t)
                    reclaim_msg = ('',' unlock')[reclaim]
                    print '%05X000 -> Physical %05X000, length %05X000, perms X:%u W:%u R:%u U:%u C:%X%s' % (t.vpage, t.ppage, t.size,
                                                                                                             t.X, t.W, t.R, t.U, t.C,
                                                                                                             reclaim_msg)
                    vn.append(t.get_TLB_entry())
                    if reclaim:
                        reclaim_mask += reclaim_index
                    reclaim_index *= 2
            self.do_tlb_compare(v,vn)
            self.write_tlb_dump(v,vn)
            self.write_tlb_reclaim(reclaim_mask)
            print '*' * 60
            print 'reclaim_mask is %X' % reclaim_mask
            for x in self.island_moves:
                print 'Vaddr %08X -> Paddr %08X, size %X' % (x[0], x[1], x[2])
            if self.phys_island_regions:
                addr = self.eip_addr_phys_island + 8
                for m in self.phys_island_regions:
                    self[(addr,4)] = struct.pack('<L', m[2])     # Write out the new third word with the number of
                                                                 #  pages used in the high half
                    addr += 12
                self.write_quick_mmu(infiles[0])
                self.phys_island_report()
        self.write_EIP(build_eip_info(None, infiles, self.island_moves, self.opt_mappings))
        self.finish()

def do_fixups(patcher, infiles, physaddr):
    for i in infiles:
        # For all sections with names of the form '.ukernel.*' or '.start'
        #  turn off the writeable flag.
        for s in i.shdr:
            sn = string_lookup(i, s.sh_name)
            if sn.startswith('.ukernel.') or sn == '.start':
                s.sh_flags &= ~1

        max_phys = 0
        for p in i.phdr:
            max_phys = max(max_phys, p.p_paddr + p.p_memsz)

        # Look for a PT_LOAD program header which spans .start address;
        #  if we find one, split it into two pieces.
        for k in range(len(i.phdr)):
            p = i.phdr[k]
            if p.p_paddr < i.start_addr:
                if p.p_paddr + p.p_memsz > i.start_addr:

                    p0 = p.copy()
                    p0.p_memsz = i.start_addr - p.p_paddr
                    p0.p_filesz = min(p0.p_filesz, p0.p_memsz)

                    p1 = p.copy()
                    p1.p_memsz -= p0.p_memsz
                    p1.p_filesz -= p0.p_filesz
                    p1.p_offset += p0.p_memsz
                    p1.p_paddr += p0.p_memsz
                    p1.p_vaddr += p0.p_memsz

                    i.phdr.pop(k)
                    i.phdr.insert(k, p1)
                    i.phdr.insert(k, p0)

        # For any PHDRs which fall before the start address, roll their
        #  physical address to the next aligned physical address past
        #  max_phys

        for p in i.phdr:
            if p.p_paddr < i.start_addr:
                pad = (p.p_paddr - max_phys) & 0xFFF
                max_phys += pad
                p.p_paddr = max_phys
                max_phys += p.p_memsz

        for p in i.phdr:
            if p.p_type == 6:
                p.p_paddr = aligndown((physaddr - p.p_memsz), 0x1000)

def parse_build_elements(cfg, els):
    def BadBuildElem(s):
        raise Exception('Failure in <build> configuration: %s' % s)

    class generic_xml:
        def __init__(self, cfg):
            setattr(cfg, self.__class__.__name__, self)
            for k,v in self.defaults:
                setattr(self, k, v)
        def add_attr(self, k, v):
            setattr(self, k, v)
        def finalize(self):
            pass
        force_init = True
        defaults = []

    # For each "simple" XML element, define a class with the same name
    #  as the XML element, have it inherit from generic_xml, and set
    #  the class variable "defaults" to a list of default attributes.

    class max_elf_tlb(generic_xml):
        defaults = [('value', 0x100000)]
        def finalize(self):
            self.value = int(self.value,0)
    class relocatable_image(generic_xml):
        defaults = [('value', True)]
        def finalize(self):
            self.value = (self.value.lower() != 'false')
    class gap_reclaim(generic_xml):
        defaults = [('value', 'normal')]
    class tlbdump_usage(generic_xml):
        defaults = [('value', 'normal')]
    class mapping_method(generic_xml):
        defaults = [('value', 'normal')]
    class allow_map_wx(generic_xml):
        defaults = [('value', 'false')]

    class section:
        def __init__(self,cfg):
            self.match = lambda x, sname: (sname==None)
            self.attrs = {}
            cfg.section_info.append(self)
        def finalize(self):
            # Allow all section elements and attributes for now.
            # In the future, check here for conflicting attributes,
            #  illegal attributes, etc.
            pass
        def add_attr(self,k,v):
            if k == 'tlb_lock' and v == '1':
                self.attrs['TLBLOCK'] = lambda s: setattr(s,'locked',True)
                self.attrs['TLBLOCK'] = lambda s: setattr(s,'reclaim',False)
            elif k == 'tlb_lock' and v == '0':
                self.attrs['TLBLOCK'] = lambda s: setattr(s,'locked',False)
                self.attrs['TLBLOCK'] = lambda s: setattr(s,'reclaim',False)
            elif k == 'tlb_lock' and v.lower() == 'root':
                self.attrs['TLBLOCK'] = lambda s: setattr(s,'locked',s.root_elf)
                self.attrs['TLBLOCK'] = lambda s: setattr(s,'reclaim',False)
            elif k == 'tlb_lock' and v.lower() == 'boot':
                self.attrs['TLBLOCK'] = lambda s: setattr(s,'locked',s.root_elf)
                self.attrs['TLBLOCK'] = lambda s: setattr(s,'reclaim',s.root_elf)
            elif k == 'mapping' and v.lower() == 'none':
                self.attrs['MAPPING'] = lambda s: setattr(s,'mapping','none')
                self.attrs['PERMS'] = lambda s: setattr(s,'sh_flags',s.sh_flags & ~5)
                self.attrs['CACHE'] = lambda s: setattr(s,'cache_policy','-1')
            elif k == 'mapping' and v.lower() == 'rx':
                self.attrs['MAPPING'] = lambda s: setattr(s,'mapping','rx')
                self.attrs['PERMS'] = lambda s: setattr(s,'sh_flags',(s.sh_flags | 4) & ~1)
            elif k == 'mapping' and v.lower() == 'rw':
                self.attrs['MAPPING'] = lambda s: setattr(s,'mapping','rw')
                self.attrs['PERMS'] = lambda s: setattr(s,'sh_flags',(s.sh_flags | 1) & ~4)
            elif k == 'mapping' and v.lower() == 'overlay':
                self.attrs['MAPPING'] = lambda s: setattr(s,'mapping','overlay')
            elif k == 'user_mode' and v.lower() == 'false':
                self.attrs['UMODE'] = lambda s: setattr(s,'user_mode',not s.root_elf)
            elif k == 'user_mode' and v.lower() == 'true':
                self.attrs['UMODE'] = lambda s: setattr(s,'user_mode',True)
            elif k == 'cache_policy':
                self.attrs['CACHE'] = lambda s,v=v: setattr(s,'cache_policy',v)
            elif k == 'physpool':
                self.attrs['PHYSPOOL'] = lambda s,v=v: setattr(s,'physpool', v)
            elif k == 'name' and not '*' in v:
                self.name = v
                self.match = lambda x, sname: (sname== x.name)
            elif k == 'name' and '*' in v:
                tmp = v.replace('.','\\.')
                tmp = v.replace('*','.*')
                import re
                self.regex = re.compile(tmp + '$')
                self.match = lambda x, sname: (sname and x.regex.match(sname) != None)
            elif k == 'match':
                import re
                self.regex = re.compile(v + '$')
                self.match = lambda x, sname: (sname and x.regex.match(sname) != None)
            else:
                BadBuildElem('%s cannot set %s to %s' % (self,k,v))

    section_attributes = section

    cfg.section_info = []

    for k,v in locals().items():
        if hasattr(v,'force_init'):
            v(cfg)

    for oe in els:
        for ie in oe.childNodes:
            if ie.nodeType == ie.ELEMENT_NODE:
                this = locals()[ie.tagName](cfg)
                if ie.attributes:
                    for k,v in ie.attributes.items():
                        this.add_attr(k,v)
                this.finalize()

class ElfInfoPatch:
    def __init__(self):
        #
        # List here the symbols we need to extract from the build
        #
        # To declare a symbol as optional (weak), prepend the name
        #  of the symbol with [w] with no intervening white space.
        #  For example:  [w]Optional_Symbol
        #
        # Weak symbols that don't exist in the build are reported
        #  as having an address of zero.
        #
        self.symbols_spec = '''
            QURTK_phys_island
            QURTK_tlb_dump
            TLB_LAST_REPLACEABLE_ENTRY
            qurtos_mmap_table
            QURTK_tlblock_entries
            QURTK_tlb_dump_relocs
            qurtos_boot_mappings
            QURTK_quick_load_MMU
            qurtos_xml_passthrough
            [w]qurtos_tlb_reclaim
            [w]pool_configs
        '''
        self.flags_spec = '''
            CONFIG_ADVANCED_MEMORY
        '''
        self.symbols = self.symbols_spec.split()
        self.flags = self.flags_spec.split()
        self.symbols.sort()
        self.flags.sort()
        self.total_words = len(self.symbols)+1
    def Signature(self):
        #
        # For now, have the signature be the actual strings themselves
        #
        r = '\n'.join(self.flags+['']+self.symbols)
        while len(r)%4:
            r+='\n'
        return r
    def Parse(self, cfg, addr, size):
        sig = self.Signature()
        s = cfg[(addr,size)]
        temp = s.split(sig)
        if len(temp) != 3:
            raise Exception('Could not find valid ELF Info Patch area')
        if temp[0][-4:] != '\0\0\0\0':
            raise Exception('Could not find valid ELF Info Patch area')
        offset = len(temp[0]) - 4
        total_size = 4+len(sig)*2+len(temp[1])
        if offset % 8:
            raise Exception('Could not find valid ELF Info Patch area')
        if total_size % 8:
            raise Exception('Could not find valid ELF Info Patch area')
        temp = struct.unpack('<'+'L'*self.total_words, temp[1][:4*self.total_words])
        bitpos = 1
        for x in self.flags:
            if temp[0] & bitpos:
                setattr(cfg, x, True)
            else:
                setattr(cfg, x, False)
            bitpos *= 2
        ix = 1
        for x in self.symbols:
            if x.startswith('[w]'):
                setattr(cfg, x[3:], temp[ix])
            else:
                setattr(cfg, x, temp[ix])
            ix += 1
        #
        # Now fill in the legacy variables
        #
        cfg.eip_addr_phys_island = cfg.QURTK_phys_island
        cfg.eip_addr_boot_maps = cfg.qurtos_boot_mappings
        cfg.eip_addr_tlb_dump = cfg.QURTK_tlb_dump
        cfg.eip_size_tlb_dump = 1+cfg.TLB_LAST_REPLACEABLE_ENTRY
        cfg.eip_addr_tlblock = cfg.QURTK_tlblock_entries
        cfg.eip_addr_mmap_table = cfg.qurtos_mmap_table
        cfg.eip_addr_tlb_relocs = cfg.QURTK_tlb_dump_relocs
        cfg.eip_addr_quick_mmu = cfg.QURTK_quick_load_MMU
        cfg.eip_size = total_size
        cfg.eip_location = addr + offset

        # Go read XML passthrough

        if cfg.qurtos_xml_passthrough:
            sz = struct.unpack('<L', cfg[(cfg.qurtos_xml_passthrough, 4)])[0]
            xml = cfg[(cfg.qurtos_xml_passthrough+4,sz)]
        else:
            xml = '<image></image>'
        from xml.dom.minidom import parseString
        cfg.xml = parseString(xml.decode('utf-8'))
        parse_build_elements(cfg, cfg.xml.documentElement.childNodes)

    def Build_Asm(self, outfilename, req_eipsize, flags):
        bitpos = 1
        flags_numeric = 0
        for x in self.flags:
            if ('-D'+x) in flags:
                flags_numeric += bitpos
            bitpos *= 2
        sig = self.Signature()
        #
        # Calculate total space used for our contents (minus padding)
        #  4 bytes for zero word at the beginning
        #  4 bytes for the flags word
        #  4 bytes for each symbol we need
        #  2 signatures
        #
        used_space = 4 + 4 + len(self.symbols)*4 + len(sig)*2
        eipsize = req_eipsize
        if eipsize < used_space:
            eipsize = used_space
        while eipsize % 8:
            eipsize += 1
        if eipsize != req_eipsize:
            print 'Adjusting EIP size from requested %s to actual %s' % (req_eipsize, eipsize)
        unused_space = eipsize - used_space
        fo = open(outfilename, 'w')
        fo.write('/* This file is automatically generated */\n')
        fo.write('#ifdef GENERATE_ELF_INFO_PATCH\n')
        fo.write('  .align 8\n')
        fo.write('ELF_INFO_PATCH:\n')
        fo.write('  .word 0\n')
        for x in sig:
            fo.write('  .byte %u\n' % ord(x))
        fo.write('  .word %u\n' % flags_numeric)
        for x in self.symbols:
            if x.startswith('[w]'):
                fo.write('  .weak %s\n' % x[3:])
                fo.write('  .word %s\n' % x[3:])
            else:
                fo.write('  .word %s\n' % x)
        fo.write('  .space %u,0\n' % unused_space)
        for x in sig:
            fo.write('  .byte %u\n' % ord(x))
        fo.write('#endif /* GENERATE_ELF_INFO_PATCH */\n')
        fo.close()
        return 0

def qurt_image_build(argv):
    try:
        if argv[1] == 'build_eip':
            return ElfInfoPatch().Build_Asm(argv[2], int(argv[3]), argv[4:])
        infilenames, outfilename, physaddr, free_uimg = parse_args(argv)
        ''' Create a list of dictionaries, one for each of the input files '''
        infiles=[Elf_File(infile=f) for f in infilenames]
        for f in infiles:
            read_elf_info(f)
        patcher = QurtElfPatch()
        patcher.free_uimg = free_uimg
        patcher.startr(infiles[0].file)
        ElfInfoPatch().Parse(patcher, infiles[0].start_addr, infiles[0].start_size)
        patcher.read_phys_island()
        do_fixups(patcher, infiles, physaddr)
        ''' Now figure out our basic relocation information '''
        process_input_files(infiles, physaddr)
        patcher.do_section_overrides(infiles)
        outfile = Elf_File(outfile=outfilename)
        ''' Populate the output file structure '''
        populate_output_file(outfile, infiles)
        dump_output_file(outfile, infiles)
        patcher.ehdr = outfile.ehdr
        patcher.phdr = outfile.phdr
        patcher.shdr = outfile.shdr
        patcher.strtab = outfile.strtab
        patcher.file = outfile.file
        patcher.ProcessFile(physaddr, outfilename, infiles)
        if not no_adjust:
            for i in infiles:
                adjust_input_file(i, rename_input_file)
        return 0
    except Exception, err:
        from traceback import format_exc as tbstr
        print tbstr()
        print >> sys.stderr, "%s: %s" % (argv[0], str(err))
        return 1

if __name__ == '__main__':
    sys.exit(qurt_image_build(sys.argv))

# Signatures of the files that this depends on
# 8dc1fab9af2d318d04c7fd08524ca7b3 /local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/core/kernel/qurt/build/mapss_b/qdsp6/8909.apqs.prod/install/modemv56_l2tags_rs/scripts/lib/qurt.py
