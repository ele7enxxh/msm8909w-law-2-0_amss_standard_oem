#******************************************************************************
#* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg/mcfg_gen/scripts/mcfg_read.pm#1 $
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
package MCFGRead;

use OLE;
use Win32::OLE::Const "Microsoft Excel";
use Benchmark;
use Data::Dumper;

# these are some helper functions to do simple conversions
sub ColumnIndexToLetter($) {
   my ($column_index) = @_;

   return chr(65 + $column_index-1);
}

# This function added to handle Excel columns between AA - ZZ
sub ColumnIndexToLetters($) {
   my ($column_index) = @_;
   my $column_str = '';
   my $max_index = 702; # 702 = maps to column ZZ in Excel
   my $alphabet_size = 26; # of letters in the alphabet
   my $alpha_repeat, $alpha_offset;

   if ($column_index > $max_index) {
      die ("Error: Column index larger than max allowed (max = $max_index): $column_index\n");
   }
   
   {
      # use integer forces integer division within scope
      use integer;
      # Find how many times the alphabet has been cycled through to reach column_index
      # This will determine the most significant letter, i.e. equivalent of the "10s" place

      $alpha_repeat = (($column_index - 1)/$alphabet_size);
      if ($alpha_repeat > 0) 
      {
         $column_str .= ColumnIndexToLetter($alpha_repeat);
      }
   }

   # This will determine the least significant letter, i.e. equivalent of the "1s" place
   $alpha_offset =  $column_index % $alphabet_size;
   if ($alpha_offset == 0) {
      # Z is the only letter in which there is no remainder from mod 26 in English alphabet
      $column_str .= 'Z';
   }
   else 
   {
      $column_str .= ColumnIndexToLetter($alpha_offset);
   }

   return $column_str;
}

sub IntColRowToExcelColRow($$) {
   my ($col, $row) = @_;

   return ColumnIndexToLetters($col) . $row;
}

sub GetSheetBounds($) {
   my ($sheet) = @_;
   my $max_index = 702;

   my $last_row = $sheet->UsedRange->Rows->Count + 1;  # UsedRange counts appear to start from 0
   my $last_col = $sheet->UsedRange->Columns->Count + 1;
   
   if ($last_col > $max_index)
   {
     # Spreadsheet formatting may cause Columns->Count to be unreasonably large
     $last_col = $max_index
   }

   #print "last row, last col:  $last_row, $last_col\n";
   return ($last_row, $last_col);
}



# Reads the rows from a worksheet and returns as an array of arrays
#
# Inputs: Excel worksheet reference obtained from OLE API calls below
# Returns: On Success: An array of references to arrays containing the fields of each row in order
#          On Failure: An empty array
sub ReadRowsFromWorksheet($$) {
   my ($sheet_name, $workbook) = @_; 

   my $sheet = OpenExcelWorksheet($workbook, $sheet_name);

   my ($last_row, $last_col) = GetSheetBounds($sheet);

   # this creates a reference to an array of array references each of which is a row in the table
   my $rows = $sheet->Range(IntColRowToExcelColRow(1, 1) . ':' . IntColRowToExcelColRow($last_col, $last_row))->{Value};
   #print Dumper($rows);

   return $rows;
}

sub ExtractMcfgSheetDataFromWorkbook($) {
   my ($workbook) = @_;
   my $summary_sheet = OpenExcelWorksheet($workbook, "Summary");
   my ($last_row, $last_col) = GetSheetBounds($summary_sheet);

   my $summary_start_row = -1;
   my $last_carrier_col = -1;
   my $mcfg_sheet_data = {};

   # determine summary table start
   for(1 .. $last_row) {
      my $leftmost_cell_text = $summary_sheet->Range("A" . $_)->{Value};

      if($leftmost_cell_text =~ /Summary/) {
         $summary_start_row = $_+1; # the start of the table should be the line directly after this heading
         last;
      }
   }

   # read in carrier namees
   for(2 .. $last_col) { # the first cell will be left blank
      my $col_hdr = ColumnIndexToLetters($_);
      my $carrier_name = $summary_sheet->Range($col_hdr . $summary_start_row)->{Value};

      # if we reach a blank column then that's the last column
      length($carrier_name) > 0 or next;

      # prepare the hash that will be used next
      $mcfg_sheet_data->{$carrier_name} = { ColumnIndex => $_ };
   }

   $summary_start_row++; # go to the actual data

   for my $row ($summary_start_row .. $last_row) {
      my $current_field = $summary_sheet->Range("A" . $row)->{Value};

      # if the field name is blank then we've reached the end of the table
      length($current_field) > 0 or next;

      for my $carrier_name (keys %{$mcfg_sheet_data}) {
         my $column_index = $mcfg_sheet_data->{$carrier_name}->{ColumnIndex};
         my $col_hdr = ColumnIndexToLetters($column_index);
         $mcfg_sheet_data->{$carrier_name}->{$current_field} = $summary_sheet->Range($col_hdr . $row)->{Value};
      }
   }

   return $mcfg_sheet_data;
}

# Opens an excel workbook given the excel file name and gives a handle to it
# 
# Inputs: The excel file name
# Returns: A handle to the workbook
sub OpenExcelWorkbook($) {
   my ($input_file) = @_;
   my $excel = CreateObject OLE "Excel.Application";
   my $workbook = $excel->Workbooks->Open($input_file);

   return $workbook;
}

# Opens a worksheet in a particular workbook given the worksheets names and gives a handle to it
#
# Inputs: The workbook which contains the sheet followed by the name of the worksheet to open
# Returns: A handle to the specified worksheet
sub OpenExcelWorksheet($$) {
   my ($workbook, $sheet_name) = @_;

   my $sheet = $workbook->Worksheets($sheet_name);

   die("Sheet '$sheet_name' not present in workbook. Verify listing in Summary tab is correct.\n") if (!defined($sheet));
   $sheet->Activate();

   return $sheet;
}

1;
