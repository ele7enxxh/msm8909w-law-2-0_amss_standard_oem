##########################################################################
# $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/pdapi_scripts/pd_DelAlm.pl#1 $
# $DateTime: 2016/12/13 07:59:45 $
# $Author: mplcsds1 $
#
# This example demonstrates using a DM Command to initialize, register,
# & activate a PDAPI client; deletes all but almanac by sending a
# CGPS_SET_PARAM_CMD; deactivate, deregister, & release a PDAPI client.
#
# Run pd_StartDiag.pl before calling this script to start a Diag Task.
#
# Usage: perl pd_DelAllButAlm.pl 11 8 (11 is the COM Port the FFA is connected
#        to & 8 is the Client Type)
#
##########################################################################

use vars qw(%opts);

use Win32::OLE;
use Win32::OLE::Variant;

use Getopt::Long;
use pd_DelAll;
use pd_StartDiag;
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
# Prompt for user input if arguments are not provided
#####################################################################

$comport = $ARGV[0];
$client_type = $ARGV[1];
check_client_type();

# Allow user input for comport
if (not defined ($comport)) {
    print("\nUsage: perl pd_DelAllButAlm.pl <COM Port> <Client Type>\n\n");
    print ("Please enter the COM port the FFA is connected to\n");
    $count = 1;
    chomp ($comport = <STDIN>);
    print("\n");
}
# Allow user input for client type
if (not defined ($client_type)) {
    if ($count == 0) {
        print("\nUsage: perl pd_DelAllButAlm.pl <COM Port> <Client Type>\n\n");
    }
    #Prompt user to enter Client Type
    print ("Please enter the Client type
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

  print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  print("Doing DeleteAllButAlm (coldstart) operation\n");
  print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

######################################################################
# Build a diagnostic request for CGPS_SET_PARAM_CMD: DELETE_PARAMS
######################################################################

  $req_coldstart = new pd_DelAll;
  $req_coldstart->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_coldstart->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_coldstart->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_coldstart->{"cgps_cmd_code"}   = 12;  # CGPS Cmd Code
  $req_coldstart->{"version"}         = 1;   # Version Number
  $req_coldstart->{"client_id"}       = $client_id;  # TEST2
  $req_coldstart->{"param_type"}      = 4;  # PA_DELETE_PARAMS
  $req_coldstart->{"param_length"}    = 64; # 16 x (4 byte fields)
  $req_coldstart->{"delete_bitmap"}   = 0x0002; # Delete all but Alm (1000|0111|1111|1101 = 34813)
                                            # DELETE_EPH         = 0x0001
                                            # DELETE_ALM         = 0x0002
                                            # DELETE_POS         = 0x0004
                                            # DELETE_TIME        = 0x0008
                                            # DELETE_IONO        = 0x0010
                                            # DELETE_UTC         = 0x0020
                                            # DELETE_HEALTH      = 0x0040
                                            # DELETE_SVDIR       = 0x0080
                                            # DELETE_SVSTEER     = 0x0100
                                            # DELETE_SADATA      = 0x0200
                                            # DELETE_RTI         = 0x0400
                                            # INJECT_TIME_UNC    = 0x0800
                                            # INJECT_POS_UNC     = 0x1000
                                            # INJECT_TIME_OFFSET = 0x2000
                                            # INJECT_POS_OFFSET  = 0x4000
                                            # DELETE_CELLDB_INFO = 0x8000

     $req_coldstart->{"gnss_delete_bitmap"} = 0x410080C;
                                            # DELETE_EPH_GLO
                                            # DELETE_EPH_SBAS
                                            # DELETE_ALM_GLO                =  U32BITMASK_BIT(2)
                                            # DELETE_ALM_SBAS               =  U32BITMASK_BIT(3)

                                            # DELETE_SVDIR_GLO
                                            # DELETE_SVDIR_SBAS
                                            # DELETE_SVSTEER_GLO
                                            # DELETE_SVSTEER_SBAS

                                            # DELETE_ALM_CORR_GLO
                                            # DELETE_CLOCK_INFO
                                            # DELETE_EPH_BDS
                                            # DELETE_ALM_BDS                =  U32BITMASK_BIT(11)

                                            # DELETE_SVDIR_BDS
                                            # DELETE_SVSTEER_BDS
                                            # DELETE_ALM_CORR_BDS
                                            # DELETE_GNSS_SV_BLACKLIST_GPS

                                            # DELETE_GNSS_SV_BLACKLIST_GLO
                                            # DELETE_GNSS_SV_BLACKLIST_BDS
                                            # DELETE_GNSS_SV_BLACKLIST_GAL
                                            # DELETE_EPH_GAL

                                            # DELETE_ALM_GAL                =  U32BITMASK_BIT(20)
                                            # DELETE_SVDIR_GAL
                                            # DELETE_SVSTEER_GAL
                                            # DELETE_ALM_CORR_GAL

                                            # DELETE_GNSS_SV_BLACKLIST_QZSS
                                            # DELETE_EPH_QZSS
                                            # DELETE_ALM_QZSS               =  U32BITMASK_BIT(26)
                                            # DELETE_SVDIR_QZSS

                                            # DELETE_SVSTEER_QZSS
                                            # DELETE_ALM_CORR_QZSS
                                            # DELETE_IONO_QZSS              


  $req_coldstart->{"gnss_reserved"}   = 0;    # not used at present
  $req_coldstart->{"time_unc"}        = 0;  # Inject Time Uncertainty
                                            # Only applicable if INJECT_TIME_UNC bit = 1
  $req_coldstart->{"pos_unc"}         = 0;  # Inject Position Uncertainty
                                            # Only applicable if INJECT_POS_UNC bit = 1
  $req_coldstart->{"time_offset"}     = 0;  # Add a Time Offset
                                            # Only applicable if INJECT_TIME_OFFSET bit = 1
  $req_coldstart->{"pos_offset"}      = 0;  # Add a Position Offset
                                            # Only applicable if INJECT_POS_OFFSET bit = 1
  $req_coldstart->{"eph_sv_mask"}     = 0xFFFFFFFF;  # Ephemeris SV Mask
  $req_coldstart->{"alm_sv_mask"}     = 0xFFFFFFFF;  # Almanac SV Mask

  $req_coldstart->{"glo_eph_sv_mask"}     = 0xFFFFFFFF;  # not used
  $req_coldstart->{"glo_alm_sv_mask"}     = 0xFFFFFFFF;  # not used
  $req_coldstart->{"sbas_eph_sv_mask"}     = 0;  # delete SBAS eph
  $req_coldstart->{"sbas_alm_sv_mask"}     = 0xFFFFFFFF;  # delete SBAS alm
  $req_coldstart->{"delete_celldb_mask"} = 0xFFFFFFFF;  # Delete Cell Database Mask
                                                        # Only applicable if DELETE_CELLDB_INFO bit = 1
  $req_coldstart->{"delete_clk_info"}     = 0xFFFFFF;  # delete clock info
  $req_coldstart->{"reserved"}     = 0;  # not used

  ######################################################################
  # Construct the DM CGPS_SET_PARAM_CMD capture request packet
  ######################################################################

  my $req_coldstart_pkt = $req_coldstart->SetCaptureCmd();

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  my $rsp_coldstart_pkt = $qxdm_app->SendDmIcdPacketEx($req_coldstart_pkt, $timeout);

  my $rsp_coldstart_len = length($rsp_coldstart_pkt);
  printf ("DM CGPS_SET_PARAM_CMD Response Packet Length: %d\n", $rsp_coldstart_len);

  $req_coldstart->ParseData($rsp_coldstart_pkt);

  $status    = $req_coldstart->GetStatus();
  $client_id = $req_coldstart->GetClientId();
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

SetColdStartParametersCmd

Syntax:   pd_DelAllButAlm [options]

Options:  -help               Display this help message.

Example:  perl pd_DelAllButAlm.pl <COM Port> <Client Type>

END

  print "$usage";
  exit (0);
}
