##  @file       elfStructs.py
#   @author     Christopher Ahn (cahn@qualcomm.com)
#   @brief      This file contains the structures for the generic manipulator
#               for ELF files.

#===============================================================================
# Copyright (c) 2013 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/build/myps/elfManipulator/include/elfStructs.py#1 $
#  $DateTime: 2016/12/13 07:59:25 $
#  $Author: mplcsds1 $
#  $Change: 11985153 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when      who     what, where, why
# --------  ------  ------------------------------------------------------------
# 05/17/13  cahn    Removed checking for HEXAGON V5.
# 03/29/13  cahn    Various new APIs added. Changed the name of functions.
# 03/28/13  cahn    Memory optimization by using generator for symbol table.
# 03/26/13  cahn    Removed check for alignment within ELF offset.
# 03/18/13  cahn    Sections of type SHT_NOBITS have empty contents.
# 03/15/13  cahn    Updated some alignment checks and Elf32_Sym optimization.
# 03/11/13  cahn    Code cleanup.
# 03/10/13  cahn    Write out modified ELF based on member variables instead of
#                   member struct.
# 03/09/13  cahn    Updated with some Hexagon-specific constants.
# 03/07/13  cahn    Specification review, cleanup and added more comments.
# 03/06/13  cahn    Added support for reading the symbol table.
# 03/05/13  cahn    Added mapping of section names from .shstrtab index.
# 03/05/13  cahn    Added support for reading the section header table.
# 03/05/13  cahn    Added support for reading the program header table.
# 03/04/13  cahn    Creation.
#===============================================================================

# Imports
import elfConstants as const
import elfUtils as utils

##
# @brief    This base class is used to get a generic data struct from the ELF
class elfStruct(object):
    ##
    # @brief    Special member function that is automatically invoked for a new
    #           instance. Initializes and verifies data struct.
    # @param    elf     A full path to the input ELF file.
    # @param    offset  The offset to start reading the ELF file from.
    # @param    size    The amount to read in from the ELF file.
    # @param    idx     The index of the structure (default '0')
    # @param    verbose Enabled verbose printing for debugging (default '0')
    def __init__ (self, elf, offset, size, idx=0, verbose=0):
        ## Input ELF file
        self.elf = elf
        ## Section or program index
        self.idx = idx
        ## Verbosity level
        self.verbose = verbose
        ## Offset that will be used internally
        self.structOffset = 0
        ## Structure containing the data
        self.struct = utils.getDataFromELF(self.elf, offset, size)
        self.setStructMemberSizes()
        self.setAndValidateStructMembers()

    ##
    # @brief    Member function to set struct member sizes
    # @pre      Subclass implementation
    def setStructMemberSizes(self):
        utils.raiseElfManipulatorError("Subclass must implement elfStruct::setStructMemberSizes")

    ##
    # @brief    Member function to get struct member size
    # @param    member  Member to get the size of
    # @pre      Struct member sizes have been set using setStructMemberSizes
    def getStructMemberSize(self, member):
        if member in self.halfMembers:
            size = const.elfDataTypes.Elf32_Half
        elif member in self.longMembers:
            size = const.elfDataTypes.Elf32_Addr # == Off == Sword == Word
        elif member in self.byteMembers:
            size = const.elfDataTypes.unsigned_char
        else:
            utils.raiseElfManipulatorError(self.__class__.__name__ + "." +
                                    member + " does not exist.")
        return size

    ##
    # @brief    Member function to get struct member from ELF
    # @param    member  Member to get the value for
    def getStructMember(self, member):
        size = self.getStructMemberSize(member)
        data = utils.convertFromELF(    self.struct,
                                        size,
                                        self.structOffset)
        self.structOffset += size
        # Print member and value if verbose is enabled
        if (self.verbose):
            print (self.__class__.__name__ + "." + member).ljust(35) + " = " + str(data)
        return data

    ##
    # @brief    Member function to extract and validate data struct
    # @pre      Subclass implementation
    def setAndValidateStructMembers(self):
        utils.raiseElfManipulatorError("Subclass must implement elfStruct::setAndValidateStructMembers")

    ##
    # @brief    Member function to write data to an ELF and update internal
    #           offset in the struct.
    # @param    member      Member variable to write out to ELF
    # @param    number      Value to pack
    # @pre      File handle has already been acquired.
    def writeToELFAndUpdateOffset(self, member, number):
        size = self.getStructMemberSize(member)
        utils.writeDataToELF(   self.elfFileHandle, self.structOffset,
                                utils.convertToELF(size, number))
        self.structOffset += size

    ##
    # @brief    Member function to write internal data out to an ELF
    # @param    elfFileHandle   File handle to the output ELF
    # @param    offset          Offset to start writing internal data
    # @pre      Subclass implementation
    def flushToELF(self, elfFileHandle, offset):
        utils.raiseElfManipulatorError("Subclass must implement elfStruct::flushToELF")
