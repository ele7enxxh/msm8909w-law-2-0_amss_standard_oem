#@PydevCodeAnalysisIgnore

'''
===============================================================================

 Update Common Info

 GENERAL DESCRIPTION
    This script performs task that are common across all builds, and which
    must be done after all builds are complete.

 Copyright (c) 2011-2012 by QUALCOMM, Incorporated.
 All Rights Reserved.
 QUALCOMM Proprietary/GTDR

-------------------------------------------------------------------------------

  $Header: //source/qcom/qct/tools/meta/00/01/common/build/update_common_info.py#34 $
  $DateTime: 2014/10/17 18:41:57 $

-------------------------------------------------------------------------------

update_common_info.py:

Usage:

      update_common_info.py <mode>

   Examples:

      update_common_info.py --nonhlos  (generates NON_HLOS.bin alone)
      update_common_info.py --hlos     (generates sparse images if rawprogram0.xml exists)
      update_common_info.py            (generates NON-HLOS.bin and sparse images)

===============================================================================
'''

import sys
import os
import os.path
import shutil
import traceback
import re
# Hack until modules deliver split files
import stat
import string
import subprocess
from optparse import OptionParser
from glob import glob

sys.path.append('../tools/meta')
import meta_lib as ml
import meta_log as lg
from xml.etree import ElementTree as et
bin_file_dests = dict()


#-------------------------------------------------------------------------#
#------------------------------ SUBROUTINES ------------------------------#
#-------------------------------------------------------------------------#

class ModemPartitionSizeNotFoundException( Exception ):
   '''An exception class for modem partition size not found.'''
   def __init__(self, list_invalid_files ):
      Exception.__init__(self)


# Function to create gen_build_flavor.cmm file
def create_gen_build_flavor_cmm ():
   common_dir = os.path.dirname(os.path.dirname(__file__))
   destn_dir  = join_paths(os.path.dirname(__file__), "../tools/cmm/gen")
   if not os.path.exists(destn_dir):
      os.makedirs(destn_dir)
   lg.log("update_common_info.py: Calling gen_cmm_data3 to create " + join_paths(destn_dir, "gen_buildflavor.cmm"))
   lg.log_exec( ["python", "../tools/meta/gen_cmm_data3.py", os.path.dirname(common_dir), join_paths(destn_dir, "gen_buildflavor.cmm")] )
# End of create_gen_build_flavor_cmm function


# Function to create partition
def  use_fatgen_toget_partitionsize ():
   lg.log("fatgen_tool path:" + fatgen_tool)
   if os.path.exists( fatgen_tool ):
      lg.log('fatgen tool exists, using it to create new NON-HLOS.bin')
         #get modem partition size from partition.xml
      try:
         modem_partition_size = getModemPartitionSize('partition.xml')
         str_modem_partition_size = str( modem_partition_size )
         #print str_modem_partition_size
         
      except ModemPartitionSizeNotFoundException , ex:
         sys.exit('ModemPartitionSizeNotFoundException raised')
   else:
      sys.exit('fatgen tool does not exist.')
   return str_modem_partition_size
# End of use_fatgen_toget_partitionsize function


# Use cpfatfs tool to add binares
def use_cpfatfs():
   for flavor in bin_file_dests:
      fat_file_dest = os.path.join(bin_file_dests[flavor], 'NON-HLOS.bin')
      
      #if NON-HLOS.bin exists delete it, a new NON-HLOS.bin will be created
      #if NON-HLOS.bin already exists, fatgen.py doesn't create it
      #deleting this file so that it is created each time
      if os.path.exists( fat_file_dest ):
         os.remove( fat_file_dest )
         lg.log("Existing " + fat_file_dest + " has been deleted.") 
      if (fatgen_build):
         # Create NON-HLOS.bin of container size as specified in partition.xml
         modem_partition_size = use_fatgen_toget_partitionsize ()
         lg.log_exec(['python',fatgen_tool,'-f','-s',modem_partition_size,'-n',fat_file_dest],verbose=0)
      else:
         # Copy fat.bin to our root directory.  Assume fat.bin is in same directory as cpfatfs.exe   
         shutil.copy(cpfatfs_path + fat_file_src, fat_file_dest)   
         
   
      os.chmod(fat_file_dest, stat.S_IREAD | stat.S_IWRITE | stat.S_IRGRP | stat.S_IWGRP | stat.S_IROTH | stat.S_IWOTH)
      fat_dest_dir = 'image'
      for fat_file in mi.get_files(attr = 'fat_file', flavor = flavor):
         lg.log('update_common_info.py:' + 'cpfatfs' + ' ' +  fat_file +  ' ' + fat_file_dest,verbose=0)
         lg.log_exec([cpfatfs_tool, fat_file_dest, fat_dest_dir, fat_file],verbose=0)
