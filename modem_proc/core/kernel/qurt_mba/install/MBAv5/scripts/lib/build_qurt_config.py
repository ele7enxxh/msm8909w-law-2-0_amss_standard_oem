"""
Processing of the machine XML element.
"""

import os, sys
import subprocess
import re

def build_qurt_config(dict, build_flags, asic, tools_path, object_path):

    pathname = os.path.dirname(sys.argv[0])    
    
    f_temp = open(pathname+'/Input/cust_config.c', 'r')
    obj_path = object_path
    if os.path.isdir(object_path) == True:
       src_file = object_path+"/qurt_config.c"
       obj_file = object_path+"/qurt_config.o"
       f_cust = open(src_file, 'w')
    else:
       if os.path.dirname(object_path) == '':
         src_file = os.path.dirname(object_path) + "qurt_config.c"
       else:  
         src_file = os.path.dirname(object_path) + "/qurt_config.c"
       obj_file = object_path
       f_cust = open(src_file, 'w')
    
    for line in f_temp:
      for k,v in dict.iteritems():
        if k.search(line) != None:
          line = re.sub(k, v, line)

      f_cust.write(line)
          
    f_cust.close()
    
    if object_path == None:
      object_path = ""
    if build_flags == None:
      build_flags = ""

    if tools_path != None:
      if (os.name == 'posix') :
        GCC = "\"" + tools_path + "\"" + "/gnu/bin/qdsp6-gcc"
      else:
        GCC = "\"" + tools_path.replace("/", "\\") + "\\gnu\\bin\\qdsp6-gcc" + "\""
    else:
      GCC = "qdsp6-gcc"     
    
    command_line = GCC + " -g -nostdinc -O2 -m" + asic + " -G0" + " " + build_flags + " -c -o " + obj_file + " " + src_file

    print command_line
    import shlex
    gcc_args = shlex.split(command_line)

    subprocess.Popen(gcc_args).wait()


