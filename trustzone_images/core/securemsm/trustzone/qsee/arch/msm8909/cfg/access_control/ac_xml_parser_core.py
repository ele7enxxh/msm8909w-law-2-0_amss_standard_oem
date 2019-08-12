#Parser to parse the AragronAccessControl.xml, to generate the partition information for TZBSP testing use
#Parser generates information to be plugged into the xpu_info_type and accesscontrol_static_attribute_type
#arrays of \tzbsptest\src\<chipset>\tzt_test_accesscontrol.c
#============================================================================
#
# tzt_test_acparser.py
#
# GENERAL DESCRIPTION
#   Extracts the partition and security configuration information out of AragornAccessControl.xml
#   and writes the information in the format the tzt_test_accesscontrol.c test requires.
#
# Copyright  2013 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#
#  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/msm8909/cfg/access_control/ac_xml_parser_core.py#1 $
#  $DateTime: 2016/06/17 14:31:11 $
#  $Author: pwbldsvc $
#  $Change: 10727476 $
#
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who       what, where, why
# --------   ---       ------------------------------------------------------
# 02/15/13   sm        Initial revision
#
#============================================================================

#For testing in pythonwin, put this in the Run Script window arguments
#--in_xml=..\..\..\trustzone\qsee\arch\msm8974\cfg\access_control\AragornAccessControl.xml --out_xml=..\..\src\msm8974\tzt_test_accesscontrol.h
#--in_xml=..\..\..\trustzone\qsee\arch\mdm9x25\cfg\access_control\ElanAccessControl.xml --out_xml=..\..\src\mdm9x25\tzt_test_accesscontrol.h

import xml.etree.ElementTree as ET
import os
import sys
import getopt
from types import *

g_devcfgxml_fp = None
g_devcfgmap_fp = None
g_devcfgvmid_arr = {}
g_log_error = True #print error messages
g_log_info = True #print informational messages
false = False
true = True

def LogMsg(logit, s ):
  if (logit == True):
    print s + "\n"
  #LogMsg_fp.write(s)
  #LogMsg_fp.write("\n")

def IsPresent(vmid):
  if not((vmid == None) or (vmid == 'false') or (vmid == '') or (vmid == 'NO_ACCESS')):
    return True
  else:
    return False

def GetAllVMIDs():
  return 0xFFFF

def GetSplitVMIDs(vmids):
  global g_devcfgvmid_arr
  groups = vmids.split(',')
  if len(groups) == 1:
    vmid_list = 1 << g_devcfgvmid_arr[vmids]
  else:
    vmid_list = 0
    for iter in groups:
      group = "".join(iter.split())
      vmid_list = vmid_list | (1 << g_devcfgvmid_arr[group])

  return vmid_list

def GetParameter(node, param_name, default_val=None):
  val = default_val
  for parameters in node.getiterator('Parameter'):
    if (parameters.attrib.get('name') == param_name):
      val = parameters.attrib.get('value')
  return val

def GetTopAcr(node, tag_name):
  racr = "0x00000000"
  wacr = "0x00000000"
  rvmids = ""
  wvmids = ""

  if ("RPU_ACR" == tag_name):
    for attribs in node.getiterator(tag_name):
      rvmids = attribs.attrib.get('suvmids')
  else:
    for attribs in node.getiterator(tag_name):
      rvmids = attribs.attrib.get('rvmid')
      wvmids = attribs.attrib.get('wvmid')

  if IsPresent(rvmids):
    if (rvmids == 'ALL'):
      racr = GetAllVMIDs()
    else:
      racr = GetSplitVMIDs(rvmids)

  if IsPresent(wvmids):
    if (wvmids == 'ALL'):
      wacr = GetAllVMIDs()
    else:
      wacr = GetSplitVMIDs(wvmids)

  return racr, wacr

