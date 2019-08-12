#!/usr/bin/python
# -*- coding: cp1252 -*-
#===============================================================================
#
# qsee_tz_info_extractor.py
#
# GENERAL DESCRIPTION
#    Generic TZ diag region parser [ A,B-Family ]
#
# Copyright (c) 2014 by QUALCOMM Technologies, Incorporated.
# All Rights Reserved.
#
#-------------------------------------------------------------------------------
#
#  $DateTime: 2016/06/17 14:31:11 $
#  $Manager       : Min Guo <mguo@qualcomm.com> $
#  $Product Owner : Park, Young Jae <yjpark@qti.qualcomm.com> $
#  $Author        : Anil Varma Biruduraju <abirudur@qualcomm.com> $
#
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in ascending date order.
#
# when       who       what, where, why
# --------   ---       ---------------------------------------------------------
# 01/22/14   abirudur  Initial Version
# 01/28/14   abirudur  Add backup TZ info (if available)
# 01/29/14   abirudur  Alpha release for review
# 01/30/14   abirudur  Corrected header information
# 01/30/14   abirudur  Added code to get the same GUI look as old tool
# 01/30/14   abirudur  Added template api to generate tables with user defined
#                      header and entries
# 01/30/14   abirudur  Optimized IO read by adding unpack structure api
# 02/02/14   abirudur  Added Table layout
# 02/04/14   abirudur  Beta release for review
# 02/18/14   abirudur  Removed -ddr01,-ocimem and flags parameters 
# 02/18/14   abirudur  Pre-release review
# 02/20/14   abirudur  Removed IMEM/DDR size check
# 02/20/14   abirudur  Input order does not matter !!
# 02/20/14   abirudur  Removed version id from filename
# 02/20/14   abirudur  Validation review
# 02/21/14   abirudur  Removed logic to infer offset from TZ [backup info]
# 02/21/14   abirudur  Final review
# 03/13/14   abirudur  Using offsets 0x5720 for B and 0x3D000 for A family chipset
#                      to find DIAG 
# 06/24/14   abirudur  Added support for 8x16 chipsets
#===============================================================================

import sys
import re
import os
import struct
import datetime
import array
import string
import bisect
import traceback
import platform
import functools
from struct import unpack
from struct import pack
from string import Template

#----------------------------------------------------------------------------
# GLOBAL VARIABLES BEGIN
#----------------------------------------------------------------------------

# Sizes
_256KB                          = 0x40000 # IMEM B-Family [High-end]
_232KB                          = 0x3A000 # IMEM-B
_4KB                            = 0x1000  # IMEM 8x16 
_16KB                           = 0x4000  # IMEM 8x16 
_24KB                           = 0x6000  # IMEM-A or B-Family IMEM [Low-end]
_32KB                           = 0x8000  # IMEM-A or B-Family IMEM [Low-end]
_512MB                          = 0x20000000 # DDR [ min-size 500MB ]

# Validation status
g_validation_error_status      = [(-1,"IMEM file not valid"),
                                  (-2,"IMEM not in range of 24KB -- 256KB !!!"),
                                  (-3,"DDR file not valid"),
                                  (-4,"DDR is too small < 512 MB !!!")]

# Data type sizes 
H_WORD                         = 2
WORD                           = 4
D_WORD                         = 8
BYTE                           = 1

# Fixed value for B-Family chipsets 
TZBSP_DIAG_NUM_OF_VMID         = 16
TZBSP_DIAG_VMID_DESC_LEN       = 7
TZBSP_DIAG_INT_NUM             = 32
TZBSP_MAX_INT_DESC             = 16

# TZ Diag magic number [ Used for force check !!! ]
TZBSP_DIAG_MAGIC_NUM           = 0x747A6461 

# Possible reasons for a reset 
g_reset_reason_map = \
        [("NO_ERROR_FATAL",0x0),
         ("NON_SECURE_WDT",0x1),
         ("SECURE_WDT    ",0x2),
         ("AHB_TIMEOUT   ",0x3),
         ("RPM_WDOG      ",0x4),
         ("RPM_ERR       ",0x5),
         ("NOC_ERROR     ",0x6),
         ("SMEM          ",0x7),
         ("XPU_VIOLATION ",0x8)]

