#******************************************************************************
#* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg/mcfg_gen/scripts/mcfg_generate.pl#1 $
#* $DateTime: 2016/12/13 07:58:21 $
#*
#* 
#******************************************************************************
#*
#* Copyright (c) 2015 Qualcomm Technologies, Inc.
#* All rights reserved.
#* Qualcomm Technologies, Inc. Confidential and Proprietary.
#*
#******************************************************************************
# This script creates an MCFG (modem config) .c file directly from an excel workbook 
# It takes an input file as a parameter and an output file name prefix. For each MCFG spreadsheet

use FindBin;
use lib "$FindBin::Bin/";
require "mcfg_write.pm";

use Benchmark;
use Cwd;
use File::Spec;

sub ReadNextFlag($) {
   my ($args) = @_;

   my $flag = shift(@$args);

   if($flag eq "-xml" or $flag eq "-i" or $flag eq "-o" or $flag eq "-b" or $flag eq "-mbn" or $flag eq "-crm") {
      return $flag;
   }
   else {
      print STDOUT "Unknown cmd option: $flag\n";
      return "";
   }
}

# parse command line arguments
# 
# Inputs: a list of command line parameters (ARGV)
# Returns: On Sucess: A list in the form (INPUT FILE, OUTPUT PREFIX, BUILD_ROOT) -- indicates correct
#                     number of command line parameters
#          On Failure: an emty list -- indicates incorrect formatting of command line parameters
sub ParseArguments {
   my @arguments = @_;
   my $input_file = "";
   my $output_prefix = "";
   my $build_root = "";
   my $xml = 0;
   my $mbn = 0;
   my $crm_buildid = "";

   my $next_flag = ReadNextFlag(\@arguments);

   while($next_flag) {
      my $next_argument = shift(@arguments);

      if($next_flag eq "-i") {
         $input_file = $next_argument;
      }
      elsif($next_flag eq "-o") {
         $output_prefix = $next_argument;
      }
      elsif($next_flag eq "-b") {
         $build_root = $next_argument;
      }
  	  elsif($next_flag eq "-xml") {
        $xml = 1;
     	  unshift(@arguments, $next_argument);
  	  }
    	elsif($next_flag eq "-mbn") {
    	   $mbn = 1;
    	   unshift(@arguments, $next_argument);
  	  }
      elsif($next_flag eq "-crm") {
    	    $crm_buildid = $next_argument;
  	   }
      else {
         return (); #invalid flag
      }

      $next_flag = ReadNextFlag(\@arguments);
   }

   if(!$input_file) {
      return ();
   }

   return ($input_file, $output_prefix, $build_root, $xml, $mbn, $crm_buildid);
}

sub LogError($) {
   my ($error) = @_;

   open(LOGFILE, "> debug.log");

   print LOGFILE "Script failed due to the following error:\n\t$error";

   close(LOGFILE);
}

sub Execute {
   # Parse arguments to get input file and output file names
   my @args = ParseArguments(@ARGV);
   @args or return "Usage is: perl mcfg_generate.pl -i <INPUT FILE> [-o <OUTPUT PREFIX>] [-b <BUILD ROOT>] [-xml] [-mbn] [-crm <crm id>]";

   my ($input_file_name, $output_file_prefix, $build_root, $xmlFormat, $mbn, $crm_buildid) = @args;
   print "Using build root: \"$build_root\"\n\n" if ($build_root);

   $input_file_name = File::Spec->rel2abs($input_file_name);

   # if using xml input format then process as XML instead
   if($xmlFormat) {
     open(FILE, "<", $input_file_name) or die "Unable to open xml file, \"$input_file_name\"";
     local $/ = undef;
     my $xml = <FILE>;
     close(FILE);
	   
     if ($mbn)
     {
       my ($length_bytes, $num_items) = MCFGWrite::GenerateMCFGTargetFrmSourceXML($xml, $input_file_name, $output_file_prefix, $build_root, $crm_buildid);
       print "\tFound $num_items items totaling $length_bytes (" . sprintf("0x%04X", $length_bytes) . ") bytes\n"
     }
     else
     {
        my $input_file_name = 'TEST_FILE.xml';
        MCFGWrite::GenerateMcfgSegmentFileXML($xml, $input_file_name, $output_file_prefix, $build_root, $MCFGWrite::GENERIC_C_OUTPUT_FILE_TYPE, 4, undef, $crm_buildid);
        #my ($length_bytes, $num_items) = MCFGWrite::GenerateMcfgSourceFileXML($xml, $output_file_prefix, $build_root);
        # GenerateMcfgSourceFileXML deprecated when duplicated code was merged to GenerateMcfgSegmentFileXML
        print "\tFound $num_items items totaling $length_bytes bytes\n";
     }

        return;
   }

   # Keep mcfg_read.pm below XML specific processing. 
   # If this require is moved to top of file compile will fail on linux server, since the Win32::OLE module that mcfg_read.pm requires is not present
   require "mcfg_read.pm";
   
   my $excelWorkbook = MCFGRead::OpenExcelWorkbook($input_file_name);
   $excelWorkbook or return "Unable to open workbook, \"$input_file_name\"";

   $SIG{__DIE__} = sub { 
      my ($error) = @_;
      LogError($error);

      # the workbook must be closed or else an instance of excel stays open
      $excelWorkbook->{Saved} = 1; # this keeps the dialog box asking if you would like to save the changes from popping up
      $excelWorkbook->Close();
   };

   my $mcfg_sheet_data = MCFGRead::ExtractMcfgSheetDataFromWorkbook($excelWorkbook);

   # Process every modem config sheet and generate a config source file for it
   for my $sheet_name (keys %{$mcfg_sheet_data}) {
     print "Generating MCFG for $sheet_name at ", $output_file_prefix . $sheet_name . ".c\n";
     my $rows = MCFGRead::ReadRowsFromWorksheet($sheet_name, $excelWorkbook);
     my ($length_bytes, $num_items) = MCFGWrite::GenerateMcfgSourceFile($rows, $mcfg_sheet_data->{$sheet_name}, $input_file_name, $output_file_prefix . $sheet_name . ".c", $build_root);

     print "\tFound $num_items items totaling $length_bytes bytes\n";
   }

   # the workbook must be closed or else an instance of excel stays open
   $excelWorkbook->{Saved} = 1; # this keeps the dialog box asking if you would like to save the changes from popping up
   $excelWorkbook->Close();

   return "";
}

print Execute(), "\n";