##
# @brief    This class represents the ELF header struct
class Elf32_Ehdr(elfStruct):
    """
    #define EI_NIDENT   16
    typedef struct {
        unsigned_char   e_ident[EINIDENT];
        Elf32_Half      e_type;
        Elf32_Half      e_machine;
        Elf32_Word      e_version;
        Elf32_Addr      e_entry;
        Elf32_Off       e_phoff;
        Elf32_Off       e_shoff;
        Elf32_Word      e_flags;
        Elf32_Half      e_ehsize;
        Elf32_Half      e_phentsize;
        Elf32_Half      e_phnum;
        Elf32_Half      e_shentsize;
        Elf32_Half      e_shnum;
        Elf32_Half      e_shstrndx;
    } Elf32_Ehdr;
    """

    ## Member function to set struct member sizes
    def setStructMemberSizes(self):
        ## List containing all members of type Elf32_Half
        self.halfMembers = ["e_type",
                            "e_machine",
                            "e_ehsize",
                            "e_phentsize",
                            "e_phnum",
                            "e_shentsize",
                            "e_shnum",
                            "e_shstrndx"
                            ]
        ## List containing all members of type Elf32_(Addr|Off|Sword|Word)
        self.longMembers = ["e_version",
                            "e_entry",
                            "e_phoff",
                            "e_shoff",
                            "e_flags"
                            ]
        ## List containing all members of type unsigned_char
        self.byteMembers = ["e_ident[EI_MAG0]",
                            "e_ident[EI_MAG1]",
                            "e_ident[EI_MAG2]",
                            "e_ident[EI_MAG3]",
                            "e_ident[EI_CLASS]",
                            "e_ident[EI_DATA]",
                            "e_ident[EI_VERSION]",
                            "e_ident[EI_OSABI]",
                            "e_ident[EI_ABIVERSION]",
                            "e_ident[EI_PAD]"
                            ]

    ## Member function to extract and validate data struct
    def setAndValidateStructMembers(self):
        if (self.verbose):
            print "============================================================"
            print " ** VERBOSE DEBUG MSG ** ELF HEADER (Elf32_Ehdr)"
            print "============================================================"

        ##  The initial bytes mark the file as an object file and provide
        #   machine-independent data with which to decode and interpret
        #   the file's contents.
        self.e_ident = []

        # Magic number identifying the file as an ELF object file
        self.e_ident.append(self.getStructMember("e_ident[EI_MAG0]"))
        if (self.e_ident[const.elfIdentification.EI_MAG0] != ord('\x7f')):
            utils.raiseElfManipulatorError("Elf32_Ehdr.e_ident[EI_MAG0] failed validation.")
        self.e_ident.append(self.getStructMember("e_ident[EI_MAG1]"))
        if (self.e_ident[const.elfIdentification.EI_MAG1] != ord('E')):
            utils.raiseElfManipulatorError("Elf32_Ehdr.e_ident[EI_MAG1] failed validation.")
        self.e_ident.append(self.getStructMember("e_ident[EI_MAG2]"))
        if (self.e_ident[const.elfIdentification.EI_MAG2] != ord('L')):
            utils.raiseElfManipulatorError("Elf32_Ehdr.e_ident[EI_MAG2] failed validation.")
        self.e_ident.append(self.getStructMember("e_ident[EI_MAG3]"))
        if (self.e_ident[const.elfIdentification.EI_MAG3] != ord('F')):
            utils.raiseElfManipulatorError("Elf32_Ehdr.e_ident[EI_MAG3] failed validation.")

        # Class should be 32-bit object
        self.e_ident.append(self.getStructMember("e_ident[EI_CLASS]"))
        if (self.e_ident[const.elfIdentification.EI_CLASS] != const.elfClass.ELFCLASS32):
            utils.raiseElfManipulatorError("Elf32_Ehdr.e_ident[EI_CLASS] failed validation.")

        # Data encoding should be LITTLE-ENDIAN
        # Example:
        #            ---- ----
        # 0x0102    | 02 | 01 |
        #            ---- ----
        self.e_ident.append(self.getStructMember("e_ident[EI_DATA]"))
        utils.setElfEncoding(self.e_ident[const.elfIdentification.EI_DATA])

        # Version should be current
        self.e_ident.append(self.getStructMember("e_ident[EI_VERSION]"))
        if (self.e_ident[const.elfIdentification.EI_VERSION] != const.elfVersion.EV_CURRENT):
            utils.raiseElfManipulatorError("Elf32_Ehdr.e_ident[EI_VERSION] failed validation.")

        # OSABI should be UNIX - System V
        self.e_ident.append(self.getStructMember("e_ident[EI_OSABI]"))
        if (self.e_ident[const.elfIdentification.EI_OSABI] != const.elfOSABI.UNIX_SYSTEM_V):
            utils.raiseElfManipulatorError("Elf32_Ehdr.e_ident[EI_OSABI] failed validation.")

        # ABI version should be 0
        self.e_ident.append(self.getStructMember("e_ident[EI_ABIVERSION]"))
        if (self.e_ident[const.elfIdentification.EI_ABIVERSION] != const.elfABIVersion.ZERO):
            utils.raiseElfManipulatorError("Elf32_Ehdr.e_ident[EI_ABIVERSION] failed validation.")

        # Remaining bytes of the ELF identifier are don't cares (zeros)
        self.structOffset += const.elfIdentification.EI_NIDENT - const.elfIdentification.EI_PAD

        ##  This member identifies the object file type. Should be executable.
        self.e_type = self.getStructMember("e_type")
        if self.e_type != const.elfType.ET_EXEC:
            utils.raiseElfManipulatorError("Elf32_Ehdr.e_type failed validation.")

        ##  This member's value specifies the required architecture for an
        #   individual file. Should be Qualcomm Hexagon.
        self.e_machine = self.getStructMember("e_machine")
        if self.e_machine != const.elfMachineType.EM_HEXAGON:
            utils.raiseElfManipulatorError("Elf32_Ehdr.e_type failed validation.")

        ##  This member identifies the object file version.
        self.e_version = self.getStructMember("e_version")

        ##  This member gives the virtual address to which the system first
        #   transfers control, thus starting the process. If the file has no
        #   associated entry point, this member holds zero.
        self.e_entry = self.getStructMember("e_entry")

        ##  This member holds the program header table's file offset in bytes.
        #   If the file has no program header table, this member holds zero.
        self.e_phoff = self.getStructMember("e_phoff")

        ##  This member holds the section header table's file offset in bytes.
        #   If the file has no section header table, this member holds zero.
        self.e_shoff = self.getStructMember("e_shoff")

        ##  This member holds processor-specific flags associated with the file.
        #   Flag names take the form EF_machine_flag. Flags should indicate that
        #   both the version of the Hexagon processor that the object file was
        #   created for AND the highest version of the Hexagon processor that
        #   the object file contains for is V5
        self.e_flags = self.getStructMember("e_flags")

        ##  This member holds the ELF header's size in bytes.
        self.e_ehsize = self.getStructMember("e_ehsize")

        ##  This member holds the size in bytes of one entry in the file's
        #   program header table; all entries are the same size.
        self.e_phentsize = self.getStructMember("e_phentsize")

        ##  This member holds the number of entries in the program header table.
        #   Thus the product of e_phentsize and e_phnum gives the table's size
        #   in bytes. If a file has no program header table, e_phnum holds the
        #   value zero.
        self.e_phnum = self.getStructMember("e_phnum")

        ##  This member holds a section header's size in bytes. A section header
        #   is one entry in the section header table; all entries are the same
        #   size.
        self.e_shentsize = self.getStructMember("e_shentsize")

        ##  This member holds the number of entries in the section header table.
        #   Thus the product of e_shentsize and e_shnum gives the section header
        #   table's size in bytes. If a file has no section header table,
        #   e_shnum holds the value zero.
        self.e_shnum = self.getStructMember("e_shnum")

        ##  This member holds the section header table index of the entry
        #   associated with the section name string table. If the file has no
        #   section name string table, this member holds the value SHN_UNDEF.
        self.e_shstrndx = self.getStructMember("e_shstrndx")

    ##
    # @brief    Member function to write internal data out to an ELF
    # @param    elfFileHandle   File handle to the output ELF
    # @param    offset          Offset to start writing internal data
    def flushToELF(self, elfFileHandle, offset):
        ## File handle
        self.elfFileHandle = elfFileHandle
        ## Offset that will be used internally
        self.structOffset = offset
        self.writeToELFAndUpdateOffset( "e_ident[EI_MAG0]",
                                        self.e_ident[const.elfIdentification.EI_MAG0])
        self.writeToELFAndUpdateOffset( "e_ident[EI_MAG1]",
                                        self.e_ident[const.elfIdentification.EI_MAG1])
        self.writeToELFAndUpdateOffset( "e_ident[EI_MAG2]",
                                        self.e_ident[const.elfIdentification.EI_MAG2])
        self.writeToELFAndUpdateOffset( "e_ident[EI_MAG3]",
                                        self.e_ident[const.elfIdentification.EI_MAG3])
        self.writeToELFAndUpdateOffset( "e_ident[EI_CLASS]",
                                        self.e_ident[const.elfIdentification.EI_CLASS])
        self.writeToELFAndUpdateOffset( "e_ident[EI_DATA]",
                                        self.e_ident[const.elfIdentification.EI_DATA])
        self.writeToELFAndUpdateOffset( "e_ident[EI_VERSION]",
                                        self.e_ident[const.elfIdentification.EI_VERSION])
        self.writeToELFAndUpdateOffset( "e_ident[EI_OSABI]",
                                        self.e_ident[const.elfIdentification.EI_OSABI])
        self.writeToELFAndUpdateOffset( "e_ident[EI_ABIVERSION]",
                                        self.e_ident[const.elfIdentification.EI_ABIVERSION])
        for i in range(const.elfIdentification.EI_NIDENT - const.elfIdentification.EI_PAD):
            self.writeToELFAndUpdateOffset("e_ident[EI_PAD]",0)
        self.writeToELFAndUpdateOffset("e_type",self.e_type)
        self.writeToELFAndUpdateOffset("e_machine",self.e_machine)
        self.writeToELFAndUpdateOffset("e_version",self.e_version)
        self.writeToELFAndUpdateOffset("e_entry",self.e_entry)
        self.writeToELFAndUpdateOffset("e_phoff",self.e_phoff)
        self.writeToELFAndUpdateOffset("e_shoff",self.e_shoff)
        self.writeToELFAndUpdateOffset("e_entry",self.e_flags)
        self.writeToELFAndUpdateOffset("e_ehsize",self.e_ehsize)
        self.writeToELFAndUpdateOffset("e_phentsize",self.e_phentsize)
        self.writeToELFAndUpdateOffset("e_phnum",self.e_phnum)
        self.writeToELFAndUpdateOffset("e_shentsize",self.e_shentsize)
        self.writeToELFAndUpdateOffset("e_shnum",self.e_shnum)
        self.writeToELFAndUpdateOffset("e_shstrndx",self.e_shstrndx)