# End of use_cpfatfs function


# Function to use fat add tool to add binares
def   use_fat_add ():
   for flavor in bin_file_dests:
      fat_file_dest = os.path.join(bin_file_dests[flavor], 'NON-HLOS.bin')

      #if NON-HLOS.bin exists delete it, a new NON-HLOS.bin will be created
      #if NON-HLOS.bin already exists, fatgen.py doesn't create it
      #deleting this file so that it is created each time
      if os.path.exists( fat_file_dest ):
         os.remove( fat_file_dest )
         lg.log("Existing " + fat_file_dest + " has been deleted.")
      if (fatgen_build):
         # Create NON-HLOS.bin of container size as specified in partition.xml
         modem_partition_size = use_fatgen_toget_partitionsize ()
         lg.log_exec(['python',fatgen_tool,'-f','-s',modem_partition_size,'-n',fat_file_dest],verbose=0)
      else:
         # Copy fat.bin to our root directory.  Assume fat.bin is in same directory as cpfatfs.exe   
         shutil.copy(fatadd_path + fat_file_src, fat_file_dest)
               
      os.chmod(fat_file_dest, stat.S_IREAD | stat.S_IWRITE | stat.S_IRGRP | stat.S_IWGRP | stat.S_IROTH | stat.S_IWOTH)
      
      # Add each "fat_file" from contents.xml
      fatFiles = mi.get_files(attr = 'fat_file', flavor=flavor)
      # Create sub directories in NON-HLOS.bin for each "sub_dir" from contents.xml
      subDirFiles = mi.get_files(attr = 'sub_dir', flavor=flavor,abs=False)
      subDirFatFiles = {}
      if len(subDirFiles) > 0:
          for relfile in subDirFiles:
              for absfile in fatFiles:
                  tempabsfile = absfile
                  if on_linux == False:		
                      tempabsfile = absfile.replace('/','\\')
                  if tempabsfile.find(relfile) != -1:
                      subDirFatFiles[absfile] = os.path.dirname(relfile)
                            

      for fat_file in fatFiles:
         fat_dest_dir = 'image'
         print fat_file
         if fat_file in subDirFatFiles:
             fat_dest_dir = os.path.join(fat_dest_dir,subDirFatFiles[fat_file])
         lg.log('update_common_info.py:' + 'fatadd' + ' ' +  fat_file +  ' ' + fat_file_dest,verbose=0)
         lg.log_exec(['python',fatadd_tool,'-n',fat_file_dest,'-f' + fat_file,'-d'+ fat_dest_dir],verbose=0)        
      
      # Add ver_info.txt
      fat_dest_dir = 'verinfo'
      bid_file = os.path.join(os.path.dirname(__file__), 'Ver_Info.txt')
      if os.path.exists(bid_file):
         lg.log('update_common_info.py:' + 'fatadd' + ' ' +  bid_file +  ' ' + fat_file_dest,verbose=0)
         lg.log_exec(['python',fatadd_tool,'-n',fat_file_dest,'-f' + bid_file,'-d'+ fat_dest_dir],verbose=0)
# End of use_fat_add function


# Function to read the value of modem partition size from partition.xml
def getModemPartitionSize(xml):
   '''This function reads the value of modem partition size from partition.xml
   If not found it raises an exception
   '''
   modem_partition_size = 0

   inputXmlTree = et.parse(xml)
   root_element = inputXmlTree.getroot()

   #get the "physical_partition" element
   list_physical_partition_elements = root_element.findall( 'physical_partition')

   for physical_partition_el in list_physical_partition_elements:
      list_partition = physical_partition_el.findall('partition')
      for list_partition_el in list_partition:
         if( list_partition_el.get('label') == 'modem' ):
            modem_partition_size = int( list_partition_el.get('size_in_kb') )
            modem_partition_size = ( modem_partition_size + 1023 ) / 1024 #change the size from KB to MB and ensure that if necessary, 1 MB extra is allocated
            return modem_partition_size
   raise ModemPartitionSizeNotFoundException()
