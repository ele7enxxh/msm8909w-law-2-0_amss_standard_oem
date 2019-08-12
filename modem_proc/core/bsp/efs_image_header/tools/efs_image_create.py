#==============================================================================
#
# EFS_IMAGE_CREATE
#
# GENERAL DESCRIPTION
#   Contains Utilities for efs image creation
#
# Copyright 2013 by QUALCOMM Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#------------------------------------------------------------------------------
#
#  $Header: //components/rel/core.mpss/3.5.c12.3/bsp/efs_image_header/tools/efs_image_create.py#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
#
#               EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
#  when      who    what, where, why
# ----------  -----  ---------------------------------------------------------
# 2013-02-25  reddyn  Remove sanity checks that fail in standalone targets.
# 2013-02-01  reddyn  Create.
#==============================================================================

import efs_image_header_gen
import efs_image_dictionary
import optparse
import os.path

EFS_HEADER_SIZE = 512
SIGNED_FILE_EXTENSION = ".img"

def partition_string_lookup(x):
    return {
        "FS1": "MODEM_RAM",
        "FS2": "SHARED_RAM",
        "FSG": "GOLDEN_RAM",
        }.get(x, "")

if __name__ == "__main__":
    parser = optparse.OptionParser("usage: %prog [options] <efs_image_meta_file> <input_file> \n" +
                                    "Try %prog -h' for more information.")
    parser.add_option("-o", "--output", dest="output_file",
                      default="", type="string",
                      help="The name of output file")
    parser.add_option("-p", "--partition", dest="partition", default="FSG",
                      type="string", help="Destination partition for the Image." +
                       "Valid values are FS1, FS2 or FSG [Defaults to FSG]")

    (options, args) = parser.parse_args()

    if len(args) != 2:
        parser.error("Please supply meta_file and input_File")

    partition_for_image = options.partition
    output_file = options.output_file

    dictionary_file = args[0]
    input_file = args[1]

    if (not os.path.isfile(input_file)):
        parser.error("Cannot find input_file:" + input_file)

    if (not os.path.isfile(dictionary_file)):
        parser.error("Cannot find meta_file:" + dictionary_file)

    if (not output_file):
        output_file = input_file + SIGNED_FILE_EXTENSION

    partition_string = partition_string_lookup (partition_for_image)
    if (not partition_string):
        parser.error("Invalid Partition : " + partition_for_image)

    efs_image_dic = efs_image_dictionary.Efs_Image_Dict ()
    efs_image_dic.parse_dict_file (dictionary_file)

    partition_ram_addr = efs_image_dic.lookup_key(partition_string + "_ADDR")
    partition_size = efs_image_dic.lookup_key(partition_string + "_SIZE")

    input_file_size = os.path.getsize(input_file)
    image_size = input_file_size + EFS_HEADER_SIZE

    efs_image_header_gen.generate_elf_header_file(output_file,
                                                  partition_ram_addr,
                                                  image_size,
                                                  EFS_HEADER_SIZE,
                                                  "SIGNED_IMAGE",
                                                  partition_for_image)
    fout = open(output_file, "a+b")
    fin = open (input_file, "rb")
    fout.write(fin.read())
    fin.close()
    fout.close()