##
# @brief    This class represents the ELF program header struct
class Elf32_Phdr(elfStruct):
    """
    typedef struct {
        Elf32_Word      p_type;
        Elf32_Off       p_offset;
        Elf32_Addr      p_vaddr;
        Elf32_Addr      p_paddr;
        Elf32_Word      p_filesz;
        Elf32_Word      p_memsz;
        Elf32_Word      p_flags;
        Elf32_Word      p_align;
    } Elf32_Phdr;
    """

    ## Member function to set struct member sizes
    def setStructMemberSizes(self):
        ## List containing all members of type Elf32_Half
        self.halfMembers = []
        ## List containing all members of type Elf32_(Addr|Off|Sword|Word)
        self.longMembers = ["p_type",
                            "p_offset",
                            "p_vaddr",
                            "p_paddr",
                            "p_filesz",
                            "p_memsz",
                            "p_flags",
                            "p_align"
                            ]
        ## List containing all members of type unsigned_char
        self.byteMembers = []

    ## Member function to extract and validate data struct
    def setAndValidateStructMembers(self):
        if (self.verbose):
            print "============================================================"
            print " ** VERBOSE DEBUG MSG ** PROGRAM HEADER (Elf32_Phdr)"
            print "============================================================"

        ##  This member tells what kind of segment this array element describes
        #   how to interpret the array element's information. All segments
        #   should be loadable
        self.p_type = self.getStructMember("p_type")
        if self.p_type != const.segmentTypes.PT_LOAD:
            #utils.raiseElfManipulatorError("Elf32_Phdr.p_type failed validation.")
            if self.verbose:
              print "Skip Elf32_Phdr.p_type validation for dynamic loading"

        ##  This member gives the offset from the beginning of the file at which
        #   the first byte of the segment resides.
        self.p_offset = self.getStructMember("p_offset")

        ##  This member gives the virtual address at which the first byte of the
        #   segment resides in memory.
        self.p_vaddr = self.getStructMember("p_vaddr")

        ##  On systems for which physical addressing is relevant, this member is
        #   reserved for the segment's physical address.
        self.p_paddr = self.getStructMember("p_paddr")

        ##  This member gives the number of bytes in the file image of the
        #   segment; it may be zero.
        self.p_filesz = self.getStructMember("p_filesz")

        ##  This member gives the number of bytes in the memory image of the
        #   segment; it may be zero.
        self.p_memsz = self.getStructMember("p_memsz")

        ##  This member gives permission flags relevant to the segment.
        self.p_flags = self.getStructMember("p_flags")
        """
        ## Flag representing execute permissions
        self.p_flagsX = self.p_flags & const.segmentFlags.PF_X
        ## Flag representing write permissions
        self.p_flagsW = self.p_flags & const.segmentFlags.PF_W
        ## Flag representing read permissions
        self.p_flagsR = self.p_flags & const.segmentFlags.PF_R
        ## Flag representing OS-specific permissions
        self.p_flagsO = self.p_flags & const.segmentFlags.PF_MASKOS
        ## Flag representing processor-specific permissions
        self.p_flagsP = self.p_flags & const.segmentFlags.PF_MASKPROC
        """

        ##  Loadable process segments must have congruent values for p_vaddr and
        #   p_offset, modulo the page size. This member gives the value to which
        #   the segments are aligned in memory and in the file. Values 0 and 1
        #   mean no alignment is required. Otherwise, p_align should be a
        #   positive, integral power of 2, and p_vaddr should equal p_offset,
        #   modulo p_align.
        self.p_align = self.getStructMember("p_align")
        # Check for congruence, if the file size is not zero
        if (self.p_align not in [0,1]):
            if utils.isPowerOfTwo(self.p_align):
                if ((self.p_filesz != 0) and
                    (self.p_vaddr%self.p_align != self.p_offset%self.p_align)):
                    utils.raiseElfManipulatorWarning(
                        "Elf32_Phdr %i failed p_align validation." %
                        (self.idx,))
            else:
                utils.raiseElfManipulatorWarning(
                    "Elf32_Phdr %i failed p_align validation." %
                    (self.idx,))

    ##
    # @brief    Member function to write internal data out to an ELF
    # @param    elfFileHandle   File handle to the output ELF
    # @param    offset          Offset to start writing internal data
    def flushToELF(self, elfFileHandle, offset):
        ## File handle
        self.elfFileHandle = elfFileHandle
        ## Offset that will be used internally
        self.structOffset = offset
        self.writeToELFAndUpdateOffset("p_type",self.p_type)
        self.writeToELFAndUpdateOffset("p_offset",self.p_offset)
        self.writeToELFAndUpdateOffset("p_vaddr",self.p_vaddr)
        self.writeToELFAndUpdateOffset("p_paddr",self.p_paddr)
        self.writeToELFAndUpdateOffset("p_filesz",self.p_filesz)
        self.writeToELFAndUpdateOffset("p_memsz",self.p_memsz)
        self.writeToELFAndUpdateOffset("p_flags",self.p_flags)
        self.writeToELFAndUpdateOffset("p_align",self.p_align)

