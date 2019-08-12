##########################################################################
# $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/pdapi_scripts/pd_AppTrack.pl#1 $
# $DateTime: 2016/12/13 07:59:45 $
# $Author: mplcsds1 $
#
# This example demonstrates using a DM command to initialize, register,
# & activate a PDAPI client using CGPS_INIT_CMD, CGPS_REGISTER_PD_CMD, and
# CGPS_ACTIVATE_CMD respectively; initiate an application based tracking
# session using CGPS_START_CMD; Deactivate, Deregister, & Release a PDAPI
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
# Usage: perl pd_AppTrack.pl 11 8 4 2 (11 is the COM Port, 8 is the client
# Type (TEST2), 4 is the number of fixes, & 2 is time between fixes)
#
##########################################################################

use vars qw(%opts);

use Win32::OLE;
use Win32::OLE::Variant;

use Getopt::Long;
use pd_StartDiag;
use pd_GetPos;
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

# Register the QXDM client
$ReqHandle = $qxdm_app2->RegisterQueueClient(256);
if ($ReqHandle == 0xFFFFFFFF)
{
  print "Unable to register as QXDM client\n";
  sleep(5);
  die;
}

# Get a configuration object
my $clientObject = $qxdm_app2->ConfigureClientByKeys($ReqHandle);
if ($clientObject == 0)
{
  print "Unable to register as QXDM client\n";
  sleep(5);
  die;
}

######################################################################
# Register for Required Events
######################################################################

$clientObject->AddEvent(605);    # EVENT_GPS_PD_SESS_START
$clientObject->AddEvent(601);    # EVENT_GPS_PD_FIX_START
$clientObject->AddEvent(614);    # EVENT_GPS_PD_POSITION
$clientObject->AddEvent(602);    # EVENT_GPS_PD_FIX_END
$clientObject->AddEvent(411);    # EVENT_GPS_PD_SESS_END

$clientObject->CommitConfig();

######################################################################
# Check for user input and prompt if arguments are not provided
######################################################################

$comport     = $ARGV[0];
$client_type = $ARGV[1];
$sessions    = $ARGV[2];
$tbf         = $ARGV[3];
$count       = 0;

# Allow user input for COM port
if (not defined ($comport))
{
  print ("\nUsage: perl pd_AppTrack.pl <COM Port> <Client Type> <Number of Sessions> <TBF>\n\n");
  print ("Please enter the COM port the FFA is connected to\n");
  chomp ($comport = <STDIN>);
  print ("\n");
  $count = 1;
}

