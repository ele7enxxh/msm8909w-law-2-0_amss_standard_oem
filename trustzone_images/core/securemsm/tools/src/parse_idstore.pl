#
# This script parses an idstore directory and writes out a file named
# certs.inp in the current directory. This certs.inp file can be used to
# provision the certificates and keys specified in the given idstore
# directory over the DRM diag interface.
#
# Usage: perl parse_idstore.pl
#        perl parse_idstore.pl <idstore_dir_path>
#
#
use strict;
use Cwd;
use English;

my %certs;
my %keys;
my %roots;
my %certdata;
my %keydata;
my %rootcertdata;

# Disable output buffering
#
$OUTPUT_AUTOFLUSH = 1;

################################################################################
# 
# CONFIGURATION PARAMETERS
#
# default_id_store_dir: default path to directory containing the id store files.
#                       can be overridden by passing in a path as a command line
#                       argument.
#
# idstore_file: name of idstore config file.
#
# cert_dir: name of subdirectory within the idstore directory that contains all
#           the certificates
#
# keys_dir: name of subdirectory within the idstore directory that contains all
#           the keys
#
# cert_id: keyword that indicates the beginning of cert data in a cert file
#
# root_id: keyword that indicates the beginning of rootcert data in a cert file
#
# key_id: keyword that indicates the beginning of key data in a key file
#
# output_file: path to output file. all the extracted configuration data,
#              cert data and key data are written to this file.
#
################################################################################

my $default_idstore_dir = "idstore";
my $idstore_file        = "idstore.conf";
my $cert_dir            = "certs";
my $keys_dir            = "keys";

my $cert_id = "CERT";
my $key_id  = "KEYS";
my $root_id = "ROOT";

my $output_file = "certs.inp";

#
# Use the user-specified idstore directory if there is one. Otherwise, use
# the default.
#
my $idstore_dir = shift;
$idstore_dir = $default_idstore_dir if (!-e $idstore_dir);

my $cwd;

$cwd = cwd ();

if (!chdir ($idstore_dir))
{
  print "\nUnable to cd to idstore directory $idstore_dir.\n";
  goto error_return;
}

#
# Open the idstore config file and parse it.
#
if (!open (IDSTORE, "<$idstore_file"))
{
  print "\nUnable to open idstore file $idstore_file: $!\n";
  goto error_return;
  print "Idstore data extraction failed.\n";
  exit -1;
}

my $i = 1;
while (<IDSTORE>)
{
   if (/\<$cert_id\>(.+)\<\/$cert_id\>/)
   {
     $certs{$1} = $i++;
   }
   if (/\<$key_id\>(.+)\<\/$key_id\>/)
   {
     $keys{$1} = $i++;
   }
   if (/\<$root_id\>(.+)\<\/$root_id\>/)
   {
     $roots{$1} = $i++;
   }
}

close IDSTORE;

#
# Open all the cert files and extract cert data
#
if (!chdir ($cert_dir))
{
  print "\nUnable to cd to cert directory $cert_dir\n";
  goto error_return;
}

my $cert;
foreach $cert (keys %certs)
{
  if (!-e $cert)
  {
    print "\nUnable to find cert file $cert\n";
    goto error_return;
  }

  if (!open (CERTFILE, "<$cert"))
  {
    print "\nUnable to open cert file $cert: $!\n";
    goto error_return;
  }

  binmode CERTFILE;

  $certdata{$cert} = '';
  while (<CERTFILE>)
  {
    $certdata{$cert} .= $_;
  }
  close CERTFILE;
}

#
# Open all the rootcert files and extract cert data
#
my $root;
foreach $root (keys %roots)
{
  if (!-e $root)
  {
    print "\nUnable to find root cert file $root\n";
    goto error_return;
  }

  if (!open (CERTFILE, "<$root"))
  {
    print "\nUnable to open root cert file $root: $!\n";
    goto error_return;
  }

  binmode CERTFILE;

  $rootcertdata{$root} = '';
  while (<CERTFILE>)
  {
    $rootcertdata{$root} .= $_;
  }
  close CERTFILE;
}

#
# Open all the key files and extract key data
#
if (!chdir ("../$keys_dir"))
{
  print "\nUnable to cd to key directory $keys_dir\n";
  goto error_return;
}

my $key;
foreach $key (keys %keys)
{
  if (!-e $key)
  {
    print "\nUnable to find key file $key\n";
    goto error_return;
  }

  if (!open (KEYFILE, "<$key"))
  {
    print "\nUnable to open key file $key: $!\n";
    goto error_return;
  }
  binmode KEYFILE;

  $keydata{$key} = '';
  while (<KEYFILE>)
  {
    $keydata{$key} .= $_;
  }
  close KEYFILE;
}

