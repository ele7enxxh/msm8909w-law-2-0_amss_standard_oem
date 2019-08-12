#******************************************************************************
#* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg/mcfg_gen/scripts/mcfg_xls_translate.pl#1 $
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
use strict;
use Getopt::Long qw(GetOptions);

use lib "../scripts";
use Data::Dumper;
require "mcfg_read.pm";
require "mcfg_write.pm";

my $FALSE = 0;
my $TRUE = 1;

my $XML_DECLARATION = '<?xml version="1.0" encoding="utf-8"?>';
my $ROOT_TAG = "NvData";
my $CARRIER_DATA = "NvConfigurationData";
my $NV_ITEM_TAG = "NvItemData";
my $EFS_DIR_TAG = "NvEfsDir";
my $EFS_FILE_TAG = "NvEfsFile";
my $GROUP_FILE_TAG = "GroupFile";
my $EFS_ITEM_TAG = "NvEfsItemData";
my $PRL_FILE_TAG = "NvPrlFile";
my $TRL_RECORD_TAG = "NvTrlRecord";
my $MEMBER_TAG = "Member";

my $verbose = $FALSE;

my $item_translator_map = {
   "scalar" => \&TranslateNvItem,
   "efs" => \&TranslateEfsFileItem,
   "group" => \&TranslateGroupFileItem,
   "efs_dir" => \&TranslateEfsDirItem,
   "efs_item" => \&TranslateEfsEnumeratedItem,
   "prl" => \&TranslatePrlItem,
   "trl" => \&TranslateTrailerRecord
};

sub PrintUsage() {
   print "Usage:\n";
   print "required:\n";
   print "\tworkbook - path to excel workbook file\n";
   print "\tspreadsheet - name of spreadsheet to translate (i.e. Verizon)\n";
   print "optional:\n";
   print "\toutput - file where XML will be written. If not specified, output is written to stdout\n";
   print "\tverbose - if flag is set then extra debugging messages are printed\n";
   print "\n";
   print "Examples:\n";
   print "\tWithout debugging: mcfg_xls_translate --workbook=C:\\Users\\me\\input.xls --spreadsheet=Verizon --output=C:\\Users\\me\\Desktop\\output.xml\n";
   print "\tWith debugging:    mcfg_xls_translate --workbook=C:\\Users\\me\\input.xls --spreadsheet=Verizon --output=C:\\Users\\me\\Desktop\\output.xml --verbose\n";
   print "\n";
}

# Format a path to adhere to the rules of Unix paths
sub CanonicalizePathUnix {
   my $path = shift;

   #Convert windows style directory delimiters to Unix style
   $path =~ s#\\#/#g; 

   return $path;
}

sub Main($) {
   my ($args) = @_;
   my $excelWorkbook = MCFGRead::OpenExcelWorkbook($args->{WorkbookFile});
   my $mcfgSheetData = MCFGRead::ExtractMcfgSheetDataFromWorkbook($excelWorkbook);
   my $spreadsheets = [keys(%$mcfgSheetData)];

   # process one spreadsheet only
   if(not $args->{All}) {
      $spreadsheets = [$args->{SpreadsheetName}];
   }

   my $buildRoot = $ENV{'BUILD_ROOT'} or die "No BUILD_ROOT variable set";

   # process each spreadsheet and generate a corresponding xml file
   for my $spreadsheet (@$spreadsheets) {
      my $rows = MCFGRead::ReadRowsFromWorksheet($spreadsheet, $excelWorkbook);
      my $mcfgData = $mcfgSheetData->{$spreadsheet};
      my $outputFile = *STDOUT;
      if($args->{OutputFile}) {
         $outputFile = OpenOutputFile($args->{OutputFile} . "_" . $spreadsheet . ".xml");
      }

      GenerateMcfgXML($rows, $mcfgData, $outputFile, $buildRoot);
   }

   # this keeps the dialog box asking if you would like to save the changes from popping up
   $excelWorkbook->{Saved} = 1;
   # closes Excel instance
   $excelWorkbook->Close();
}

sub OpenOutputFile($) {
   my ($outputFileLocation) = @_;
   my $outputFileHandle;

   if($outputFileLocation) {
      open($outputFileHandle, ">", $outputFileLocation) or die "cannot open $outputFileLocation for writing";
   }
   else {
      $outputFileHandle = *STDOUT;
   }

   return $outputFileHandle;
}