def GetXpuMcr(node, tag_name):
  msae = 0
  cfgere = 0
  clere = 0
  eie = 0
  dcdee = 0
  cfgeie = 0
  cleie = 0

  try:
    for attributes in node.getiterator(tag_name):
      msae_str = attributes.attrib.get('msae')
      cfgere_str = attributes.attrib.get('cfgere')
      clere_str = attributes.attrib.get('clere')
      eie_str = attributes.attrib.get('eie')
      dcdee_str = attributes.attrib.get('dcdee')
      cfgeie_str = attributes.attrib.get('cfgeie')
      cleie_str = attributes.attrib.get('cleie')

    if msae_str.lower() == 'true':
      msae = 1
    if cfgere_str.lower() == 'true':
      cfgere = 1
    if clere_str == 'true':
      clere = 1
    if eie_str == 'true':
      eie = 1
    if dcdee_str == 'true':
      dcdee = 1
    if cfgeie_str == 'true':
      cfgeie = 1
    if cleie_str == 'true':
      cleie = 1

  except:
    LogMsg(g_log_error, "CR attributes missing")

  return msae, cfgere, clere, eie, dcdee, cfgeie, cleie

def GetXpuCr(node, tag_name):
  xpuvmide = 0
  cfgere   = 0
  clere    = 0
  eie      = 0
  dcdee    = 0
  amte     = 0
  cfgeie   = 0
  cleie    = 0

  try:
    for attributes in node.getiterator(tag_name):
      xpuvmide_str = attributes.attrib.get('xpuvmide')
      cfgere_str = attributes.attrib.get('cfgere')
      clere_str = attributes.attrib.get('clere')
      eie_str = attributes.attrib.get('eie')
      dcdee_str = attributes.attrib.get('dcdee')
      amte_str = attributes.attrib.get('amte')
      cfgeie_str = attributes.attrib.get('cfgeie')
      cleie_str = attributes.attrib.get('cleie')

    if xpuvmide_str.lower() == 'true':
      xpuvmide = 1
    if cfgere_str.lower() == 'true':
      cfgere = 1
    if clere_str == 'true':
      clere = 1
    if eie_str == 'true':
      eie = 1
    if dcdee_str == 'true':
      dcdee = 1
    if amte_str == 'true':
      amte = 1
    if cfgeie_str == 'true':
      cfgeie = 1
    if cleie_str == 'true':
      cleie = 1

  except:
    LogMsg(g_log_error, "CR attributes missing")

  return xpuvmide, cfgere, clere, eie, dcdee, amte, cfgeie, cleie

def GetRgnScr(rgn, sec):
  ns = 1
  msaclrwe = 0
  vmidclrwe = 0
  msaclroe = 0
  vmidclroe = 0

  if("TZBSP_XPU_SEC" == sec):
    ns = 0

  try:
    for attributes in rgn.getiterator("RGn_SCR"):
      msaclrwe_str = attributes.attrib.get('msaclrwe')
      vmidclrwe_str = attributes.attrib.get('vmidclrwe')
      msaclroe_str = attributes.attrib.get('msaclroe')
      vmidclroe_str = attributes.attrib.get('vmidclroe')

    if msaclrwe_str.lower() == 'true':
      msaclrwe = 1
    if vmidclrwe_str.lower() == 'true':
      vmidclrwe = 1
    if msaclroe_str.lower() == 'true':
      msaclroe = 1
    if vmidclroe_str.lower() == 'true':
      vmidclroe = 1

  except:
    LogMsg(g_log_error, "RGn_SCR attributes missing")

  return ns, msaclrwe, vmidclrwe, msaclroe, vmidclroe

def GetRgnMcr(rgn, sec):
  msae = 0
  sclrwe = 0
  vmidclrwe = 0
  sclroe = 0
  vmidclroe = 0

  if("TZBSP_XPU_MODEM_PROT" == sec):
    msae = 1

  try:
    for attributes in rgn.getiterator("RGn_MCR"):
      sclrwe_str = attributes.attrib.get('sclrwe')
      vmidclrwe_str = attributes.attrib.get('vmidclrwe')
      sclroe_str = attributes.attrib.get('sclroe')
      vmidclroe_str = attributes.attrib.get('vmidclroe')

    if sclrwe_str.lower() == 'true':
      sclrwe = 1
    if vmidclrwe_str.lower() == 'true':
      vmidclrwe = 1
    if sclroe_str.lower() == 'true':
      sclroe = 1
    if vmidclroe_str.lower() == 'true':
      vmidclroe = 1

  except:
    LogMsg(g_log_error, "RGn_MCR attributes missing")

  return msae, sclrwe, vmidclrwe, sclroe, vmidclroe

