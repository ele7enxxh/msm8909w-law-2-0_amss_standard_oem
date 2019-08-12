##  @file       elfFileClass.py
#   @author     Christopher Ahn (cahn@qualcomm.com)
#   @brief      This file contains the class for ELF files.

#===============================================================================
# Copyright (c) 2013 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/build/myps/elfManipulator/include/elfFileClass.py#1 $
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
# 09/10/14  cahn    Added moveupElfOffsetSectionByName.
# 05/12/14  cahn    Added support for SHN_ABS symbols to updateSymbolValueByName
# 03/12/14  cahn    Fix for sections whose end address is at the start of
#                   another segment.
# 02/04/14  cahn    Added removeBssOverlaySectionByName.
# 01/21/14  cahn    Fixed bug in removeSectionByName and simplified logic to
#                   only support removal of single section segment.
# 11/21/13  cahn    Added updateSymbolValuesByDict.
# 07/22/13  cahn    Added getSymbolByNameList.
# 04/05/13  cahn    Check for symbol existence in printSymbolInfoByName.
# 04/05/13  cahn    Added getSymbolByName and printSymbolInfoByName.
# 04/03/13  cahn    When moving up a section, mark symbols as undefined.
# 04/03/13  cahn    Added compression level for compressSectionByName, and
#                   moveSectionIntoPadding (kept internal for now).
# 03/29/13  cahn    Various new APIs added. Changed the name of functions.
# 03/28/13  cahn    Memory optimization by using generator for symbol table.
# 03/26/13  cahn    Support for ro_fatal compression.
# 03/18/13  cahn    Added support for running compression and getting section by
#                   name in preparation of QShrink and ro_fatal integration.
# 03/18/13  cahn    Added support for resizing of sections and updating symbol
#                   value by name.
# 03/15/13  cahn    Added support for removal of section.
# 03/11/13  cahn    Code cleanup.
# 03/10/13  cahn    Write out modified ELF based on member variables instead of
#                   member struct.
# 03/10/13  cahn    Added functionality for writing out modified ELF.
#                   Includes placeholder function for modified ELF verification.
# 03/07/13  cahn    Specification review, cleanup and added Doxygen comments.
# 03/07/13  cahn    Creation.
#===============================================================================

# Imports
import elfConstants as const
import elfStructs as structs
import elfUtils as utils
import zlib

##
# @brief    Generator function to go through the symbol table
# @param    symtab      The .symtab section
# @param    strtab      The .strtab section
# @pre      The .symtab and .strtab Elf32_Shdr objects are populated
def Elf32_SymGenerator(symtab, strtab):
    assert len(symtab.contents) % symtab.sh_entsize == 0
    for i in xrange(0, len(symtab.contents), symtab.sh_entsize) :
        yield structs.Elf32_Sym(symtab.contents[i : i + symtab.sh_entsize], strtab)