#----------------------------------------------------------------------------
# GLOBAL VARIABLES END
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# CLASS DEFINITIONS BEGIN 
#----------------------------------------------------------------------------
class TZ_BSP_Dump() :       
    def __init__ (self,                  
                  p_ocimem_file_name,
                  p_ebi_cs0_file_name) :
        self.l_ocimem     = p_ocimem_file_name
        self.l_ddr01      = p_ebi_cs0_file_name             
        self.l_log_binary = ''                                        
        self.l_backup_offset_list  = []
        self.ring_buffer_len       = 2048       
        self.l_brute_force  = 0 # Last option 
        self.l_cpu_count = 0
        self.base_offset = 0
        self.tz_base_offset = 0
        self.l_current_stage = 0
        self.addr_to_symbol_dict = {}
        self.symbol_to_addr_dict = {}
        self.diag_offset = 0
        self.tzbsp_diag_struct_general_map = []
        self.tzbsp_diag_struct_vmid_map    = []
        self.tzbsp_diag_struct_boot_map    = []
        self.tzbsp_diag_struct_reset_map   = []
        self.tzbsp_diag_struct_int_map     = []
        self.preprocess_meta_info()
        self.tz_base_offset = self.getOffSet()
        self.error_state = 0
        
    #-------------------------------------------------------------------------------
    # Preprocessing meta info to use the right files for a given TZBSP instance
    #-------------------------------------------------------------------------------
    def preprocess_meta_info(self) :
        if self.l_ocimem != "DEFAULT":            
            self.l_backup_offset_list = [0,0,0] # This version will not make use of
                                                # backup info
        
    #-------------------------------------------------------------------------------
    # Get offset 
    #-------------------------------------------------------------------------------
    def getOffSet(self):
        l_offset = 0;#For now we assume that both input files are useful and scan from
                     # beginning
        try:
            
            self.l_log_binary = "CHECK_BOTH" #No option left but to brute-force                            
            
            return l_offset
        except:            
            return l_offset

    #-------------------------------------------------------------------------------
    # Mapping the data structure that is of interest
    #-------------------------------------------------------------------------------
    def createSymbolMap(self,symbol) :
        if symbol in "g_tzbsp_diag":             
             self.tzbsp_diag_struct_general_map = \
                     [["Magic Number          ",'I',0], #0
                      ["Version               ",'I',0], #1
                      ["CPU Count             ",'I',0], #2
                      ["VMID Info Offset      ",'I',0], #3
                      ["Boot Info Offset      ",'I',0], #4
                      ["Reset Info Offset     ",'I',0], #5
                      ["Interrupt Info Offset ",'I',0], #6
                      ["Ring Buffer Offset    ",'I',0], #7
                      ["Ring Buffer Length    ",'I',0]] #8
                     
             self.tzbsp_diag_struct_vmid_map    = \
                     [["VMID",'B'],                                  #0
                      ["DESC","S-" + str(TZBSP_DIAG_VMID_DESC_LEN)]] #1
                     
             self.tzbsp_diag_struct_boot_map    = \
                     [("Warm Entry",'I'),    #0
                      ("Warm Exit",'I'),     #1
                      ("PC Entry",'I'),      #2
                      ("PC Exit",'I'),       #3                      
                      ("WarmJump Addr",'I'), #4
                      ("WarmJump Inst",'I')] #5
                     
             self.tzbsp_diag_struct_reset_map   = \
                     [("Reset Type  ",'I'), #0
                      ("Reset Count",'I')]  #1
                     
             self.tzbsp_diag_struct_int_map     = \
                     [["Intr Info",'H'], #0
                      ["Avail",'B'],     #1
                      ["Spare",'B'],     #2
                      ["Intr Type",'I'], #3
                      ["Description","S-" + str(TZBSP_MAX_INT_DESC)], #4
                      ["Intr Cnts",'Q']] #5

    #-------------------------------------------------------------------------------
    # Process/Decode the diag region
    #-------------------------------------------------------------------------------
    def processDiagData(self,offset) :        
        try:            
            self.base_offset = offset                 
            bin_file = open(self.l_log_binary, "rb")
            l_file_size = os.path.getsize(self.l_log_binary)
            
            if self.l_brute_force  != 1:            
                if l_file_size == _232KB:
                   offset = offset - _24KB               

                if l_file_size == _24KB:
                   display_side_banner("2nd argument is not valid",6,"=")
                   self.error_state += 1
                   display_help()
                   
                if (self.diag_offset > 0x0D000000 and \
                    self.diag_offset < 0x0FF00000) and \
                    l_file_size < _256KB:
                   display_side_banner("2nd argument should be DDR",6,"=")
                   self.error_state += 1
                   display_help()

                if l_file_size > _256KB and (offset < 0xFFFFF):
                   display_side_banner("2nd argument should be SMEM",6,"=")
                   self.error_state += 1
                   display_help()            
                    
            bin_file.seek(offset)
            l_loop_index = 0

            # Get the TZ Diag header 
            l_unpack_info = build_unpack_structure_string(self.tzbsp_diag_struct_general_map,1,1)      
            
            raw_data = bin_file.read(l_unpack_info[0])
            raw_data = unpack(l_unpack_info[1],raw_data)
            if len(raw_data) == 0:                
                raise          
            
            fill_unpacked_data(self.tzbsp_diag_struct_general_map,raw_data,2)
            # Place holder to get the No. of CPU count used for banner display
            self.tzbsp_diag_struct_int_map[5][1] = "Q-" + \
                                                   str(self.tzbsp_diag_struct_general_map[2][2])
            
            if  self.tzbsp_diag_struct_general_map[0][2] != TZBSP_DIAG_MAGIC_NUM:
                l_error_state = 1
                raise
            else:
                display_side_banner("TZ BSP DIAG INFO BEGIN",1,"=")
                display_side_banner("GENERAL BASE INFO",15,"=")

            for general_info_entry in self.tzbsp_diag_struct_general_map:
                log(" %-14s -> [0x%08x]" %(general_info_entry[0],
                                           general_info_entry[2]))

            
            l_unpack_info = build_unpack_structure_string(self.tzbsp_diag_struct_vmid_map,
                                                          1,
                                                          TZBSP_DIAG_NUM_OF_VMID)

            raw_data = bin_file.read(l_unpack_info[0])
            raw_data = unpack(l_unpack_info[1],raw_data)
            if len(raw_data) == 0:                
                raise
            
            log("\n")
            
            display_inline_banner("VMID MAP",5,"-")            
            display_line(27,"-","+")
            log(" + %-5s | %-5s | %-7s +" %("INDEX",
                                            self.tzbsp_diag_struct_vmid_map[0][0],
                                            self.tzbsp_diag_struct_vmid_map[1][0]))
            display_line(27,"-","+")
            
            for index in range(0,len(raw_data),2):
                l_string = str(raw_data[index + 1]).decode('ascii',errors='ignore')
                match = re.match(r'(\S*)',l_string)
                if match:                   
                    l_string = match.group(1)
                log(" + %-5d | 0x%-3x | %-7s +" %(l_loop_index,
                                                raw_data[index],
                                                l_string))
                l_loop_index += 1

            display_line(27,"-","+")
            log("\n")

            l_unpack_info = build_unpack_structure_string(self.tzbsp_diag_struct_boot_map,
                                                          1,
                                                          self.tzbsp_diag_struct_general_map[2][2])

            raw_data = bin_file.read(l_unpack_info[0])
            raw_data = unpack(l_unpack_info[1],raw_data)
            if len(raw_data) == 0:                
                raise
            
            display_inline_banner("BOOT STATUS",40,"-")
            display_line(100,"-","+")
            log(" + %-8s | %-8s | %-10s | %-10s | %-10s | %-15s | %-15s +" %("CPU",
                                            self.tzbsp_diag_struct_boot_map[0][0],
                                            self.tzbsp_diag_struct_boot_map[1][0],
                                            self.tzbsp_diag_struct_boot_map[2][0],
                                            self.tzbsp_diag_struct_boot_map[3][0],
                                            self.tzbsp_diag_struct_boot_map[4][0],
                                            self.tzbsp_diag_struct_boot_map[5][0]))
            display_line(100,"-","+")
            l_loop_index = 0
            
            for index in range(0,len(raw_data),6):
                log(" + %-8d | 0x%08x | 0x%08x | 0x%08x | 0x%08x | 0x%-13x | 0x%-13x +"
                    %(l_loop_index,
                      raw_data[index],
                      raw_data[index + 1],
                      raw_data[index + 2],
                      raw_data[index + 3],
                      raw_data[index + 4],
                      raw_data[index + 5]))
                l_loop_index += 1

            display_line(100,"-","+")
            log("\n")

            l_unpack_info = build_unpack_structure_string(self.tzbsp_diag_struct_reset_map,
                                                          1,
                                                          self.tzbsp_diag_struct_general_map[2][2])

            raw_data = bin_file.read(l_unpack_info[0])
            raw_data = unpack(l_unpack_info[1],raw_data)
            if len(raw_data) == 0:                
                raise
            
            display_inline_banner("RESET INFO",15,"-")
            display_line(50,"-","+")
            log(" + %-8s | %-20s | %-12s +" %("CPU",
                                            self.tzbsp_diag_struct_reset_map[0][0],
                                            self.tzbsp_diag_struct_reset_map[1][0]))
                                            
            display_line(50,"-","+")
            l_loop_index = 0
            
            for index in range(0,len(raw_data),2):
                log(" + %-8d | (%02d)[%-14s] | 0x%08x   +"
                    %(l_loop_index,
                      raw_data[index],
                      str(g_reset_reason_map[raw_data[index]][0]),
                      raw_data[index + 1]))
                l_loop_index += 1

            display_line(50,"-","+")
            log("\n")

            raw_data = bin_file.read(WORD)
            raw_data = unpack("<I",raw_data)
            if len(raw_data) == 0:                
                raise
            
            l_unpack_info = build_unpack_structure_string(self.tzbsp_diag_struct_int_map,
                                                          1,
                                                          raw_data[0])            
            raw_data = bin_file.read(l_unpack_info[0])
            raw_data = unpack(l_unpack_info[1],raw_data)
            if len(raw_data) == 0:                
                raise
            
            display_inline_banner("INTERRUPT INFO",59,"-")
            display_line(145,"-","+")
            if self.tzbsp_diag_struct_general_map[2][2] == 4:
                log(" + %-6s | %-10s | %-10s | %-10s | %-10s | %-15s  "\
                    "| IntrCnts => %-10s | %-10s | %-10s | %-10s +"
                    %("INDEX",
                      self.tzbsp_diag_struct_int_map[0][0],
                      self.tzbsp_diag_struct_int_map[1][0],
                      self.tzbsp_diag_struct_int_map[2][0],
                      self.tzbsp_diag_struct_int_map[3][0],
                      self.tzbsp_diag_struct_int_map[4][0],
                      "CPU0",
                      "CPU1",
                      "CPU2",
                      "CPU3",))
                
            elif self.tzbsp_diag_struct_general_map[2][2] == 2:
                log(" + %-6s | %-10s | %-10s | %-10s | %-10s | %-15s  "\
                    "| IntrCnts => %-10s | %-10s | %-10s | %-10s +"
                    %("INDEX",
                      self.tzbsp_diag_struct_int_map[0][0],
                      self.tzbsp_diag_struct_int_map[1][0],
                      self.tzbsp_diag_struct_int_map[2][0],
                      self.tzbsp_diag_struct_int_map[3][0],
                      self.tzbsp_diag_struct_int_map[4][0],
                      "CPU0",
                      "CPU1",
                      "",
                      "",))
            
            
            display_line(145,"-","+")
            l_loop_index = 0            

            if self.tzbsp_diag_struct_general_map[2][2] == 4:
                for index in range(0,len(raw_data),9):
                    l_string = str(raw_data[index + 4]).decode('ascii',errors='ignore')
                    l_entry  = ''
                    
                    for char in l_string:
                        if ord(char) < 127:
                            match = re.match(r'(\w)',char)
                            if match or char == ' ':
                                l_entry += char
                        else:
                            break
                    
                    log(" + %-6d | 0x%08x | 0x%08x | 0x%08x | 0x%08x | %-16s "\
                        "| IntrCnts => 0x%08x | 0x%08x | 0x%08x | 0x%08x +"
                        %(l_loop_index,
                          raw_data[index],
                          raw_data[index + 1],
                          raw_data[index + 2],
                          raw_data[index + 3],
                          l_entry,
                          raw_data[index + 5],
                          raw_data[index + 6],
                          raw_data[index + 7],
                          raw_data[index + 8]))
                    l_loop_index += 1
                    
            elif self.tzbsp_diag_struct_general_map[2][2] == 2:
                for index in range(0,len(raw_data),7):
                    l_string = str(raw_data[index + 4]).decode('ascii',errors='ignore')
                    l_entry  = ''
                    
                    for char in l_string:
                        if ord(char) < 127:
                            match = re.match(r'(\w)',char)
                            if match or char == ' ':
                                l_entry += char
                        else:
                            break
                    
                    log(" + %-6d | 0x%08x | 0x%08x | 0x%08x | 0x%08x | %-16s "\
                        "| IntrCnts => 0x%08x | 0x%08x | %-10s | %-10s +"
                        %(l_loop_index,
                          raw_data[index],
                          raw_data[index + 1],
                          raw_data[index + 2],
                          raw_data[index + 3],
                          l_entry,
                          raw_data[index + 5],
                          raw_data[index + 6],
                          "",
                          ""))
                    l_loop_index += 1

            display_line(145,"-","+")
            log("\n")

            display_inline_banner("TZBSP LOG [ RING BUFFER ]",20,"-")                              
            byteRead = (bin_file.read(self.ring_buffer_len)).decode('ascii',errors='ignore')            
            byteRead = byteRead.replace("\r\n","\n") # Line De-limiter 
            offset+=self.ring_buffer_len
            if len(byteRead) ==  0:
                tzbsp_enable_log_instructions()
            else:
                log(byteRead)
            display_line(80,"-","+")           
                
        except:
            
            # This case is when the offset info is not valid
            if self.l_brute_force == 0 and self.error_state != 1:
                self.l_brute_force = 1
                self.l_log_binary = "CHECK_BOTH"            
                l_offset = self.forceCheckTZDiagMG()               
                if l_offset == 0:                   
                    self.l_log_binary = self.l_ddr01                
                    l_offset = self.forceCheckTZDiagMG()
                
                tzbsp_handle.processDiagData(l_offset)
            else:
                # Tried force check [ Need proper dump to proceed !!! Giving up]
                print " -------------------------------------------------------------------"
                print " Please check if the provided IMEM/DDR are valid                    "
                print " -------------------------------------------------------------------"            
                sys.exit(0)
                
    #-------------------------------------------------------------------------------
    # Force check for diag region // in absence of elf
    #-------------------------------------------------------------------------------
    def forceCheckTZDiagMG(self) :
        l_offset       = 0
        l_file_size    = 0
        l_limit_search = 0 # Limit search
        l_status       = 0       

        if self.l_log_binary != "CHECK_BOTH" and self.l_brute_force == 1:
            l_file_name = os.path.basename(self.l_log_binary)
            l_file_size = os.path.getsize(self.l_log_binary)
            bin_file = open(self.l_log_binary, "rb")
             
            if( l_file_size > 0x50000):
                l_limit_search = l_file_size
                self.tz_base_offset = 0x06000000
            else:
                l_limit_search = l_file_size
                self.tz_base_offset = 0

            l_offset = self.tz_base_offset
            log(" SCANNING - [%s] " %(l_file_name))            
            while l_offset < (self.tz_base_offset + l_limit_search):              
               bin_file.seek(l_offset)
               byteRead = bin_file.read(WORD)           
               if len(byteRead) == 0:               
                  l_offset = 0
                  break
               value = unpack('<L',byteRead)
               if value[0] == TZBSP_DIAG_MAGIC_NUM:
                  l_status = 1
                  break             
               l_offset+=_4KB # Search 4K aligned addresses only [Time Saver]               

            bin_file.close()
            if l_status != 1:
               l_offset = 0 # reset offset if not found
            return l_offset 
            
        elif self.l_log_binary != "CHECK_BOTH" and self.l_brute_force == 0:
                l_file_name = os.path.basename(self.l_log_binary)
                l_file_size = os.path.getsize(self.l_log_binary)
                bin_file = open(self.l_log_binary, "rb")
                if self.l_log_binary == self.l_ocimem:                    
                    self.tz_base_offset = [0x5720,
                                           0x720]
                    l_offset_list = self.tz_base_offset
                
                    #log(" SCANNING - [%s] " %(l_file_name))
                    for l_offset_entry in l_offset_list:
                       if l_file_size >= l_offset_entry:
                           bin_file.seek(l_offset_entry)
                           byteRead = bin_file.read(WORD)           
                           if len(byteRead) == 0:                      
                              l_offset = 0
                              break
                           value = unpack('<L',byteRead)
                           
                           self.l_log_binary = self.l_ocimem

                           if (value[0] & 0xfe000000) == 0xfe000000:   #B                               
                               self.diag_offset = value[0]
                               l_offset = value[0] & 0xFFFFF
                               break
                           elif (value[0] & 0xff000000) == 0x0f000000: #B                               
                                self.diag_offset = value[0]
                                l_offset = value[0]
                                break
                           elif (value[0] & 0xfd000000) == 0x0d000000: #B                                
                                self.diag_offset = value[0]
                                l_offset = value[0]
                                break
                           elif (value[0] & 0x2a000000) == 0x2a000000: #A                                
                                self.diag_offset = value[0]
                                l_offset = value[0] & 0xFFFFF
                                break
                           elif (value[0] & 0x86000000) == 0x86000000: #8x16                                 
                                self.diag_offset = value[0]
                                l_offset = value[0] & 0xFFFFFFF
                                break
                            
                # If the above prediction goes wrong we do brute force scan
                # at later stage
                if l_file_size == _24KB:
                   self.l_log_binary = self.l_ddr01 
                                 
                if l_file_size == _32KB:
                   self.l_log_binary = self.l_ddr01 
                
                if l_file_size == _16KB:
                   self.l_log_binary = self.l_ddr01 

                bin_file.close()
                return l_offset
        else:
            if self.l_brute_force == 0:
                self.l_log_binary = self.l_ocimem            
                l_offset = self.forceCheckTZDiagMG()            
                
            if l_offset == 0:
                self.l_brute_force  = 1 # Only when no valid information in SMEM
                #log("Fallback to IMEM and DDR scan")
                self.l_log_binary = self.l_ocimem            
                l_offset = self.forceCheckTZDiagMG()               
                if l_offset == 0:                   
                    self.l_log_binary = self.l_ddr01                
                    l_offset = self.forceCheckTZDiagMG()                        
          
        return l_offset # could not find magic number [ memory dump feature disabled ]
    
