##########################################################################
# $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/pdapi_scripts/pd_AppTrack_xspi.pl#1 $
# $DateTime: 2016/12/13 07:59:45 $
# $Author: mplcsds1 $
#
# This example demonstrates using a DM command to 
# - initialize, register, & activate a PDAPI client using CGPS_INIT_CMD, 
# CGPS_REGISTER_PD_CMD, and CGPS_ACTIVATE_CMD respectively; 
# - initiate an application based tracking session using CGPS_START_CMD; 
# - Deactivate, Deregister, & Release a PDAPI client using CGPS_DEACTIVATE_CMD, 
# CGPS_REGISTER_PD_CMD, and CGPS_RELEASE_CMD respectively.
# - inject XSPI data when a fix starts
#
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
# Usage: perl pd_AppTrack_xspi.pl 11 8 4 2 1 99 (11 is the COM Port, 8 is the client
# Type (TEST2), 4 is the number of fixes, & 2 is time between fixes, 1 = spi_mode 99 = confidence )
#
##########################################################################

use vars qw(%opts);

use Win32::OLE;
use Win32::OLE::Variant;

use Getopt::Long;
use pd_StartDiag;
use pd_GetPos;
use pd_EndDiag;
use pd_InjectSpi;

use File::Basename;
use strict; 
use warnings;

###### prototypes ##########
sub check_client_type($);

###### parameters & globals #######
my $PROGNAME = basename($0);
my $COM_PORT = "com-port";
my $CLIENT_TYPE = "client-type";
my $NUMBER_OF_FIXES = "sessions";
my $TBF = "tbf";
my $SPI_MODE = "spi-mode";
my $SPI_CONFIDENCE = "spi-confidence";

my %opts = ();
###### begin "main" ##########
unless(GetOptions (\%opts, 'help', 
			  "$COM_PORT=i", 
			  "$CLIENT_TYPE=i", 
			  "$NUMBER_OF_FIXES=i", 
			  "$TBF=i", 
			  "$SPI_MODE=i",
                          "$SPI_CONFIDENCE=i"))
{
   $opts{'help'} = 1;
}

&usage() if ($opts{'help'});

foreach(sort(keys(%opts)))
{
   print("\t$_ => [$opts{$_}]\n");
}

if(@ARGV)
{
   print("\n following args left in ARGV\n");
   foreach(@ARGV)
   {
      print("\t[$_]\n");
   }
}

my $spi; 
my $confidence; 
my $comport;
my $client_type;
my $sessions;
my $tbf;
my $count;


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

if(exists $opts{$NUMBER_OF_FIXES})
{
   $sessions = $opts{$NUMBER_OF_FIXES};
}
else
{
   $sessions    = $ARGV[2];
}

if(exists $opts{$TBF})
{
   $tbf = $opts{$TBF};
}
else
{
   $tbf         = $ARGV[3];
}

$count       = 0;

if(exists $opts{$SPI_MODE})
{
   $spi = $opts{$SPI_MODE};
}
else
{
   $spi         = $ARGV[4];
}

if(exists $opts{$SPI_CONFIDENCE})
{
   $confidence = $opts{$SPI_CONFIDENCE};
}
else
{
   $confidence  = $ARGV[5];
}

# Allow user input for COM port
if (not defined ($comport))
{
  # print ("\nUsage: perl pd_AppTrack.pl <COM Port> <Client Type> <Number of Sessions> <TBF>\n\n");
  print ("Please enter the COM port the FFA is connected to\n");
  chomp ($comport = <STDIN>);
  print ("\n");
}

# Allow user input for Client Type
if (not defined ($client_type))
{
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
  $client_type = check_client_type($client_type);
  print("\n");
}

printf("using client_type = %d\n", $client_type);

# Allow user input for Number of Sessions
if (not defined ($sessions))
{
  $sessions = 0;
  if ($count == 0)
  {
    print("\nUsage: perl pd_AppTrack.pl <COM Port> <Client Type> <Number of Sessions> <TBF>\n\n");
    $count = 1;
  }

  while($sessions <= 0)
  {
     # Prompt user to enter Number of Sessions
     print ("\nPlease enter the Number of Sessions to run (any integer greater than 0)\n");
     chomp ($sessions = <STDIN>);
  }
  print ("\n");
}

# Allow user input for TBF
if (not defined ($tbf))
{
  $tbf = 0;
  if ($count == 0)
  {
    print("\nUsage: perl pd_AppTrack.pl <COM Port> <Client Type> <Number of Sessions> <TBF>\n\n");
    $count = 1;
  }

  while($tbf < 1)
  {
     # Prompt user to enter TBF
     print ("\nPlease enter TBF (any integer greater than 0)\n");
     chomp ($tbf = <STDIN>);
  }
  print("\n");
}

#Allow user input for spi
# 
if( not defined ($spi)) {
   print("\n spi not defined. Please specify one of the following values\n");
   while(1) {
      print("\n0 = unknown");
      print("\n1 = stationary");
      print("\n2 = non-stationary\n");

      chomp($spi = <STDIN>);
      if($spi == 0){
         print("setting spi unknown");
         last;
      }
      if($spi == 1){
         print("setting spi stationary");
         last;
      }
      if($spi == 2){
         print("setting spi non-stationary");
         last;
      }
   }
   
}

