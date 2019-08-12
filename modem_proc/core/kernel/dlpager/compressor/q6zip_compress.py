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

from compress_process import CompressionProcess

from q6zip_constants import *

this_path = os.path.split(
	inspect.getframeinfo(inspect.currentframe()).filename)

print 'Loaded %s from %s' % (this_path[1], this_path[0])

#Development parameters
DEV = 0  #enable to create intermediate dict.bin and compressed.bin
PRELOADED_DICT = 0  #enable to use pregenerated dict.bin
INFO_ENABLED = True	#enable to print info statements

#Algorithm parameters
DICTIONARY1_BITS = DICT1_BITS
DICTIONARY2_BITS = DICT2_BITS
DICTIONARY1_SIZE = 2**DICTIONARY1_BITS
DICTIONARY2_SIZE = 2**DICTIONARY2_BITS
DICTIONARY_SIZE  =DICTIONARY1_SIZE+DICTIONARY2_SIZE
BLOCK_SIZE = 1024 # same as PAGE_SIZE but in words
CHUNK_SIZE = BLOCK_SIZE/N_CHUNKS
LOOKBACK_LEN = 2**LB_BITS

def info(arg):
	if INFO_ENABLED:
		print arg


def chunks(l, n, size=BLOCK_SIZE):
	""" Divide l amongst n groups, making sure first n-1 groups are multiples of BLOCK_SIZE
	"""
	wordsUsed = 0
	threads = n
	for i in xrange(n-1):
		blocksPerThread = int(math.ceil(((len(l)-wordsUsed) / float(threads)) / size))
		threads -= 1
		yield l[wordsUsed:wordsUsed+blocksPerThread*size]
		wordsUsed += blocksPerThread*size
	yield l[wordsUsed:]