#-------------------------------------------------------------------------------
# CLASS DEFINITIONS END 
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# HELPER FUNCTIONS 
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Logging to io streams 
#-------------------------------------------------------------------------------
def log(p_log):
    try:
        print(p_log)
    except:
        print("Cannot log to output stream")

#-------------------------------------------------------------------------------
# Convert to string 
#-------------------------------------------------------------------------------
def get_string(p_value):
    try:
        return str(p_value)
    except:
        log("Cannot convert to string")

#-------------------------------------------------------------------------------
# Convert to hex 
#-------------------------------------------------------------------------------
def get_hex(p_value):
    try:
        return hex(p_value)
    except:
        log("Cannot convert to hex")

#-------------------------------------------------------------------------------
# Display line
#-------------------------------------------------------------------------------
def display_line(p_len,p_decorator,p_limiter):
    l_line_str = ""

    if p_limiter != " ":
        p_len -= 2
                
    for index in range(0,p_len):
        l_line_str += p_decorator

    if p_limiter != " ":
        l_line_str = " " + p_limiter + l_line_str + p_limiter
    else:
        l_line_str = " " + l_line_str

    log(l_line_str)

#-------------------------------------------------------------------------------
# Display inline banner
#-------------------------------------------------------------------------------
def display_inline_banner(p_inline_banner,p_extra,p_decorator):
    l_banner_str = " "
    
    for index in range(0,len(p_inline_banner)/2 + p_extra):
        l_banner_str  += p_decorator
        
    l_banner_str += p_inline_banner
    
    for index in range(0,len(p_inline_banner)/2 + p_extra):
        l_banner_str  += p_decorator
        
    log(l_banner_str)
    return len(l_banner_str)

