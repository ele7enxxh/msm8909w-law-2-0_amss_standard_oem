#File:      q6zip_ro_uncompress.py
#Copyright: (C) 2014.
#Author:    hzhi

#This script is interface for T32 to invoke q6zip_ro_uncompress executable.


import os
import subprocess
import sys
from sys import platform as _platform


def q6zip_ro_decompress(compressed_file, dict_file, decompressed_file):
#{{{
    print "Running q6zip_ro_decompress..."
    curr_dir = os.getcwd()
    # linux
    if _platform.startswith('linux'):
        executable_path = os.path.join(curr_dir, "q6zip_ro_decompressor")
        if os.path.isfile( executable_path ):
            print "Find executable for Linux, executing..."
            retCode = subprocess.call([str(executable_path), compressed_file, dict_file, decompressed_file])
            if retCode == 0:
                print "RO decompression on Linux platform done!!!"
            else:
                print "RO decompression on Linux platform failed!!!"
        else:
            print "No executable for Linux found, aborting..."
            sys.exit(2)

    # windows
    elif _platform.startswith('win'):
        executable_path = os.path.join(curr_dir, "q6zip_ro_decompressor.exe")
        if os.path.isfile( executable_path ):
            print "Find executable for Windows, executing..."
            retCode = subprocess.call([str(executable_path), compressed_file, dict_file, decompressed_file])
            if retCode == 0:
                print "RO decompression on Windows platform done!!!"
            else:
                print "RO decompression on Windows platform failed!!!"
        else:
            print "No executable for Windows found, aborting..."
            sys.exit(2)
    # OS X
    else :
        print "Current OS not supported, aborting..."
        sys.exit(2)

    return 0
#}}}

def main():
#{{{
    print "Running..."
    arguments_in = sys.argv[1:]
    if len(arguments_in) < 2 or len(arguments_in) > 3:
        print "Usage: python q6zip_ro_uncompress.py <COMPRESSED_BIN> <DICT.BIN> <DECOMPRESSED_BIN>"
        sys.exit(1)

    if len(arguments_in) == 3:
        decompressed_file = arguments_in[2]
    else:
        decompressed_file = "decompressed.bin"

    compressed_file = arguments_in[0]
    dict_file = arguments_in[1]

    #validate specified file existence. 
    if not os.path.isfile(compressed_file) or not os.path.isfile(dict_file):
        print "Input file does not exists."
        sys.exit(1)
    
    #file input.
    try:
        f_in = open(compressed_file, "rb")
        f_dict = open(dict_file, "rb")
    except: 
        print "Failed to read from input binary file, aborting..."
        sys.exit(2)

    print "Start to process input files." 
    
    f_in.close()
    f_dict.close()
    #file output.
    try:
        f_out = open(decompressed_file, "wb")
    except:
        print "Failed to write into output binary file, aborting..."
        sys.exit(2)

    f_out.close()
    q6zip_ro_decompress(compressed_file, dict_file, decompressed_file)
        
#}}}

def ctypes_dll (compressed_file, dict_file, decompressed_file):
#{{{
    #locate shared library file in the same directory.
    _file = "libnew_q6zip_uncompress.so"
    _path = os.path.join(*(os.path.split(__file__)[:-1] + (_file,)))
    _q6zip_ro = cdll.LoadLibrary(_path)
    print "DLL loaded!!!"
    
    #int q6zip_uncompress
    #(
    #   char* out_buf, int* out_buf_size, 
    #   char* in_buf, int in_buf_size, 
    #   char* dict
    #).

    q6zip_ro_decompress = _q6zip_ro.main
    q6zip_ro_decompress.argtypes = (ctypes.c_char_p, ctypes.c_char_p)
    q6zip_ro_decompress.restype = ctypes.c_int

    q6zip_ro_decompress(compressed_file, dict_file, decompressed_file)

    print "File processing done!!!"

#}}}

if __name__ == "__main__":
    print "Running q6zip_ro_decompress..."
    main()
