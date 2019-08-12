###########################################################################
# $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/pdapi_scripts/pd_AppTrackDiag.pl#1 $
# $DateTime: 2016/12/13 07:59:45 $
# $Author: mplcsds1 $
#
# This example demonstrates using a DM Command to initialize, register,
# & activate a PDAPI client using CGPS_INIT_CMD, CGPS_REGISTER_PD_CMD, and
# CGPS_ACTIVATE_CMD respectively; initiate a GPS Standalone position fix via
# PDAPI using CGPS_START_CMD; Deactivate, Deregister, & Release a PDAPI
# client using CGPS_DEACTIVATE_CMD, CGPS_REGISTER_PD_CMD, and
# CGPS_RELEASE_CMD respectively.
#
# Run pd_StartDiag.pl before calling this script to start a Diag Task
#
# Values that need to be modified for pdsm_get_pos()
#--------------------------------------------------- 
# 1.operating_type
# 2.qos_gps_sess_tout 
# 3.qos_acc_thrshld
# 4.server_ipv4_adrs
# 5.server_ipv4_port
#---------------------------------------------------
#
# Usage: perl pd_GetPos.pl 11 8 (11 is the COM Port the FFA is connected
#        to & 8 is the Client Type (TEST2))
#
##########################################################################

use vars qw(%opts);

use Win32::OLE;
use Win32::OLE::Variant;

use Getopt::Long;
use pd_StartDiag;
use pd_Getpos;
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

######################################################################
# Create QXDM2 instance
######################################################################

$qxdm_app2 = $qxdm_app->GetIQXDM2();
if ($qxdm_app2 == 0)
{
   print "QXDM does not support required interface\n";
   sleep(5);
   die;
}

# Register the QXDM client.
$ReqHandle = $qxdm_app2->RegisterQueueClient( 256 );
if ($ReqHandle == 0xFFFFFFFF)
{
   print "Unable to register as QXDM client\n";
   sleep(5);
   die;
}

# Get a configuration object.
my $clientObject = $qxdm_app2->ConfigureClientByKeys( $ReqHandle );
if ($clientObject == 0)
{
   print "Unable to register as QXDM client\n";
   sleep(5);
   die;
}

######################################################################
# Register for Required Events
######################################################################

$clientObject->AddEvent( 605 );       #EVENT_GPS_PD_SESS_START
$clientObject->AddEvent( 601 );       #EVENT_GPS_PD_FIX_START
$clientObject->AddEvent( 614 );       #EVENT_GPS_PD_POSITION
$clientObject->AddEvent( 602 );       #EVENT_GPS_PD_FIX_END
$clientObject->AddEvent( 411 );       #EVENT_GPS_PD_SESS_END

$clientObject->CommitConfig();

######################################################################
# Check for user input and prompt if arguments are not provided
######################################################################

$comport     = $ARGV[0];
$client_type = $ARGV[1];
$count       = 0;
check_client_type();

# Allow user input for comport
if (not defined ($comport)) {
    print("\nUsage: perl pd_GetPos.pl <COM Port> <Client Type>\n\n");
    print ("Please enter the COM port the FFA is connected to\n");
    chomp ($comport = <STDIN>);
    print("\n");
    $count = 1;
}

