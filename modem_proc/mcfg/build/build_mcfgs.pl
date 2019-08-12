#******************************************************************************
#* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg/build/build_mcfgs.pl#1 $
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
#
# build_mcfgs
#
# A tool to generate configuration .c files and compile them into MBNs
#
# USAGE:
#
# Sample usage:
#
#   perl build_mcfgs.pl --build_id=ACETWTAA --build_ver=3125771 
#     --configs=mcfg_sw:Verizon,mcfg_hw:all,mcfg_sw:Sprint
#
# For a complete list of parameters, see the '--usage' flag
#
#   perl mcfg_test.pl --usage
#
#
# DEVELOPERS:
#   This tool extends the hashes used in libpdc for storing all data related
#   to a given configuration. See libpdc.pm for more information.
#
#  %config_hash = {
#     type              Configuration type: either '0' or '1' (see libpdc.pm)
#     typename          Configuration type name: either 'mcfg_hw' or 'mcfg_sw'
#     workbook          The spreadsheet containing the configuration worksheets
#     name              The name of the worksheet (ie, the config name)
#     pathname          The pathname of the associate MBN
#     id                The configuration ID (the SHA1 hash of the MBN)
#  }
#
#
# DEPENDECY NOTE:
#   This tool is very dependent upon its own location as well as the components
#   it uses. As of the time of writing, the following paths are assumed:
#
#   MCFG_DIR    ../
#   Workbooks   $MCFG_DIR/mcfg_gen
#   MBN Output  $BUILD_ROOT/modem_proc/build/ms/bin/$BUILD_ID
#   mcfg_gen    ../mcfg_gen
#   Configs     $MCFG_DIR/configs
#


use strict;
use warnings;

use Cwd;
use Cwd 'abs_path';
use File::Spec;
use File::Basename;
use File::Path;
use POSIX;

use lib "../mcfg_gen/scripts";
require "mcfg_write.pm";


######################################################################
#
# GLOBAL VARIABLES
#
######################################################################
my $BUILD_ID         = undef;
my $BUILD_VER        = undef;
my $BUILD_ROOT       = undef;
my $BUILD_SIGNED     = undef;
my $CONFIGS          = "";
my @CONFIG_LIST      = ();
my $FORCE_REBUILD    = 0;
my $XML_INPUT        = 0;
my $FORCE_REGENERATE = 0;
my $SOURCES_ONLY     = 0;
my $CONFIG_VARIANT   = "generic/common/Default";
my $MCFG_DIR         = GetMCFGRoot();
my $CONFIG_SRC_DIR   = "$MCFG_DIR/mcfg_gen/$CONFIG_VARIANT/";
my $HW_CFGS_WORKBOOK = '';
my $SW_CFGS_WORKBOOK = '';
my $LEGACY_BUILDER   = 0;
my $CRM_BUILDID      = "";