##
# @brief    This class provides an abstraction to the ELF file.
class elfFile:
    ##
    # @brief    Special member function that is automatically invoked for a new
    #           instance. Initializes object data based on input elf file.
    # @param    elf     A full path to the input ELF file used to initialize a
    #                   new class instance.
    def __init__ (self, elf):
        ##  The full path to the input ELF file
        self.elf = elf
        ##  ELF Header
        self.elfHeader = self.__getElfHeaderFromElf()
        ##  ELF Program Header Table
        self.programHeaderTable = self.__getProgramHeaderTable()
        ##  ELF Section Header Table
        self.sectionHeaderTable = self.__getSectionHeaderTable()
        self.__updateAllSectionStringNames()
        ##  Section to Segment Mapping
        self.sectionToSegmentDict = self.__getSectionToSegmentDict()

    ##
    # @brief    Member function to get the ELF header describing the file's
    #           organization
    # @returns  ELF header
    def __getElfHeaderFromElf(self):
        # ELF header size, as determined by sizeof(Elf32_Ehdr)
        size = (const.elfIdentification.EI_NIDENT
                * const.elfDataTypes.unsigned_char
                + 8 * const.elfDataTypes.Elf32_Half
                + 2 * const.elfDataTypes.Elf32_Word
                + 1 * const.elfDataTypes.Elf32_Addr
                + 2 * const.elfDataTypes.Elf32_Off
                )
        # Return the elf header object
        return structs.Elf32_Ehdr(self.elf, 0, size)

    ##
    # @brief    Member function to get the ELF program header table (segments)
    # @pre      ELF header has been populated
    # @returns  ELF program header table
    def __getProgramHeaderTable(self):
        # Retrieve required information from ELF header and iterate through
        # each program header
        offset = self.elfHeader.e_phoff
        size = self.elfHeader.e_phentsize
        programHeaderTable = []
        for ph in range(self.elfHeader.e_phnum):
            programHeaderTable.append(structs.Elf32_Phdr(self.elf,offset,size,ph))
            offset += size
        # Return the program header table as an array
        return programHeaderTable

    ##
    # @brief    Member function to get the ELF section header table (sections)
    # @pre      ELF header has been populated
    # @returns  sectionHeaderTable    ELF section header table
    def __getSectionHeaderTable(self):
        # Retrieve required information from ELF header and iterate through
        # each section header
        offset = self.elfHeader.e_shoff
        size = self.elfHeader.e_shentsize
        sectionHeaderTable = []
        for sh in range(self.elfHeader.e_shnum):
            sectionHeaderTable.append(structs.Elf32_Shdr(self.elf,offset,size,sh))
            offset += size
        # Return the section header table as an array
        return sectionHeaderTable

    ##
    # @brief  Member function to update all sections with their respective
    #         string names from the section name string table
    # @pre    ELF header AND section header table have been populated
    def __updateAllSectionStringNames(self):
        if (self.elfHeader.e_shstrndx != const.specialSectionsIndexes.SHN_UNDEF):
            # Iterate through each section header table entry and update its
            # name with the corresponding string in the section name string table
            for sh in self.sectionHeaderTable:
                sh.setSectionStrName(utils.getStringFromContents(
                    self.sectionHeaderTable[self.elfHeader.e_shstrndx].contents,
                    sh.sh_name))

    ##
    # @brief    Member function to map each section to its corresponding
    #           segment.
    # @pre      ELF program header table and section header table have been
    #           populated.
    # @returns  sectionToSegmentDict    ELF section to segment mapping
    def __getSectionToSegmentDict(self):
        sectionToSegmentDict = {}
        phCount = 0
        # Pseudocode
        # for (each segment in program header table)
        #   for (each section in section header table)
        #       if section in segment, add it to the dictionary
        # Since each section can only belong to 1 segment, a minor optimization
        # could be to mark a section as "consumed". However, due to the small
        # sizes, this overhead is negligible to the overall runtime.
        for ph in self.programHeaderTable:
            if (ph.p_vaddr == 0):
                continue
            for sh in self.sectionHeaderTable:
                if (ph.p_vaddr <= sh.sh_addr < ph.p_vaddr + ph.p_memsz):
                    if sectionToSegmentDict.has_key(ph.idx):
                        sectionToSegmentDict[ph.idx].append(sh.idx)
                    else:
                        sectionToSegmentDict[ph.idx] = [sh.idx,]

        # Return the section to segment mapping as a dictionary
        return sectionToSegmentDict

    def removeBssOverlaySectionByName(self, sectionName):
        removableSectionFound = 0
        for rmShIdx in range(self.elfHeader.e_shnum):
            if self.sectionHeaderTable[rmShIdx].sh_nameStr == sectionName:
                # A section corresponding to the entry point is not "removable"
                if (self.sectionHeaderTable[rmShIdx].sh_addr
                    == self.elfHeader.e_entry):
                    print "WARNING: '" + sectionName + "' not removable (entry point)"
                    return
                # Only a section of type SHT_PROGBITS is "removable"
                if (self.sectionHeaderTable[rmShIdx].sh_type
                    != const.sectionTypes.SHT_NOBITS):
                    print "WARNING: '" + sectionName + "' not removable (NOBITS)"
                    return
                # Only a multi-section segment is removable
                for rmShPhIdx in self.sectionToSegmentDict:
                    if rmShIdx in self.sectionToSegmentDict[rmShPhIdx]:
                        if len(self.sectionToSegmentDict[rmShPhIdx]) > 1:
                            removableSectionFound = 1
                            break
                        print "WARNING: '" + sectionName + "' not removable (multi-section)"
                        return
                break
        if not removableSectionFound:
            print "WARNING: Section '" + sectionName + "' is currently not removable OR does not exist."
            return
        
        #=======================================================================
        # STEP 2: Update the ELF header
        #=======================================================================
        # If section string table index is greater than the removed section
        # index, update the offset
        if (self.elfHeader.e_shstrndx > rmShIdx):
            self.elfHeader.e_shstrndx -= 1
        # Decrement the number of sections
        self.elfHeader.e_shnum -= 1
        # cahn - dont need to do this, number of program headers remains the same
        # Decrement the number of segments
        # self.elfHeader.e_phnum -= 1
        
        #=======================================================================
        # STEP 3: Update program and section headers, and the mapping
        #=======================================================================
        # cahn - dont need to do this, number of program headers remains the same
        # Go through all program segments, decrementing the idx
        #self.programHeaderTable.remove(self.programHeaderTable[rmShPhIdx])
        #for ph in self.programHeaderTable:
        #    if ph.idx > rmShPhIdx:
        #        ph.idx -= 1

        # Go through all sections, decrementing the idx
        self.sectionHeaderTable.remove(self.sectionHeaderTable[rmShIdx])
        for sh in self.sectionHeaderTable:
            if sh.idx > rmShIdx:
                sh.idx -= 1
        
        # Update sectionToSegmentDict
        self.sectionToSegmentDict = self.__getSectionToSegmentDict()
        
        #=======================================================================
        # STEP 4: Update symbol table
        #=======================================================================
        # Find the symbol table. Update sh_link if required.
        symtabIndex = 0
        for symtab in self.sectionHeaderTable:
            if (symtab.sh_type == const.sectionTypes.SHT_SYMTAB):
                break
            symtabIndex +=1
        if symtabIndex > rmShIdx:
            symtab.sh_link -= 1

        # Iterate through the symbol table, updating the section index
        symtab = self.getSectionByName(".symtab")
        strtab = self.getSectionByName(".strtab")
        if (symtab != const.RC_ERROR and strtab != const.RC_ERROR):
            symbolCount = 0
            contents = ""
            for symbol in Elf32_SymGenerator(symtab, strtab):
                if symbol.st_shndx == rmShIdx:
                    # If section index matches, then set to UNDEF
                    contents += symtab.contents[symtab.sh_entsize * 
                                                symbolCount
                                                : symtab.sh_entsize
                                                * symbolCount
                                                + const.Elf32_SymOffsets.st_shndx]
                    contents += utils.convertToELF( 
                                    const.elfDataTypes.Elf32_Half,
                                    const.specialSectionsIndexes.SHN_UNDEF
                                    )
                elif ((symbol.st_shndx > rmShIdx) and (symbol.st_shndx < 0xff00)):
                    # If section index is greater than removed section, decrement
                    contents += symtab.contents[symtab.sh_entsize * 
                                                symbolCount
                                                : symtab.sh_entsize
                                                * symbolCount
                                                + const.Elf32_SymOffsets.st_shndx]
                    contents += utils.convertToELF( 
                                    const.elfDataTypes.Elf32_Half,
                                    symbol.st_shndx - 1
                                    )
                else:
                    contents += symtab.contents[symtab.sh_entsize * 
                                                symbolCount:
                                                symtab.sh_entsize * 
                                                (symbolCount + 1)]
                symbolCount += 1
            symtab.contents = contents
        else:
            utils.printErrorAndExit("Missing .symtab and/or .strtab section")
    ##
    # @brief    Member function to remove a section by name
    # @param    sectionName     Name of section to remove
    # @note     Section corresponding to the entry point is not "removable"
    # @note     Only a section of type SHT_PROGBITS is "removable"
    # @note     Only a section in a program segment is "removable"
    # @note     Only a single-section segment is "removable"
    def removeSectionByName(self, sectionName):
        #=======================================================================
        # STEP 1:   Find the section to remove and corresponding segment. In
        #           addition, perform checks on all noted assumptions.
        #=======================================================================
        # rmShIdx = section header table index of section to remove
        # rmShPhIdx = program header table index of corresponding segment
        removableSectionFound = 0
        for rmShIdx in range(self.elfHeader.e_shnum):
            if self.sectionHeaderTable[rmShIdx].sh_nameStr == sectionName:
                # A section corresponding to the entry point is not "removable"
                if (self.sectionHeaderTable[rmShIdx].sh_addr
                    == self.elfHeader.e_entry):
                    print "WARNING: '" + sectionName + "' not removable (entry point)"
                    return
                # Only a section of type SHT_PROGBITS is "removable"
                if (self.sectionHeaderTable[rmShIdx].sh_type
                    != const.sectionTypes.SHT_PROGBITS):
                    print "WARNING: '" + sectionName + "' not removable (PROGBITS)"
                    return
                # Only a single-section segment is "removable"
                for rmShPhIdx in self.sectionToSegmentDict:
                    if rmShIdx in self.sectionToSegmentDict[rmShPhIdx]:
                        if len(self.sectionToSegmentDict[rmShPhIdx]) == 1:
                            removableSectionFound = 1
                            break
                        print "WARNING: '" + sectionName + "' not removable (multi-section)"
                        return
                break
        if not removableSectionFound:
            print "WARNING: Section '" + sectionName + "' is currently not removable OR does not exist."
            return

        #=======================================================================
        # STEP 2: Update the ELF header
        #=======================================================================
        # If section string table index is greater than the removed section
        # index, update the offset
        if (self.elfHeader.e_shstrndx > rmShIdx):
            self.elfHeader.e_shstrndx -= 1
        # Decrement the number of sections
        self.elfHeader.e_shnum -= 1
        # Decrement the number of segments
        self.elfHeader.e_phnum -= 1

        #=======================================================================
        # STEP 3: Update program and section headers, and the mapping
        #=======================================================================

        # Go through all program segments, decrementing the idx
        self.programHeaderTable.remove(self.programHeaderTable[rmShPhIdx])
        for ph in self.programHeaderTable:
            if ph.idx > rmShPhIdx:
                ph.idx -= 1

        # Go through all sections, decrementing the idx
        self.sectionHeaderTable.remove(self.sectionHeaderTable[rmShIdx])
        for sh in self.sectionHeaderTable:
            if sh.idx > rmShIdx:
                sh.idx -= 1

        # Update sectionToSegmentDict
        self.sectionToSegmentDict = self.__getSectionToSegmentDict()

        #=======================================================================
        # STEP 4: Update symbol table
        #=======================================================================
        # Find the symbol table. Update sh_link if required.
        symtabIndex = 0
        for symtab in self.sectionHeaderTable:
            if (symtab.sh_type == const.sectionTypes.SHT_SYMTAB):
                break
            symtabIndex +=1
        if symtabIndex > rmShIdx:
            symtab.sh_link -= 1

        # Iterate through the symbol table, updating the section index
        symtab = self.getSectionByName(".symtab")
        strtab = self.getSectionByName(".strtab")
        if (symtab != const.RC_ERROR and strtab != const.RC_ERROR):
            symbolCount = 0
            contents = ""
            for symbol in Elf32_SymGenerator(symtab, strtab):
                if symbol.st_shndx == rmShIdx:
                    # If section index matches, then set to UNDEF
                    contents += symtab.contents[symtab.sh_entsize * 
                                                symbolCount
                                                : symtab.sh_entsize
                                                * symbolCount
                                                + const.Elf32_SymOffsets.st_shndx]
                    contents += utils.convertToELF( 
                                    const.elfDataTypes.Elf32_Half,
                                    const.specialSectionsIndexes.SHN_UNDEF
                                    )
                elif ((symbol.st_shndx > rmShIdx) and (symbol.st_shndx < 0xff00)):
                    # If section index is greater than removed section, decrement
                    contents += symtab.contents[symtab.sh_entsize * 
                                                symbolCount
                                                : symtab.sh_entsize
                                                * symbolCount
                                                + const.Elf32_SymOffsets.st_shndx]
                    contents += utils.convertToELF( 
                                    const.elfDataTypes.Elf32_Half,
                                    symbol.st_shndx - 1
                                    )
                else:
                    contents += symtab.contents[symtab.sh_entsize * 
                                                symbolCount:
                                                symtab.sh_entsize * 
                                                (symbolCount + 1)]
                symbolCount += 1
            symtab.contents = contents
        else:
            utils.printErrorAndExit("Missing .symtab and/or .strtab section")

    ##
    # @brief    Member function to get the a section's segment
    # @param    sectionName     Section name of interest
    # @returns  Elf32_Phdr if successful, const.RC_ERROR otherwise
    def getProgramHeaderBySectionName(self, sectionName):
        for ph in self.programHeaderTable:
            if self.sectionToSegmentDict.has_key(ph.idx):
                for sh in self.sectionToSegmentDict[ph.idx]:
                    if sectionName == self.sectionHeaderTable[sh].sh_nameStr:
                        return ph
        return const.RC_ERROR

    ##
    # @brief    Member function to move up a section's ELF offset by name
    # @param    sectionName     Name of the section to move up
    # @note     USE AT YOUR OWN RISK! This assumes that there are no
    #           overlapping sections/segments in ELF offsets. Furthermore,
    #           the callee is responsible for knowing the order in which
    #           this API should be called.
    def moveupElfOffsetSectionByName(self, sectionName):
        section = self.getSectionByName(sectionName)
        if section != const.RC_ERROR:
            segment = self.getProgramHeaderBySectionName(sectionName)
            if segment != const.RC_ERROR:
                # Only move up a section from a single-section segment
                if len(self.sectionToSegmentDict[segment.idx]) != 1:
                    print "WARNING: Cannot move up section from multi-section segment"
                    return
                previousSegment = None
                minDistance = -1
                # Find the closest segment preceding the segment to move up
                for ph in self.programHeaderTable:
                    if ph.p_offset < segment.p_offset and ph.p_memsz > 0:
                        distance = segment.p_offset - ph.p_offset
                        if minDistance == -1:
                            minDistance = distance
                            previousSegment = ph
                        elif distance < minDistance:
                            minDistance = distance
                            previousSegment = ph
                if previousSegment:
                    # Determine offset and move up elfOffset
                    newElfOffset = ( previousSegment.p_offset +
                                        previousSegment.p_align * (
                                        previousSegment.p_filesz/
                                        previousSegment.p_align+1)
                                        )
                    assert (segment.p_offset == section.sh_offset)
                    elfOffset = segment.p_offset - newElfOffset
                    segment.p_offset -= elfOffset
                    section.sh_offset -= elfOffset
                    # Verification (post elfOffset manipulation)
                    if (segment.p_align not in [0,1]):
                        if utils.isPowerOfTwo(segment.p_align):
                            if ((segment.p_vaddr%segment.p_align) != (segment.p_offset%segment.p_align)):
                                utils.raiseElfManipulatorError("Elf32_Phdr.p_align failed validation.")
                        else:
                            utils.raiseElfManipulatorError("Elf32_Phdr.p_align failed validation.")
        else:
            print "WARNING: moveupElfOffsetSectionByName on non-existing section"

    ##
    # @brief    Member function to move up a section by name
    # @param    sectionName     Name of the section to move up
    # @note     USE AT YOUR OWN RISK! This function will move up the vaddr of
    #           the section, corresponding segment
    # @note     No changes to ELF offsets are made, since we cannot assume if
    #           the previous manipulations resulted in a gap (e.g. QSHRINK)
    def moveupSectionByName(self, sectionName, align=0):
        if align < 0:
            utils.raiseElfManipulatorError(
                'Alignment must be greater than or equal to 0'
            )

        if not utils.isPowerOfTwo(align) and align not in (0, 1):
            utils.raiseElfManipulatorError(
                'Alignment must be 0, 1, or a power of two'
            )

        if align == 0:
            align = 1

        section = self.getSectionByName(sectionName)
        if section != const.RC_ERROR:
            segment = self.getProgramHeaderBySectionName(sectionName)
            if segment != const.RC_ERROR:
                # Only move up a section from a single-section segment
                if len(self.sectionToSegmentDict[segment.idx]) != 1:
                    print "WARNING: Cannot move up section from multi-section segment"
                    return
                previousSegment = None
                minDistance = -1
                # Find the closest segment preceding the segment to move up
                for ph in self.programHeaderTable:
                    if ph.p_vaddr < segment.p_vaddr:
                        distance = segment.p_vaddr - ph.p_vaddr
                        if minDistance == -1:
                            minDistance = distance
                            previousSegment = ph
                        elif distance < minDistance:
                            minDistance = distance
                            previousSegment = ph
                if previousSegment:
                    # Determine offset and move up vaddr, paddr, sh_addr, and
                    # all symbol table entries in that section
                    newSegmentStart = ( previousSegment.p_vaddr +
                                        previousSegment.p_align * (
                                        previousSegment.p_memsz/
                                        previousSegment.p_align+1)
                                        )

                    newSegmentStart = (
                        (newSegmentStart + align - 1)
                        & ~(align - 1)
                    )

                    assert ((newSegmentStart % segment.p_align) == 0)
                    offset = segment.p_vaddr - newSegmentStart
                    segment.p_vaddr -= offset
                    segment.p_paddr -= offset
                    section.sh_addr -= offset

                    # cahn - This is removed due to speed up link time - may
                    # need to be re-enabled later for correctness.
                    #symtab = self.getSectionByName(".symtab")
                    #strtab = self.getSectionByName(".strtab")
                    #if (symtab != const.RC_ERROR and strtab != const.RC_ERROR):
                    #    symbolCount = 0
                    #    contents = ""
                    #    for symbol in Elf32_SymGenerator(symtab, strtab):
                    #        if symbol.st_shndx == section.idx:
                    #            contents += symtab.contents[symtab.sh_entsize * 
                    #                                        symbolCount
                    #                                        : symtab.sh_entsize
                    #                                        * symbolCount
                    #                                        + const.Elf32_SymOffsets.st_shndx]
                    #            contents += utils.convertToELF( 
                    #                            const.elfDataTypes.Elf32_Half,
                    #                            const.specialSectionsIndexes.SHN_UNDEF
                    #                            )
                    #        else:
                    #            contents += symtab.contents[symtab.sh_entsize * 
                    #                                        symbolCount:
                    #                                        symtab.sh_entsize * 
                    #                                        (symbolCount + 1)]
                    #        symbolCount += 1
                    #    symtab.contents = contents
                    #else:
                    #    utils.printErrorAndExit("Missing .symtab and/or .strtab section")
            else:
                print "WARNING: Cannot move up section without a segment"
        else:
            print "WARNING: Cannot move up non-existing section"


    ##
    # @brief    Member function to move a section into padding
    # @param    sectionName     Name of the section to move into padding
    # @param    prevSectionName Name of the section used for padding start
    # @param    nextSectionName Name of the section used for padding end
    # @returns  const.RC_SUCCESS if successful. const.RC_ERROR otherwise.
    # @note     Hidden for now...
    def __moveSectionIntoPadding(self, sectionName, prevSectionName, nextSectionName):
        section = self.getSectionByName(sectionName)
        prevSection = self.getSectionByName(prevSectionName)
        nextSection = self.getSectionByName(nextSectionName)
        segment = self.getProgramHeaderBySectionName(sectionName)
        prevSegment = self.getProgramHeaderBySectionName(prevSectionName)
        nextSegment = self.getProgramHeaderBySectionName(nextSectionName)
        if ((section != const.RC_ERROR) and
            (prevSection != const.RC_ERROR) and
            (nextSection != const.RC_ERROR) and
            (segment != const.RC_ERROR) and
            (prevSegment != const.RC_ERROR) and
            (nextSegment != const.RC_ERROR)):
            # Only move a section from a single-section segment
            if len(self.sectionToSegmentDict[segment.idx]) != 1:
                print "WARNING: Cannot move up section from multi-section segment"
                return
            # Determine the start of the new segment, taking account the
            # alignment of the previous segment
            newSegmentStart = ( prevSegment.p_vaddr +
                                prevSegment.p_align * (
                                prevSegment.p_memsz/
                                prevSegment.p_align+1)
                                )
            # Compute the available size
            availableSize = nextSegment.p_vaddr - newSegmentStart
            # Try to squeeze in the segment
            if segment.p_memsz < availableSize:
                offset = segment.p_vaddr - newSegmentStart
                segment.p_vaddr -= offset
                segment.p_paddr -= offset
                section.sh_addr -= offset

                symtab = self.getSectionByName(".symtab")
                strtab = self.getSectionByName(".strtab")
                if (symtab != const.RC_ERROR and strtab != const.RC_ERROR):
                    symbolCount = 0
                    contents = ""
                    for symbol in Elf32_SymGenerator(symtab, strtab):
                        if symbol.st_shndx == section.idx:
                            contents += symtab.contents[symtab.sh_entsize * 
                                                        symbolCount
                                                        : symtab.sh_entsize
                                                        * symbolCount
                                                        + const.Elf32_SymOffsets.st_value]
                            contents += utils.convertToELF( 
                                            const.elfDataTypes.Elf32_Addr,
                                            symbol.st_value - offset
                                            )
                            contents += symtab.contents[symtab.sh_entsize *
                                                        symbolCount
                                                        + const.Elf32_SymOffsets.st_size:
                                                        symtab.sh_entsize *
                                                        (symbolCount + 1)]
                        else:
                            contents += symtab.contents[symtab.sh_entsize * 
                                                        symbolCount:
                                                        symtab.sh_entsize * 
                                                        (symbolCount + 1)]
                        symbolCount += 1
                    symtab.contents = contents
                    return const.RC_SUCCESS
                else:
                    utils.printErrorAndExit("Missing .symtab and/or .strtab section")
            else:
                print "".join(["Section ",
                               sectionName,
                               "is too large to fit between ",
                               prevSectionName,
                               "and ",
                               nextSectionName])
                return const.RC_ERROR
        else:
            print "WARNING: Cannot find 1 or more of specified sections/segments"
            return const.RC_ERROR

    ##
    # @brief    Member function to remove a section by name
    # @param    sectionName     Name of section to resize
    # @param    newSize         New size of the section
    # @note     Only a section making up an entire program segment is "resizable"
    # @note     Only a section with a current size >= new size is "resizable"
    def resizeSectionByName(self, sectionName, newSize):
        # If newSize is zero, then we need to remove the section
        if newSize == 0:
            self.removeSectionByName(sectionName)
        # Determine if the section is resizable
        resizableSectionFound = 0
        for section in self.sectionHeaderTable:
            if section.sh_nameStr == sectionName:
                # Only a section with a smaller new size is "resizable"
                if section.sh_size < newSize:
                    break
                for segment in self.programHeaderTable:
                    if self.sectionToSegmentDict.has_key(segment.idx):
                        if section.idx in self.sectionToSegmentDict[segment.idx]:
                            # Only a section making up an entire program segment
                            # is "resizable"
                            if len(self.sectionToSegmentDict[segment.idx]) == 1:
                                resizableSectionFound = 1
                            break
                break
        if not resizableSectionFound:
            utils.raiseElfManipulatorError("Section '" + sectionName +
                "' is currently not resizable OR does not exist.")
        # Update the program and section headers
        sizeDelta = section.sh_size - newSize
        if(segment.p_memsz):
            segment.p_memsz -= sizeDelta
        if(segment.p_filesz):
            segment.p_filesz -= sizeDelta
        section.sh_size = newSize

    ##
    # @brief    Member function to update a symbol value by name
    # @param    symbolName      Name of symbol to update
    # @param    newValue        New value for the symbol
    def updateSymbolValueByName(self, symbolName, newValue):
        symtab = self.getSectionByName(".symtab")
        strtab = self.getSectionByName(".strtab")
        if (symtab != const.RC_ERROR and strtab != const.RC_ERROR):
            assert len(symtab.contents) % symtab.sh_entsize == 0
            for i in xrange(0, len(symtab.contents), symtab.sh_entsize) :
                symbol = structs.Elf32_Sym(symtab.contents[i : i + symtab.sh_entsize], strtab)
                if symbol.st_nameStr == symbolName:
                    # If the symbol is an absolute symbol, st_value must be
                    # updated directly
                    if symbol.st_shndx == const.specialSectionsIndexes.SHN_ABS:
                        symtab.contents = "".join([
                            symtab.contents[:i+const.Elf32_SymOffsets.st_value],
                            utils.convertToELF(const.elfDataTypes.Elf32_Addr, newValue),
                            symtab.contents[i+const.Elf32_SymOffsets.st_value+const.elfDataTypes.Elf32_Addr:]
                            ])
                        return
                    elif symbol.st_shndx <= self.elfHeader.e_shnum:
                        # For other, non-special, section header indices, st_value
                        # value must be used to determine the offset into a section
                        offset = symbol.st_value - self.sectionHeaderTable[symbol.st_shndx].sh_addr
                        self.sectionHeaderTable[symbol.st_shndx].contents = "".join([
                            self.sectionHeaderTable[symbol.st_shndx].contents[:offset],
                            utils.convertToELF(symbol.st_size, newValue),
                            self.sectionHeaderTable[symbol.st_shndx].contents[offset+symbol.st_size:]
                            ])
                        return
        else:
            utils.raiseElfManipulatorError("Missing .symtab and/or .strtab section")

        print "WARNING: Symbol '" + symbolName + "' could not be found"

    ##
    # @brief    Member function to update a symbol values by dictionary
    # @param    symbolDict      Dictionary containing (symbol, new value) entries
    def updateSymbolValuesByDict(self, symbolDict):
        symtab = self.getSectionByName(".symtab")
        strtab = self.getSectionByName(".strtab")
        if (symtab != const.RC_ERROR and strtab != const.RC_ERROR):
            for i in xrange(0, len(symtab.contents), symtab.sh_entsize) :
                symbol = structs.Elf32_Sym(symtab.contents[i : i + symtab.sh_entsize], strtab)
                if not symbolDict:
                    break
                if symbolDict.has_key(symbol.st_nameStr):
                    if symbol.st_shndx == const.specialSectionsIndexes.SHN_ABS:
                        # If the symbol is an absolute symbol, st_value must be
                        # updated directly
                        symtab.contents = "".join([
                            symtab.contents[:i+const.Elf32_SymOffsets.st_value],
                            utils.convertToELF(const.elfDataTypes.Elf32_Addr, symbolDict[symbol.st_nameStr]),
                            symtab.contents[i+const.Elf32_SymOffsets.st_value+const.elfDataTypes.Elf32_Addr:]
                            ])
                    elif symbol.st_shndx <= self.elfHeader.e_shnum:
                        # For other, non-special, section header indices, st_value
                        # value must be used to determine the offset into a section
                        #
                        # For executable files, st_value holds a virtual address
                        offset = symbol.st_value - self.sectionHeaderTable[symbol.st_shndx].sh_addr
                        self.sectionHeaderTable[symbol.st_shndx].contents = "".join([
                            self.sectionHeaderTable[symbol.st_shndx].contents[:offset],
                            utils.convertToELF(symbol.st_size, symbolDict[symbol.st_nameStr]),
                            self.sectionHeaderTable[symbol.st_shndx].contents[offset+symbol.st_size:]
                            ])
                    else:
                        utils.raiseElfManipulatorError("Internal error: unhandled case")
                    # Remove the entry from the symbolDict
                    del symbolDict[symbol.st_nameStr]
            if symbolDict:
                print "WARNING: Could not update all symbols. Remaining symbols:"
                print symbolDict
        else:
            utils.raiseElfManipulatorError("Missing .symtab and/or .strtab section")

    ##
    # @brief    Member function to get symbol information by name
    # @param    symbolName      Name of symbol to get a symbol
    # @returns  Elf32_Sym if successful, RC_ERROR otherwise.
    def getSymbolByName(self, symbolName):
        symtab = self.getSectionByName(".symtab")
        strtab = self.getSectionByName(".strtab")
        if (symtab != const.RC_ERROR and strtab != const.RC_ERROR):
            for symbol in Elf32_SymGenerator(symtab, strtab):
                if symbol.st_nameStr == symbolName:
                    return symbol
        else:
            utils.raiseElfManipulatorError("Missing .symtab and/or .strtab section")

        print "WARNING: Symbol '" + symbolName + "' could not be found"
        return const.RC_ERROR

    ##
    # @brief    Member function to get symbol information by list of symbol names
    # @param    symbolNameList      List of strings including names of symbols
    # @returns  List of Elf32_Sym types matching symbolNameList if successful, 
    #           RC_ERROR otherwise.
    def getSymbolByNameList(self, symbolNameList):
        symtab = self.getSectionByName(".symtab")
        strtab = self.getSectionByName(".strtab")
        if (symtab != const.RC_ERROR and strtab != const.RC_ERROR):
            symbolList = []

            # Make the symbolNameList unique
            symbolNameList = list(set(symbolNameList))

            for symbol in Elf32_SymGenerator(symtab, strtab):
                # Inefficient, but assumed that symbolNameList is relatively small
                if symbol.st_nameStr in symbolNameList:
                    symbolList.append(symbol)

        else:
            utils.raiseElfManipulatorError("Missing .symtab and/or .strtab section")
            return const.RC_ERROR
        return symbolList

    ##
    # @brief    Member function to compress a section using zlib
    # @param    sectionName         Name of section to compress
    # @param    compressionLevel    Level of compression to run (default '6')
    # @returns  Elf_Shdr object if section was found, const.RC_ERROR otherwise
    def compressSectionByName(self, sectionName, compressionLevel=6):
        compressSectionFound = 0
        for sh in self.sectionHeaderTable:
            if sh.sh_nameStr == sectionName:
                compressSectionFound = 1
                break
        if compressSectionFound:
            """ SAMPLE COMPRESSION RATIOS ACHIEVED
                                Size        Compression Ratio
            Pre-compression	    2157350	    1.000
            zlib level1	        409562      0.190
            zlib level2         397771      0.184
            zlib level3	        393306      0.182
            zlib level4	        365163      0.169
            zlib level5	        357080      0.166
            zlib level6         351872      0.163 (default)
            zlib level7	        351165      0.163
            zlib level8	        349906      0.162
            zlib level9	        350398      0.162
            """
            sh.contents = zlib.compress(sh.contents, compressionLevel)
            self.resizeSectionByName(sectionName, len(sh.contents))
            return sh
        else:
            print "WARNING: Section '" + sectionName + "' could not be found"
            return const.RC_ERROR

    ##
    # @brief    Member function to get a section by its name
    # @param    sectionName     Name of section to get
    # @returns  Elf_Shdr object if section was found, const.RC_ERROR otherwise
    def getSectionByName(self, sectionName):
        for sh in self.sectionHeaderTable:
            if sh.sh_nameStr == sectionName:
                return sh
        return const.RC_ERROR

    ##
    # @brief    Member function to get a section by its index
    # @param    sectionIndex    Index of the section to get
    # @returns  Elf_Shdr object if section was found, const.RC_ERROR otherwise
    def getSectionByIndex(self, sectionIndex):
        for sh in self.sectionHeaderTable:
            if sh.idx == sectionIndex:
                return sh
        return const.RC_ERROR

    ##
    # @brief    Member function to get a section by address
    # @param    address     Address of the section to get
    # @returns  Elf_Shdr object if section was found, const.RC_ERROR otherwise
    def getSectionByAddress(self, address):
        for sh in self.sectionHeaderTable:
            if sh.sh_addr == address:
                return sh
        return const.RC_ERROR

    ##
    # @brief    Member function to get a segment by its index
    # @param    segmentIndex    Index of the section to get
    # @returns  Elf_Phdr object if section was found, const.RC_ERROR otherwise
    def getSegmentByIndex(self, segmentIndex):
        for ph in self.programHeaderTable:
            if ph.idx == segmentIndex:
                return ph
        return const.RC_ERROR

    ##
    # @brief    Member function to get a segment by its index
    # @param    address     Address of the segment to get
    # @returns  Elf_Phdr object if section was found, const.RC_ERROR otherwise
    def getSegmentByAddress(self, address):
        for ph in self.programHeaderTable:
            if ph.p_vaddr == address:
                return ph
        return const.RC_ERROR

    ##
    # @brief    Member function to get a string starting at an address
    # @param    address     Address pointing to the start of the string
    # @returns  string if section was found, const.RC_ERROR otherwise
    def getStringByAddress(self, address):
        for section in self.sectionHeaderTable:
            if (address >= section.sh_addr and 
                address <= section.sh_addr + section.sh_size):
                offset = section.sh_addr + section.sh_size - address
                return utils.getStringFromContents(section.contents, offset)
        return const.RC_ERROR

    ##
    # @brief    Member function to read data by address
    # @param    address     Address pointing to the start of the data
    # @param    size        Amount of data to read
    # @returns  data if section was found, const.RC_ERROR otherwise
    def readDataByAddress(self, address, size):
        for section in self.sectionHeaderTable:
            if (address >= section.sh_addr and 
                address <= section.sh_addr + section.sh_size):
                offset = section.sh_addr + section.sh_size - address
                return utils.convertFromELF(section.contents[offset:offset+size],size)
        return const.RC_ERROR

    ##
    # @brief    Member function to set data by address
    # @param    address     Address pointing to the start of the data
    # @param    size        Amount of data to set
    # @param    number      Number to pack
    # @returns  const.RC_*
    def setDataByAddress(self, address, size, number):
        for section in self.sectionHeaderTable:
            if (address >= section.sh_addr and 
                address <= section.sh_addr + section.sh_size):
                offset = section.sh_addr + section.sh_size - address
                section.contents = "".join([section.contents[:offset],
                                            utils.convertToELF(size, number),
                                            section.contents[offset+size:]])
                return const.RC_SUCCESS
        return const.RC_ERROR

    ##
    # @brief    Member function to verify ELF data after user modifications
    # @todo     This function is currently a placeholder; Need to determine how
    #           aggressively we want to verify the ELF after user modifications.
    def verify(self):
        print ""

    ##
    # @brief    Member function to write out the ELF after user modifications
    # @param    elf     Full destination path for modified ELF
    def writeOutELF(self, elf):
        # STEP 1: Create a new ELF file
        try:
            elfFileHandle = open(elf, 'wb')
        except IOError:
            utils.raiseElfManipulatorError("Failed to open " + elf)

        # STEP 2: Write out the ELF header
        self.elfHeader.flushToELF(elfFileHandle, 0)

        # STEP 3: Write out program headers
        phCount = 0
        for ph in self.programHeaderTable:
            offset = self.elfHeader.e_phoff + phCount * self.elfHeader.e_phentsize
            ph.flushToELF(elfFileHandle, offset)
            phCount += 1

        # STEP 4: Write out section headers
        shCount = 0
        for sh in self.sectionHeaderTable:
            offset = self.elfHeader.e_shoff + shCount * self.elfHeader.e_shentsize
            sh.flushToELF(elfFileHandle, offset)
            shCount += 1

        # STEP 5: Write out all sections
        for sh in self.sectionHeaderTable:
            if sh.sh_type != const.sectionTypes.SHT_NOBITS:
                utils.writeDataToELF(   elfFileHandle,
                                        sh.sh_offset,
                                        sh.contents
                                        )

        # Flush out data and close the modified ELF file
        elfFileHandle.flush()
        elfFileHandle.close()

    ##
    # @brief    Member function to print symbol information
    # @param    symbolName  Symbol name to print information for
    def printSymbolInfoByName(self, symbolName):
        symbol = self.getSymbolByName(symbolName)
        if symbol != const.RC_ERROR:
            offset = symbol.st_value - self.sectionHeaderTable[symbol.st_shndx].sh_addr
            print ''.join(["Name".ljust(23),
                           " ",
                           "Value".ljust(10),
                           " ",
                           "Size".ljust(10),
                           " ",
                           "Type".ljust(5),
                           " ",
                           "Bind".ljust(5),
                           " ",
                           "Vis".ljust(4),
                           " ",
                           "Ndx".ljust(3),
                           " ",
                           "Contents".ljust(10)
                           ])
            print ''.join([symbol.st_nameStr.ljust(23),
                           " ",
                           str(hex(symbol.st_value)).ljust(10),
                           " ",
                           str(symbol.st_size).ljust(10),
                           " ",
                           str(symbol.st_info&0xf).ljust(5),
                           " ",
                           str(symbol.st_info>>4).ljust(5),
                           " ",
                           str(symbol.st_other&0x3).ljust(4),
                           " ",
                           str(symbol.st_shndx).ljust(3),
                           " ",
                           hex(utils.convertFromELF(
                               self.sectionHeaderTable[symbol.st_shndx].contents,
                               symbol.st_size,
                               offset))
                           ])

    ##
    # @brief    Member function to print ELF information, partially modeled
    #           from readelf -a.
    # @pre      All ELF data have been populated.
    # @note     Code is sub-optimal, but it is only intended for debug.
    def printInfo (self):
        # ELF Header
        print "ELF Header:"
        print " Magic:".ljust(36) + ''.join([
            str(self.elfHeader.e_ident[const.elfIdentification.EI_MAG0]),
            " ",
            str(self.elfHeader.e_ident[const.elfIdentification.EI_MAG1]),
            " ",
            str(self.elfHeader.e_ident[const.elfIdentification.EI_MAG2]),
            " ",
            str(self.elfHeader.e_ident[const.elfIdentification.EI_MAG3]),
            " ",
            str(self.elfHeader.e_ident[const.elfIdentification.EI_CLASS]),
            " ",
            str(self.elfHeader.e_ident[const.elfIdentification.EI_DATA]),
            " ",
            str(self.elfHeader.e_ident[const.elfIdentification.EI_VERSION]),
            " 0 0 0 0 0 0 0 0 0"
             ])

        print " Class:".ljust(36) + str(self.elfHeader.e_ident[const.elfIdentification.EI_CLASS])
        print " Data:".ljust(36) + str(self.elfHeader.e_ident[const.elfIdentification.EI_DATA])
        print " Version:".ljust(36) + str(self.elfHeader.e_ident[const.elfIdentification.EI_VERSION])
        print " OS/ABI:".ljust(36) + str(self.elfHeader.e_ident[const.elfIdentification.EI_OSABI])
        print " ABI Version:".ljust(36) + str(self.elfHeader.e_ident[const.elfIdentification.EI_ABIVERSION])
        print " Type:".ljust(36) + str(self.elfHeader.e_type)
        print " Machine:".ljust(36) + str(self.elfHeader.e_machine)
        print " Version:".ljust(36) + str(hex(self.elfHeader.e_version))
        print " Entry point address:".ljust(36) + str(hex(self.elfHeader.e_entry))
        print " Start of program headers:".ljust(36) + str(self.elfHeader.e_phoff)
        print " Start of section headers:".ljust(36) + str(self.elfHeader.e_shoff)
        print " Flags:".ljust(36) + str(hex(self.elfHeader.e_flags))
        print " Size of this header:".ljust(36) + str(self.elfHeader.e_ehsize)
        print " Size of program headers:".ljust(36) + str(self.elfHeader.e_phentsize)
        print " Number of program headers:".ljust(36) + str(self.elfHeader.e_phnum)
        print " Size of section headers:".ljust(36) + str(self.elfHeader.e_shentsize)
        print " Number of section headers:".ljust(36) + str(self.elfHeader.e_shnum)
        print " Section header string table index: " + str(self.elfHeader.e_shstrndx)
        print

        # Section Header Table
        print "Section Headers:"
        print ''.join([" [",
                       "Nr]".rjust(3),
                       " ",
                       "Name".ljust(36),
                       " ",
                       "Type".ljust(4),
                       " ",
                       "Addr".ljust(10),
                       " ",
                       "Off".ljust(10),
                       " ",
                       "Size".ljust(10),
                       " ",
                       "ES".ljust(2),
                       " ",
                       "Flg".ljust(10),
                       " ",
                       "Lk".ljust(2),
                       " ",
                       "Inf".ljust(6),
                       " ",
                       "Al".ljust(5)
                       ])
        shCount = 0
        for sh in self.sectionHeaderTable:
            print ''.join([" [",
                           (str(shCount) + "]").rjust(3),
                           " ",
                           sh.sh_nameStr.ljust(36),
                           " ",
                           str(sh.sh_type).ljust(4),
                           " ",
                           str(hex(sh.sh_addr)).ljust(10),
                           " ",
                           str(hex(sh.sh_offset)).ljust(10),
                           " ",
                           str(hex(sh.sh_size)).ljust(10),
                           " ",
                           str(sh.sh_entsize).ljust(2),
                           " ",
                           str(hex(sh.sh_flags)).ljust(10),
                           " ",
                           str(int(sh.sh_link)).ljust(2),
                           " ",
                           str(int(sh.sh_info)).ljust(6),
                           " ",
                           str(int(sh.sh_addralign)).ljust(5)
                           ])
            shCount += 1
        print

        # Program Header Table
        print "Program Headers:"
        print ''.join([" Type ",
                   "Offset".ljust(10),
                   " ",
                   "VirtAddr".ljust(10),
                   " ",
                   "PhysAddr".ljust(10),
                   " ",
                   "FileSiz".ljust(10),
                   " ",
                   "MemSiz".ljust(10),
                   " ",
                   "Flg".ljust(3),
                   " ",
                   "Align".ljust(10)
                   ])
        phCount = 0
        for ph in self.programHeaderTable:
            print ''.join([" ",
                       str(ph.p_type).ljust(4),
                       " ",
                       str(hex(ph.p_offset)).ljust(10),
                       " ",
                       str(hex(ph.p_vaddr)).ljust(10),
                       " ",
                       str(hex(ph.p_paddr)).ljust(10),
                       " ",
                       str(hex(ph.p_filesz)).ljust(10),
                       " ",
                       str(hex(ph.p_memsz)).ljust(10),
                       " ",
                       str(hex(ph.p_flags)).ljust(3),
                       " ",
                       str(hex(ph.p_align)).ljust(10)
                       ])
            phCount += 1
        print

        # Section to Segment mapping
        print "Section to Segment mapping"
        print " Segment   Section..."
        for ph in range(self.elfHeader.e_phnum):
            lineToPrint = " " + str(ph).rjust(7) + "   "
            if (self.sectionToSegmentDict.has_key(ph)):
                for sh in self.sectionToSegmentDict[ph]:
                    lineToPrint += str(self.sectionHeaderTable[sh].sh_nameStr) + " "
            print lineToPrint
        print

        # Symbol table
        symtab = self.getSectionByName(".symtab")
        strtab = self.getSectionByName(".strtab")
        if (symtab != const.RC_ERROR and strtab != const.RC_ERROR):
            print ''.join(["Symbol table '",
                       symtab.sh_nameStr,
                       "' contains ",
                       str(len(symtab.contents)/symtab.sh_entsize),
                       " entries:"
                       ])
            print ''.join(["   Num: ",
                       "Value".ljust(10),
                       " ",
                       "Size".ljust(10),
                       " ",
                       "Type".ljust(5),
                       " ",
                       "Bind".ljust(5),
                       " ",
                       "Vis".ljust(4),
                       " ",
                       "Ndx".ljust(10),
                       " ",
                       "Name".ljust(10),
                       ])
            symNum = 0
            for symbol in Elf32_SymGenerator(symtab, strtab):
                print ''.join([" ",
                               (str(symNum) + ":").rjust(6),
                               " ",
                               str(hex(symbol.st_value)).ljust(10),
                               " ",
                               str(symbol.st_size).ljust(10),
                               " ",
                               str(symbol.st_info&0xf).ljust(5),
                               " ",
                               str(symbol.st_info>>4).ljust(5),
                               " ",
                               str(symbol.st_other&0x3).ljust(4),
                               " ",
                               str(symbol.st_shndx).ljust(10),
                               " ",
                               symbol.st_nameStr.ljust(10)
                               ])
                symNum += 1
        else:
            utils.raiseElfManipulatorError("Missing .symtab and/or .strtab section")

# vim: set ts=4 sw=4 et ai nosi:
