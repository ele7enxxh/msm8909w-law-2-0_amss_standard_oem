##  @file       elfConstants.py
#   @author     Christopher Ahn (cahn@qualcomm.com)
#   @brief      This file contains the constants for the generic manipulator for
#               ELF files.

#===============================================================================
# Copyright (c) 2013 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/perf/scripts/elfManipulator/include/elfConstants.py#1 $
#  $DateTime: 2016/12/13 07:59:26 $
#  $Author: mplcsds1 $
#  $Change: 11985160 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when      who     what, where, why
# --------  ------  ------------------------------------------------------------
# 03/28/13  cahn    Added Elf32_SymOffsets constants.
# 03/09/13  cahn    Updated with some Hexagon-specific constants.
# 03/07/13  cahn    Specification review, cleanup and added Doxygen comments.
# 03/06/13  cahn    Added support for reading of the symbol table.
# 03/05/13  cahn    Added mapping of section names from .shstrtab index.
# 03/05/13  cahn    Added support for reading of the section header table.
# 03/05/13  cahn    Added support for reading of the program header table.
# 03/04/13  cahn    Creation.
#===============================================================================

## Return code: Success
RC_SUCCESS = 0
## Return code: Error/Failure
RC_ERROR = 1

##
# @brief    This class defines ELF 32-bit data types
# @note     64-bit datatypes not currently supported
class elfDataTypes:
    ## Unsigned program address
    Elf32_Addr      = 4
    ## Unsigned medium integer
    Elf32_Half      = 2
    ## Unsigned file offset
    Elf32_Off       = 4
    ## Signed large integer
    Elf32_Sword     = 4
    ## Unsigned large integer
    Elf32_Word      = 4
    ## Unsigned small integer
    unsigned_char   = 1

##
# @brief    This class defines ELF indentification indexes
class elfIdentification:
    ## File identification (0x7f)
    EI_MAG0         = 0
    ## File identification (E)
    EI_MAG1         = 1
    ## File identification (L)
    EI_MAG2         = 2
    ## File identification (F)
    EI_MAG3         = 3
    ## File class
    EI_CLASS        = 4
    ## Data encoding
    EI_DATA         = 5
    ## File version
    EI_VERSION      = 6
    ## Operating system/ABI identification
    EI_OSABI        = 7
    ## Version of ABI
    EI_ABIVERSION   = 8
    ## Start of padding bytes
    EI_PAD          = 9
    ## Size of e_ident[]
    EI_NIDENT       = 16

##
# @brief    This class defines ELF classes (or capacities)
class elfClass:
    ## Invalid class
    ELFCLASSNONE    = 0
    ## 32-bit objects
    ELFCLASS32      = 1
    ## 64-bit objects
    ELFCLASS64      = 2

##
# @brief    This class defines ELF encoding schemes
class elfData:
    ## Invalid data encoding
    ELFDATANONE     = 0
    ## Little Endian
    ELFDATA2LSB     = 1
    ## Big Endian
    ELFDATA2MSB     = 2

##
# @brief  This class defines ELF header versions
class elfVersion:
    ## Invalid version
    EV_NONE     = 0
    ## Current version
    EV_CURRENT  = 1

##
# @brief    This class defines OS- or ABI- specific ELF extensions
# @note     Does not conform to the ELF specifications
class elfOSABI:
    ## No extensions or unspecified
    ELFOSABI_NONE   = 0
    ## 'UNIX - System V' based on readelf
    UNIX_SYSTEM_V   = 0

##
# @brief    This class defines the versions of the ABI to which the ELF can be
#           targeted.
# @note     Does not conform to the ELF specifications
class elfABIVersion:
    ## Unspecified
    ELFABIVER_NONE  = 0
    ## '0' based on readelf
    ZERO            = 0

##
# @brief    This class defines ELF object file types
class elfType:
    ## No file type
    ET_NONE     = 0
    ## Relocatable file
    ET_REL      = 1
    ## Executable file
    ET_EXEC     = 2
    ## Shared object file
    ET_DYN      = 3
    ## Core file
    ET_CORE     = 4
    ## Lower bound of OS-specific files
    ET_LOOS     = 0xfe00
    ## Upper bound of OS-specific files
    ET_HIOS     = 0xf2ff
    ## Lower bound of processor-specific files
    ET_LOPROC   = 0xff00
    ## Upper bound of processor-specific files
    ET_HIPROC   = 0xffff

##
# @brief    This class defines the ELF architectures
# @note     Does not include all machine types defined by the ELF specifications
class elfMachineType:
    ## No machine
    EM_NONE     = 0
    ## AT&T WE 32100
    EM_M32      = 1
    ## SPARC
    EM_SPARC    = 2
    ## Intel 80386
    EM_386      = 3
    ## Motorola 68000
    EM_68K      = 4
    ## Motorola 80860
    EM_88K      = 5
    ## MIPS RS3000
    EM_MIPS     = 8
    ## Qualcomm Hexagon
    EM_HEXAGON  = 164

