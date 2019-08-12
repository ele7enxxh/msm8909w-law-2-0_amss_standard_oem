################################################################################
# $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/pdapi_scripts/pd_GpsClientsStartStop.pm#1 $
# $DateTime: 2016/12/13 07:59:45 $
# $Author: mplcsds1 $
#
# This Perl package facilitiate setup and teardown of the GPS Diag clients.
#
# Revision History
# when       who    what, where, why
# --------   ---    ----------------------------------------------------------
# 08/17/10   ssk     Initial version.
#
################################################################################

package pd_GpsClientsStartStop;

require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw(GpsClientStart GpsClientStop GpsClientEventsProc);

use strict;

# CGPS client type
# PDSM_CLIENT_TYPE_TEST2
use constant  CLIENT_TYPE_TEST2 => 8; 
#PDSM_CLIENT_TYPE_XTRA
use constant CLIENT_TYPE_XTRA => 11; 


use vars qw( %opts);

use lib "..\\..\\PdapiScripts";
use lib "..\\..\\pdapi_scripts";

use lib "..\\..\\standalone_rfv\\single_sv_standalone";


use Win32::OLE;
use Win32::OLE::Variant;

use Getopt::Long;
use Cwd;

use pd_StartDiag;
use pd_EndDiag;


# Set the timeout to 2 seconds.
use constant TIMEOUT => 2000;


# Clear options for single sv track
use constant SINGLE_SV_CLR => 1;
use constant SINGLE_SV_NOCLR => 2;


# Process Client Events
my $CanStartNextSession = 0;
my $GPS_PD_SESS_START_RECEIVED = 0;
my $GPS_PD_FIX_START_RECEIVED = 0;
my $GPS_PD_XTRA_DATA_ACK_RECEIVED = 0;


################################################################################
# METHOD:
#   GpsClientStart
#
# DESCRIPTION:
#   Starts the CGPS Clients
#
# RETURN VALUE:
#   None
#
################################################################################
sub GpsClientStart
{

  ################################################################################
  # Create QXDM instance
  ################################################################################
  print main::LOGFILE "Start Client Invoking Application \n";
  print "Start Client Invoking Application \n";
  
  $main::qxdm_app = new Win32::OLE 'QXDM.Application';
  if (!$main::qxdm_app)
  {
    print main::LOGFILE "ERROR: Unable to invoke the QXDM application.\n";
    print "ERROR: Unable to invoke the QXDM application.\n";    
    die;
  }

  ################################################################################
  # Create QXDM2 instance
  ################################################################################
  print main::LOGFILE "Start Client Create QXDM Instance \n";
  print "Start Client Create QXDM Instance \n";

  $main::qxdm_app2 = $main::qxdm_app->GetIQXDM2();
  if ($main::qxdm_app2 == 0)
  {
   print main::LOGFILE "QXDM does not support required interface\n";
   print "QXDM does not support required interface\n";   
   sleep(5);
   die;
  }

  print main::LOGFILE "Start Client Register \n";
  print "Start Client Register \n";  
  # Register the QXDM client.
  $main::ReqHandle = $main::qxdm_app2->RegisterQueueClient( 256 );
  if ($main::ReqHandle == 0xFFFFFFFF)
  {
   print main::LOGFILE "Unable to register as QXDM client\n";
   print main::LOGFILE "Unable to register as QXDM client\n";   
   sleep(5);
   die;
  }

  print main::LOGFILE "Start Client Configure  \n";
  print "Start Client Configure  \n";  
  # Get a configuration object.
  $main::clientObjectGlobal = $main::qxdm_app2->ConfigureClientByKeys( $main::ReqHandle );
  if ($main::clientObjectGlobal == 0)
  {
   print main::LOGFILE "Unable to register as QXDM client\n";
   sleep(5);
   die;
  }

  ################################################################################
  # Register for Required Events
  ################################################################################

  $main::clientObjectGlobal->AddEvent( 605 );      #EVENT_GPS_PD_SESS_START
  $main::clientObjectGlobal->AddEvent( 601 );      #EVENT_GPS_PD_FIX_START
  $main::clientObjectGlobal->AddEvent( 614 );      #EVENT_GPS_PD_POSITION
  $main::clientObjectGlobal->AddEvent( 602 );      #EVENT_GPS_PD_FIX_END
  $main::clientObjectGlobal->AddEvent( 411 );      #EVENT_GPS_PD_SESS_END
  $main::clientObjectGlobal->AddEvent( 1927 );     #EVENT_GPSONEXTRA_DATA_ACK (0x0840 )
  $main::clientObjectGlobal->AddEvent( 1376 );     #EVENT_GPSONEXTRA_START_DOWNLOAD (0x0560 )
  $main::clientObjectGlobal->AddEvent( 1377 );     #EVENT_GPSONEXTRA_END_DOWNLOAD (0x0561)

  $main::clientObjectGlobal->CommitConfig();

  ################################################################################
  #  Open QXDM Connection by setting the COM port from command line.
  ################################################################################

  # COM Port value should be supplied as first argument to this script
  $main::qxdm_app->{COMPort} = $main::COMPORT;

  # Wait until phone is connected
  while ($main::qxdm_app->{IsPhoneConnected} == 0)
  {
    print main::LOGFILE "Unable to Connect to the Phone. CHECK USB connection. Retrying...\n";
    sleep(1);
  }

  $main::qxdm_app->SendScript("logging on");

  # Start the CGPS task
   &start_cgps_task;


  #start the test2 client
  &setup_cgps_client_test2;

  #start the XTRA client
  &setup_cgps_client_xtra;

  #$main::qxdm_app = NULL;
}