#-------------------------------------------------------------------------------
# Display banner line
#-------------------------------------------------------------------------------
def display_banner_line(p_side_banner,p_repeat,p_extra,p_decorator):
    l_banner_str = " "
    
    for index in range(0,len(p_side_banner)+len(" [  ] ")+p_extra):
        l_banner_str  += p_decorator
    for index in range(0,p_repeat):
        log(l_banner_str)
        
    return len(l_banner_str)

#-------------------------------------------------------------------------------
# Display side banner 
#-------------------------------------------------------------------------------
def display_side_banner(p_side_banner,p_extra,p_decorator):
    l_display_banner = ' '
    l_align_center = display_banner_line(p_side_banner,1,p_extra,p_decorator)
    l_align_center = l_align_center/2 - len(p_side_banner)/2 - 1       
    for index in range(0,l_align_center):
        l_display_banner += ' '

    l_display_banner += p_side_banner    
    log((" %s " %(l_display_banner)))
    display_banner_line(p_side_banner,1,p_extra,p_decorator)


#-------------------------------------------------------------------------------
# Build unpack string to get bytes from file with specified data types 
#-------------------------------------------------------------------------------
def build_unpack_structure_string(p_struct,p_type_pos,p_multiplier):
    l_little_end = "<" # Used to specify the target as little endian
    l_unpack_str = "" 
    l_struct_len = 0
    for l_var in p_struct:
        if l_var[p_type_pos] == 'I': #4
            l_unpack_str += 'I'
            l_struct_len += WORD           
        elif l_var[p_type_pos] == 'B': #1
            l_unpack_str += 'B'
            l_struct_len += BYTE
        elif l_var[p_type_pos] == 'H': #2
            l_unpack_str += 'H'
            l_struct_len += H_WORD
        elif l_var[p_type_pos] == 'Q': #8
            l_unpack_str += 'Q'
            l_struct_len += D_WORD
        elif 'Q' in l_var[p_type_pos]: #8(*)
            match = re.match(r'Q-(\d*)',l_var[p_type_pos])
            if match:
                byte_count = int(match.group(1)) * D_WORD
                l_struct_len += byte_count
                for index in range(0,int(match.group(1))):
                    l_unpack_str += 'Q'
            else:
                sys.exit(0)
        elif 'S' in l_var[p_type_pos]: #1(*)            
            match = re.match(r'S-(\d*)',l_var[p_type_pos])
            if match:
                byte_count = match.group(1)
                l_struct_len += int(byte_count)                
                l_unpack_str += str(byte_count) + 's'
            else:
                sys.exit(0)
        else:             
             sys.exit(0)

    if p_multiplier > 1:
        l_tmp_unpack_repeater = l_unpack_str
        for multipler in range(0,p_multiplier-1):  
            l_unpack_str += l_tmp_unpack_repeater
            
        l_struct_len  = l_struct_len * p_multiplier

    l_unpack_str = l_little_end  + l_unpack_str 
    return [l_struct_len,l_unpack_str]       
            
