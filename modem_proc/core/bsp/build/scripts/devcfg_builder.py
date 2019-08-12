#===============================================================================
#
# Device Config Builder
#
# GENERAL DESCRIPTION
#    Contains builder(s) defenitions
#
# Copyright (c) 2012 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/core.mpss/3.5.c12.3/bsp/build/scripts/devcfg_builder.py#1 $
#  $DateTime: 2016/12/13 07:59:23 $
#  $Author: mplcsds1 $
#  $Change: 11985146 $
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
# 11/14/11   jay     Device Configuration builder implementation
#===============================================================================
import re
import os
import subprocess
import string
from SCons.Script import *
import types

#------------------------------------------------------------------------------
# Globals
#------------------------------------------------------------------------------
DEVCFG_ENV_DESCRIPTOR = 'DEVCFG_INFO_COLLECTOR'
DEVCFG_CFILE_DESCRIPTOR = 'DEVCFG_CFILE_INFO_COLLECTOR'
DEVCFG_CURRENT_USED_TAGS = 'DEVCFG_CURRENT_USED_XML_TAGS'
DEVCFG_CFILE_DICT = 'DEVCFG_CFILE_DICT'

file_idx = 0

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
    return True

def generate(env):
    """
    Generate function for devcfg builder. 
    Sets up the action, scanner and builder function to be called by clients to
    pass xml, devId details to the builder.
    """

    rootenv = env.get('IMAGE_ENV')
    rootenv[DEVCFG_ENV_DESCRIPTOR] = []
    rootenv[DEVCFG_CFILE_DESCRIPTOR] = []
    rootenv[DEVCFG_CURRENT_USED_TAGS] = []
    rootenv[DEVCFG_CFILE_DICT] = {}
    
    devcfg_xml_auto_generate(env)
    devcfg_c_auto_generate(env)
    devcfg_c_struct_generate(env)
    
    return

#===============================================================================
# DEV CFG STRUCT FILE Install Rules
#===============================================================================
def devcfg_c_struct_generate(env):
    
    rootenv = env.get('IMAGE_ENV')   
    # load builder into enviroment
    env.Append(BUILDERS = {'QualifyStructNames' : Builder(action=devcfg_qualify_struct_name_builder)})
    
#===============================================================================
# DEV CFG STRUCT FILE Install Rules
#===============================================================================
def devcfg_c_auto_generate(env):
    
    rootenv = env.get('IMAGE_ENV')   
    # load builder into enviroment
    devcfg_c_act = env.GetBuilderAction(devcfg_c_builder, action_source=None)
    devcfg_c_bld = env.Builder(action = devcfg_c_act)
    rootenv.Append(BUILDERS = {'GenerateCFile' : devcfg_c_bld})

#===============================================================================
# DEV CFG XML build rules
#===============================================================================
def devcfg_xml_auto_generate(env):
    
    rootenv = env.get('IMAGE_ENV')
    # Add method to enviroment. 
    # AddDevCfgInfo is the function that needs to be called by the clients to pass
    # dev cfg xml file name with location, devID is needed to be added, APIs needed
    # to compile any .h files included in the xml file.
    rootenv.AddMethod(add_devcfg_info, "AddDevCfgInfo")
    
    rootenv.AddMethod(get_devcfg_currently_used_tags, "GetDevcfgRelevantTags")
    rootenv.AddMethod(get_devcfg_c_file_tag, "GetDevcfgCFile")
    
    # load builder into enviroment
    # Set action_source to None as there are no sources passed into the action function. 
    # If this isn't set explicitly, then build command with verbose=2 fails.
    devcfg_act = env.GetBuilderAction(devcfg_xml_builder, action_source=None)
    devcfg_target_scan = env.Scanner(devcfg_target_scan_func, name='DevCfgTargetScanner')
    devcfg_bld = env.Builder(action = devcfg_act,
                             target_scanner = devcfg_target_scan
                             #suffix = '.xml'
                             )
    rootenv.Append(BUILDERS = {'DevcfgBuilder' : devcfg_bld})
    
    return
 