# End of function getModemPartitionSize()


# Function to implement join_paths with "None"
def join_paths(*arg):
   arg = [x for x in arg if x is not None]
   return os.path.join(*arg)
# End of function join_paths


# Local version to update var values in given text
def update_var_values (text, var, val):
   # Search text to look for instances of variable.  
   # Variables are surrounded by $[var_name], and may already
   # contain a value, such as: $[var_name:value].
   # Currently, this pattern can only match one var/value pair per string.
   pattern = "\A(.*\$\[%s*)(?::[^}]*)?(\].*)\Z" % var
   re_obj = re.compile(pattern)
   
   # Look for "${" first, since this is much faster than a regex search
   try:
      if ((text != None) and (text.count('$[') >= 1)):
         # Look for a matching variable in our string
         match_obj = re_obj.search(text)
         if match_obj:
            groups = match_obj.groups()
            # If val is None (eg flavor) replace with empty
            if val:
               new_text = groups[0] + ':' + val + groups[1]
            else:
               new_text = groups[0] + ':' + groups[1]
            return new_text
      return text
   except:
      return text
# End of function update_var_values()


# Local version to clean vars in given text
def clean_vars (text):
   #Clean variables from the specified text
   if text.count('$['):
      pattern = "\A(.*)\$\[[^:]*(?::(.*))?\](.*)\Z"
      re_obj = re.compile(pattern)
      match_obj = re_obj.search(text)
      while match_obj:
         text = ''.join(match_obj.groups())
         match_obj = re_obj.search(text)
   return text
# End of function clean_vars()


# Function to process and get updated params for a tool execution
def evaluate_params (step_dict_flavor, params = None):
   if params is None:
      params = step_dict_flavor['params']    # Used to pass different value in recursion
   params_split = params.split()
   for i in range(len(params_split)):
      if params_split[i].startswith('@'):
         try:
            # Try to replace its value from the dictionary
            params_split[i] = evaluate_params (step_dict_flavor, params = step_dict_flavor[params_split[i][1:]])
         except KeyError:
            # If not found evaluate for a regular varialbe
            params_split[i] = evaluate_params (step_dict_flavor, params = str(eval(params_split[i][1:])))
   new_params_split = []
   for i in range(len(params_split)):
      if params_split[i] == '+':
         if '/' in params_split[i-1] or '\\' in params_split[i-1]:
            new_params_split[-1] = join_paths(new_params_split[-1],params_split[i+1])
         else:
            new_params_split[-1] = new_params_split[-1] + params_split[i+1]
      elif params_split[i-1] == '+':
         pass
      else:
         new_params_split.append(params_split[i])
   params = ' '.join(new_params_split)
   # Replace the escaped @ and return
   return clean_vars(params.replace('\\','/').replace('/@','@'))    # Backslash missing in log_exec cmd, during split?
# End of function get_file_params()


# Function to make a copy of step with updated flavor values
def create_flavor_dict (step_dict, flavor):
   # Update flavor with current flavor value in step
   step_dict_flavor = {}
   step_dict_flavor = step_dict.copy()
   contains_flavor_flag = False
   for key in step_dict_flavor:
      if ('$[flavor' in step_dict_flavor[key]):
         contains_flavor_flag = True  # To check whether to loop on flavors while executing
      step_dict_flavor[key] = update_var_values(step_dict[key], "flavor", flavor)
   return (step_dict_flavor, contains_flavor_flag)
# End of function create_flavor_dict()


