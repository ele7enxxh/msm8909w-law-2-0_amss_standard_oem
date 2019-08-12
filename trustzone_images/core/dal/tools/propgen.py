#=====================================================================================================
#
# Progen.py
#
# GENERAL DESCRIPTION
#    Generate and Pack Properties expressed in xml in a binary format
#
# Copyright (c) 2011,2012, 2013 by Qualcomm Technologies Incorporated.
# All Rights Reserved.
# Qualcomm Confidential and Proprietary
#
#------------------------------------------------------------------------------------------------------
#
#  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/dal/tools/propgen.py#1 $
#  $DateTime: 2016/06/17 14:31:11 $
#  $Author: pwbldsvc $
#  $Change: 10727476 $
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when       who     what, where, why
# --------   ---     -----------------------------------------------------------------------------------
# 09/25/12   aa      Added Support to parse structure template expressed in xml files

# Notes : 
#[jvedadi]
#  Originally, propgen only accept unsigned int for Driver IDs, but now Strings IDs are supported.
#  
# STRING BASED ID SUPPORT : 
#  All String IDs are isolated in a separate array (driver_list) which contains
#  [String Name, Hash Value, Property Offset, Struct pointer, Size of Collisions, and Collisions' list.]
#  If there were no String IDs, [driver_list] is assigned to NULL.
#  Hash values are used to optimize the search process and avoid String- compare to find property Offset.
#  If a collision accrues while computing Hash values, Collisions' list array maintains collisions indexes.
#  Collision's list is used only if there is a collision and String compare is required in this case.
#  Function hex_to_integer is removed, and built-in function  int  is used instead.
#  offset fixed
#  Check for Supported Char:  
#  Supported_char = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890%()!#$*+,-./:;=?@[]^_`{|}~'
# 
#  Version:1.0
#=====================================================================================================
import xml.dom.minidom
import sys
import re
import os
from struct import *
from types import *

#------------------------------------------------------------------------------
# Globals
#------------------------------------------------------------------------------
global NameSection, StringSection, ByteSection,  SegmentOffset, UintSection, DevSection, StructSection 
global DeviceID
UintSection = []
ByteSection = []
StringSection =[]
NameSection = []
DevSection = []
StructSection = []
DeviceID = []
SegmentOffset = []
#[vkaushik] change for struct props
StructPtrs = []
StructIncFiles = []
PropNames = []
StringDeviceIdx = 0
StringDeviceNames = {}
ConfigStructList = {}
HashCollisionOffset = {}
CollisionDictionary = {}
StructDef = {}
device_offset = 0

#------------------------------------------------------------------------------
# AlignSectionAddr : Make Section word aligned
#------------------------------------------------------------------------------
def AlignSectionAddr():
    NameSectionLen = len(NameSection)
    StringSectionLen = len(StringSection)
    ByteSectionLen = len(ByteSection)

    if NameSectionLen%4 != 0:
        for tmpBytes in range(0,4-(NameSectionLen%4)):
            NameSection.append(0)

    if StringSectionLen%4 != 0:
        for tmpBytes in range(0,4-(StringSectionLen%4)):
            StringSection.append(0)

    if ByteSectionLen%4 != 0:
        for tmpBytes in range(0,4-(ByteSectionLen%4)):
            ByteSection.append(0)
    return

#------------------------------------------------------------------------------
# ComputeCollisionIndex : Return index where collision info for a hash is stored 
#------------------------------------------------------------------------------
def ComputeCollisionIndex(Myhash,device):
   if len(HashCollisionOffset[Myhash])==1:
      return "NULL"
   else:
      CollisionDictionary[device] = HashCollisionOffset[Myhash]
      return device
      #StringDeviceNames[node.getAttribute("id")]= [DriverName,device_offset, hash,StringDeviceIdx,DriverName]