# Creates a hash from command line arguments
#
# Input: array of command line arguments(i.e. @ARGV)
# Return: A hash reference containing the following values:
#    WorkbookFile - a string which is the path to the xls file with mcfg information
#    SpreadsheetName - a string with the name of the spreadsheet to translate to XML (i.e. Verizon)
#    OutputFile - a string which is the path to the xml file to create
sub ParseArguments {
   my ($workbookFile, $spreadsheetName, $outputFile, $all) = "";

   @ARGV = @_;

   GetOptions('workbook=s' => \$workbookFile,
               'spreadsheet=s' => \$spreadsheetName,
             'output=s' => \$outputFile,
              'all' => \$all,
             'verbose' => \$verbose);

   return {
      WorkbookFile => $workbookFile,
      SpreadsheetName => $spreadsheetName,
      All => $all,
      OutputFile => $outputFile
   };
}

# Checks arguments to make sure they make sense and things aren't
# missing
#
# Input: A hash reference in the format produced by ParseArguments
# Return: $TRUE if arguments are valid, $FALSE otherwise
sub ArgumentsAreValid($) {
   my ($args) = @_;

   if(not $args->{WorkbookFile}) {
      return $FALSE;
   }
   if(not $args->{All}) {
      if(not $args->{SpreadsheetName}) {
         return $FALSE;
      }
   }

   return $TRUE;
}

# Takes a group mcfg_item and compares the expected group version to 
# the actual group version listed in the group xml
sub GroupVersionMismatch($$) {
   my ($mcfg_item, $buildRoot) = @_;
   my $mismatch = 0;
   if ($mcfg_item->{ItemType} eq 'group') {
      my $expected_group_version = MCFGWrite::Trim($mcfg_item->{Version});
      my $group_xml_file = $mcfg_item->{Filename}; 

      # so path is at modem_proc\..
      # otherwise cwd would be at modem_proc\mcfg\build
      chdir $buildRoot;

      # read xml group file
      open my $fh, '<', $group_xml_file or die "Can't open file '$group_xml_file'!";
      my $xml_group_content = do { local $/; <$fh> };
      #print Dumper($xml_group_content);

      # parse xml
      my $parser = new XML::Parser(Style => 'Tree');
      my $xmlParseTree = $parser->parse($xml_group_content);
      $xmlParseTree = MCFGWrite::XmlStripBlanks($xmlParseTree);
      my $top = shift(@{$xmlParseTree->[1]}); # remove top level attributes hash
      $xmlParseTree = $xmlParseTree->[1];
      my $group_sheet_data = $xmlParseTree->[1]->[0];
      my $actual_group_version = MCFGWrite::Trim( $$group_sheet_data{'version'} );

      if (!(MCFGWrite::strmatch( $expected_group_version, $actual_group_version ))) {
         print "\nGroup version mismatch for '$group_xml_file'\n";
         print "Expected group version: '$expected_group_version'\n";
         print "Actual group version: '$actual_group_version'\n";
         $mismatch = 1;
      }
   }
   return $mismatch;
}