def devcfg_c_builder(target, source, env):    
    each_xml_tag = env['EACH_XML_TAG'] 
    c_output_file = str(target[0])
    c_input_file = str(source[0])
    dir_name,file_name = os.path.split(c_output_file)
    dir_name = env.RealPath(dir_name)
    if not os.path.exists(dir_name):
       os.makedirs(dir_name)
    file_src_fp = open(c_input_file,"r")
    file_dest_fp = open(c_output_file,"w")
    for i in range(len(source)-1):
      file = str(source[i+1])
      file_dest_fp.write('#include "' + '..' + (file.replace('\\','/')).split(env['BUILDPATH']+'/'+each_xml_tag)[1] + '"\n')
    file_dest_fp.write(file_src_fp.read())
    file_src_fp.close()
    file_dest_fp.close()     
    return 
    
def devcfg_target_scan_func(node, env, path):
    """
    Scanner adds the include paths to dal config environment which will 
    be used to compile autogenerated c files while rebuilding. This gets
    called from the dal config scons environment.
    """
    rootenv = env.get('IMAGE_ENV')
    
    #Instead of returning any nodes just add the PP XMLs as deps
    tag_dicts = env.get('DEVCFG_XML_DICT')
    PP_XML_TAGS = []
    for each_xml_tag, pp_xml_tag in tag_dicts.items():
        PP_XML_TAGS.append(pp_xml_tag)
    
    for sub_dict in rootenv.get('DEVCFG_INFO_COLLECTOR', []):
        for pp_xml_tag in PP_XML_TAGS:
           xml_file_list = sub_dict.get(pp_xml_tag, [])
           c_file_list = sub_dict.get(pp_xml_tag.replace('pp','cc'), [])
           for file in xml_file_list:
              env.Depends(node, file)
           for file in c_file_list:
              env.Depends(node, file)
    return []

def get_devcfg_currently_used_tags(env, targets, thread_input_dict):
    rootenv = env.get('IMAGE_ENV')
    return rootenv[DEVCFG_CURRENT_USED_TAGS]

def get_devcfg_c_file_tag(env, targets, thread_input_dict,xml_tag):
    rootenv = env.get('IMAGE_ENV')
    return rootenv[DEVCFG_CFILE_DICT][xml_tag]

def devcfg_qualify_struct_name_emitter(target, source, env):
    return (target, source)    
    
def devcfg_qualify_struct_name_builder(target,source,env):
    # import pdb;pdb.set_trace()
    dir_name,file_name = os.path.split(str(target[0]))
    dir_name = env.RealPath(dir_name)
    if not os.path.exists(dir_name):
       os.makedirs(dir_name)
    fp = open(str(target[0]),"w")
    xml_tag = env['XML_TAG']
    xmlfile_fullpath = str(source[0])  
    struct_name_list = extract_c_struct_variable_from_xml(xmlfile_fullpath)
    for struct in struct_name_list:
        fp.write('#define '+struct+' '+struct+'_'+xml_tag+'\n')
    fp.close()
    return 

               
def extract_c_struct_variable_from_xml(xml_file):
    unique_struct_list = []
    temp_line = ""
    inside = 0
    xml_file_fp = open(xml_file, "r")
    for line in xml_file_fp:
        line = line.strip()
        if line.startswith('<props') and ((line.find('DALPROP_ATTR_TYPE_STRUCT_PTR') != -1) or (line.find('0x00000012') != -1)):
            temp_line = line
            if (line.endswith('</props>') == False):
                inside = 1
                continue
            else:
                struct_name = find(temp_line)
                if (struct_name != None) and (struct_name not in unique_struct_list):
                    unique_struct_list.append(struct_name)
        if (inside == 1):
            temp_line += line
            if (line.endswith('</props>') == False):
                continue
            else:
                inside = 0
                struct_name = find(temp_line)
                if (struct_name != None) and (struct_name not in unique_struct_list):
                    unique_struct_list.append(struct_name)
        
    return unique_struct_list
  
