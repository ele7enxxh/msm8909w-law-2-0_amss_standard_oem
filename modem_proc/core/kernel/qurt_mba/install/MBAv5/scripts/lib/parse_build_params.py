import os, sys
import re

def parse_build_params():
   pathname = os.path.dirname(sys.argv[0]) 
   f_build_param = open(pathname+"/Input/build_params.txt", "r")
   asic_pattern = re.compile("Q6VERSION=")
   config_flags_pattern = re.compile("BUILD_CONFIG_FLAGS=")
   return_pattern = re.compile("\n")

   for line in f_build_param:
     if asic_pattern.search(line) != None:
       asic = re.sub(asic_pattern,'',line)
       asic = re.sub(return_pattern,'',asic)
     if config_flags_pattern.search(line) != None:
       build_flags = re.sub(config_flags_pattern,'',line)
       build_flags = re.sub(return_pattern,'',build_flags)

   return (build_flags,asic)
     