def GetXpuScr(node, tag_name):
  xpunse = 0
  scfgere = 0
  clere = 0
  seie = 0
  sdcdee = 0
  nscfge = 0
  nsrgclee = 0
  amte = 0
  scfgeie = 0
  scleie = 0

  try:
    for attributes in node.getiterator(tag_name):
      xpunse_str = attributes.attrib.get('xpunse')
      scfgere_str = attributes.attrib.get('scfgere')
      clere_str = attributes.attrib.get('clere')
      seie_str = attributes.attrib.get('seie')
      sdcdee_str = attributes.attrib.get('sdcdee')
      nscfge_str = attributes.attrib.get('nscfge')
      nsrgclee_str = attributes.attrib.get('nsrgclee')
      scfgeie_str = attributes.attrib.get('scfgeie')
      scleie_str = attributes.attrib.get('scleie')
      if("MPU_SCR" == tag_name):
        amte_str = attributes.attrib.get('amte')
      else:
        amte_str = 'false'

    if xpunse_str.lower() == 'true':
      xpunse = 1
    if scfgere_str.lower() ==  'true':
      scfgere = 1
    if clere_str.lower() ==  'true':
      clere = 1
    if seie_str.lower() == 'true':
      seie = 1
    if sdcdee_str.lower() == 'true':
      sdcdee = 1
    if nscfge_str.lower() == 'true':
      nscfge = 1
    if nsrgclee_str.lower() == 'true':
      nsrgclee = 1
    if scfgeie_str.lower() ==  'true':
      scfgeie = 1
    if scleie_str.lower() == 'true':
      scleie = 1
    if amte_str.lower() == 'true':
      amte = 1

  except:
    LogMsg(g_log_error, "SCR attributes missing")

  return xpunse, scfgere, clere, seie, sdcdee, nscfge, nsrgclee, amte, scfgeie, scleie

def GetFlags(node, flag_name):
  enabled = 0
  static =  0
  owner = 0
  #milestone and has_override is an optional attribute in RGn_FLAGS and defaults to 0 if not present
  has_milestone = 0
  has_milestone_str = None
  has_override = 0
  has_override_str = None
  try:
    for flags in node.getiterator(flag_name):
      enabled_str = flags.attrib.get('enabled')
      static_str = flags.attrib.get('static')
      owner_str = flags.attrib.get('owner')
      has_milestone_str = flags.attrib.get('milestone')
      has_override_str = flags.attrib.get('has_override')

    if enabled_str.lower() == 'true':
      enabled = 1
    if static_str.lower() == 'true':
      static = 1
    if owner_str.lower() == 'tz':
      owner = 1

    #milestone is an optional attribute in RGn_FLAGS and defaults to 0 if not present
    #note has_milestone=true means this partition/rgn/range has a POST_has_milestone use-case which needs to be used
    if has_milestone_str != None:
      if has_milestone_str.lower() == 'true':
        has_milestone = 1

    #has_override is an optional attribute in RGn_FLAGS and defaults to 0 if not present
    #note has_override=true means this partition/rgn/range is overriden in the OVERRIDE use-case
    if has_override_str != None:
      if has_override_str.lower() == 'true':
        has_override = 1

  except:
    LogMsg(g_log_error, "RGn_Flags not present")
    enabled = -1

  return enabled, static, owner, has_milestone, has_override

def GetSingleAC(attrib):
    #This is single vmid case
    #Note no need to differentiate regarding access_type as the XML should not
    #have rovmid #if access_type='0')
  rwvmid = '0'
  rwe = 0
  rwge = 0
  rovmid = '0'
  roe = 0
  roge = 0

  if (attrib.get('rwge') == 'true'):
    rwge = 1
  if  (attrib.get('roge') == 'true'):
    roge = 1
  if  (attrib.get('roe') == 'true'):
    roe = 1
  if  (attrib.get('rwe') == 'true'):
    rwe = 1

  rwvmid = attrib.get('rwvmid')
  if IsPresent(rwvmid):
    rwvmid = rwvmid
  else:
    rwvmid = 'NO_ACCESS'

  rovmid = attrib.get('rovmid')
  if IsPresent(rovmid):
    rovmid = rovmid
  else:
    rovmid = 'NO_ACCESS'

  return rwvmid, rwe, rwge, rovmid, roe, roge