##
# @brief    This class defines reserved section header table indexes
# @note     Add logic for SHN_XINDEX case if this becomes applicable
class specialSectionsIndexes:
    ## Undefined
    SHN_UNDEF               = 0
    ## Lower bound of the range of reserved indexes.
    SHN_LORESERVE           = 0xff00
    ## Lower bound for reserved processor-specific semantics
    SHN_LOPROC              = 0xff00
    ## Upper bound for reserved processor-specific semantics
    SHN_HIPROC              = 0xff1f
    ## Lower bound for reserved OS-specific semantics
    SHN_LOOS                = 0xff20
    ## Upper bound for eserved OS-specific semantics
    SHN_HIOS                = 0xff3f
    ## Absolute values for the corresponding reference
    SHN_ABS                 = 0xfff1
    ## Symbols defined relative to this section are common symbols
    SHN_COMMON              = 0xfff2
    ## Escape value; section header index is too big to fit in containing field
    SHN_XINDEX              = 0xffff
    ## Upper bound of the range of reserved indexes
    SHN_HIRESERVE           = 0xffff
    ## Other access sizes
    SHN_HEXAGON_SCOMMON     = 0xff00
    ## Byte-sized access
    SHN_HEXAGON_SCOMMON_1   = 0xff01
    ## Half-word-sized access
    SHN_HEXAGON_SCOMMON_2   = 0xff02
    ## Word-sized access
    SHN_HEXAGON_SCOMMON_4   = 0xff03
    ## Double-word-size access
    SHN_HEXAGON_SCOMMON_8   = 0xff04

##
# @brief    This class defines ELF section types
class sectionTypes:
    ## Inactive
    SHT_NULL            = 0
    ## Information defined by the program
    SHT_PROGBITS        = 1
    ## Symbol table
    SHT_SYMTAB          = 2
    ## String table
    SHT_STRTAB          = 3
    ## Relocation entries with explicit addends
    SHT_RELA            = 4
    ## Symbol hash table
    SHT_HASH            = 5
    ## Information for dynamic linking
    SHT_DYNAMIC         = 6
    ## "Special" marks for the file
    SHT_NOTE            = 7
    ## No space, but otherwise SHT_PROGBITS
    SHT_NOBITS          = 8
    ## Relocation entries without explicit addends
    SHT_REL             = 9
    ## Reserved, unspecified semantics
    SHT_SHLIB           = 10
    ## Symbol table
    SHT_DYNSYM          = 11
    ## Array of pointers to init functions
    SHT_INIT_ARRAY      = 14
    ## Array of pointers to termination functions
    SHT_FINI_ARRAY      = 15
    ## Array of pointers to preinit functions
    SHT_PREINIT_ARRAY   = 16
    ## Section group; related sections
    SHT_GROUP           = 17
    ## Required if any of the section header indexes referenced by SHT_SYMTAB
    #  contain the escape value SHN_XINDEX
    SHT_SYMTAB_SHNDX    = 18
    ## Lower bound for reserved OS-specific semantics
    SHT_LOOS            = 0x60000000
    ## Upper bound for reserved OS-specific semantics
    SHT_HIOS            = 0x6fffffff
    ## Lower bound for reserved processor-specific semantics
    SHT_LOPROC          = 0x70000000
    ## Upper bound for reserved processor-specific semantics
    SHT_HIPROC          = 0x7fffffff
    ## Lower bound of reserved indexes for application programs
    SHT_LOUSER          = 0x80000000
    ## Upper bound of reserved indexes for application programs
    SHT_HIUSER          = 0xffffffff

##
# @brief    This class defines ELF section attribute flags
class sectionFlags:
    ## Data should be writable during process execution
    SHF_WRITE               = 0x1
    ## Occupies memory during execution
    SHF_ALLOC               = 0x2
    ## Executable machine instructions
    SHF_EXECINSTR           = 0x4
    ## Merged to eliminate duplication
    SHF_MERGE               = 0x10
    ## Null-terminated char strings
    SHF_STRINGS             = 0x20
    ## Section header table index
    SHF_INFO_LINK           = 0x40
    ## Special ordering requirements for link editors
    SHF_LINK_ORDER          = 0x80
    ## Special OS-specific processing
    SHF_OS_NONCONFORMING    = 0x100
    ## Member of a section group
    SHF_GROUP               = 0x200
    ## Thread-Local Storage
    SHF_TLS                 = 0x400
    ## Reserved for OS-specific semantics
    SHF_MASKOS              = 0x0ff00000
    ## Reserved for processor-specific semantics
    SHF_MASKPROC            = 0xf0000000
    ## Residing in the small data area
    SHF_HEXAGON_GPREL       = 0x10000000

##
# @brief    This class defines ELF section group flags
class sectionGroups:
    ## COMDAT group
    GRP_COMDAT 	    = 0x1
    ## Reserved for OS-specific semantics
    GRP_MASKOS 	    = 0x0ff00000
    ## Reserved for processor-specific semantics
    GRP_MASKPROC    = 0xf0000000