# Takes spreadsheet data, translates it to XML and writes
# the XML to the specified output file
#
# Input: 
#    $rows - rows from spreadsheet as read in by MCFGRead::ReadRowsFromWorksheet
#    $mcfgSheetData - hash with Carrier Index, Full MCFG Version and Configuration Type properties 
#    $outputFile - a file handle to the file where XML is to be written
# Return:
#    None
sub GenerateMcfgXML {
   my ($rows, $mcfgSheetData, $outputFile, $xml_content, $buildRoot) = @_;
   my $mcfg_item = {};

   MCFGWrite::SetBuildRoot($buildRoot);

   my @xml_lines = ();

   push(@xml_lines, ($XML_DECLARATION . "\n"));
   #print $outputFile ($XML_DECLARATION . "\n");

   # write opening tag
   push(@xml_lines, (CreateXMLOpenTag($ROOT_TAG, []) . "\n"));
   #print $outputFile (CreateXMLOpenTag($ROOT_TAG, []) . "\n");

   my $version = $mcfgSheetData->{"Full MCFG Version"};
   if (!$version) {
      # For Group spreadsheets
      $version = $mcfgSheetData->{"Group Version"}
   }

   my $attributes = [
      {Name => "carrierIndex", Value => $mcfgSheetData->{"Carrier Index"}},
      {Name => "version", Value => $version},
      {Name => "type", Value => $mcfgSheetData->{"Configuration Type"}}
   ];

   if (!defined($mcfgSheetData->{"Configuration Type"}) || ($mcfgSheetData->{"Configuration Type"} eq ''))
   {
     die("Error: Configuration type value in source file is blank. Pls set type value to a valid configuration type (e.g. SW = 1 or HW = 0).\nIf generating from an Excel workbook this is done in the summary sheet\n");
   }

   # write carrier data
   push(@xml_lines, ("\t" . CreateXMLStandalone($CARRIER_DATA, $attributes) . "\n"));
   #print $outputFile ("\t" . CreateXMLStandalone($CARRIER_DATA, $attributes) . "\n");

   $mcfg_item = MCFGWrite::ExtractNextMcfgItemFull($rows);
   # write each NvItem
   
   my $version_mismatch = 0;
   while(%$mcfg_item) {
      #print Dumper ($mcfg_item);
      my $xml = TranslateMcfgItemToXML($mcfg_item);
      if (GroupVersionMismatch($mcfg_item, $buildRoot)) {
         $version_mismatch = 1;
         print $outputFile $xml_content;
         close($outputFile);
         die()
      }
      push(@xml_lines, $xml);
   
      $mcfg_item = MCFGWrite::ExtractNextMcfgItemFull($rows);
   }

   # write closing tag
   push(@xml_lines, (CreateXMLCloseTag($ROOT_TAG)));
   #print $outputFile (CreateXMLCloseTag($ROOT_TAG));

   foreach my $xml (@xml_lines) {
      print $outputFile $xml;
   }

}

sub attributesToString($) {
   my ($tagAttributes) = @_;
   my $tag = "";

   for my $attribute (@$tagAttributes) {
      my $attributeName = $attribute->{Name};
      my $attributeValue = $attribute->{Value};

      $tag .= " " . $attributeName . "=\"" . $attributeValue . "\"";
   }

   return $tag;
}

sub CreateXMLOpenTag($$) {
   my ($tagName, $tagAttributes) = @_;

   return "<$tagName" . attributesToString($tagAttributes) . ">";
}

sub CreateXMLCloseTag($) {
   my ($tagName) = @_;

   return "</$tagName>";
}

sub CreateXMLStandalone($$) {
   my ($tagName, $tagAttributes) = @_;

   return "<$tagName" . attributesToString($tagAttributes) . "/>";
}

# invokes the handler function corresponding to the item type (efs, efs_dir, trl, etc.)
# the handler function returns an xml string generated from that item
# Input: 
#    $item - reference to item as returned by ExtractNextMcfgItemFull in mcfg_write.pm
# Return:
#    an XML string
sub TranslateMcfgItemToXML($) {
   my ($item)   = @_;
   my $translateCallback = $item_translator_map->{$item->{ItemType}};
   return $translateCallback->($item);
}

# handler function for normal NV items
sub TranslateNvItem($) {
   my ($item) = @_;
   my $id = $item->{ID} ? $item->{ID} : $item->{Filepath};
   my $mcfgAttributes = $item->{Attributes};
   my $mcfgVariant = $item->{Variant};
   my $members = $item->{Elements};
   my $xml = "";

   LogDebug("Translating NV Item to XML: ", $verbose);
   LogDebug("\tID: $id", $verbose);
   LogDebug("\tAttributes: " . sprintf("0x%02X", $mcfgAttributes), $verbose);
   LogDebug("\tMembers: " . scalar(@$members), $verbose);

   $xml .= "\t" . CreateXMLOpenTag($NV_ITEM_TAG, [
            {Name => "id", Value => "$id"},
            {Name => "mcfgAttributes", Value => sprintf("0x%02X", $mcfgAttributes) },
            {Name => "mcfgVariant", Value => $mcfgVariant},
           ]) . "\n";
   $xml .= TranslateMembersToXML($members);
   $xml .= "\t" . CreateXMLCloseTag($NV_ITEM_TAG) . "\n";
   
   return $xml;
}