##
# @brief    This class represents the ELF section header struct
class Elf32_Shdr(elfStruct):
    """
    typedef struct {
        Elf32_Word      sh_name;
        Elf32_Word      sh_type;
        Elf32_Word      sh_flags;
        Elf32_Addr      sh_addr;
        Elf32_Off       sh_offset;
        Elf32_Word      sh_size;
        Elf32_Word      sh_link;
        Elf32_Word      sh_info;
        Elf32_Word      sh_addralign;
        Elf32_Word      sh_entsize;
    } Elf32_Shdr;
    """

    ## Member function to set struct member sizes
    def setStructMemberSizes(self):
        ## List containing all members of type Elf32_Half
        self.halfMembers = []
        ## List containing all members of type Elf32_(Addr|Off|Sword|Word)
        self.longMembers = ["sh_name",
                            "sh_type",
                            "sh_flags",
                            "sh_addr",
                            "sh_offset",
                            "sh_size",
                            "sh_link",
                            "sh_info",
                            "sh_addralign",
                            "sh_entsize"
                            ]
        ## List containing all members of type unsigned_char
        self.byteMembers = []

    ## Member function to extract and validate data struct
    def setAndValidateStructMembers(self):
        if (self.verbose):
            print "============================================================"
            print " ** VERBOSE DEBUG MSG ** SECTION HEADER (Elf32_Shdr)"
            print "============================================================"

        ##  This member specifies the name of the section. Its value is an index
        #   into the section header string table section, giving the location of
        #   a null terminated string.
        self.sh_name = self.getStructMember("sh_name")

        ##  This member categorizes the section's contents and semantics.
        self.sh_type = self.getStructMember("sh_type")
        if (self.sh_type not in [const.sectionTypes.SHT_NULL,
                                 const.sectionTypes.SHT_PROGBITS,
                                 const.sectionTypes.SHT_NOBITS,
                                 const.sectionTypes.SHT_STRTAB,
                                 const.sectionTypes.SHT_SYMTAB
                                ]):
            #utils.raiseElfManipulatorError("Elf32_Shdr.sh_type failed validation.")
            if self.verbose:
              print "Skip Elf32_Shdr.sh_type validation for dynamic loading"

        ##  Sections support 1-bit flags that describe miscellaneous attributes.
        self.sh_flags = self.getStructMember("sh_flags")

        ##  If the section will appear in the memory image of a process, this
        #   member gives the address at which the section's first byte should
        #   reside. Otherwise, the member contains 0.
        self.sh_addr = self.getStructMember("sh_addr")

        ##  This member's value gives the byte offset from the beginning of the
        #   file to the first byte in the section. One section type, SHT_NOBITS,
        #   occupies no space in the file, and its sh_offset member locates the
        #   conceptual placement in the file.
        self.sh_offset = self.getStructMember("sh_offset")

        ##  This member gives the section's size in bytes. Unless the section
        #   type is SHT_NOBITS, the section occupies sh_size bytes in the file.
        #   A section of type SHT_NOBITS may have a non-zero size, but it
        #   occupies no space in the file.
        self.sh_size = self.getStructMember("sh_size")

        ##  This member holds a section header table index link, whose
        #   interpretation depends on the section type.
        self.sh_link = self.getStructMember("sh_link")
        # Print out detailed interpretation
        if (self.verbose):
            if (self.sh_type == const.sectionTypes.SHT_DYNAMIC):
                print "\tThe section header index of the string table used by entries in the section"
            elif (self.sh_type == const.sectionTypes.SHT_HASH):
                print "\tThe section header index of the symbol table to which the hash table applies"
            elif (  self.sh_type == const.sectionTypes.SHT_REL or
                    self.sh_type == const.sectionTypes.SHT_RELA):
                print "\tThe section header index of the associated symbol table"
            elif (  self.sh_type == const.sectionTypes.SHT_SYMTAB or
                    self.sh_type == const.sectionTypes.SHT_SYMTAB):
                print "\tThe section header index of the associated string table"
            else:
                print "\tSHN_UNDEF"

        ##  This member holds extra information, whose interpretation depends on
        #   the section type.
        self.sh_info = self.getStructMember("sh_info")
        # Print out detailed interpretation
        if (self.verbose):
            if (self.sh_type == const.sectionTypes.SHT_DYNAMIC):
                print "\t0"
            elif (self.sh_type == const.sectionTypes.SHT_HASH):
                print "\t0"
            elif (  self.sh_type == const.sectionTypes.SHT_REL or
                    self.sh_type == const.sectionTypes.SHT_RELA):
                print "\tThe section header index of the section to which the relocation applies"
            elif (  self.sh_type == const.sectionTypes.SHT_SYMTAB or
                    self.sh_type == const.sectionTypes.SHT_DYNSYM):
                print "\tOne greater than the symbol table index of the last local symbol (binding STB_LOCAL)"
            else:
                print "\t0"

        ##  Some sections have address alignment constraints. For example, if a
        #   section holds a doubleword, the system must ensure doubleword
        #   alignment for the entire section. That is, the value of sh_addr must
        #   be congruent to 0, modulo the value of sh_addralign. Currently, only
        #   0 and positive integral powers of two are allowed. Values 0 and 1
        #   mean the section has no alignment constraints.
        self.sh_addralign = self.getStructMember("sh_addralign")
        if (self.sh_addralign not in [0,1]):
            if (self.sh_addr % self.sh_addralign):
                # debug print:
                # section_values = [
                #     (i, getattr(self, i))
                #     for i in self.__dict__.keys()
                #     if (
                #         (
                #             i.startswith('sh_')
                #             or i == 'idx'
                #         )
                #         and not hasattr(getattr(self, i), '__call__')
                #     )
                # ]
                # print 'section attributes: %r' % (dict(section_values),)
                utils.raiseElfManipulatorError("Elf32_Shdr.sh_addralign failed validation.")
            if not utils.isPowerOfTwo(self.sh_addralign):
                utils.raiseElfManipulatorError("Elf32_Shdr.sh_addralign failed validation.")

        ##  Some sections hold a table of fixed-size entries, such as a symbol
        #   table. For such a section, this member gives the size in bytes of
        #   each entry. The member contains 0 if the section does not hold a
        #   table of fixed-size entries.
        self.sh_entsize = self.getStructMember("sh_entsize")

        ##  This member contains the contents of the section.
        if self.sh_type != const.sectionTypes.SHT_NOBITS:
            self.contents = utils.getDataFromELF(   self.elf,
                                                    self.sh_offset,
                                                    self.sh_size)

    ##
    # @brief    Member function to set the section name with provided string
    # @param    sectionName     String to be used as the section name
    def setSectionStrName(self, sectionName):
        ##  This member contains the string name of the section.
        self.sh_nameStr = sectionName

    ##
    # @brief    Member function to get section attribute based on flags
    # @param    attribute   Attribute to get
    # @returns  Non-zero if the attribute is enabled. Zero otherwise.
    def getSectionAttribute(self, attribute):
        if attribute in [const.sectionFlags.SHF_WRITE,
                         const.sectionFlags.SHF_ALLOC,
                         const.sectionFlags.SHF_EXECINSTR,
                         const.sectionFlags.SHF_MERGE,
                         const.sectionFlags.SHF_STRINGS,
                         const.sectionFlags.SHF_INFO_LINK,
                         const.sectionFlags.SHF_LINK_ORDER,
                         const.sectionFlags.SHF_GROUP,
                         const.sectionFlags.SHF_TLS,
                         const.sectionFlags.SHF_OS_NONCONFORMING,
                         const.sectionFlags.SHF_MASKOS,
                         const.sectionFlags.SHF_MASKPROC
                        ]:
            return self.sh_flags & attribute
        else:
            utils.raiseElfManipulatorError("No such section attribute")

    ##
    # @brief    Member function to set section attribute based on flags
    # @param    attribute   Attribute to set
    def setSectionAttribute(self, attribute):
        if attribute in [const.sectionFlags.SHF_WRITE,
                         const.sectionFlags.SHF_ALLOC,
                         const.sectionFlags.SHF_EXECINSTR,
                         const.sectionFlags.SHF_MERGE,
                         const.sectionFlags.SHF_STRINGS,
                         const.sectionFlags.SHF_INFO_LINK,
                         const.sectionFlags.SHF_LINK_ORDER,
                         const.sectionFlags.SHF_GROUP,
                         const.sectionFlags.SHF_TLS,
                         const.sectionFlags.SHF_OS_NONCONFORMING,
                         const.sectionFlags.SHF_MASKOS,
                         const.sectionFlags.SHF_MASKPROC
                        ]:
            self.sh_flags |= attribute
        else:
            utils.raiseElfManipulatorError("No such section attribute")
      
    ##
    # @brief    Member function to write internal data out to an ELF
    # @param    elfFileHandle   File handle to the output ELF
    # @param    offset          Offset to start writing internal data
    def flushToELF(self, elfFileHandle, offset):
        ## File handle
        self.elfFileHandle = elfFileHandle
        ## Offset that will be used internally
        self.structOffset = offset
        self.writeToELFAndUpdateOffset("sh_name",self.sh_name)
        self.writeToELFAndUpdateOffset("sh_type",self.sh_type)
        self.writeToELFAndUpdateOffset("sh_flags",self.sh_flags)
        self.writeToELFAndUpdateOffset("sh_addr",self.sh_addr)
        self.writeToELFAndUpdateOffset("sh_offset",self.sh_offset)
        self.writeToELFAndUpdateOffset("sh_size",self.sh_size)
        self.writeToELFAndUpdateOffset("sh_link",self.sh_link)
        self.writeToELFAndUpdateOffset("sh_info",self.sh_info)
        self.writeToELFAndUpdateOffset("sh_addralign",self.sh_addralign)
        self.writeToELFAndUpdateOffset("sh_entsize",self.sh_entsize)