#------------------------------------------------------------------------------
# PrintStringDeviceNameStruct : Populate Meta deta for string based devices
#------------------------------------------------------------------------------
def PrintStringDeviceNameStruct(putnull):
   #{/dev/buses/i2c1, 1234, DALI2C_DriverInfo,  {2,5,8 },
   for device in StringDeviceNames:
                     # ignore null Driver names     
      if putnull ==0 and (StringDeviceNames[device][0]=="NULL" or StringDeviceNames[device][0]=="null"):         
         ConfigStructList[StringDeviceNames[device][3]] = "{0,0,0,0,0,0}"
         continue                  
      ConfigStructList[StringDeviceNames[device][3]] = "{\""+device #String Name
      ConfigStructList[StringDeviceNames[device][3]] +="\","+str(StringDeviceNames[device][2])+"u"# hash
      ConfigStructList[StringDeviceNames[device][3]] +=", "+str(StringDeviceNames[device][1])# offset
      if (putnull==0):
         ConfigStructList[StringDeviceNames[device][3]] +=", &DAL"+StringDeviceNames[device][0]+"_DriverInfo"#Driver
      else:
         ConfigStructList[StringDeviceNames[device][3]] +=", NULL"#Struct Pointer
      # Getting Collision list 
      MyHash = StringDeviceNames[device][2]
      Collisions = ComputeCollisionIndex(MyHash,"Collision_"+str(StringDeviceNames[device][2]))
      if(len(HashCollisionOffset[MyHash])==1):
         ConfigStructList[StringDeviceNames[device][3]] +=", 0"
      else:
         ConfigStructList[StringDeviceNames[device][3]] +=", "+ str(len(HashCollisionOffset[MyHash]))
      ConfigStructList[StringDeviceNames[device][3]] +=", "+Collisions
      ConfigStructList[StringDeviceNames[device][3]] +=" "+"}"

