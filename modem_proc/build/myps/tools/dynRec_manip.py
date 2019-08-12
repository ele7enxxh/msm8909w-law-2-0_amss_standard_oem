##  @file       dynRec_manip.py.
#   @author     hzhi.
#   @brief      This script provides a generic manipulator for dynamic reclaiming segment.
#   @version    1.0.
#   @todo       Currently has hard-coded constant numbers which needs be optimized.
#===============================================================================
# Copyright (c) 2014 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/build/myps/tools/dynRec_manip.py#1 $
#  $DateTime: 2016/12/13 07:59:25 $
#  $Author: mplcsds1 $
#  $Change: 11985153 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when      who     ver     what, where, why
# --------  ------  ----    ----------------------------------------------------
# 07/17/15  hzhi    2.2     For CR 872615, add post-link detector for wrong RFC code base.
# 03/17/15  hzhi    2.1     Fix CR 806573, postlink bug when feature is disabled but framework mainlined.
# 11/26/14  hzhi    2.0     Add support for dynrec v2 API.
# 09/24/14  skotalwa 1.0    fix for CR729716.Veneer reuse
# 09/04/14  hzhi    1.0     Creation for FR20842 Dynamic Reclaiming feature.
#-------------------------------------------------------------------------------

import sys
import re
import os
import inspect

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

import elfUtils as utils
import elfConstants as const
import elfFileClass as elfFileClass
# }}}

##
# @brief    Read contents from the specified address to specifed size
# @param    elf          The elfFileClass object to read from
# @param    address      The starting point to read from
# @param    size        amount specified to be read (max of 4 bytes)
# @pre      returns what was read in string format or returns a non-zero value
def readDataByAddress(elf, address, size):
 for section in elf.sectionHeaderTable:
  if (address >= section.sh_addr and
  address <= section.sh_addr + section.sh_size):
   offset = address - section.sh_addr
   return utils.convertFromELF(section.contents[offset:offset+size],size)
 return const.RC_ERROR

##
# @brief    Write contents starting at the specified address
# @param    elf          The elfFileClass object to read from
# @param    address      The starting point to read from
# @param    size         size of the contents that are being written (max of 4 bytes)
# @pre      returns 0 on successful write otherwise a non-zero value
def setDataByAddress(elf, address, size, number):
  for section in elf.sectionHeaderTable:
    if (address >= section.sh_addr and
        address <= section.sh_addr + section.sh_size):
    #{
      offset = address - section.sh_addr
      section.contents = "".join([section.contents[:offset],
                                 utils.convertToELF(size, number),
                                 section.contents[offset+size:]])
      return const.RC_SUCCESS
    #}
  return const.RC_ERROR

##
# @brief    get symbols that have the specified expression in their string names
# @param    elf          The elfFileClass object to read from
# @param    expression   string to look for in each symbol string name
# @pre      returns all symbols with matching expression or non-zero value otherwise
def getLinkerVars(elf, expression, symtab, strtab, symbols):
 symbolList = []
 if (symtab != const.RC_ERROR and strtab != const.RC_ERROR):
  for symbol in symbols:
  # Inefficient, but assumed that symbolNameList is relatively small
   if re.match( expression, symbol.st_nameStr, flags=0):
    #print "S: " + symbol.st_nameStr + "  V: " + str(hex(symbol.st_value))
    symbolList.append(symbol)
 else:
  print("Missing .symtab and/or .strtab section")
  return const.RC_ERROR
 return symbolList

def getSectionName(elf, expression):
       SectionNameList = []
       for sh in elf.sectionHeaderTable:
           if re.match(expression, sh.sh_nameStr, flags=0):
               SectionNameList.append(sh.sh_nameStr)
               if(len(SectionNameList)!= 0):
	         return SectionNameList
       return const.RC_ERROR

