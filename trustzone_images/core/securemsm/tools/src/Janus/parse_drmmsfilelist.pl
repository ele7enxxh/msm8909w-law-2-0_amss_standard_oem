#
# This script parses config file and writes out a file named
# drmms.inp in the current directory. This drmms.inp file can be used to
# ???
#
# Usage perl parse_drmmsfilelist.pl
#
# Requirement:
#   o The script must be run from the directory that contains ???
#
use strict;
use Cwd;
sub convert_len_to_bytes;

my $wmdmr_dir  = "__wmdrm_sfs";
my $drmmsfiles;
my $drmms_file = "drmms.conf";
my $qxdm_inp_file = "drmms.inp";
my $delete_dmrms_file = "delete_janus_provision.inp";

my $cwd;

$cwd = cwd ();

chdir $wmdmr_dir or
  die "Unable to cd to $wmdmr_dir";


# Locate all of the DRMMS files for which partitioning packets will be generated.
open(DRMMS, "<$drmms_file") or die("Unable to open the drmms file [$drmms_file]: $!");
while (<DRMMS>) {
   if (/\<FILE\>(.+)\<\/FILE\>/) {
      $drmmsfiles->{$1} = 1;
   }
}
close DRMMS;

# For each file listed, pull the packet data.
my $drmmsfile;
my $drmmsfile_data;
foreach $drmmsfile (keys %{$drmmsfiles}) {
  $drmmsfile_data = '';
  die("Unable to find file $drmmsfile") if (!-e $drmmsfile);

  open(DRMFILE, "<$drmmsfile") or die ("Unable to open drmms file: $!");
  binmode DRMFILE;
  while (<DRMFILE>) {
    $drmmsfile_data .= $_;
  }
  close DRMFILE;

  $drmmsfiles->{$drmmsfile} = $drmmsfile_data;
}

# Format of data provisioning packet
#
# Byte 0          -> Subsystem command code: 75
# Byte 1          -> Subsystem ID:           58
# Byte 2          -> Subsystem command ID:    0
# Byte 3          -> Reserved
# Bytes  4- 7     -> Protocol Version:        1
# Bytes  8-11     -> Data Type:
#                      0: DRM_DIAG_ROOTCERT_DATA
#                      1: DRM_DIAG_CERT_DATA
#                      2: DRM_DIAG_PRIVATE_KEY_DATA
#                      3: DRM_DIAG_PUBLIC_KEY_DATA
#                      4: DRM_DIAG_COMMIT_DATA
#                      5: DRM_DIAG_JANUS_DATA
# Bytes 12-15     -> Packet Sequence Number:
#                     For files greater than 1K we spilit them into 1K chunks
# Bytes 16-19     -> logicalNameLen:
#                      length of file name in bytes
# Bytes 20-23     -> dataLen
#                      length of file data in bytes
# Remaining bytes -> File name, followed by file data
#

chdir $cwd;

open(INPFILE, ">$qxdm_inp_file") or
  die("Unable to open $qxdm_inp_file for writing");

# Delete the WMDRM certs, keys and rights database before sending provisioning data.
# Porting the delete_wmdrm_file.inp command data to the drmms.inp file.
open(DELETEFILE, "<$delete_dmrms_file") or
  die("Unable to open $delete_dmrms_file for reading deleting data");

while(<DELETEFILE>)
{
  if(/^#/) {               #Line is ignored
      next;
  }
  my($line) = $_;
  print INPFILE "$line";
}
close(DELETEFILE);

my $drmmsfile_len_bytes;
my $FileName;
my $FileNameLength;
my $FileNameLengthBytes;
my $i;

my $pktSize = 1024;
my $byteCounter=0;

foreach $drmmsfile (keys %{$drmmsfiles})
{
   my $packets = [];
   # Break up the data portion into packets, each of which size=$pktSize
   for (my $j=0; $j<=(length($drmmsfiles->{$drmmsfile})); $j+=$pktSize) {
      push(@{$packets}, substr($drmmsfiles->{$drmmsfile}, $j, $pktSize));
      $byteCounter = $j + $pktSize;
   }
   if($byteCounter<(length($drmmsfiles->{$drmmsfile}))) {
      push(@{$packets}, substr($drmmsfiles->{$drmmsfile}, 
                               $byteCounter, 
                               (length($drmmsfiles->{$drmmsfile}))));
   }

   # Stage the drmms file information
   $FileName            = join (',', map ord, split (//, $drmmsfile));
   $FileNameLength      = length $drmmsfile;
   $FileNameLengthBytes = convert_len_to_bytes ($FileNameLength);

   # For each packet, convert and output to INPFILE file
   my $squence_num = 0;
   foreach my $packet (@{$packets}) {
      $squence_num++;
      my $packetNameLengthBytes  = convert_len_to_bytes (length $packet);
      my $packetData             = join (',', map ord, split (//, $packet));

      my $supplemental ="";
      $supplemental = " packet ".($squence_num) if (scalar(@{$packets})>1);
      print INPFILE <<end_drmmsfiledata;
\!Provisioning data for drmmsfile $drmmsfile $supplemental
\{75,58,3,0,1,0,0,0,5,0,0,0,$squence_num,0,0,0,$FileNameLengthBytes,$packetNameLengthBytes,$FileName,$packetData\}

end_drmmsfiledata
   }
}

close INPFILE;

exit 0;

sub convert_len_to_bytes {
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