#------------------------------------------------------------------------------
# PrintConfig : Generate DALModConfig and pack all sections
#------------------------------------------------------------------------------      
def PrintConfig(ModName):
   #Header Size is 24 i.e. Total Len (4) + Name Section Len(4) + String Sec Len(4) + Byte Sec Len (4) + Uint Sec Len (4) + Num Devices (4)
   ConfigHeaderLen = 28 + 4*len(DeviceID)
   global UintSection, DevSection
   #UintSection = PackData(UintSection, 3)

   DevSection = PackData(DevSection, 3)
   SegmentOffset = [ConfigHeaderLen + len(NameSection) + len(StringSection) + len(ByteSection) + len(UintSection) + len(StructSection) + len(DevSection) ,
         ConfigHeaderLen,
         ConfigHeaderLen + len(NameSection),
         ConfigHeaderLen + len(NameSection) + len(StringSection),
         ConfigHeaderLen + len(NameSection) + len(StringSection) + len(ByteSection),
         ConfigHeaderLen + len(NameSection) + len(StringSection) + len(ByteSection) + len(UintSection),
         len(DeviceID) / 2]
   for i in range(len(DeviceID)):
      if i%2 == 1:
         DeviceID[i] += SegmentOffset[5] + len(StructSection)
   SegmentOffset.extend(DeviceID)
   SegmentOffset = PackData(SegmentOffset, 3)
   PropBin = SegmentOffset + NameSection + StringSection + ByteSection + UintSection + StructSection + DevSection
   CONFIG = open(DirName + "/" + sys.argv[3], 'a')
   CONFIG.write ("#include \"DALStdDef.h\" \n")
   CONFIG.write ("#include \"DALSysTypes.h\" \n")
        #need to align prop array on 32 bit boundaries [vkaushik]
   CONFIG.write ("#include \"dalconfig.h\" \n\n")

   #define image name
   if ModName == "modem":
      CONFIG.write("#ifndef DAL_CONFIG_IMAGE_MODEM \n")
      CONFIG.write("#define DAL_CONFIG_IMAGE_MODEM \n")
      CONFIG.write("#endif \n")
   elif ModName == "dsp":
      CONFIG.write("#ifndef DAL_CONFIG_IMAGE_DSP \n")
      CONFIG.write("#define DAL_CONFIG_IMAGE_DSP \n")
      CONFIG.write("#endif \n")
   elif ModName == "boot":
      CONFIG.write("#ifndef DAL_CONFIG_IMAGE_BOOT \n")
      CONFIG.write("#define DAL_CONFIG_IMAGE_BOOT \n")
      CONFIG.write("#endif \n")
   elif ModName == "boot_wm6":
      CONFIG.write("#ifndef DAL_CONFIG_IMAGE_BOOT_WM_6 \n")
      CONFIG.write("#define DAL_CONFIG_IMAGE_BOOT_WM_6 \n")
      CONFIG.write("#endif \n")
   elif ModName == "boot_wm7":
      CONFIG.write("#ifndef DAL_CONFIG_IMAGE_BOOT_WM_7 \n")
      CONFIG.write("#define DAL_CONFIG_IMAGE_BOOT_WM_7 \n")
      CONFIG.write("#endif \n")
   elif ModName == "tz":
      CONFIG.write("#ifndef DAL_CONFIG_IMAGE_TZ \n")
      CONFIG.write("#define DAL_CONFIG_IMAGE_TZ \n")
      CONFIG.write("#endif \n")
   elif ModName == "wcn":
      CONFIG.write("#ifndef DAL_CONFIG_IMAGE_WCN \n")
      CONFIG.write("#define DAL_CONFIG_IMAGE_WCN \n")
      CONFIG.write("#endif \n")
   elif ModName == "sps":
      CONFIG.write("#ifndef DAL_CONFIG_IMAGE_SPS \n")
      CONFIG.write("#define DAL_CONFIG_IMAGE_SPS \n")
      CONFIG.write("#endif \n")
   elif ModName == "rpm":
      CONFIG.write("#ifndef DAL_CONFIG_IMAGE_RPM \n")
      CONFIG.write("#define DAL_CONFIG_IMAGE_RPM \n")
      CONFIG.write("#endif \n")
   elif ModName == "uefi":
      CONFIG.write("#ifndef DAL_CONFIG_IMAGE_UEFI \n")
      CONFIG.write("#define DAL_CONFIG_IMAGE_UEFI \n")
      CONFIG.write("#endif \n")
   else:
      CONFIG.write("#ifndef DAL_CONFIG_IMAGE_APPS \n")
      CONFIG.write("#define DAL_CONFIG_IMAGE_APPS \n")
      CONFIG.write("#endif \n")

   #include struct files
   for inc_files in StructIncFiles:
      CONFIG.write ("#include \"%s\"\n" %inc_files)
    
   #struct ptrs
   for struct_ptrs in StructPtrs:
      local_struct =0
      for struct in StructDef:
         name = StructDef[struct][0]
         if name == struct_ptrs :
            local_struct =1
            break
      if local_struct == 1 :
         continue
      CONFIG.write ("extern void * %s;\n" %struct_ptrs)

   #struct
   for index in StructDef:
      name = StructDef[index][0]
      data_type = StructDef[index][1]
      value = StructDef[index][2]
      CONFIG.write ("\n\nstatic " +  data_type + "\t" + name + "=" + value + ";\n")
      
   CONFIG.write ("\nconst void * DALPROP_PropBin[] = {\n")
   for i in range(0, len(PropBin), 4):
      if i%20 == 0:
         CONFIG.write ("\n\t\t\t")
      if type(PropBin[i]) is StringType:
         CONFIG.write ("%s" %PropBin[i])
      else:
         if i+3<len(PropBin):
            CONFIG.write ("(void *)" + "0x%.2x" %PropBin[i+3] + "%.2x" %PropBin[i+2] + "%.2x" %PropBin[i+1] + "%.2x" %PropBin[i])
         elif i+2<len(PropBin):
            CONFIG.write ("(void *)" + "0x%.4x" %PropBin[i+2] + "%.2x" %PropBin[i+1] + "%.2x" %PropBin[i])
         elif i+1<len(PropBin):
            CONFIG.write ("(void *)" + "0x%.6x" %PropBin[i+1] + "%.2x" %PropBin[i])
         else:
            CONFIG.write ("(void *)" + "0x%.8x" %PropBin[i])
      if i != len(PropBin) - 4:
         CONFIG.write (", ")
      else:
         CONFIG.write (" };\n")
    
