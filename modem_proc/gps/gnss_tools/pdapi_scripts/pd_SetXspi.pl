##########################################################################
# $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/pdapi_scripts/pd_SetXspi.pl#1 $
# $DateTime: 2016/12/13 07:59:45 $
# $Author: mplcsds1 $
#
# This example demonstrates using a DM Command to enable external SPI 
# information injection to the GNSS ENGINE
#
# Before calling this script, a PDAPI client will need to be
# initialized (created). Also this client will then need to register
# for PD events and activate itself before it can query for the revision.
# (Run pd_StartDiag.pl)
#
# Usage: perl pd_SetXspi.pl 11 8 (11 is the COM Port the FFA is connected
#        to & 8 is the Client Type)
#
##########################################################################

use vars qw(%opts);

use Win32::OLE;
use Win32::OLE::Variant;
use IPC::Open2;

use Getopt::Long;
use pd_SetXspi;
use strict;
use warnings;
use pd_StartDiag;
use pd_EndDiag;

sub check_client_type($);

my %opts = ();

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

my $comport = $ARGV[0];
my $client_type = $ARGV[1];
my $mode = $ARGV[2];

# Allow user input for comport
if (not defined ($comport)) {
    print("\nUsage: perl pd_SetXspi.pl <COM Port> <Client Type>\n\n");
    print ("Please enter the COM port the FFA is connected to\n");
    # $count = 1;
    chomp ($comport = <STDIN>);
    print("\n");
}

# check client type
if((not defined ($client_type)) || (0 > $client_type)){
   $client_type = -1;
   print("checking client type \n");
   $client_type = check_client_type($client_type);
}

print("using client_type = $client_type \n");

#Allow spi mode input
# 
if( not defined ($mode)) {
   print("\n spi mode not defined. Please specify one of the following values\n");
   while(1) {
      print("\n0 = disabled");
      print("\n1 = enabled");

      chomp($mode = <STDIN>);
      if($mode == 0){
         print("setting spi mode disabled");
         last;
      }
      if($mode == 1){
         print("setting spi mode enabled");
         last;
      }
   }
   
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

  print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  print("setting SPI mode\n");
  print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

######################################################################
# Build a diagnostic request for CGPS_SET_PARAM_CMD: SET XSPI MODE
######################################################################


  my $req_xspi_injection = new pd_SetXspi;
  $req_xspi_injection->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_xspi_injection->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_xspi_injection->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_xspi_injection->{"cgps_cmd_code"}   = 12;  # CGPS_SET_PARAM_CMD
  $req_xspi_injection->{"version"}         = 50;   # Version Number
  $req_xspi_injection->{"client_id"}       = $client_id;  # TEST2
  $req_xspi_injection->{"param_type"}      = 22;  # GPSDIAG_PA_SET_EXTERNAL_SPI
  $req_xspi_injection->{"param_length"}    = 9; # mode + 2 x 4 byte reserved fields
  $req_xspi_injection->{"mode"}   = $mode; #0 = disabled, 1 = enabled 
  $req_xspi_injection->{"reserved_0"}     = 0;  # not used
  $req_xspi_injection->{"reserved_1"}     = 0;  # not used



  ######################################################################
  # Construct the DM CGPS_SET_PARAM_CMD capture request packet
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

SetSpiCmd

Syntax:   perl pd_SetXspi.pl <com port> <SPI>
   <com port> = com port the phone is connected to
   <SPI>      = stationary position indication
              0 = disabled
              1 = enabled
Options:  -help               Display this help message.

Syntax:   pd_SetXspi [options]

Options:  -help               Display this help message.

Example:  perl pd_SetXspi.pl <COM Port> <Client Type>

END

  print "$usage";
  exit (0);
}

sub check_client_type($)
{
  my $client_type = shift;
  while (($client_type < 0) || ($client_type > 9))
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
  return $client_type;
}

