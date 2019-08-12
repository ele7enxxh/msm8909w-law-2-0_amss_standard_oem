#===============================================================================
#
# GENERAL DESCRIPTION
#    ARM memory map analysis tool
#
#   This tool parses the standard ARM .map and .lf files and generates analysis 
#   report for memory usage.
#   Updated to support QShrink 3.0 statistics.
#
# Copyright (c) 2013, 2015 Qualcomm Atheros, Inc.
# All Rights Reserved.
# Qualcomm Atheros Confidential and Proprietary
# kmg
#-------------------------------------------------------------------------------
import sys
import re
import os
import glob
import optparse
import subprocess

#Constants for map file parser states
#map file sections 0=none, 1=header, 2=LocalSymbols, 3=GlobalSymbols, 4=MemoryMap, 
#                  5=ComponentObjSizes, 6=ComponentLibSizes, 7=ImageSize, 8=TotalSize
SECTION_NONE = 0
SECTION_HEADER = 1
SECTION_LOCAL_SYMBOLS = 2
SECTION_GLOBAL_SYMBOLS = 3
SECTION_MEMORY_MAP = 4
SECTION_COMPONENT_SIZE = 7
SECTION_TOTAL_SIZE = 8
SECTION_END = 99

#current parser state
parseSection = SECTION_NONE
#previous parsed section
lastsection = 0

#track parser sub-state within section
subState = 0

# section name constant for ELF section containing QSR strings
QSR_REGION_NAME = "QSR_STRING"

#Dictionary to map object names to containing library
# key: object name
# data: library name
obj2libMap = {}

#Dictionary to map object file to directory
# key: object name
# data: directory path
fileMap = {}

#Flag if QSHRINK details found
qshrinkIdentified = False