# Function to process the standard keys in each step before running
def preprocess_step (step_dict_flavor, flavor):
   # If step is "exec" process the tool name to get path
   if ('tool_name' in step_dict_flavor):
      tool_name = step_dict['tool_name']
      tool_build = mi.get_tool_build(tool_name, 'workflow')
      if tool_build:
         # If valid build exists, get the full path to tool
         tool_pfn = join_paths(mi.get_build_path(tool_build), 
                               mi.get_tool_path(tool_name, 'workflow'), 
                               tool_name)
         step_dict_flavor['tool_name'] = tool_pfn
      else:
         step_dict_flavor['tool_name'] = None
         # The tool for this exec step is not present. Need not proceed...
         return
   
   # If destination directory is specified, create if not found
   if ('destn_dir' in step_dict_flavor):
      destn_dir = clean_vars(step_dict_flavor['destn_dir'])
      if not os.path.exists(destn_dir):
         lg.log("update_common_info.py: Creating directory " + destn_dir)
         os.makedirs(destn_dir)
      # After evaluating map it back to the dictionary
      step_dict_flavor['destn_dir'] = destn_dir
   
   # If execution directory is specified, create and cd to it
   if ('exec_dir' in step_dict_flavor):
      exec_dir = clean_vars(step_dict_flavor['exec_dir'])
      if not os.path.exists(exec_dir):
         os.makedirs(exec_dir)
      # Save the current path and change to exec_dir
      start_dir = os.getcwd()
      step_dict_flavor['start_dir'] = start_dir
      os.chdir(exec_dir)
      step_dict_flavor['exec_dir'] = exec_dir
      lg.log("Changed working directory to " + step_dict_flavor['exec_dir']);
      
   # If root directory is specified, get the root of that build
   if ('root_dir' in step_dict_flavor):
      if ('$[build' in step_dict_flavor['root_dir']):
         build = clean_vars(step_dict_flavor['root_dir'])
         root_dir = mi.get_build_path(build)
         step_dict_flavor['root_dir'] = root_dir
      
   # Get list of source files based on the given key
   src_files = []
   if ('src_files' in step_dict_flavor):
      src_files = step_dict_flavor['src_files']
      if '$' in src_files:     
         # If source_files is listed as var:value pair, use get_files and filter with provided attr
         # This will return a list of files filtered with flavor and attribute
         attr = clean_vars(src_files)
         src_files = mi.get_files(attr=attr, flavor=flavor)
      else:
         # Treat is just as a file or a directory path and make it a list
         src_files = [clean_vars(src_files)]
      step_dict_flavor['src_files'] = src_files
   src_file_vars = []
   if ('src_file_vars' in step_dict_flavor):
      # If source is listed as file_vars, use get_file_vars
      # This will return a dictionary with var:[file] pairs
      attr = clean_vars(step_dict_flavor['src_file_vars'])
      src_file_vars = mi.get_file_vars(attr, flavor = flavor)
      step_dict_flavor['src_file_vars'] = src_file_vars
# End of preprocess_step()


# Functions to run each step in workflow based on its type
def workflow_copy(step_dict):
   # Execution step should specify the tool name to run
   if not ('destn_dir' in step_dict or 'src_files' in step_dict):
      lg.log("update_common_info.py: Invalid copy step! Source Files or Destination not specified")
   else:
      for flavor in flavors:
         # Make a copy of step_dict for current flavor
         (step_dict_flavor, flavor_flag) = create_flavor_dict(step_dict, flavor)
         
         # Preprocess the standard items in the step
         preprocess_step(step_dict_flavor, flavor)
         
         lg.log("update_common_info: --------------------------------------------------")
         lg.log("update_common_info:  Starting copy")
         
         if ('src_files' in step_dict_flavor.keys()):
            src_files = step_dict_flavor['src_files']
            destn_dir = step_dict_flavor['destn_dir']
            # Copy each file in the list
            for src_file in src_files:
               # For wildcards
               for file in glob(src_file):
                  lg.log("Copying " + file + " to " + destn_dir)
                  shutil.copy(file, destn_dir)
            lg.log("update_common_info: --------------------------------------------------\n")
      
         # Don't have to repeat if the step doesn't contain flavor info
         if (len(flavors) > 0) and (not flavor_flag):
            break

def workflow_delete(step_dict):
   for flavor in flavors:
      # Make a copy of step_dict for current flavor
      (step_dict_flavor, flavor_flag) = create_flavor_dict(step_dict, flavor)

      # Preprocess the standard items in the step
      preprocess_step(step_dict_flavor, flavor)
      
      # Once destination path is processed, try deleting it
      for key in step_dict_flavor:
         # Delete step should contain only keys of destinations to delete
         destn = clean_vars(step_dict_flavor[key])
         try:
            if os.path.isdir(destn):
               shutil.rmtree(destn)
               lg.log("update_common_info.py: Deleted the directory " + destn);
            else:
               os.remove(destn)
               lg.log("update_common_info.py: Deleted the file " + destn);
         except:
            lg.log("update_common_info.py: Unable to delete " + destn);

      # Don't have to repeat if the step doesn't contain flavor info
      if (len(flavors) > 0) and (not flavor_flag):
         break

