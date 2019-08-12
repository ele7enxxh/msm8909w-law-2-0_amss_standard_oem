################################################################################
# $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/pdapi_scripts/pd_PowerConsumptionAlmDownloadTrack.pl#1 $
# $DateTime: 2016/12/13 07:59:45 $
# $Author: mplcsds1 $
#
# This script is to set up the device to perform gps tracking.  The current
# almanac will be retrieved from the web and pushed onto the device.  A GPS
# standalone session to get fixes at a 1Hz rate is started.
#
#
# Usage: perl pd_PowerConsumptionAlmDownloadTrack.pl <COM Port> [<alignment>] [<cold start>] [<tbf>]
#   COM Port: The Com Port the device is on. ie 2 if the device is on COM2.
#   alignment: The alignment to use when generating the almanac files.
#               4 - use 4-byte alignment
#               8 - use 8-byte alignment (default).
#   cold start: 0 - Starts the stand alone session (does not delete or download
#                   new data (default).
#               1 - Delete previous data, and download new almanac data.
#   tbf       : The time between fixes in seconds.  If no TBF is specified
#                 then a default value of 1 sec will be used.                     
#
# Revision History
# when       who    what, where, why
# --------   ---    ----------------------------------------------------------
# 03/21/12   sp     Added del all followed by xtra injection.
# 08/16/10   ssk    Move and rename power script. Add tbf parameter support
# 09/10/08   jlp    Move SvNoExist check so it is done everytime the script
#                   is run.  Added checking ..\..\almanac_provisioning_scripts
#                   for the AlmFileXX files.  It goes through each file,
#                   checking the almanac health bits.  If the health bits are
#                   not 0x00, the file is deleted.
#
################################################################################

use vars qw(%opts);
use lib "..\\standalone_rfv\\single_sv_standalone";
use lib "..\\almanac_provisioning_scripts";

use Win32::OLE;
use Win32::OLE::Variant;

use Getopt::Long;
use Cwd;
use pd_DelAll;
use pd_StartDiag;
use pd_Getpos;
use pd_EndDiag;

# Set the timeout to 2 seconds.
use constant TIMEOUT => 2000;

# CGPS client type
use constant CLIENT_TYPE => 8;

%opts = ();
GetOptions (\%opts, 'help');

&usage() if ($opts{'help'});

################################################################################
# Check for user input and prompt if arguments are not provided
################################################################################

$comport     = $ARGV[0];

# Whether the almanac files should be 4-byte aligned or 8-byte aligned.
$alignment   = $ARGV[1];

# Whether to do a cold start, or go right into the standalone session.
$cold_start  = $ARGV[2];

# Time Between Fixes.
$tbf         = $ARGV[3];

# Allow user input for comport
if (not defined ($comport))
{
  print("\nUsage: perl pd_PowerConsumptionAlmDownloadTrack.pl <COM Port> [<alignment>] [<cold start>] [<tbf>]\n\n");
  print ("Please enter the COM port the FFA is connected to\n");
  chomp ($comport = <STDIN>);
  print("\n");
}

if (not defined ($alignment))
{
  $alignment = 8;
}
elsif(($alignment != 8) && ($alignment != 4))
{
  die "Invalid alignment parameter $alignment\n";
}

if (not defined ($cold_start))
{
  $cold_start = 0;
}
elsif(($cold_start != 0) && ($cold_start != 1))
{
  die "Invalid cold start parameter $cold_start\n";
}

if (not defined ($tbf))
{
  $tbf = 1;
}

# get the current working directory.  We want to change back to this directory
# after changing to other directories.
my $working_dir = getcwd();

################################################################################
# Create QXDM instance
################################################################################

my $qxdm_app = new Win32::OLE 'QXDM.Application';
if (!$qxdm_app)
{
  print "ERROR: Unable to invoke the QXDM application.\n";
  die;
}

################################################################################
# Create QXDM2 instance
################################################################################

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

################################################################################
# Register for Required Events
################################################################################