class MemoryMap:
    """Store and analyze memory map details """
    #
    # _loadRegions is a dictionary which contains the load region details
    # {<load region>: tuple ( [<execute regions>], base, size, max, type)}
    # _executeRegions is a dictionary which contains the execute region details
    # {<execute region>: tuple ( [<input sections>], base, size, max, type)}
    #
    global mapFiles
    _baseAddress = None
    _loadRegions = {}
    _executeRegions = {}
    _curLoadRegion = ''
    _curExecuteRegion = ''
    _hiLoadRegion = None  #highest load region
    def add_load_region(self, regionInfo):
        """Add load region to database"""
        global qshrinkIdentified
        self._curLoadRegion = regionInfo[0]
        regionBase = int(regionInfo[1],16)
        self._loadRegions[self._curLoadRegion] = [],regionBase,int(regionInfo[2],16),int(regionInfo[3],16),regionInfo[4]
        # remember address of lowest load region for base address
        if not self._baseAddress or self._baseAddress > regionBase:
            self._baseAddress = regionBase
        #remember highest found region
        if not self._hiLoadRegion or self._loadRegions[self._hiLoadRegion][1] < regionBase:
            self._hiLoadRegion = self._curLoadRegion
        # check if QSR load region found. If yes, report QShrink stats
        if "QSR_" in regionInfo[0]:
            qshrinkIdentified = True
    def add_execution_region(self, regionInfo):
        """Add execution region to database"""
        self._curExecuteRegion = regionInfo[0]
        self._loadRegions[self._curLoadRegion][0].append(self._curExecuteRegion)
        self._executeRegions[self._curExecuteRegion] = [],int(regionInfo[1],16),int(regionInfo[2],16),int(regionInfo[3],16),regionInfo[4]
    def add_region_detail(self,detailInfo):
        """Add region details to database"""
        if (detailInfo[2] != "PAD"):
            # If this region comes from a library, update the object name to library mapping
            libobjRE = r"([^\(]*)\(([^\)]*)\)"
            libobj = re.match(libobjRE, detailInfo[7])
            if libobj:
                obj2libMap[libobj.group(2)] = libobj.group(1)

            # If the detail info is a QSHRINK string (has "QSR_STR" prefix), add size to object record.
            if ("QSR_STR" in detailInfo[6]):
                # pad size to 4 byte alignment
                strSize = (int(detailInfo[1],16) + 3) & (~3)
                qsrInfo.accumulate_qsr_component(detailInfo[7],strSize)
        pass
    def load_region_size (self, loadRegion):
        """Compute size and end address tuple of load region"""
        # Computes best guess at size of the current load region
        # For most regions, where execution region addresses are within load region and not an overlay, the sum of the 
        # size of the execution regions appears accurate, since this will include ZI and uninit data.  If sum of
        # the execution regions is zero, use the load region size.
        # For overlays, use max of the sum of the execution regions for the common overlays.
        # If execution region is not within the load region (region will be relocated to different address), 
        # then best size estimate is the load region size, since Zi and uninit should not be counted
        regionSize = 0
        for executeRegionName in loadRegion[0]:
            e = self._executeRegions[executeRegionName]
            if (e[1] >= loadRegion[1] and e[1] < loadRegion[1] + loadRegion[3]):
                regionSize += e[2]
        if (regionSize > 0):
            regionEnd = loadRegion[1]+regionSize
        elif loadRegion[2] > 0:
            regionEnd = loadRegion[1]+loadRegion[2]
        else:
            regionEnd = loadRegion[1]+loadRegion[3]
        return (regionSize, regionEnd)
    def update_region_size (self, executeRegionName, size):
        """revise execute region size.  Primarily used to update QSR section size from ELF file"""
        executeRegion = self._executeRegions[executeRegionName]
        self._executeRegions[executeRegionName] = executeRegion[0],executeRegion[1],size,executeRegion[3],executeRegion[4]
    def image_size (self):
        """report best estimate of image size"""
        # Size estimate assumes memory is allocated as a single block from lowest address
        # to highest used load address, and region with highest load address determines end of memory used.
        # Calculation assumes that highest load region is NOT an overlay.
        #
        #QSHRINK:  with QShrink, last region is "QSR_NOTPAGED" and map file doesn't have modified/compressed info.
        # "QSR_NOTPAGED" reduced region size can be extracted from QSR ELF file.
        hiLoadRegion = self._loadRegions[self._hiLoadRegion]
        regionSpan = self.load_region_size(hiLoadRegion)
        return regionSpan[1] - self._baseAddress
    def image_gaps (self):
        """report total of inter-section gaps within RAM image for potentially unused RAM"""
        sortedLoadRegions = sorted(self._loadRegions.items(), key=lambda x: x[1][1])
        gapSize = 0;
        regionEnd = -1
        for loadRegion in sortedLoadRegions:
            info = loadRegion[1]
            # not 1st region & gap to next region found?
            if self._baseAddress != info[1] and (regionEnd < info[1]):
                gapSize += info[1] - regionEnd
            # retrieve extents of region
            regionExtent = self.load_region_size(info)
            regionEnd = regionExtent[1]
        return gapSize
    def display_map(self,fileO):
        """Write summary of memory map regions"""
        sortedLoadRegions = sorted(self._loadRegions.items(), key=lambda x: x[1][1])
        regionEnd = -1
        fileO.write("Imported from: "+mapFiles[0]+"\n")
        fileO.write( "Load_Region,Execute_Region, Base, Size, Max, Attributes\n")
        for loadRegion in sortedLoadRegions:
            info = loadRegion[1]
            # not 1st region & gap to next region
            if self._baseAddress != info[1] and (regionEnd < info[1]):
                fileO.write("Gap:,"+str( info[1] - regionEnd)+"\n")
            fileO.write(loadRegion[0] + ",," + str(info[1]) + "," + str(info[2]) + "," + str(info[3]) + "," + str(info[4])+"\n")

            for executeRegion in info[0]:
                e = self._executeRegions[executeRegion]
                fileO.write("," + executeRegion + "," + str(e[1]) + "," + str(e[2]) + "," + str(e[3]) + "," + str(e[4])+"\n")
            fileO.write("\n")
        imageSize = self.image_size()
        fileO.write("Total DDR space," + str(imageSize) + ",Bytes,"+hex(imageSize)+"\n")

# create global memory map
memoryMap = MemoryMap()


class QSRInfo:
    """Track the QShrink String sizes per component (objs)"""
    # _qsrComponent is a dictionary of objects with QSR entries
    # {<component>: qsrSize)
    _qsrComponent = {}
    def accumulate_qsr_component(self,componentName,size):
        """Accumulate QSR record lengths for component"""
        self._qsrComponent[componentName] = self._qsrComponent.get(componentName,0) + size
        pass
    def get_qsr_component(self, componentName):
        return self._qsrComponent.get(componentName,0)
    def qsr_size(self):
        """Return total size of QSR strings found in image"""
        size = 0
        for comp in self._qsrComponent.keys():
            size += self._qsrComponent[comp]
        return size

#global QSR info
qsrInfo = QSRInfo()