################################################################################
# METHOD:
#   GpsClientStop
#
# DESCRIPTION:
#   Client Stop subroutines
#
# RETURN VALUE:
#   None
#
################################################################################
sub GpsClientStop
{
  ################################################################################
  # QXDM instance
  ################################################################################
  print main::LOGFILE "Stop Client Invoking Application n \n";

  #stop the xtra client
  &teardown_cgps_client_xtra;
  
  #stop the test2 client
  &teardown_cgps_client_test2;

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
  my $req_start_task_pkt = $main::qxdm_app->SendDmIcdPacketEx($req_starttask_pkt,
   TIMEOUT);
  printf main::LOGFILE ("\nSending CGPS_DIAG_START_TASK command...\n");
}


################################################################################
# METHOD:
#   init_cgps_client_xtra
#
# DESCRIPTION:
#   Initializes the cgps client
#
# RETURN VALUE:
#   None
#
################################################################################
sub init_cgps_client_xtra
{
  ############################################################################
  # Build a diagnostic request for CGPS_INIT_CMD to initialize the client
  ############################################################################

  my $req_apicmd1 = new pd_StartDiag;
  $req_apicmd1->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_apicmd1->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_apicmd1->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_apicmd1->{"cgps_cmd_code"}   = 0;   # CGPS Cmd Code
  $req_apicmd1->{"version"}         = 1;   # Version Number
  $req_apicmd1->{"client_id"}       = CLIENT_TYPE_XTRA; # Client Type test2

  ###########################################################################
  # Construct the DM CGPS_INIT_CMD capture request packet
  ###########################################################################

  my $req_apicmd_pkt = $req_apicmd1->SetCaptureCmd();
  printf main::LOGFILE ("Sending CGPS_INIT_CMD: Initialize XTRA client type %d\n", CLIENT_TYPE_XTRA);

  ###########################################################################
  # Send command request packet, receive command response packet
  ###########################################################################

  my $rsp_apicmd_pkt = $main::qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, TIMEOUT);

  my $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf main::LOGFILE ("DM CGPS_INIT_CMD Response Packet Length: %d\n", $rsp_apicmd_len);

  $req_apicmd1->ParseData($rsp_apicmd_pkt);

  my $status    = $req_apicmd1->GetStatus();
  $main::client_id_xtra = $req_apicmd1->GetClientId();
  printf main::LOGFILE ("Status = %d, Client Id = %d\n\n", $status, $main::client_id_xtra);

  sleep(2);
}