$clientObject->AddEvent( 605 );       #EVENT_GPS_PD_SESS_START
$clientObject->AddEvent( 601 );       #EVENT_GPS_PD_FIX_START
$clientObject->AddEvent( 614 );       #EVENT_GPS_PD_POSITION
$clientObject->AddEvent( 602 );       #EVENT_GPS_PD_FIX_END
$clientObject->AddEvent( 411 );       #EVENT_GPS_PD_SESS_END

$clientObject->CommitConfig();

################################################################################
#  Open QXDM Connection by setting the COM port from command line.
################################################################################

# COM Port value should be supplied as first argument to this script
$qxdm_app->{COMPort} = $comport;

# Wait until phone is connected
while ($qxdm_app->{IsPhoneConnected} == 0)
{
  print "Unable to Connect to the Phone. Retrying...\n";
  sleep(1);
}

$qxdm_app->SendScript("logging on");

################################################################################
# Setup CGPS Diag Client
################################################################################
&setup_cgps_client;

if($cold_start)
{
  ##############################################################################
  # Delete all of the CGPS parameters (alm, eph, etc.)
  ##############################################################################
  &cgps_delete;

  # Need to tear down the device since the low priority efs task on some targets
  # that handled reading/writing the data was changed.  It is no longer
  # asynchronous since the IPC buffer was being overflowed during testing.  This
  # script was deleting the data, which was marked for deletion, then loading
  # the almanac, which was deleted when the device reset after loading the
  # almanac.  The fix is to reset the device after the delete to ensure the
  # almanac data is not deleted.  This adds about 10s to the runtime of the
  # script.
  &teardown_cgps_client;
  &reset_device;
  &setup_cgps_client;

 &cgps_load_xtra;

  ##############################################################################
  # Push the almanac data to the device.
  ##############################################################################
  &cgps_load_alm;
}

##############################################################################
# Check for Sv's that don't exist
##############################################################################
chdir '..\AlmanacScripts';
system("perl SvNoExist.pl $comport") == 0 or
 die "\nFailed to check for Sv's that don't exist\n";
chdir $working_dir;


################################################################################
# Start the diag get position process
################################################################################
&cgps_start_get_pos($tbf);

$qxdm_app = NULL;


################################################################################
# METHOD:
#   usage
#
# DESCRIPTION:
#   Display usage information.
#
# RETURN VALUE:
#   None
#
################################################################################
sub usage
{
  my $usage = <<END;

pd_PowerConsumptionAlmDownloadTrack

Syntax:   pd_PowerConsumptionAlmDownloadTrack.pl [options] <COM Port> [<alignment>] [<cold start>] [<tbf>]

Options:  -help               Display this help message.

Parameters:
  Com Port    The communications port as a number used to communicate with the
              device.

  alignment   The alignment to use when generating the almanac files.
              4 - use 4-byte alignment
              8 - use 8-byte alignment (default)

  cold start  Deletes all data and downloads almanac information.
              0 - Start the tracking session (does not delete data and download
                  new almanac)
              1 - Does a cold start (Delete data and download new almanac before
                  starting tracking session).

  tbf         The time between fixes in seconds. If no TBF is specificed
              then a default value of 1 sec will be used.                    

Example:  perl pd_PowerConsumptionAlmDownloadTrack.pl 2 8 1 1

END

  print "$usage";
  exit (0);
}


################################################################################
# METHOD:
#   reset_device
#
# DESCRIPTION:
#   Resets the device.  This will also wait for the device to come back online.
#
# RETURN VALUE:
#   None
#
################################################################################
sub reset_device
{
  ############################################################################
  # Reset the device so the alamanac data will be read
  ############################################################################
  print("~~~~~~~~~~~~~~\n");
  print("Reseting Phone\n");
  print("~~~~~~~~~~~~~~\n");
  $qxdm_app->OfflineDigital();
  $qxdm_app->ResetPhone();

  # Wait for phone to disconnect (or 5s incase it was missed)
  for($i = 0; ($i < 5), ($qxdm_app->{IsPhoneConnected} == 1); $i++)
  {
    sleep(1);
  }

  print "Trying to Reconnect to the phone";
  while ($qxdm_app->{IsPhoneConnected} == 0)
  {
    print ".";
    sleep(1);
  }

  print "\n";
  # If it takes a while for the device to connect to USB, increase this sleep time.
  sleep(13);
}