def GetAC(attrib, multi_vmid, perm_type):
  sec = 'TZBSP_XPU_NON_SEC'
  if (attrib.get('sec') == 'true'):
     sec = 'TZBSP_XPU_SEC'
  elif (attrib.get('msa') == 'true'):
     sec = 'TZBSP_XPU_MODEM_PROT'

  r_vmid = '0'
  w_vmid = '0'
  #This is multi vmid case
  if (perm_type == '0'):
    #This is full access/no access permission
    rwvmid = attrib.get('rwvmid')
    if IsPresent(rwvmid):
      if (rwvmid == 'ALL'):
        r_vmid = GetAllVMIDs()
      else:
        r_vmid = GetSplitVMIDs(rwvmid)
      w_vmid = r_vmid
  else:
    #This is seperate R and W access permission
    wvmid = attrib.get('wvmids')
    if IsPresent(wvmid):
      if (wvmid == 'ALL'):
        w_vmid = GetAllVMIDs()
      else:
        w_vmid = GetSplitVMIDs(wvmid)

    rvmid = attrib.get('rvmids')
    if IsPresent(rvmid):
      if (rvmid == 'ALL'):
        r_vmid = GetAllVMIDs()
      else:
        r_vmid = GetSplitVMIDs(rvmid)

  return sec, r_vmid, w_vmid

def FindInList(list, name):
  for i in list:
    if i.attrib.get('name') == name:
      return i.attrib.get('index')
  return -1

def GetIntIndex(index, lookuplist):
  is_ppid = 0
  index_num = FindInList(lookuplist, index)
  if (index_num > -1):
    #We found the name in the RGIndices list. Some index's like
    #the ones for PMIC SPMI are hex values for the PPID's and not
    #really indices so taking care of the hex
    if (index_num.find("0x") != -1):
      int_index = int(index_num,16)
      #An indice in hex means it's a PPID
      is_ppid = 1
    else:
      int_index = int(index_num,10)
  else:
    #index was listed directly e.g index="0" in the XML.
    int_index = int(index)

  return int_index, is_ppid

def WriteRGnToFile(xpu_type, xpu_index, rgn, index, startaddr, endaddr, multi_vmid, perm_type, nrg):
  (sec, r_vmid, w_vmid) = GetAC(rgn.attrib, multi_vmid, perm_type)
  (enabled, static, owner, has_milestone, has_override) = GetFlags(rgn, 'RGn_FLAGS')
  if (enabled == -1):
    LogMsg(g_log_error, "Attention! RGn_Flags missing...manually setting enabled=0 so this partition is skipped")
    enabled = 0

  if (has_override == 0):
    if (0 == nrg):
      if("MPU" == xpu_type):
        rgn_prefix = "PRTn"
      else:
        rgn_prefix = "RGn"
      g_devcfgxml_fp.write("<props name=\"" + rgn_prefix + "[]\" type=\"" + rgn_prefix + "_t\">\n{\n")

    g_devcfgxml_fp.write("{\n");
    if("MPU" == xpu_type):
      #if (startaddr != endaddr):
        g_devcfgxml_fp.write("%s,\n%s,\n" % (startaddr, endaddr));
    g_devcfgxml_fp.write("%d,\n" %(index))

    (ns, msaclrwe, vmidclrwe, msaclroe, vmidclroe) = GetRgnScr(rgn, sec);
    g_devcfgxml_fp.write("{%d, %d, %d, %d, %d},\n" % (ns, msaclrwe, vmidclrwe, msaclroe, vmidclroe))
    (msae, sclrwe, vmidclrwe, sclroe, vmidclroe) = GetRgnMcr(rgn, sec);
    g_devcfgxml_fp.write("{%d, %d, %d, %d, %d},\n" % (msae, sclrwe, vmidclrwe, sclroe, vmidclroe))

    if("MPU" == xpu_type):
      g_devcfgxml_fp.write("{1, ");
    else:
      g_devcfgxml_fp.write("{0, ");
    g_devcfgxml_fp.write("%d, %d, %d, %d, %d},\n" % (enabled, static, owner, has_milestone, has_override));

    g_devcfgxml_fp.write("{\n")
    if("1" == multi_vmid):
      g_devcfgxml_fp.write("{%s,\n" % (r_vmid))
      g_devcfgxml_fp.write("%s},\n" % (w_vmid))
    else:
      (rwvmid, rwe, rwge, rovmid, roe, roge) = GetSingleAC(rgn.attrib)
      #g_devcfgxml_fp.write("{%s, %d, %d, %s, %d, %d},\n" % (rwvmid, rwe, rwge, rovmid, roe, roge))
      val = g_devcfgvmid_arr[rwvmid] | (rwe << 5) | (rwge << 6) | (g_devcfgvmid_arr[rovmid] << 7) | (roe << 12) | (roge << 13)
      g_devcfgxml_fp.write("{%d, 0},\n" % (val))
    g_devcfgxml_fp.write("},\n")

    g_devcfgxml_fp.write("},\n");
    nrg = nrg + 1

  if (owner == 0):
    LogMsg(g_log_info, "Skipping....startaddr:%s for xpu_index:%d as TZ is not owner)" % (startaddr, xpu_index))
  if (has_override == 1):
    LogMsg(g_log_info, "Skipping....startaddr:%s for xpu_index:%d has override=true" % (startaddr, xpu_index))

  return nrg

