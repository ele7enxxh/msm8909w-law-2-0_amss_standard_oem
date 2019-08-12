import sys
import struct
import rw_py_compress

debug_enabled = True
temp_folder   = 'c:\\temp\\'

##
# @brief    Console debug control
# @param    Message to print
def debug(arg):

    if (debug_enabled):
        print arg
        sys.stdout.flush()
 
def decompress_n(n):
    debug("Starting... n " + str(n))
    n2 = str(n)
    debug("n: " + str(n) + ", n2: " + n2)
    compressed_file_name = temp_folder + 'compressedrw' + n2 + '.bin'
    decompressed_file_name = temp_folder + 'decompressedrw' + n2 + '.bin'
    debug("compressed_file_name: " + compressed_file_name)
    debug("decompressed_file_name: " + decompressed_file_name)

    # Input
    try:
         compressed_file = open(compressed_file_name, 'rb')
    except:
         print "1 Error opening file " + compressed_file_name + ", aborting."
         sys.exit(2)
     
    # Output
    try:
        decompressed_file = open(decompressed_file_name, 'wb')
    except:
        print "2 Error opening file " + decompressed_file_name + ", aborting."
        sys.exit(2)

    debug("Reading file...")
    # Process 
    compressed = []
    while (1):
        line = compressed_file.read(4)
        if (len(str(line)) == 0): # end of file
            break
        compressed.append(struct.unpack('I', line)[0])
     
    debug("compressed_size " + str(len(compressed)))
    decompressed = []
    debug("Starting delta Uncompress...")
    rw_py_compress.deltaUncompress(compressed, decompressed)
    debug("Delta Uncompress completed")

    debug("decompressed_size " + str(len(decompressed)))
    if (len(decompressed) != 1024):
         print "Incorrect decompressed size " + str(len(decompressed))
         exit(-1)
    debug("Writing file...")
    for i in decompressed:
         decompressed_file.write(struct.pack('I', i))
    debug("File write completed")
          
    compressed_file.close()
    decompressed_file.close()
    debug("Done")

##
# @brief    Main entry point
#
def main():
    debug("Total blocks: " + str(int(sys.argv[1], 16)))
    i = int(sys.argv[1])
    #for i in range(int(sys.argv[1], 16)):
    decompress_n(i)
    
if __name__ == "__main__":
    main()
     