class Components:
    """Store and analyze each component (object file) details """
    # _componentDB and _libComponentDB are dictionaries which contains the object file details:
    # {<component>: tuple (codeSize, ROData, RWData, ZIData, QSTR)
    global qshrinkIdentified
    _componentDB = {}
    _libComponentDB = {}
    _functionalGroupStats = []
    def add_obj_component(self,component):
        """Add object's size info to database"""
        self._componentDB[component[5]] = int(component[0]),int(component[2]),int(component[3]),int(component[4]),qsrInfo.get_qsr_component(component[5])
        pass
    def add_libobj_component(self,component):
        """Add a library object's sizes to database"""
        objname = obj2libMap[component[5]]+"("+component[5]+")"
        self._libComponentDB[objname] = int(component[0]),int(component[2]),int(component[3]),int(component[4]),qsrInfo.get_qsr_component(objname)
        pass
    def add_lib_component(self,component):
        """Add library sizes to database (NOP)"""
        # lib is "summary" of libobj section, so info is redundant.
        pass
    def display_objects (self,fileO):
        """Write memory details of object files not within libraries"""
        codeSum,roSum,rwSum,ziSum,qsrSum = 0,0,0,0,0
        fileO.write("Imported from: "+mapFiles[0]+"\n")
        fileO.write("Object Sizes\n\n")
        if qshrinkIdentified == True:
            fileO.write("Code,RO Data,RW Data,ZI Data,QSR Strings,Object Name\n")
        else:
            fileO.write("Code,RO Data,RW Data,ZI Data,Object Name\n")
        sortedObjects = sorted(self._componentDB.items()+self._libComponentDB.items(), key=lambda x: x[0])
        for obj in sortedObjects:
            code,ro,rw,zi,qsr = obj[1]
            codeSum += code
            roSum += ro
            rwSum += rw
            ziSum += zi
            qsrSum += qsr
            if qshrinkIdentified == True:
                fileO.write(str(code)+","+str(ro)+","+str(rw)+","+str(zi)+","+str(qsr)+","+obj[0]+"\n")
            else:
                fileO.write(str(code)+","+str(ro)+","+str(rw)+","+str(zi)+","+obj[0]+"\n")
        fileO.write("\n______________________________________________________________________\n")
        if qshrinkIdentified == True:
            fileO.write(str(codeSum)+","+str(roSum)+","+str(rwSum)+","+str(ziSum)+","+str(qsrSum)+","+"object size,Note: total does not include padding\n")
        else:
            fileO.write(str(codeSum)+","+str(roSum)+","+str(rwSum)+","+str(ziSum)+","+"object size,Note: total does not include padding\n")
        fileO.write(str(codeSum+roSum+rwSum+ziSum)+",Total object Size\n")
        fileO.write("\n")
        pass
    def display_functional_grouping (self, fileO):
        """Write memory details grouped by functional area
           If file0 == None, gather statistics only
        """
        # groupings are currently hard-coded to 2 levels
        # could make this support variable levels...
        if fileO:
            fileO.write("Imported from: "+mapFiles[0]+"\n")
            fileO.write("Size by functional area\n\n")
        sortedFiles = sorted(fileMap.items(), key=lambda x: x[1])
        curLevel1 = ""
        curLevel2 = ""
        codeSum1,roSum1,rwSum1,ziSum1,qsrSum1 = 0,0,0,0,0
        codeSum,roSum,rwSum,ziSum,qsrSum = 0,0,0,0,0
        report = ""
        for file in sortedFiles:
            if os.sep == '/':
                reLevel = r"([^/]+)/*([^/]+)(?:/|$)"
            else:
                reLevel = r"([^\\]+)\\*([^\\]+)(?:\\|$)"
            level = re.match(reLevel,file[1])
            if (level.group(1) != curLevel1):
                #new category
                if (codeSum1 > 0 or roSum1 > 0 or rwSum1 > 0 or ziSum1 > 0):
                    if qshrinkIdentified == True:
                        report = ",,"+str(codeSum1)+","+str(roSum1)+","+str(rwSum1)+","+str(ziSum1)+","+str(qsrSum1)+",FA "+curLevel1+" Total"
                    else:
                        report = ",,"+str(codeSum1)+","+str(roSum1)+","+str(rwSum1)+","+str(ziSum1)+",FA "+curLevel1+" Total"
                    report += "\n"+",FA Size =,"+str(codeSum1+roSum1+rwSum1+ziSum1)+"\n"
                    self._functionalGroupStats.append((curLevel1,codeSum1,roSum1,rwSum1,ziSum1,qsrSum1))
                curLevel1 = level.group(1)
                curLevel2 = ""
                codeSum1,roSum1,rwSum1,ziSum1,qsrSum1 = 0,0,0,0,0
                report += "\n"+curLevel1
            if (level.group(2) != curLevel2):
                if (codeSum > 0 or roSum > 0 or rwSum > 0 or ziSum > 0):
                    if qshrinkIdentified == True:
                        report1 = ",,"+str(codeSum)+","+str(roSum)+","+str(rwSum)+","+str(ziSum)+","+str(qsrSum)+",Sub-Group Total"
                    else:
                        report1 = ",,"+str(codeSum)+","+str(roSum)+","+str(rwSum)+","+str(ziSum)+",Sub-Group Total"
                    report = report1+"\n"+",Sub-Group Size =,"+str(codeSum+roSum+rwSum+ziSum)+"\n"+report
                else:
                    if fileO:
                        fileO.write("\n")
                if fileO:
                    fileO.write(report+"\n")
                report = ""
                curLevel2 = level.group(2)
                codeSum,roSum,rwSum,ziSum,qsrSum = 0,0,0,0,0
                if fileO:
                    fileO.write(","+curLevel2+"\n")
                    if qshrinkIdentified == True:
                        fileO.write(",,Code,RO Data,RW Data,ZI Data,QSR Strings,Object Name\n")
                    else:
                        fileO.write(",,Code,RO Data,RW Data,ZI Data,Object Name\n")
            #is this file library or object?
            try:
                code,ro,rw,zi,qsr = self._componentDB[file[0]]
                codeSum  += code
                codeSum1 += code
                roSum  += ro
                roSum1 += ro
                rwSum  += rw
                rwSum1 += rw
                ziSum  += zi
                ziSum1 += zi
                qsrSum  += qsr
                qsrSum1 += qsr
                if fileO:
                    if qshrinkIdentified == True:
                        fileO.write(",,"+str(code)+","+str(ro)+","+str(rw)+","+str(zi)+","+str(qsr)+","+file[0]+"\n")
                    else:
                        fileO.write(",,"+str(code)+","+str(ro)+","+str(rw)+","+str(zi)+","+file[0]+"\n")
            except:
                try:
                    # 'file' is a library file.  Search object mapping for all objects within library 'file'
                    # inefficient algorithm - should find way to not iterate across DB for each file
                    for j in obj2libMap:
                        if (obj2libMap[j] != file[0]):
                            continue
                        code,ro,rw,zi,qsr = self._libComponentDB[obj2libMap[j]+"("+j+")"]
                        codeSum  += code
                        codeSum1 += code
                        roSum  += ro
                        roSum1 += ro
                        rwSum  += rw
                        rwSum1 += rw
                        ziSum  += zi
                        ziSum1 += zi
                        qsrSum  += qsr
                        qsrSum1 += qsr
                        if fileO:
                            if qshrinkIdentified == True:
                                fileO.write(",,"+str(code)+","+str(ro)+","+str(rw)+","+str(zi)+","+str(qsr)+","+file[0]+"("+j+")\n")
                            else:
                                fileO.write(",,"+str(code)+","+str(ro)+","+str(rw)+","+str(zi)+","+file[0]+"("+j+")\n")
                except :
                    # lib has no matching object files
                    obj = None
                    pass

        #summary for final grouping
        if (codeSum1 > 0 or roSum1 > 0 or rwSum1 > 0 or ziSum1 > 0):
            if qshrinkIdentified == True:
                report = ",,"+str(codeSum1)+","+str(roSum1)+","+str(rwSum1)+","+str(ziSum1)+","+str(qsrSum1)+",FA "+curLevel1+" Total"
            else:
                report = ",,"+str(codeSum1)+","+str(roSum1)+","+str(rwSum1)+","+str(ziSum1)+",FA "+curLevel1+" Total"
            report += "\n"+",FA Size =,"+str(codeSum1+roSum1+rwSum1+ziSum1)+"\n"
            self._functionalGroupStats.append((curLevel1,codeSum1,roSum1,rwSum1,ziSum1,qsrSum1))
        else:
            report = "\n"
        if (codeSum > 0 or roSum > 0 or rwSum > 0 or ziSum > 0):
            if qshrinkIdentified == True:
                report1 = ",,"+str(codeSum)+","+str(roSum)+","+str(rwSum)+","+str(ziSum)+","+str(qsrSum)+",Sub-Group Total"
            else:
                report1 = ",,"+str(codeSum)+","+str(roSum)+","+str(rwSum)+","+str(ziSum)+",Sub-Group Total"
            report = report1+"\n"+",Sub-Group Size =,"+str(codeSum+roSum+rwSum+ziSum)+"\n"+report
        if fileO:
            fileO.write(report+"\n")
    def print_functional_group_stats (self,file=None):
        """Display statistics for functional groups. Default output is console"""
        if len(self._functionalGroupStats) < 1:
            # statistics not yet collected, compute now
            self.display_functional_grouping(None)
        report = "{0:16} {1:>9}{2:>9}{3:>9}{4:>9}{5:>10}".format("Functional_Area","Code","RO","RW","ZI","Total")
        if file:
            file.write(report+"\n")
        else:
            print report
        for stats in self._functionalGroupStats:
            fa,code,ro,rw,zi,qsr = stats
            report = "{0:16} {1:9d}{2:9d}{3:9d}{4:9d}{5:10d}".format(fa,code,ro,rw,zi,code+ro+rw+zi)
            if file:
                file.write(report+"\n")
            else:
                print report