# GetMCFGRoot
# 
# Parameters:
#   None
#
# Purpose: 
#   Finds the MCFG folder and returns its absolute path. This script 
#   may be run from 2 different relative levels:
#       a) MCFG folder = cwd()/.. if running build_mcfgs.pl from command line
#       b) MCFG folder = cwd()/../../../.. if build_mcfgs.exe is called by a spreadsheet.
#   This wasn't needed before since both build_mcfgs.pl and the spreadsheet were
#   one directory level away from the MCFG folder
#
# Returns:
#   Absolute path to MCFG folder
sub GetMCFGRoot {
   my $mcfg_dir = "mcfg";
   my $curr_dir = getcwd();

   do {
      # Traverse up the directory string by removing the last directory separator and foldername
      $curr_dir =~ s#^(.*)/[^/]+$#$1#;
   } while (($curr_dir !~ m#.*$mcfg_dir$#) && $curr_dir ne '');

   die ("MCFG folder not found in parent path.") if ($curr_dir eq '');

   return abs_path("$curr_dir");
}

# Remove the trailing directory separator from a provided path
#
# Parameters: directory path
#
# Returns: None
#
sub rmTrailingSeparator {
   my $path = shift;

   #print "path before: $path\n";
   $path =~ s#(\\|\/)+$##g; #remove trailing directory separators

   #print "path after: $path\n";
   return $path
}

# Updates directory and file paths according to the image/sw-product 
# arguments passed to the script. If no variant is given, the 
# "generic/common" combination is reassigned
#
# Parameters: None
#
# Returns: None
#
sub update_variant_paths {

   $CONFIG_VARIANT    = rmTrailingSeparator($CONFIG_VARIANT);
   $CONFIG_SRC_DIR    = "$MCFG_DIR/mcfg_gen/$CONFIG_VARIANT/";

   set_spreadsheet_paths();
}

# scans CONFIG_SRC_DIR directory for all hardware or software XML configs
# and returns the carrier name for each of the configs
#
# Parameters:
#    configType - "mcfg_sw" or "mcfg_hw"
#
# Returns:
#    a list of config names ex: ("Verizon", "Sprint", "ATT")
sub get_config_list_xml($) {
   my ($configType) = @_;
   my $fileNamePrefix = $configType . "_gen_";
   my $fileNamePrefixLength = length($fileNamePrefix);
   my $fileNameSuffixLength = length(".xml");

   # ls on $CONFIG_DIR
   opendir(configDir, "$CONFIG_SRC_DIR") || die "Cant open config directory $CONFIG_SRC_DIR\n";
   my @ls = readdir(configDir);
   closedir(configDir);

   # get list of config files matching given config type
   # then make a list of config names from that
   my @xmlFileConfigList = grep(/$fileNamePrefix.+\.xml/, @ls);
   my @configNameList = map { substr($_, $fileNamePrefixLength, -1*$fileNameSuffixLength) } @xmlFileConfigList; 

   return @configNameList;
}

sub main {
   parse_commandline_parameters();
   update_variant_paths();

   # if the script is run in XML mode, this part is skipped
   # otherwise, the spreadsheet is read and XMLs generated
   # for each config that needs to be built
   if(!$XML_INPUT) {
      # These modules are currently dependent on Win32::OLE.pm.
      # Requiring these only when generating XML files ensures there
      # are no OLE.pm dependency issues in Linux if an XML source is provided.
      require "mcfg_read.pm";
      require "mcfg_xls_translate.pl";

      build_config_list(\&get_worksheet_list);

      # make XML files if not already made
      for my $cfg (@CONFIG_LIST) {
         prepare_cfg($cfg);

         if (!src_exists($cfg) or $FORCE_REGENERATE) {
            build_src($cfg);
         }
      }
   }

   # reset the config list. This ensures that it is empty if
   # the above if statement was run
   @CONFIG_LIST = ();
   # scan directory for XML configs and build the config list
   build_config_list(\&get_config_list_xml);

   print "\n";
   for my $cfg (@CONFIG_LIST) {
      prepare_cfg($cfg);

     if($cfg->{name} eq "Default") {
         build_default_source($cfg);
     }

     if ($LEGACY_BUILDER)
     {
       if($BUILD_SIGNED) {
          if(!mbn_exists($cfg)) {
             print "\n\nError: the placeholder mbn for $cfg->{typename}:$cfg->{name} has not yet been created.\nRun the standard MBN build command first.\n";
          }
          elsif (-s $cfg->{pathname} != 0) {
             print "\n\nError: invalid placeholder mbn for $cfg->{typename}:$cfg->{name} (the mbn is non-empty).\nRun the standard MBN build command first.\n";
          }
          else {
             print "\nBuilding signed MBN for $cfg->{typename}:$cfg->{name}\n\n";
             build_signed_mbn($cfg);
          }
          next;
       }

       if (!$SOURCES_ONLY) {
          if (!mbn_exists($cfg) or $FORCE_REBUILD) {
             print "\nBuilding MBN for $cfg->{typename}:$cfg->{name}...";
             build_mbn($cfg);
             print "done\n";
          } else {
             print "The MBN for $cfg->{typename}:$cfg->{name} already exists.\n";
          }
       }
     }
     else
     {
       if($BUILD_SIGNED) {
          if(!mbn_exists($cfg)) {
             print "\n\nError: the placeholder mbn for $cfg->{typename}:$cfg->{name} has not yet been created.\nRun the standard MBN build command first.\n";
          }
          elsif (-s $cfg->{pathname} != 0) {
             print "\n\nError: invalid placeholder mbn for $cfg->{typename}:$cfg->{name} (the mbn is non-empty).\nRun the standard MBN build command first.\n";
          }
          else {
             print "\nBuild_mcfgs must be run in legacy mode to generate a signed mbn. See --usage for more info\n";
            # print "\nBuilding signed MBN for $cfg->{typename}:$cfg->{name}\n\n";
          }
          #next;
          return;
       }

       if (!$SOURCES_ONLY) {
          if (!mbn_exists($cfg) or $FORCE_REBUILD) {
            my $xmlFileLocation = File::Spec->canonpath("$MCFG_DIR/mcfg_gen/$CONFIG_VARIANT/" . $cfg->{typename} . "_gen_" . $cfg->{name} . ".xml");

            local $/ = undef;
            open(FILE, "<", $xmlFileLocation) or die "Unable to open xml file, \"$xmlFileLocation\"";
            my $xml = <FILE>;
            close(FILE);

            print "\nBuilding MBN for $cfg->{typename}:$cfg->{name}...\n";
            my $input_file_name = $xmlFileLocation;  # Needed argument for adding filename to comments of .c file
            my ($length_bytes, $num_items) = MCFGWrite::GenerateMCFGTargetFrmSourceXML($xml, $input_file_name, $cfg->{pathname}, $BUILD_ROOT, $CRM_BUILDID);
            print "done\n";
          } else {
             print "The MBN for $cfg->{typename}:$cfg->{name} already exists.\n";
          }
       }
     }
     print "\n";
   }
}

# set_spreadsheet_paths()
#
# If workbook paths have not been passed to the script as an argument then 
# set them according to the current image / SW-product
# 
# Parameters:
#  None
#
# Returns:
#  None
#
sub set_spreadsheet_paths {

   if ($HW_CFGS_WORKBOOK eq '') {
      $HW_CFGS_WORKBOOK = "$MCFG_DIR/mcfg_gen/$CONFIG_VARIANT/MCFG_HW_Items_List_Macro.xlsm";
   }

   if ($SW_CFGS_WORKBOOK eq '') {
      $SW_CFGS_WORKBOOK = "$MCFG_DIR/mcfg_gen/$CONFIG_VARIANT/MCFG_SW_Items_List_Macro.xlsm";
   }
}

#
# parse_commandline_parameters()
#
# Walk through the list of provided command-line parameters and set the
# global control variables accordingly.
#
# Parameters:
#  None
#
# Returns:
#  None
#
sub parse_commandline_parameters {
   use Getopt::Long;
   my $print_usage = 0;

   $BUILD_ROOT = getcwd();
   while(1) {
      if (-e "$BUILD_ROOT/modem_proc") {
         last;
      }
      my $PARENT = abs_path("$BUILD_ROOT/..");
      if ($BUILD_ROOT eq $PARENT) {
         last;
      } else {
         $BUILD_ROOT = $PARENT;
      }
   }

   # Optional command line arguments override the defaults
   GetOptions(
      'build_id:s' => \$BUILD_ID,
      'build_ver=s' => \$BUILD_VER,
      'build_root=s' => \$BUILD_ROOT,
      'sign-build' => \$BUILD_SIGNED,
      'configs=s' => \$CONFIGS,
      'force-regenerate' => \$FORCE_REGENERATE,
      'force-rebuild' => \$FORCE_REBUILD,
      'sources-only' => \$SOURCES_ONLY,
      'xml' => \$XML_INPUT,
      'hw_cfgs_workbook=s' => \$HW_CFGS_WORKBOOK,
      'sw_cfgs_workbook=s' => \$SW_CFGS_WORKBOOK,
      'source-dir=s' => \$CONFIG_VARIANT,
      "usage" => \$print_usage,
      "legacy" => \$LEGACY_BUILDER,
      'crm=s' => \$CRM_BUILDID,
   );

   if ($print_usage) {
      print "\nUsage:   build_mcfgs <parameters>\n";
      print "\nRequired Parameters:\n";
      print "  --build_id=<build id>         The build ID\n";
      print "\nOptional Parameters:\n";
      print "  --build_root=<directory>      The root directory of the build\n";
      print "  --build_ver=<build version>   The build version\n";
      print "  --configs=<config list>       The configurations to build\n";
      print "                                eg: mcfg_sw:Verizon,mcfg_hw:all\n";
      print "  --sources-only                Only generate the source files\n";
      print "  --sign-build                  Use the cert to sign the MBN\n";
      print "  --force-regenerate            Always regenerate sources files\n";
      print "  --force-rebuild               Always rebuild the MBN\n";
      print "  --hw_cfgs_workbook=<file>     The HW configs spreadsheet\n";
      print "  --hw_cfgs_workbook=<file>     The SW configs spreadsheet\n";
      print "  --source-dir=<sw-product>     The subdirectory path between mcfg_gen and the source xmls or xlsm workwookbook\n";
      print "  --xml                         Ignore workbook options. Use XML configs in mcfg_gen/image/sw_product subdirectory as input\n";
      print "  --legacy                      Uses legacy build scripts that require a complete build image\n";
      print "  --crm=<crm_buildid>           CRM build id";

      exit(0);
   }

   if (! -e "$BUILD_ROOT/modem_proc/build/ms" && $LEGACY_BUILDER) {
      print "\nError: invalid build root. Missing 'modem_proc/build/ms'\n";
      exit(0);
   }
}

# Identifies the config variant based on the spreadsheet name or lack thereof
sub get_subVariant_path {
   my ($cfg_name) = @_;
   my @subVariant = ();

   # otherwise, hyphens interpreted as follows
   #  # of hypens in name : how spreadsheet name is interpreted
   #  0 : ss name = variant
   #  1 : ss name = variant-type1
   #  2 : ss name = variant-type1-type2
   @subVariant = split('-', $cfg_name);

   # Take section before 1st hyphen and move to end of array so it becomes 
   # type1/type2/variant, e.g.  EPS_Only-3CS-DS --> 3CS/DS/EPS_Only
   push(@subVariant, shift(@subVariant));

   return  join ('/', @subVariant);
}
#
# build_config_list()
#
# Builds the list of configurations to be verified. If no '--configs' option
# was specified, the user will be asked to choose from a list of the available
# configurations.
#
# Parameters:
#    allConfigBuilder - callback to handle an item which is specified as all
#
# Returns:
#  None - the configurations are pushed into the global CONFIG_LIST
#
sub build_config_list($) {
   my ($allConfigList) = @_;

   if($CONFIGS) {
      my @configs = split(/,\s*/, $CONFIGS);
      my @config_list = ();
      
      while (@configs) {
         my $cfg_pair = shift(@configs);

         $cfg_pair =~ /(\S+):(\S+)/;

         if (!$1 or !$2) {
            die "Invalid configuration pair '$cfg_pair'\n";
         }

         if ($1 ne "mcfg_hw" and $1 ne "mcfg_sw") {
            die "Invalid configuration type '$1'";
         }

         if ($2 eq "all") {
            my @configNameList = $allConfigList->($1);
            unshift(@configs, map{"$1:$_"} reverse(@configNameList));
         } else {
            my $cfg_ref = {};
            $cfg_ref->{typename} = $1;
            $cfg_ref->{name} = $2;
            $cfg_ref->{variant_subpath} = get_subVariant_path($2);
            push(@CONFIG_LIST, $cfg_ref);
         }
      }
   } else {
      print "\nInteractive mode. Use '--configs' to enable automation mode.\n";
      print "See '--usage' for the complete list of parameters.\n\n";

      print "Which configuration type would you like to build:\n";
      my $choice = select_from_list("Hardware", "Software");
      my $cfg_type = $choice == 0 ? "mcfg_hw" : "mcfg_sw";
      my @configNames = $allConfigList->($cfg_type);

      print "\nSelect a configuration:\n";
      $choice = select_from_list(@configNames);
      my $cfg_name = $configNames[$choice];
      my $cfg_variant_subpath = get_subVariant_path( $configNames[$choice] );

      push(@CONFIG_LIST, {typename => $cfg_type, name => $cfg_name, variant_subpath => $cfg_variant_subpath});
   }
}

#
# select_from_list()
#
# A helper function that presents the user with a numerical listing of options
# and returns the user's selection.
#
# Parameters:
#  The list of options
#
# Returns
#  The index of the selected option
#
sub select_from_list {
   for my $i (1 .. scalar(@_)) {
      print " $i. $_[$i-1]\n";
   }
   print "> ";

   chomp(my $choice = <>);

   die "Error: not a digit.\n" if ($choice eq "" or !isdigit($choice));
   die "Error: invalid selection.\n" if ($choice < 1 or $choice > scalar(@_));

   return $choice-1;
}

#
# get_worksheet_list()
#
# Retrieves the list of available worksheets available in a given workbook
#
# Parameters:
#  $typename        The type of workbook. Either 'mcfg_sw' or 'mcfg_hw'
#
# Returns
#  The list of available configurations of the given type
#
sub get_worksheet_list {

   my $typename = shift;

   my $workbook_source = $typename eq "mcfg_hw" ? $HW_CFGS_WORKBOOK :
                                                  $SW_CFGS_WORKBOOK;

   my $workbook = MCFGRead::OpenExcelWorkbook($workbook_source);
   my $cfgs = MCFGRead::ExtractMcfgSheetDataFromWorkbook($workbook);
   $workbook->Close();

   return keys(%$cfgs);
}

#
# prepare_cfg()
#
# Initializes a configuration hash
#
# Parameters:
#  $cfg           A reference to the newly created and incomplete config hash
#
# Returns:
#  None - changes are made directly via the reference
#
sub prepare_cfg {
   my $cfg = shift;

   $cfg->{workbook} = $cfg->{typename} eq "mcfg_hw" ? $HW_CFGS_WORKBOOK :
                                                      $SW_CFGS_WORKBOOK;

   $cfg->{pathname} = "$MCFG_DIR/configs/$cfg->{typename}/$CONFIG_VARIANT/$cfg->{variant_subpath}/$cfg->{typename}.mbn";
}

#
# src_exists()
#
# Checks if the .c file associated with a given configuration exists
#
# Parameters:
#  $cfg           A reference to the configuration hash
#
# Returns:
#  $TRUE if the associated source file exists; $FALSE otherwise
#
sub src_exists {
   my $cfg = shift;

   return -e "$MCFG_DIR/mcfg_gen/$CONFIG_VARIANT/" . $cfg->{typename} . "_gen_" . $cfg->{name} . ".xml";
}

#
# mbn_exists()
#
# Checks if the MBN associated with a given configuration exists
#
# Parameters:
#  $cfg           A reference to the configuration hash
#
# Returns:
#  $TRUE if the associated MBN exists; $FALSE otherwise
#
sub mbn_exists {
   my $cfg = shift;

   return -e $cfg->{pathname};
}

#
# build_src()
#
# Builds the .c file that was generated from the worksheet
#
# Parameters:
#  $cfg           A reference to the configuration hash
#
# Returns:
#  None
#
sub build_src {
   my $cfg = shift;
   my $xmlFileLocation = "$MCFG_DIR/mcfg_gen/$CONFIG_VARIANT/" . $cfg->{typename} . "_gen_" . $cfg->{name} . ".xml";
   my $xmlOutputFile_read;
   my $xmlOutputFile_write;
   my $workbook = MCFGRead::OpenExcelWorkbook($cfg->{workbook});
   $workbook or die "Unable to open workbook '$cfg->{workbook}'";
   my $mcfg_sheet_data = MCFGRead::ExtractMcfgSheetDataFromWorkbook($workbook);
   my $rows = MCFGRead::ReadRowsFromWorksheet($cfg->{name}, $workbook);

   # Close workbook as soon as possible to ensure no file handles are orphened if die occurs
   $workbook->{Saved} = 1; # Suppress the "Save?" dialog box
   $workbook->Close();

   my $xml_content = '';
   if (-e $xmlFileLocation)
   {
     open($xmlOutputFile_read, "+<", $xmlFileLocation) or die "cannot open $xmlFileLocation for reading";
     $xml_content = do { local $/; <$xmlOutputFile_read> };
     close($xmlOutputFile_read) || die "Couldn't close file properly";
   }

   # translate spreadsheet to XML file
   open($xmlOutputFile_write, "+>", $xmlFileLocation) or die "cannot open $xmlFileLocation for writing";

   print STDOUT "Generating XML file for $cfg->{name}...";
   GenerateMcfgXML($rows, $mcfg_sheet_data->{$cfg->{name}}, $xmlOutputFile_write, $xml_content, $BUILD_ROOT);
   close($xmlOutputFile_write);
   print STDOUT "done\n";
}

sub build_default_source($) {
   my ($cfg) = @_;
   my $xmlFileLocation = "$MCFG_DIR/mcfg_gen/$CONFIG_VARIANT/" . $cfg->{typename} . "_gen_" . $cfg->{name} . ".xml";
   my $def_output_file_name = "$MCFG_DIR/mcfg_gen/src/$cfg->{typename}_gen_devcfg.c";

   open(FILE, "<", $xmlFileLocation) or die "Unable to open xml file, \"$xmlFileLocation\"";
   local $/ = undef;
   my $xml = <FILE>;
   close(FILE);
   
   MCFGWrite::GenerateMcfgSegmentFileXML($xml, $xmlFileLocation, $def_output_file_name, "$BUILD_ROOT/", $MCFGWrite::DEFAULT_C_OUTPUT_FILE_TYPE, 0, undef, $CRM_BUILDID);
   $MCFGWrite::DEFAULT_C_OUTPUT_FILE_TYPE;  # perl complains if this constant isn't referred to more than once, results in warning
   #MCFGWrite::GenerateMcfgDefaultSourceFileXML($xml, $def_output_file_name, "$BUILD_ROOT/");
   # GenerateMcfgDefaultSourceFileXML deprecated when duplicate code was merged to GenerateMcfgSegmentFileXML
}

#
# build_mbn()
#
# Builds a configuration .c file into an MBN and places it in the configs folder
#
# Parameters
#  $cfg           A reference to the configuration hash
#
# Returns:
#  None
#
sub build_mbn {
   use Cwd;

   my ($cfg, $signBuild) = @_;

   # Using the full path to the build scr ensures the OS can always
   # find the file. Issues seen on linux in which files are not found
   # in current working directory.
   my $build_scr = 'build.cmd'; # Build script for windows
   # $^O variable contains OS name
   my $os = $^O; 
   if ($os !~ /Win/) 
   {
      $build_scr = 'build.sh';# Build script for linux
   }
   $build_scr = File::Spec->catfile("$BUILD_ROOT/modem_proc/build/ms", $build_scr);
 
   my @build_params = ("$build_scr", "BUILD_ID=$BUILD_ID");
   if ($BUILD_VER) {
      push(@build_params, "BUILD_VER=$BUILD_VER");
   }

   push(@build_params, "image=$cfg->{typename}");

   if ($cfg->{typename} eq "mcfg_hw") {
      push(@build_params, "MCFG_HW_TYPE=$cfg->{name}");
   } 
   elsif ($cfg->{typename} eq "mcfg_sw") {
      push(@build_params, "MCFG_SW_TYPE=$cfg->{name}");
   }

   push(@build_params, "MCFG_SW_PRODUCT=$CONFIG_VARIANT");

   chdir("$BUILD_ROOT/modem_proc/build/ms") or die ("Cannot change to ms dir. Build root: '$BUILD_ROOT' - Error: $!\n");

   if (! $signBuild) {
      print "Cleaning the build directory...\n\n";
      push(@build_params, "--clean");
      if (system(@build_params)) {
         die "Error cleaning build directory\n";
      }
      pop(@build_params);
   }

   if (system(@build_params)) {
      print "Called build with following parameters:\n";
      print join(" ", @build_params). "\n";
      die "Error building mbn for $cfg->{name}\n";
   }

   # Make sure parent directory exists before attempting to write output file
   my $dirpath = dirname($cfg->{pathname});
   if (! -d "$dirpath") {
      mkpath($dirpath) or die "Could not create parent directory: $!";
   }

   use File::Copy 'move';
   my $s = File::Spec->canonpath( "$BUILD_ROOT/modem_proc/build/ms/bin/$BUILD_ID/$cfg->{typename}.mbn" );

   if (! move($s, $cfg->{pathname})) {
      die "Error moving $s to $cfg->{pathname}: $!";
   }

   # Save the authentication related files
   #
   use File::Spec;
   my $src_dir = File::Spec->rel2abs("$BUILD_ROOT/modem_proc/build/ms/bin/$BUILD_ID/$cfg->{typename}");
   my $dest_dir = File::Spec->rel2abs("$MCFG_DIR/configs/$cfg->{typename}/$cfg->{name}/$cfg->{typename}");
   system("xcopy $src_dir $dest_dir /I /E /R /Y");

   $src_dir = File::Spec->rel2abs("$BUILD_ROOT/modem_proc/build/bsp/$cfg->{typename}_img/build/$BUILD_ID");
   $dest_dir = File::Spec->rel2abs("$MCFG_DIR/configs/$cfg->{typename}/$cfg->{name}/$BUILD_ID");
   system("xcopy $src_dir $dest_dir /I /E /R /Y");

   print "\nMBN for $cfg->{typename}:$cfg->{name} built successfully\n\n";
}

#
# build_signed_mbn()
#
# Copies the authentication hash files from the configs directory into the
# build directory, enabling the build system to build an authenticated image.
#
# Parameters
#  $cfg           A reference to the configuration hash
#
# Returns:
#  None
#
sub build_signed_mbn {
   my $cfg = shift;

   # Restore the authentication related files
   use File::Spec;
   my $src = File::Spec->rel2abs("$MCFG_DIR/configs/$cfg->{typename}/$CONFIG_VARIANT/$cfg->{variant_subpath}/$cfg->{typename}");
   my $dest = File::Spec->rel2abs("$BUILD_ROOT/modem_proc/build/ms/bin/$BUILD_ID/$cfg->{typename}");
   system("xcopy $src $dest /I /E /R /Y");

   $src = File::Spec->rel2abs("$MCFG_DIR/configs/$cfg->{typename}/$CONFIG_VARIANT/$cfg->{variant_subpath}/$BUILD_ID");
   $dest = File::Spec->rel2abs("$BUILD_ROOT/modem_proc/build/bsp/$cfg->{typename}_img/build/$BUILD_ID");
   system("xcopy $src $dest /I /E /R /Y");

   $src = File::Spec->rel2abs("$MCFG_DIR/configs/$cfg->{typename}/$CONFIG_VARIANT/$cfg->{variant_subpath}/$cfg->{typename}.mbn");
   $dest = File::Spec->rel2abs("$BUILD_ROOT/modem_proc/build/bsp/$cfg->{typename}_img/build/$BUILD_ID");
   system("copy $src $dest");



   # With the authentication files in place, the build system should
   # automatically build a signed version of the mbn
   build_mbn($cfg, 1);
}

main();