################################################################################
# METHOD:
#   register_cgps_client_xtra
#
# DESCRIPTION:
#   Registers the cgps client
#
# RETURN VALUE:
#   None
#
################################################################################
sub register_cgps_client_xtra
{
  ############################################################################
  # Build a diagnostic request for CGPS_REGISTER_PD_CMD to register the client
  # Registers the client to receive registered PD events
  ############################################################################

  my $req_apicmd2 = new pd_StartDiag;
  $req_apicmd2->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_apicmd2->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_apicmd2->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_apicmd2->{"cgps_cmd_code"}   = 205;   # CGPS Cmd Code
  $req_apicmd2->{"version"}         = 1;   # Version Number
  $req_apicmd2->{"client_id"}  = $main::client_id_xtra;  # Client ID from Init
  $req_apicmd2->{"reg_type"}        = 0;   # Register


  ############################################################################
  # Construct the DM CGPS_REGISTER_PD_CMD capture request packet
  ############################################################################

  my $req_apicmd_pkt = $req_apicmd2->SetCaptureCmd();
  printf main::LOGFILE ("Sending CGPS_REGISTER_PD_CMD: Register XTRA client ID %d\n", $main::client_id_xtra);

  ############################################################################
  # Send command request packet, receive command response packet
  ############################################################################

  my $rsp_apicmd_pkt = $main::qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, TIMEOUT);

  my $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf main::LOGFILE ("DM CGPS_REGISTER_PD_CMD Response Packet Length: %d\n",
   $rsp_apicmd_len);

  $req_apicmd2->ParseData($rsp_apicmd_pkt);

  my $status    = $req_apicmd2->GetStatus();
  $main::client_id_xtra = $req_apicmd2->GetClientId();
  printf main::LOGFILE ("Status = %d, Client Id = %d\n\n", $status, $main::client_id_xtra);

  sleep(2);
}


################################################################################
# METHOD:
#   activate_cgps_client_xtra
#
# DESCRIPTION:
#   Activates the cgps client
#
# RETURN VALUE:
#   None
#
################################################################################
sub activate_cgps_client_xtra
{
  ############################################################################
  # Build a diagnostic request for CGPS_ACTIVATE_CMD to activate the client
  # Activates the client so that it can be notified of PD events
  ############################################################################

  my $req_apicmd3 = new pd_StartDiag;
  $req_apicmd3->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_apicmd3->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_apicmd3->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_apicmd3->{"cgps_cmd_code"}   = 7;   # CGPS Cmd Code
  $req_apicmd3->{"version"}         = 1;   # Version Number
  $req_apicmd3->{"client_id"}       = $main::client_id_xtra;  # Client ID from Init

  ############################################################################
  # Construct the DM CGPS_ACTIVATE_CMD capture request packet
  ############################################################################

  my $req_apicmd_pkt = $req_apicmd3->SetCaptureCmd();
  printf main::LOGFILE ("Sending CGPS_ACTIVATE_CMD: Activate XTRA client ID %d\n", $main::client_id_xtra);

  ############################################################################
  # Send command request packet, receive command response packet
  ############################################################################

  my $rsp_apicmd_pkt = $main::qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, TIMEOUT);

  my $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf main::LOGFILE ("DM CGPS_ACTIVATE_CMD Response Packet Length: %d\n", $rsp_apicmd_len);

  $req_apicmd3->ParseData($rsp_apicmd_pkt);

  my $status    = $req_apicmd3->GetStatus();
  $main::client_id_xtra = $req_apicmd3->GetClientId();
  printf main::LOGFILE ("Status = %d, Client Id = %d\n\n", $status, $main::client_id_xtra);
}