component = Components()


#General layout of an ARM mapfile:
# Section tag "ARM Linker"
# "Image Symbol Table"
# Section tag "Local Symbols"
# format "Symbol Name                              Value     Ov Type        Size  Object(Section)"
# "Global Symbols"
# format "Symbol Name                              Value     Ov Type        Size  Object(Section)"
# "Memory Map of the image"
# "Image Entry point : %x"
# 
# Repeated section
# "Load Region <name> (Base: %x, Size: %x, Max: %x, <attributes>)"
# Repeated section
# "Execution Region <name> (Base: %x, Size: %x, Max: %x, <attributes>)"
# format "Base Addr    Size         Type   Attr      Idx    E Section Name        Object"
# 
# Section tag "Image component sizes"
# format "Code (inc. data)   RO Data    RW Data    ZI Data      Debug   Object Name"
# <object totals 3 lines>
# format "Code (inc. data)   RO Data    RW Data    ZI Data      Debug   Library Member Name"
# format "Code (inc. data)   RO Data    RW Data    ZI Data      Debug   Library Name"
# 
# <library totals 1 line>
# 
# format "Code (inc. data)   RO Data    RW Data    ZI Data      Debug"
# format "Total RO  Size (Code + RO Data)              2324712 (2270.23kB)"
# format "Total RW  Size (RW Data + ZI Data)           3591400 (3507.23kB)"
# format "Total ROM Size (Code + RO Data + RW Data)    2521736 (2462.63kB)"
# 
######