def find(xml_node):
    temp_line = ""
    line_parts = xml_node.split('>',1)
    value = line_parts[1].split('<',1)
    struct_name =  value[0].strip()
    return struct_name

def resolve_platform_all_tag(env,thread_input_dict,tag_list):
    plat_key = 'NULL'
    plat_value =[]
    soc = 'NULL'
    for key,value in thread_input_dict.items():
        if 'PLATFORM_ALL' in key or 'platform_all' in key:
           plat_key = key
           plat_value = value
           soc = key.split('_')[0]
    if plat_key is not 'NULL' and soc is not 'NULL':    
        plat_tag_list = []
        for tags in tag_list:
          #find all platform tags for that soc
          if 'PLATFORM' in tags and soc in tags:
             plat_tag_list.append(tags) 
        for plat in plat_tag_list:
          thread_input_dict[plat] = plat_value 
        del thread_input_dict[plat_key]   
    return thread_input_dict  
    
def resolve_soc_all_tag(env,thread_input_dict,tag_list):
    key = 'NULL'
    if thread_input_dict.has_key('SOC_XML'):
       key = 'SOC_XML'
    elif thread_input_dict.has_key('soc_xml'):
       key = 'soc_xml'   
       
    if key is not 'NULL':   
       soc_list = []
       for tags in tag_list:
          if 'PLATFORM' not in tags:
             soc_list.append(tags)        
       list = thread_input_dict[key]      
       for soc in soc_list:
          thread_input_dict[soc] = list 
       del thread_input_dict[key]     
    return thread_input_dict  

def find_file_name_list(env,thread_input_dict,each_xml_tag,ext):  
    file_list = []
    for file in  thread_input_dict[each_xml_tag]:
         fullpath = env.RealPath(str(file))
         fname = os.path.basename(fullpath)
         fname_ext = os.path.splitext(fname)[1]
         if ext.lower() in fname_ext.lower():
            file_list.append(fullpath)
    return set(file_list)

def check_and_ignore_xml_file_duplicates(env,rootenv,xml_file_list,each_xml_tag):
    dupe_list = []
    for grpList in rootenv[DEVCFG_ENV_DESCRIPTOR]:
       for xml_file in xml_file_list:
          exisiting_xml_list = grpList.get(each_xml_tag, [])
          for existing_file in exisiting_xml_list:
              if xml_file in existing_xml_list:
                 env.PrintWarning("Duplicate Device Configuration Input File Specified " + xml_file)
              dupe_list.append(xml_file)
    for file in dupe_list:
       xml_file_list.remove(file)
    return xml_file_list 