#-------------------------------------------------------------------------------
# Display instructions to enable TZ logs
#-------------------------------------------------------------------------------
def tzbsp_enable_log_instructions():
    print(' Please changes to the following files to enable TZ logs :: \n'
          ' 1)Filename :: tzbsp_oem_log.c                              \n'
          '   Function :: tzbsp_oem_allow_logging                      \n'
          '   Change   :: RETURN TRUE by default.                    \n\n'
          ' 2)Filename :: qsee_log.h                                   \n'
          '   Change   :: Enable logging for all levels                \n'
          '               #define ENABLE_QSEE_LOG_MSG_LOW   1          \n'
          '               #define ENABLE_QSEE_LOG_MSG_MED   1          \n'
          '               #define ENABLE_QSEE_LOG_MSG_HIGH  1          \n'
          '               #define ENABLE_QSEE_LOG_MSG_ERROR 1          \n'
          '               #define ENABLE_QSEE_LOG_MSG_FATAL 1          \n'
          '               #define ENABLE_QSEE_LOG_MSG_DEBUG 1          \n'
          '               #define TZ_LOG_ON                            \n' 
          '               #define ENABLE_TZ_LOG_MSG_LOW     1          \n'
          '               #define ENABLE_TZ_LOG_MSG_MED     1          \n'
          '               #define ENABLE_TZ_LOG_MSG_HIGH    1          \n' 
          '               #define ENABLE_TZ_LOG_MSG_ERROR   1          \n' 
          '               #define ENABLE_TZ_LOG_MSG_FATAL   1          \n' 
          '               #define ENABLE_TZ_LOG_MSG_DEBUG   1          \n'
          ' 3)Rebuild TZ and flash binary and perform your test case   \n'
          ' 4)Get the tzbsp/qsee logs as follows                       \n'
          '   >adb root                                                \n'
          '   >adb shell                                               \n'
          '   >mount –t debugfs none /d                                \n'
          '   >cat /sys/kernel/debug/tzdbg/log                         \n'
          '   >cat /sys/kernel/debug/tzdbg/qsee_log                    \n')