def PrintXpuTop(node, xpu_type, amt_type):
  g_devcfgxml_fp.write("<device id =\"%s\">\n" % node.attrib.get("name"));
  if("MPU" == xpu_type):
    if("1" == amt_type):
      g_devcfgxml_fp.write("<props name=\"ctrl_reg\" type=\"mpu_amt_ctrl_reg_t\">\n");
    else:
      g_devcfgxml_fp.write("<props name=\"ctrl_reg\" type=\"mpu_noamt_ctrl_reg_t\">\n");
  else:
    g_devcfgxml_fp.write("<props name=\"ctrl_reg\" type=\"xpu_ctrl_reg_t\">\n");

  g_devcfgxml_fp.write("{\n");

  (xpunse, scfgere, clere, seie, sdcdee, nscfge, nsrgclee, amte, scfgeie, scleie) = GetXpuScr(node, xpu_type + '_SCR');
  g_devcfgxml_fp.write("{%d, %d, %d, %d, %d, %d, %d, %d, %d, %d},\n" % (xpunse, scfgere, clere, seie, sdcdee, nscfge, nsrgclee, amte, scfgeie, scleie));

  (xpuvmide, cfgere, clere, eie, dcdee, amte, cfgeie, cleie) = GetXpuCr(node, xpu_type + '_CR');
  g_devcfgxml_fp.write("{%d, %d, %d, %d, %d, %d, %d, %d},\n" % (xpuvmide, cfgere, clere, eie, dcdee, amte, cfgeie, cleie));

  (msae, cfgere, clere, eie, dcdee, cfgeie, cleie) = GetXpuMcr(node, xpu_type + '_MCR');
  g_devcfgxml_fp.write("{%d, %d, %d, %d, %d, %d, %d},\n" % (msae, cfgere, clere, eie, dcdee, cfgeie, cleie));

  (enabled, static, owner, has_milestone, has_override) = GetFlags(node, xpu_type + '_FLAGS')
  if("MPU" == xpu_type):
    g_devcfgxml_fp.write("{1, ");
  else:
    g_devcfgxml_fp.write("{0, ");
  g_devcfgxml_fp.write("%d, %d, %d, %d, %d},\n" % (enabled, static, owner, has_milestone, has_override));

  (suvmids, unused)= GetTopAcr(node, "RPU_ACR");
  g_devcfgxml_fp.write("%s,\n" % (suvmids));

  if("MPU" == xpu_type):
    (ump_racr, ump_wacr)= GetTopAcr(node, "UMP_xCR");
    g_devcfgxml_fp.write("%s,\n%s,\n" % (ump_racr, ump_wacr));

    if("1" == amt_type):
      (amt_racr, amt_wacr)= GetTopAcr(node, "AMT_xCR");
      g_devcfgxml_fp.write("%s,\n%s,\n" % (amt_racr, amt_wacr));

  g_devcfgxml_fp.write("}\n</props>\n");