#Allow user input for confidence
# 
if( not defined ($confidence)) 
{
   print("\n confidence not defined");
   $confidence = -1;
   while(($confidence < 0) || ($confidence > 100)) 
   {
      printf("Please specify an integer between 0 - 100 inclusive\n");
      chomp($confidence = <STDIN>);
   }
   printf("setting confidence = %d\n", $confidence);
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
  sleep (2);

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
  my $PrevIndex = $qxdm_app2->GetClientItemCount( $ReqHandle ) - 1;   

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  my $start = time();
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
  my $CanStartNextSession = 0;
  my $GPS_PD_SESS_START_RECEIVED = 0;
  my $GPS_PD_FIX_START_RECEIVED = 0;

  my $sess_start_time = time();
  my $sess_start_timeout = 10;
  my $sess_end_timeout =  $req_getpos->{"qos_gps_sess_tout"} + 15;
  my $current_time;

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
    my $CurrIndex = $qxdm_app2->GetClientItemCount( $ReqHandle ) - 1; # Get index of last item in client

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

              ### inject spi 
              my $req_xspi_injection = new pd_InjectSpi;
              $req_xspi_injection->{"cmd_code"}        = 75;  # Diag Cmd Code
              $req_xspi_injection->{"subsys_id"}       = 13;  # Diag Subsystem ID
              $req_xspi_injection->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
              $req_xspi_injection->{"cgps_cmd_code"}   = 201; # CGPS Cmd Code = CGPS_INJECT_EXTERNAL_SPI
              $req_xspi_injection->{"version"}         = 1;   # Version Number
              $req_xspi_injection->{"client_id"}       = $client_id;   # test client id
              $req_xspi_injection->{"mode"}            = $spi;   # spi mode
              $req_xspi_injection->{"confidence"}      = $confidence;   # confidence
              $req_xspi_injection->{"reserved_0"}        = 0;   # not used
              $req_xspi_injection->{"reserved_1"}       = 0;

              printf("\n sending xspi injection packet with following settings ");
              printf("\ncmd_code=%d\nsubsys_id=%d\nsubsys_cmd_code=%d\ncgps_cmd_code=%d\nversion=%d\nclient_id=%d\nmode=%d\nconfidence=%d\nreserved_0=%d\nreserved_1=%d\n",
                $req_xspi_injection->{"cmd_code"},
              $req_xspi_injection->{"subsys_id"},
              $req_xspi_injection->{"subsys_cmd_code"},
              $req_xspi_injection->{"cgps_cmd_code"},
              $req_xspi_injection->{"version"},
              $req_xspi_injection->{"client_id"},
              $req_xspi_injection->{"mode"},
              $req_xspi_injection->{"confidence"},
              $req_xspi_injection->{"reserved_0"} ,
              $req_xspi_injection->{"reserved_1"}  ,
              );
              ######################################################################
              # Construct the DM Query_Version capture request packet
              ######################################################################

              my $req_xspi_injection_pkt = $req_xspi_injection->SetCaptureCmd();

              ######################################################################
              # Send command request packet, receive command response packet
              ######################################################################

              my $rsp_xspi_inject_pkt = $qxdm_app->SendDmIcdPacketEx($req_xspi_injection_pkt, $timeout);

              my $rsp_xspi_inject_len = length($rsp_xspi_inject_pkt);
              #printf ("DM inject SPI Response Packet Length: %d\n", $rsp_version_len);

              $req_xspi_injection->ParseData($rsp_xspi_inject_pkt);

              $status    = $req_xspi_injection->GetStatus();
              printf ("Status = %d\n\n", $status);

              ### done inject spi

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

  my $end = time();

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
sub check_client_type($)
{
  my $client_type = shift;
  while (($client_type < 0) || ($client_type > 9))
  {
    print<<EOF_CLIENT_TYPE;
\nClient Type out of range. Please enter a value between 0-9
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
EOF_CLIENT_TYPE
    chomp ($client_type = <STDIN>);
  }
  printf("using client type [%d] \n", $client_type);
  return $client_type;
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

ApplicationTrackingCmd

Syntax:   $PROGNAME [options]

Options:  
  When no options are specified, options have positional significance, as specified below
  $PROGNAME <COM Port> <Client Type> <Number of Sessions> <TBF> <spi_mode> <spi_confidence>
  i.e.
   ARGV[0] = COM PORT
   ARGV[1] = CLIENT TYPE
   ARGV[2] = number of fixes
   ARGV[3] = time between fixes
   ARGV[4] = spi_mode
   ARGV[5] = confidence 

Following named options may also be specified on the command line. DO NOT mix named
and positional arguments

--help               
   Display this help message.

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

--$NUMBER_OF_FIXES <nubmer of sessions>

--$TBF <time betwee fixes in seconds>
   min of 1 second

--$SPI_MODE <mode>
   0 - unknown
   1 - stationary
   2 - non-stationary

--$SPI_CONFIDENCE <confidence>
   between 0 - 100 


Example:
  perl $PROGNAME --com-port 23 --client-type 8 --sessions 100 --tbf 1 --spi-mode 1 --spi-confidence 99  

   perl $PROGNAME 23 8 100 1 1 99
   where 
   23    = com-port the phone is connected to 
   8     = client-type 
   100   = sessions 
   1     = tbf 
   1     = spi-mode (stationary)
   99    = spi-confidence 

END

  print "$usage";
  exit (0);
}