#driver_entry driver_list[] = {{ /dev/buses/i2c1, 1234, {2,5,8 }, }
   PrintStringDeviceNameStruct(1)
   if len(ConfigStructList)!=0:
      for Col_array in CollisionDictionary:
         CONFIG.write ("\nconst uint32 "+Col_array+" [] = {"+str(CollisionDictionary[Col_array])[1:-1]+"};")
      CONFIG.write("\n\n\nconst StringDevice driver_list[] = ")
      CONFIG.write("{\n\t\t\t")
      for list in range(len(ConfigStructList)):
         CONFIG.write(ConfigStructList[list])
         if(list <len(ConfigStructList)-1):
            CONFIG.write (",\n\t\t\t")
      CONFIG.write ("\n};\n")
      CONFIG.write("\n\nconst DALProps DALPROP_PropsInfo = {(const byte*)DALPROP_PropBin, NULL , " +str(len(ConfigStructList))+", driver_list};\n")
   else:
      CONFIG.write("\n\nconst DALProps DALPROP_PropsInfo = {(const byte*)DALPROP_PropBin, NULL , 0, NULL};\n")

#------------------------------------------------------------------------------
# PackData : Pack data as a byte array
#------------------------------------------------------------------------------      
def PackData(value, type1):
   if type1 == 1: 		#String
      value = list(value)
      for index in range (len(value)):
         value[index] = ord(value[index])
      value.append(0)
   elif type1 == 3:		#UINT32
      value_temp = []
      value=list(value)
      for index in range (len(value)):
         if type(value[index]) is StringType:
            value_temp.append(value[index])
            # Since this is a enum which is 4-bytes long and the PropBin list
            # is handled as 4-bytes at a time, appending 3 0's at the end of
            # the string.
            for j in range(3):
               
                value_temp.append(0)
         elif type(value[index]) is ListType:
            for idx in range(len(value[index])):
               value_temp.append(value[index][idx])
            if len(value[index]) % 4 != 0:
               for idx2 in range(4 - (len(value[index]) % 4)):
                  value_temp.append(0)
         else:
            for j in (unpack('BBBB',pack('I', value[index]))):
               value_temp.append(j)
      value = value_temp
      
   return value

#------------------------------------------------------------------------------
# GenerateGlobalDef : Read & Parse the Global Section for driver and return 
# data in a dictionary(GlobalDef)
#------------------------------------------------------------------------------   
def GenerateGlobalDef(pDriver):
   GlobalDef = {}
   for global_defs in pDriver.getElementsByTagName("global_def"):
      global_defs.normalize()
      for node in global_defs.childNodes:
         if node.nodeType == node.TEXT_NODE:
            continue
         name = node.getAttribute("name")
         type = node.getAttribute("type")
         type = int(type,0)
         value = node.firstChild.data
         if type == 1: #DALPROP_DATA_TYPE_STRING
            value = value.strip()
         elif (type == 2 or type ==3):
            re.sub("\s*", '', value)
            value = value.split(',')
            for index in range (len(value)-1):
               #removing whitespace as this cause eval to fail [vkaushik]
               value[index] = value[index].lstrip()
               value[index] = eval(value[index])
               #value[index] = int(value[index], 16)
            del value[-1]
            value.insert(0, len(value)-1)
         elif (type == 4):
            #insert the include file if not already in our list
            value = value.strip()
            try:
               StructIncFiles.index(value)
            except ValueError:
               StructIncFiles.append(value)
         value = PackData(value, type)
         GlobalDef[name] = value
   return GlobalDef

#------------------------------------------------------------------------------
# Implement DJB2 hash 
#------------------------------------------------------------------------------          
def HashDJB2(key):
   myhash = 5381;
   for c in key:
      myhash = ((myhash << 5) + myhash) + ord(c);
   import ctypes;
   return myhash & 0xFFFFFFFF
   

