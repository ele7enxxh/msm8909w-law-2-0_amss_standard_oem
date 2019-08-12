#! /usr/bin/env python

import multiprocessing
import math
import sys
import struct
from collections import Counter
from q6zip_constants import *

DEBUG_ENABLED = False
DEBUG2_ENABLED = False

def debug(arg):
	if DEBUG_ENABLED:
		sys.stdout.write(arg)

def debug2(arg):
	if DEBUG2_ENABLED:
		sys.stdout.write(arg)


#Pseudo enum type
def enum(**enums):
	return type('Enum', (), enums)

CODE = enum(
			NO_MATCH	      = (     0b011, 3, 0xFFFFFFFF ,"NO_MATCH"),
			DICT1_MATCH       = (     0b100, 3, 0xFFFFFFFF ,"DICT1_MATCH"),
			MATCH_8N_SQ0      = (     0b001, 3, 0xFFFFFFFF ,"MATCH_8N_SQ0"),
			MATCH_8N_SQ1      = (     0b111, 3, 0xFFFFFFFF ,"MATCH_8N_SQ1"),
			MATCH_6N_2x0_SQ0  = (     0b000, 3, 0xFFFFFF00 ,"MATCH_6N_2x0_SQ0"), 	#MATCH_6Nx0
			MATCH_6N_2x0_SQ1  = (     0b110, 3, 0xFFFFFF00 ,"MATCH_6N_2x0_SQ1"), 	#MATCH_6Nx0
			DICT2_MATCH       = (    0b0101, 4, 0xFFFFFFFF ,"DICT2_MATCH"),
			MATCH_5N_3x0_SQ0  = (    0b0010, 4, 0xFFFFF000 ,"MATCH_5N_3x0_SQ0"),     #MATCH_5Nx0
			MATCH_4N_4x0_SQ0  = (   0b01101, 5, 0xFFFF0000 ,"MATCH_4N_4x0_SQ0"), 	#MATCH_4Nx1
			MATCH_5N_3x0_SQ1  = (   0b11101, 5, 0xFFFFF000 ,"MATCH_5N_3x0_SQ1"),     #MATCH_5Nx0
			MATCH_4N_4x0_SQ1  = (  0b001010, 6, 0xFFFF0000 ,"MATCH_4N_4x0_SQ1"), 	#MATCH_4Nx1
			MATCH_6N_2x2_SQ0  = (  0b111010, 6, 0xFFFF00FF ,"MATCH_6N_2x2_SQ0"),     #MATCH_6Nx1
			MATCH_6N_2x4_SQ0  = (  0b101010, 6, 0xFF00FFFF ,"MATCH_6N_2x4_SQ0"),     #MATCH_6Nx2
			MATCH_6N_2x2_SQ1  = ( 0b1011010, 7, 0xFFFF00FF ,"MATCH_6N_2x2_SQ1"),     #MATCH_6Nx1
			MATCH_6N_2x4_SQ1  = ( 0b0011010, 7, 0xFF00FFFF ,"MATCH_6N_2x4_SQ1"),     #MATCH_6Nx2
			END_BLOCK         = ( 0b0011010, 7, 0xFFFFFFFF ,"END_BLOCK")       # Alised onto MATCH_6N_2x4_SQ1
			)