def workflow_exec(step_dict):
   # Execution step should specify the tool name to run
   if not ('tool_name' in step_dict or 'params' in step_dict):
      lg.log("update_common_info.py: Invalid exec step! Tool name or command not specified")
   else:
      for flavor in flavors:
         # Make a copy of step_dict for current flavor
         (step_dict_flavor, flavor_flag) = create_flavor_dict(step_dict, flavor)
         
         # Preprocess the standard items in the step
         preprocess_step(step_dict_flavor, flavor)
         
         # After preprocess check if the tool is available and execute
         if step_dict_flavor['tool_name']:
            lg.log("update_common_info: --------------------------------------------------")
            lg.log("update_common_info:  Calling " + step_dict_flavor['tool_name'].split('/')[-1] + " " + (flavor or ""))
            
            # Check if the step has file_vars or files and loop accordingly
            if ('src_file_vars' in step_dict_flavor.keys()) and (len(step_dict_flavor['src_file_vars']) > 0):
               src_file_vars = step_dict_flavor['src_file_vars']
               for key in src_file_vars.keys():
                  step_dict_flavor['src_file_var'] = key
                  for file in src_file_vars[key]:
                     step_dict_flavor['src_file'] = file
                     params = evaluate_params(step_dict_flavor)
                     lg.log("update_common_info: --------------------------------------------------\n")
                     lg.log_exec(params)
            elif ('src_files' in step_dict_flavor.keys()) and (len(step_dict_flavor['src_files']) > 0):
               src_files = step_dict_flavor['src_files']
               for src_file in src_files:
                  step_dict_flavor['src_file'] = src_file
                  params = evaluate_params(step_dict_flavor)
                  lg.log("update_common_info: --------------------------------------------------\n")
                  lg.log_exec(params)
            else:
               params = evaluate_params(step_dict_flavor)
               lg.log("update_common_info: --------------------------------------------------\n")
               lg.log_exec(params)
            
            if ('start_dir' in step_dict_flavor.keys()):
               # cd'd to exec_dir in preprocessing, so revert back
               os.chdir(step_dict_flavor['start_dir'])
               del step_dict_flavor['start_dir']
         
         # Don't have to repeat if the step doesn't contain flavor info
         if (len(flavors) > 0) and (not flavor_flag):
            break
# End of workflow type functions


#-------------------------------------------------------------------------#
#--------------------------------- MAIN ----------------------------------#
#-------------------------------------------------------------------------#

#---------------------------------------------------------#
# Option Parser                                           #
#---------------------------------------------------------#
parser = OptionParser()
parser.add_option("--cmm",action="store_true",dest="cmm",help="Create cmm script",default=False)
parser.add_option("--nonhlos",action="store_true",dest="non_hlos",help="NON HLOS",default=False)
parser.add_option("--hlos",action="store_true",dest="hlos",help="HLOS",default=False)
parser.add_option("--wflow_filter",action="store",type="string",dest="wflow_filter",help="Workflow Filter",default=None)
(options, args) = parser.parse_args()

# If not arguments are passed, then set all to True
# Can all options be checked in single shot?
if not (options.cmm and options.non_hlos and options.hlos):
   options.cmm = True
   options.non_hlos = True
   options.hlos = True
workflow_filter_list = []
if options.wflow_filter:
   workflow_filter_list = options.wflow_filter.strip().split(',')

#---------------------------------------------------------#
# Get bearings                                            #
#---------------------------------------------------------#
on_linux = sys.platform.startswith("linux")

#---------------------------------------------------------#
# Initialize logger and print some diagnostic info        #
#---------------------------------------------------------#
lg = lg.Logger('update_common')
lg.log("update_common_info:Platform is:" + sys.platform,verbose=0)
lg.log("update_common_info:Python Version is:" + sys.version,verbose=0)
lg.log("update_common_info:Current directory is:" + os.getcwd(),verbose=0)

