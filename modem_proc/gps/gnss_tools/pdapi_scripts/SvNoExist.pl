# 
#$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/pdapi_scripts/SvNoExist.pl#1 $ $DateTime: 2016/12/13 07:59:45 $ $Author: mplcsds1 $
#*****************************************************************************
# Description:
# This file checks ..\..\almanac_provisioning_scripts for the AlmFileXX files.
# For each file that is not there, the corresponding Sv is put into the no
# exist list.  Once all files are checked, the no exist list is loaded to the
# device.
#
# Note: No more than 8 Sv's can be in the No Exist List.
#
# Example Usage:
# perl SvNoExist.pl <COM Port>
#   COM Port: The Com Port the device is on. ie 2 if the device is on COM2.
# 
#*****************************************************************************

use vars qw(%opts);
use SvNoExist;
use Win32::OLE;
use Win32::OLE::Variant;
use Getopt::Long;

# Set the timeout to 2 seconds.
use constant TIMEOUT => 2000;

%opts = ();
GetOptions( \%opts, 'help');

&usage() if ($opts{'help'});

my $SvNoExistMask = 0;
my $port_num = int($ARGV[0]);

################################################################################
# Check for user input and prompt if arguments are not provided
################################################################################
$port_num = $ARGV[0];

# Allow user input for comport
if (not defined ($port_num))
{
  print("\nUsage: perl pd_GetPos.pl <COM Port>\n\n");
  print ("Please enter the COM port the FFA is connected to\n");
  chomp ($port_num = <STDIN>);
  print("\n");
}

################################################################################
# Create QXDM instance
################################################################################
my $qxdm_app = new Win32::OLE 'QXDM.Application';
if (!$qxdm_app)
{
  print "ERROR: Unable to invoke the QXDM application.\n";
  die;
}

# Specify QXDM COM port 
$qxdm_app->{COMPort} = $port_num;

# Wait until phone is connected
while( $qxdm_app->{IsPhoneConnected} == 0 )
{
   print "Unable to Connect to the Phone. Retrying.\n";
   sleep(1);
}

################################################################################
# Go through and check which AlmFileXX files are not present
################################################################################
for($i = 0; $i < 32; $i++)
{
  $alm_file = sprintf("../almanac_provisioning_scripts/AlmFile%02u", $i);

  if(!(-e "$alm_file"))
  {
    $SvNoExistMask |= (1 << $i);
  }
}

################################################################################
# Build the packet
################################################################################
$req = new SvNoExist;
$req->{"no_exist_mask"}        = $SvNoExistMask;
$req->{"action"}               = 0;
                                 # 0: Set Sv No Exist to no_exist_mask

# Initialize the request as a Variant.
my $req_pkt = $req->BuildSvNoExistPkt();

# Send command.
printf("Sending Standalone/Single-SV cmd \n");
my $rsp_pkt_var = $qxdm_app->SendDmIcdPacketEx($req_pkt, TIMEOUT);
my $rsp_len_var = length($rsp_pkt_var);

printf("Sv No Exist Response: ");
my @results = unpack("C$rsp_len_var", $rsp_pkt_var);
print "@results\n";

$qxdm_app = NULL;

################################################################################
# METHOD:
#   usage
#
# DESCRIPTION:
#   Display usage information.
#
# RETURN VALUE:
#   None
#
################################################################################
sub usage
{
  my $usage = <<END;

SvNoExist

Syntax:   SvNoExist [options] <COM Port>

Options:  -help               Display this help message.

Parameters:
  Com Port    The communications port as a number used to communicate with the
              device.

Example:  perl SvNoExist.pl 2

END

  print "$usage";
  exit (0);
}