class CompressionProcess(multiprocessing.Process):

	def __init__(self, dict1Size, dict1Bits, dict2Size, dict2Bits, blockSize, lookbackLen, lookbackBits, instrChunk, dictionary, findFlip1Lookup, findFlip2Lookup, q, statisticsWords,statisticsBits,chunkSize,partial=True):
		super(CompressionProcess,self).__init__()

		self.listOfMasks = [0xFFFFFFFF,0xFFFFFF00,0xFFFF00FF,0xFF00FFFF,0xFFFFF000,0xFFFF0000,0xFF787E18]
		self.listOfSuperLookbackMasks = [0xFFFFFFFF,0xFFFFFF00,0xFFFFF000]

		self.enableSuperLookback=True

		self.DICTIONARY1_SIZE = dict1Size
		self.DICTIONARY1_BITS = dict1Bits
		self.DICTIONARY2_SIZE = dict2Size
		self.DICTIONARY2_BITS = dict2Bits
		self.BLOCK_SIZE = blockSize 	  #in terms of words
		self.LOOKBACK_LEN = lookbackLen
		self.LOOKBACK_BITS = lookbackBits

		self.instrList = instrChunk
		self.wordSymbolByteDict = dictionary
		self.compressedStream = q
		self.statisticsWords = statisticsWords
		self.statisticsBits = statisticsBits
		self.codeNumOccurances = Counter()
		self.codeBitCount = Counter()

		self.blockStream = []
		self.offsetdict = {}
                self.chunkSize = chunkSize
                self.partial = partial

	def run(self):

		#run the compress class method individually on each block of the instrList
		for instrList in [self.instrList[i:i+self.BLOCK_SIZE] for i in range(0, len(self.instrList), self.BLOCK_SIZE)]:
			instrList.extend([0] * (self.BLOCK_SIZE-len(instrList)))
			self.compress(instrList)
		#copy stats to threadsafe structure
		for (stat,count) in self.codeNumOccurances.iteritems():
			self.statisticsWords[stat] = count
		for (stat,count) in self.codeBitCount.iteritems():
			self.statisticsBits[stat] = count


	############################################################################
	#check if there's a match directly after where last match was found
	############################################################################
	def findSequentialLookback(self,instrList,listLen,instr,codeTuple,*extraBitsList):
		if (self.lastOffsetFromEndOfListX==False):
			return (False)
		codeMask=codeTuple[2]
		codeStr=codeTuple[3]
		instr&=codeMask
		lastOffset = self.lastOffsetFromEndOfListX
		if lastOffset<listLen:
			#next=instrList[listLen - lastOffset - 1]
			next=instrList[listLen + lastOffset ]
			if (next&codeMask) == instr:
				codeBits=codeTuple[0]
				codeLen=codeTuple[1]
				self.pushCompressedBits(codeStr,(codeBits,codeLen))
				for extraBits in extraBitsList:
					self.pushCompressedBits(codeStr,extraBits)

				self.codeNumOccurances[codeStr]+=1
				return (True)
		return (False)

	def findSuperLookback(self,instrList,listLen,instr,firstMask,maxLookBackLen):
		#Looks for a super mask match
		if self.enableSuperLookback:
			if firstMask in self.listOfSuperLookbackMasks:
				if (listLen+1)<len(instrList):
					firstMaskedInstr=instr&firstMask
					for secondMask in self.listOfSuperLookbackMasks:
						secondMaskedInstr=instrList[listLen+1]&secondMask
						superInstr=(firstMaskedInstr<<32)|secondMaskedInstr
						superMask=(firstMask<<32)|secondMask
						idx = self.offsetdict.get((superMask,superInstr),False)
						if idx != False:
							#convert from index to an offset from end
							negOffset=idx-listLen
							if negOffset >= 0:
								print "negOffset>=0 %d"%negOffset
							if negOffset > (-maxLookBackLen):
								#print "idx=%d deltaOffset=%d listLen=%d negOffset=%d maxLookBackLen=%d"%(idx,deltaOffset,listLen,negOffset,-maxLookBackLen)
								return(negOffset)

		#Super mask not found, so look for a single mask match
		idx = self.offsetdict.get((firstMask,instr),False)
		if idx == False:
			return (False)
		if (instrList[idx]&firstMask)!=instr:
			print "Dictionary error"
		#convert from index to an offset from end
		deltaOffset=(listLen - idx - 1)
		negOffset=idx-listLen
		if negOffset >= 0:
			print "negOffset>=0 %d"%negOffset
		if negOffset > (-maxLookBackLen):
			#print "idx=%d deltaOffset=%d listLen=%d negOffset=%d maxLookBackLen=%d"%(idx,deltaOffset,listLen,negOffset,-maxLookBackLen)
			return(negOffset)
		return(False)

	def findLookback(self,instrList,listLen,instr,codeTuple,*extraBitsList):
		codeMask=codeTuple[2]
		codeStr=codeTuple[3]
		instr&=codeMask
		maxLookBackLen=self.LOOKBACK_LEN

		#check if masked match exists in list
		negOffset=self.findSuperLookback(instrList,listLen,instr,codeMask,maxLookBackLen)
		#if no match, return nothing
		if negOffset == False:
			return(False)

		#otherwise, store match in dictionary of matches, and return match
		self.lastOffsetFromEndOfListX=negOffset

		codeBits=codeTuple[0]
		codeLen=codeTuple[1]
		self.pushCompressedBits(codeStr,(codeBits,codeLen))
		#self.pushCompressedBits(codeStr,(offset,self.LOOKBACK_BITS))
		self.pushCompressedBits(codeStr,(negOffset,self.LOOKBACK_BITS))

		for extraBits in extraBitsList:
			self.pushCompressedBits(codeStr,extraBits)

		self.codeNumOccurances[codeStr]+=1
		return (True)

	def  findDictionaryEntry1(self,instr):
		try:
			index=self.wordSymbolByteDict[instr]
			if index<self.DICTIONARY1_SIZE:
				codeTuple=CODE.DICT1_MATCH
				codeStr=codeTuple[3]
				codeBits=codeTuple[0]
				codeLen=codeTuple[1]
				self.pushCompressedBits(codeStr,(codeBits,codeLen))
				self.pushCompressedBits(codeStr,(index,self.DICTIONARY1_BITS))
				self.codeNumOccurances[codeStr]+=1
				return(True)
			return (False)
		except KeyError:
			return(False)

	def  findDictionaryEntry2(self,instr):
		try:
			index=self.wordSymbolByteDict[instr]
			if index>=self.DICTIONARY1_SIZE:
				codeTuple=CODE.DICT2_MATCH
				codeStr=codeTuple[3]
				index=index-self.DICTIONARY1_SIZE
				codeBits=codeTuple[0]
				codeLen=codeTuple[1]
				self.pushCompressedBits(codeStr,(codeBits,codeLen))
				self.pushCompressedBits(codeStr,(index,self.DICTIONARY2_BITS))
				self.codeNumOccurances[codeStr]+=1
				return(True)
			return (False)
		except KeyError:
			return(False)

	compressedPartialWord=0
	compressedPartialBits=0

	def pushCompressedBits(self,codeStr,bitsNumBitsTuple):

		#print " ", "{0:#0{1}b}".format(bits,numBits+2),
		bits=bitsNumBitsTuple[0]
		numBits=bitsNumBitsTuple[1]
		bits=bits&((1<<numBits)-1)
		self.compressedPartialWord |= bits<<self.compressedPartialBits
		self.compressedPartialBits += numBits
		self.codeBitCount[codeStr]+= numBits
                debug(" %s"%codeStr)
		if self.compressedPartialBits >= 32:
			compressedWord = self.compressedPartialWord & 0xFFFFFFFF
			self.compressedPartialWord >>= 32
			self.compressedPartialBits -= 32
			word=struct.pack('I', compressedWord & 0xFFFFFFFF)
			self.blockStream.append(word)
                        debug(" %08X"%compressedWord)

	############################################
	def encode(self,instrList,i,instr):
		if (self.findSequentialLookback(instrList,i,instr,CODE.MATCH_8N_SQ1)):
			return
		if (self.findSequentialLookback(instrList,i,instr,CODE.MATCH_6N_2x0_SQ1,(instr&0xFF,8))):
			return
		if (self.findLookback(instrList,i,instr,CODE.MATCH_8N_SQ0)):
			return
		if (self.findDictionaryEntry1(instr)):
			return
		if (self.findSequentialLookback(instrList,i,instr,CODE.MATCH_6N_2x2_SQ1,((instr>>8)&0xFF,8))):
			return
                if (not ((instr>>16)&0xFF == CHUNK_END_MARKER)):
                        if (self.findSequentialLookback(instrList,i,instr,CODE.MATCH_6N_2x4_SQ1,((instr>>16)&0xFF,8))):
                            return
		if (self.findDictionaryEntry2(instr)):
			return
		if (self.findSequentialLookback(instrList,i,instr,CODE.MATCH_5N_3x0_SQ1,((instr&0xFFF),12))):
			return
		if (self.findLookback(instrList,i,instr,CODE.MATCH_6N_2x0_SQ0,(instr&0xFF,8))):
			return
		if (self.findSequentialLookback(instrList,i,instr,CODE.MATCH_4N_4x0_SQ1,(instr&0xFFFF,16))):
			return
		if (self.findLookback(instrList,i,instr,CODE.MATCH_6N_2x2_SQ0,((instr>>8)&0xFF,8))):
			return
		if (self.findLookback(instrList,i,instr,CODE.MATCH_6N_2x4_SQ0,((instr>>16)&0xFF,8))):
			return
		if (self.findLookback(instrList,i,instr,CODE.MATCH_5N_3x0_SQ0,((instr&0xFFF),12))):
			return
		if (self.findLookback(instrList,i,instr,CODE.MATCH_4N_4x0_SQ0,(instr&0xFFFF,16))):
			return

		###############################
		# No match so send the raw bits
		###############################
		codeTuple=CODE.NO_MATCH
		codeBits=codeTuple[0]
		codeLen=codeTuple[1]
		codeStr=codeTuple[3]
		self.pushCompressedBits(codeStr,(codeBits,codeLen))
		self.pushCompressedBits(codeStr,(instr&0xFFFFFFFF,32))
		self.codeNumOccurances[codeStr]+=1
		#print "[%4.4d] instr=0x%8.8x"%(i,instr)


	############################################
	def compress(self,instrList):
                potentialChunkEnd = False
                chunk = 0
                last_out = 0
                last_stream_len = 0
                self.stream = []
		self.blockStream = []
		self.offsetdict.clear()
		self.lastOffsetFromEndOfListX=False

		#############################################
		for i,instr in enumerate(instrList):
                        debug("\n%3d %08X"%(i,instr))
			self.encode(instrList,i,instr)
			#Add new instr to the lookback dictionary

                        instrsFirstChunk = 0
                        if (not potentialChunkEnd and (i == instrsFirstChunk or (chunk > 0 and i >= ((chunk * self.chunkSize) - 1) and i < (self.BLOCK_SIZE - 1)))):
                                potentialChunkEnd = True
                                debug("\npotentialChunkEnd at i %d, chunk size %d"%(i,self.chunkSize))
                                chunk = chunk + 1
			for mask in self.listOfMasks:
				maskedInstr=instr&mask
				self.offsetdict[(mask,maskedInstr)] = i

			if self.enableSuperLookback:
				for firstMask in self.listOfSuperLookbackMasks:
					firstMaskedInstr=instr&firstMask
					for secondMask in self.listOfSuperLookbackMasks:
						if (i+1)<len(instrList):
							secondMaskedInstr=instrList[i+1]&secondMask
							superInstr=(firstMaskedInstr<<32)|secondMaskedInstr
							superMask=(firstMask<<32)|secondMask
							self.offsetdict[(superMask,superInstr)] = i

                        endOfChunk = False

                        # 0xC000 are the bits for end of packet bits
                        if (self.partial and potentialChunkEnd and (((instr & 0xC000) == 0xC000) or ((instr & 0xC000) == 0x0000))):
                                endOfChunk = True

                        if endOfChunk:
                                potentialChunkEnd = False
                                global N_COMPRESSION_BLOCKS
                                # Only these combinations supported
                                if chunk != 0 and ((N_COMPRESSION_BLOCKS == N_CHUNKS) or \
                                            (N_COMPRESSION_BLOCKS == 2 and N_CHUNKS == 4 and chunk == 3)):
                                    self.offsetdict.clear()
                                    self.lastOffsetFromEndOfListX = False

                                codeTuple=CODE.END_BLOCK
                                codeBits=codeTuple[0]
                                codeLen=codeTuple[1]
                                codeStr=codeTuple[3]
                                self.pushCompressedBits(codeStr,(codeBits,codeLen))
                                self.pushCompressedBits(codeStr,(CHUNK_END_MARKER, 8))

                                # build metadata to match:
                                # typedef struct {
                                #         unsigned short last_sequencial:10;
                                #         unsigned short bits_left:6;
                                #         unsigned short in_delta:10;
                                #         unsigned short out_delta_from_chunk_size:6;
                                # } chunk_metadata_t;

                                out_delta_from_chunk_size = i + 1 - last_out
                                if chunk != 1:
                                        out_delta_from_chunk_size = out_delta_from_chunk_size - self.chunkSize
                                last_out = i + 1
                                in_delta = len(self.blockStream) - last_stream_len
                                last_stream_len = len(self.blockStream)

                                bits_left = 32-self.compressedPartialBits

                                last_sequencial = -1
                                if (self.lastOffsetFromEndOfListX != False):
                                        last_sequencial = self.lastOffsetFromEndOfListX

                                debug2("\nEOB and metadata at i: %d  out delta: %d  in_delta: %d  "\
                                               "bits_left: %d  last out: %d"%\
                                               (i,out_delta_from_chunk_size,in_delta,bits_left,last_out))


                                word = ((out_delta_from_chunk_size & 0x3F) << 26) | \
                                    ((in_delta                  & 0x3FF) << 16) | \
                                    ((bits_left                 & 0x3F) << 10) | \
                                    ((last_sequencial           & 0x3FF))
                                chunk_metadata=struct.pack('I',word)
                                self.stream.append(chunk_metadata)

                        if (i == (self.BLOCK_SIZE - 1)): #end of block
                                self.endOfStream()
                                debug2("\nEOS at:%i"%(i))

		## END FOR LOOP #######################################

                self.stream.extend(self.blockStream)
                self.compressedStream.append(self.stream)
        def endOfStream(self):
		#######################################################
		#finish block is aliased onto the lowest freq code
		#we check for the number of words decoded on rx'ing this code
		#######################################################
		codeTuple=CODE.END_BLOCK
		codeBits=codeTuple[0]
		codeLen=codeTuple[1]
		codeStr=codeTuple[3]
		self.pushCompressedBits(codeStr,(codeBits,codeLen))
		self.pushCompressedBits(codeStr,(CHUNK_END_MARKER, 8))

		#######################################################
		#pad and push remaining bits in word stream
		#######################################################
		if self.compressedPartialBits>0:
			compressedWord = self.compressedPartialWord & 0xFFFFFFFF
			word=struct.pack('I', compressedWord & 0xFFFFFFFF)
			self.blockStream.append(word)
                        debug(" %08X\n\n"%compressedWord)
                else:
                        debug(" \n\n")
		self.compressedPartialBits = 0
		self.compressedPartialWord = 0

