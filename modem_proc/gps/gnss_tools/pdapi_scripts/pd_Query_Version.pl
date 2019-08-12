##########################################################################
# $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/pdapi_scripts/pd_Query_Version.pl#1 $
# $DateTime: 2016/12/13 07:59:45 $
# $Author: mplcsds1 $
#
# This example demonstrates using a DM Command to get the Major Rev used to 
# differentiate between Aries & Callisto. This command does not start a fix;
# rather it just sets the parameter that need to be deleted.
#
# Before calling this script, a PDAPI client will need to be
# initialized (created). Also this client will then need to register
# for PD events and activate itself before it can query for the revision.
# (Run pd_InitClient.pl)
#
# Usage: perl pd_Query_Version.pl 11 (11 is the com port the FFA is connected to)
#
##########################################################################

use vars qw(%opts);

use Win32::OLE;
use Win32::OLE::Variant;
use IPC::Open2;

use Getopt::Long;
use pd_Query_Version;

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
# Prompt for user input if arguments are not provided
#####################################################################

$comport = $ARGV[0];

# Allow user input for comport
if (not defined ($comport)) {
    print("\nUsage: perl pd_Query_Version.pl <COM port>\n\n");
    print ("Please enter the COM port the FFA is connected to\n");
    $count = 1;
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
# Build a diagnostic request for Quering the version
######################################################################

  $req_version = new pd_Query_Version;
  $req_version->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_version->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_version->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_version->{"cgps_cmd_code"}   = 200; # CGPS Cmd Code
  $req_version->{"version"}         = 1;   # Version Number
  $req_version->{"subsystem_type"}  = 0;   # Subsystem type
  $req_version->{"subcommand"}      = 0;   # sub comand
  $req_version->{"reserved"}        = 0;   # not used
  
  ######################################################################
  # Construct the DM Query_Version capture request packet
  ######################################################################

  my $req_version_pkt = $req_version->SetCaptureCmd();

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  my $rsp_version_pkt = $qxdm_app->SendDmIcdPacketEx($req_version_pkt, $timeout);

  my $rsp_version_len = length($rsp_version_pkt);
  #printf ("DM Query Version Response Packet Length: %d\n", $rsp_version_len);

  $req_version->ParseData($rsp_version_pkt);

  $Major_Rev    = $req_version->GetMajorRev();
  #printf ("Status = %d, Major Rev = %d\n\n", $status, $Major_Rev);
  print STDOUT $Major_Rev;

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

GetVersionCmd

Syntax:   pd_Query_Version [options]

Options:  -help               Display this help message.

Example:  perl pd_Query_Version.pl <com port>

END

  print "$usage";
  exit (0);
}
