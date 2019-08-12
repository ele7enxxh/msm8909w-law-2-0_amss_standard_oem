##############################################################################
# $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/pdapi_scripts/pd_StartDiag.pl#1 $
# $DateTime: 2016/12/13 07:59:45 $
# $Author: mplcsds1 $
#
# This example demonstrates using a DM Command to start a DIAG Task
# 
# This should be the first perl script called, before running any other
# scripts.
#
# Usage: perl pd_StartDiag.pl 11 (11 is the com port the FFA is connected to)
#
##############################################################################

use vars qw(%opts);

use Win32::OLE;
use Win32::OLE::Variant;

use Getopt::Long;
use pd_StartDiag;

%opts = ();
GetOptions (\%opts, 'help');

&usage() if ($opts{'help'});


######################################################################
# Create QXDM instance
######################################################################

my $qxdm_app = new Win32::OLE 'QXDM.Application';
if (!$qxdm_app)
{
  print "ERROR: Unable to invoke the QXDM application.\n";
  die;
}

#####################################################################
# Promt for user input if arguments are not provided
#####################################################################

$comport = $ARGV[0];

# Allow user input for comport
if (not defined ($comport)) {
    print("\nUsage: perl pd_StartDiag.pl <COM port>\n\n");
    print ("Please enter the COM port the FFA is connected to\n");
    chomp ($comport = <STDIN>);
    print("\n");
}

######################################################################
# Open QXDM Connection by setting the COM port from command line. 
######################################################################

# COM Port value should be supplied as first argument to this script
$qxdm_app->{COMPort} = $comport; # Default is COM1

# Wait until phone is connected
while ($qxdm_app->{IsPhoneConnected} == 0)
{
  print "Unable to Connect to the Phone. Retrying...\n";
  sleep(1);
}

######################################################################
# Global variables initialized here
######################################################################

my @client_ids;
# Set the timeout to 2 seconds.
my $timeout = 2000;

######################################################################
# Build a diagnostic request for CGPS_DIAG_START_TASK to start the
# GPSDIAG task
######################################################################

  my $req_starttask_len = 4;
  my $req_starttask_pkt = Variant(VT_ARRAY | VT_UI1, $req_starttask_len);

  $req_starttask_pkt->Put(0, 75); # Diag Cmd Code
  $req_starttask_pkt->Put(1, 13); # Diag Subsystem ID
  $req_starttask_pkt->Put(2,  8); # Diag Subsystem Cmd Code
  $req_starttask_pkt->Put(3,  0);
  printf ("Sending CGPS_DIAG_START_TASK command...\n");
 ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  my $rsp_apicmd_pkt = $qxdm_app->SendDmIcdPacketEx($req_starttask_pkt, $timeout);

  my $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf ("DM CGPS_INIT_CMD Response Packet Length: %d\n", $rsp_apicmd_len);

  sleep(2);

  $qxdm_app = NULL;

######################################################################
# METHOD:
#   usage
#
# DESCRIPTION:
#   Display usage information.
#
# RETURN VALUE:
#   None
#
######################################################################
sub usage
{
  my $usage = <<END;

CGPS_DIAG_START_TASK

Syntax:   pd_StartDiag [options]

Options:  -help               Display this help message.

Example:  perl pd_StartDiag.pl <COM Port>

END

  print "$usage";
  exit (0);
}
