###########################################################################
# $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/pdapi_scripts/pd_SetTimeInfo.pl#1 $
# $DateTime: 2016/12/13 07:59:45 $
# $Author: mplcsds1 $
#
# This example demonstrates using a DM Command to initiate injecting 
# time information into the LSM and the MGP via the PDAPI using the
# CGPS_INJECT_TIME_CMD. This example first initializes the client and
# registers for PD events. 
#
# The following values should be set by the user before starting the
# script: 
#
# 1. COM PORT for QXDM should be modified in the line 
#    $qxdm_app->{COMPort} = 1; # Default is COM1
# 2. Any pdsm_xtra_inject_time_info related values
#
##########################################################################

use vars qw(%opts);

use Win32::OLE;
use Win32::OLE::Variant;

use Getopt::Long;
use pd_StartDiag;
use pd_SetTimeInfo;
use pd_EndDiag;
use POSIX;


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

######################################################################
# Check for user input and prompt if arguments are not provided
######################################################################

$comport     = $ARGV[0];
$client_type = $ARGV[1];
$count       = 0;

# Allow user input for comport
if (not defined ($comport)) {
    print("\nUsage: perl pd_GetPos.pl <COM port> <Client Type>\n\n");
    print ("Please enter the COM port the FFA is connected to\n");
    chomp ($comport = <STDIN>);
    print("\n");
    $count = 1;
}

# Allow user input for Client Type
if (not defined ($client_type)) {
    if ($count == 0) {
        print("\nUsage: perl pd_GetPos.pl <COM port> <Client Type>\n\n");
    }
    # Prompt user to enter Client Type
    print ("Please enter the Client Type
    0 - BROWSER         
    1 - UI                 
    2 - PDA            
    3 - DM                
    4 - NI        
    5 - BREW            
    6 - JAVA            
    7 - TEST           
    8 - TEST2
    9 - OEM\n");
    chomp ($client_type = <STDIN>);
    print("\n");
}

######################################################################
# Open QXDM Connection by setting the COM port from command line.
######################################################################

# COM Port value should be supplied as first argument to this script
$qxdm_app->{COMPort} = $comport;

# Wait until phone is connected
while ($qxdm_app->{IsPhoneConnected} == 0)
{
  print "Unable to Connect to the Phone. Retrying...\n";
  sleep(1);
}

$qxdm_app->SendScript("logging on");

######################################################################
# Global variables initialized here
######################################################################

my @client_ids;
# Set the timeout to 2 seconds.
my $timeout = 2000;

######################################################################
# Subroutine to wait for an event for a specified timeout
######################################################################

sub eventQXDM
{
  my ($timeEvent, $timeOut) = @_;

  $_ = $timeEvent;
  printf ("    QXDM ===>  wait_event $timeEvent \n");
  return if ($configAttributes{"offline"});

  unless (exists $eventHash{$timeEvent})
  {
    printf ("Invalid event <$timeEvent>\n");
    return;
  }

  my $rspPkt = $qxdm_app->WaitEvent ($eventHash{$timeEvent}, $timeOut);
  my $rspLen = length ($rspPkt);

  if ($rspLen <= 0)
  {
    printf ("Event <$timeEvent> not received, timeOut.\n");
    return 0;
  }
  else
  {
    # Unpack the binary data.
    my (@results) = unpack ("c$rspLen", $rspPkt);
    printf ("Event name = $results[18], Event data = $results[19]\n");
    printf ("Received event <$timeEvent> Length: <$rspLen> @results\n");
    return ($results[18], $results[19]);
  }
}

######################################################################

my @client_ids;
# Set the timeout to 2 seconds.
my $timeout = 2000;

sleep(2);
my $time_ms = (time() - 315964800) * 1000.0; 	#   unix timestamp + sec btwn Jan 1,1970 & Jan 6,1980
my $upper1 = $time_ms / (65536.0 * 65536.0);
my $upper = floor($upper1);
my $lower = $time_ms - (65536*65536)*$upper;
my $tunc = 7500; 						# set time uncertainty to 7.5 seconds
 
print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
print("Initialize, Register, & Activate the client\n");
print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

#######################################################################
# Build a diagnostic request for CGPS_INIT_CMD to initialize the client
#######################################################################

  $req_apicmd1 = new pd_StartDiag;
  $req_apicmd1->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_apicmd1->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_apicmd1->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_apicmd1->{"cgps_cmd_code"}   = 0;   # CGPS Cmd Code
  $req_apicmd1->{"version"}         = 1;   # Version Number
  $req_apicmd1->{"client_id"}       = $client_type; # Client Type

  ######################################################################
  # Construct the DM CGPS_INIT_CMD capture request packet
  ######################################################################

  my $req_apicmd_pkt = $req_apicmd1->SetCaptureCmd();
  printf ("Sending CGPS_INIT_CMD: Initialize client type %d\n", $clientType);

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  my $rsp_apicmd_pkt = $qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, $timeout);

  my $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf ("DM CGPS_INIT_CMD Response Packet Length: %d\n", $rsp_apicmd_len);

  $req_apicmd1->ParseData($rsp_apicmd_pkt);

  $status    = $req_apicmd1->GetStatus();
  $client_id = $req_apicmd1->GetClientId();
  my $orig_client_id = $client_id;
  printf ("Status = %d, Client Id = %d\n\n", $status, $client_id);

  sleep(2);

