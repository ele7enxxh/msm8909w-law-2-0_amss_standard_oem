#! /usr/bin/env python
import sys
import struct
from q6zip_constants import *

BLOCK_SIZE = 1024

debug_enabled = False

##
# @brief    Console debug control
# @param    Message to print
def debug(arg):

    if (debug_enabled):
        print arg
        sys.stdout.flush()

def pushCompressedBits(bits,numBits,compressedWordStream):
    global numCompressedWords
    global compressedPartialWord
    global numCompressedPartialBits
    temp = bits << numCompressedPartialBits                                 
    numCompressedPartialBits += numBits                                                                                        
    compressedPartialWord = compressedPartialWord | temp   
    if numCompressedPartialBits >= 32:                                                                               
        compressedWordStream.append(compressedPartialWord & 0xFFFFFFFF)
        numCompressedWords += 1                                              
        compressedPartialWord = compressedPartialWord >> 32   
        numCompressedPartialBits -= 32              

def finalizeCompressedBits(compressedWordStream,compressedPartialWord,numCompressedPartialBits):  
    global numCompressedWords
    if numCompressedPartialBits > 0:                                                                                                                                                                                                    
        compressedWordStream.append(compressedPartialWord & 0xFFFFFFFF)  
        numCompressedWords += 1   

def checkAnchor(anchor,val,compressed):
    global anchors
    anchor_val = anchors[anchor]; 
    if anchors[anchor] == val: 
        pushCompressedBits((anchor << 2) + 1,2 + 2,compressed)
        return 1
    elif ((anchor_val & 0xFFFFFC00) == (val & 0xFFFFFC00)):
        pushCompressedBits(((val&0x3FF)<<(2+2))+((anchor<<2)+2),2+2+10,compressed)
        anchors[anchor] = val; 
        return 1
    else:
        return 0

def GET_BITS(hold,n):
     return hold & ((1 << n) - 1)
def SKIP_BITS_W_CHECK(compressed,size,n): 
    global numAvailBits   
    global hold
    global in_index
    numAvailBits -= n                                             
    hold = hold >> n                                                  
    if numAvailBits <= 32: 
        if in_index != size:
            hold |= compressed[in_index] << numAvailBits              
            in_index += 1                                                                         
            numAvailBits += 32                                
def deltaCompress (uncompressed,compressed):
    global numCompressedWords
    global compressedPartialWord
    global numCompressedPartialBits
    global anchors
    anchors = [0,0,0,0]
    numCompressedWords = 0
    compressedPartialWord = 0
    numCompressedPartialBits = 0
    anchorIndex = 3
    for i in xrange(len(uncompressed)-1):
        val = uncompressed[i]
        if (val == 0):
            pushCompressedBits(0,2,compressed)
        else:
            anchor = 0
            if checkAnchor(anchor,val,compressed) == 1:
                continue
            anchor = (anchor + 1) & (4 - 1)
            if checkAnchor(anchor,val,compressed) == 1:
                continue
            anchor = (anchor +1 ) & (4 - 1)
            if checkAnchor(anchor,val,compressed) == 1:
                continue
            anchor = (anchor + 1 ) & (4 - 1)
            if checkAnchor(anchor,val,compressed) == 1:
                continue
            anchorIndex = (anchorIndex + 1) & (4 - 1)
            anchors[anchorIndex] = val
            pushCompressedBits(3,2,compressed)
            pushCompressedBits(val,32,compressed)
    val = uncompressed[len(uncompressed)-1]
    pushCompressedBits(3,2,compressed)
    pushCompressedBits(val,32,compressed)
    finalizeCompressedBits(compressed,compressedPartialWord,numCompressedPartialBits)
    return numCompressedWords