# handler function for efs file items
sub TranslateEfsFileItem($) {
   my ($item) = @_;
   my $mcfgAttributes = $item->{Attributes};
   my $mcfgVariant = $item->{Variant};
   my $targetPath = $item->{Filepath};
   my $buildPath = CanonicalizePathUnix( $item->{Filename} );
   my $members = $item->{Elements}->[0]; 
   my $xml = "";

   LogDebug("Translating EFS File Item to XML: ", $verbose);
   LogDebug("\tTarget Path: $targetPath", $verbose);
   LogDebug("\tBuild Path: $buildPath", $verbose);
   LogDebug("\tAttributes: " . sprintf("0x%02X", $mcfgAttributes), $verbose);

   if (! MCFGWrite::IsMetaData($members))
   {
     $xml .= "\t" . CreateXMLStandalone($EFS_FILE_TAG, [
              {Name => "mcfgAttributes", Value => sprintf("0x%02X", $mcfgAttributes)},
              {Name => "mcfgVariant", Value => $mcfgVariant},
              {Name => "targetPath", Value => $targetPath},
              {Name => "buildPath", Value => $buildPath},
             ]) . "\n";
   }
   else
   {
     $xml .= "\t" . CreateXMLOpenTag($EFS_FILE_TAG, [
              {Name => "mcfgAttributes", Value => sprintf("0x%02X", $mcfgAttributes)},
              {Name => "mcfgVariant", Value => $mcfgVariant},
              {Name => "targetPath", Value => $targetPath},
              {Name => "buildPath", Value => $buildPath},
             ]) . "\n";
     # anon arr ref used to satisfy TranslateMembersToXML
     $xml .= TranslateMembersToXML([ $members ]);
     $xml .= "\t" . CreateXMLCloseTag($EFS_FILE_TAG) . "\n";
   }

   return $xml;
}

# handler function for group file items
sub TranslateGroupFileItem($) {
   my ($item) = @_;
   my $mcfgAttributes = $item->{Attributes};
   #my $mcfgVariant = $item->{Variant};
   #my $targetPath = $item->{Filepath};
   #my $members = $item->{Elements}->[0]; 
   my $version = $item->{Version};
   my $buildPath = CanonicalizePathUnix( $item->{Filename} );

   my $xml = "";

   LogDebug("Translating Group File Item to XML: ", $verbose);
   LogDebug("\tBuild Path: $buildPath", $verbose);
   LogDebug("\tAttributes: " . sprintf("0x%02X", $mcfgAttributes), $verbose);

  $xml .= "\t" . CreateXMLStandalone($GROUP_FILE_TAG, [
           {Name => "mcfgAttributes", Value => sprintf("0x%02X", $mcfgAttributes)},
           #{Name => "mcfgVariant", Value => $mcfgVariant},
           {Name => "version", Value => $version},
           {Name => "buildPath", Value => $buildPath},
          ]) . "\n";


   return $xml;
}


# handler function for efs dir items
sub TranslateEfsDirItem($) {
   my ($item) = @_;
   my $mcfgAttributes = $item->{Attributes};
   my $mcfgVariant = $item->{Variant};
   my $targetPath = $item->{Filepath};
   my $buildPath = CanonicalizePathUnix( $item->{Filename} );
   my $members = $item->{Elements};
   my $xml = "";

   LogDebug("Translating EFS File Item to XML: ", $verbose);
   LogDebug("\tTarget Path: $targetPath", $verbose);
   LogDebug("\tBuild Path: $buildPath", $verbose);
   LogDebug("\tAttributes: " . sprintf("0x%02X", $mcfgAttributes), $verbose);

   $xml .= "\t" . CreateXMLStandalone($EFS_DIR_TAG, [
            {Name => "mcfgAttributes", Value => sprintf("0x%02X", $mcfgAttributes)},
            {Name => "mcfgVariant", Value => $mcfgVariant},
            {Name => "targetPath", Value => $targetPath},
            {Name => "buildPath", Value => $buildPath}
           ]) . "\n";
   $xml .= TranslateMembersToXML($members);

   return $xml;
}