def WritePrologueToFile():
  g_devcfgxml_fp.write("#include \"tzbsp_xpu_devconfig.h\"\n");
  g_devcfgxml_fp.write("<driver name=\"NULL\">\n");

def WriteEpilogueToFile():
  g_devcfgxml_fp.write("</driver>\n");

def WriteMapPrologue():
  g_devcfgmap_fp.write("#include \"HALxpu2.h\"\n")
  g_devcfgmap_fp.write("void tzbsp_memzi(uint32* addr, uint32 size);\n")
  g_devcfgmap_fp.write("const char * g_tzbsp_xpu_devcfg_map[HAL_XPU2_COUNT] = {0};\n")
  g_devcfgmap_fp.write("void tzbsp_xpu_devcfg_map_init(void)\n{\n")
  g_devcfgmap_fp.write("tzbsp_memzi((uint32 *)g_tzbsp_xpu_devcfg_map, 4*HAL_XPU2_COUNT);\n")

def WriteMapEpilogue():
  g_devcfgmap_fp.write("}\n")

def  PopulateVmidMap(root):
  global g_devcfgvmid_arr
  for node in root.getiterator('VMIDmap'):
    for vm in node.getiterator('vm'):
      id = vm.attrib.get("id")
      name = vm.attrib.get("name")
      g_devcfgvmid_arr[name] = int(id)
      LogMsg(g_log_info, "VMID Map %s -> %d" % (name, g_devcfgvmid_arr[name]))

#----------------------------------------------------------------------------
# usage
# Help on how to use this script
#----------------------------------------------------------------------------
def usage ():
  print("========================================================")
  print("-h, --help       print this help\n")
  print("usage\n")
  print("   where <xml_file> specifies the full path + filename of the XML to parse")
  print("   where <header dir> specifies where the full path + filename of the test header file to be written\n")
  print("   where <chipset> specifies target. This is an optional argument \n")
  print("e.g: tzt_xml_parser.py --in_xml=\"c:\\temp\\AccessControl.xml\" --out_xml=\"c:\\test\\inc\\tztac.h\"")
  print("e.g: tzt_xml_parser.py --in_xml=\"c:\\temp\\AccessControl.xml\" --out_xml=\"c:\\test\\inc\\tztac.h\" --chipset=\"msm8974\"")
  print("========================================================\n")