#------------------------------------------------------------------------------
# Generate the array DevSection based on properties . The last element of the 
# array returned has the len of the array  
#------------------------------------------------------------------------------   
def GenerateArray(DriverName, node, GlobalDef, GlobalOffset):
   Supported_char = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890%()!#$*+,-./:;=?@[]^_`{|}~'  
   global StringDeviceIdx
   global device_offset
   if len(DevSection):
      # poping the last element 
      device_offset += DevSection.pop()
   
   if node.getAttribute("id")[:2] =="0x":
      if DriverName == "NULL" or DriverName == "null":
         raise ValueError("You may not have a NULL Driver and uint32 Device ID")
      try :
         nod_id = int(node.getAttribute("id"),16)
         if nod_id > 0xFFFFFFFF:
            raise ValueError
      except:
         raise ValueError(node.getAttribute("id")+" is invalid uint32 Device Name. Remove \"0x\" if it is a String ID")
      DeviceID.append(nod_id)
      DeviceID.append(device_offset)
   else:
      if len(node.getAttribute("id"))==0:
         print "ERROR *** Empty Device Name"
         raise ValueError("Empty Device Name")
      if StringDeviceNames.has_key(node.getAttribute("id")):
         #Raise error if duplicate device names
         print "ERROR *** Duplicate Device Name '%s'" %node.getAttribute("id")
         raise ValueError("Duplicate Device Name")
      if not all(L in list(Supported_char)  for L in node.getAttribute("id")):
         print "ERROR *** Device Name '%s' has NOT-Supported Characters" %node.getAttribute("id")
         print "Supported Charactors are :"
         print str(list(Supported_char))[1:-1]
         raise ValueError("NOT-Supported Characters Device Name")
      else:
         #device name is string and not uint32 number
         hash = HashDJB2(node.getAttribute("id"))
         StringDeviceNames[node.getAttribute("id")]= [DriverName,device_offset, hash,StringDeviceIdx]
         if HashCollisionOffset.has_key(hash):
            HashCollisionOffset[hash].append(StringDeviceIdx)
         else:
            HashCollisionOffset[hash] = [StringDeviceIdx]
         StringDeviceIdx +=1
   
   #Every property is stored in 2 words(header[type,nameoffset], value)
   valLen = 8* len(node.getElementsByTagName("props"))
   
   for Props in node.getElementsByTagName("props"):
      
      # if the prop is a string then store in namesection
      name = Props.getAttribute("name")
      if(len(name) != 0):
         name = name.encode('ascii', 'ignore')
         # make sure the prop name is unique in the NameSection
         if name not in PropNames:
            PropNames.append(name)
            nameoffset = len(NameSection)
            charname = PackData(name, 1)
            NameSection.extend(charname)
         else:
            nameoffset = 0;
            loc = PropNames.index(name)
            for idx in range(loc):
               nameoffset += len(PropNames[idx]) + 1
      else:
         nameoffset = Props.getAttribute("id")
         if(nameoffset.isdigit() is True):
            nameoffset = eval(nameoffset)
      
      type1 = Props.getAttribute("type").strip()
      #if the prop is primitive type
      if type1[:2] =="0x":
         type1 = int(type1, 16)
      char_data = re.sub("(\s*|\n)", '', Props.firstChild.data)
      
      if type1 == 0x02:   #DALPROP_ATTR_TYPE_UINT32
         try:
            value = eval(char_data)
         except NameError:
            value= str(char_data)
      
      elif type1 == 0x08: # DALPROP_ATTR_TYPE_BYTE_SEQ
         value = char_data.split(',')
         for index in range(len(value) - 1):
            value[index] = value[index].lstrip()
            value[index] = eval(value[index])   
         value.insert(0, len(value) - 1)
         # fix the length and pop the 'end'
         value[0] -= 1
         if "end" == value[len(value) - 1]:
            value.pop()
         while len(value)%4 != 0:
            value.append(0)
         valLen +=len(value)-4
      
      elif type1 == 0x12:   #DALPROP_ATTR_TYPE_STRUCT_PTR
         try:
            StructPtrs.index(char_data)
         except ValueError:
            StructPtrs.append(char_data)
         value = StructPtrs.index(char_data)
         #len_struct = "sizeof(" + str(char_data) + ")" 
         name_addr = "(void *)&" + str(char_data)
      
      # if its a user defined type, doesnot have pre-defined hex type
      elif (re.match( r'\D', str(type1))):           
         data_type = str(type1) 
         type1 = 0x12          # DALPROP_ATTR_TYPE_STRUCT_PTR    
         struct_index = len(StructPtrs)
         if(len(name) !=0):
            array_symbol_index = name.find('[')
            if(array_symbol_index > 0):
               name_split = name.split('[')
               name = name_split[0] + "_" + str(struct_index) + "[]"
               name_addr = "(void *)&" + name_split[0] + "_" + str(struct_index)
            else:
               name = name + "_" + str(struct_index)
               name_addr = "(void *)&" + name
         else:
            name = "devcfg" + str(nameoffset) + "_" + str(struct_index)
            name_addr = "(void *)&" + name
         
         StructPtrs.append(name)
         value = StructPtrs.index(name)
         StructDef.setdefault(value, [name, data_type, char_data])
         #len_struct = "sizeof(" + name + ")" 
         
      else:
         if char_data in GlobalOffset:
            value = GlobalOffset[char_data]
         else:
            if type1 == 0x18:
               value = len(ByteSection)
               ByteSection.extend(GlobalDef[char_data])
            elif type1 == 0x11:
               value = len(StringSection)
               StringSection.extend(GlobalDef[char_data])
            elif type1 == 0x14:
               value = len(UintSection)
               UintSection.extend(GlobalDef[char_data])
         GlobalOffset[char_data] = value
      
      #logic to store type and nameoffset in 32 bit, higher 8 bits for type
      str_nameoffset = str(nameoffset)
      if(str_nameoffset.isdigit() is True) : 
         type1 = type1 & 0x000000FF #discard all higher 28 bits
         nameoffset = nameoffset & 0x00FFFFFF #discard all higher 8 bits
         type_nameoffset = (type1<<24) | nameoffset #123456780000...| 00000000xxxxx.....= 12345678xxxxxxx
         if(type1 == 0x12):
            DevSection.extend([type_nameoffset, name_addr])
         else:
            DevSection.extend([type_nameoffset, value])
      else:
         #str_type = str(type1) + " & 0x000000FF" #discard all higher 28 bits
         type1 = (type1 & 0x000000FF) << 24 #discard all higher 28 bits and store on higher 8 bit in word
         str_nameoffset = str(nameoffset) +  " & 0x00FFFFFF" #discard all higher 8 bits
         type1 = type1 & 0x000000FF #discard all higher 28 bits
         type_nameoffset = "(" + str(type1) + "| (" + str_nameoffset + "))"
         if(type1 == 0x12):
            DevSection.extend([type_nameoffset, name_addr])
         else:
            DevSection.extend([type_nameoffset, value])
        
   DevSection.append(0xFF00FF00)
   # The length of the End marker
   valLen+=4
   DevSection.append(valLen)

