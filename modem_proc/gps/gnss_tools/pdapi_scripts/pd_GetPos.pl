###########################################################################
# $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/pdapi_scripts/pd_GetPos.pl#1 $
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
# Values that need to be specified for pdsm_get_pos()
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

use File::Basename;
use Getopt::Long;

use pd_StartDiag;
use pd_Getpos;
use pd_EndDiag;

use strict;
use warnings;

###### prototypes ##########
sub check_client_type();
sub check_operating_type(\%\%);

###### parameters & globals #######
my $PROGNAME = basename($0);
my $COM_PORT = "com-port";
my $CLIENT_TYPE = "client-type";
my $OPERATING_TYPE ="operating-type";
my $SERVER_IPV4_ADRS = "server-ipv4-adrs";
my $SERVER_IPV4_PORT = "server-ipv4-port";

# !!Change to the local Custom Server Address!!
my $DEFAULT_IPV4_ADDR = "10.227.208.139"; # SD117 KOALA
my $DEFAULT_IPV4_PORT = 4911;

my ($OPERATING_TYPE_STANDALONE, 
    $OPERATING_TYPE_MSBASED,
    $OPERATING_TYPE_MSASSISTED,
    $OPERATING_TYPE_OPTIMAL_SPEED,
    $OPERATING_TYPE_OPTIMAL_ACCURACY,
    $OPERATING_TYPE_OPTIMAL_DATA,
    $OPERATING_TYPE_REFERENCE_POSITION) = (1..7);

my $DEFAULT_OPERATING_TYPE = $OPERATING_TYPE_STANDALONE; # 1 = Standalone
                                # 2 = MS-BASED
                                # 3 = MS-ASSISTED
                                # 4 = Optimal Speed
                                # 5 = Optimal Accuracy
                                # 6 = Optimal Data
                                # 7 = Reference Position

my %operating_type_names = ($OPERATING_TYPE_STANDALONE=> "OPERATING_TYPE_STANDALONE",
    $OPERATING_TYPE_MSBASED=>"OPERATING_TYPE_MSBASED",
    $OPERATING_TYPE_MSASSISTED=>"OPERATING_TYPE_MSASSISTED",
    $OPERATING_TYPE_OPTIMAL_SPEED=>"OPERATING_TYPE_OPTIMAL_SPEED",
    $OPERATING_TYPE_OPTIMAL_ACCURACY=>"OPERATING_TYPE_OPTIMAL_ACCURACY",
    $OPERATING_TYPE_OPTIMAL_DATA=>"OPERATING_TYPE_OPTIMAL_DATA",
    $OPERATING_TYPE_REFERENCE_POSITION=>"OPERATING_TYPE_REFERENCE_POSITION");

my %opts = ();

###### begin "main" ##########
unless(GetOptions (\%opts, 'help', 
			  "$COM_PORT=i", 
			  "$CLIENT_TYPE=i", 
			  "$OPERATING_TYPE=i", 
			  "$SERVER_IPV4_ADRS=s", 
			  "$SERVER_IPV4_PORT=i"))
{
	 $opts{'help'} = 1;
}

&usage() if ($opts{'help'});

unless(exists $opts{$SERVER_IPV4_ADRS})
{
	 $opts{$SERVER_IPV4_ADRS}  = $DEFAULT_IPV4_ADDR; 
}

unless(exists $opts{$SERVER_IPV4_PORT})
{
	 $opts{$SERVER_IPV4_PORT} = $DEFAULT_IPV4_PORT;
}

unless(exists $opts{$OPERATING_TYPE})
{
	$opts{$OPERATING_TYPE} = $DEFAULT_OPERATING_TYPE;
}

# check any user specified options here, if at fault, raise help flag 
check_operating_type(%opts, %operating_type_names);

# check_server_ip
# check_server_ip_port

&usage() if ($opts{'help'});

printf("\n using following options \n");

foreach(sort(keys(%opts)))
{
	print("\t$_ => [$opts{$_}]\n");
}