################################################################################
# METHOD:
#   start_cgps_task
#
# DESCRIPTION:
#   Starts the CGPS task
#
# RETURN VALUE:
#   None
#
################################################################################
sub start_cgps_task
{
  printf("Start GPS DIAG task...\n");
  my $req_starttask_len = 4;
  my $req_starttask_pkt = Variant(VT_ARRAY | VT_UI1, $req_starttask_len);

  $req_starttask_pkt->Put(0, 75); # Diag Cmd Code
  $req_starttask_pkt->Put(1, 13); # Diag Subsystem ID
  $req_starttask_pkt->Put(2,  8); # Diag Subsystem Cmd Code
  $req_starttask_pkt->Put(3,  0);
  my $req_start_task_pkt = $qxdm_app->SendDmIcdPacketEx($req_starttask_pkt,
   TIMEOUT);
  printf ("Sending CGPS_DIAG_START_TASK command...\n");
}


################################################################################
# METHOD:
#   init_cgps_client
#
# DESCRIPTION:
#   Initializes the cgps client
#
# RETURN VALUE:
#   None
#
################################################################################
sub init_cgps_client
{
  ############################################################################
  # Build a diagnostic request for CGPS_INIT_CMD to initialize the client
  ############################################################################

  $req_apicmd1 = new pd_StartDiag;
  $req_apicmd1->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_apicmd1->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_apicmd1->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_apicmd1->{"cgps_cmd_code"}   = 0;   # CGPS Cmd Code
  $req_apicmd1->{"version"}         = 1;   # Version Number
  $req_apicmd1->{"client_id"}       = $CLIENT_TYPE; # Client Type

  ###########################################################################
  # Construct the DM CGPS_INIT_CMD capture request packet
  ###########################################################################

  my $req_apicmd_pkt = $req_apicmd1->SetCaptureCmd();
  printf ("Sending CGPS_INIT_CMD: Initialize client type %d\n", $clientType);

  ###########################################################################
  # Send command request packet, receive command response packet
  ###########################################################################

  my $rsp_apicmd_pkt = $qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, TIMEOUT);

  my $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf ("DM CGPS_INIT_CMD Response Packet Length: %d\n", $rsp_apicmd_len);

  $req_apicmd1->ParseData($rsp_apicmd_pkt);

  $status    = $req_apicmd1->GetStatus();
  $client_id = $req_apicmd1->GetClientId();
  printf ("Status = %d, Client Id = %d\n\n", $status, $client_id);

  sleep(2);
}


################################################################################
# METHOD:
#   release_cgps_client
#
# DESCRIPTION:
#   Releases the cgps client
#
# RETURN VALUE:
#   None
#
################################################################################
sub release_cgps_client
{
  ############################################################################
  # Build a diagnostic request for CGPS_RELEASE_CMD to Release the client
  ############################################################################

  $req_apicmd3 = new pd_EndDiag;
  $req_apicmd3->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_apicmd3->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_apicmd3->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_apicmd3->{"cgps_cmd_code"}   = 9;   # CGPS Cmd Code
  $req_apicmd3->{"version"}         = 1;   # Version Number
  $req_apicmd3->{"client_id"}       = $client_id;  # Client ID from Deactivate

  ############################################################################
  # Construct the DM CGPS_RELEASE_CMD capture request packet
  ############################################################################

  my $req_apicmd_pkt = $req_apicmd3->SetCaptureCmd();
  printf ("Sending CGPS_RELEASE_CMD: Release client ID %d\n", $client_id);

  ############################################################################
  # Send command request packet, receive command response packet
  ############################################################################

  my $rsp_apicmd_pkt = $qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, $timeout);

  my $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf ("DM CGPS_RELEASE_CMD Response Packet Length: %d\n", $rsp_apicmd_len);

  $req_apicmd3->ParseData($rsp_apicmd_pkt);

  $status    = $req_apicmd3->GetStatus();
  $client_id = $req_apicmd3->GetClientId();
  printf ("Status = %d, Client Id = %d\n", $status, $client_id);
  sleep(2);
}