#------------------------------------------------------------------------------
# This function will print DALMod file
#------------------------------------------------------------------------------
def PrintModDir(ModName, DriverList):
   ConfigHeaderLen = 28 + 4*len(DeviceID)
   global UintSection, DevSection
   for i in StringDeviceNames:
      StringDeviceNames[i][1] +=ConfigHeaderLen + len(NameSection) + len(StringSection) + len(ByteSection) + len(UintSection) + len(StructSection)
   filename = "DALModDir_" + ModName +".c"
   MODDIR = open(DirName + "/" + filename, 'w')
   MODDIR.write ("#include \"DALStdDef.h\" \n")
   MODDIR.write ("#include \"DALReg.h\" \n\n")
   MODDIR.write ("#include\"DALSysTypes.h\"\n")
   for drivers in DriverList:
      if drivers == "NULL" or drivers == "null":
         continue 
      MODDIR.write ("extern DALREG_DriverInfo DAL%s_DriverInfo;\n" %drivers)
   MODDIR.write ("\nstatic DALREG_DriverInfo * DALDriverInfoArr[] = {\n")
   driver_device_id_len =0
   for drivers in DriverList:
      if drivers == "NULL" or drivers == "null":
         continue 
      driver_device_id_len += 1  
      MODDIR.write ("\t& DAL%s_DriverInfo" %drivers)
      MODDIR.write (",\n")
   MODDIR.write ("};\n\n")
   MODDIR.write ("DALREG_DriverInfoList gDALModDriverInfoList = {"+str(driver_device_id_len) + ", DALDriverInfoArr}; \n") 
   PrintStringDeviceNameStruct(0)
   if(len(ConfigStructList)!=0):
      for Col_array in CollisionDictionary:
         MODDIR.write ("\nuint32 "+Col_array+" [] = {"+str(CollisionDictionary[Col_array])[1:-1]+"};")
      MODDIR.write("\n\n\nStringDevice DAL_Mod_driver_list[] = ")
      MODDIR.write("{\n\t\t\t")  
      driver_string_device_len = 0;   
      for list in range(len(ConfigStructList)):
         if(ConfigStructList[list] == "{0,0,0,0,0,0}"): 
            continue
         driver_string_device_len+=1 
         MODDIR.write(ConfigStructList[list])
         if(list <len(ConfigStructList)-1):
            MODDIR.write (",\n\t\t\t")
      if driver_string_device_len ==0:
         MODDIR.write ("{NULL}\n")
      MODDIR.write ("\n};\n")
      MODDIR.write ("\nDALProps DAL_Mod_Info = {NULL, 0 ,"+str(driver_string_device_len)+", DAL_Mod_driver_list};\n")
   else:
      MODDIR.write ("\nDALProps DAL_Mod_Info = {NULL, 0 ,0 , NULL};\n")
      