##
# @brief    This class represents the ELF symbol struct
class Elf32_Sym(object):
    """
    typedef struct{
        Elf32_Word      st_name;
        Elf32_Addr      st_value;
        Elf32_Word      st_size;
        unsigned_char   st_info;
        unsigned_char   st_other;
        Elf32_Half      st_shndx;
    }Elf32Sym;
    """
    ##
    # @brief    Special member function that is automatically invoked for a new
    #           instance. Initializes struct members.
    # @param    symbolData  The struct data extracted from .symtab contents
    # @param    strtab      The .strtab section
    def __init__(self, symbolData, strtab):
        ##  This member holds an index into the object file's symbol string
        #   table, which holds the character representations of the symbol
        #   names. If the value is non-zero, it represents a string table index
        #   that gives the symbol name. Otherwise, the symbol table entry has no
        #   name.
        self.st_name = utils.convertFromELF(    symbolData,
                                                const.elfDataTypes.Elf32_Word,
                                                const.Elf32_SymOffsets.st_name)
        ##  This member gives the value of the associated symbol. Depending on
        #   the context, this may be an absolute value, an address, etc.
        self.st_value = utils.convertFromELF(   symbolData,
                                                const.elfDataTypes.Elf32_Addr,
                                                const.Elf32_SymOffsets.st_value)
        ##  Many symbols have associated sizes. For example, a data object's
        #   size is the number of bytes contained in the object. This member
        #   holds 0 if the symbol has no size or an unknown size.
        self.st_size = utils.convertFromELF(    symbolData,
                                                const.elfDataTypes.Elf32_Word,
                                                const.Elf32_SymOffsets.st_size)
        ##  This member specifies the symbol's type and binding attributes.
        self.st_info = utils.convertFromELF(    symbolData,
                                                const.elfDataTypes.unsigned_char,
                                                const.Elf32_SymOffsets.st_info)
        ##  This member currently specifies a symbol's visibility
        self.st_other = utils.convertFromELF(   symbolData,
                                                const.elfDataTypes.unsigned_char,
                                                const.Elf32_SymOffsets.st_other)
        ##  Every symbol table entry is "defined" in relation to some section;
        #   this member holds the relevant section header table index.
        self.st_shndx = utils.convertFromELF(   symbolData,
                                                const.elfDataTypes.Elf32_Half,
                                                const.Elf32_SymOffsets.st_shndx)
        if self.st_name:
            ##  This member contains the string name of the symbol.
            self.st_nameStr = utils.getStringFromContents(  strtab.contents,
                                                            self.st_name)
        else:
            self.st_nameStr = ""

    ##
    # @brief    Member function to determine if a symbol is weak
    # @return   True if the symbol is weak, false otherwise.
    def isWeak(self):
        if ((self.st_info >> 4) == const.symbolBinding.STB_WEAK):
            return True
        else:
            return False