def compress(page_size=BLOCK_SIZE*4, VA_start=0, input=None, nonpartialStart=0):
	threadSafe = Manager()
	start = time.time()
	instrList=[]
	instrListSubset=[]
	instrListNonPartial=[]
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
                maxText = 0
                if (nonpartialStart != 0):
                  maxText = nonpartialStart
		for word in (input[i:i+4] for i in range(0,maxText,4)):
			if len(word) == 4:
				instrList.append( struct.unpack('I',word)[0] )
		instrListSubset = instrList

                if (nonpartialStart != 0):
                    print "Size of partial is %u"%(len(instrList)*4)
                for word in (input[i:i+4] for i in range(maxText,len(input),4)):
                    if len(word) == 4:
                        instrListNonPartial.append( struct.unpack('I',word)[0] )
                print "Size of non-partial is %u"%(len(instrListNonPartial)*4)


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
				dict_list[idx] = struct.pack('I', word & 0xFFFFFFFF)
				idx += 1

	else:
		countDict = Counter(instrList + instrListNonPartial)

		for n,(word,count) in enumerate(countDict.most_common(DICTIONARY_SIZE)):
			dictionary[word] = n
			dict_list[n] = struct.pack('I', word & 0xFFFFFFFF)

		if DEV:
			with open("dict.bin","w") as f:
				for word in dict_list:
					f.write(word)


	dictionaryTime=time.time()-start
	###########################################################################
	start = time.time()


	n_blocks = 0
	n_threads = int((cpu_count()*3)/2)
	#n_threads = 1
	processes = []
	#Launch a thread for each chunk of blocks
	for n,instrChunk in enumerate(chunks(instrListSubset,n_threads,page_size/4)):
		chunkBlocks = len(instrChunk)/float(BLOCK_SIZE)
		n_blocks += math.ceil(chunkBlocks)
		if len(instrChunk) > 0:
			#print "Thread", n, "assigned",  chunkBlocks, "blocks"
			q = threadSafe.list()
			statsWords = threadSafe.dict()
			statsBits = threadSafe.dict()
			p = CompressionProcess(DICTIONARY1_SIZE, DICTIONARY1_BITS,DICTIONARY2_SIZE, DICTIONARY2_BITS, BLOCK_SIZE, LOOKBACK_LEN, LB_BITS, instrChunk, dictionary, None, None, q, statsWords,statsBits, CHUNK_SIZE, True)
			processes.append((p,q,statsWords,statsBits))
			p.start()
	#print "----"

        # non-partial
	for n,instrChunk in enumerate(chunks(instrListNonPartial,n_threads,page_size/4)):
		chunkBlocks = len(instrChunk)/float(BLOCK_SIZE)
		n_blocks += math.ceil(chunkBlocks)
		if len(instrChunk) > 0:
			#print "Thread", n, "assigned",  chunkBlocks, "blocks"
			q = threadSafe.list()
			statsWords = threadSafe.dict()
			statsBits = threadSafe.dict()
			p = CompressionProcess(DICTIONARY1_SIZE, DICTIONARY1_BITS,DICTIONARY2_SIZE, DICTIONARY2_BITS, BLOCK_SIZE, LOOKBACK_LEN, LB_BITS, instrChunk, dictionary, None, None, q, statsWords,statsBits, CHUNK_SIZE, False)
			processes.append((p,q,statsWords,statsBits))
			p.start()


	v_addrs = []  #list of block starting addresses
	va = VA_start + 2 + 2 + 4 * DICTIONARY_SIZE + 4 * n_blocks  #2 bytes for n_blocks, 2 for 0, 4 per dict entry, 4 per block start addr

	#Join the threads and process results
	compressed_text = []
	statsWordsCounter = Counter()
	statsBitsCounter = Counter()
	for n,(p,q,statsWords,statsBits) in enumerate(processes):
		p.join()
		for block in q:
			v_addrs.append( struct.pack('I',va) )
			for word in block:
				compressed_text.append(word)
			va += 4 * len(block)
		print "Thread", n, "compressed",  len(q), "blocks"

		#consolidate thread statistics
		for (stat,count) in statsWords.items():
			statsWordsCounter[stat] += count
		for (stat,count) in statsBits.items():
			statsBitsCounter[stat] += count
	print "----"

	if DEV:
		with open("compressed.bin", "wb") as f:
			for word in compressed_text:
				f.write(word)

	compressTime = time.time()-start
   ############################################################################

	print "File Read Time:", fileReadTime,"s"
	print "Dictionary Time:", dictionaryTime, "s"
	print "Compression Time:", compressTime, "s"

	#build meta data
	metadata = [struct.pack("H",n_blocks), struct.pack("H",Q6ZIP_RO_ALGORITHM_VERSION)]
	metadata += dict_list
	metadata += v_addrs
	metadata += compressed_text

	#print compression stats
	info("########################################################################")
        info("")
	info("                Type    #Words     #TBits #Bits #WordsSaved Ratio %%")
	wordsUncompressed=0
	wordsCompressed=0
	for (stat,wordCount) in statsWordsCounter.most_common():
		bitCount=statsBitsCounter[stat]
		bitsPerWord=0
		if bitCount!=0:
			bitsPerWord=int(float(bitCount)/float(wordCount))
		wordsSaved=wordCount-(bitCount/32)
		ratio=float(bitCount/32)/float(wordCount)
		wordsUncompressed+=wordCount
		wordsCompressed+=((bitCount+31)/32)

	for (stat,wordCount) in statsWordsCounter.most_common():
		bitCount=statsBitsCounter[stat]
		bitsPerWord=0
		if bitCount!=0:
			bitsPerWord=int(float(bitCount)/float(wordCount))
		wordsSaved=wordCount-(bitCount/32)
		ratio=float(bitCount/32)/float(wordCount)
#		info('symb2freq["%20s"] = %10d #%10d %10d %10d  %2.4f'%(stat,wordCount,bitCount,bitsPerWord,wordsSaved,ratio))
		info('%20s %9d %9d %3d %9d %2.4f %%=%2.4f'%(stat,wordCount,bitCount,bitsPerWord,wordsSaved,ratio, float(wordCount)/float(wordsUncompressed)*100.0))
	#print("==================================")
	#for (stat,bitCount) in statsBitsCounter.most_common():
		#info('symb2freq["%20s"] = %10d'%(stat,bitCount))

        wordsCompressed = wordsCompressed + DICTIONARY_SIZE
	totalRatio=float(wordsCompressed)/float(wordsUncompressed)

	info("T=%dKB C=%dKB R=%2.4f S=%dKB D=%dKB(%d/%d) LB=%d"%
             (wordsUncompressed/256.0,
              wordsCompressed/256.0,
              totalRatio,
              wordsUncompressed/256.0 - wordsCompressed/256.0,
              DICTIONARY_SIZE*4/1024.0, DICTIONARY1_BITS, DICTIONARY2_BITS,
              LB_BITS))

	return ''.join(metadata)  #joins list elements together as string with no spaces

if __name__ == '__main__':
	compress()