def new_state(state):
    """Update parser to new state"""
    global parseSection
    global subState
    # print "[",state,parseSection,subState,"]"
    parseSection = state
    subState = 0
    return

# strings which mark each section
topParse = {
    "============": SECTION_NONE,
    "ARM Linker, ": SECTION_HEADER,
    "Image Symbol": SECTION_HEADER,
    "    Local Sy": SECTION_LOCAL_SYMBOLS,
    "    Symbol N": SECTION_LOCAL_SYMBOLS,
    "    Global S": SECTION_GLOBAL_SYMBOLS,
    "Memory Map o": SECTION_MEMORY_MAP,
    "  Image Entr": SECTION_MEMORY_MAP,
    "  Load Regio": SECTION_MEMORY_MAP,
    "    Executio": SECTION_MEMORY_MAP,
    "    Base Add": SECTION_MEMORY_MAP,
    "  **** No se": SECTION_MEMORY_MAP,
    "    --------": SECTION_HEADER,
    "      Code (": SECTION_COMPONENT_SIZE,
    "    Total RO": SECTION_TOTAL_SIZE,
    }

def parse_idle (line):
    """Parse line of map file and determine current section"""
    global topParse
    global parseSection
    if (len(line) == 1):
        # blank line, marks the end of a section.  Go back to idle state
        if (parseSection <> 0):
            lastsection = parseSection
        new_state(SECTION_NONE)
    elif (parseSection == SECTION_NONE):
        new_state(topParse.get(line[0:12], SECTION_HEADER))
        # global sections
        # print "Processing",sections[parseSection]
    else:
        # print "other",parseSection,":",line,
        pass
    return


#symbolRE fields
# symbol name
# value (address)
# Ov Type
# symbol size
# objectfile name (and segment)
symbolRE = r"\s+(\S+)\s+0x([0-9A-Fa-f]+)\s+(Number|Section|ARM Code|Thumb Code|Data)\s+(\d+)\s+(\S+|\S+\s+ABSOLUTE)"
SYMBOL_SS_HEADER = 1
SYMBOL_SS_TABLE = 2

