'''
===============================================================================

 Gen CMM Data 2

 GENERAL DESCRIPTION
    This script generates a CMM script for consumption by Trace32.

 Copyright (c) 2011-2011 by QUALCOMM, Incorporated.
 All Rights Reserved.
 QUALCOMM Proprietary/GTDR

-------------------------------------------------------------------------------

  $Header: //source/qcom/qct/tools/meta/00/01/common/tools/meta/gen_cmm_data2.py#1 $
  $DateTime: 2012/07/05 14:44:45 $

===============================================================================

gen_cmm_data2.py:

Usage:

      gen_cmm_data2 <temp_cmm_script_file_name> <product_flavor>

   Examples:

      gen_cmm_data2  c:\\temp\\temp2.cmm mmode_flash

Description:

gen_cmm_data1.py and gen_cmm_data2.py work together.  gen_cmm_data1.py should be
called from a CMM script to generate a temporary CMM script that will define variables
containing the root paths to each of the component builds, and, if defined in the
contents.xml file, a list of flavors for the builds.  gen_cmm_data2.py receives one
of the flavors from gen_cmm_data1.py and returns a list of file paths for that flavor.

===============================================================================
'''

import sys
import os
import os.path
import meta_log as lg

if len(sys.argv) in [2,3]:
   temp_file = sys.argv[1]
   if len(sys.argv) == 3:
      product_flavor = sys.argv[2]
   else:
      product_flavor = None
else:
   print "Error: Invalid number of parameters."
   print __doc__
   sys.exit()

dst_path = os.path.dirname(temp_file)
if (dst_path != '' and    # '' implies current directory, and is allowed
    not os.path.isdir(dst_path)):
   print "Can't find destination directory:", dst_path
   print __doc__
   sys.exit(1)
logfile = os.path.join(dst_path,'gen_cmm_data2')

#delete EXCEPTION_gen_cmm_data2.txt and SUCCESS_gen_cmm_data2.txt, if they exists
exception_file_path = os.path.join( dst_path,'EXCEPTION_gen_cmm_data2.txt' )
if os.path.exists( exception_file_path ):
   os.remove( exception_file_path )

success_file_path = os.path.join( dst_path,'SUCCESS.txt' )
if os.path.exists( success_file_path ):
   os.remove( success_file_path )

lg = lg.Logger(logfile, save_old_log_file=False)

lg.log("Platform is:" + sys.platform)
lg.log("Python Version is:" + sys.version)
lg.log("Current directory is:" + os.getcwd())
lg.log("Args are:" + str(sys.argv))

cmm_script_header = ''
cmm_tag_template = '''GLOBAL &%s
&%s="%s"
'''
cmm_script_trailer = '''ENDDO'''
exception = False
sys.path.append(os.path.dirname(__file__))
import meta_lib as ml

mi = ml.meta_info(logger=lg)

product_flavors = mi.get_product_flavors()
if product_flavor and product_flavor not in mi.get_product_flavors():
   lg.log("Error: " + product_flavor + " is not a valid Product Flavor.  Valid Product Flavors are:\n" + str(product_flavors) + "\n")
   
   #create an exception file to indicate exception   
   exception_FILE = open( exception_file_path,'w')
   exception_str = "Exception: " + product_flavor + " is not a valid product flavor."
   exception_FILE.write( exception_str )
   exception_FILE.close()
   
   raise RuntimeError('Invalid Flavor')

cmm_script = open(temp_file, 'w')
cmm_script.write(cmm_script_header)

file_vars = mi.get_file_vars(attr = 'cmm_file_var', flavor = product_flavor, abs = False)
for var_name in file_vars:
   lg.log("Processing CMM Var: " + var_name)
   file_list = file_vars[var_name]
   cmm_file_array = ''
   count = 0
   for file in file_list:
      count += 1
      cmm_file_array += file + ';'
   if count == 1:
      cmm_file_array = cmm_file_array[:-1]
   cmm_script.write(cmm_tag_template % (var_name, var_name, cmm_file_array))
cmm_script.close ()

#create an empty success file to indicate success
success_FILE = open( success_file_path,'w')
success_FILE.close()