###########################################################################
#$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/pdapi_scripts/pd_NotifyVerify.pl#1 $
# $DateTime: 2016/12/13 07:59:45 $
# $Author: mplcsds1 $
#
# This example demonstrates using a DM Command to send a notification
# verification response to the PDAPI using CGPS_NTFY_VRFY_SND_USR_ACTN_CMD.
# First, the script waits for a notification verification request. Once it
# receives the request event, a response is sent back.
#
# 1. Set the variable $req_notifyverify->{"accept_deny"} = 0; # 0=accept, 
#                                                             # 1=deny,
#                                                             # 2=No Response
#
# for a PRESUPL MT the following variables need to set
# 11. Set the variable $$req_notifyverify->{"url_length"} = 19; # if PRES_UPL, the length of the URL
# 12. Set the variable $$req_notifyverify->{"url"} = "http://1.2.3.4:1000"; # if PRES_UPL the URL; else 0 
# all the other fields are dont' care.
#
###########################################################################

use vars qw(%opts);

use Win32::OLE;
use Win32::OLE::Variant;

use Getopt::Long;
use pd_NotifyVerify;
use pd_StartDiag;
use pd_EndDiag;

# This is the PDSM Event that will be received by this script
use constant EVENT_NOTIFY_REQ     => "EVENT_NOTIFY_REQ";
my %eventHash = (EVENT_NOTIFY_REQ => 1034);

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

$clientObject->AddEvent(1034);       #EVENT_NOTIFY_VERIFY_SEND_USER_ACTION?

$clientObject->CommitConfig();

######################################################################
# Check for user input and prompt if arguments are not provided
######################################################################

$comport     = $ARGV[0];
$client_type = $ARGV[1];
$count       = 0;
check_client_type();

# Allow user input for COM Port
if (not defined ($comport))
{
  print("\nUsage: perl pd_NotifyVerify.pl <COM Port> <Client Type>\n\n");
  print ("Please enter the COM port the FFA is connected to: \n");
  chomp ($comport = <STDIN>);
  print("\n");
  $count = 1;
}