# Allow user input for Client Type
if (not defined ($client_type))
{
  if ($count == 0)
  {
    print("\nUsage: perl pd_AppTrack.pl <COM Port> <Client Type> <Number of Sessions> <TBF>\n\n");
    $count = 1;
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

# Allow user input for Number of Sessions
if (not defined ($sessions))
{
  if ($count == 0)
  {
    print("\nUsage: perl pd_AppTrack.pl <COM Port> <Client Type> <Number of Sessions> <TBF>\n\n");
    $count = 1;
  }

  # Prompt user to enter Number of Sessions
  print ("Please enter the Number of Sessions to run (any integer value other than 0)\n");
  chomp ($sessions = <STDIN>);
  print ("\n");
}

# Allow user input for TBF
if (not defined ($tbf))
{
  if ($count == 0)
  {
    print("\nUsage: perl pd_AppTrack.pl <COM Port> <Client Type> <Number of Sessions> <TBF>\n\n");
    $count = 1;
  }

  # Prompt user to enter TBF
  print ("Please enter TBF (any integer value)\n");
  chomp ($tbf = <STDIN>);
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
  print "Unable to connect to the phone. Retrying...\n";
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
print("Initialize, Register, & Activate the Client\n");
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
  sleep (2);

######################################################################
# Build a diagnostic request for CGPS_START_CMD to do a Standalone fix
######################################################################

  $req_getpos = new pd_GetPos;
  $req_getpos->{"cmd_code"}          = 75;            # Diag Cmd Code
  $req_getpos->{"subsys_id"}         = 13;            # Diag Subsystem ID
  $req_getpos->{"subsys_cmd_code"}   = 100;           # Diag Subsystem Cmd Code
  $req_getpos->{"cgps_cmd_code"}     = 10;            # CGPS Cmd Code
  $req_getpos->{"version"}           = 1;             # Version Number
  $req_getpos->{"session_type"}      = 1;             # 0 = latest position
                                                      # 1 = new position
  $req_getpos->{"operating_type"}    = 1;             # 1 = Standalone
                                                      # 2 = MS-BASED
                                                      # 3 = MS-ASSISTED
                                                      # 4 = Optimal Speed
                                                      # 5 = Optimal Accuracy
                                                      # 6 = Optimal Data
                                                      # 7 = Reference Position
  $req_getpos->{"server_option"}     = 0;             # 0 = NV Default
                                                      # 1 = Custom Local
  $req_getpos->{"server_addr_type"}  = 0;             # 0 = IpV4
                                                      # 1 = IpV6
  $req_getpos->{"class_id"}          = 0;             # not used
  $req_getpos->{"qos_gps_sess_tout"} = 60;            # QoS timeout value between 0-255 seconds
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
# Set up Application Tracking loop for the number of fixes required
######################################################################

my $i = 0;
my $res;

print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
print("Starting App Tracking Session\n");
print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

for ($i = 0; $i < $sessions; $i++)
{
  printf("\n********** Starting Session %d of %d **********\n\n", $i+1, $sessions);

  ######################################################################
  # Construct the DM CGPS_START_CMD capture request packet
  ######################################################################

  #sleep 2;
  my $req_getpos_pkt = $req_getpos->SetCaptureCmd();

  $PrevIndex = $qxdm_app2->GetClientItemCount($ReqHandle) - 1;

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  $start = time();
  my $rsp_getpos_pkt = $qxdm_app->SendDmIcdPacketEx($req_getpos_pkt, $timeout);

  my $rsp_getpos_len = length($rsp_getpos_pkt);
  printf ("\nDM CGPS_START_CMD Response Packet Length = %d\n", $rsp_getpos_len);

  $req_getpos->ParseData($rsp_getpos_pkt);

  $status    = $req_getpos->GetStatus();
  $client_id = $req_getpos->GetClientId();
  printf ("Status = %d, Client Id = %d\n\n", $status, $client_id);

  ###############################################################################
  # Waiting for SESSION START, FIX START, POSITION, FIX END, & SESSION END Events
  ###############################################################################

  # Process Client Events
  $CanStartNextSession = 0;
  $GPS_PD_SESS_START_RECEIVED = 0;
  $GPS_PD_FIX_START_RECEIVED = 0;

  $sess_start_time = time();
  $sess_start_timeout = 10;
  $sess_end_timeout =  $req_getpos->{"qos_gps_sess_tout"} + 15;

  while ($CanStartNextSession == 0)
  {
    # Checking for GPS_PD_SESSION_START Event timeout
    if ($GPS_PD_SESS_START_RECEIVED == 0)
    {
      $current_time = time();
      if ($current_time - $sess_start_time > $sess_start_timeout)
      {
        printf ("Timeout: Event GPS_PD_SESSION_START not received\n\n");
        exit;
      }
    }

    # Checking for GPS_PD_SESSION_END Event timeout
    $current_time = time();
    if ($current_time - $sess_start_time > $sess_end_timeout)
    {
      printf ("Timeout: Event GPS_PD_SESSION_END not received\n\n");
      exit;
    }
    $CurrIndex = $qxdm_app2->GetClientItemCount($ReqHandle) - 1; # Get index of last item in client

    # Process all new items
    for (my $i2 = $PrevIndex + 1; $i2 <= $CurrIndex; $i2++)
    {
      my $Item = $qxdm_app2->GetClientItem($ReqHandle, $i2); # Get Item
      my $ItemKey = $Item->GetItemKeyText();
      if ($GPS_PD_SESS_START_RECEIVED == 0)
      {
        $current_time = time();
        if ($current_time - $sess_start_time > $sess_start_timeout)
        {
          printf ("Timeout: Event GPS_PD_SESS_START not received\n\n");
          exit;
        }
        else
        {
          if ($ItemKey eq "[00605]")
          {
            printf ("Received Event GPS_PD_SESS_START!!\n\n");
            $GPS_PD_SESS_START_RECEIVED = 1;
          }
        }
      }
      if ($GPS_PD_SESS_START_RECEIVED == 1)
      {
        if ($ItemKey eq "[00601]")
        {
          printf ("Received Event GPS_PD_FIX_START!!\n\n");
   	      $GPS_PD_FIX_START_RECEIVED = 1;
   	    }
      }
      if ($GPS_PD_SESS_START_RECEIVED == 1 || GPS_PD_FIX_START_RECEIVED == 1)
      {
        $current_time = time();
        if ($current_time - $sess_start_time > $sess_end_timeout)
        {
          printf ("Timeout: Event GPS_PD_SESSION_END not received\n\n");
          exit;
        }
        else
        {
          if ($ItemKey eq "[00614]")
          {
            printf ("Received Event GPS_PD_POSITION!!\n\n");
          }
          if ($ItemKey eq "[00602]")
          {
            printf ("Received Event GPS_PD_FIX_END!!\n\n");
          }
          if ($ItemKey eq "[00411]")
          {
            printf ("Received Event GPS_PD_SESSION_END!!\n\n");
            $CanStartNextSession = 1;
          }
        }
      }
      else
      {
        if ($ItemKey eq "[00614]" or "[00602]" or "[00411]")
        {
          printf ("Events received out of order\n");
          exit;
        }
      }
    }
    $PrevIndex = $CurrIndex; # Keep processing all new items while Session is not done.
  }

  $end = time();

  ######################################################################
  # If the fix was obtained before the time between fixes specified
  # ($time_bw_fixes), wait until the next position fix is ready to
  # send. If the fix was obtained after the next fix was due, send
  # the subsequent request out immediately.
  ######################################################################

  if (($end - $start) < $tbf)
  {
    if ($i < ($sessions - 1))
    {
      sleep($tbf - ($end - $start));
    }
  }
  $req_getpos->{"num_fixes"} = $sessions - $i - 1;
}
sleep(2);

print("\n");
print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
print("Deactivate, Deregister, & Release the Client\n");
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
  $req_apicmd1->{"client_id"}       = $client_id; # Client Type

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

ApplicationTrackingCmd

Syntax:   pd_AppTrack [options]

Options:  -help               Display this help message.

Example:  perl pd_AppTrack.pl <COM Port> <Client Type> <Number of Sessions> <TBF>

END

  print "$usage";
  exit (0);
}