def add_devcfg_info(env, targets, thread_input_dict):
    """
    When AddDevCfgInfo is called by the client, this add_devcfg_xml gets invoked in the
    devcfg builder env & the details passed by the clients are stored in a dictionary here.
    """
    # import pdb;pdb.set_trace()
    rootenv = env.get('IMAGE_ENV')
    if rootenv.GetUsesFlag('USES_DEVCFG') is False:
        return    

    # TODO: the check for USES_MBA can be removed once the ASSOC flag can be 
    # added to the modem sconscript 
    if env.GetUsesFlag('USES_MBA') is True:
        if env.get('DEVCONFIG_ASSOC_FLAG') not in targets:
            return

    # DEVCFG_XML needs to be passed whenever AddDevCfgInfo is called from the client scons file.
    # if DEVCFG_XML not in thread_input_dict:
        # devcfg_error(env, DEVCFG_XML + 'required')
    tag_list = env.get('DEVCFG_XML_TAGS')
    tag_dicts = env.get('DEVCFG_XML_DICT')
    if tag_list is None:
        err_str = 'DEVCFG_XML_TAGS are not found. Check build\ms\image_cfg.xml to make sure tags have been added'
        err_str += 'Also, check tbc_core_devcfg.py to make sure these are being generated\n'
        devcfg_error(env, err_str)
    #resolve any soc_all tag    
    thread_input_dict = resolve_soc_all_tag(env,thread_input_dict,tag_list)
    thread_input_dict = resolve_platform_all_tag(env,thread_input_dict,tag_list)
    
    #import pdb;pdb.set_trace()
    for each_xml_tag, pp_xml_tag in tag_dicts.items():
        # Dictionary to hold dal device id and device config xml locations
        newDict = {}                 
        if each_xml_tag in thread_input_dict:
        
            xml_file_list = find_file_name_list(env,thread_input_dict,each_xml_tag,'.xml')
            xml_file_list = check_and_ignore_xml_file_duplicates(env,rootenv,xml_file_list,each_xml_tag)
            sources_c_file_gen_builder = []            
            for file in xml_file_list:
               #Obtain the parent directory name of the file
               path, xml_fname = os.path.split(file)
               residue, dir_name = os.path.split(path)
               #Obtain just the file name + ext
               xml_fname_no_ext = os.path.splitext(xml_fname)[0]
               #Preprocess the XML
               pp_xml = env.PreProcess('${BUILDPATH}/' + each_xml_tag + '/' + dir_name + '/'+ xml_fname + '.pp', file)
               #Cleanup the Preprocessed XML
               pp_xml_cl = env.PreProcessClean('${BUILDPATH}/' + each_xml_tag + '/' + dir_name + '/' + xml_fname + '.i', pp_xml)
               newDict.setdefault(pp_xml_tag, [  ]).append(pp_xml_cl[0])
               devcfg_struct_qual_name = env.RealPath('${BUILDPATH}/'+ each_xml_tag +'/'+ dir_name +'/'+'devcfg_'+xml_fname_no_ext+'.h')
               devcfg_struct_qual = env.QualifyStructNames(devcfg_struct_qual_name,pp_xml_cl,XML_TAG=each_xml_tag)
               sources_c_file_gen_builder.append(devcfg_struct_qual)
            
            c_file_list = find_file_name_list(env,thread_input_dict,each_xml_tag,'.c')
            cc_xml_tag = pp_xml_tag.replace('pp','cc')
            for file in c_file_list:
                #Obtain the parent directory name of the file
                path, c_fname = os.path.split(file)
                residue, dir_name = os.path.split(path)
                for build_tag in targets :
                    if build_tag in env:
                       build_path = env.RealPath('${BUILDPATH}/'+each_xml_tag+'/')
                       if not os.path.exists(build_path):
                           os.makedirs(build_path)
                       pp_cfile = env.PreProcess(build_path + '/' + dir_name + '/' + c_fname + '.pp', file)
                       pp_cfile_cl = env.PreProcessClean(build_path + '/' + dir_name + '/' + c_fname  +'.i', pp_cfile)
                       file_prefix = each_xml_tag.replace('xml','devcfg')
                       sources_c_file_gen_builder.insert(0,pp_cfile_cl)
                       output_c_file = env.GenerateCFile(build_path + '/' + dir_name + '/' + file_prefix +'_'+c_fname,sources_c_file_gen_builder,EACH_XML_TAG=each_xml_tag)
                       output_obj_node = env.AddObject(build_tag , output_c_file[0])
                       newDict.setdefault(cc_xml_tag, [  ]).append(output_obj_node[0])
                       sources_c_file_gen_builder.pop(0)                     
                       
            rootenv[DEVCFG_ENV_DESCRIPTOR].append(newDict)
            
            if each_xml_tag not in rootenv[DEVCFG_CURRENT_USED_TAGS]:
                rootenv[DEVCFG_CURRENT_USED_TAGS].append(each_xml_tag)
                rootenv.Replace(DEVCFG_ENV_USED_XML_TAGS = rootenv[DEVCFG_CURRENT_USED_TAGS])
                #rootenv.Append(CPPDEFINES = ["DEVCFG_ENV_USED_XML_TAGS=$(DEVCFG_ENV_USED_XML_TAGS)"]) 
                #rootenv.Append(CPPDEFINES = ["DEVCFG_ENV_USED_XML_TAGS=$(DEVCFG_ENV_USED_XML_TAGS)"]) 
    #print 'rootenv[DEVCFG_ENV_DESCRIPTOR]: ', rootenv[DEVCFG_ENV_DESCRIPTOR]
    #print 'rootenv[DEVCFG_CURRENT_USED_TAGS]: ', rootenv[DEVCFG_CURRENT_USED_TAGS]

    return
    