chdir $cwd;

################################################################################
#
# Format of data provisioning packet
#
# Byte 0          -> Subsystem command code: 75
# Byte 1          -> Subsystem ID:           58
# Byte 2          -> Subsystem command ID:    0
# Byte 3          -> Reserved
# Bytes  4-7      -> Protocol Version:        1
# Bytes  8-11     -> Data Type:
#                      0: DRM_DIAG_ROOTCERT_DATA
#                      1: DRM_DIAG_CERT_DATA
#                      2: DRM_DIAG_PRIVATE_KEY_DATA
#                      3: DRM_DIAG_PUBLIC_KEY_DATA
#                      4: DRM_DIAG_COMMIT_DATA
# Bytes 12-15     -> Format Type:
#                      0: DRM_DIAG_PKCS8_FORMAT
#                      1: DRM_DIAG_PKCS12_FORMAT
#                      2: DRM_DIAG_RAW_FORMAT
# Bytes 16-19     -> logicalNameLen:
#                      length of cert or key name in bytes
# Bytes 20-23     -> dataLen
#                      length of cert or key data in bytes
# Remaining bytes -> cert or key name, followed by cert or key data
#
################################################################################

if (!open (OUTPUTFILE, ">$output_file"))
{
  print "\nUnable to open output file $output_file for writing\n";
  goto error_return;
}

my $cert_len_bytes;
my $cert_name;
my $cert_name_len;
my $cert_name_len_bytes;
my $cert_data;
foreach $cert (sort {$certs{$a} <=> $certs{$b}} keys %certs)
{
  print "Storing data for cert file $cert\n";
  $cert_name           = join (',', map ord, split (//, $cert));
  $cert_name_len       = length $cert;
  $cert_name_len_bytes = convert_len_to_bytes ($cert_name_len);
  $cert_data           = $certdata{$cert};
  $cert_len_bytes      = convert_len_to_bytes (length $cert_data);
  $cert_data           = join (',', map ord, split (//, $cert_data));
  print OUTPUTFILE <<end_certdata;
\!Provisioning data for cert $cert
\{75,58,0,0,1,0,0,0,1,0,0,0,2,0,0,0,$cert_name_len_bytes,$cert_len_bytes,$cert_name,$cert_data\}

end_certdata
}

foreach $cert (sort {$roots{$a} <=> $roots{$b}} keys %roots)
{
  print "Storing data for root cert file $cert\n";
  $cert_name           = join (',', map ord, split (//, $cert));
  $cert_name_len       = length $cert;
  $cert_name_len_bytes = convert_len_to_bytes ($cert_name_len);
  $cert_data           = $rootcertdata{$cert};
  $cert_len_bytes      = convert_len_to_bytes (length $cert_data);
  $cert_data           = join (',', map ord, split (//, $cert_data));
  print OUTPUTFILE <<end_rootcertdata;
\!Provisioning data for root cert $cert
\{75,58,0,0,1,0,0,0,0,0,0,0,2,0,0,0,$cert_name_len_bytes,$cert_len_bytes,$cert_name,$cert_data\}

end_rootcertdata
}

my $key_len_bytes;
my $key_name;
my $key_name_len;
my $key_name_len_bytes;
my $key_data;
foreach $key (sort {$keys{$a} <=> $keys{$b}} keys %keys)
{
  print "Storing data for key file $key\n";
  $key_name           = join (',', map ord, split (//, $key));
  $key_name_len       = length $key;
  $key_name_len_bytes = convert_len_to_bytes ($key_name_len);
  $key_data           = $keydata{$key};
  $key_len_bytes      = convert_len_to_bytes (length $key_data);
  $key_data           = join (',', map ord, split (//, $key_data));
  print OUTPUTFILE <<end_keydata;
\!Provisioning data for key $key
\{75,58,0,0,1,0,0,0,2,0,0,0,2,0,0,0,$key_name_len_bytes,$key_len_bytes,$key_name,$key_data\}

end_keydata
}

print OUTPUTFILE <<end_commitcmd;
\!Commit all provisioned data
\{75,58,0,0,0,0,0,0,4,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0\}
end_commitcmd

close OUTPUTFILE;

print <<success_print;

===============================================================================
SUCCESS. Idstore data extraction successful.
Output stored in file $output_file in the current directory.
===============================================================================
success_print

chdir $cwd;

exit 0;

error_return:
  print <<error_print;

===============================================================================
FAILURE. Idstore data extraction failed!
Please examine debug output to see where the failure occurred.
===============================================================================
error_print

chdir $cwd;

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