#---------------------------------------------------------#
# Load the Meta-Info file                                 #
#---------------------------------------------------------#
lg.log("update_common_info:Loading the Meta-Info file",verbose=0)
import meta_lib as ml
mi = ml.meta_info(logger=lg)

#---------------------------------------------------------#
# Get flavor information                                  #
#---------------------------------------------------------#
flavors = mi.get_product_flavors()
# Returns empty list [] if no flavors found
if len(flavors) == 0:
   flavors.append(None)
else:
   lg.log('flavors found')

#---------------------------------------------------------#
# Setup our bin directory                                 #
#---------------------------------------------------------#
for flavor in flavors:
   path = join_paths('./bin/', flavor)
   # For no flavors will just create ./bin/
   bin_file_dests[flavor] = path
   try:
      os.makedirs(path)
   except:
      # Ignore the exception that gets raised if the directory already exists.
      pass

#---------------------------------------------------------#
# Creating gen_buildflavor.cmm file                       #
#---------------------------------------------------------#
if options.cmm:
   create_gen_build_flavor_cmm()

#---------------------------------------------------------#
# Run PIL-Splitter                                        #
#---------------------------------------------------------#
pil_splitter_tool_name = 'pil-splitter.py'
# From contents.xml get the build in which tool is present
pil_splitter_build  = mi.get_tool_build(pil_splitter_tool_name)

# Run only if the tool is present
if pil_splitter_build:
   # Get the full path for tool
   pil_splitter = os.path.join(mi.get_build_path(pil_splitter_build),
                               mi.get_tool_path(pil_splitter_tool_name),
                               pil_splitter_tool_name)
   pil_splitter = pil_splitter.replace('\\', '/')
   lg.log("pil-splitter path: "+ pil_splitter)

   dest_dir  = '../tools/misc'
   if os.path.exists(pil_splitter):
      # Copy it into the meta-build, and run it from there.
      if not os.path.exists(dest_dir):
         os.mkdir(dest_dir)
      lg.log("Copying pil-splitter from host build.")
      shutil.copy(pil_splitter, dest_dir)

   # Always use local copy of pil_splitter
   pil_splitter = os.path.join (dest_dir, pil_splitter_tool_name)
   
   # Loop through each flavor and run the tool. Takes "None" when no flavor is present
   for flavor in flavors:
      # Pil-Splitter requires a source file, and target prefix as parameters
      # The target prefix can include path information.  Files in contents.xml
      # that need to be split will have a 'pil-split' attribute set to the
      # prefix to use for that file.  When we call get_file_vars, it will
      # return a dictionary mapping the prefix to the file to be split.
      # We must then prepend any path information to the prefix.
      pil_split_bins_dir = join_paths('bin', flavor, 'pil_split_bins')
      if flavor:
         lg.log('Processing the flavor ' + flavor)
      lg.log('pil_split_bins_dir: ' + pil_split_bins_dir)
      if not os.path.exists(pil_split_bins_dir):
         os.makedirs(pil_split_bins_dir)
      pil_splitter_files = mi.get_file_vars(attr = 'pil_split', flavor = flavor)
       
      for prefix in pil_splitter_files.keys():
         if not pil_splitter_files[prefix]:
            lg.log('Source file for prefix '+prefix+' is Null')
            sys.exit('Unable to call pil-splitter on prefix '+prefix)
         prefix_path = os.path.join(pil_split_bins_dir, prefix)
         # There should only be one source file per prefix
         pil_split_src = pil_splitter_files[prefix][0]
         lg.log('update_common_info: Calling pil-splitter on ' + pil_split_src)
         lg.log_exec(['python', pil_splitter, pil_split_src, prefix_path])