print("\n following args left in ARGV\n");
foreach(@ARGV)
{
  print("\t[$_]\n");
}
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

my $qxdm_app2 = $qxdm_app->GetIQXDM2();
if ($qxdm_app2 == 0)
{
   print "QXDM does not support required interface\n";
   sleep(5);
   die;
}

# Register the QXDM client.
my $ReqHandle = $qxdm_app2->RegisterQueueClient( 256 );
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
my $comport;
my $client_type;

if(exists $opts{$COM_PORT})
{
	$comport = $opts{$COM_PORT};
}
else
{
	$comport     = $ARGV[0];
}

if(exists $opts{$CLIENT_TYPE})
{
	 $client_type = $opts{$CLIENT_TYPE};
}
else
{
	$client_type = $ARGV[1];
}

my $count       = 0;
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
        $count = 1;
    }
    # Prompt user to enter Client Type
    print<<EOF_CLIENT_TYPE_PROMPT;
Please enter the Client Type
    0 - BROWSER         
    1 - UI                 
    2 - PDA            
    3 - DM                
    4 - NI        
    5 - BREW            
    6 - JAVA            
    7 - TEST           
    8 - TEST2
    9 - OEM\n
EOF_CLIENT_TYPE_PROMPT
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
# Build a diagnostic request for CGPS_INIT_CMD to initialize the client
#######################################################################

my $req_apicmd1 = new pd_StartDiag;
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
  printf ("Sending CGPS_INIT_CMD: Initialize client type %d\n", $client_type);

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  my $rsp_apicmd_pkt = $qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, $timeout);

  my $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf ("DM CGPS_INIT_CMD Response Packet Length: %d\n", $rsp_apicmd_len);

  $req_apicmd1->ParseData($rsp_apicmd_pkt);

  my $status    = $req_apicmd1->GetStatus();
  my $client_id = $req_apicmd1->GetClientId();
  printf ("Status = %d, Client Id = %d\n\n", $status, $client_id);

  sleep(2);

############################################################################
# Build a diagnostic request for CGPS_REGISTER_PD_CMD to register the client
# Registers the client to receive registered PD events
############################################################################

  my $req_apicmd2 = new pd_StartDiag;
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

  $req_apicmd_pkt = $req_apicmd2->SetCaptureCmd();
  printf ("Sending CGPS_REGISTER_PD_CMD: Register client ID %d\n", $client_id);

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  $rsp_apicmd_pkt = $qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, $timeout);

  $rsp_apicmd_len = length($rsp_apicmd_pkt);
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

  my $req_apicmd3 = new pd_StartDiag;
  $req_apicmd3->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_apicmd3->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_apicmd3->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_apicmd3->{"cgps_cmd_code"}   = 7;   # CGPS Cmd Code
  $req_apicmd3->{"version"}         = 1;   # Version Number
  $req_apicmd3->{"client_id"}       = $client_id;  # Client ID from Init

  ######################################################################
  # Construct the DM CGPS_ACTIVATE_CMD capture request packet
  ######################################################################

  $req_apicmd_pkt = $req_apicmd3->SetCaptureCmd();
  printf ("Sending CGPS_ACTIVATE_CMD: Activate client ID %d\n", $client_id);

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  $rsp_apicmd_pkt = $qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, $timeout);

  $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf ("DM CGPS_ACTIVATE_CMD Response Packet Length: %d\n", $rsp_apicmd_len);

  $req_apicmd3->ParseData($rsp_apicmd_pkt);

  $status    = $req_apicmd3->GetStatus();
  $client_id = $req_apicmd3->GetClientId();
  printf ("Status = %d, Client Id = %d\n\n", $status, $client_id);

######################################################################
# Get Position Cmd parameters used for Apptracking only
######################################################################

  my $sessions  = 1; # not used (tbf only used for app tracking)
  my $tbf       = 1; # not used (num > 1 fix -> used for app tracking)
  sleep(2);

  print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  print("Apply get_pos to Start A-GPS Session\n");
  print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

