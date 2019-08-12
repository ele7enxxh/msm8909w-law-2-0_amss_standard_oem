###########################################################################
# $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/pdapi_scripts/pd_EndDiag.pl#1 $
# $DateTime: 2016/12/13 07:59:45 $
# $Author: mplcsds1 $
#
# This example demonstrates using a DM Command to end a Diag Task
#
# This perl script should be called, after all the positioning sessions have
# ended
#
# Usage: perl pd_EndDiag.pl 11 (11 is the com port the FFA is connected to)
#
#############################################################################

use vars qw(%opts);

use Win32::OLE;
use Win32::OLE::Variant;

use Getopt::Long;
use pd_EndDiag;

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
    print("\nUsage: perl pd_EndDiag.pl <COM port>\n\n");
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
# Build a diagnostic request for CGPS_DIAG_END_TASK to END the
# GPSDIAG task
######################################################################

  my $req_endtask_len = 4;
  my $req_endtask_pkt = Variant(VT_ARRAY | VT_UI1, $req_endtask_len);

  $req_endtask_pkt->Put(0, 75); # Diag Cmd Code
  $req_endtask_pkt->Put(1, 13); # Diag Subsystem ID
  $req_endtask_pkt->Put(2, 12); # Diag Subsystem Cmd Code
  $req_endtask_pkt->Put(3,  0);
  printf ("Sending CGPS_DIAG_END_TASK command...\n");

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  my $rsp_apicmd_pkt = $qxdm_app->SendDmIcdPacketEx($req_endtask_pkt, $timeout);

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

CGPS_DIAG_END_TASK

Syntax:   pd_EndDiag [options]

Options:  -help               Display this help message.

Example:  pd_EndDiag.pl <COM Port>
END

  print "$usage";
  exit (0);
}