################################################################################
# METHOD:
#   register_cgps_client
#
# DESCRIPTION:
#   Registers the cgps client
#
# RETURN VALUE:
#   None
#
################################################################################
sub register_cgps_client
{
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

  ############################################################################
  # Construct the DM CGPS_REGISTER_PD_CMD capture request packet
  ############################################################################

  my $req_apicmd_pkt = $req_apicmd2->SetCaptureCmd();
  printf ("Sending CGPS_REGISTER_PD_CMD: Register client ID %d\n", $client_id);

  ############################################################################
  # Send command request packet, receive command response packet
  ############################################################################

  my $rsp_apicmd_pkt = $qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, TIMEOUT);

  my $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf ("DM CGPS_REGISTER_PD_CMD Response Packet Length: %d\n",
   $rsp_apicmd_len);

  $req_apicmd2->ParseData($rsp_apicmd_pkt);

  $status    = $req_apicmd2->GetStatus();
  $client_id = $req_apicmd2->GetClientId();
  printf ("Status = %d, Client Id = %d\n\n", $status, $client_id);

  sleep(2);
}


################################################################################
# METHOD:
#   deregister_cgps_client
#
# DESCRIPTION:
#   deregisters the cgps client
#
# RETURN VALUE:
#   None
#
################################################################################
sub deregister_cgps_client
{
  ############################################################################
  # Build a diagnostic request for CGPS_REGISTER_PD_CMD to Deregister the
  # client
  ############################################################################

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
  printf ("Sending CGPS_REGISTER_PD_CMD: DeRegister client ID %d\n",
   $client_id);

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  my $rsp_apicmd_pkt = $qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, $timeout);

  my $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf ("DM CGPS_REGISTER_PD_CMD Response Packet Length: %d\n",
   $rsp_apicmd_len);

  $req_apicmd2->ParseData($rsp_apicmd_pkt);

  $status    = $req_apicmd2->GetStatus();
  $client_id = $req_apicmd2->GetClientId();
  printf ("Status = %d, Client Id = %d\n\n", $status, $client_id);

  sleep(2);
}
 

################################################################################
# METHOD:
#   activate_cgps_client
#
# DESCRIPTION:
#   Activates the cgps client
#
# RETURN VALUE:
#   None
#
################################################################################
sub activate_cgps_client
{
  ############################################################################
  # Build a diagnostic request for CGPS_ACTIVATE_CMD to activate the client
  # Activates the client so that it can be notified of PD events
  ############################################################################

  $req_apicmd3 = new pd_StartDiag;
  $req_apicmd3->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_apicmd3->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_apicmd3->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_apicmd3->{"cgps_cmd_code"}   = 7;   # CGPS Cmd Code
  $req_apicmd3->{"version"}         = 1;   # Version Number
  $req_apicmd3->{"client_id"}       = $client_id;  # Client ID from Init

  ############################################################################
  # Construct the DM CGPS_ACTIVATE_CMD capture request packet
  ############################################################################

  my $req_apicmd_pkt = $req_apicmd3->SetCaptureCmd();
  printf ("Sending CGPS_ACTIVATE_CMD: Activate client ID %d\n", $client_id);

  ############################################################################
  # Send command request packet, receive command response packet
  ############################################################################

  my $rsp_apicmd_pkt = $qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, TIMEOUT);

  my $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf ("DM CGPS_ACTIVATE_CMD Response Packet Length: %d\n", $rsp_apicmd_len);

  $req_apicmd3->ParseData($rsp_apicmd_pkt);

  $status    = $req_apicmd3->GetStatus();
  $client_id = $req_apicmd3->GetClientId();
  printf ("Status = %d, Client Id = %d\n\n", $status, $client_id);
}


