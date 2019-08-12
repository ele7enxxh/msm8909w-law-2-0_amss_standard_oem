################################################################################
# $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/almanac_provisioning_scripts/generate_almanac.pl#1 $
# $Revision: #1 $
# $DateTime: 2016/12/13 07:59:45 $
#
# Fetches the current weekly GPS almanac file from
# http://www.navcen.uscg.gov/gps/current/current.alm in yuma format.  Renames
# the file to webalm.txt and passes the test file to Almgen_8_Byte_Alignment to
# generate the MSM-formatted almanac files.  alm_download.pl should be called
# after this script to download the MSM-formatted almanac files to the device.
#
#  Copyright (c) 2007 - 2012 QUALCOMM Incorporated.
#  Qualcomm Confidential and Proprietary. All Rights Reserved.
#  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
#  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
#
# usage: perl generate_almanac.pl [<alignment>]
#
#   alignment -  4 to generate 4-byte aligned almanac files
#                8 to generate 8-byte aligned almanac files (default)
#
# Revision History
# when       who    what, where, why
# --------   ---    ----------------------------------------------------------
# 09/10/08   jlp    Delete old AlmFiles before the new ones are generated.
# 06/06/08   jlp    Added command line parameter for determining 4 or 8 byte
#                   alignment.  This will default to 8 byte alignment if the
#                   parameter is not passed in.
# 05/19/08   jlp    Updated to get only the current almanac from the new
#                   location.  Defaulted to almgen_8_byte_alignment.
#     
################################################################################


print ("Version: generate_almanac.pl#2\n\n");


use LWP::Simple;
use File::Copy;
use strict;

################################################################################
# Check for user input
################################################################################

# Whether the almanac files should be 4-byte aligned or 8-byte aligned.
my $alignment = $ARGV[0];

if(not defined ($alignment))
{
  $alignment = 8;
}
elsif(($alignment != 8) && ($alignment != 4))
{
  die "Invalid alignment parameter $alignment";
}

# delete the old almanac files
my @old_alm_files = glob("AlmFile*");
my $old_alm_file_count = @old_alm_files;

if($old_alm_file_count > 32)
{
  die "Too many almfiles to delete ($old_alm_file_count), exiting!\n";
}
elsif($old_alm_file_count)
{
  my $deleted_file_count = unlink @old_alm_files;
  if(unlink $deleted_file_count != $old_alm_file_count)
  {
    die "Deleted $deleted_file_count out of $old_alm_file_count files.  Exiting!\n";
  }
}

# url of website with almanac data
my $URL = "http://www.navcen.uscg.gov/";
my $currentAlmanac = "?pageName=currentAlmanac&format=yuma";

# download latest almanc file
my $content;
unless (defined ($content = get "$URL$currentAlmanac"))
{
   die "could not get $URL$currentAlmanac\n";
}


# save to disk as "webalm.txt" since this is the file expected by the ALMGEN
# program
my $savefile = "webalm.txt";
open SAVEFILE, ">$savefile" or die "failed to write to savefile $savefile\n";
print SAVEFILE "$content";
close SAVEFILE;

# check for bad file download
print "\nValidating download $savefile\n";
open SAVEFILE, "$savefile" or die "Error opening $savefile for output.\n";
my $savefile_opened = 1;
while (<SAVEFILE>)
{
  CheckForErroneousFile($_);
 
}
if ($savefile_opened == 1)
{
  print "\t$savefile file check passed\n";
  close SAVEFILE;
}

my $almfile = $savefile;

print "\nGenerating MSM-formatted almanac files from $savefile\n";
if($alignment == 4)
{
  system( 'almgen_4_byte_alignment' ) == 0 or die "ALMGEN failed to generate almanac files\n";
}
else
{
  system( 'almgen_8_byte_alignment' ) == 0 or die "ALMGEN failed to generate almanac files\n";
}


################################   FUNCTIONS   ################################
sub CheckForErroneousFile
{
  if (/(<[^>]+>)/)
  {
    print("\t#### Abort processing. Found erroneous input:\n\t$1\n");
    if ($savefile_opened == 1)
    {
      close SAVEFILE;
      print("\t### Deleting bad file $savefile\n");
      print("\t### Please does manual download from $URL$currentAlmanac\n");
      system("del /q $savefile");
      exit;
    }
  }
} # CheckForErroneousFile