# Allow user input for Client Type
if (not defined ($client_type))
{
  if ($count == 0)
  {
    print("\nUsage: perl pd_NotifyVerify.pl <COM Port> <Client Type>\n\n");
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
# Open QXDM Connection by setting the COM Port from command line.
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

###############################################################
# Subroutine to wait for an event for a specified timeout
################################################################

sub eventQXDM
{
  my ($timeEvent, $timeOut) = @_;

  $_ = $timeEvent;
  printf ("    QXDM ===>  wait_event $timeEvent \n");
  return if ($configAttributes{"offline"});

  unless (exists $eventHash{$timeEvent})
  {
    printf("Invalid event <$timeEvent>\n");
    return;
  }

  my $rspPkt = $qxdm_app->WaitEvent($eventHash{$timeEvent}, $timeOut);
  my $rspLen = length($rspPkt);

  if ($rspLen <= 0)
  {
    printf("Event <$timeEvent> not received, timeOut.\n");
    return 0;
  }
  else
  {
    # Unpack the binary data.
    my (@results) = unpack("c$rspLen", $rspPkt);
    printf ("Event name = $results[18], Event data = $results[19] respLen = $rspLen\n");
    printf ("Received event <$timeEvent> Length: <$rspLen> @results\n");

    return ($results[14], $results[15], $results[16], $results[17], $results[18], $results[19]);
  }
}

######################################################################
# Global variables initialized here
######################################################################

my @client_ids;
# Set the timeout to 2 seconds
my $timeout = 2000;

sleep(2);

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
  printf ("Status = %d, Client Id = %d\n\n", $status, $client_id);

  sleep(2);

#############################################################################
# Build a diagnostic request for CGPS_REGISTER_LCS_CMD to register the client
# Registers the client to receive registered PD events
#############################################################################

  $req_apicmd2 = new pd_StartDiag;
  $req_apicmd2->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_apicmd2->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_apicmd2->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_apicmd2->{"cgps_cmd_code"}   = 3;   # CGPS Cmd Code
  $req_apicmd2->{"version"}         = 1;   # Version Number
  $req_apicmd2->{"client_id"}       = $client_id;  # Client ID from Init
  $req_apicmd2->{"reg_type"}        = 0;   # Register

  ######################################################################
  # Construct the DM CGPS_REGISTER_LCS_CMD capture request packet
  ######################################################################

  my $req_apicmd_pkt = $req_apicmd2->SetCaptureCmd();
  printf ("Sending CGPS_REGISTER_LCS_CMD: Register client ID %d\n", $client_id);

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  my $rsp_apicmd_pkt = $qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, $timeout);

  my $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf ("DM CGPS_REGISTER_LCS_CMD Response Packet Length: %d\n", $rsp_apicmd_len);

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

#########################################################################
# Build a diagnostic request for CGPS_NTFY_VRFY_SND_USR_ACTN_CMD 
#########################################################################

  $req_notifyverify = new pd_NotifyVerify;
  $req_notifyverify->{"cmd_code"}         = 75;  # Diag Cmd Code
  $req_notifyverify->{"subsys_id"}        = 13;  # Diag Subsystem ID
  $req_notifyverify->{"subsys_cmd_code"}  = 100; # Diag Subsystem Cmd Code
  $req_notifyverify->{"cgps_cmd_code"}    = 16;  # CGPS Cmd Code
  $req_notifyverify->{"version"}          = 1;   # Version Number
  $req_notifyverify->{"accept_deny"}      = 0;   # 0 = Accept,
                                                 # 1 = Deny,
                                                 # 2 = No Response (from user)
  $req_notifyverify->{"client_id"}        = $client_id; # Client ID from Init
  $req_notifyverify->{"cmd_type"}         = 1;   # Command Type
                                                 # 0 = Vx
                                                 # 1 = SUPL
                                                 # 2 = UMTS CP
  # SUPL-related data
  $req_notifyverify->{"notif_priv_type"}  = 2;   # Notification Privacy Type
                                                 # 0 = No Notify, No Verify
                                                 # 1 = Notify Only
                                                 # 2 = Notify Verify Allow No Response
                                                 # 3 = Notify Verify Not Allow No Response
                                                 # 4 = Privacy Needed
                                                 # 5 = Privacy Override
  $req_notifyverify->{"lcs_flags"}        = 0x1FF; # LCS flags
  $req_notifyverify->{"presence"}         = 1;   # SLP Session ID Present?
  $req_notifyverify->{"session_id_0"}     = 0;   # Session ID field 0
  $req_notifyverify->{"session_id_1"}     = 0;   # Session ID field 1
  $req_notifyverify->{"session_id_2"}     = 0;   # Session ID field 2
  $req_notifyverify->{"session_id_3"}     = 0;   # Session ID field 3
  $req_notifyverify->{"slp_addr_type"}    = 1;   # SLP Address Type
                                                 # 0 = IPV4 Address
                                                 # 1 = URL Address
                                                 # 2 = IPV6 Address
  $req_notifyverify->{"url_len"}          = 19;  # Length of the URL value
  $req_notifyverify->{"url"} = "http://1.2.3.4:1000"; # URL value

  # Length of the SUPL-related data (variable depending on URL length)
  $req_notifyverify->{"resp_data_len"}    = 10 + $req_notifyverify->{"url_len"};

  #########################################################################
  # Construct the DM CGPS_NTFY_VRFY_SND_USR_ACTN_CMD capture request packet
  #########################################################################

  my $req_notifyverify_pkt = $req_notifyverify->SetCaptureCmd();
  printf ("Sending CGPS_NTFY_VRFY_SND_USR_ACTN_CMD: Notify Verify client ID %d\n", $client_id);

##### WHAT IS GOING ON HERE?!? #####

# Wait for the Notification Verification request event for 95 seconds
my @res = eventQXDM(EVENT_NOTIFY_REQ, 95000);

# Fill in the client_type from the notification request
#$req_notifyverify->{"client_id"}  = $res[0];
#printf("client_type = %d\n",  $res[0]);

#if ($res[4] == 7)
#{
#  printf("Recvd Notification verification request \n");
#  if ($res[5] == 0) {
#    printf("Recvd No Notify No Verify request. \n");
#  } elsif ($res[5] == 1) {
#    printf("Rcvd User Notify only request. \n");
#  } elsif ($res[5] == 2) {
#    printf("Rcvd User Notify Allowed if No resp request. \n");
#  } elsif ($res[5] == 3) {
#    printf("Rcvd User Notify Not Allowed if No Resp request. \n");
#  } elsif ($res[5] == 4) {
#    printf("Rcvd Privcay Reqd request. \n");
#  } elsif ($res[5] == 5) {
#    printf("Rcvd Privacy override request. \n");
#  }

  printf ("Sending Response type = %d to PDAPI\n", $req_notifyverify->{"accept_deny"});

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  my $rsp_notifyverify_pkt = $qxdm_app->SendDmIcdPacketEx($req_notifyverify_pkt, $timeout);

  my $rsp_notifyverify_len = length($req_notifyverify_pkt);
  printf ("DM CGPS_NTFY_VRFY_SND_USR_ACTN_CMD Response Packet Length: %d\n", $rsp_notifyverify_len);

  $req_notifyverify->ParseData($rsp_notifyverify_pkt);

  $status    = $req_notifyverify->GetStatus();
  $client_id = $req_notifyverify->GetClientId();
  printf ("Status = %d, Client Id = %d\n\n", $status, $client_id);
#} 

##### WHAT IS GOING ON HERE?!? #####

# BLAH BEGIN
#my $req_apicmd_pkt = $req_apicmd->SetCaptureCmd();
#printf("Sending DeRegiter UI command\n");
#my $rsp_apicmd_pkt_var = $qxdm_app->SendDmIcdPacketEx( $req_apicmd_pkt, $timeout);
#my $rsp_apicmd_len_var = length($rsp_apicmd_pkt_var);
#printf( "\nDM Response length: %d\n ", $rsp_apicmd_len_var );
#$req_apicmd->ParseData($rsp_apicmd_pkt_var);
#my $status = $req_apicmd->GetStatus();
#my $client_id = $req_apicmd->GetClientId();
# BLAH END

  $qxdm_app->SendScript("logging off");

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

NotifyVerifySendUserActionCmd

Syntax:   pd_NotifyVerify [options]

Options:  -help               Display this help message.

Example:  perl pd_NotifyVerify.pl <COM Port>

END

  print "$usage";
  exit (0);
}