#-------------------------------------------------------------------------------
# Display instructions to enable memory dump
#-------------------------------------------------------------------------------
def tzbsp_enable_memory_dump():
    print(' Please do the following changes :-                          \n'
          ' [Boot image]                                                \n'
          ' 1)Filename :- boot_dload_debug.c                            \n'
          '   Function :- boot_dload_is_tz_dump_allowed                 \n'
          '   Change   :- return TRUE                                   \n' 
          ' [TZ Image]                                                  \n'
          ' 2)Filename :- tzbsp_dload_mode.c                            \n'
          '   Functions:- tzbsp_allow_memory_dump                       \n'
          '               tzbsp_security_allows_mem_dump                \n'
          '   Change   :- return TRUE                                   \n'
          ' 3)Rebuild TZ/sbl1 and perform your test case                \n')

#-------------------------------------------------------------------------------
# Display script usable arguments
#-------------------------------------------------------------------------------
def display_help():
   print " ---------------------------------------------------------------------"
   print(' Usage: python tz_diag_parser.py <options> [args]                  \n'      
         '        *[arg]   --- SMEM                                          \n'
         '        *[arg]   --- IMEM/DDR                                      \n'       
         '      <--help>   --- Displays tool usage commands/options          \n'
         ' NOTE: <options> with a * are mandatory                            \n'
         ' Supported chipsets::  [ A,B Family ]                              \n'         
         ' Example:-                                                         \n'
         ' --> tz_diag_parser.py imem.bin ddr01.bin                          \n')         
   print " --------------------------------------------------------------------"
   sys.exit()