def local_section(line):
    """Parse local symbols"""
    # subState 1 = symbol header
    # subState 2 = symbol table
    global subState
    global symbolRE
    if (len(line) == 1):
        #blank line @ end of section?
        if (subState == SYMBOL_SS_TABLE):
            new_state(SECTION_NONE)
            return
        else:
            subState = subState + 1
    else:
        if (subState == SYMBOL_SS_TABLE):
            # process local symbol info
            # none of this information is used in analysis, so skip it
#            symbolObj = re.match(symbolRE,line)
#            if symbolObj:
#                pass
#            else:
#                print "unparse:",line,
            pass
        pass
    return

def global_section(line):
    """Parse global symbols"""
    # subState 1 = symbol header
    # subState 2 = symbol table
    global subState
    if (len(line) == 1):
        #blank line @ end of section?
        if (subState == SYMBOL_SS_TABLE):
            new_state(SECTION_NONE)
            return
        else:
            subState = subState + 1
    else:
        if (subState == SYMBOL_SS_TABLE):
            # process global symbol info
            # none of this information is used in analysis, so skip it
#            symbolObj = re.match(symbolRE,line)
#            if symbolObj:
#                pass
#            else:
#                print "unparse:",line,
            pass
        pass
    return

# region table substates
REGION_SS_NONE = 0
REGION_SS_HEADER = 1
REGION_SS_ENTRY_POINT = 2
REGION_SS_LOAD = 3
REGION_SS_EXECUTION = 4
REGION_SS_OBJ_HEADER = 5
REGION_SS_OBJECT = 6
REGION_SS_NO_SECTION = 7
REGION_SS_END = 8

# strings which mark each subsection within region tables
memoryMapParse = {
    "Memory Map o": REGION_SS_HEADER,
    "  Image Entr": REGION_SS_ENTRY_POINT,
    "  Load Regio": REGION_SS_LOAD,
    "    Executio": REGION_SS_EXECUTION,
    "    Base Add": REGION_SS_OBJ_HEADER,
    #substate 6 is processing object lines
    "  **** No se": REGION_SS_NO_SECTION,
    "============": REGION_SS_END
    }

#regionRE fields
# region name
# base address
# actual size
# max allowed size
# Attributes
regionRE = r".+Region (\S+) \(Base: 0x(\w+), Size: 0x(\w+), Max: 0x(\w+), (.+)\)"

#regionDetail fields
# base addr
# size
# type (PAD | type)
# Attr
# Idx (ignored?)
# E (dropped)
# section Name
# Object
regionDetailRE =  r"\s+0x(\w+)\s+0x(\w+)\s+(?:(PAD)|(?:(\S+)\s+(\S+)\s+(\d+)[\s\*]+(\S+)\s+(\S+)))"

def memory_map_section(line):
    """Parse map section"""
    global subState
    global regionRE
    global regionDetailRE
    global memoryMap
    if (len(line) == 1):
        #blank line @ end of section
        if (subState == REGION_SS_OBJECT):
            pass
        if (subState == REGION_SS_END):
            new_state(SECTION_NONE)
            # End memory map region
            return
        elif (subState == REGION_SS_OBJ_HEADER):
            subState=REGION_SS_OBJECT
        elif (subState == REGION_SS_OBJECT):
            subState=REGION_SS_NONE
    else:
        subState = memoryMapParse.get(line[0:12], subState)
        if (subState == REGION_SS_ENTRY_POINT):
            # parse Image entry point string
            pass
        elif (subState == REGION_SS_LOAD):
            # parse load region string
            regionObj = re.match(regionRE,line)
            if regionObj:
                # print regionObj.groups()
                memoryMap.add_load_region(regionObj.groups())
                pass
            else:
                # print "unparse:",line,
                pass
            pass
        elif (subState == REGION_SS_EXECUTION):
            # parse execution region string
            regionObj = re.search(regionRE,line)
            if regionObj:
                # print regionObj.groups()
                memoryMap.add_execution_region(regionObj.groups())
                pass
            else:
                # print "unparse:",line,
                pass
            pass
        elif (subState == REGION_SS_OBJ_HEADER):
            # skip header line.  segments processed above
            pass
        elif (subState == REGION_SS_NO_SECTION):
            #empty segment, close results.
            pass
        elif (subState == REGION_SS_END):
            #end of memoryMap section, next blank line drops out
            pass

        if (subState == REGION_SS_OBJECT):
            # process segment elements
            regionObj = re.search(regionDetailRE,line)
            if regionObj:
                memoryMap.add_region_detail(regionObj.groups())
                pass
            else:
                # print "unparse:",line,
                pass
            pass
    return