############################################################################
# Build a diagnostic request for CGPS_REGISTER_PD_CMD to register the client
# Registers the client to receive registered PD events
############################################################################

  $req_apicmd2 = new pd_StartDiag;
  $req_apicmd2->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_apicmd2->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_apicmd2->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_apicmd2->{"cgps_cmd_code"}   = 1;   # CGPS Cmd Code
  $req_apicmd2->{"version"}         = 1;   # Version Number
  $req_apicmd2->{"client_id"}       = $client_id;  # Client ID from Init
  $req_apicmd2->{"reg_type"}        = 0;   # Register

  ######################################################################
  # Construct the DM CGPS_REGISTER_PD_CMD capture request packet
  ######################################################################

  my $req_apicmd_pkt = $req_apicmd2->SetCaptureCmd();
  printf ("Sending CGPS_REGISTER_PD_CMD: Register client ID %d\n", $client_id);

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  my $rsp_apicmd_pkt = $qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, $timeout);

  my $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf ("DM CGPS_REGISTER_PD_CMD Response Packet Length: %d\n", $rsp_apicmd_len);

  $req_apicmd2->ParseData($rsp_apicmd_pkt);

  $status    = $req_apicmd2->GetStatus();
  $client_id = $req_apicmd2->GetClientId();
  printf ("Status = %d, Client Id = %d\n\n", $status, $client_id);

  sleep(2);

#########################################################################
# Build a diagnostic request for CGPS_ACTIVATE_CMD to activate the client
# Activates the client so that it can be notified of PD events
#########################################################################

  $req_apicmd3 = new pd_StartDiag;
  $req_apicmd3->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_apicmd3->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_apicmd3->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_apicmd3->{"cgps_cmd_code"}   = 7;   # CGPS Cmd Code
  $req_apicmd3->{"version"}         = 1;   # Version Number
  $req_apicmd3->{"client_id"}       = $client_id;  # Client ID from Init

  ######################################################################
  # Construct the DM CGPS_ACTIVATE_CMD capture request packet
  ######################################################################

  my $req_apicmd_pkt = $req_apicmd3->SetCaptureCmd();
  printf ("Sending CGPS_ACTIVATE_CMD: Activate client ID %d\n", $client_id);

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  my $rsp_apicmd_pkt = $qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, $timeout);

  my $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf ("DM CGPS_ACTIVATE_CMD Response Packet Length: %d\n", $rsp_apicmd_len);

  $req_apicmd3->ParseData($rsp_apicmd_pkt);

  $status    = $req_apicmd3->GetStatus();
  $client_id = $req_apicmd3->GetClientId();
  printf ("Status = %d, Client Id = %d\n\n", $status, $client_id);