def main():
#{{{{
  print "Running Dynamic Reclaiming post-linking processing..."
  elf_handle = elfFileClass.elfFile(sys.argv[1])

  Dict = {}
  #NameList = getSectionName(elf_handle, ".dynamicReclaim.*")
  #print(NameList)
  print
  print "-> Searching Dynamic Reclaiming section..."
  sh_src = elf_handle.getSectionByName(".dynamicReclaim")
  if sh_src == const.RC_ERROR:
  #{
    print " "*3 + "-> ! No Dynamic Reclaiming segment found, quit execution."
    elf_handle.writeOutELF(sys.argv[2])
    sys.exit()
  #}
  Dict["old_dynrec_base_address"] = sh_src.sh_addr

  elf_handle.moveupSectionByName(".dynamicReclaim")
  sh_src = elf_handle.getSectionByName(".dynamicReclaim")


  Dict["dynrec_base_address"] = sh_src.sh_addr
  Dict["dynrec_section_size"] = sh_src.sh_size

  Dict["image_vend"] = (sh_src.sh_addr + sh_src.sh_size + 4096) & (~4095)

  #=========================================
  # updated to remove dupllicates from list.
  #=========================================

  elf = elf_handle
  symtab = elf.getSectionByName(".symtab")
  strtab = elf.getSectionByName(".strtab")
  print
  print "-> Getting Dynamic Reclaiming linker symbols..."
  symbols = elfFileClass.Elf32_SymGenerator(symtab, strtab)

  #SymbolList = getLinkerVars(elf, "__dynrec_.*_rfc_.*", symtab, strtab, symbols)
  SymbolList = getLinkerVars(elf, "__dynrec_.*", symtab, strtab, symbols)
  if len(SymbolList) == 0:
  #{
    print " "*3 +"-> ! No dynrec symbols found <==> feature disabled, quit execution."
    elf_handle.updateSymbolValuesByDict(Dict)
    elf_handle.writeOutELF(sys.argv[2])
    sys.exit()
  #}

  dynrecSymbol_text_start_preorder_list = []
  dynrecSymbol_new_client_list = []
  dynrecSymbol_text_start_list = []
  for symbol in SymbolList:
  #}
    if re.match( ".*dynrec_text_start.*rfc_.*", symbol.st_nameStr, flags=0) != None:
    #{
      print " "*3\
        + "-> %-30s:" %"Found RFC symbol"\
        + "\tS:\t%-40s, " %symbol.st_nameStr\
        + "\t\tV:\t%-15s. " %str(hex(symbol.st_value))
      dynrecSymbol_text_start_preorder_list.append(symbol)
    #}
    elif re.match( ".*dynrec_.*_wtr_4905.*", symbol.st_nameStr, flags=0) != None:
    #{
      print " "*3\
        + "-> %-30s:" %"Found new client symbol"\
        + "\tS:\t%-40s, " %symbol.st_nameStr\
        + "\t\tV:\t%-15s. " %str(hex(symbol.st_value))
      dynrecSymbol_new_client_list.append(symbol)
    #}
  #}

  dynrecAddrTableSize = len(dynrecSymbol_text_start_preorder_list)
  if dynrecAddrTableSize == 0:
  #{
    print " "*3 + "-> ! No dynrec RFC text starts symbols found, skip rest of RFC symbol processing."
    print "-> Starting new client symbol processing..."
  #}
  else:
  #{
    print
    print "-> Continuing with RFC symbol processing..."
    print " "*3 + "-> Sorting RFC symbols based on virtual address..."
    dynrecSymbol_text_start_preorder_list = sorted(dynrecSymbol_text_start_preorder_list, key=lambda x: x.st_value, reverse=False)
    print " "*3 + "-> Sorted text_start RFC linker symbols: "
    for k in range(0, len(dynrecSymbol_text_start_preorder_list)):
    #{
      print " "*6 + "\tS:\t%-40s" %dynrecSymbol_text_start_preorder_list[k].st_nameStr
    #}
    print

    print "-> Getting \'text_end_\', \'data_start_\', and \'data_end_\' RFC symbols..."
    dynrecSymbol_text_end_preorder_list = []
    dynredSymbol_data_start_preorder_list = []
    dynredSymbol_data_end_preorder_list = []
    for j in range(0, dynrecAddrTableSize):
    #{
      symbolTextStart = dynrecSymbol_text_start_preorder_list[j].st_nameStr
      symbolTextEnd = symbolTextStart.replace("_start_", "_end_")
      symbolDataStart = symbolTextStart.replace("_text_", "_data_")
      symbolDataEnd = symbolDataStart.replace("_start_", "_end_")

      for symbol in SymbolList:
      #{
        if symbolTextEnd == symbol.st_nameStr:
        #{
          dynrecSymbol_text_end_preorder_list.append(symbol)
        #}
        elif symbolDataStart == symbol.st_nameStr:
        #{
          dynredSymbol_data_start_preorder_list.append(symbol)
        #}
        elif symbolDataEnd == symbol.st_nameStr:
        #{
          dynredSymbol_data_end_preorder_list.append(symbol)
        #}
        else:
        #{
          continue
        #}
        print " "*3\
          + "-> Found Symbol:"\
          + "\tS:\t%-40s, " %symbol.st_nameStr\
          + "\t\tV:\t%-15s. " %str(hex(symbol.st_value))
      #}
    #}
    print
    print "-> All RFC linker symbols are found, continuing to sort..."

    if len(dynrecSymbol_text_end_preorder_list) != dynrecAddrTableSize or \
      len(dynredSymbol_data_start_preorder_list) != dynrecAddrTableSize or \
      len(dynredSymbol_data_end_preorder_list) != dynrecAddrTableSize:
    #{
      print " "*3 + "-> ! Missing addr symbols for text ends, data starts or data ends, quit execution."
      elf_handle.updateSymbolValuesByDict(Dict)
      elf_handle.writeOutELF(sys.argv[2])
      sys.exit()
    #}

    print " "*3 + "-> Printing pre-order list entries:"
    for x in range(0, dynrecAddrTableSize):
    #{
      print " "*6\
        + "\tS:\t%-40s," %dynrecSymbol_text_start_preorder_list[x].st_nameStr\
        + "\t\tV:\t%-15s." %str(hex(dynrecSymbol_text_start_preorder_list[x].st_value))
      print " "*6\
        + "\tS:\t%-40s," %dynrecSymbol_text_end_preorder_list[x].st_nameStr\
        + "\t\tV:\t%-15s." %str(hex(dynrecSymbol_text_end_preorder_list[x].st_value))
      print " "*6\
        + "\tS:\t%-40s," %dynredSymbol_data_start_preorder_list[x].st_nameStr\
        + "\t\tV:\t%-15s." %str(hex(dynredSymbol_data_start_preorder_list[x].st_value))
      print " "*6\
        + "\tS:\t%-40s," %dynredSymbol_data_end_preorder_list[x].st_nameStr\
        + "\t\tV:\t%-15s." %str(hex(dynredSymbol_data_end_preorder_list[x].st_value))
    #}
    print
    print "-> Done printing pre-order list symbols, continuing to remove symbols of empty RFC modules..."

    # moving dynrecSymbol_text_start_list[] definition out of if branch
    # to avoid post-link error when feature is checked in but disabled.
    #dynrecSymbol_text_start_list = []
    dynrecSymbol_text_end_list = []
    dynredSymbol_data_start_list = []
    dynredSymbol_data_end_list = []

    print " "*3 + "-> Removing empty modules..."
    print " "*6 + "-> Found non-empty RFC modules:"
    for j in range(0, dynrecAddrTableSize):
    #{{
      symbol_va_start_text = dynrecSymbol_text_start_preorder_list[j]
      symbol_va_end_text = dynrecSymbol_text_end_preorder_list[j]
      symbol_va_start_data = dynredSymbol_data_start_preorder_list[j]
      symbol_va_end_data = dynredSymbol_data_end_preorder_list[j]
      if(symbol_va_start_text.st_value < symbol_va_end_text.st_value and
        symbol_va_start_data.st_value < symbol_va_end_data.st_value):
      #{
        print " "*9 + "-> %d" %j
        print " "*12\
          + "\tS:\t%-40s," %symbol_va_start_text.st_nameStr\
          + "\t\tV:\t%-15s." %str(hex(symbol_va_start_text.st_value))
        print " "*12\
          + "\tS:\t%-40s," %symbol_va_end_text.st_nameStr\
          + "\t\tV:\t%-15s." %str(hex(symbol_va_end_text.st_value))
        print " "*12\
          + "\tS:\t%-40s," %symbol_va_start_data.st_nameStr\
          + "\t\tV:\t%-15s." %str(hex(symbol_va_start_data.st_value))
        print " "*12\
          + "\tS:\t%-40s," %symbol_va_end_data.st_nameStr\
          + "\t\tV:\t%-15s." %str(hex(symbol_va_end_data.st_value))
        dynrecSymbol_text_start_list.append(symbol_va_start_text)
        dynrecSymbol_text_end_list.append(symbol_va_end_text)
        dynredSymbol_data_start_list.append(symbol_va_start_data)
        dynredSymbol_data_end_list.append(symbol_va_end_data)
      #}
    #}}

    print " "*3 + "-> Removed empty modules..."
    print
    print "-> Continuing to add linker symbols for new clients..."
  #}

  # start dynrec new client symbol processing.
  print " "*3 + "-> Adding new client symbols to \'dynrec_linker_var_arr[]\'..."
  dynrecSymbol_new_client_list_size = len(dynrecSymbol_new_client_list)
  if dynrecSymbol_new_client_list_size == 0:
  #{
    print " "*6 + "-> ! No dynrec new client symbols found, skip processing."
    print
  #}
  else:
  #{
    for j in range(0, len(dynrecSymbol_new_client_list)):
    #{
      sym_data_start = dynrecSymbol_new_client_list[j].st_nameStr
      if(sym_data_start.find("__dynrec_data_start_") != -1):
      #{
        dynredSymbol_data_start_list.append(dynrecSymbol_new_client_list[j])
        print " "*9\
          + "-> Adding %d:" %j\
          + "\tS:\t%-40s," %sym_data_start\
          + "\t\tV:\t%-15s." %str(hex(dynrecSymbol_new_client_list[j].st_value))
        sym_data_end = sym_data_start.replace("_start_", "_end_")
        sym_text_start = sym_data_start.replace("_data_", "_text_")
        sym_text_end = sym_text_start.replace("_start_", "_end_")
        for symbol in dynrecSymbol_new_client_list:
        #{
          if symbol.st_nameStr == sym_data_end:
          #{
            dynredSymbol_data_end_list.append(symbol)
          #}
          elif symbol.st_nameStr == sym_text_start:
          #{
            dynrecSymbol_text_start_list.append(symbol)
          #}
          elif symbol.st_nameStr == sym_text_end:
          #{
            dynrecSymbol_text_end_list.append(symbol)
          #}
          else:
          #{
            continue
          #}
          print " "*9\
            + "-> Adding %d:" %j\
            + "\tS:\t%-40s," %symbol.st_nameStr\
            + "\t\tV:\t%-15s." %str(hex(symbol.st_value))
        #}
      #}
    #}
    print " "*6 + "-> Added new client modules."
    print
  #}

  print "-> Checking Dynamic Reclaiming symbols collected so far..."

  if(len(dynrecSymbol_text_start_list) == 0):
  #{{
    print " "*3 + "-> ! No module to be dynamic reclaimed, quit execution."
    elf_handle.updateSymbolValuesByDict(Dict)
    elf_handle.writeOutELF(sys.argv[2])
    sys.exit()
  #}}

  if(dynrecSymbol_text_start_list[0].st_nameStr.find("_preload") != -1):
  #{{
    print " "*3 + "-> Processing symbols of first RFC module for veneer issue workaround..."
    dynrecSymbol_text_start_list[0].st_nameStr = dynrecSymbol_text_start_list[0].st_nameStr.rstrip("_preload")
    dynrecSymbol_text_end_list[0].st_nameStr = dynrecSymbol_text_end_list[0].st_nameStr.rstrip("_preload")
    dynredSymbol_data_start_list[0].st_nameStr = dynredSymbol_data_start_list[0].st_nameStr.rstrip("_preload")
    dynredSymbol_data_end_list[0].st_nameStr = dynredSymbol_data_end_list[0].st_nameStr.rstrip("_preload")
    print " "*3 + "-> Done stripping first preloaded card back to original name..."
  #}}
  else:
  #{{
    print " "*3 + "-> Did not find \"_preload\" pattern in first card, maybe it's empty."
  #}}
  print
  #============================================================================================================

  print "-> Continuing to update dynrec_linker_var_arr[] in symbol table..."
  Dict["dynrec_numentries"] = len(dynrecSymbol_text_start_list)
  print " "*3 + "-> Collected %d" %len(dynrecSymbol_text_start_list) + " modules in total."
  elf_handle.updateSymbolValuesByDict(Dict)

  dynrec_linker_var_arr = elf_handle.getSymbolByName("dynrec_linker_var_arr")
  if(dynrec_linker_var_arr == const.RC_ERROR):
  #{
    print " "*3 + "-> ! Cannot find \'dynrec_linker_var_arr[]\', please check source code correctness, quit execution."
    elf_handle.updateSymbolValuesByDict(Dict)
    elf_handle.writeOutELF(sys.argv[2])
    sys.exit()
  #}
  api_called = elf_handle.getSymbolByName("mcfg_dynrec_load")
  if(api_called == const.RC_ERROR):
  #{
    print " "*3 + "-> ! CDynrec feature is enabled but mcfg_dynrec_* API has not been called, please check source code correctness, quit execution."
    elf_handle.updateSymbolValuesByDict(Dict)
    elf_handle.writeOutELF(sys.argv[2])
    sys.exit()
  #}

  data_ptr = dynrec_linker_var_arr.st_value
  val = readDataByAddress(elf_handle, data_ptr + 1, 1)


  print " "*3 + "-> Adding following entries to \'dynrec_linker_var_arr[]\':"
  print
  print " "*6\
    + "%5s: " %"index"\
    + "\t" + "%10s:" %"client_id"\
    + "\t" + "%20s:" %"is_module_loaded"\
    + "\t" + "%20s:" %"str_id[]"\
    + "\t" + "%20s:" %"is_client_reclaimed"\
    + "\t" + "%15s:" %"text_begin"\
    + "\t" + "%15s:" %"text_end"\
    + "\t" + "%15s:" %"data_begin"\
    + "\t" + "%15s:" %"data_end"

  client_dict = {'rfc':          1,
                 'wtr_4905':     2,
                 'rfc_internal': 3
                }

  for i  in range(0,len(dynrecSymbol_text_start_list)):
  #{
    curr_entry = dynrecSymbol_text_start_list[i]
    curr_entry_name = curr_entry.st_nameStr
    curr_entry_addr = curr_entry.st_value
    curr_entry_generic_val = 0
    marker = re.sub('__dynrec_text_start_', '', curr_entry_name, count=1)
    client_id = 0
    for j in client_dict.keys():
    #{
      if marker.find(j) != -1:
      #{
        client_id = client_dict[j]# MCFG_DYNREC_CLIENT_RFC
        marker = marker.rstrip("_")
        break
      #}
    #}
    if client_id > 3 or client_id < 1:
    #{
      print " "*6 + "-> ! Invalid str_id[]: %-15s, skip to next." %marker
      continue
    #}

    if setDataByAddress(elf_handle, data_ptr,1,client_id) == const.RC_ERROR:# data_ptr -> 'client_id'
    #{
      print " "*6 + "-> ! Failed to set symbol: %-40s\t, skip to next." %curr_entry_name
      continue
    #}
    data_ptr = data_ptr + 1# data_ptr -> 'is_module_loaded'
    setDataByAddress(elf_handle, data_ptr,1,0)

    data_ptr = data_ptr + 1# data_ptr -> str_id[]
    str_ptr = data_ptr
    for char in marker:
    #{
      setDataByAddress(elf_handle, str_ptr,1,ord(char))
      str_ptr = str_ptr + 1
    #}

    data_ptr = data_ptr + 50# data_ptr -> 'is_client_reclaimed'
    setDataByAddress(elf_handle, data_ptr,4,0)

    data_ptr = data_ptr + 4# data_ptr -> text_address_begin
    setDataByAddress(elf_handle, data_ptr,4,dynrecSymbol_text_start_list[i].st_value)
    data_ptr = data_ptr + 4 # data_ptr -> text_address_end
    setDataByAddress(elf_handle, data_ptr,4,dynrecSymbol_text_end_list[i].st_value)
    data_ptr = data_ptr + 4 # data_ptr -> rodata_data_address_begin
    setDataByAddress(elf_handle, data_ptr,4,dynredSymbol_data_start_list[i].st_value)
    data_ptr = data_ptr + 4 # data_ptr -> rodata_data_address_end
    setDataByAddress(elf_handle, data_ptr,4,dynredSymbol_data_end_list[i].st_value)
    data_ptr = data_ptr + 4 # data_ptr -> starting address of next entry.

    print " "*6\
      + "%5d:" %i\
      + "\t" + "%10d " %client_id\
      + "\t" + "%20s " %curr_entry_generic_val\
      + "\t" + "%20s " %marker\
      + "\t" + "%20s " %curr_entry_generic_val\
      + "\t" + "%15s " %str(hex(dynrecSymbol_text_start_list[i].st_value))\
      + "\t" + "%15s " %str(hex(dynrecSymbol_text_end_list[i].st_value))\
      + "\t" + "%15s " %str(hex(dynredSymbol_data_start_list[i].st_value))\
      + "\t" + "%15s " %str(hex(dynredSymbol_data_end_list[i].st_value))

  #}
  print
  print "-> Done updating \'dynrec_linker_var_arr[]\', continuing to update ELF file..."
  elf_handle.writeOutELF(sys.argv[2])
  print
  print "-> Done Dynamic Reclaiming post-linking process, finish."
  print
  return 0

#}}}}

if __name__ == '__main__':
#{{{{
  main()
#}}}}
