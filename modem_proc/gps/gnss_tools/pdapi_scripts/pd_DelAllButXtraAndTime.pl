##########################################################################
# $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/pdapi_scripts/pd_DelAllButXtraAndTime.pl#1 $
# $DateTime: 2016/12/13 07:59:45 $
# $Author: mplcsds1 $
#
# This example demonstrates using a DM Command to delete various CGPS
# related parameters before making a fix by sending a 
# CGPS_SET_PARAM_CMD. This script deletes almanac,ephemeris, position, 
# time, iono, UTC, RTI, Health, SV Direction, SV Steering, and Sensitivity 
# Assiatance in order to do a cold start. This command does not start a fix;
# rather it just sets the parameter that need to be deleted.
#
# Before calling this script, a PDAPI client will need to first be
# initialized (created). Also, this client will then need to register
# for PD events and activate itself before it can set delete all
# related parameters. (Run pd_InitClient.pl)
#
# Usage: perl pd_DelAll.pl 11 (11 is the com port the FFA is connected to)
#
##########################################################################

use vars qw(%opts);

use Win32::OLE;
use Win32::OLE::Variant;

use Getopt::Long;
use pd_DelAll;
#use pd_DelAll_kg_callisto;

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

# Allow user input for comport
if (not defined ($comport)) {
    print("\nUsage: perl pd_DelAll.pl <COM port>\n\n");
    print ("Please enter the COM port the FFA is connected to\n");
    $count = 1;
    chomp ($comport = <STDIN>);
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

######################################################################
# Build a diagnostic request for CGPS_SET_PARAM_CMD: DELETE_PARAMS
######################################################################

  $req_coldstart = new pd_DelAll;
  $req_coldstart->{"cmd_code"}		= 75;	    # Diag Cmd Code
  $req_coldstart->{"subsys_id"}		= 13;	    # Diag Subsystem ID
  $req_coldstart->{"subsys_cmd_code"}	= 100;	    # Diag Subsystem Cmd Code
  $req_coldstart->{"cgps_cmd_code"}	= 12;	    # CGPS Cmd Code
  $req_coldstart->{"version"}		= 1;	    # Version Number
  $req_coldstart->{"client_id"}		= 1001;	    # TEST2
  $req_coldstart->{"param_type"}	= 4;	    # PA_DELETE_PARAMS
  $req_coldstart->{"param_length"}	= 64;	    # 16 x (4 byte fields)
  $req_coldstart->{"delete_bitmap"}	= 0x07B5;   # Delete all but XTRA (Alm, Alm Corr, Health & Time)and CELL-ID(0000|0111|1011|1101)
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
						    # DELETE_CELLDB_INFO = 0X8000
  $req_coldstart->{"gnss_delete_bitmap"} = 0x5B6FB6FB; # Delete All But GLO/BDS/GAL/QZSS Alm & GLO/BDS/GAL/QZSS Alm COrr
                                            # DELETE_EPH_GLO                =  U32BITMASK_BIT(0)
                                            # DELETE_EPH_SBAS               =  U32BITMASK_BIT(1)
                                            # DELETE_ALM_GLO
                                            # DELETE_ALM_SBAS               =  U32BITMASK_BIT(3)

                                            # DELETE_SVDIR_GLO              =  U32BITMASK_BIT(4)
                                            # DELETE_SVDIR_SBAS             =  U32BITMASK_BIT(5)
                                            # DELETE_SVSTEER_GLO            =  U32BITMASK_BIT(6)
                                            # DELETE_SVSTEER_SBAS           =  U32BITMASK_BIT(7)

                                            # DELETE_ALM_CORR_GLO
                                            # DELETE_CLOCK_INFO             =  U32BITMASK_BIT(9)
                                            # DELETE_EPH_BDS                =  U32BITMASK_BIT(10)
                                            # DELETE_ALM_BDS

                                            # DELETE_SVDIR_BDS              =  U32BITMASK_BIT(12)
                                            # DELETE_SVSTEER_BDS            =  U32BITMASK_BIT(13)
                                            # DELETE_ALM_CORR_BDS
                                            # DELETE_GNSS_SV_BLACKLIST_GPS  =  U32BITMASK_BIT(15)

                                            # DELETE_GNSS_SV_BLACKLIST_GLO  =  U32BITMASK_BIT(16)
                                            # DELETE_GNSS_SV_BLACKLIST_BDS  =  U32BITMASK_BIT(17)
                                            # DELETE_GNSS_SV_BLACKLIST_GAL  =  U32BITMASK_BIT(18)
                                            # DELETE_EPH_GAL                =  U32BITMASK_BIT(19)

                                            # DELETE_ALM_GAL
                                            # DELETE_SVDIR_GAL              =  U32BITMASK_BIT(21)
                                            # DELETE_SVSTEER_GAL            =  U32BITMASK_BIT(22)
                                            # DELETE_ALM_CORR_GAL

                                            # DELETE_GNSS_SV_BLACKLIST_QZSS =  U32BITMASK_BIT(24)
                                            # DELETE_EPH_QZSS               =  U32BITMASK_BIT(25)
                                            # DELETE_ALM_QZSS
                                            # DELETE_SVDIR_QZSS             =  U32BITMASK_BIT(27)

                                            # DELETE_SVSTEER_QZSS           =  U32BITMASK_BIT(28)
                                            # DELETE_ALM_CORR_QZSS          
                                            # DELETE_IONO_QZSS              =  U32BITMASK_BIT(30)

  $req_coldstart->{"reserved_delete_bitmap"}	= 0;
  $req_coldstart->{"time_unc"}			= 0;	# Inject Time Uncertainty. # Only applicable if INJECT_TIME_UNC bit = 1
  $req_coldstart->{"pos_unc"}			= 0;	# Inject Position Uncertainty. # Only applicable if INJECT_POS_UNC bit = 1
  $req_coldstart->{"time_offset"}		= 0;	# Add a Time Offset. # Only applicable if INJECT_TIME_OFFSET bit = 1
  $req_coldstart->{"pos_offset"}		= 0;	# Add a Position Offset. # Only applicable if INJECT_POS_OFFSET bit = 1
  $req_coldstart->{"eph_sv_mask"}		= 0;	# Ephemeris SV Mask
  $req_coldstart->{"alm_sv_mask"}		= 0;	# Almanac SV Mask
  $req_coldstart->{"glo_eph_sv_mask"}		= 0;
  $req_coldstart->{"glo_alm_sv_mask"}		= 0;
  $req_coldstart->{"sbas_eph_sv_mask"}		= 0;
  $req_coldstart->{"sbas_alm_sv_mask"}		= 0;
  $req_coldstart->{"delete_celldb_mask"}	= 0xFFFFFFFF;	# Delete Cell Database Mask. # Only applicable if DELETE_CELLDB_INFO bit = 1
  $req_coldstart->{" delete_clock_info_mask"}   = 0;

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

SetColdStartParametersCmd

Syntax:   pd_DelAll [options]

Options:  -help               Display this help message.

Example:  perl pd_DelAll.pl <com port>

END

  print "$usage";
  exit (0);
}