#---------------------------------------------------------#
# NON-HLOS Operations                                     #
#---------------------------------------------------------#
if options.non_hlos:
   #---------------------------------------------------------#
   # Write the FAT image                                     #
   #---------------------------------------------------------#
   lg.log("update_common_info:Writing FAT images")

   fat_file_src  = 'fat.bin'
   fatfiles = []
   # check for fat file exist or not.
   # if flavors exists, we pass one flavor element to mi.get_files() 
   # this is done just to avoid DataIntegrity exception
   # because we are only checking if there is atleast one file type element with 'fat_file' attribute
   fatfiles = mi.get_files(attr = 'fat_file', flavor = flavors[0] )
   if len(fatfiles) > 0:
      fatbin_exists = False
      fatadd_build = None
      fatgen_build = None
      cpfatfs_build = None  
      try:
         #cheking if fat add tool exists or not
         fatadd_build  = mi.get_tool_build('fatadd')
         fatadd_path   = mi.get_build_path(fatadd_build)
         fatadd_path  += mi.get_tool_path('fatadd')
         fatadd_tool  =  fatadd_path + 'fatadd.py'
         temp_path = os.path.abspath( os.path.join( fatadd_path , 'fat.bin' ) )
         lg.log("fat.bin location: " + temp_path)
         if os.path.exists( temp_path ):
            fatbin_exists = True
      except:
         pass

      try:
         #checking if fatgen tool exists or not
         fatgen_build  = mi.get_tool_build('fatgen')
         fatgen_path   = mi.get_build_path( fatgen_build )
         fatgen_path  += mi.get_tool_path('fatgen')
         fatgen_tool  =  os.path.join(fatgen_path, 'fatgen.py')
         lg.log("fatgen tool location: " + fatgen_tool)
      except:
         pass 

      try:
         #checking if cpfatfs tool exists or not
         cpfatfs_build  = mi.get_tool_build('cpfatfs')
         if cpfatfs_build:
            cpfatfs_path   = mi.get_build_path(cpfatfs_build)
            cpfatfs_path  += mi.get_tool_path('cpfatfs')
            cpfatfs_tool   = cpfatfs_path + 'cpfatfs'
            temp_path = os.path.abspath( os.path.join( cpfatfs_path , 'fat.bin' ) )
            lg.log("cpfatfs tool location: " + temp_path)
            if os.path.exists( temp_path ):
               fatbin_exists = True
            if not on_linux:
               cpfatfs_tool += '.exe'
      except:
         pass 
  
      if(fatgen_build or fatbin_exists) and (fatadd_build or cpfatfs_build):
         if (fatadd_build):
            use_fat_add ()
         elif (cpfatfs_build):
            use_cpfatfs ()  
         else:
            lg.log('Warning: No tool found to add component binaries to NON-HLOS.bin.')
      else:
         lg.log('Warning: Unable to generate / process NON-HLOS.bin.')
      
      
   #---------------------------------------------------------#
   # Encode NON-HLOS.build                                   #
   #---------------------------------------------------------#
   try :
      ptool = mi.get_tool_build('enc_img')
   except :
      pass
      ptool = ''
   if ptool:  # Returns a build
      run_tool = True
      # Get the root path to the build
      enc_img_build_root = mi.get_build_path('tz')
      # Get rel path to tool
      enc_img_tool_path = mi.get_tool_path('enc_img')
      # Start processing parameters
      params = mi.get_tool_params('enc_img')
      for i in range(0, len(params)):
         if params[i].startswith("--tools_dir="):
            params[i] = params[i] % enc_img_build_root
      
         elif params[i].startswith("--keys="):
            key_dir = params[i].split('=')[1]
            if not key_dir:
               run_tool = False
   
      if run_tool:
         lg.log("Running Encoder on NON-HLOS.bin")
         cmd = ['python', os.path.join(enc_img_build_root, enc_img_tool_path, "enc_img.py"), "--ip_file=NON-HLOS.bin", "--op_file=NON-HLOS.enc"] + params
         lg.log_exec(cmd)
   
   #---------------------------------------------------------#
   # Process partition.xml                                   #
   # Only run ptool at build time.  msp is run at download   #
   # time.                                                   #
   #---------------------------------------------------------#

   lg.log("update_common_info.py: Processing partition.xml",verbose=0)
   # Find the tools
   ptool_build  = mi.get_tool_build('ptool')
   if ptool_build:
      ptool_path   = mi.get_build_path(ptool_build)
      ptool_path  += mi.get_tool_path('ptool')
      ptool_path   = os.path.abspath(ptool_path)
      ptool_tool   = ptool_path + '/ptool.py'
      ptool_params = mi.get_tool_params('ptool')

      # Define our file names
      partition_xml   = 'partition.xml'
      rawprogram0_xml = 'rawprogram0.xml'

      if ptool_params:
         for i in range(len(ptool_params)):
            ptool_params[i] = ptool_params[i] % partition_xml   # Replace %s with path\name of partition.xml file
            ptool_params[i] = ptool_params[i].split()             # Split the command line into a list of arguments
      else:
         ptool_params = [['-x', partition_xml]]

      # Execute the commands
      for params in ptool_params:
         lg.log('update_common_info:' + ' ' +  'ptool.py'+ ' ' + partition_xml + ' ' + 'gpt',verbose=0)
         lg.log_exec(['python', ptool_tool] + params,verbose=0)

   nand_build  = mi.get_tool_build('nand_mbn_generator')
   if nand_build:
      # Define our file names
      partition_xml   = 'partition_nand.xml'
      rawprogram0_xml = 'rawprogram0.xml'
      partition_mbn = 'partition.mbn'
      nand_path   = mi.get_build_path(nand_build)
      nand_path  += mi.get_tool_path('nand_mbn_generator')
      nand_path   = os.path.abspath(nand_path)
      if os.path.exists(nand_path):
         nand_tool   = nand_path + '/nand_mbn_generator.py'
         nand_partition_xml=nand_path+'/partition_nand.xml'
         print nand_partition_xml
         lg.log('update_common_info:' + ' ' +  'nand_mbn_generator.py'+ ' ' + partition_xml + ' ' + 'gpt',verbose=0)
         lg.log_exec(['python', nand_tool,partition_xml,partition_mbn],verbose=0)

