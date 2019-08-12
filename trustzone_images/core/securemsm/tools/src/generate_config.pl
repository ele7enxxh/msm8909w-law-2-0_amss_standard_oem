################################################################################
#
# This script parses an xml config file and writes out its contents
# in a form that can be sent to the target over Diag.
#
# Usage: perl generate_config.pl <drm_system> <config_file> <output_file>
#
#        <drm_system> can be either oma or janus
#
################################################################################
use strict;
use Cwd;
use English;

my %keys;
my %values;

# Disable output buffering
#
$OUTPUT_AUTOFLUSH = 1;

################################################################################
# 
# CONFIGURATION PARAMETERS
#
# config_file: path to the config file to be read in.
#
# output_file: path to output file. all the extracted configuration data
#              is written to this file.
#
################################################################################

my $default_system      = "oma";
my $default_config_file = "drm_config.xml";
my $default_output_file = "drm_config.inp";

#
# Supported DRM system are OMA and Janus. If nothing is specified, default
# to OMA.
#
my $drm_system = shift;
$drm_system = lc ($drm_system);
if (($drm_system ne "oma") and ($drm_system ne "janus"))
{
  print "\nNo DRM system specified. Defaulting to $default_system ...\n";
  $drm_system = $default_system;
}

my $config_file = shift;
if (!-e $config_file)
{
  $config_file = $default_config_file;
  print "\nUsing default config file $default_config_file\n";
}
else
{
  print "\nUsing user-specified config file $config_file\n";
}

my $output_file = shift;
if (!$output_file)
{
  $output_file = $default_output_file;
  print "\nUsing default output file $default_output_file\n";
}
else
{
  print "\nUsing user-specified output file $output_file\n";
}

#
# Open the config file and read it.
#
if (!open (CONFIG_FILE, "<$config_file"))
{
  print "\nUnable to open config file $config_file: $!\n";
  goto error_return;
}

my $config_data = "";
while (<CONFIG_FILE>)
{
  $config_data .= $_;
}

close CONFIG_FILE;

################################################################################
#
# Format of config file provisioning packet
#
# Byte 0          -> Subsystem command code: 75
# Byte 1          -> Subsystem ID:           58
# Byte 2          -> Subsystem command ID:    6
# Byte 3          -> Reserved
# Bytes  4-7      -> Protocol Version:        2
# Bytes  8-11     -> DRM Platform:            0 for OMA, 1 for Janus
# Bytes  8-11     -> Data Len: length of config data in bytes
# Remaining bytes -> config data
#
################################################################################

if (!open (OUTPUTFILE, ">$output_file"))
{
  print "\nUnable to open output file $output_file for writing\n";
  goto error_return;
}

my $drm_system_bytes = ($drm_system eq "oma") ?
  convert_len_to_bytes (0) : convert_len_to_bytes (1);

my $config_len_bytes = convert_len_to_bytes (length $config_data);

$config_data         = join (',', map ord, split (//, $config_data));

  print OUTPUTFILE <<end_configdata;
\!Provisioning data for config file $config_file
\{75,58,6,0,2,0,0,0,$drm_system_bytes,$config_len_bytes,$config_data\}

end_configdata

close OUTPUTFILE;

print <<success_print;

===============================================================================
SUCCESS. Config data extraction successful.
Output stored in file $output_file in the current directory.
===============================================================================
success_print

exit 0;

error_return:
  print <<error_print;

===============================================================================
FAILURE. Config data extraction failed!
Please examine debug output to see where the failure occurred.
===============================================================================
error_print

exit -1;

sub convert_len_to_bytes ()
{
  my $len = $_[0];
  my $i;
  my $len_bytes = '';

  $len_bytes = $len & 0xFF;
  for ($i = 1; $i < 4; $i++)
  {
    $len >>= 8;
    $len_bytes .= (',' . ($len & 0xFF));
  }

  return $len_bytes;
}