def devcfg_xml_builder(target, source, env):
    """
    devcfg_xml_builder gets invoked as its a rule in dal\config\SConscript to create the master xml file
    and DALDeviceId_autogen.h.
    The master xml file will be the #includes of all the xml files passed by the clients.
    target[0] will have the master xml location & name.
    """
    rootenv = env.get('IMAGE_ENV')
    
    if rootenv.GetUsesFlag('USES_DEVCFG') is False:
        return None
        
    # Creation of master dal xml file
    target_xml_full = env.RealPath(str(target[0]))
    target_env_full = env.RealPath(str(target[1]))    
    master_xml = devcfg_create_master_xml_file(rootenv, target_xml_full , target_env_full , env['CONFIG'], env, env['UPDATE_ENV'])
    
    return
    
    
def devcfg_create_master_xml_file(env, filepathname, env_filepathname, env_config_list, dalenv, update_env):
    """
    devcfg_create_master_xml_file creates the master dal xml file that includes all the 
    xml files passed in by various scons files
    """
    
    # Determine the module name that needs to be added to the module tag in the xml file
    moduleName = ''

    proc_name = env.get('PROC_CONFIG')

    if proc_name == 'wcn':
        moduleName = 'wcn'
    elif proc_name == 'modem' or proc_name == 'qdsp6_sw':
        moduleName = 'modem'
    elif env.has_key('CORE_SPS'):
        moduleName = 'dsps'
    elif env.has_key('CORE_RPM') or env.has_key('RPM_IMAGE'):
        moduleName = 'rpm'
    elif env.has_key('TZOS_IMAGE'):
        moduleName = 'tz'
    elif env.has_key('BUILD_BOOT_CHAIN') or env.has_key('BUILD_TOOL_CHAIN'):
        moduleName = 'boot'
    elif env.has_key('APPS_PROC'):
        moduleName = 'apps'
    else:
        # If none of the above are applicable, then throw an error to indicate 
        # that a proper build env needs to be present for module name
        devcfg_error(env, 'Need image definition for determining module name for dal master cfg file')
    moduleNameStr = '<module name="' + moduleName + '">\n'
    
    # Create the master xml file and open for writing
    try:
        devcfg_xml_file = open(filepathname, 'w')
        devcfg_env_file = open(env_filepathname, 'w')
    except IOError:
        errStr = 'Could not create dal master XML file' + filepathname
        devcfg_error(env, errStr)
    
    
    HeaderList = []
    HeaderPath = []
    
    # Add the include files and the xml header tags
    devcfg_xml_file.write('#include "DALPropDef.h"\n')
    devcfg_xml_file.write('#include "DALDeviceId.h"\n')
    devcfg_xml_file.write('#include "dalconfig.h"\n\n')
    devcfg_xml_file.write('<?xml version="1.0"?>\n')
    devcfg_xml_file.write('<dal>\n')
    devcfg_xml_file.write(moduleNameStr)

    # Write all the xml files as #includes in the xml file
    for xml_loc in env[DEVCFG_ENV_DESCRIPTOR]:
        for env_config in env_config_list:
            if env_config in env.get('DEVCFG_XML_DICT').keys() and env.get('DEVCFG_XML_DICT')[env_config] in xml_loc:
                for each_xml_file in xml_loc[env.get('DEVCFG_XML_DICT')[env_config]]:                      #Extract path for the corresponding .pp file
                    path = each_xml_file.path
                    path_pp_file = re.sub(r'\.i$','.pp',path)
                    #get Header information (Header files and their Paths) from the corresponding .pp files
                    pp_xml_file_fp = open(str(path_pp_file),"r")
                    hdr_path_line = ""
                    for line in pp_xml_file_fp:
                        if line.startswith('#'):
                            hdr_path_line = line.split('"')[1]
                            build_root = str(env.get('BUILD_ROOT')).replace('\\', '/')
                            hdr_path_line_new = hdr_path_line.replace('\\', '/')
                            hdr_path, hdr_file = os.path.split(hdr_path_line)
                            if '.h' in hdr_file.lower() and build_root.lower() in hdr_path_line_new.lower() :
                                hdr_file = 'enum_header_path "' + hdr_file + '"\n'
                                if hdr_file not in HeaderList:
                                    HeaderList.append(hdr_file)
                                    devcfg_xml_file.write(hdr_file)
                                if hdr_path not in HeaderPath:
                                    HeaderPath.append(hdr_path)
                                    devcfg_env_file.write(hdr_path + "\n")
                                    
                    pp_xml_file_fp.close()
                    devcfg_xml_file.write(each_xml_file.get_contents())
                    devcfg_xml_file.write('\n')
    
    
    # Write the closing tags
    devcfg_xml_file.write('</module>' + '\n')
    devcfg_xml_file.write('</dal>\n') 
   
    # Close the Master XML file
    devcfg_xml_file.close()
    devcfg_env_file.close()
    #Store Header Paths extracted to environment
    if(update_env == True) :
       dalenv.get('CPPPATH').extend(HeaderPath)
    
  

    return    

