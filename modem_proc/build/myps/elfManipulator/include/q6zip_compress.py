#! /usr/bin/env python
import time
import os
import sys
import struct
import math
import inspect

try:
	from collections import Counter

except ImportError:
	print 'Caught ImportError while trying to import collections'
	print 'Your python version could be too old. We require python >= 2.7'
	print 'You have python', sys.version
	print

	raise

from multiprocessing import cpu_count, Manager
from Queue import Queue
from functools import partial
from compress_process import CompressionProcess

this_path = os.path.split(
	inspect.getframeinfo(inspect.currentframe()).filename)

print 'Loaded %s from %s' % (this_path[1], this_path[0])


PRELOADED_DICT = 0
DICTIONARY_SIZE = 2048
DICTIONARY_BITS = int(math.ceil(math.log(float(DICTIONARY_SIZE),2)))
BLOCK_SIZE = 1024
LOOKBACK_LEN = 256
LOOKBACK_BITS = int(math.ceil(math.log(float(LOOKBACK_LEN),2)))


INFO_ENABLED = 1

def info(arg):
	if INFO_ENABLED:
		print arg
		print "---"

def chunks(l, n, size=BLOCK_SIZE):
	""" Divide l amongs n groups, making sure first n-1 groups are multiples of BLOCK_SIZE
	"""
	wordsUsed = 0
	threads = n
	for i in xrange(n-1):
		blocksPerThread = int(math.ceil(((len(l)-wordsUsed) / float(threads)) / size))
		threads -= 1
		yield l[wordsUsed:wordsUsed+blocksPerThread*size]
		wordsUsed += blocksPerThread*size
	yield l[wordsUsed:]



Mod37BitPosition = [
					32, 0, 1, 26, 2, 23, 27, 0, 
					3, 16, 24, 30, 28, 11, 0, 13,
					4, 7, 17, 0, 25, 22, 31, 15,
					29, 10, 12, 6, 0, 21, 14, 9,
					5, 20, 8, 19, 18
				   ]

POPCOUNT_TABLE16 = [0] * 2**16
for index in xrange(2**16):
	POPCOUNT_TABLE16[index] = (index & 1) + POPCOUNT_TABLE16[index >> 1]

findFlip1Lookup = []
for x in xrange(2**16):
	if POPCOUNT_TABLE16[x] == 1:
		bitSet = Mod37BitPosition[(-x & x) % 37]
		findFlip1Lookup.append(bitSet)
		continue
	findFlip1Lookup.append(-1)

findFlip2Lookup = {}
for firstBitSet in xrange(16):
	for absOffset in xrange(8):
		x = 1 << firstBitSet
		x |= 1 << (absOffset+firstBitSet+1)
		findFlip2Lookup[x] = (firstBitSet,absOffset)

		
def compress(page_size=BLOCK_SIZE*4, VA_start=0, input=None):
	threadSafe = Manager()
	start = time.time()
	instrList=[]
	instrListSubset=[]
	dictionary={}

	if not input:
		if (len(sys.argv) < 2):
			print "Usage: " + sys.argv[0] + " <file-to-compress> [<compression-type>] [<subset-start>] [<subset-size>]"
			exit(-2)

		inputFile = sys.argv[1]

		type = 'f'
		subset_start = 0
		subset_size = 0xFFFFFFFF

		if (len(sys.argv) > 2):
			type = sys.argv[2][0:1]
		if (len(sys.argv) > 3):
			subset_start = int(sys.argv[3], 16) + 1
		if (len(sys.argv) > 4):
			subset_size = int(sys.argv[4], 16)

		with open(inputFile,"rb") as f:
			word = f.read(4)
			bytes_read = 4
			while word:
				word = struct.unpack('I', word)[0]
				instrList.append(word)
				if bytes_read >= subset_start and bytes_read <= (subset_start+subset_size):
					instrListSubset.append(word)
				word = f.read(4)
				if len(word)!=4:
					info("Warning file not a multiple of words: len(last_read)=%d bytes_read=%d"%(len(word),bytes_read))
					break
				bytes_read += 4

	else:
		for word in (input[i:i+4] for i in range(0,len(input),4)):
			if len(word) == 4:
				instrList.append( struct.unpack('I',word)[0] )
		instrListSubset = instrList

			
	fileReadTime=time.time()-start
	###########################################################################
	start = time.time()

	dict_list = [struct.pack('I', 0)]*DICTIONARY_SIZE

	if PRELOADED_DICT:
		with open("dict.bin","rb") as f:
			idx = 0
			word = f.read(4)
			while word and len(word) == 4:
				word = struct.unpack('I', word)[0]
				dictionary[word] = idx
				idx += 1

	else:
		countDict = Counter(instrList)
		with open("dict_new.bin","w") as f:
			for n,(word,count) in enumerate(countDict.most_common(DICTIONARY_SIZE)):
				dictionary[word] = n
				dict_list[n] = struct.pack('I', word & 0xFFFFFFFF)
				# print n, "-", word
				word = struct.pack('>I', word)
				f.write(word)
	
	dictionaryTime=time.time()-start
	###########################################################################
	start = time.time()


	n_blocks = 0
	n_threads = cpu_count() + cpu_count() / 2
	processes = []
	for n,instrChunk in enumerate(chunks(instrListSubset,n_threads,page_size/4)):
		chunkBlocks = len(instrChunk)/float(BLOCK_SIZE)
		n_blocks += math.ceil(chunkBlocks)
		if len(instrChunk) > 0:
			print "Thread", n, "assigned",  chunkBlocks, "blocks"
			q = threadSafe.list()
			p = CompressionProcess(instrChunk, dictionary, findFlip1Lookup, findFlip2Lookup, q)
			processes.append((p,q))
			p.start()
	print "----"

	metadata = [struct.pack("H",n_blocks),struct.pack("H",0)]
	metadata += dict_list

	v_addrs = []
	va = VA_start + 2 + 2 + 4 * DICTIONARY_SIZE + 4 * n_blocks
	compressed_text = []
	with open("compressed.bin", "wb") as f:
		for n,(p,q) in enumerate(processes):
			p.join()
			for block in q:
				v_addrs.append( struct.pack('I',va) )
				for word in block:
					compressed_text.append(word)
					# print bin(struct.unpack('I', word)[0])
					f.write(word)
				va += 4 * len(block)
			print "Thread", n, "compressed",  len(q), "blocks"

	metadata += v_addrs
	metadata += compressed_text
	print "----"

	compressTime = time.time()-start
   ############################################################################
		
	print "File Read Time:", fileReadTime,"s"
	print "Dictionary Time:", dictionaryTime, "s"
	print "Compression Time:", compressTime, "s"
	

	return ''.join(metadata)



if __name__ == '__main__':
	compress()