################################################################################
# METHOD:
#   init_cgps_client_test2
#
# DESCRIPTION:
#   Initializes the cgps client
#
# RETURN VALUE:
#   None
#
################################################################################
sub init_cgps_client_test2
{
  ############################################################################
  # Build a diagnostic request for CGPS_INIT_CMD to initialize the client
  ############################################################################

  my $req_apicmd4 = new pd_StartDiag;
  $req_apicmd4->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_apicmd4->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_apicmd4->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_apicmd4->{"cgps_cmd_code"}   = 0;   # CGPS Cmd Code
  $req_apicmd4->{"version"}         = 1;   # Version Number
  $req_apicmd4->{"client_id"}       = CLIENT_TYPE_TEST2; # Client Type test2

  ###########################################################################
  # Construct the DM CGPS_INIT_CMD capture request packet
  ###########################################################################

  my $req_apicmd_pkt = $req_apicmd4->SetCaptureCmd();
  printf main::LOGFILE ("Sending CGPS_INIT_CMD: Initialize TEST2 client type %d\n", CLIENT_TYPE_TEST2);

  ###########################################################################
  # Send command request packet, receive command response packet
  ###########################################################################

  my $rsp_apicmd_pkt = $main::qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, TIMEOUT);

  my $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf main::LOGFILE ("DM CGPS_INIT_CMD Response Packet Length: %d\n", $rsp_apicmd_len);

  $req_apicmd4->ParseData($rsp_apicmd_pkt);

  my $status    = $req_apicmd4->GetStatus();
  $main::client_id_test2 = $req_apicmd4->GetClientId();
  printf main::LOGFILE ("Status = %d, Client Id = %d\n\n", $status, $main::client_id_test2);

  sleep(2);
}


################################################################################
# METHOD:
#   register_cgps_client_test2
#
# DESCRIPTION:
#   Registers the cgps client
#
# RETURN VALUE:
#   None
#
################################################################################
sub register_cgps_client_test2
{
  ############################################################################
  # Build a diagnostic request for CGPS_REGISTER_PD_CMD to register the client
  # Registers the client to receive registered PD events
  ############################################################################

  my $req_apicmd5 = new pd_StartDiag;
  $req_apicmd5->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_apicmd5->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_apicmd5->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_apicmd5->{"cgps_cmd_code"}   = 1;   # CGPS Cmd Code
  $req_apicmd5->{"version"}         = 1;   # Version Number
  $req_apicmd5->{"client_id"} = $main::client_id_test2;  # Client ID from Init
  $req_apicmd5->{"reg_type"}        = 0;   # Register

  ############################################################################
  # Construct the DM CGPS_REGISTER_PD_CMD capture request packet
  ############################################################################

  my $req_apicmd_pkt = $req_apicmd5->SetCaptureCmd();
  printf main::LOGFILE ("Sending CGPS_REGISTER_PD_CMD: Register TEST2 client ID %d\n", $main::client_id_test2);

  ############################################################################
  # Send command request packet, receive command response packet
  ############################################################################

  my $rsp_apicmd_pkt = $main::qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, TIMEOUT);

  my $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf main::LOGFILE ("DM CGPS_REGISTER_PD_CMD Response Packet Length: %d\n",
   $rsp_apicmd_len);

  $req_apicmd5->ParseData($rsp_apicmd_pkt);

  my $status    = $req_apicmd5->GetStatus();
  $main::client_id_test2 = $req_apicmd5->GetClientId();
  printf main::LOGFILE ("Status = %d, Client Id = %d\n\n", $status, $main::client_id_test2);

  sleep(2);
}