if options.hlos:

#---------------------------------------------------------#
# Run checksparse, if available.                          #
#---------------------------------------------------------#

   start_dir = os.getcwd()
   rawprogram_unsparse_xml = 'rawprogram_unsparse.xml'
   raw_program_xml = os.path.join(start_dir, 'rawprogram0.xml')
   if os.path.exists(raw_program_xml):
      checksparse_build  = mi.get_tool_build('checksparse')
      if checksparse_build:
         for flavor in bin_file_dests:
            sparse_image_dest = os.path.join(bin_file_dests[flavor], 'sparse_images')
            sparse_image_info = mi.get_file_vars(attr='sparse_image_path')
            if sparse_image_info and len(sparse_image_info['true']):
               # There should only be one file, and the attr value is 'true'
               sparse_image_path = os.path.dirname(sparse_image_info['true'][0])
               if os.path.isdir(sparse_image_path):
                  checksparse_path   = mi.get_build_path(checksparse_build)
                  checksparse_path  += mi.get_tool_path('checksparse')
                  checksparse_path   = os.path.abspath(checksparse_path)
                  checksparse_tool   = checksparse_path + '/checksparse.py'
               if not os.path.exists(sparse_image_dest):
                  os.makedirs(sparse_image_dest,mode = 0755)
               os.chdir(sparse_image_dest)
               lg.log("update_common_info:Executing checksparse tool",verbose=0)
               lg.log_exec(['python', checksparse_tool,'-i',raw_program_xml,'-s',sparse_image_path,'-o',rawprogram_unsparse_xml],verbose=0)
               os.chdir(start_dir)
      else:
         lg.log("update_common_info: Checksparse tool does not exist. Could not create sparse images.")
   else:
      lg.log("update_common_info: " + raw_program_xml + " path does not exist. Could not create sparse images.")


#-------------------------------------------------------------------------#
# WORKFLOW                                                                #
#-------------------------------------------------------------------------#

# Get the workflow block from contents.xml
workflow_elem = mi.find_root_element('workflow')

# Define a dictionary mapping for type of steps in workflow
run_step = {
   'copy': workflow_copy,
   'exec': workflow_exec,
   'delete': workflow_delete
}

# Loop through each step in the workflow if found
if workflow_elem is not None:
   for step_elem in workflow_elem:
      # Each step should have a type
      if 'type' in step_elem.attrib:
         if ('filter' not in step_elem.attrib) or (len(workflow_filter_list) == 0) or (step_elem.attrib['filter'] in workflow_filter_list):
            step_dict = ml.XmlToDict(step_elem)
            step_func = run_step[step_elem.attrib['type']]
            step_func(step_dict)

lg.log("update_common_info.py:============ UPDATE COMMON INFO COMPLETE====================",verbose=0)
