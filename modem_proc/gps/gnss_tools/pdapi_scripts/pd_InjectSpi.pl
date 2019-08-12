##########################################################################
# $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/pdapi_scripts/pd_InjectSpi.pl#1 $
# $DateTime: 2016/12/13 07:59:45 $
# $Author: mplcsds1 $
#
# This example demonstrates using a DM Command to inject external SPI 
# information to the GNSS ENGINE
# 
# Before calling this script, a PDAPI client will need to be
# initialized (created). Also this client will then need to register
# for PD events and activate itself before it can query for the revision.
# (Run pd_StartDiag.pl)
#
# Usage: perl pd_InjectSpi.pl 11 1 34 (11 is the com port the FFA is connected to, 
#                                      1 = spi mode, possible values are 0 = uknown 1 = stationary, 2 = non-stationary,
#                                      34 = confidence )
#
##########################################################################

use vars qw(%opts);

use Win32::OLE;
use Win32::OLE::Variant;
use IPC::Open2;

use Getopt::Long;
use pd_InjectSpi;

use File::Basename;
use strict;
use warnings;
use pd_StartDiag;
use pd_EndDiag;

###### prototypes ##########
sub check_client_type($);

###### parameters & globals #######
my $PROGNAME = basename($0);
my $COM_PORT = "com-port";
my $CLIENT_TYPE = "client-type";
my $SPI_MODE = "spi-mode";
my $SPI_CONFIDENCE = "spi-confidence";

my %opts = ();
###### begin "main" ##########
unless(GetOptions (\%opts, 'help', 
			  "$COM_PORT=i", 
			  "$CLIENT_TYPE=i", 
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

#####################################################################
# Prompt for user input if arguments are not provided
#####################################################################

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

$count       = 0;

if(exists $opts{$SPI_MODE})
{
   $spi = $opts{$SPI_MODE};
}
else
{
   $spi         = $ARGV[2];
}

if(exists $opts{$SPI_CONFIDENCE})
{
   $confidence = $opts{$SPI_CONFIDENCE};
}
else
{
   $confidence  = $ARGV[3];
}

# Allow user input for COM port
if (not defined ($comport))
{
  # print ("\nUsage: perl pd_AppTrack.pl <COM Port> <Client Type> <Number of Sessions> <TBF>\n\n");
    print ("Please enter the COM port the FFA is connected to\n");
    chomp ($comport = <STDIN>);
    print("\n");
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
  print("injecting SPI \n");
  print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

######################################################################
# Build a diagnostic request for injecting SPI
######################################################################

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

InjectSpiCmd

Syntax:   $PROGNAME [options]

Options:  
  When no options are specified, options have positional significance, as specified below
  $PROGNAME <COM Port> <Client Type> <spi_mode> <spi_confidence>
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

--$SPI_MODE <mode>
   0 - unknown
   1 - stationary
   2 - non-stationary

--$SPI_CONFIDENCE <confidence>
   between 0 - 100 


Example:  
   perl $PROGNAME --com-port 23 --client-type 8 --spi-mode 1 --spi-confidence 99  
   
   perl $PROGNAME 11 8 1 99
   where 
      11 is the com port the FFA is connected to, 
      8 is client type
      1 = stationary
      99 = 99% confidence


END

  print "$usage";
  exit (0);
}