######################################################################
# Build a diagnostic request for CGPS_START_CMD to do a Standalone fix
######################################################################

  my $req_getpos = new pd_GetPos;
  $req_getpos->{"cmd_code"}          = 75;            # Diag Cmd Code
  $req_getpos->{"subsys_id"}         = 13;            # Diag Subsystem ID
  $req_getpos->{"subsys_cmd_code"}   = 100;           # Diag Subsystem Cmd Code
  $req_getpos->{"cgps_cmd_code"}     = 10;            # CGPS Cmd Code
  $req_getpos->{"version"}           = 1;             # Version Number
  $req_getpos->{"session_type"}      = 1;             # 0 = latest position
                                                      # 1 = new position
  $req_getpos->{"operating_type"} = $opts{$OPERATING_TYPE};

  $req_getpos->{"server_option"}     = 1;             # 0 = NV Default
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
  $req_getpos->{"client_id"}         = $client_id;    # Returned by PDAPI during client registration
  $req_getpos->{"reserved"}          = 0;             # not used
  $req_getpos->{"server_ipv4_adrs"} = $opts{$SERVER_IPV4_ADRS};
  $req_getpos->{"server_ipv4_port"} = $opts{$SERVER_IPV4_PORT};
  
  printf("\n using following getpos\n");

  foreach(sort(keys(%${req_getpos})))
  {
	print("\treq_getpos->{$_} => [$req_getpos->{$_}]\n");
      if($_ eq "operating_type"){
		printf("\t\t operating_type [%d] = [%s]\n", $req_getpos->{$_}, $operating_type_names{$req_getpos->{$_}});
      }
  }


  ######################################################################
  # Construct the DM CGPS_START_CMD capture request packet
  ######################################################################

  my $req_getpos_pkt = $req_getpos->SetCaptureCmd();
  my $PrevIndex = $qxdm_app2->GetClientItemCount( $ReqHandle ) - 1;   

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

  ################################################################################
  # Waiting for SESSION START, FIX START, POSITION, FIX END, & SESSION END Events
  ################################################################################

  # Process Client Events
  my $CanStartNextSession = 0;
  my $GPS_PD_SESS_START_RECEIVED = 0;
  my $GPS_PD_FIX_START_RECEIVED = 0;
 
  my $sess_start_time = time();
  my $sess_start_timeout = 10;
  my $sess_end_timeout =  $req_getpos->{"qos_gps_sess_tout"} + 15;
  my $current_time;

  while ($CanStartNextSession == 0)
  {
    # Checking for GPS_PD_SESSION_START Event timeout.
    if ($GPS_PD_SESS_START_RECEIVED == 0)
    {
        $current_time = time();
        if ($current_time - $sess_start_time > $sess_start_timeout)
        {
          printf ("Timeout: Event GPS_PD_SESSION_START not received\n\n");
          exit;
        }
    }
    # Checking for GPS_PD_SESSION_END Event timeout.
    $current_time = time();
    if ($current_time - $sess_start_time > $sess_end_timeout)
    {
	printf ("Timeout: Event GPS_PD_SESSION_END not received\n\n");
	exit;
    }
    my $CurrIndex = $qxdm_app2->GetClientItemCount( $ReqHandle ) - 1; # Get index of last item in client
    
    # Process all new items
    for ( my $i2 = $PrevIndex + 1; $i2 <= $CurrIndex; $i2++ )
    {
	my $Item = $qxdm_app2->GetClientItem($ReqHandle, $i2 ); # Get Item
      my $ItemKey = $Item->GetItemKeyText();
	if ($GPS_PD_SESS_START_RECEIVED == 0)
	{  
	  $current_time = time();
	  if ($current_time - $sess_start_time > $sess_start_timeout)
	  {
	    printf ("Time out: Event GPS_PD_SESS_START not received.!!\n");
	    exit;
 	  }
	  else
	  {
	    if ($ItemKey eq "[00605]")
	    {
		printf ("Received event GPS_PD_SESS_START!!\n\n");
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
	if ($GPS_PD_SESS_START_RECEIVED == 1 || $GPS_PD_FIX_START_RECEIVED == 1)
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

  print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
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
  $req_apicmd1->{"client_id"}       = $client_id; # Client Type

  ######################################################################
  # Construct the DM CGPS_DEACTIVATE_CMD capture request packet
  ######################################################################

  $req_apicmd_pkt = $req_apicmd1->SetCaptureCmd();
  printf ("Sending CGPS_DEACTIVATE_CMD: Deactivate client type %d\n", $client_type);

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  $rsp_apicmd_pkt = $qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, $timeout);

  $rsp_apicmd_len = length($rsp_apicmd_pkt);
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

  $req_apicmd_pkt = $req_apicmd2->SetCaptureCmd();
  printf ("Sending CGPS_REGISTER_PD_CMD: DeRegister client ID %d\n", $client_id);

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  $rsp_apicmd_pkt = $qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, $timeout);

  $rsp_apicmd_len = length($rsp_apicmd_pkt);
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

  $req_apicmd_pkt = $req_apicmd3->SetCaptureCmd();
  printf ("Sending CGPS_RELEASE_CMD: Release client ID %d\n", $client_id);

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  $rsp_apicmd_pkt = $qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, $timeout);

  $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf ("DM CGPS_RELEASE_CMD Response Packet Length: %d\n", $rsp_apicmd_len);

  $req_apicmd3->ParseData($rsp_apicmd_pkt);

  $status    = $req_apicmd3->GetStatus();
  $client_id = $req_apicmd3->GetClientId();
  printf ("Status = %d, Client Id = %d\n", $status, $client_id);
  sleep(2);

  undef $qxdm_app;


######################################################################
# METHOD:
#   check_client_type
#
# DESCRIPTION:
#   determines if the client type is valid. if not, prompts for one
#
# RETURN VALUE:
#   None
#
######################################################################
sub check_client_type()
{
  while ((not defined $client_type) || ($client_type > 9))
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

Options:  
  When no options are specified, options have positional significance, as specified below

  pd_GetPos.pl <COM Port> <Client Type>
  
  i.e. ARGV[0] = COM PORT
       ARGV[1] = CLIENT TYPE

Following named options may also be specified on the command line. DO NOT mix named
and positional arguments

--$COM_PORT < COM port numer>
   this is the virtual COM port the phone is connected to.
   See QPST configuration dialog box

--$CLIENT_TYPE <client type>
    0 - BROWSER         
    1 - UI                 
    2 - PDA            
    3 - DM                
    4 - NI        
    5 - BREW            
    6 - JAVA            
    7 - TEST           
    8 - TEST2
    9 - OEM

--$OPERATING_TYPE <operation>
    1 = Standalone
    2 = MS-BASED
    3 = MS-ASSISTED
    4 = Optimal Speed
    5 = Optimal Accuracy
    6 = Optimal Data
    7 = Reference Position

--$SERVER_IPV4_ADRS
   IP address of server in aa.bb.cc.dd dotted-decimal format

--$SERVER_IPV4_PORT
   port number of above server to connect to

--help               Display this help message.

Example:  
  perl $PROGNAME <COM Port> <Client Type>
  perl $PROGNAME --$CLIENT_TYPE 8 --$OPERATING_TYPE 1 --$COM_PORT 18

END

  print "$usage";
  exit (0);
}


######################################################################
# METHOD:
#   check_operating_type
#
# DESCRIPTION:
#   verify valid operating type has been given. if not, prompt for one
#
# RETURN VALUE:
#   None
#
######################################################################
sub check_operating_type(\%\%)
{
	my $opts_ref = shift;
	my $operating_type_names = shift;
	while(not exists $operating_type_names->{$opts_ref->{$OPERATING_TYPE}})
      {
		print("Please specify operating type as one of the following:\n");
		foreach(sort keys %${operating_type_names})
            {
			printf("\t%d - %s\n", $_, $operating_type_names->{$_});
            }
		my $input = <stdin>;
		chomp $input;
		$opts_ref->{$OPERATING_TYPE} = $input;
      }
}