# handler function for normal efs items
sub TranslateEfsEnumeratedItem($) {
   my ($item) = @_;
   my $filepath = $item->{Filepath};
   my $mcfgAttributes = $item->{Attributes};
   my $mcfgVariant = $item->{Variant};
   my $members = $item->{Elements};
   my $xml = "";

   LogDebug("Translating EFS File Item to XML: ", $verbose);
   LogDebug("\tTarget Path: $filepath", $verbose);
   LogDebug("\tAttributes: " . sprintf("0x%02X", $mcfgAttributes), $verbose);
   LogDebug("\tMembers: " . scalar(@$members), $verbose);

   $xml .= "\t" . CreateXMLOpenTag($EFS_ITEM_TAG, [
            {Name => "mcfgAttributes", Value => sprintf("0x%02X", $mcfgAttributes)},
            {Name => "mcfgVariant", Value => $mcfgVariant},
            {Name => "fullpathname", Value => $filepath},
           ]) . "\n";
   $xml .= TranslateMembersToXML($members);
   $xml .= "\t" . CreateXMLCloseTag($EFS_ITEM_TAG) . "\n";
}

# handler function for prl items
sub TranslatePrlItem($) {
   my ($item) = @_;
   my $mcfgAttributes = $item->{Attributes};
   my $buildPath = CanonicalizePathUnix( $item->{Elements}->[0]->{Filename} );
   my $version = $item->{Elements}->[0]->{Version};
   my $xml = "";

   LogDebug("Translating Prl File Item to XML: ", $verbose);
   LogDebug("\tBuild Path: $buildPath", $verbose);
   LogDebug("\tAttributes: " . sprintf("0x%02X", $mcfgAttributes), $verbose);

   $xml .= "\t" . CreateXMLStandalone($PRL_FILE_TAG, [
            {Name => "mcfgAttributes", Value => sprintf("0x%02X", $mcfgAttributes)},
            {Name => "buildPath", Value => $buildPath},
            {Name => "prlVersion", Value => $version}
           ]) . "\n";

   return $xml;
}

# handler function for trailer record items
sub TranslateTrailerRecord($) {
   my ($item) = @_;
   my $mcfgAttributes = $item->{Attributes};
   my $members = $item->{Elements};
   my $xml = "";

   LogDebug("Translating Trailer Record to XML: ", $verbose);
   LogDebug("\tAttributes: " . sprintf("0x%02X", $mcfgAttributes), $verbose);
   LogDebug("\tMembers: " . scalar(@$members), $verbose);

   $xml .= "\t" . CreateXMLOpenTag($TRL_RECORD_TAG, [
            {Name => "mcfgAttributes", Value => sprintf("0x%02X", $mcfgAttributes)}
           ]) . "\n";
   $xml .= TranslateMembersToXML($members);
   $xml .= "\t" . CreateXMLCloseTag($TRL_RECORD_TAG) . "\n";

   return $xml;
}

# create an XML string from array of members for an item
# Inputs:
#    reference to an array of members (i.e. $item->{Elements})
# Return:
#    an XML string
sub TranslateMembersToXML($) {
   my ($members) = @_;
   my $xml = "";

   for my $member (@$members) {
     # ensures file specific data for efs type is not stored as a member
     if (MCFGWrite::IsMetaData($member))
     {
       $xml .= "\t\t" . TranslateMemberToXML($member) . "\n";
     }
   }

   return $xml;
}

# create an XML string from a single member
# Inputs:
#    reference to a member (i.e. $item->{Elements}->[0])
# Return:
#    an XML string
sub TranslateMemberToXML($) {
   my ($member) = @_;
   my $memberValue = $member->{Value};
   my ($size, $type, $value) = @{ToXMLSizeTypeValue($member->{Size}, $member->{Type}, $memberValue)};
   my $attributes = [
      {
         Name => "sizeOf", # size
         Value => "$size"
      },
      {
         Name => "type", # type
         Value => "$type"
      }
   ];

   LogDebug("\tTranslating Member to XML: ", $verbose);
   LogDebug("\t\tSize: $size", $verbose);
   LogDebug("\t\tType: $type", $verbose);
   LogDebug("\t\tValue: $value", $verbose);
   
   return CreateXMLOpenTag($MEMBER_TAG, $attributes) . $value . CreateXMLCloseTag($MEMBER_TAG);
}

sub valueIsPositive($$) {
   my ($value, $size) = @_;
   my $signBitPosition = $size*8 - 1;
   my $signBit = ($value >> $signBitPosition) & 0x1;

   if($signBit) {
      return $FALSE;
   }
   else {
      return $TRUE;
   }
}