#-------------------------------------------------------------------------------
# Fill unpacked structure at the specified offset of the list passed in
#-------------------------------------------------------------------------------
def fill_unpacked_data(p_container,p_unpacked_data,p_index):    
    l_unpack_index = 0
    for entry in p_container:        
        entry[p_index] = p_unpacked_data[l_unpack_index]
        l_unpack_index += 1
        
#-------------------------------------------------------------------------------
# Check mandatory fields // Need this to finalize which processing file to use
#-------------------------------------------------------------------------------
def check_mandatory_fields(p_ocimem_file_name,p_ebi_cs0_file_name):
    
    
    l_check_status = 0
    if p_ocimem_file_name == "DEFAULT":
        display_side_banner("Missing  SMEM file !",6,"=")           
        l_check_status = 1

    l_ocimem_file_size  = os.path.getsize(p_ocimem_file_name)    
    
    if p_ebi_cs0_file_name == "DEFAULT" and l_ocimem_file_size == _24KB:
        display_side_banner("Missing  IMEM/DDR file !",6,"=")
        l_check_status = 1

    if p_ebi_cs0_file_name == "DEFAULT" and l_ocimem_file_size <= _256KB \
                                        and l_ocimem_file_size >= _232KB:
        p_ebi_cs0_file_name = p_ocimem_file_name
        
    if( l_ocimem_file_size > _256KB):
        display_side_banner("SMEM is too big!",6,"=")           
        l_check_status = 1   

    if l_check_status == 0:
        return [p_ocimem_file_name,p_ebi_cs0_file_name]
    else:
        return []
  
#-------------------------------------------------------------------------------
# Check command line length
#-------------------------------------------------------------------------------
def check_cmd_len_in_range(count,next_index):
    if next_index <= count:        
        return True
    else:        
        return False

#-------------------------------------------------------------------------------
# Get next index of the cmd arguments
#-------------------------------------------------------------------------------
def get_next_index(count,cur_index):
    cur_index = cur_index + 1
    if check_cmd_len_in_range(count,cur_index) is True:        
        return cur_index
    else:       
        return -1       

