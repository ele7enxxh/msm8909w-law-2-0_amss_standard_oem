#!/usr/bin/python

import struct, os, sys, getopt
import math



    if start_sector<0:
        # Means do this NUM_DISK_SECTORS - start_sector
        start_sector = (AvailablePartitions[disk_name]/512) + start_sector # remember start_sector is negative, so add it

    if verbose:
        device_log("Moving to sector %i" % start_sector)
    try:
        opfile.seek(int(start_sector*512))
    except:
        PrintBigError("Could not move to sector %d in %s" % (start_sector,disk_name))

    temp = [0]*(512*num_sectors)
    bytes_read = struct.pack("%iB"%(512*num_sectors),*temp)
    if verbose:
        device_log("Writing %i zeros" % len(temp))
    try:
        opfile.write(bytes_read)
    except:
        PrintBigError("Could not write at sector %d in %s" % (Zeroout['start_sector'],disk_name))

    try:
        opfile.close()
    except:
        device_log("\tWARNING: Could not close %s" % disk_name)
        pass

try:
    opts, args = getopt.getopt(sys.argv[1:], "b:", ["bytes="])
except getopt.GetoptError, err:
    # print help information and exit:
    sys.exit()

for o, a in opts:
    if o in ("-b", "--bytes"):
        rawprogram_filename = a
    else:
        assert False, "unhandled option"