def devcfg_create_master_cfile(env, filepathname, env_config_list):
    """
    devcfg_create_master_c_file creates the master dal c file that includes all the 
    processed c files passed in by various scons
    """
    global file_idx
       
    # Create the master xml file and open for writing
    try:
        devcfg_c_file = open(filepathname, 'w')
    except IOError:
        errStr = 'Could not create dal master C file' + filepathname
        devcfg_error(env, errStr)
    
    #Obtain just the file name + ext
    cfname = os.path.basename(filepathname)
    cfname_no_ext = os.path.splitext(cfname)[0]

    # Add the guard
    devcfg_c_file.write ("#ifndef __"+cfname_no_ext+"_H__\n")
    devcfg_c_file.write ("#define __"+cfname_no_ext+"_H__\n\n")
    #devcfg_c_file.write ("#include \"stdio.h\" \n")
    devcfg_c_file.write("#ifndef NULL\n")
    devcfg_c_file.write("#define NULL  0\n")
    devcfg_c_file.write("#endif\n")
    
    wrote_content = 0    
    # Write all the xml files as #includes in the xml file
    #import pdb; pdb.set_trace()
    for cfile_loc in env[DEVCFG_CFILE_DESCRIPTOR]:
        for env_config in env_config_list:
            if env_config in env.get('DEVCFG_XML_DICT').keys() and env.get('DEVCFG_XML_DICT')[env_config] in cfile_loc:
                for cfile in cfile_loc[env.get('DEVCFG_XML_DICT')[env_config]][0] :
                    devcfg_c_file.write('namespace xml_'+ str(file_idx)  + ' { \n ')
                    devcfg_c_file.write(cfile.get_contents())
                    devcfg_c_file.write('}\n')
                    wrote_content = 1;
                    file_idx = file_idx+1
                    
    if wrote_content == 0:
        devcfg_c_file.write ("#include \"DALStdDef.h\" \n")
        devcfg_c_file.write ("#include \"DALSysTypes.h\" \n")
    
    # Write the closing tags
    devcfg_c_file.write ("\n#endif //end of __" + cfname_no_ext + "_H__\n\n")   
        
    # Close the file
    devcfg_c_file.close()
    
    return    

def devcfg_error(env, Info, Info1=None):
    """
    Error handler for devcfg framework. Info can be a string that describes the error and
    Info1 can be any data that needs to be printed along with the error message.
    """
    env.PrintError("DevCfg error found: " + Info)
    env.PrintError(Info1)
    raise Exception(Info)