sub ToXMLSizeTypeValue($$$) {
   my ($xlsSize, $xlsType, $xlsValue) = @_;
   my ($xmlSize, $xmlType) = "";
   my $valueArray = [];
   
   # check for indefinite size types (need to convert to fixed size to
   #  simplify processing)    
   if($xlsType =~ /int\[\]/) {
      my $arraySize = scalar(@$xlsValue);

      $xlsType = "int[$arraySize]";
   }
   elsif($xlsType =~ /string/ && not($xlsSize)) {
      my $stringLength = length($xlsValue);

      $xlsSize = $stringLength;
   }

   #               [XLS]                            [XML]
   #
   # [XLS TYPE] [XLS SIZE] [XLS VALUE(S)]  |  [XML TYPE] [XML SIZE]
   # int        1          negative        |  int8       1
   # int        1          positive        |  uint8      1
   # int        2          negative        |  int16      1
   # int        2          positive        |  uint16     1
   # int        4          negative        |  int32      1
   # int        4          positive        |  uint32     1
   # int        8          negative        |  int64      1
   # int        8          positive        |  uint64     1
   # int[n]     1          negative        |  int8       n
   # int[n]     1          positive        |  uint8      n
   # int[n]     2          negative        |  int16      n
   # int[n]     2          positive        |  uint16     n
   # int[n]     4          negative        |  int32      n
   # int[n]     4          positive        |  uint32     n
   # int[n]     8          negative        |  int64      n
   # int[n]     8          positive        |  uint64     n
   # string     n          *               |  uint8      n

   if($xlsType =~ /^int/) {
      my $isPositive;

      if($xlsType =~ /^int$/) {
         # larger int values (> 4B) are stored in array form
         if(ref($xlsValue) eq 'ARRAY') {
            shift(@$xlsValue);
            for my $i (0 .. (scalar(@$xlsValue)-1)) {
               $xlsValue->[$i] = hex($xlsValue->[$i]);
            }

            return ToXMLSizeTypeValue(1, "int[$xlsSize]", $xlsValue);
         }
         else {
            $isPositive = valueIsPositive($xlsValue, $xlsSize);
            $xmlSize = 1;
            $valueArray = [$xlsValue];
         }
      }
      elsif($xlsType =~ /^int\[\d+\]$/) {
         if(scalar(@$xlsValue) == 0) {
            $isPositive = $TRUE;
         }
         elsif(scalar(@$xlsValue) == 1) {
            $isPositive = $xlsValue->[0] >= 0;
         }
         else {
            $isPositive = $TRUE;

            for my $part (@$xlsValue) {
               if(not valueIsPositive($part, $xlsSize)) {
                  $isPositive = $FALSE;
               }
            }
         }

         $xmlSize = int(substr($xlsType, length("int["), -1));
         $valueArray = $xlsValue;
      }

      $xmlType = ($isPositive?"u":"") . "int" . ($xlsSize * 8);
   }
   elsif($xlsType =~ /string/) {
      if(not defined $xlsSize) {
         $xlsSize = length($xlsValue);
      }

      $xmlSize = $xlsSize;
      $xmlType = "uint8";

      my @unpacked = unpack("C*", $xlsValue);
      $valueArray = \@unpacked;
   }

   return [$xmlSize, $xmlType, ArrayToXMLString($valueArray, $xlsSize)];
}

sub negativeValueToString($$) {
   my ($value, $size) = @_;
   my $base = (undef, 0xFFFFFF00, 0xFFFF0000, 0xFF000000, 0x00000000)[$size];

   return sprintf("%d", ($base | $value) & ~$base);
}

sub ArrayToXMLString($$) {
   my ($values, $size) = @_;
   my $valueString = "";

   for my $part (@$values) {
      if(valueIsPositive($part, $size)) {
         $valueString .= "$part ";
      }
      else {
         $valueString .= negativeValueToString($part, $size) . " ";
      }
   }

   return $valueString;
}

sub Log($$$) {
   my ($message, $fileHandle, $verbose) = @_;
   
   if($verbose) {
      print $fileHandle $message;
   }
}

sub LogDebug($$) {
   my ($message, $verbose) = @_;

   Log("[DEBUG] " . $message . "\n", *STDOUT, $verbose);
}

sub LogError($$) {
   my ($message, $verbose) = @_;

   Log("[ERROR] " . $message . "\n", *STDERR, $verbose);
}

# code only runs of script is executed from command line
unless(caller) {
   my $args = ParseArguments(@ARGV);

   if(not ArgumentsAreValid($args)) {
      PrintUsage();
      die;
   }

   Main($args);
}