#------------------------------------------------------------------------------
# CheckDupDriverID : This function will check for duplicate drivers
#------------------------------------------------------------------------------      
def CheckDupDriverID(ModName, DriverList):
   # check if 'driver' is duplicated in the DriverList
   idx = 1
   for driver in DriverList:
      if driver in DriverList[idx:] and driver!= "NULL" and driver!= "null":
         # found a duplicate driver and no need to proceed any further
         print "ERROR *** DAL Driver '%s'" % driver, "has been included more than once for SW image '%s'" % ModName
         print "ERROR *** Please check XML files for '%s'" % ModName
         raise ValueError(driver)
      idx += 1

#------------------------------------------------------------------------------
# CheckDupDeviceID : This function will check for duplicate deviceid
#------------------------------------------------------------------------------
def CheckDupDeviceID(ModName):
   # build the device ID list
   devIDList = []
   for i in range(0, len(DeviceID), 2):
      devIDList.append(DeviceID[i])
   # check if there are any duplicated device IDs
   idx = 1
   for dev in devIDList:
      if dev in devIDList[idx:]:
         # find a duplicate device ID and no need to proceed any further
         print "ERROR *** DAL Device ID 0x%.8x" % dev, "has been included more than once for SW image '%s'" % ModName
         print "ERROR *** Please check XML files for '%s'" % ModName
         raise ValueError(hex(dev))
      idx += 1

#------------------------------------------------------------------------------
# Print_Debug : Print Debug Information
#------------------------------------------------------------------------------
def Print_Debug ():
    print "\nSegmentOffset: ", SegmentOffset, "Len: ", len(SegmentOffset)
    print "\nName Section:", NameSection, "Len: ",len(NameSection)
    print "\nByte Section: ", ByteSection, "Len: ",len(ByteSection)
    print "\nString Section:" , StringSection, "Len: ",len(StringSection)
    print "\nUINT Section:", UintSection, "Len: ",len(UintSection)
    print "\nDevice Section: ", DevSection

#------------------------------------------------------------------------------
# Main function for Propgen
# sys.argv[1] : Master XML File
# sys.argv[2] : Path where Master XML file is generated.
#------------------------------------------------------------------------------
DirName = sys.argv[2]
if not os.path.isdir(DirName):
      os.mkdir( DirName )
ModList = xml.dom.minidom.parse(sys.argv[1])
for Mod in ModList.getElementsByTagName("module"):
   DriverList = []
   ModName = Mod.getAttribute("name")   
   for Driver in Mod.getElementsByTagName("driver"):
      GlobalDef = {}
      DriverList.append(Driver.getAttribute("name"))
      GlobalDef = GenerateGlobalDef(Driver)
      GlobalOffset = {}
      for Device in Driver.getElementsByTagName("device"):
         GenerateArray(Driver.getAttribute("name"),Device, GlobalDef, GlobalOffset)
   # Pop the last element of the array, since it is nothing but the len
   if len(DevSection):
      DevSection.pop()
   AlignSectionAddr()
   # check for duplicated driver IDs and device IDs
   CheckDupDriverID(ModName, DriverList)
   CheckDupDeviceID(ModName)
   PrintModDir(ModName, DriverList)
PrintConfig(ModName)
#Print_Debug()