def deltaUncompress(compressed,uncompressed):
    debug("deltaUncompress")
    global numAvailBits   
    global hold
    global in_index
    anchors = [0,0,0,0]
    anchorIndex = 3
    numAvailBits = 0
    hold = 0
    in_index = 0
    out_index = 0
    compressed_size = len(compressed)
    SKIP_BITS_W_CHECK(compressed,compressed_size,0)
    # if size != 1:
        # __SKIP_BITS_W_CHECK(compressed,compressed_size,0)
    while out_index < BLOCK_SIZE:
        code = GET_BITS(hold,2)
        if code == 0:
            SKIP_BITS_W_CHECK(compressed,compressed_size,2)
            uncompressed.append(0)
            out_index += 1
        elif code == 1:
            anchor = GET_BITS(hold>>2,2)
            SKIP_BITS_W_CHECK(compressed,compressed_size,2+2)
            uncompressed.append(anchors[anchor])
            out_index += 1
        elif code == 2:
            anchor = GET_BITS(hold>>2,2)
            delta = GET_BITS(hold>>(2+2),10)
            SKIP_BITS_W_CHECK(compressed,compressed_size,2+2+10)
            val = (anchors[anchor] & 0xFFFFFC00) | delta
            uncompressed.append(val)
            out_index += 1
            anchors[anchor] = val
        elif code == 3:
            SKIP_BITS_W_CHECK(compressed,compressed_size,2)
            val = hold & 0xFFFFFFFF
            SKIP_BITS_W_CHECK(compressed,compressed_size,32)
            uncompressed.append(val)
            out_index += 1
            anchorIndex = (anchorIndex + 1) & (4 - 1)
            anchors[anchorIndex] = val 
    return out_index

def main():
    if len(sys.argv) > 1:
        uncompressed = []
        with open(sys.argv[1], 'rb') as f:
            while True:
                word = f.read(4)
                if len(word) != 4:
                    break
                uncompressed.append( struct.unpack('I', word)[0] )
    else:
        uncompressed = [0xFFFFFFFF] * 1024
    print "len of uncompressed = %d"%(len(uncompressed))
    print "uncompressed[%d] = 0x%x"%(len(uncompressed)/2, uncompressed[len(uncompressed)/2]) 
    compressed = []
    compressed_size = deltaCompress(uncompressed,compressed)
    print "compressed len words = %d"%(compressed_size)
    print "compression ratio = %f" % (float(len(compressed))/len(uncompressed))
    new_uncompressed = []
    uncompressed_size = deltaUncompress(compressed,new_uncompressed)
    print "uncompressed len words = %d"%(uncompressed_size)
    print "uncompressed[%d] = 0x%x"%(len(uncompressed)/2, uncompressed[len(uncompressed)/2])


def rw_py_compress(page_size=BLOCK_SIZE*4, VA_start=0, input=None):
    instrList=[]
    for word in (input[i:i+4] for i in xrange(0,len(input),4)):
        if len(word) == 4:
            instrList.append( struct.unpack('I',word)[0] )

    n_blocks = len(instrList)/BLOCK_SIZE
    print "n_blocks of RW = %d"%(n_blocks)
    v_addrs = []
    va = VA_start + 2 + 2 + 4 * n_blocks  #2 bytes for n_blocks, 2 for version, 4 per block start addr

    compressed_text = []
    for block in xrange(n_blocks):
        v_addrs.append( struct.pack('I',va) )
        compressed = []
        #print "calling deltaCompress, block = %d"%(block)
        deltaCompress(instrList[block*BLOCK_SIZE:(block+1)*BLOCK_SIZE],compressed)
        #print "compressed len = %d"%(len(compressed))
        for word in compressed:
            compressed_text.append(struct.pack('I',word))
        va += 4 * len(compressed)

    metadata = [struct.pack("H",n_blocks), struct.pack("H",Q6ZIP_RW_ALGORITHM_VERSION)]
    metadata += v_addrs
    metadata += compressed_text

    return ''.join(metadata)  #joins list elements together as string with no spaces

if __name__ == "__main__":
    main()
