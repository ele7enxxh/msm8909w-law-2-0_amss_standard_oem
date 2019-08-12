'''
===============================================================================

 Update Meta

 GENERAL DESCRIPTION
    This is an internal script used by build.py and other tools
    to parse the build pairs for meta_info, generate goto scripts and 
    validate file paths in contents.xml

 Copyright (c) 2011-2012 by QUALCOMM, Incorporated.
 All Rights Reserved.
 QUALCOMM Proprietary/GTDR

-------------------------------------------------------------------------------

  $Header: //source/qcom/qct/tools/meta/00/01/common/tools/meta/update_meta.py#10 $
  $DateTime: 2014/06/20 18:30:47 $

===============================================================================
'''

import sys
import os
import os.path
import meta_lib as ml
from glob import glob

def update_meta (lg_obj, pairs):
   #---------------------------------------------------------#
   # Print some diagnostic info to the log                   #
   #---------------------------------------------------------#
   import meta_lib as ml
   lg_obj.log("update_meta.py: Platform is " + sys.platform)
   lg_obj.log("update_meta.py: Python Version is " +  sys.version)
   lg_obj.log("update_meta.py: Current directory is " + os.getcwd())
   lg_obj.log("update_meta.py: Parameters are ")
   for pair in pairs:
      lg_obj.log("update_meta.py: " +  "   " + pair.strip())
   
   #---------------------------------------------------------#
   # Update/Create the Meta-Info file                        #
   #---------------------------------------------------------#
   mi = ml.meta_info(logger = lg_obj )
   lg_obj.log_highlight("UPDATE META - Processing Parameters")
   lg_obj.log("update_meta.py: Updating/Creating Meta-Info file")
   builds = []   # Save this info for later

   for p in pairs:
      lg_obj.log("update_meta.py: Processing " + p)
      if ('=' in p):
         var, val = p.strip().split('=')
         if "--flavors" in var:
            selected_pf_list = val.strip().split(',')
            mi.filter_product_flavors(selected_pf_list)
         else:
            mi.update_var_values(var, val)
      elif (':' in p):
         tag, loc = p.strip().split(':',1)
         if tag_not_there(tag, mi):
            lg_obj.log_error('Tag "' + tag + '" not found')
            return 2  
         builds.append(tag)  # Save this info for later
         if mi.update_build_info(tag, loc):
            lg_obj.log_error('Build "' + loc + '" not found')
            return 3  
         if tag == 'common' :
            lg_obj.log("update_meta.py: Updating ProductLine using "+ p + " build info")
            mi.update_pl_from_buildid(loc)
      else:
         lg_obj.log_error('Please rerun with the format: modem:M8960AAAAAGAAM0000 var=value')
         return 1
   mi.save()
 
   #---------------------------------------------------------#
   # Create goto scripts                                     #
   #---------------------------------------------------------#
   
   lg_obj.log_highlight("UPDATE META - Creating goto scripts")

   # Define template for goto scripts
   goto_template = '''#! /usr/bin/python

import sys
import os
import subprocess

sys.path.append(os.path.join(os.path.dirname(__file__), 'common/tools/meta'))
import meta_lib as ml
mi = ml.meta_info()
path = mi.get_build_path('%s')

on_linux = sys.platform.startswith('linux')

if on_linux:
   if path is not None:
      try:
         subprocess.Popen(['gnome-open', path]).wait()
      except:
         try:
            subprocess.Popen(['xdg-open', path]).wait()
         except:
            ErrorMessageBox("Command not supported by Linux. Only Gnome/xdg are supported")   
   else:
      ErrorMessageBox('goto_%s.py', 'Cannot find path for %s.')

else:
   if path is not None:
      subprocess.Popen(['start', path], shell=True).wait()
   else:
      ErrorMessageBox('goto_%s.py', 'Cannot find path for %s.')

'''

   for tag in mi.get_build_list():
      # Don't process the current build
      if tag != 'common':
         path      = mi.get_build_path(tag)
         image_dir = mi.get_image_dir(tag)
         # Don't process builds that don't have image directories
         # (This tests for builds that haven't been updated)
         lg_obj.log("update_meta.py: goto " + tag);
         if path and image_dir and os.path.exists(path + image_dir):
            goto_script = open(os.path.join(os.path.dirname(__file__), '../../../goto_' + tag + '.py'), 'w')
            goto_script.write(goto_template % (tag, tag, tag, tag, tag))
            goto_script.close()
   
   lg_obj.log_highlight("UPDATE META COMPLETE")
   return 0


def validate_filepaths( mi, lg_obj ):
   '''
   This function is the entry point to test the validity of files mentioned in contents.xml in the elements like <download_file> , <file_ref>, <dload_boot_image>
   '''

   lg_obj.log_underline("update_meta.py: Validating File paths in contents.xml")
   # initialize the list for invalid files and master file list
   list_invalid_files = []   
   # get the list of all files to be validated   
   master_file_list = mi.get_files( expand_wildcards = False ) # False corresponds to no wild card expansion is to be done
   # get the list of files that have ignore attribute
   ignored_file_list = mi.get_files( attr='ignore', expand_wildcards = False ) 

   lg_obj.log("update_meta.py: Total number of file paths found in contents.xml = " + str( len( master_file_list ) ))

   #remove ignored files from the master_file_list
   file_set_to_check = set(master_file_list) - set(ignored_file_list) # convert lists to sets and subtract to remove ignored files 
      
   #iterate through the file_set_to_check and process each file
   for filepath in file_set_to_check:
      # as the file path may have a wild character, check using glob if there is a least one file which matches the pattern specified by the filepath
      matching_file_list = glob( filepath )
      if not matching_file_list:
         list_invalid_files.append( filepath ) #no match for filename pattern: treating this as invalid file, add path to list of invalid files

   #done processing all files

   #if the list of invalid files has atleast one entry, raise an exception
   if( len( list_invalid_files ) > 0 ):
      raise ml.InvalidFilePathException( list_invalid_files  )
   else:
      lg_obj.log("update_meta.py: file path validation passed.")

#end of function validate_filepaths()

def tag_not_there(tag, mi):
   for t in mi.get_build_list():
      if t==tag:
         return False
   return True
