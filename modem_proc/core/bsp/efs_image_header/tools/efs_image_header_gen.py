#==============================================================================
#
# EFS IMAGE  HEADER GENERATION MODULE
#
# GENERAL DESCRIPTION
#   Contains Utilities for efs image header generation
#
# Copyright 2013 by QUALCOMM Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/core.mpss/3.5.c12.3/bsp/efs_image_header/tools/efs_image_header_gen.py#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
#
#               EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
#  when      who    what, where, why
# ----------  -----  ---------------------------------------------------------
# 2013-02-01  reddyn  Create.
#==============================================================================

import struct

#----------------------------------------------------------------------------
# GLOBAL VARIABLES BEGIN
#----------------------------------------------------------------------------

# MBN Definitions
MBN_HDR_SIZE = 40
MBN_HDR_VERSION = 3
EFS_HEADER_MAGIC = "IMGEFS-" # We add the '-' to ensure compatibility for
                             # golden copy check.

#----------------------------------------------------------------------------
# GLOBAL VARIABLES END
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# CLASS DEFINITIONS BEGIN
#----------------------------------------------------------------------------

"""
#----------------------------------------------------------------------------
# Header Class Notes:
# In order to properly read and write the header structures as binary data,
# the Python Struct library is used to align and package up the header objects
# All Struct objects are initialized by a special string with the following
# notation. These structure objects are then used to decode binary data in
# order to fill out the appropriate class in Python, or they are used to
# package up the Python class so that we may write the binary data out.
#----------------------------------------------------------------------------

      Format  | C Type        | Python Type | Standard Size
      -----------------------------------------------------
     1) I    | unsigned int  | integer     | 4 bytes

"""


#----------------------------------------------------------------------------
# The MBN Header Class
#----------------------------------------------------------------------------
class Efs_Mbn_Hdr:

    def __init__(self, partition_name, destination_address, image_offset,
                 image_size):
        """
        Populate the MBN image header for EFS partitions.
        """

        # The image id field to identify which image is being loaded.
        self.image_id = self.__lookup_image_id__(partition_name)
        # The version of the MI boot image header.
        self.header_version_num = MBN_HDR_VERSION
        # The offset of the image from the end of the header.
        self.image_offset_from_header = image_offset - MBN_HDR_SIZE
        # The destination address to which the image is to be loaded.
        self.image_dest_addr_ptr = destination_address
        # The size of the image.
        self.image_size = image_size
        self.code_size = image_size

        # Pointers to signature and certificate if authentication is desired.
        self.signature_ptr = 0
        self.signature_size = 0
        self.cert_chain_ptr = 0
        self.cert_chain_size = 0
        return

    def __lookup_image_id__(self, partition_name):
        image_id = {
                    "FS1": 16,
                    "FS2": 17,
                    "FSG": 20
                    }.get(partition_name)
        return image_id


    def getPackedData(self):
        # Structure object to align and package the EFS MBN Header
        s = struct.Struct('I' * 10)
        values = [self.image_id,
                  self.header_version_num,
                  self.image_offset_from_header,
                  self.image_dest_addr_ptr,
                  self.image_size,
                  self.code_size,
                  self.signature_ptr,
                  self.signature_size,
                  self.cert_chain_ptr,
                  self.cert_chain_size
                 ]
        return s.pack(*values)


def get_packed_string (raw_string):
    s = struct.Struct('s')
    return s.pack(raw_string)
# main
def generate_elf_header_file (header_file_name, partition_ram_addr, final_image_size,
                              header_size, magic_string, partition_name):

    image_offset = header_size
    magic_string = EFS_HEADER_MAGIC + " " + magic_string
    destination_address = 0
    image_size = 0

    if(partition_ram_addr != 0) :
        destination_address = partition_ram_addr + header_size

    if(final_image_size != 0) :
        image_size = final_image_size - header_size

    mbn_header_packed = Efs_Mbn_Hdr (partition_name, destination_address,
                                     image_offset, image_size).getPackedData()

    fout = open(header_file_name, 'wb')
    fout.write(mbn_header_packed)
    fout.write(magic_string)
    fout.seek(header_size - 1)
    fout.write("\x00")
    fout.close()