#############################################################################
# Build a diagnostic request for CGPS_INJECT_TIME_CMD to inject external time 
#############################################################################

  $req_set_time_info = new pd_SetTimeInfo;
  $req_set_time_info->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_set_time_info->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_set_time_info->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_set_time_info->{"cgps_cmd_code"}   = 17;  # CGPS Cmd Code
  $req_set_time_info->{"version"}         = 1; 	 # Version Number
  $req_set_time_info->{"client_id"}       = $client_id; # will be returned by PDAPI upon calling this script
  $req_set_time_info->{"time_lower"}      = $lower;     # lower 32 bits of time. Time is a uint64 in msec
  $req_set_time_info->{"time_upper"}      = $upper;     # upper 32 bits of time
  $req_set_time_info->{"time_unc"}        = $tunc;      # time uncertainty
  $req_set_time_info->{"time_src"}        = 0;   # 0=ref to Host time
                                                 # 1=ref to Network time
  $req_set_time_info->{"is_gps"}          = 1;   # 0=ref to UTC time
                                                 # 1=ref to GPS time
                                                 # valid only if time_src is set to Network time
  $req_set_time_info->{"disc_count"}           = 0;	# discontinuity count
  $req_set_time_info->{"force_flag"}           = 0;	# 0= don't force, 
													#1= force acceptance

  ######################################################################
  # Construct the DM CGPS_INJECT_TIME_CMD capture request packet
  ######################################################################

  my $req_set_time_pkt = $req_set_time_info->SetCaptureCmd();
  printf ("Sending CGPS_INJECT_TIME_CMD: Inject Time client ID %d\n", $client_id);

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  my $rsp_set_time_pkt_var = $qxdm_app->SendDmIcdPacketEx( $req_set_time_pkt, $timeout);
  my $rsp_set_time_len_var = length($rsp_set_time_pkt_var);
  printf ("DM CGPS_INJECT_TIME_CMD Response Packet Length: %d\n", $rsp_set_time_len_var);
 
  printf("\nInjected data: time=" . sprintf("%0.1f",$time_ms) . ", tunc=" . sprintf("%6.1f",$tunc) . ", timeSrc=" . sprintf("%1d",$req_set_time_info->{"time_src"}) . ", force=" . sprintf("%1d",$req_set_time_info->{"force_flag"}) . "\n");

  $req_set_time_info->ParseData($rsp_set_time_pkt_var);

  printf( "\nDM Response length: %d\n ", $rsp_set_time_len_var );
  $status    = $req_set_time_info->GetStatus();
  $client_id = $req_set_time_info->GetClientId();
  printf ("Status = %d, Client Id = %d\n\n", $status, $client_id);

  print("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
print("DeActivate, DeRegister, & Release the client\n");
print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

#############################################################################
# Build a diagnostic request for CGPS_DEACTIVATE_CMD to deactivate the client
#############################################################################

  $req_apicmd1 = new pd_EndDiag;
  $req_apicmd1->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_apicmd1->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_apicmd1->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_apicmd1->{"cgps_cmd_code"}   = 8;   # CGPS Cmd Code
  $req_apicmd1->{"version"}         = 1;   # Version Number
  $req_apicmd1->{"client_id"}       = $orig_client_id; # Client Type

  ######################################################################
  # Construct the DM CGPS_DEACTIVATE_CMD capture request packet
  ######################################################################

  my $req_apicmd_pkt = $req_apicmd1->SetCaptureCmd();
  printf ("Sending CGPS_DEACTIVATE_CMD: Deactivate client type %d\n", $clientType);

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  my $rsp_apicmd_pkt = $qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, $timeout);

  my $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf ("DM CGPS_DEACTIVATE_CMD Response Packet Length: %d\n", $rsp_apicmd_len);

  $req_apicmd1->ParseData($rsp_apicmd_pkt);

  $status    = $req_apicmd1->GetStatus();
  $client_id = $req_apicmd1->GetClientId();
  printf ("Status = %d, Client Id = %d\n\n", $status, $client_id);

  sleep(2);

##############################################################################
# Build a diagnostic request for CGPS_REGISTER_PD_CMD to Deregister the client
##############################################################################

  $req_apicmd2 = new pd_EndDiag;
  $req_apicmd2->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_apicmd2->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_apicmd2->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_apicmd2->{"cgps_cmd_code"}   = 1;   # CGPS Cmd Code
  $req_apicmd2->{"version"}         = 1;   # Version Number
  $req_apicmd2->{"client_id"}       = $client_id;  # Client ID from Deactivate
  $req_apicmd2->{"reg_type"}        = 1;   # Deregister

  ######################################################################
  # Construct the DM CGPS_REGISTER_PD_CMD capture request packet
  ######################################################################

  my $req_apicmd_pkt = $req_apicmd2->SetCaptureCmd();
  printf ("Sending CGPS_REGISTER_PD_CMD: DeRegister client ID %d\n", $client_id);

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  my $rsp_apicmd_pkt = $qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, $timeout);

  my $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf ("DM CGPS_REGISTER_PD_CMD Response Packet Length: %d\n", $rsp_apicmd_len);

  $req_apicmd2->ParseData($rsp_apicmd_pkt);

  $status    = $req_apicmd2->GetStatus();
  $client_id = $req_apicmd2->GetClientId();
  printf ("Status = %d, Client Id = %d\n\n", $status, $client_id);

  sleep(2);

#######################################################################
# Build a diagnostic request for CGPS_RELEASE_CMD to Release the client
#######################################################################

  $req_apicmd3 = new pd_EndDiag;
  $req_apicmd3->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_apicmd3->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_apicmd3->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_apicmd3->{"cgps_cmd_code"}   = 9;   # CGPS Cmd Code
  $req_apicmd3->{"version"}         = 1;   # Version Number
  $req_apicmd3->{"client_id"}       = $client_id;  # Client ID from Deactivate

  ######################################################################
  # Construct the DM CGPS_RELEASE_CMD capture request packet
  ######################################################################

  my $req_apicmd_pkt = $req_apicmd3->SetCaptureCmd();
  printf ("Sending CGPS_RELEASE_CMD: Release client ID %d\n", $client_id);

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  my $rsp_apicmd_pkt = $qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, $timeout);

  my $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf ("DM CGPS_RELEASE_CMD Response Packet Length: %d\n", $rsp_apicmd_len);

  $req_apicmd3->ParseData($rsp_apicmd_pkt);

  $status    = $req_apicmd3->GetStatus();
  $client_id = $req_apicmd3->GetClientId();
  printf ("Status = %d, Client Id = %d\n", $status, $client_id);
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

CGPS_INJECT_TIME_CMD

Syntax:   pd_SetTimeInfo [options]

Options:  -help               Display this help message.
Example:  perl pd_SetTimeInfo.pl <com port> <Client Type>

END

  print "$usage";
  exit (0);
}
