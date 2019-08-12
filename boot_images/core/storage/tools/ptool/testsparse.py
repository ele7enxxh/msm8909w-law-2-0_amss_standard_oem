#!/usr/bin/python
#===========================================================================

#  This script parses "rawprogram.xml" looking for sparse="true" tags
#  if found, it tries to find the sparse files and updates rawprogram.xml
#  such that msp.py, T32 and QPST can program them

# REFERENCES

#  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/tools/ptool/testsparse.py#1 $
#  $DateTime: 2015/09/01 00:30:35 $ 
#  $Author: pwbldsvc $

# when          who     what, where, why 
# --------      ---     ------------------------------------------------------- 
# 2011-08-17    ah      Original version

# Copyright (c) 2007-2010
# Qualcomm Technologies Incorporated.
# All Rights Reserved.
# Qualcomm Confidential and Proprietary
# ===========================================================================*/

import struct, os, sys
import re
import shutil
from types import *
import math
import struct

if sys.version_info < (2,5): 
    sys.stdout.write("\n\nERROR: This script needs Python version 2.5 or greater, detected as ")
    print sys.version_info
    sys.exit()  # error

from copy import deepcopy
import getopt


"""
typedef struct sparse_header {
  __le32	magic;		/* 0xed26ff3a */
  __le16	major_version;	/* (0x1) - reject images with higher major versions */
  __le16	minor_version;	/* (0x0) - allow images with higer minor versions */
  __le16	file_hdr_sz;	/* 28 bytes for first revision of the file format */
  __le16	chunk_hdr_sz;	/* 12 bytes for first revision of the file format */
  __le32	blk_sz;		/* block size in bytes, must be a multiple of 4 (4096) */
  __le32	total_blks;	/* total blocks in the non-sparse output image */
  __le32	total_chunks;	/* total chunks in the sparse input image */
  __le32	image_checksum; /* CRC32 checksum of the original data, counting "don't care" */
                /* as 0. Standard 802.3 polynomial, use a Public Domain */
                /* table implementation */
} sparse_header_t;
typedef struct chunk_header {
  __le16	chunk_type;	/* 0xCAC1 -> raw; 0xCAC2 -> fill; 0xCAC3 -> don't care */
  __le16	reserved1;
  __le32	chunk_sz;	/* in blocks in output image */
  __le32	total_sz;	/* in bytes of chunk input file including chunk header and data */
} chunk_header_t;
"""
# Struct related definitions
sparse_header_t = struct.Struct('<LHHHHLLLL')
chunk_header_t = struct.Struct('<HHLL')
SPARSE_HEADER_MAGIC = 0xed26ff3a
CHUNK_TYPE_RAW = 0xCAC1
CHUNK_TYPE_FILL = 0xCAC2
CHUNK_TYPE_DONT_CARE = 0xCAC3
_LARGEFILE64_SOURCE = 1
_FILE_OFFSET_BITS = 64
COPY_BUF_SIZE = (1024*1024)
#This will be malloc'ed with the size of blk_sz from the sparse file header
SPARSE_HEADER_MAJOR_VER = 1
SPARSE_HEADER_LEN = sparse_header_t.size
CHUNK_HEADER_LEN = chunk_header_t.size

def usage():
    print """
Usage: python simg2img.py [OPTION...]
Parses the sparse images from the input xml and generates the output chunks
as well as the new xml.

Examples:
  # Look for sparse images in rawprogram0.xml, create update rawprogram0.xml

  Options:

  -i, --input=FILE           rawprogram0.xml
  -o, --output=FILE          new rawprogram0.xml
  -s, --search_path=PATH     path to sparse image
  -t, --location=PATH        path to store the unsparsed images
  -?, --help                 give this help list

  Example

  python checksparse.py -i rawprogram0.xml -s C:\path1 -s C:\path2 -s C:\path3
  python checksparse.py -i rawprogram0.xml -s C:\path1 -s C:\path2 -o rawprogram00.xml

    """

