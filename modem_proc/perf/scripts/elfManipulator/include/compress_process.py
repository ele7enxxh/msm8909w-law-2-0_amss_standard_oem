#! /usr/bin/env python

import multiprocessing
import math
import struct

class CompressionProcess(multiprocessing.Process):

	DICTIONARY_SIZE = 2048
	DICTIONARY_BITS = int(math.ceil(math.log(float(DICTIONARY_SIZE),2)))
	BLOCK_SIZE = 1024
	LOOKBACK_LEN = 256
	LOOKBACK_BITS = int(math.ceil(math.log(float(LOOKBACK_LEN),2)))

	def __init__(self, instrChunk, dictionary, findFlip1Lookup, findFlip2Lookup, q):
		super(CompressionProcess,self).__init__()
		self.instrList = instrChunk
		self.wordSymbolByteDict = dictionary
		self.findFlip1Lookup = findFlip1Lookup
		self.findFlip2Lookup = findFlip2Lookup
		self.compressedStream = q
		self.blockStream = []

	def run(self):
		for block in [self.instrList[i:i+self.BLOCK_SIZE] for i in range(0, len(self.instrList), self.BLOCK_SIZE)]:
			self.compress(block)

	offsetdict = {}
	lastOffsetFromEndOfList={}
	def findOffsetFromEndOfList(self,L,maxLookBackLen,value,mask):
		listLen = idx = len(L)
		last = self.lastOffsetFromEndOfList.get(mask,float("inf"))
		value&=mask
		if last<listLen:        
			try: #464 -1
				each=L[listLen - last - 1]
			except IndexError:
				print "listLen=",listLen,"last=",last
				print hex(mask)
			if (each&mask) == value:
				return last

		count = self.offsetdict.get((mask,value),-1)
		if count<= 0:
			return -1

		offset=(idx - count - 1)    
		if offset >= maxLookBackLen:
			return -1
		else:
			self.lastOffsetFromEndOfList[mask]=offset

		return offset

	def findFlip1(self,L,maxOffset,instr):
		for count,each in enumerate(reversed(L), start=0):
			if count == maxOffset:
				return (-1,-1)

			delta=each^instr
			if ((delta&0xFFFF0000)==0):        
				zeros = self.findFlip1Lookup[delta]
				if zeros != -1:
					return(count, zeros)    

		return(-1,-1)	
	
	def findFlip1New(self,L,maxOffset,instr):
		mask = 0xFFFFFFFF
		for zeros in xrange(16):
			zeros=15-zeros
			flippedInstr = instr ^ (1<<zeros)
			try:
				offset = self.offsetdict[(mask,flippedInstr)]
				count = len(L) - offset - 1
				if count >= maxOffset:
					return (-1,-1)
				#print("instr=0x%x flippedBit=%d matchInList=0x%x"%(instr,zeros,L[offset]))
				return (count,zeros)
			except KeyError:
				pass
		return (-1,-1) 
		
	
	
	def findFlip2(self,L,maxOffset,instr):
		for count, each in enumerate(reversed(L),start=0):
			if count == maxOffset:
				return (-1,-1,-1)

			delta=each^instr
			try:
				lookup = self.findFlip2Lookup[delta]
				if lookup != -1:
					return (count, lookup[0], lookup[1])
			except KeyError:
				pass

		return(-1,-1,-1)

	lastLookback=None
	def pushLookbackCompressedBits(self,offset,lookBackLenBits):
		lookmatch=0
		if (self.lastLookback != None) and (self.lastLookback==offset):        
			self.pushCompressedBits(1,1)
			lookmatch=1
		else:
			self.pushCompressedBits(0,1)
			self.pushCompressedBits(offset,lookBackLenBits)
			lookmatch=0
		self.lastLookback=offset
		return lookmatch

	compressedPartialWord=0
	compressedPartialBits=0

	def pushCompressedBits(self,bits,numBits):

		#print " ", "{0:#0{1}b}".format(bits,numBits+2),

		self.compressedPartialWord |= bits<<self.compressedPartialBits
		self.compressedPartialBits += numBits

		if self.compressedPartialBits >= 32:
			compressedWord = self.compressedPartialWord & 0xFFFFFFFF
			self.compressedPartialWord >>= 32
			self.compressedPartialBits -= 32
			word=struct.pack('I', compressedWord & 0xFFFFFFFF)
			self.blockStream.append(word)


	def compress(self,block):
		lookBackLen = self.LOOKBACK_LEN
		lookBackLenBits = self.LOOKBACK_BITS
		tableNumBits = self.DICTIONARY_BITS
		windowLength = self.BLOCK_SIZE

		self.offsetdict.clear()
		self.lastOffsetFromEndOfList.clear()
		self.lastLookback=None

		lastInstrLookback=[]
		lastInstr=None
		masks = [0xFFFFFFFF,0xFFFFFF00, 0x1fffff000,0x00000FFF,0xFFFF00FF,0xFF00FFFF,0xFFFFF000,0xFFF0FF00,0xF00FFFF0,0xFFFE0000,0x0001FE00,0x000001FF,0xFF0FF000,0xFFFF0000,0x0000FFFF,0b111011000011110011111101]
		first_dict_entry = True