################################################################################
# METHOD:
#   activate_cgps_client_test2
#
# DESCRIPTION:
#   Activates the cgps client
#
# RETURN VALUE:
#   None
#
################################################################################
sub activate_cgps_client_test2
{
  ############################################################################
  # Build a diagnostic request for CGPS_ACTIVATE_CMD to activate the client
  # Activates the client so that it can be notified of PD events
  ############################################################################

  my $req_apicmd6 = new pd_StartDiag;
  $req_apicmd6->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_apicmd6->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_apicmd6->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_apicmd6->{"cgps_cmd_code"}   = 7;   # CGPS Cmd Code
  $req_apicmd6->{"version"}         = 1;   # Version Number
  $req_apicmd6->{"client_id"}       = $main::client_id_test2;  # Client ID from Init

  ############################################################################
  # Construct the DM CGPS_ACTIVATE_CMD capture request packet
  ############################################################################

  my $req_apicmd_pkt = $req_apicmd6->SetCaptureCmd();
  printf main::LOGFILE ("Sending CGPS_ACTIVATE_CMD: Activate TEST2 client ID %d\n", $main::client_id_test2);

  ############################################################################
  # Send command request packet, receive command response packet
  ############################################################################

  my $rsp_apicmd_pkt = $main::qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, TIMEOUT);

  my $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf main::LOGFILE ("DM CGPS_ACTIVATE_CMD Response Packet Length: %d\n", $rsp_apicmd_len);

  $req_apicmd6->ParseData($rsp_apicmd_pkt);

  my $status    = $req_apicmd6->GetStatus();
  $main::client_id_test2 = $req_apicmd6->GetClientId();
  printf main::LOGFILE ("Status = %d, Client Id = %d\n\n", $status, $main::client_id_test2);
}