################################################################################
# METHOD:
#   deactivate_cgps_client
#
# DESCRIPTION:
#   deactivates the cgps client
#
# RETURN VALUE:
#   None
#
################################################################################
sub deactivate_cgps_client
{
  ############################################################################
  # Build a diagnostic request for CGPS_DEACTIVATE_CMD to deactivate the
  # client
  ############################################################################

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
  printf ("Sending CGPS_DEACTIVATE_CMD: Deactivate client type %d\n",
   $clientType);

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  my $rsp_apicmd_pkt = $qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, $timeout);

  my $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf ("DM CGPS_DEACTIVATE_CMD Response Packet Length: %d\n",
   $rsp_apicmd_len);

  $req_apicmd1->ParseData($rsp_apicmd_pkt);

  $status    = $req_apicmd1->GetStatus();
  $client_id = $req_apicmd1->GetClientId();
  printf ("Status = %d, Client Id = %d\n\n", $status, $client_id);

  sleep(2);
}

################################################################################
# METHOD:
#   setup_cgps_client
#
# DESCRIPTION:
#   Starts the CGPS task, and sets up the client
#
# RETURN VALUE:
#   None
#
################################################################################
sub setup_cgps_client
{
  # Start the CGPS task
  &start_cgps_task;

  print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  print("Initialize, Register, & Activate the client\n");
  print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

  &init_cgps_client;
  &register_cgps_client;
  &activate_cgps_client;
}


################################################################################
# METHOD:
#   teardown_cgps_client
#
# DESCRIPTION:
#   Ends the cgps client.
#
# RETURN VALUE:
#   None
#
################################################################################
sub teardown_cgps_client
{
  print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  print("DeActivate, DeRegister, & Release the client\n");
  print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

  &deactivate_cgps_client;
  &deregister_cgps_client;
  &release_cgps_client;
}