#		print "q6zip3 lite"
		for n,instr in enumerate(block):


			#############################################
			if lastInstr != None:
				
				for m in masks:
					self.offsetdict[(m,lastInstr&m)] = len(lastInstrLookback)
				lastInstrLookback.append(lastInstr)
					
			lastInstr=instr    
			
			"""
			print "[",
			for x in lastInstrLookback[-5:-1]:
				print "{0:#0{1}x},".format(x,10),
			print "]",
			
			
			print "\n", "{0:#0{1}x}".format(instr,10),
			"""
			
			#############################################
			offset=self.findOffsetFromEndOfList(lastInstrLookback,lookBackLen,instr,0xFFFFFFFF)
						
			if offset!=-1:
				self.pushCompressedBits(0b00,2)
				#self.pushCompressedBits(offset,lookBackLenBits)
				lookmatch=self.pushLookbackCompressedBits(offset,lookBackLenBits)
				continue
			#############################################
			try:
				index=self.wordSymbolByteDict[instr]
				# print " ", index,
				if first_dict_entry:
					first_dict_entry = False
					self.pushCompressedBits(0b11,2)
					self.pushCompressedBits(0b111,3)
					self.pushCompressedBits(0b11,2)
					self.pushCompressedBits(2, lookBackLenBits)

				self.pushCompressedBits(0b01,2)
				self.pushCompressedBits(index,tableNumBits)
				continue
			except KeyError:
				pass
			#############################################
			# (offset,bit)=self.findFlip1(lastInstrLookback,lookBackLen,instr)

			# if offset!=-1:
				# self.pushCompressedBits(0b11,2)
				# self.pushCompressedBits(0b011,3)
				# self.pushCompressedBits(offset,lookBackLenBits)
				# self.pushCompressedBits(bit,4)
				# continue
			# #############################################
			# (offset,bit1,bit2)=self.findFlip2(lastInstrLookback,lookBackLen,instr)

			# if offset!=-1:
				# self.pushCompressedBits(0b11,2)
				# self.pushCompressedBits(0b010,3)
				# self.pushCompressedBits(offset,lookBackLenBits)
				# self.pushCompressedBits(bit1,4)
				# self.pushCompressedBits(bit2,3)
				# continue
			#############################################
			offset=self.findOffsetFromEndOfList(lastInstrLookback,lookBackLen,instr,0xFFFFFF00)

			if offset!=-1:
				self.pushCompressedBits(0b11,2)
				self.pushCompressedBits(0b000,3)
				self.pushCompressedBits(offset,lookBackLenBits)
				self.pushCompressedBits(instr&0xFF,8)
				continue
			#############################################
			offsetMs=self.findOffsetFromEndOfList(lastInstrLookback,lookBackLen,instr,0x1FFFFF000)
			if (offsetMs!=-1):            
				offsetLs=self.findOffsetFromEndOfList(lastInstrLookback,lookBackLen,instr,0x00000FFF)            
				if (offsetLs!=-1):
					self.pushCompressedBits(0b11,2)
					self.pushCompressedBits(0b111,3)
					self.pushCompressedBits(0b01,2)
					self.pushCompressedBits(offsetMs,lookBackLenBits)
					self.pushCompressedBits(offsetLs,lookBackLenBits)
					continue
			############################################
			offset=self.findOffsetFromEndOfList(lastInstrLookback,lookBackLen,instr,0xFFFF00FF)

			if offset!=-1:
				self.pushCompressedBits(0b11,2)
				self.pushCompressedBits(0b110,3)
				self.pushCompressedBits(0b00,2)
				self.pushCompressedBits(offset,lookBackLenBits)
				self.pushCompressedBits((instr>>8)&0xFF,8)
				continue            
			#############################################
			offset=self.findOffsetFromEndOfList(lastInstrLookback,lookBackLen,instr,0xFF00FFFF)

			if offset!=-1:
				self.pushCompressedBits(0b11,2)
				self.pushCompressedBits(0b110,3)
				self.pushCompressedBits(0b11,2)
				self.pushCompressedBits(offset,lookBackLenBits)
				self.pushCompressedBits((instr>>16)&0xFF,8)
				continue
			#############################################
			offset=self.findOffsetFromEndOfList(lastInstrLookback,lookBackLen,instr,0xFFFFF000)

			if offset!=-1:
				self.pushCompressedBits(0b11,2)
				self.pushCompressedBits(0b001,3)
				self.pushCompressedBits(offset,lookBackLenBits)
				self.pushCompressedBits((instr&0xFFF),12)
				continue
			#############################################
			offset=self.findOffsetFromEndOfList(lastInstrLookback,lookBackLen,instr,0xFFF0FF00)

			if offset!=-1:
				self.pushCompressedBits(0b11,2)
				self.pushCompressedBits(0b110,3)
				self.pushCompressedBits(0b01,2)
				self.pushCompressedBits(offset,lookBackLenBits)
				self.pushCompressedBits(((instr>>16)&0xF),4)
				self.pushCompressedBits((instr&0xFF),8)
				continue
			#############################################
			offset=self.findOffsetFromEndOfList(lastInstrLookback,lookBackLen,instr,0xF00FFFF0)

			if offset!=-1:
				self.pushCompressedBits(0b11,2)
				self.pushCompressedBits(0b111,3)
				self.pushCompressedBits(0b10,2)
				self.pushCompressedBits(offset,lookBackLenBits)
				self.pushCompressedBits(((instr>>20)&0xFF),8)
				self.pushCompressedBits((instr&0xF),4)
				continue
			#############################################
			offsetTop=self.findOffsetFromEndOfList(lastInstrLookback,lookBackLen,instr,0xFFFE0000)
			if (offsetTop!=-1):
				offsetMid=self.findOffsetFromEndOfList(lastInstrLookback,lookBackLen,instr,0x0001FE00)
				if (offsetMid!=-1):
					offsetEnd=self.findOffsetFromEndOfList(lastInstrLookback,lookBackLen,instr,0x000001FF)
					if (offsetEnd!=-1):
						self.pushCompressedBits(0b11,2)
						self.pushCompressedBits(0b111,3)
						self.pushCompressedBits(0b00,2)
						self.pushCompressedBits(offsetTop,lookBackLenBits)
						self.pushCompressedBits(offsetMid,lookBackLenBits)
						self.pushCompressedBits(offsetEnd,lookBackLenBits)
						continue
			#############################################
			offset=self.findOffsetFromEndOfList(lastInstrLookback,lookBackLen,instr,0xFF0FF000)

			if offset!=-1:
				self.pushCompressedBits(0b11,2)
				self.pushCompressedBits(0b101,3)
				self.pushCompressedBits(offset,lookBackLenBits)
				self.pushCompressedBits((instr>>20)&0xF,4)
				self.pushCompressedBits((instr&0xFFF),12)
				continue
			#############################################
			offset=self.findOffsetFromEndOfList(lastInstrLookback,lookBackLen,instr,0xFFFF0000)

			if offset!=-1:
				self.pushCompressedBits(0b11,2)
				self.pushCompressedBits(0b100,3)
				self.pushCompressedBits(offset,lookBackLenBits)
				self.pushCompressedBits(instr&0xFFFF,16)
				continue
			#############################################
			offset=self.findOffsetFromEndOfList(lastInstrLookback,lookBackLen,instr,0x0000FFFF)

			if offset!=-1:
				self.pushCompressedBits(0b11,2)
				self.pushCompressedBits(0b110,3)
				self.pushCompressedBits(0b10,2)
				self.pushCompressedBits(offset,lookBackLenBits)
				self.pushCompressedBits((instr>>16)&0xFFFF,16)
				continue
			#############################################
			offset=self.findOffsetFromEndOfList(lastInstrLookback,lookBackLen,instr,0b111011000011110011111101)

			if offset!=-1 and offset != 1 and offset != 2: # end of stream

				self.pushCompressedBits(0b1111111,7)
				self.pushCompressedBits(offset,lookBackLenBits)
				self.pushCompressedBits(
					(((instr>>24)&0xFF) << 8) + \
					(((instr>>20)&0x1) << 7) + \
					(((instr>>14)&0xF) << 3) + \
					(((instr>>8)&0x3) << 1) + \
					(((instr>>1)&0x1) << 0), 16)
				continue
			#############################################
			self.pushCompressedBits(0b10,2)
			self.pushCompressedBits(instr&0xFFFFFFFF,32)
			continue
		#############################################


		#finish block
		self.pushCompressedBits(0b11,2)
		self.pushCompressedBits(0b111,3)
		self.pushCompressedBits(0b11,2)
		self.pushCompressedBits(1,lookBackLenBits)
		self.pushCompressedBits(0xADE5,16) # signature

		if self.compressedPartialBits>0:
			compressedWord = self.compressedPartialWord & 0xFFFFFFFF
			word=struct.pack('I', compressedWord & 0xFFFFFFFF)
			self.blockStream.append(word)
		self.compressedPartialBits = 0
		self.compressedPartialWord = 0
		self.compressedStream.append(self.blockStream)
		self.blockStream = []