def PrintBigError(sz):
    print "\t _________________ ___________ "
    print "\t|  ___| ___ \\ ___ \\  _  | ___ \\"
    print "\t| |__ | |_/ / |_/ / | | | |_/ /"
    print "\t|  __||    /|    /| | | |    / "
    print "\t| |___| |\\ \\| |\\ \\\\ \\_/ / |\\ \\ "
    print "\t\\____/\\_| \\_\\_| \\_|\\___/\\_| \\_|\n"

    if len(sz)>0:
        print sz
        sys.exit(1)


def process_raw_chunk(fp_in, fp_out, blocks, blk_sz): 
    length = (1L * blocks) * blk_sz
    chunk = None

    while length!=0:
        if length > COPY_BUF_SIZE:
            chunk = COPY_BUF_SIZE
        else:
            chunk = length
        copybuf = fp_in.read(chunk)
        fp_out.write(copybuf)
        length -= chunk

def EnsureDirectoryExists(filename):
    dir = os.path.dirname(filename)

    try:
        os.stat(dir)
    except:
        os.makedirs(dir)

def main():
    if len(sys.argv) < 2:
        usage()
        sys.exit(-1)
    try:
        opts, args = getopt.getopt(sys.argv[1:], "i:s:", ["input=", "search_path="])
    except getopt.GetoptError, err:
        # print help information and exit:
        print str(err) # will print something like "option -a not recognized"
        usage()
        sys.exit(-2)


    FileToTest              = None
    paths = []
    for o, a in opts:
        if o in ("-i", "--input"):
            FileToTest = a
        elif o in ("-s", "--search_path"):
            paths.append(a)
        else:
            assert False, "unhandled option"

    if FileToTest is None:
        print "\nERROR: No file to test was provided\n"
        print "Ex:\tpython testsparse.py -i userdata.img.ext4 -s \\some\\path\n"
        sys.exit(0)

    filename = find_file(FileToTest, paths)
    if filename is None:
        PrintBigError("ERROR: Could not find file")

    results = TestIfSparse(filename)
    if results == -1:
        sys.exit(0)
    else:
        sys.exit(1)


def TestIfSparse(filename):
    # Misc variable declarations
    in_fp = None
    out_fp = None
    sparse_header = {}
    chunk_header = {}
    total_blocks = 0
    chunk_number = 1
    file_name = None
    file_ext = None
    results = []
    temp = None

    basename, file_ext = os.path.splitext(filename)
    file_name = os.path.basename(basename)
    in_fp = open(filename, "rb")

    all_bytes = sparse_header_t.unpack(in_fp.read(SPARSE_HEADER_LEN))
    sparse_header['magic'] = all_bytes[0]
    sparse_header['major_version'] = all_bytes[1]
    sparse_header['minor_version'] = all_bytes[2]
    sparse_header['file_hdr_sz'] = all_bytes[3]
    sparse_header['chunk_hdr_sz'] = all_bytes[4]
    sparse_header['blk_sz'] = all_bytes[5]
    sparse_header['total_blks'] = all_bytes[6]
    sparse_header['total_chunks'] = all_bytes[7]
    sparse_header['image_checksum'] = all_bytes[8]

    if sparse_header['magic'] != SPARSE_HEADER_MAGIC:
        print "Bad magic ", sparse_header['magic'], " '%s' is probably not a sparse image\n" % filename
        return -1

    if sparse_header['major_version'] != SPARSE_HEADER_MAJOR_VER:
        print "Unknown major version number ", sparse_header['major_version']
        return -1

    print "\n** SPARSE FILE DETECTED **\n"
    return 1

def find_file(filename, search_paths):
    print "\n\n\tLooking for",filename
    print "\t"+"-"*40
    for x in search_paths:
        #print "\tSearching ",x
        temp = os.path.join(x, filename)
        print "\tSearching for **%s**" % temp
        if os.path.exists(temp):
            print "\n\t**Found %s (%i bytes)" % (temp,os.path.getsize(temp))
            return temp

    print "\n\t** Not found in search paths, now checking CWD\n"
    ## search cwd last
    print "\tSearching ",os.getcwd()
    if os.path.exists(filename):
        print "\n\t**Found %s (%i bytes)" % (filename,os.path.getsize(filename))
        return filename

    print "\tCound't find file OR perhaps you don't have permission to run os.stat() on this file\n"
    return None


if __name__ == "__main__":
    main()