################################################################################
# METHOD:
#   cgps_delete
#
# DESCRIPTION:
#   Deletes all CGPS data (alm, eph, etc.).  setup_cgps_client must be called
#   before this function.
#
# RETURN VALUE:
#   None
#
################################################################################
sub cgps_delete
{
  ############################################################################
  # Build a diagnostic request for CGPS_SET_PARAM_CMD: DELETE_PARAMS
  ############################################################################

  printf ("\nDeleting All GPS Parameters\n");

  $req_coldstart = new pd_DelAll;
  $req_coldstart->{"cmd_code"}        = 75;       # Diag Cmd Code
  $req_coldstart->{"subsys_id"}       = 13;       # Diag Subsystem ID
  $req_coldstart->{"subsys_cmd_code"} = 100;      # Diag Subsystem Cmd Code
  $req_coldstart->{"cgps_cmd_code"}   = 12;       # CGPS Cmd Code
  $req_coldstart->{"version"}         = 1;        # Version Number
  $req_coldstart->{"client_id"}       = $client_id;     # TEST2
  $req_coldstart->{"param_type"}      = 4;        # PA_DELETE_PARAMS
  $req_coldstart->{"param_length"}    = 64;       # 16 x (4 byte fields)
  $req_coldstart->{"delete_bitmap"}   = 0x87FF;   # Delete All (1000|0111|1111|1111 = 0x87FF = 34815)
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

  $req_coldstart->{"gnss_delete_bitmap"} = 0;     # not used at present
  $req_coldstart->{"gnss_reserved"}   = 0;        # not used at present
  $req_coldstart->{"time_unc"}        = 0;        # Inject Time Uncertainty
                                                  # Only applicable if INJECT_TIME_UNC bit = 1
  $req_coldstart->{"pos_unc"}         = 0;        # Inject Position Uncertainty
                                                  # Only applicable if INJECT_POS_UNC bit = 1
  $req_coldstart->{"time_offset"}     = 0;        # Add a Time Offset
                                                  # Only applicable if INJECT_TIME_OFFSET bit = 1
  $req_coldstart->{"pos_offset"}      = 0;        # Add a Position Offset
                                                  # Only applicable if INJECT_POS_OFFSET bit = 1
  $req_coldstart->{"eph_sv_mask"}     = 0xFFFFFFFF;  # Ephemeris SV Mask
  $req_coldstart->{"alm_sv_mask"}     = 0xFFFFFFFF;  # Almanac SV Mask

  $req_coldstart->{"glo_eph_sv_mask"}     = 0;    # not used
  $req_coldstart->{"glo_alm_sv_mask"}     = 0;    # not used
  $req_coldstart->{"sbas_eph_sv_mask"}     = 0;   # delete SBAS alm
  $req_coldstart->{"sbas_alm_sv_mask"}     = 0;   # delete SBAS eph
  $req_coldstart->{"delete_celldb_mask"} = 0xFFFFFFFF;  # Delete Cell Database Mask
                                                        # Only applicable if DELETE_CELLDB_INFO bit
  $req_coldstart->{"delete_clk_info"}     = 0;    # delete clock info
  $req_coldstart->{"reserved"}     = 0;           # not used

  ############################################################################
  # Construct the DM CGPS_SET_PARAM_CMD capture request packet
  ############################################################################

  my $req_coldstart_pkt = $req_coldstart->SetCaptureCmd();

  ############################################################################
  # Send command request packet, receive command response packet
  ############################################################################

  my $rsp_coldstart_pkt = $qxdm_app->SendDmIcdPacketEx($req_coldstart_pkt,
   TIMEOUT);

  my $rsp_coldstart_len = length($rsp_coldstart_pkt);
  printf ("DM CGPS_SET_PARAM_CMD Response Packet Length: %d\n",
   $rsp_coldstart_len);

  $req_coldstart->ParseData($rsp_coldstart_pkt);

  ############################################################################
  $status    = $req_coldstart->GetStatus();
  $client_id = $req_coldstart->GetClientId();
  printf ("Status = %d, Client Id = %d\n\n", $status, $client_id);
}

 
################################################################################
# METHOD:
#   cgps_load_alm
#
# DESCRIPTION:
#   Loads almanac data to the device.  As a side affect, the device is reset.
#   setup_cgps_client must be called before this function.
#
# RETURN VALUE:
#   None
#
################################################################################
sub cgps_load_alm
{
  chdir '..\almanac_provisioning_scripts';
  printf ("\nRetrieve alamac data from the internet\n");
  system("perl generate_almanac.pl $alignment") == 0 or
   die "\nFailed to retrieve the almanac data\n";
  chdir $working_dir;
  &sv_alm_health_check;
  chdir '..\almanac_provisioning_scripts';
  printf("Pushing the almanac data onto the device\n");
  system("perl alm_download.pl $comport") == 0 or
   die "\nFailed to load the almanac data\n";
  chdir $working_dir;
}

################################################################################
# METHOD:
#   cgps_load_xtra
#
# DESCRIPTION:
#   Injects XTRA2 bin file to DUT
#
# RETURN VALUE:
#   None
#
################################################################################
sub cgps_load_xtra
{
  printf ("\nRetrieve alamac data from the internet\n");
  system("perl pd_XtraDiagInjector.pl $comport") == 0 or
   die "\nFailed to inject XTRA data\n";
}


