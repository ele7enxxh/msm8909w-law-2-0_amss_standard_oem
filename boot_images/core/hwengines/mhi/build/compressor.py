import os
import shutil
import subprocess
import string
import SCons.Scanner
import SCons.Node.FS
import re;
from SCons.Script import *
import pickle 
import threading
from functools import partial

id_count=0 
lock=threading.RLock()

def get_id():
   global id_count
   global lock

   lock.acquire()
   id_count+=1
   count=id_count
   lock.release()
   return count

def replace_tags(table, m):
   msg_id=get_id()
  
   args=describe_args(str(m.group(2)))
   new_msg=str(msg_id)+m.group(1)+args+','+m.group(2)

   table[msg_id]=m.group(2)

   return new_msg

def describe_args(fmt):
   argstring='"'

   #find % to figure out how many arguments and describe them with 
   #a simple string
   i = re.finditer("%([xdsl]*)", fmt)

   for m in i:
      if (m.group(1)=='x' or m.group(1)=='d'):
         argstring+='4'
      elif( m.group(1)=='llx' or m.group(1)=='lld'):
         argstring+='8'


   argstring+='"'


   return argstring

   

def compress(src, dest):
   table={}
   f=open(src)
   text=f.read()
   f.close()

   print "generating file" 
   f=open(dest, "w")

   #insert @ID tags and @STRING tags so we can then find them easier and replace
   #them
   pattern=re.compile(r'(mhi_osal_debug.*?)(".*?")', re.DOTALL)
   text=pattern.sub(r'\1@ID, @STR$\2$',text)

   #replace the tags
   pattern=re.compile(r"@ID(.*?)@STR\$(.*?)\$", re.DOTALL)
   text=pattern.sub(partial(replace_tags, table), text)
   f.write(text)
   f.close()

   #pickle it
   f=open(os.path.dirname(dest)+"/"+os.path.basename(dest)+".p", "wb", 0)
   pickle.dump(table, f)
   f.close()



#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('compressor')

def generate(env):
   compressor_register(env)

#===============================================================================
# COMPRESSOR builder 
#===============================================================================
def compressor_register(env):
   compressor_act = env.GetBuilderAction(compressor_builder)
   compressor_bld = env.Builder(action = SCons.Action.Action(compressor_builder, "Compressing"), 
                               #source_scanner = SCons.Scanner.C.CScanner(),
                               emitter = compressor_emitter,
                               suffix = '.o', src_suffix = '.c')

   env.Append(BUILDERS = {'CompressorBuilder' : compressor_bld})

def compressor_builder(target, source, env):
   
   for t, s in zip(target,source):
      compress(str(s),str(t))


   return None

def compressor_emitter(target, source, env):

   #import pdb; pdb.set_trace()
   altered_targets=[]
   #create targets from sources dont rely on scons 
   #it will create one object file for all the c files
   for s in source:
      #t.name=t.name[:t.name.rfind('.')]+'_compressed.c'
      name=str(os.path.basename(str(s)))
      name=name[:name.rfind('.')]
      dest=os.path.dirname(str(target[0])) 
      altered_targets.append(dest+"/"+name+'_compressed.c')
     
      #add dependency on string map so it regenerated  
      env.Depends(dest+'/'+"name.p", s )
  
   

   return (altered_targets,source)