#-------------------------------------------------------------------------------
# Validate the file size if in range [ Limits inclusive ]
#-------------------------------------------------------------------------------
def check_file_size_above_range(p_file_name,p_lower_limit,p_upper_limit,p_type):
    l_file_size = os.path.getsize(p_file_name)
    l_status = False;

    if p_type == 1: # Ignore upper limit
        if l_file_size >= p_lower_limit:
            l_status = True
    elif p_type == 0:
        if l_file_size >= p_lower_limit and l_file_size <= p_upper_limit:
            l_status = True
    else:
        l_status = False

    return l_status
       
#-------------------------------------------------------------------------------
# Preprocessing and error handling of command line arguments
#-------------------------------------------------------------------------------
def validate_input():   
   l_ocimem_file_name  = "DEFAULT"
   l_ebi_cs0_file_name = "DEFAULT"      
   
   arguments = sys.argv[1:]
   arg_count = len(arguments)

   if arg_count == 0:
      display_side_banner("Reason :: Please enter an option to process",6,"-")
      display_help()      
   
   # Command line book keeping
   l_cur_index = 0
   l_tmp_index = 0
   l_validation_status = 0
   
   
   l_cur_index = get_next_index(arg_count,l_cur_index) 
   
   while l_cur_index <= arg_count and l_cur_index != -1 and l_validation_status >= 0:

      if '--help' in sys.argv[l_cur_index]:
         display_help()
         
      if l_cur_index == 1: # Added hard code check to simplify argument length        
         
         # OCIMEM filename            
         l_ocimem_file_name = sys.argv[l_cur_index]

         # OCIMEM path/file not valid
         if not os.path.exists(l_ocimem_file_name):
            l_error_str = "\n PATH[ " + l_ocimem_file_name + " ]\n" \
                                " [FILE DOES NOT EXISTS] !!! "
            log(l_error_str)

            l_ocimem_file_name = "NO_FILE"
            l_validation_status = -1
            
            l_cur_index = get_next_index(arg_count,l_cur_index)
            
            if l_cur_index == -1:                
                break
            else:
                # Lets continue to see if we have any argument that might be useful
                continue
         else:             
             #OK .... got the right IMEM
             l_cur_index = get_next_index(arg_count,l_cur_index)          
             continue

      elif l_cur_index == 2: # Added hard code check to simplify argument length
          
         # DDR CS0 filename            
         l_ebi_cs0_file_name = sys.argv[l_cur_index]

         # DDR CS0 path/file not valid
         if not os.path.exists(l_ebi_cs0_file_name):
            l_error_str = "\n PATH[ " + l_ebi_cs0_file_name + " ]\n" \
                                " [FILE DOES NOT EXISTS] !!! "
            log(l_error_str)

            l_ebi_cs0_file_name = "NO_FILE"
            
            l_validation_status = -3
           
            l_cur_index = get_next_index(arg_count,l_cur_index)

            # Lets continue to see if we have any argument that might be useful
            if l_cur_index == -1:                
                break
            else:
                # Lets continue to see if we have any argument that might be useful
                continue
         else:
             # OK .... got the DDR CS0
             l_cur_index = get_next_index(arg_count,l_cur_index)
             continue      
      elif '--help' in sys.argv[l_cur_index]:
         display_help()
      else:
         print ' Syntax error at :: ' + sys.argv[l_cur_index]
         display_help()

   if l_validation_status < 0:       
       l_decode_error =  -(l_validation_status) - 1
       l_error_string =  g_validation_error_status[l_decode_error][1]               
       display_help()

   l_file_list = check_mandatory_fields(l_ocimem_file_name,l_ebi_cs0_file_name)
   
   if len(l_file_list) is 0:
      # We cannot proceed forward without mandatory data
      display_side_banner("Reason :: Please enter mandatory fields",1,"-")      
      display_help()

   return l_file_list


#-------------------------------------------------------------------------------
# Preprocessing and error handling of command line arguments
#-------------------------------------------------------------------------------
def perform_force_check(p_tzbsp_handle):            
    
    result = p_tzbsp_handle.forceCheckTZDiagMG()
    if result == 0:
        display_side_banner("No TZ BSP Diag Info Present !!",6,"-")        
        tzbsp_enable_memory_dump();
        sys.exit()
    else:
        return result

#----------------------------------------------------------------------------
# MAIN SCRIPT BEGIN
#----------------------------------------------------------------------------

# Validate Input and return binary list
l_binary_list = validate_input()

# Initialization pass in the meta info
tzbsp_handle = TZ_BSP_Dump(l_binary_list[0], # smem/imem
                           l_binary_list[1]) # imem/ddr0                                                     

# Get offset of the tz_bsp_diag log
tzbsp_diag_offset = perform_force_check(tzbsp_handle)

# Create data structures to map the diag info
tzbsp_handle.createSymbolMap("g_tzbsp_diag")

# Start processing diag data
tzbsp_handle.processDiagData(tzbsp_diag_offset)    

#----------------------------------------------------------------------------
# MAIN SCRIPT END 
#----------------------------------------------------------------------------

