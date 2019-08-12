#==============================================================================
#
# EFS IMAGE DICTIONARY GENERATION MODULE
#
# GENERAL DESCRIPTION
#   Contains Utilities for efs image dictionary  generation
#
# Copyright 2013 by QUALCOMM Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#------------------------------------------------------------------------------
#
#  $Header: //components/rel/core.mpss/3.5.c12.3/bsp/efs_image_header/tools/efs_image_dictionary.py#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
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
import csv

MAGIC_KEY = "EFS_IMAGE_KEYS"
MAGIC_VALUE = "VALUES"

#----------------------------------------------------------------------------
# EFS dictionary class
#----------------------------------------------------------------------------
class Efs_Image_Dict:

    efs_image_dict = dict()

    def parse_preprocessed(self, pre_processed):
        # Open pre processed file in line buffering.
        fin = open (pre_processed, "r", 1)

        self.efs_image_dict.clear()

        for line in fin:
            raw_line = line
            # Extract only the lines that start with the special string.
            valid_line = raw_line.find ("const unsigned int FS_")

            if valid_line != -1:
                keyvalue_pair = raw_line.replace ("const unsigned int FS_", "")
                key, val = keyvalue_pair.split ("=");

                # Generate the Key
                key = key.replace (" ", "")

                # Generate the value
                val = val.replace (";", "")
                val = val.replace (" ", "")
                val = int(val, 0);

                # Insert key and value into dictionary.
                self.efs_image_dict [key] = val

        fin.close()


    def lookup_key(self, key):
        # Lookup the value for the Key in the Dictionary.
        # Return zero if key not found
        return self.efs_image_dict.get(key, 0)

    def write_dictionary_to_file (self, dictionary_file):
        # Write the dictionary to a CSV file.
        csv_writer = csv.writer(open(dictionary_file, "wb"))
        # Insert a heading for the csv file
        csv_writer.writerow([MAGIC_KEY, MAGIC_VALUE])
        #Sort the keys.
        sorted_keys = sorted(self.efs_image_dict)
        for key in sorted_keys:
            val = self.efs_image_dict[key]
            if "ADDR" in key :
                val = hex(val) # Easier to represent address as hex values.
            csv_writer.writerow([key, val])
        return

    def parse_dict_file (self, dictionary_file):
        # Read the dictionary from a CSV file.
        self.efs_image_dict.clear()

        reader = csv.reader(open(dictionary_file, "rb"))
        for row in reader:
            key, val = row
            # Ignore the heading in the csv file
            if (key != MAGIC_KEY):
                self.efs_image_dict[key] = int (val, 0)

        return