# Allow user input for Client Type
if (not defined ($client_type)) {
    if ($count == 0) {
        print("\nUsage: perl pd_GetPos.pl <COM Port> <Client Type>\n\n");
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
    check_client_type();
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

print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
print("Initialize, Register, & Activate the client\n");
print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

#######################################################################
# Build a diagnostic request for CGPS_DIAG_START_TASK to start the
# GPSDIAG task
#######################################################################

  $req_apicmd1 = new pd_StartDiag;
  $req_apicmd1->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_apicmd1->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_apicmd1->{"subsys_cmd_code"} = 8; # Diag Subsystem Cmd Code
  $req_apicmd1->{"cgps_cmd_code"}   = 0;   # CGPS Cmd Code
  $req_apicmd1->{"version"}         = 1;   # Version Number
  $req_apicmd1->{"client_id"}       = $client_type; # Client Type

  ######################################################################
  # Construct the DM CGPS_INIT_CMD capture request packet
  ######################################################################

  my $req_apicmd_pkt = $req_apicmd1->SetCaptureCmd();
  printf ("Sending CGPS_DIAG_START_TASK command...\n");

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  my $rsp_apicmd_pkt = $qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, $timeout);

  my $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf ("DM CGPS_INIT_CMD Response Packet Length: %d\n", $rsp_apicmd_len);

  $req_apicmd1->ParseData($rsp_apicmd_pkt);

  $status    = $req_apicmd1->GetStatus();
  $client_id = $req_apicmd1->GetClientId();
  printf ("Status = %d, Client Id = %d\n\n", $status, $client_id);

  sleep(2);




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

######################################################################
# Get Position Cmd parameters used for Apptracking only
######################################################################

  my $sessions  = 3000;
  my $tbf       = 1;
  sleep(2);

  print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  print("Apply get_pos to Start A-GPS Session\n");
  print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

######################################################################
# Build a diagnostic request for CGPS_START_CMD to do a Standalone fix
######################################################################

  $req_getpos = new pd_GetPos;
  $req_getpos->{"cmd_code"}          = 75;            # Diag Cmd Code
  $req_getpos->{"subsys_id"}         = 13;            # Diag Subsystem ID
  $req_getpos->{"subsys_cmd_code"}   = 100;           # Diag Subsystem Cmd Code
  $req_getpos->{"cgps_cmd_code"}     = 10;            # CGPS Cmd Code
  $req_getpos->{"version"}           = 1;             # Version Number
  $req_getpos->{"session_type"}      = 2;             # 0 = latest position
                                                      # 1 = new position
                                                      # 2 = tracking mode w/ independent fixes
  $req_getpos->{"operating_type"}    = 1;             # 1 = Standalone
                                                      # 2 = MS-BASED
                                                      # 3 = MS-ASSISTED
                                                      # 4 = Optimal Speed
                                                      # 5 = Optimal Accuracy
                                                      # 6 = Optimal Data
                                                      # 7 = Reference Position
  $req_getpos->{"server_option"}     = 1;             # 0 = NV Default
                                                      # 1 = Custom Local
  $req_getpos->{"server_addr_type"}  = 0;             # 0 = IpV4
                                                      # 1 = IpV6
  $req_getpos->{"class_id"}          = 0;             # not used
  $req_getpos->{"qos_gps_sess_tout"} = 2;             # QoS timeout value between 0-255 seconds
                                                      # Default for 1x         = 16 sec
                                                      # Default for UMTS       = 45 sec
                                                      # Default for Standalone = 60 sec
  $req_getpos->{"qos_acc_thrshld"}   = 50;            # QoS accuracy threshold in meters
  $req_getpos->{"time_bw_fixes"}     = $tbf;          # tbf only used for app tracking
  $req_getpos->{"num_fixes"}         = $sessions;     # num > 1 fix -> used for app tracking
  $req_getpos->{"client_id"}         = $client_id;    # Returned by PDAPI duriing client registration
  $req_getpos->{"reserved"}          = 0;             # not used
  $req_getpos->{"server_ipv4_adrs"}  = "10.1.2.154";  # !!Change to the local Custom Server Address!!
  $req_getpos->{"server_ipv4_port"}  = 4911;          # !!Change to the local Custom SIA port number!!

  ######################################################################
  # Construct the DM CGPS_START_CMD capture request packet
  ######################################################################

  my $req_getpos_pkt = $req_getpos->SetCaptureCmd();
  $PrevIndex = $qxdm_app2->GetClientItemCount( $ReqHandle ) - 1;   

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  my $rsp_getpos_pkt = $qxdm_app->SendDmIcdPacketEx($req_getpos_pkt, $timeout);

  my $rsp_getpos_len = length($rsp_getpos_pkt);
  printf ("\nDM CGPS_START_CMD Response Packet Length = %d\n", $rsp_getpos_len);

  $req_getpos->ParseData($rsp_getpos_pkt);

  $status    = $req_getpos->GetStatus();
  $client_id = $req_getpos->GetClientId();
  printf ("Status = %d, Client Id = %d\n\n", $status, $client_id);

  $qxdm_app = NULL;

sub check_client_type()
{
  while ($client_type > 9)
  {
    print("\nClient Type out of range. Please enter a value between 0-9
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
  }
  return;
}

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

GetPositionCmd

Syntax:   pd_GetPos [options]

Options:  -help               Display this help message.

Example:  perl pd_GetPos.pl <COM Port> <Client Type>

END

  print "$usage";
  exit (0);
}