################################################################################
# METHOD:
#   setup_cgps_client_test2
#
# DESCRIPTION:
#   Starts the CGPS task, and sets up the client
#
# RETURN VALUE:
#   None
#
################################################################################
sub setup_cgps_client_test2
{
 
  print main::LOGFILE ("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  print main::LOGFILE ("Initialize, Register, & Activate the TEST2 client\n");
  print main::LOGFILE ("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");


  print ("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  print ("Initialize, Register, & Activate the TEST2 client\n");
  print ("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

  print main::LOGFILE ("Initialize, Register, & Activate the TEST2 client\n");
  
  &init_cgps_client_test2;
  &register_cgps_client_test2;
  &activate_cgps_client_test2;
}



################################################################################
# METHOD:
#   setup_cgps_client_xtra
#
# DESCRIPTION:
#   Starts the CGPS task, and sets up the client
#
# RETURN VALUE:
#   None
#
################################################################################
sub setup_cgps_client_xtra
{
  # Start the CGPS task

  print main::LOGFILE ("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  print main::LOGFILE ("Initialize, Register, & Activate the XTRA client\n");
  print main::LOGFILE ("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

  print ("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  print ("Initialize, Register, & Activate the XTRA client\n");
  print ("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

  &init_cgps_client_xtra;
  &register_cgps_client_xtra;
  &activate_cgps_client_xtra;  
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
sub release_cgps_client_xtra
{
  ############################################################################
  # Build a diagnostic request for CGPS_RELEASE_CMD to Release the  XTRA client
  ############################################################################

  my $req_apicmd7 = new pd_EndDiag;
  $req_apicmd7->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_apicmd7->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_apicmd7->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_apicmd7->{"cgps_cmd_code"}   = 9;   # CGPS Cmd Code
  $req_apicmd7->{"version"}         = 1;   # Version Number
  $req_apicmd7->{"client_id"}       = $main::client_id_xtra;  # Client ID recieved from Deactivate

  ############################################################################
  # Construct the DM CGPS_RELEASE_CMD capture request packet
  ############################################################################

  my $req_apicmd_pkt = $req_apicmd7->SetCaptureCmd();
  printf main::LOGFILE ("Sending CGPS_RELEASE_CMD: Release client ID %d\n", $main::client_id_xtra);

  ############################################################################
  # Send command request packet, receive command response packet
  ############################################################################

  my $rsp_apicmd_pkt = $main::qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, TIMEOUT);

  my $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf main::LOGFILE ("DM CGPS_RELEASE_CMD Response Packet Length: %d\n", $rsp_apicmd_len);

  $req_apicmd7->ParseData($rsp_apicmd_pkt);

  my $status    = $req_apicmd7->GetStatus();
  $main::client_id_xtra = $req_apicmd7->GetClientId();
  printf main::LOGFILE ("Status = %d, Client Id = %d\n", $status, $main::client_id_xtra);
  sleep(2);
}


################################################################################
# METHOD:
#   deregister_cgps_client_xtra
#
# DESCRIPTION:
#   deregisters the cgps client
#
# RETURN VALUE:
#   None
#
################################################################################
sub deregister_cgps_client_xtra
{
  ############################################################################
  # Build a diagnostic request for CGPS_REGISTER_PD_CMD to Deregister the
  # client
  ############################################################################

  my $req_apicmd8 = new pd_EndDiag;
  $req_apicmd8->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_apicmd8->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_apicmd8->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_apicmd8->{"cgps_cmd_code"}   = 1;   # CGPS Cmd Code
  $req_apicmd8->{"version"}         = 1;   # Version Number
  $req_apicmd8->{"client_id"}       = $main::client_id_xtra;  # Client ID from Deactivate
  $req_apicmd8->{"reg_type"}        = 1;   # Deregister

  ######################################################################
  # Construct the DM CGPS_REGISTER_PD_CMD capture request packet
  ######################################################################

  my $req_apicmd_pkt = $req_apicmd8->SetCaptureCmd();
  printf main::LOGFILE ("Sending CGPS_REGISTER_PD_CMD: DeRegister client ID %d\n",
   $main::client_id_xtra);

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  my $rsp_apicmd_pkt = $main::qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, TIMEOUT);

  my $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf main::LOGFILE ("DM CGPS_REGISTER_PD_CMD Response Packet Length: %d\n",
   $rsp_apicmd_len);

  $req_apicmd8->ParseData($rsp_apicmd_pkt);

  my $status    = $req_apicmd8->GetStatus();
  $main::client_id_xtra = $req_apicmd8->GetClientId();
  printf main::LOGFILE ("Status = %d, Client Id = %d\n\n", $status, $main::client_id_xtra);

  sleep(2);
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
sub deactivate_cgps_client_xtra
{
  ############################################################################
  # Build a diagnostic request for CGPS_DEACTIVATE_CMD to deactivate the
  # client
  ############################################################################

  my $req_apicmd9 = new pd_EndDiag;
  $req_apicmd9->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_apicmd9->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_apicmd9->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_apicmd9->{"cgps_cmd_code"}   = 8;   # CGPS Cmd Code
  $req_apicmd9->{"version"}         = 1;   # Version Number
  $req_apicmd9->{"client_id"}       = $main::client_id_xtra; # Client Type

  ######################################################################
  # Construct the DM CGPS_DEACTIVATE_CMD capture request packet
  ######################################################################

  my $req_apicmd_pkt = $req_apicmd9->SetCaptureCmd();
  printf main::LOGFILE ("Sending CGPS_DEACTIVATE_CMD: Deactivate client type %d\n",
   CLIENT_TYPE_XTRA);

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  my $rsp_apicmd_pkt = $main::qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, TIMEOUT);

  my $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf main::LOGFILE ("DM CGPS_DEACTIVATE_CMD Response Packet Length: %d\n",
   $rsp_apicmd_len);

  $req_apicmd9->ParseData($rsp_apicmd_pkt);

  my $status    = $req_apicmd9->GetStatus();
  $main::client_id_xtra = $req_apicmd9->GetClientId();
  printf main::LOGFILE ("Status = %d, Client Id = %d\n\n", $status, $main::client_id_xtra);

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
sub release_cgps_client_test2
{
  ############################################################################
  # Build a diagnostic request for CGPS_RELEASE_CMD to Release the client
  ############################################################################

  my $req_apicmd10 = new pd_EndDiag;
  $req_apicmd10->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_apicmd10->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_apicmd10->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_apicmd10->{"cgps_cmd_code"}   = 9;   # CGPS Cmd Code
  $req_apicmd10->{"version"}         = 1;   # Version Number
  $req_apicmd10->{"client_id"}       = $main::client_id_test2;  # Client ID from Deactivate

  ############################################################################
  # Construct the DM CGPS_RELEASE_CMD capture request packet
  ############################################################################

  my $req_apicmd_pkt = $req_apicmd10->SetCaptureCmd();
  printf main::LOGFILE ("Sending CGPS_RELEASE_CMD: Release client ID %d\n", $main::client_id_test2);

  ############################################################################
  # Send command request packet, receive command response packet
  ############################################################################

  my $rsp_apicmd_pkt = $main::qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, TIMEOUT);

  my $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf main::LOGFILE ("DM CGPS_RELEASE_CMD Response Packet Length: %d\n", $rsp_apicmd_len);

  $req_apicmd10->ParseData($rsp_apicmd_pkt);

  my $status    = $req_apicmd10->GetStatus();
  $main::client_id_test2 = $req_apicmd10->GetClientId();
  printf main::LOGFILE ("Status = %d, Client Id = %d\n", $status, $main::client_id_test2);
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
sub deregister_cgps_client_test2
{
  ############################################################################
  # Build a diagnostic request for CGPS_REGISTER_PD_CMD to Deregister the
  # client
  ############################################################################

  my $req_apicmd11 = new pd_EndDiag;
  $req_apicmd11->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_apicmd11->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_apicmd11->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_apicmd11->{"cgps_cmd_code"}   = 1;   # CGPS Cmd Code
  $req_apicmd11->{"version"}         = 1;   # Version Number
  $req_apicmd11->{"client_id"}       = $main::client_id_test2;  # Client ID from Deactivate
  $req_apicmd11->{"reg_type"}        = 1;   # Deregister

  ######################################################################
  # Construct the DM CGPS_REGISTER_PD_CMD capture request packet
  ######################################################################

  my $req_apicmd_pkt = $req_apicmd11->SetCaptureCmd();
  printf main::LOGFILE ("Sending CGPS_REGISTER_PD_CMD: DeRegister client ID %d\n",
   $main::client_id_test2);

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  my $rsp_apicmd_pkt = $main::qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, TIMEOUT);

  my $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf main::LOGFILE ("DM CGPS_REGISTER_PD_CMD Response Packet Length: %d\n",
   $rsp_apicmd_len);

  $req_apicmd11->ParseData($rsp_apicmd_pkt);

  my $status    = $req_apicmd11->GetStatus();
  $main::client_id_test2 = $req_apicmd11->GetClientId();
  printf main::LOGFILE ("Status = %d, Client Id = %d\n\n", $status, $main::client_id_test2);

  sleep(2);
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
sub deactivate_cgps_client_test2
{
  ############################################################################
  # Build a diagnostic request for CGPS_DEACTIVATE_CMD to deactivate the
  # client
  ############################################################################

  my $req_apicmd12 = new pd_EndDiag;
  $req_apicmd12->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_apicmd12->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_apicmd12->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_apicmd12->{"cgps_cmd_code"}   = 8;   # CGPS Cmd Code
  $req_apicmd12->{"version"}         = 1;   # Version Number
  $req_apicmd12->{"client_id"}       = $main::client_id_test2; # Client Type

  ######################################################################
  # Construct the DM CGPS_DEACTIVATE_CMD capture request packet
  ######################################################################

  my $req_apicmd_pkt = $req_apicmd12->SetCaptureCmd();
  printf main::LOGFILE ("Sending CGPS_DEACTIVATE_CMD: Deactivate client type %d\n",
   CLIENT_TYPE_TEST2);

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  my $rsp_apicmd_pkt = $main::qxdm_app->SendDmIcdPacketEx($req_apicmd_pkt, TIMEOUT);

  my $rsp_apicmd_len = length($rsp_apicmd_pkt);
  printf main::LOGFILE ("DM CGPS_DEACTIVATE_CMD Response Packet Length: %d\n",
   $rsp_apicmd_len);

  $req_apicmd12->ParseData($rsp_apicmd_pkt);

  my $status    = $req_apicmd12->GetStatus();
  $main::client_id_test2 = $req_apicmd12->GetClientId();
  printf main::LOGFILE ("Status = %d, Client Id = %d\n\n", $status, $main::client_id_test2);

  sleep(2);
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
sub teardown_cgps_client_test2
{
  print main::LOGFILE ("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  print main::LOGFILE ("DeActivate, DeRegister, & Release the TEST2 client\n");
  print main::LOGFILE ("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

  print  ("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  print  ("DeActivate, DeRegister, & Release the TEST2 client\n");
  print  ("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

  &deactivate_cgps_client_test2;
  &deregister_cgps_client_test2;
  &release_cgps_client_test2;
}


################################################################################
# METHOD:
#   teardown_cgps_client_xtra
#
# DESCRIPTION:
#   Ends the cgps client.
#
# RETURN VALUE:
#   None
#
################################################################################
sub teardown_cgps_client_xtra
{
  print main::LOGFILE ("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  print main::LOGFILE ("DeActivate, DeRegister, & Release the XTRA client\n");
  print main::LOGFILE ("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

  print ("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  print ("DeActivate, DeRegister, & Release the XTRA client\n");
  print ("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

  &deactivate_cgps_client_xtra;
  &deregister_cgps_client_xtra;
  &release_cgps_client_xtra;
}

################################################################################
# METHOD:
#   GpsClientEventsProc
#
# DESCRIPTION:
#   Process the events recieved by the Client
#
# RETURN VALUE:
#   None
#
################################################################################
sub GpsClientEventsProc
{
  #printf main::LOGFILE ("GpsClientEventsProc Start 1:%d 2:%d\n",$main::CurrIndex,$main::PrevIndex);  

  # Process all new items
  for ( my $i2 = $main::PrevIndex + 1; $i2 <= $main::CurrIndex; $i2++ )
  {
    my $Item = $main::qxdm_app2->GetClientItem($main::ReqHandle, $i2 ); # Get Item
    my $ItemKey = $Item->GetItemKeyText();
    my $current_time;
        
    #printf main::LOGFILE ("GpsClientEventsProc Index:%d itemkey:%s\n",$i2 ,$ItemKey);          
        
    if ($ItemKey eq "[01927]")
    {
      printf main::LOGFILE ("Received Event GPS_PD_XTRA_DATA_ACK_RECEIVED!!\n\n");
      $GPS_PD_XTRA_DATA_ACK_RECEIVED = 1;
    }
        
    if ($GPS_PD_SESS_START_RECEIVED == 0)
    {  
      $current_time = time();
      
      if ($ItemKey eq "[00605]")
      {
  	printf main::LOGFILE ("Received event GPS_PD_SESS_START!!\n\n");
 	$GPS_PD_SESS_START_RECEIVED = 1;
      }
	  
    }

  if ($GPS_PD_SESS_START_RECEIVED == 1)
  {
    if ($ItemKey eq "[00601]")
    {
      printf main::LOGFILE ("Received Event GPS_PD_FIX_START!!\n\n");
      $GPS_PD_FIX_START_RECEIVED = 1;
    }
  }    

  if ($GPS_PD_SESS_START_RECEIVED == 1 || $GPS_PD_FIX_START_RECEIVED == 1)
  { 
    $current_time = time();
    if(0)
    {
      printf main::LOGFILE ("Timeout: Event GPS_PD_SESSION_END_MGP_off not received\n\n");
      exit;
    }
    else
    {
      if ($ItemKey eq "[00614]")
      {
         printf main::LOGFILE ("Received Event GPS_PD_POSITION!!\n\n");
      }
      if ($ItemKey eq "[00602]")
      {
        printf main::LOGFILE ("Received Event GPS_PD_FIX_END!!\n\n");
      }
      if ($ItemKey eq "[00411]")
      {
        printf main::LOGFILE ("Received Event GPS_PD_SESSION_END!!\n\n");
        my $PdSessionEndReceived = 1;
      }
      if ($ItemKey eq "[01301]")
      {
        printf main::LOGFILE ("Received Event GPS_LM_MGP_OFF!!\n\n");
      }
    }
  }
  else
  {
    if ($ItemKey eq "[00411]")
    {
     printf main::LOGFILE ("Events received out of order\n");
      exit;
     }
    }	  
  } 
  #printf main::LOGFILE ("GpsClientEventsProc End 1:%d 2:%d\n",$main::CurrIndex,$main::PrevIndex);    
}
############################################
#Return value for this perl module
1;