##
# @brief    This class defines symbol bindings, which determine linkage
#           visibilities and behavior for entities
class symbolBinding:
    ## Not visible outside object file with definition
    STB_LOCAL   = 0
    ## Visible to all object files being combined
    STB_GLOBAL  = 1
    ## Like globals, but definitions have low precedence
    STB_WEAK    = 2
    ## Lower bound reserved of OS-specific semantics
    STB_LOOS    = 10
    ## Upper bound reserved of OS-specific semantics
    STB_HIOS    = 12
    ## Lower bound reserved of processor-specific semantics
    STB_LOPROC  = 13
    ## Upper bound reserved of processor-specific semantics
    STB_HIPROC  = 15

##
# @brief    This class defines symbol types, which provide general
#           classifications for entities
class symbolTypes:
    ## Unspecified
    STT_NOTYPE  = 0
    ## A data object, such as a variable, and array, and so on
    STT_OBJECT  = 1
    ## Function or other executable code
    STT_FUNC    = 2
    ## Section; exists primarily for relocation
    STT_SECTION = 3
    ## Name of source file associated with the object file
    STT_FILE    = 4
    ## Unitialized common block
    STT_COMMON 	= 5
    ## Thread-Local storage entity
    STT_TLS     = 6
    ## Lower bound of OS-specific semantics
    STT_LOOS    = 10
    ## Upper bound of OS-specific semantics
    STT_HIOS    = 12
    ## Lower bound of processor-specific semantics
    STT_LOPROC  = 13
    ## Upper bound of processor-specific semantics
    STT_HIPROC  = 15

##
# @brief    This class defines how symbols may be accessed once they have
#           become part of an executable or shared object
class symVisibility:
    ## As specified by binding type
    STV_DEFAULT     = 0
    ## Defined by processor supplements to further constrain hidden symbols
    STV_INTERNAL    = 1
    ## Not visible to other components
    STV_HIDDEN      = 2
    ## Visibile in other components but not preemtable
    STV_PROTECTED   = 3

##
# @brief    This class defines the the version of the Hexagon processor that the
#           object file was created for.
class hexagonCreateVersionFlag:
    ## HEXAGON V2 (QDSP6V2)
    EF_HEXAGON_MACH_V2  = 0x1
    ## HEXAGON V3 (QDSP6V3)
    EF_HEXAGON_MACH_V3  = 0x2
    ## HEXAGON V4 (QDSP6V4)
    EF_HEXAGON_MACH_V4  = 0x3
    ## HEXAGON V5 (QDSP6V5)
    EF_HEXAGON_MACH_V5  = 0x4

##
# @brief    This class defines the highest version of the Hexagon processor that
#           the object file contains code for
class hexagonContainVersionFlag:
    ## Indicates that processor version is same as in bits [3:0] of e_flags
    EF_HEXAGON_ISA_MACH = 0x00
    ## Hexagon V2 ISA
    EF_HEXAGON_ISA_V2   = 0x10
    ## Hexagon V3 ISA
    EF_HEXAGON_ISA_V3   = 0x20
    ## Hexagon V4 ISA
    EF_HEXAGON_ISA_V4   = 0x30
    ## Hexagon V5 ISA
    EF_HEXAGON_ISA_V5   = 0x40

##
# @brief    This class defines ELF segment types
class segmentTypes:
    ## Unused
    PT_NULL     = 0
    ## Loadable segment, defined by p_filesz and p_memsz
    PT_LOAD     = 1
    ## Dynamic linking information
    PT_DYNAMIC  = 2
    ## Path name to invoke an interpreter
    PT_INTERP   = 3
    ## Auxiliary information
    PT_NOTE     = 4
    ## Reserved, unspecified semantics
    PT_SHLIB    = 5
    ## Program header table
    PT_PHDR     = 6
    ## Thread-Local Storage
    PT_TLS      = 7
    ## Lower bound for OS-specific semantics
    PT_LOOS     = 0x60000000
    ## Upper bound for OS-specific semantics
    PT_HIOS     = 0x6fffffff
    ## Lower bound for processor-specific semantics
    PT_LOPROC   = 0x70000000
    ## Upper bound for processor-specific semantics
    PT_HIPROC   = 0x7fffffff

##
# @brief  This class defines access permissions for ELF segments
class segmentFlags:
    ## Execute
    PF_X        = 0x1
    ## Write
    PF_W        = 0x2
    ## Read
    PF_R        = 0x4
    ## Reserved for OS-specific semantics
    PF_MASKOS   = 0x0ff00000
    ## Reserved for processor-specific semantics
    PF_MASKPROC = 0xf0000000

##
# @brief    This class stores offsets for each member of the Elf32_Sym structure
class Elf32_SymOffsets:
    ## Offset of Elf32_Sym.st_name
    st_name     = 0
    ## Offset of Elf32_Sym.st_value
    st_value    = st_name + elfDataTypes.Elf32_Word
    ## Offset of Elf32_Sym.st_size
    st_size     = st_value + elfDataTypes.Elf32_Addr
    ## Offset of Elf32_Sym.st_info
    st_info     = st_size + elfDataTypes.Elf32_Word
    ## Offset of Elf32_Sym.st_other
    st_other    = st_info + elfDataTypes.unsigned_char
    ## Offset of Elf32_Sym.st_shndx
    st_shndx    = st_other + elfDataTypes.unsigned_char
