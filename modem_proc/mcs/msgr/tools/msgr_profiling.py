# --------------------------------------------------------------------------- #
# File Name :: msgr_umid_log_parser.py
# Purpose   :: Parses the output from wp.v.w for msgr_log_xxxx_table logs and 
# outputs a csv. The output file is placed in the same directory as the first 
# datafile. Use the -h option on the command line for more details.
# --------------------------------------------------------------------------- #

# Copyright (c) 2013 Qualcomm Technologies Incorporated.
# All Rights Reserved.
# Qualcomm Confidential and Proprietary

# Export of this technology or software is regulated by the U.S. Government.
# Diversion contrary to U.S. law prohibited.

# All ideas, data and information contained in or disclosed by
# this document are confidential and proprietary information of
# Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
# By accepting this material the recipient agrees that this material
# and the information contained therein are held in confidence and in
# trust and will not be used, copied, reproduced in whole or in part,
# nor its contents revealed in any manner to others without the express
# written permission of Qualcomm Technologies Incorporated.
# --------------------------------------------------------------------------- #

#==============================================================================
#
#                        EDIT HISTORY FOR MODULE
#
# This section contains comments describing changes made to the module.
# Notice that changes are listed in reverse chronological order.
#
# $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/msgr/tools/msgr_profiling.py#1 $
#
# when       who    what, where, why
# --------   ---    -----------------------------------------------------------
# 08/09/10   gbs    converted from msgr_umid_log_parser.py
# 07/09/10   gbs    directory access added, changed output format, added move
# 07/08/10   gbs    file created
#
#==============================================================================


# --------------------------------------------------------------------------- #
#                       IMPORT MODULES & DEFINITONS                           #
# --------------------------------------------------------------------------- #
import sys
import os
import time
import re
from optparse import OptionParser


def main():
     # This is the start of the program
     # it is assumed that each of the log variables will have the same structure
     # created by "wp.v.w %f msgr_profiling_data" with any lines that don't 
     # contain actual data (lines without numeric values) removed from the output

     if None == options.filename:
          print "Use " + sys.argv[0] + " -h for help"
          sys.exit()

     # find all of the entries in each files
     entry_array = []
     for file in options.filename:
          try:
               in_file = open( file )
               line = in_file.readline()

               while "" != line:
                    catx = -1
                    i = 0

                    temp = get_name_field_of_line( line )
                    if "" == temp:
                         continue
                    else:
                         # entry_array.append( temp )
                         # find the categories

                         for entry in entry_array:

                              if temp == entry[0]:
                                   catx = i
                                   break
                              else:
                                   i = i + 1
                         if -1 == catx:
                              entry_array.append([temp])

                    temp = get_value_field_of_line( line )
                    entry_array[i].append( temp )
                    line = in_file.readline()

               in_file.close()
          except IOError:
               print "ERROR: Could not open file " + file + ", check spelling and that the file exists"

     # if data was found, continue on
     if [] != entry_array:
          # convert to non hex
          for x in entry_array:
               x[1:] = convert_numbers( x[1:] )

          # save the parsed data
          # create the file name to save the data to
          output_name = "c:\profile_log" + str(time.time()) + ".csv"

          # remove the \ if it is the first character
          # the r"\" is used to match a '\', it tells python to ignore special characters
          # '\' must still be escaped for the R.E.
          if "" != re.match(r"\\", output_name) and "" == re.match(r"\\\\", output_name): 
               output_name = re.sub(r"^\\", "", output_name) 
          print "Output to " + output_name

          # output to csv file
          out_file = open(output_name, "w")

          # make sure file opened
          try:
               # output a UMID log entry per row
               for x in range( 0, len( entry_array[0] )):

                    # output each element for each entry
                    for y in range( 0, len(entry_array)):
                         out_file.write( entry_array[y][x] + ",")
                    out_file.write("\n")
          except IOError:
               print "ERROR: Could not write to " + output_name
               sys.exit(1)

          out_file.close()
     else:
          print "ERROR: No data was found.  Make sure your files have been created"
          print     "\tusing 'wp.v.w <UMID_log_var>'"
          sys.exit(1)


# --------------------------------------------------------------------------- #
#                         FUNCTION DEFINITIONS                                #
# --------------------------------------------------------------------------- #
def convert_numbers( list_to_convert ):
     # Finds category str_to_convert_on and then converts the hex values into
     #  base 10 values if the category exits and the entries are valid data to convert
     # precondition: list_to_convert must not be empty and the first entry must
     #  contain the list of data types
     # return: None

     # convert to an int
     for x in range(0, len( list_to_convert )):
          #for y in x :
          list_to_convert[x] = int(list_to_convert[x], 0)

     # convert back to ascii to be able to write to file
     for x in range(0, len( list_to_convert )):
          #for y in x:
          list_to_convert[x] = str( list_to_convert[x] )

     return list_to_convert


def parse_args():
     # Parses the command line arguments and returns them using optparse
     # precondition: None
     # return: the return of parse_args()

     parser = OptionParser()
     parser.add_option( "-f", "--file", dest="filename", action="append", type="string",
                        help="""FILENAME should be a file created by 'wp.v.w %f
                              msgr_profiling_data' that you wish to parse for data.  
                              All lines of the output that do not have numberical
                              values must be deleted from the output file""" )
     parser.add_option( "-v", "--verbose", dest="verbose", action="store_true",
                        default=False, help="output information as script parses")
     return parser.parse_args()

def get_name_field_of_line( line ):
     # parses file_hd for the variable fields in a wp.v.w PRACTICE command on 
     #  a msgr UMID log variable, only takes action on an empty list
     # precondition: file_hd must be an already opend file
     # return: None
     
     # split the first line of the file
     entries = line
     if [] != entries:

          # find the categories of each data field
          single_entry = re.split("[ =]", entries)
          if [] == single_entry:
               return ""

          # parse element by element of each entry
          for y in single_entry[:]:
               # seperate name from data
               # element = re.split(" = ", y)

               # find non-empty elements from split
               if y == "":
                    single_entry.remove("")
               else:
                    # search for the name, the first non-empty string of each element
                    #for x in element:
                     #    if x == "":
                      #        element.remove("")
                       #  else:
                    if None == re.match("^0x", y):
                         return y.lstrip().rstrip()

          return ""

     return ""


def get_value_field_of_line( line ):
     # parses file_hd for the variable fields in a wp.v.w PRACTICE command on 
     #  a msgr UMID log variable, only takes action on an empty list
     # precondition: file_hd must be an already opend file
     # return: None
     
     # split the first line of the file
     entries = line
     if [] != entries:

          # find the categories of each data field
          single_entry = re.split("[ =,)]", entries)
          if [] == single_entry:
               return ""

          # parse element by element of each entry
          for y in single_entry[:]:
               # seperate name from data
               # element = re.split(" = ", y)

               # find non-empty elements from split
               if y != "":
                   # single_entry.remove("")
               #else:
                    # search for the name, the first non-empty string of each element
                    #for x in element:
                     #    if x == "":
                      #        element.remove("")
                       #  else:
                              if None != re.match("^0x", y):
                                   return y.lstrip().rstrip()
                              #else:
                               #    single_entry.remove("")

          return ""

     return ""


# --------------------------------------------------------------------------- #
#                             ENTRY POINT                                     #
# --------------------------------------------------------------------------- #

print "Running " + sys.argv[0] + "\n"
cur_dir =  os.getcwd()

if __name__ == "__main__":
     # parse the command line arguments
     (options, args) = parse_args()
     main()
else:
     print "Not main"