#image fields
# Code Size
# data within code (also included in Code Size #)
# RO data
# RW Data
# ZI Data
# object Name
imageSizeRE = r"\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+\d+\s+(\S+)"

# there are 3 sub-sections here
# objects, library objects, and libraries
def component_size_section (line):
    """Parse section size entries"""
    global subState
    global imageSizeRE
    if (len(line) == 1):
        #blank line @ end of section?
        if (subState == 12):
            #all done: wrap things up
            new_state(99)
            return
        else:
            subState = subState + 1
    else:
        if (subState == 1):
            # process object component size info
            imageSizeObj = re.match(imageSizeRE,line)
            if imageSizeObj:
                component.add_obj_component(imageSizeObj.groups())
                pass
            else:
                # print "unparse:",line,
                pass
            pass
        elif (subState >= 2 and subState <=4):
            #do nothing for object totals - can recaculate these
            pass
        elif (subState == 5):
            # process library component size info
            imageSizeObj = re.match(imageSizeRE,line)
            if imageSizeObj:
                component.add_libobj_component(imageSizeObj.groups())
                # print imageSizeObj.groups()
                pass
            else:
                # print "unparse:",line,
                pass
            pass
        elif (subState >= 6 and subState <= 8):
            #do nothing for library totals
            pass
        elif (subState == 9):
            # process library size info
            imageSizeObj = re.search(imageSizeRE,line)
            if imageSizeObj:
                #print imageSizeObj.groups()
                pass
            else:
                # print "unparse:",line,
                pass
            pass
        elif (subState >= 10 and subState <=12):
            #do nothing for library totals - can recaculate these
            pass
    return

def total_size_section (line):
    """Parse total size section"""
    #Total size information can be computed from other sections.  This section can be skipped
    # print subState,":",line,
    pass

def report ():
    pass

handleState = {
    SECTION_NONE: parse_idle,
    SECTION_HEADER: parse_idle,
    SECTION_LOCAL_SYMBOLS: local_section,
    SECTION_GLOBAL_SYMBOLS: global_section,
    SECTION_MEMORY_MAP: memory_map_section,
    SECTION_COMPONENT_SIZE: component_size_section,
    SECTION_TOTAL_SIZE: total_size_section,
    SECTION_END: report
    }

def parse (line):
    """Parse a line of map file"""
    global parseSection
    handleState.get(parseSection,parse_idle)(line)

def hex_int (str):
    """Convert string to number.  Checks if input is decimal or hex value"""
    base = 10
    if '0x' in str: base=16
    return int(str,base)

def my_check_output(*popenargs, **kwargs):
    """see subprocess.check_output.  Current build environment may use older
    Python library, so providing local version for this function."""
    if 'stdout' in kwargs:
        raise ValueError('stdout argument not allowed, it will be overridden.')
    process = subprocess.Popen(stdout=subprocess.PIPE, *popenargs, **kwargs)
    output, unused_err = process.communicate()
    retcode = process.poll()
    if retcode:
        cmd = kwargs.get("args")
        if cmd is None:
            cmd = popenargs[0]
        raise subprocess.CalledProcessError(retcode, cmd, output=output)
    return output