def main(argv):
  global g_devcfgxml_fp
  global g_devcfgmap_fp
  #print argv

  #Step: Get the XML path/file to parse and path/file of the test header to write
  #from the argument list

  if type(argv) is StringType:
    #This happens when Sconscript calls the function with string argument
    argv = argv.split(' ')
  try:
      opts, args = getopt.getopt(argv, '', ["in_xml=", "out_xml=", "chipset="])
  except getopt.GetoptError, err:
      # LogMsg help information and exit:
      print str(err)
      usage()
      sys.exit(2)

  in_xml = None
  out_xml = None

  if (len(sys.argv) < 2):
      usage()
      sys.exit(2)

  for opt, arg in opts:
      if (opt == "--in_xml"):
          in_xml = arg
          LogMsg(g_log_info, "--in_xml is %s" % in_xml)
      elif (opt == "--out_xml"):
          out_xml = arg
          LogMsg(g_log_info, "--out_xml is %s" % out_xml)
      elif (opt == "--chipset"):
          # note this argument is optional and currently not used
          chipset = arg
          LogMsg(g_log_info, "--chipset is %s" % chipset)

  if (in_xml is None):
    print "XML file not specified"
    usage()
    sys.exit(2)

  if (out_xml is None):
    print "Output XML file not specified"
    usage()
    sys.exit(2)

  g_devcfgxml_fp = open(out_xml, 'wb')
  g_devcfgmap_fp = open("../../src/tzbsp_xpu_devcfg_map.c", 'wb')
  tree = ET.parse(in_xml)

  root = tree.getroot()

  #Step: Write preamble to output xml
  WritePrologueToFile()

  WriteMapPrologue()

  PopulateVmidMap(root)
  #Step: Find the default HW configurations. These values can be overriden in each XPU, but if
  #the XPU does not override, these defaults should be used
  for node in root.getiterator('HWConfigDefaults'):
    default_client_addr_lsb = GetParameter(node,'CLIENT_ADDR_lSB',None)
    default_multi_vmid = GetParameter(node,'MULTI_VMID', None)
    default_perm_type = GetParameter(node,'PERMISSION_TYPE', None)
    default_amt_type = GetParameter(node,'AMT_HW_ENABLE', None)
    if ((default_client_addr_lsb == None) or (default_multi_vmid == None) or (default_perm_type == None) or (default_amt_type == None)):
      LogMsg(g_log_error, "Attention! No CLIENT_ADDR_lSB, MULTI_VMID, PERMISSION_TYPE or NUM_RES_GRPspecified")
      raise Exception

  #Step: Find the list of all the RGIndices as we'll need them later
  RGIndicesList = root.findall('HWConfigDefaults/RGIndices/RGIndex')

  #=========PARSE ALL THE MPUs=========================================
  node_count = 0
  for node in root.getiterator('MPU'):
    xpu_name = node.attrib.get("name")
    LogMsg (g_log_info, "Processing MPU: %s" % xpu_name)
    g_devcfgmap_fp.write("  g_tzbsp_xpu_devcfg_map[HAL_XPU2_" + xpu_name + "] = \"" + xpu_name + "\";\n")
    multi_vmid = GetParameter(node, 'MULTI_VMID',      default_multi_vmid)
    perm_type  = GetParameter(node, 'PERMISSION_TYPE', default_perm_type)
    amt_type   = GetParameter(node, 'AMT_HW_ENABLE', default_amt_type)

    #Parse top level flags and control registers
    PrintXpuTop(node, "MPU", amt_type)
    nrg = 0;

    #Parse each partition. There is only 1 index per partition
    for usecase in node.getiterator('PROTECTION'):
      if('DEFAULT' == usecase.attrib.get('usecase')):
        for prtn in usecase.getiterator('PRTn'):
          #Find the index of for the partition by looking it up in the RGIndices list
          index = prtn.attrib.get('index')

          #We don't expect index to be ... and specified in name tag for MPU's
          if (index == '...'):
            LogMsg (g_log_error, "Attention! index of ... not expected for an MPU!!!")
            raise Exception

          int_index, is_ppid = GetIntIndex(index, RGIndicesList)
          if (is_ppid == 1):
            LogMsg (g_log_error, "Attention! index listed in hex not expected for an MPU!!!")
            raise Exception

          #Find start and end address
          startaddr = prtn.attrib.get('start')
          if (startaddr == None):
            LogMsg(g_log_error, "No start address found for xpu_index:%d, partition: %d" % (node_count, int_index))
            raise Exception

          endaddr = prtn.attrib.get('end')
          if (endaddr == None):
            LogMsg(g_log_error, "No start address found for xpu_index:%d, partition: %d" % (node_count, int_index))
            raise Exception

          #Write test data to file
          nrg = WriteRGnToFile("MPU", node_count, prtn, int_index, startaddr, endaddr, multi_vmid, perm_type, nrg)

        if (0 != nrg):
          g_devcfgxml_fp.write("}\n</props>\n");

    g_devcfgxml_fp.write("<props name=\"nrg\" type=\"uint32\">%s</props>\n" % (nrg))
    g_devcfgxml_fp.write("</device>\n");
    node_count = node_count + 1

  #===========PARSE ALL THE RPUs=========================================
  #A resource group protects multiple registers. Each resource group is a
  #fixed size, starting from the base address + offset of the resource group
  #number (index)
  for node in root.getiterator('RPU'):
    xpu_name = node.attrib.get("name")
    LogMsg (g_log_info, "Processing RPU: %s" % xpu_name)
    g_devcfgmap_fp.write("  g_tzbsp_xpu_devcfg_map[HAL_XPU2_" + xpu_name + "] = \"" + xpu_name + "\";\n")
    multi_vmid = GetParameter(node, 'MULTI_VMID', default_multi_vmid)
    perm_type =  GetParameter(node, 'PERMISSION_TYPE', default_perm_type)
    client_addr_lsb = GetParameter(node, 'CLIENT_ADDR_lSB', default_client_addr_lsb)
    size_of_partition = (pow(2,int(client_addr_lsb,10))) #2^client_addr_lsb

    PrintXpuTop(node, "RPU", "0")
    nrg = 0;

    #XML can group multiple resource groups with common security settings
    #so there can be an RGn XML tag with multiple indexes (resource groups)
    #specified.
    for usecase in node.getiterator('PROTECTION'):
      if('DEFAULT' == usecase.attrib.get('usecase')):
        for rgn in usecase.getiterator('RGn'):
          index = rgn.attrib.get('index')
          if (index == '...'):
            #The resource groups are listed under the <name> tag in RPU's, and
            #are seperated by a ,. Get each name and look it up in the RG indices
            #list
            for rg in rgn.getiterator('name'):
              #LogMsg rg.text
              groups = rg.text.split(',')
              for i in groups:
                g = "".join(i.split())
                #Have seen that some have the last index name listed with a , so python
                #generates an empty extra group, so making sure the g is not empty
                if (g != ''):
                  int_index, is_ppid = GetIntIndex(g, RGIndicesList)

                  #Write test data to file.
                  nrg = WriteRGnToFile("RPU", node_count, rgn, int_index, 0, 0, multi_vmid, perm_type, nrg)
                else:
                  LogMsg (g_log_error, "Attention...index list %s ending with comma" % (groups))
          else:
            int_index, is_ppid = GetIntIndex(index, RGIndicesList)

            #Write test data to file
            nrg = WriteRGnToFile("RPU", node_count, rgn, int_index, 0, 0, multi_vmid, perm_type, nrg)

        if (0 != nrg):
          g_devcfgxml_fp.write("}\n</props>\n");

    g_devcfgxml_fp.write("<props name=\"nrg\" type=\"uint32\">%s</props>\n" % (nrg))
    g_devcfgxml_fp.write("</device>\n");
    node_count = node_count + 1

  #===========PARSE ALL THE APUs=========================================
  for node in root.getiterator('APU'):
    xpu_name = node.attrib.get("name")
    LogMsg (g_log_info, "Processing APU: %s" % xpu_name)
    g_devcfgmap_fp.write("  g_tzbsp_xpu_devcfg_map[HAL_XPU2_" + xpu_name + "] = \"" + xpu_name + "\";\n")
    multi_vmid = GetParameter(node, 'MULTI_VMID', default_multi_vmid)
    perm_type =  GetParameter(node, 'PERMISSION_TYPE', default_perm_type)

    PrintXpuTop(node, "APU", "0")
    nrg = 0;

    #XML can group multiple resource groups with common security settings
    #so there can be an RGn XML tag with multiple indexes (resource groups)
    #specified.
    for usecase in node.getiterator('PROTECTION'):
      if('DEFAULT' == usecase.attrib.get('usecase')):
        for rgn in usecase.getiterator('RGn'):
          index = rgn.attrib.get('index')
          if (index == '...'):
            #The resource groups are listed under the <name> tag in RPU's, and
            #are seperated by a ,. Get each name and look it up in the RG indices
            #list
            for rg in rgn.getiterator('name'):
              #LogMsg rg.text
              groups = rg.text.split(',')
              for i in groups:
                g = "".join(i.split())
                #Have seen that some have the last index name listed with a , so python
                #generates an empty extra group, so making sure the g is not empty
                if (g != ''):
                  int_index, is_ppid = GetIntIndex(g, RGIndicesList)

                  #Write test data to file.
                  nrg = WriteRGnToFile("RPU", node_count, rgn, int_index, 0, 0, multi_vmid, perm_type, nrg)
                else:
                  LogMsg (g_log_error, "Attention...index list %s ending with comma" % (groups))
          else:
            int_index, is_ppid = GetIntIndex(index, RGIndicesList)

            #Write test data to file
            nrg = WriteRGnToFile("RPU", node_count, rgn, int_index, 0, 0, multi_vmid, perm_type, nrg)

        if (0 != nrg):
          g_devcfgxml_fp.write("}\n</props>\n");

    g_devcfgxml_fp.write("<props name=\"nrg\" type=\"uint32\">%s</props>\n" % (nrg))
    g_devcfgxml_fp.write("</device>\n");
    node_count = node_count + 1

  #Complete the test array
  WriteEpilogueToFile()
  WriteMapEpilogue()
  g_devcfgxml_fp.close()
  g_devcfgmap_fp.close()

if __name__ == "__main__":
  main(sys.argv[1:])