################################################################################
# METHOD:
#   cgps_start_get_pos
#
# DESCRIPTION:
#   Starts the process for the diag task to get positions. setup_cgps_client
#   must be called before this function.
#
# RETURN VALUE:
#   None
#
################################################################################
sub cgps_start_get_pos
{
  my $tbf = shift @_;

  die "Time Between Fixes parameter needs to be defined" if not defined $tbf or
   $tbf < 0;

  ############################################################################
  # Get Position Cmd parameters used for Apptracking only
  ############################################################################

  print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  print("Apply get_pos to Start a GPS Session\n");
  print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

  ############################################################################
  # Build a diagnostic request for CGPS_START_CMD to do a Standalone fix
  ############################################################################

  $req_getpos = new pd_GetPos;
  $req_getpos->{"cmd_code"}          = 75;            # Diag Cmd Code
  $req_getpos->{"subsys_id"}         = 13;            # Diag Subsystem ID
  $req_getpos->{"subsys_cmd_code"}   = 100;           # Diag Subsystem Cmd Code
  $req_getpos->{"cgps_cmd_code"}     = 10;            # CGPS Cmd Code
  $req_getpos->{"version"}           = 1;             # Version Number
  $req_getpos->{"session_type"}      = 2;             # 0 = latest position
                                                      # 1 = new position
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
  $req_getpos->{"qos_gps_sess_tout"} = 90;            # QoS timeout value
                                                      #  between 0-255 seconds
                                                      # Default for 1x         
                                                      #  = 16 sec
                                                      # Default for UMTS       
                                                      #  = 45 sec
                                                      # Default for Standalone 
                                                      #  = 60 sec
  $req_getpos->{"qos_acc_thrshld"}   = 50;            # QoS accuracy threshold
                                                      #  in meters
  $req_getpos->{"time_bw_fixes"}     = $tbf;          # tbf only used for app
                                                      #  tracking
  $req_getpos->{"num_fixes"}         = 9999999;       # num > 1 fix -> used for
                                                      #  app tracking
  $req_getpos->{"client_id"}         = $client_id;    # Returned by PDAPI
                                                      #  duriing client
                                                      #  registration
  $req_getpos->{"reserved"}          = 16;            # testing only
  $req_getpos->{"server_ipv4_adrs"}  = "10.1.2.154";  # !!Change to the local
                                                      #  Custom Server Address!!
  $req_getpos->{"server_ipv4_port"}  = 4911;          # !!Change to the local
                                                      #  Custom SIA port
                                                      #  number!!

  ############################################################################
  # Construct the DM CGPS_START_CMD capture request packet
  ############################################################################

  my $req_getpos_pkt = $req_getpos->SetCaptureCmd();
  $PrevIndex = $qxdm_app2->GetClientItemCount( $ReqHandle ) - 1;   

  ############################################################################
  # Send command request packet, receive command response packet
  ############################################################################

  my $rsp_getpos_pkt = $qxdm_app->SendDmIcdPacketEx($req_getpos_pkt, TIMEOUT);

  my $rsp_getpos_len = length($rsp_getpos_pkt);
  printf ("\nDM CGPS_START_CMD Response Packet Length = %d\n", $rsp_getpos_len);

  $req_getpos->ParseData($rsp_getpos_pkt);

  $status    = $req_getpos->GetStatus();
  $client_id = $req_getpos->GetClientId();
  printf ("Status = %d, Client Id = %d\n\n", $status, $client_id);
}


################################################################################
# METHOD:
#   sv_alm_health_check
#
# DESCRIPTION:
#   Checks ..\..\almanac_provisioning_scripts for the AlmFileXX files.  It goes
#   through each file, checking the almanac health bits.  If the health bits are
#   not 0x00, the file is deleted.
#
# RETURN VALUE:
#   None
#
################################################################################
sub sv_alm_health_check
{
  # health_pos is the byte offset in the almanac file where the SV health
  # information resides.  This is dependent on the byte alignment of the file.
  my $health_pos;

  # SV health from the alm file.
  my $sv_health;


  if ((not defined ($alignment)) || $alignment == 8)
  {
    $health_pos = 9;
  }
  elsif($alignment != 4)
  {
    $health_pos = 5;
  }
  else
  {
    return;
  }

  printf("Check almanacs for unhealthy Sv's\n");

  ################################################################################
  # Go through and check which AlmFileXX files have unhealthy SVs.
  ################################################################################
  for($i = 0; $i < 32; $i++)
  {
    $alm_file = sprintf("../../almanac_provisioning_scripts/AlmFile%02u", $i);

    # don't search for SV's that don't have an almanac file, or have any of their
    # health bits set
    if(-e "$alm_file")
    {
      open ALM, $alm_file or die "Can't open $alm_file!  Exiting!\n";
      seek ALM, $health_pos, SEEK_SET;
      read ALM, $sv_health, 1;
      close ALM;

      if(not defined ($sv_health))
      {
        die "$alm_file is not formated properly.  Exiting!\n";
      }

      if((unpack "C", $sv_health) != 0)
      {
        if(!(unlink $alm_file))
        {
          die "Could not delete $alm_file.  Exiting!\n";
        }
      }
    }
  }
}