def runMapScan (p1,build_id,build_root,mapDirectory,limit,stats):
    global mapFiles
    print "MapScan invoked",p1,build_id,build_root,mapDirectory,limit,stats
    if build_root == None:
        build_ms_root = '.'
    else:
        build_ms_root = os.path.join(build_root, 'build','ms')
    #anchor
    if not build_id:
        # If not otherwise defined, extract build id from mapfile directory name.
        path = os.path.abspath(mapDirectory)
        build_id = os.path.basename(path)

    #remove imagesize ok file, if exists
    image_size_name =  os.path.join(build_ms_root, build_id+"_imageSize.ok")
    if os.access(image_size_name,os.F_OK):
        os.remove(image_size_name)

    f = os.path.join(mapDirectory,'*.map')
    mapFiles = glob.glob(f)
    lfFiles = glob.glob(os.path.join(mapDirectory,'*.lf'))

    if len(mapFiles) < 1 :
        print "Can't open .map file:",f
        exit(-1)

    # parse file paths in .lf files
    # strip off ..\ from beginning, then separate path from file name
    if os.sep == '/':
        fnameRE = r"(?:\.\./)*(.+)/([^/]+)\n"
    else:
        fnameRE = r"(?:\.\.\\)*(.+)\\([^\\]+)\n"

    for lfFile in lfFiles:
        lffile = open(lfFile, 'r')

        for line in lffile:
            if line[0] != '#':
                lfMap = re.match(fnameRE, line).groups()
                if lfMap:
                    fileMap[lfMap[1]] = lfMap[0]

    #process 1st map file found
    mapfile = open(mapFiles[0], 'r')

    if (not mapfile):
        print "can't open map file",mapFiles[0]
        exit(-1)

    if os.sep == '/':
        re1 = r".+/(\S+)\.[^/.]*$"
    else:
        re1 = r".+\\(\S+)\.[^\\.]*$"

    mapBaseName = os.path.join(build_ms_root, (re.match(re1, mapFiles[0]).group(1)))

    for line in mapfile:
        parse(line)
        if (parseSection == 99):
            # time to wrap things up and summarize
            break;

    mapfile.close()

    if qshrinkIdentified:
        # try to extract reduced QShrink section size from QShrink elf
        # it's a lot of work to extract a single #, but it's the most straightforward approach
        try:
            fromelf = os.path.join(os.getenv("ARMROOT",""),"bin", "fromelf")
            f = os.path.join(mapDirectory,'*qsr2.elf')
            qsrElf = glob.glob(f)
            #python 262 is missing this function
            #elfInfo = subprocess.check_output([fromelf,qsrElf[0]],stderr=subprocess.STDOUT)
            elfInfo = my_check_output([fromelf,qsrElf[0]],stderr=subprocess.STDOUT)
        except subprocess.CalledProcessError:
            print "Unable to retrieve qshrink info from ", qsrElf
            pass
        else:
            # print elfInfo
            # find QSR_STRING section and retrieve revised length
            rEx = QSR_REGION_NAME + r".*?:\s(\d+)"
            seglen = re.search(rEx,elfInfo, flags=re.DOTALL)
            qsrNewLen = int(seglen.group(1))
            print "QSR reduced size = ", qsrNewLen
            print "\n\n"
            memoryMap.update_region_size(QSR_REGION_NAME,qsrNewLen)
        pass

    if stats:
        fileOutput = open(mapBaseName+"_map.csv","w")
        memoryMap.display_map(fileOutput)
        fileOutput.close()
        fileOutput = open(mapBaseName+"_objects.csv","w")
        component.display_objects(fileOutput)
        fileOutput.close()
        fileOutput = open(mapBaseName+"_areas.csv","w")
        component.display_functional_grouping(fileOutput)
        fileOutput.close()

    print "\nMemory statistics\n-----------------"
    component.print_functional_group_stats()

    imageSize = memoryMap.image_size()
    gapSize = memoryMap.image_gaps()

    if qshrinkIdentified:
        print "\nTotal QSR string section size",qsrInfo.qsr_size(),"("+hex(qsrInfo.qsr_size())+")"
        print "QSR string section size reduced by QShrink to", qsrNewLen
        qsrReduction = qsrInfo.qsr_size() - qsrNewLen

    print "\nImage Size =",imageSize, "(0x"+hex(imageSize)+") with",gapSize,"unused bytes in gaps."

    if limit:
        if (imageSize <= limit or limit == 0):
            fileOutput = open(image_size_name,"w")
            component.print_functional_group_stats(fileOutput)
            fileOutput.write("\n Image_size Image_limit Unused_space\n")
            fileOutput.write(" {0:10d} {1:11d} {2:12d}\n".format(imageSize, limit, limit-imageSize+gapSize))
            fileOutput.write(" 0x{0:>08x}  0x{1:>08x}     0x{2:>06x}\n".format(imageSize, limit, limit-imageSize+gapSize))
            if (qshrinkIdentified==True and qsrReduction > 0):
                fileOutput.write("\nImage size reflects {0:d} (0x{0:>08x}) byte reduction with QSHRINK\n".format(qsrReduction))
            fileOutput.close()
        else:
            print "\nERROR: Image Size exceeds limit by",imageSize-limit,"bytes."
            exit (-2)


##############
# Main code. Should not be invoked, but called by imageStats.py
##############
print "Invoked from:",__name__
if __name__ == '__main__':
    print "use imageStats.py to run"
    exit(-1)