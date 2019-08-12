
#this scipt loads the log and analyzes it 
import sys
import glob 
from struct import *
import pickle
import pprint 
import re

if len(sys.argv)<2:
   print "Usage extract_mhi_log.py log_file compressed_file_root"
   quit()

dump=sys.argv[1]
compressed_dir=sys.argv[2]

if dump==None:
   print "Usage extract_mhi_log.py log_file compressed_file_root"
   quit()

if compressed_dir==None:
   print "Usage extract_mhi_log.py log_file compressed_file_root"
   quit()

#size of log, should change in future to get from the log itself
logsize=2000

#read in the file
f=open(dump, "rb")
log=f.read()
f.close()

tail,head=unpack_from("<LL", log)
raw_data=log[8:]

#build the raw_log from head to tail
print "Tail %#x" % tail
print "Head %#x" % head
if tail > head:
   raw_log=raw_data[head:tail]
elif tail < head:
   raw_log=raw_data[head:]
   raw_log+=raw_data[:tail]
else:
   print "Log is empty"

pprint.pprint(raw_log)

#build the dictionary
str_map={}
files=glob.glob(compressed_dir+"/*.c.p")

for f in files:
  str_map.update(pickle.load(open(f, "rb")))

#walk the raw log and print the messages

i=0
while (i<len(raw_log)):
   #unpack the header
   log_id, num_items=unpack_from("<Hb", raw_log, i)

   #print i
   #print "Log id: %#x" % log_id
   #print "num_items: %#d" % num_items 

   args=unpack_from("<"+"q"*num_items, raw_log, i+calcsize("<Hb"))

   #strip out ll python doesnt understand it 
   str_map[log_id]=re.sub(r'%llx', r'%x', str_map[log_id])

   print str_map[log_id] % args
   i+=calcsize("<HB")+8*num_items



