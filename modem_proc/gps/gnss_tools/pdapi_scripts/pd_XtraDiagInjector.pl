################################################################################
# $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/pdapi_scripts/pd_XtraDiagInjector.pl#1 $
# $Revision: #1 $
# $DateTime: 2016/12/13 07:59:45 $
#
# Parses the test configuration file, Fetches the current XTRA bin file from XTRA
# server as per configuration,Gets SNTP time FROM SNTP server,
# passes the file to Injector.
#
# NOTE : Look at XtraDiagInjector.cfg for TEST CONFIGURATION details
#
# usage: perl XtraDiagClient.pl 
#
#
# Revision History
# when       who    what, where, why
# --------   ---    ----------------------------------------------------------
# 03/21/2012 sp     Moved COM port selection from config file to cmd line param
# 08/23/2010 ssk    New XTRA Diag Client
################################################################################

use strict;

our $client_id_test2;
our $client_id_xtra;
our $qxdm_app;
our $qxdm_app2;
our $clientObjectGlobal;
our $ReqHandle;
our $CurrIndex;
our $PrevIndex;


our $COMPORT;
our $ USE_COARSE_POS; 
our $ALTITUDE; 
our $LONGITUDE; 
our $LATITUDE; 
our $LOC_UNC_HOR; 
our $LOC_UNC_VERT; 
our $CONFIDENC_HOR; 
our $CONFIDENCE_VERT; 
our $ALT_DEF; 
our $MAXSEG_SIZE; 
our $DIAGHDR_SIZE;
our $INJECT_COARSE_TIME; 
our $XTRASNTPURL1; 
our $XTRASNTPURL2; 
our $XTRASNTPURL3; 
our $XTRASNTPURL4; 
our $XTRASNTPURL5; 
our $XTRASNTPURL6; 
our $XTRAHTTPURL; 
our $RESETPHONE;
our $SM_VERSION;  # tells us if this is Gen7 or Gen8 system
our $XTRAFILESIZE;
our $GlobalGPSTime;
our $GlobalUTCTime;

use lib "..\\..\\PdapiScripts";
use lib "..\\..\\pdapi_scripts";

use pd_DelAll;
use pd_SetTimeInfo;
use pd_GpsClientsStartStop;
use pd_ExternCoarsePosInj;
use pd_XtraBinFileInject;
use pd_SntpGetTime;


use Win32::OLE;
use Win32::OLE::Variant;
use POSIX;

use Getopt::Long;
use Cwd;
use pd_StartDiag;
use pd_EndDiag;

# Set the timeout to 2 seconds.
use constant TIMEOUT => 2000;

# CGPS client type
use constant CLIENT_TYPE => 8;

# Clear options for single sv track
use constant SINGLE_SV_CLR => 1;
use constant SINGLE_SV_NOCLR => 2;

use LWP::Simple;
use File::Copy;

my $XtraDataFileDone = 0;




#################################
# Initialize the configuration  #
#################################

$SM_VERSION = 7;

my $Logfile = "XtraInjectorLogs.txt";
open LOGFILE, ">$Logfile" or die "failed to write to savefile $Logfile\n";

print LOGFILE  "Version: pd_XtraDiagClient.pl#1 \n\n";


#####################################
# Sub routines Section              #
#####################################

################################################################################
# METHOD:
#   usage
#
# DESCRIPTION:
#   This subroutine Diag XTRA testers Configuration file
# RETURN VALUE:
#   None
#
################################################################################
sub ParseConfigFile
{ 

 #This is a  simple parser. It iteratively parses each non-commented line .
 #It looks for keys and associated values until end of file is reached

  open(MYINPUTFILE, "<XtraDiagInjector.cfg");
  while(<MYINPUTFILE>)
  {
    # Good practice to store $_ value because
    # subsequent operations may change it.
    my($line) = $_;

    # Good practice to always strip the trailing
    # newline from the line.
    chomp($line);
 
    # Convert the line to upper case.
    $line =~ tr/[a-z]/[A-Z]/;

  
    # Print the line to the screen and add a newline
    #print LOGFILE  "$line \n";
 
    $_  = $line;
 
    if (/^#/)
    {
      #print LOGFILE  " ignoring comment  \n";
    }
    elsif(/MAXSEG_SIZE/)
    {
      $_ =~    s/MAXSEG_SIZE//g;
      $_ =~    s/ //g;
      $_ =~    s/=//g;
      $MAXSEG_SIZE = $_;   
      print LOGFILE "MAXSEG_SIZE in config file is: $MAXSEG_SIZE \n";
      $DIAGHDR_SIZE = 40;      
    }
    elsif(/XTRASNTPURL1/)
    {
      $_ =~    s/XTRASNTPURL1//g;
      $_ =~    s/ //g;
      $_ =~    s/=//g;      
      $_ =~    s/SNTP:\/\///g;
      $XTRASNTPURL1 = $_;  
      print LOGFILE  "XTRASNTPURL1 in config file is: $XTRASNTPURL1 \n";
    }  
    elsif(/XTRASNTPURL2/)
    {
      $_ =~    s/XTRASNTPURL2//g;
      $_ =~    s/ //g;
      $_ =~    s/=//g;      
      $_ =~    s/SNTP:\/\///g;
      $XTRASNTPURL2 = $_;  
      print LOGFILE  "XTRASNTPURL2 in config file is: $XTRASNTPURL2 \n";
    }  
    elsif(/XTRASNTPURL3/)
    {
      $_ =~    s/XTRASNTPURL3//g;
      $_ =~    s/ //g;
      $_ =~    s/=//g;      
      $_ =~    s/SNTP:\/\///g;
      $XTRASNTPURL3 = $_;  
      print LOGFILE  "XTRASNTPURL3 in config file is: $XTRASNTPURL3 \n";
    }  
    elsif(/XTRASNTPURL4/)
    {
      $_ =~    s/XTRASNTPURL4//g;
      $_ =~    s/ //g;
      $_ =~    s/=//g;      
      $_ =~    s/SNTP:\/\///g;
      $XTRASNTPURL4 = $_;  
      print LOGFILE  "XTRASNTPURL4 in config file is: $XTRASNTPURL4 \n";
    }  
    elsif(/XTRASNTPURL5/)
    {
      $_ =~    s/XTRASNTPURL5//g;
      $_ =~    s/ //g;
      $_ =~    s/=//g;      
      $_ =~    s/SNTP:\/\///g;
      $XTRASNTPURL5 = $_;  
      print LOGFILE  "XTRASNTPURL5 in config file is: $XTRASNTPURL5 \n";
    }  
    elsif(/XTRASNTPURL6/)
    {
      $_ =~    s/XTRASNTPURL6//g;
      $_ =~    s/ //g;
      $_ =~    s/=//g;      
      $_ =~    s/SNTP:\/\///g;
      $XTRASNTPURL6 = $_;  
      print LOGFILE  "XTRASNTPURL6 in config file is: $XTRASNTPURL6 \n";
    }  
    elsif(/XTRAHTTPURL/)
    {
      $_ =~    s/XTRAHTTPURL//g;
      $_ =~    s/ //g;
      $_ =~    s/=//g;
      $_ =~    s/\n//g;
      $_ =~    s/\t//g;
      $_ =~    s/\r//g;
    
     $XTRAHTTPURL = $_;   
     if ( $XtraDataFileDone != 1)
     {
       print LOGFILE  "URL used for downloading bin file is: $XTRAHTTPURL \n";
       $XtraDataFileDone = &GetXtraDataFile;
     }
     else
     {
       # no need to do anythign as XTRA file download was successful
     }   
    }
    elsif(/INJECT_COARSE_TIME/)
    {
      $_ =~    s/INJECT_COARSE_TIME//g;
      $_ =~    s/ //g;
      $_ =~    s/=//g;
      $INJECT_COARSE_TIME = $_;   
      print LOGFILE  "INJECT_COARSE_TIME in config file is: $INJECT_COARSE_TIME \n";
    }
    elsif(/USE_COARSE_POS/)
    {
      $_ =~    s/USE_COARSE_POS//g;
      $_ =~    s/ //g;
      $_ =~    s/=//g;
      $USE_COARSE_POS = $_;   
      print LOGFILE  "USE_COARSE_POS in config file  is: $USE_COARSE_POS \n";
    }
    elsif(/ALTITUDE/)
    {
      $_ =~    s/ALTITUDE//g;
      $_ =~    s/ //g;
      $_ =~    s/=//g;
      $ALTITUDE = $_;   
      print LOGFILE  "ALTITUDE in config file is:  $ALTITUDE\n";
    }
    elsif(/LONGITUDE/)
    {
      $_ =~    s/LONGITUDE//g;
      $_ =~    s/ //g;
      $_ =~    s/=//g;
      $LONGITUDE = $_;   
      print LOGFILE  "LONGITUDE in config file  is: $LONGITUDE \n";
    }
    elsif(/LATITUDE/)
    {
      $_ =~    s/LATITUDE//g;
      $_ =~    s/ //g;
      $_ =~    s/=//g;
      $LATITUDE = $_;   
      print LOGFILE  "LATITUDE in config file is: $LATITUDE \n";
    }
    elsif(/LOC_UNC_HOR/)
    {
      $_ =~    s/LOC_UNC_HOR//g;
      $_ =~    s/ //g;
      $_ =~    s/=//g;
      $LOC_UNC_HOR = $_;   
      print LOGFILE  "LOC_UNC_HOR in config file is: $LOC_UNC_HOR \n";
    }
    elsif(/LOC_UNC_VERT/)
    {
      $_ =~    s/LOC_UNC_VERT//g;
      $_ =~    s/ //g;
      $_ =~    s/=//g;
      $LOC_UNC_VERT = $_;   
      print LOGFILE  "LOC_UNC_VERT in config file  is: $LOC_UNC_VERT \n";
    }
    elsif(/CONFIDENC_HOR/)
    {
      $_ =~    s/CONFIDENC_HOR//g;
      $_ =~    s/ //g;
      $_ =~    s/=//g;
      $CONFIDENC_HOR = $_;   
      print LOGFILE  "CONFIDENC_HOR in config file is: $CONFIDENC_HOR \n";
    }
    elsif(/CONFIDENCE_VERT/)
    {
      $_ =~    s/CONFIDENCE_VERT//g;
      $_ =~    s/ //g;
      $_ =~    s/=//g;
      $CONFIDENCE_VERT = $_;   
      print LOGFILE  "CONFIDENCE_VERT in config file is: $CONFIDENCE_VERT \n";
    }
    elsif(/ALT_DEF/)
    {
      $_ =~    s/ALT_DEF//g;
      $_ =~    s/ //g;
      $_ =~    s/=//g;
      $ALT_DEF = $_;   
      print LOGFILE  "ALT_DEF in config file is: $ALT_DEF \n";
    }
    elsif(/RESETPHONE/)   
    {
      $_ =~    s/RESETPHONE//g;
      $_ =~    s/ //g;
      $_ =~    s/=//g;
      $RESETPHONE = $_;   
      print LOGFILE  "RESETPHONE FLAG in config file is: $RESETPHONE \n";   
    }
    else
    {   
      $_ =~    s/\s//g; 
      $_ =~    s/ //g;
      $_ =~    s/=//g;
      if ($_)
      {
        print LOGFILE  "Invalid Syntax in XTRA config file \n";   
        print LOGFILE  "$_ \n";
      }
    }
  }
 
 close MYINPUTFILE;
} # End of ParseConfigFile

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
  print main::LOGFILE ("~~~~~~~~~~~~~~\n");
  print main::LOGFILE  ("Reseting Phone\n");
  print main::LOGFILE  ("~~~~~~~~~~~~~~\n");
  
  print main::LOGFILE ("~~~~~~~~~~~~~~\n");
  print main::LOGFILE ("Reseting Phone\n");
  print main::LOGFILE ("~~~~~~~~~~~~~~\n");
  
  $main::qxdm_app->OfflineDigital();
  $main::qxdm_app->ResetPhone();

  # Wait for phone to disconnect (or 5s incase it was missed)
  my $i;
  for($i = 0; ($i < 5), ($main::qxdm_app->{IsPhoneConnected} == 1); $i++)
  {
    sleep(1);
  }

  print main::LOGFILE "Trying to Reconnect to the phone";
  print "Trying to Reconnect to the phone";
  while ($main::qxdm_app->{IsPhoneConnected} == 0)
  {
    print main::LOGFILE ".";
    print ".";
    sleep(1);
  }

  print main::LOGFILE "\n";
  sleep(5);
}


################################################################################
# METHOD:
#   usage
#
# DESCRIPTION:
#   This subroutine resets the phone
#
# RETURN VALUE:
#   None
#
################################################################################
sub reset_phone
{ 

  ##############################################################################
  # Delete all of the CGPS parameters (alm, eph, XTRA Data etc.)
  ##############################################################################
  &cgps_delete;
  
  ##############################################################################
  # Reset the device if needed
  ##############################################################################
  if ($RESETPHONE == 1)
  { 
    &reset_device;
  }
} # End of reset_phone


################################################################################
# METHOD:
#   usage
#
# DESCRIPTION:
#   This subroutine downloads the XTRA.bin file
#
# RETURN VALUE:
#   None
#
################################################################################
sub GetXtraDataFile
{ 
  my $content;
  print LOGFILE ("Downloading XTRA data file from  = %s \n", $XTRAHTTPURL);

  # Download latest XTRA file

  chomp($XTRAHTTPURL);
  $XTRAHTTPURL = lc($XTRAHTTPURL);
  unless (defined ($content = get "$XTRAHTTPURL"))
  {
    warn "could not get file from $XTRAHTTPURL \n \n";
    return -1;
  }

  # save to disk as "xtra.bin" since this is the file used for both XTRA1 and XTRA2
  my $savefile = "xtra.bin";
  open SAVEFILE, ">$savefile" or die "failed to write to savefile $savefile\n";
  binmode(SAVEFILE);
  print SAVEFILE "$content";
  close SAVEFILE;

  # check for bad file download
  my $Localfilesize = -s $savefile ;

  print LOGFILE  "\nValidating download $savefile filesize = $Localfilesize \n";
  open SAVEFILE, "$savefile" or die "Error opening $savefile for output.\n";
  my $savefile_opened = 1;
  if ($savefile_opened == 1)
  {
    print LOGFILE  "\n$savefile file check passed\n";
    close SAVEFILE;  
    return 1;
  }
  else
  {
    return -1;
  }

} # End of GpsInvokeClientStart


################################################################################
# METHOD:
#   usage
#
# DESCRIPTION:
#   This subroutine Init and register for the client, delete all old almanac information
#
# RETURN VALUE:
#   None
#
################################################################################
sub GpsInvokeClientStart
{ 

  &GpsClientStart;

  print LOGFILE ("Starting Clients XtraClientId =" . sprintf("%d",$client_id_xtra). ",test2_id =". sprintf("%d",$client_id_test2) . "\n\n"); 

} # end sub GpsInvokeClientStart


################################################################################
# METHOD:
#   usage
#
# DESCRIPTION:
# Stop the client
#
# RETURN VALUE:
#   None
#
################################################################################
sub GpsClientsStop
{ 
  &GpsClientStop;
  print LOGFILE  "\n Clients Stopped \n";
} # end  of sub GpsClientsStop



################################################################################
# METHOD:
#   usage
#
# DESCRIPTION:
#   This subroutine inject the Time into the Gpsone Engine.
#
# RETURN VALUE:
#   None
#
################################################################################
# Start time injection 
sub XtraTimeInject
{
  print LOGFILE  "\n\n---------- Starting Xtra Time Injection ----------\n\n";

#############################################################################
# Build a diagnostic request for CGPS_INJECT_TIME_CMD to inject external time 
#############################################################################
  my $req_set_time_info;
  #my $time_ms = (time() - 315964800) * 1000.0;  #   unix timestamp + sec btwn Jan 1,1970 & Jan 6,1980
  #my $upper1 = $time_ms / (65536.0 * 65536.0);
  #my $upper = floor($upper1);
  #my $lower = $time_ms - (65536*65536)*$upper;
  
  #$req_set_time_info->{"time_lower"}      = $lower;     # lower 32 bits of time. Time is a uint64 in msec
  #$req_set_time_info->{"time_upper"}      = $upper;     # upper 32 bits of time

  my $time_ms = ($GlobalGPSTime * 1000.0 );  #    sec From Jan 1,1900
  my $upper11 = $time_ms / (65536.0 * 65536.0);
  my $upper1 = floor($upper11);
  my $lower1 = $time_ms - (65536*65536)*$upper1;
  my $tunc = 1000; # 7500;   
  
  $req_set_time_info = new pd_SetTimeInfo;
  $req_set_time_info->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_set_time_info->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_set_time_info->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_set_time_info->{"cgps_cmd_code"}   = 17;  # CGPS Cmd Code = PDSM_PD_CMD_INJECT_TIME
  $req_set_time_info->{"version"}         = 1;   # Version Number
  $req_set_time_info->{"client_id"}       = $client_id_test2; # will be returned by PDAPI upon calling this script
  
  
  #$req_set_time_info->{"time_lower"}      = $GlobalGPSTime*1000;     # lower 32 bits of time. Time is a uint64 in msec
  #$req_set_time_info->{"time_upper"}      = 0;     # upper 32 bits of time


  $req_set_time_info->{"time_lower"}      = $lower1;     # lower 32 bits of time. Time is a uint64 in msec
  $req_set_time_info->{"time_upper"}      = $upper1;     # upper 32 bits of time
    
  $req_set_time_info->{"time_unc"}        = $tunc;      # time uncertainty
  $req_set_time_info->{"time_src"}        = 1;   # 0=ref to Host time
                                                 # 1=ref to Network time
                                                 
  $req_set_time_info->{"is_gps"}          = 1;   # 0=ref to UTC time
                                                 # 1=ref to GPS time
                                                 # valid only if time_src is set to Network time
  $req_set_time_info->{"disc_count"}           = 0;     # discontinuity count
  $req_set_time_info->{"force_flag"}           = 0;     # 0= don't force, 
                                                                                                        #1= force acceptance

  ######################################################################
  # Construct the DM CGPS_INJECT_TIME_CMD capture request packet
  ######################################################################

  my $req_set_time_pkt = $req_set_time_info->SetCaptureCmd();
  #print ("Sending CGPS_INJECT_TIME_CMD: Inject Time client ID %d\n", $client_id_test2);
  printf LOGFILE  ("Sending CGPS_INJECT_TIME_CMD: Inject Time client ID %d\n", $client_id_test2);  


  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  my $rsp_set_time_pkt_var = $qxdm_app->SendDmIcdPacketEx( $req_set_time_pkt, TIMEOUT);
  my $rsp_set_time_len_var = length($rsp_set_time_pkt_var);
  print LOGFILE ("DM CGPS_INJECT_TIME_CMD Response Packet Length :" . sprintf("%d",$rsp_set_time_len_var) . "\n");
 
  printf LOGFILE   ("\nInjected data: time=" . sprintf("%0.1f",$upper1) . ", tunc=" . sprintf("%6.1f",$tunc) . ", timeSrc=" . sprintf("%1d",$req_set_time_info->{"time_src"}) . ", force=" . sprintf("%1d",$req_set_time_info->{"force_flag"}) . "\n");

  $req_set_time_info->ParseData($rsp_set_time_pkt_var);

  printf LOGFILE  ( "\nDM Time Injection Response len: %d\n ", $rsp_set_time_pkt_var );
  my $status    = $req_set_time_info->GetStatus();
  my $client_id = $req_set_time_info->GetClientId();  
   
  print LOGFILE ("Time inject Status =". sprintf("%d",$status) ."Client Id =". sprintf("%d",$req_set_time_info->GetClientId() ). "\n\n");    

  sleep(2);
  
  print LOGFILE  "\n\n---------- End of Xtra Time Injection ---------- \n\n";  
}  # end sub XtraTimeInject


################################################################################
# METHOD:
#   usage
#
# DESCRIPTION:
#   This subroutine injects coarse position into gpsone
#
# RETURN VALUE:
#   None
#
################################################################################
sub XtraCoarsePosInject 
{ 
  print LOGFILE  "\nCoarse Position Inject \n";
 
  sleep(2);

  my $opt_field_mask = 0x6; #PDSM_PD_EXTERN_COARSE_POS_GPS_TIME_VALID |
                            #PDSM_PD_EXTERN_COARSE_POS_LAT_LONG_VALID |
                            #PDSM_PD_EXTERN_COARSE_POS_ALT_VALID

  my $time_info_type = 0;   #PDSM_PD_EXTERN_COARSE_POS_TIME_GPS
  my $timestamp_sec = (time() - 315964800);# * 1000.0;  # unix timestamp + sec btwn Jan 1,1970 & Jan 6,1980; 
  my $source = 0; #PDSM_PD_EXTERN_COARSE_POS_SOURCE_UNKNOWN;

  #my $latitude = 30.0* 4294967296.0 / 180.0;
  my $latitude = $LATITUDE; 
  $latitude &= 0xFFFFFFFF;        #needed for uint32toByte() when the number is negative 

  #my $longitude = -90.1* 2147483648.0 / 180.0;
  my $longitude = $LONGITUDE;
  $longitude &= 0xFFFFFFFF;  #needed for uint32toByte() when the number is negative
   
   
  my $alt_def = 0; #PDSM_PD_EXTERN_COARSE_POS_ALT_HAE;

  #my $altitude = 44;
  my $altitude = $ALTITUDE;
  $altitude &= 0xFFFF;  
   
  #my $loc_unc_vert = 10;
  #my $confidence_hor = 39;
  #my $confidence_vert = 39;
  #my $loc_unc_hor = 3333;    #Unit in meter. Punc = 10000/ 3 = 3333(HEPE)

  my $loc_unc_vert = $LOC_UNC_VERT;
  my $loc_unc_hor = $LOC_UNC_HOR;    #Unit in meter. Punc = 10000/ 3 = 3333(HEPE)
  my $confidence_hor = $CONFIDENC_HOR;
  my $confidence_vert = $CONFIDENCE_VERT;
 
  #############################################################################
  # Build a diagnostic request for CGPS_EXTERN_COARSE_POS_INJ_CMD to inject 
  # external coarse position 
  #############################################################################

  my $req_extern_coarse_pos_info = new pd_ExternCoarsePosInj;
  $req_extern_coarse_pos_info->{"cmd_code"}        = 75;  # Diag Cmd Code
  $req_extern_coarse_pos_info->{"subsys_id"}       = 13;  # Diag Subsystem ID
  $req_extern_coarse_pos_info->{"subsys_cmd_code"} = 100; # Diag Subsystem Cmd Code
  $req_extern_coarse_pos_info->{"cgps_cmd_code"}   = 18;  # PDSM_PD_CMD_INJECT_POS  CGPS_EXTERN_COARSE_POS_INJ_CMD
  $req_extern_coarse_pos_info->{"version"}         = 1;          # Version Number
  $req_extern_coarse_pos_info->{"client_id"}       = $client_id_test2; #1001  will be returned by PDAPI upon calling this script

  $req_extern_coarse_pos_info->{"opt_field_mask"}  = $opt_field_mask;    
  $req_extern_coarse_pos_info->{"time_info_type"}  = $time_info_type;    
  $req_extern_coarse_pos_info->{"timestamp_sec"}   = $timestamp_sec; 
  $req_extern_coarse_pos_info->{"source"}          = $source;
  $req_extern_coarse_pos_info->{"latitude"}        = $latitude;   
  $req_extern_coarse_pos_info->{"longitude"}       = $longitude;        
  $req_extern_coarse_pos_info->{"loc_unc_hor"}     = $loc_unc_hor; 
  $req_extern_coarse_pos_info->{"alt_def"}         = $alt_def;
  $req_extern_coarse_pos_info->{"altitude"}        = $altitude;   
  $req_extern_coarse_pos_info->{"loc_unc_vert"}    = $loc_unc_vert;     
  $req_extern_coarse_pos_info->{"confidence_hor"}  = $confidence_hor; 
  $req_extern_coarse_pos_info->{"confidence_vert"} = $confidence_vert; 

  ######################################################################
  # Construct the DM CGPS_INJECT_TIME_CMD capture request packet
  ######################################################################

  my $req_extern_coarse_pos_pkt = $req_extern_coarse_pos_info->SetCaptureCmd();
  #print ("Sending CGPS_EXTERN_COARSE_POS_INJ_CMD: client ID %d\n", $client_id_test2);
  printf LOGFILE ("Sending CGPS_EXTERN_COARSE_POS_INJ_CMD: client ID %d\n", $client_id_test2);
  

  ######################################################################
  # Send command request packet, receive command response packet
  ######################################################################

  my $rsp_extern_coarse_pos_pkt_var = $qxdm_app->SendDmIcdPacketEx( $req_extern_coarse_pos_pkt, TIMEOUT);
  my $rsp_extern_coarse_pos_len_var = length($rsp_extern_coarse_pos_pkt_var);
  print LOGFILE ("DM_CGPS_EXTERN_COARSE_POS_INJ_CMD Response Packet Length: " . sprintf("%d",$rsp_extern_coarse_pos_len_var)  . "\n"); 
 
  $req_extern_coarse_pos_info->ParseData($rsp_extern_coarse_pos_pkt_var);

  my $status    = $req_extern_coarse_pos_info->GetStatus();
  my $client_id = $req_extern_coarse_pos_info->GetClientId();
  print LOGFILE ("Coarse POS inject Status =". sprintf("%d",$status) ."Client Id =". sprintf("%d",$req_extern_coarse_pos_info->GetClientId()). "\n\n");  
  
} # end of sub XtraCoarsePosInject


################################################################################
# METHOD:
#   usage
#
# DESCRIPTION:
#   This subroutine Injects the XTRA Data
#
# RETURN VALUE:
#   None
#
################################################################################
sub InjectXtraData
{ 

  #Let us initialize, Register, & Activate the client
  &GpsInvokeClientStart;

  $PrevIndex = $qxdm_app2->GetClientItemCount( $ReqHandle ) - 1;   
 
  #lets get the  time from the XTRA SNTP server
  &Obtain_NTP_Time;

  #Inject coarse time;
  if (lc($INJECT_COARSE_TIME) eq "true")
  {
    #lets call the time injector function
    &XtraTimeInject;
  }
  
  #lets call the Coarse Position injector
  &XtraCoarsePosInject;

  #lets call the XTRA bin file injector 
  # we call this in a loop for the different XTRA URLs in the config file.
  my $i;
  for($i = 0; $i < 1; $i++)
  {
    my $filename = "xtra.bin"; #'/path/to/your/file.doc';
    $XTRAFILESIZE = -s $filename;
    printf LOGFILE ("\n Size of Xtra Bin file is: %d \n", $XTRAFILESIZE);  
    
    &GpsXtraBinFileSend;
  }

  &GpsClientEventsProc;
  $PrevIndex = $CurrIndex;
  $CurrIndex = $qxdm_app2->GetClientItemCount( $ReqHandle ) - 1; # Get index of last item in client

} # End of InjectXtraData


################################################################################
# METHOD:
#   usage
#
# DESCRIPTION:
#   Starts tests
#
# RETURN VALUE:
#   None
#
################################################################################
sub GpsInit
{

  # get the current working directory.  We want to change back to this directory
  # after changing to other directories.
  my $working_dir = getcwd();

  ################################################################################
  # Create QXDM instance
  ################################################################################

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

  $main::qxdm_app2 = $main::qxdm_app->GetIQXDM2();
  if ($main::qxdm_app2 == 0)
  {
    print main::LOGFILE "QXDM does not support required interface\n";
    print "QXDM does not support required interface\n";
    sleep(5);
    die;
  }

  # Register the QXDM client.
  $ReqHandle = $main::qxdm_app2->RegisterQueueClient( 256 );
  if ($ReqHandle == 0xFFFFFFFF)
  {
     print main::LOGFILE "Unable to register as QXDM client\n";
     print "Unable to register as QXDM client\n";
     sleep(5);
     die;
  }

  # Get a configuration object.
  my $clientObject = $main::qxdm_app2->ConfigureClientByKeys( $ReqHandle );
  if ($clientObject == 0)
  {
     print main::LOGFILE "Unable to register as QXDM client\n";
     print "Unable to register as QXDM client\n";
     sleep(5);
     die;
  }

################################################################################
# Register for Required Events
################################################################################

#$clientObject->AddEvent( 605 );       #EVENT_GPS_PD_SESS_START
#$clientObject->AddEvent( 601 );       #EVENT_GPS_PD_FIX_START
#$clientObject->AddEvent( 614 );       #EVENT_GPS_PD_POSITION
#$clientObject->AddEvent( 602 );       #EVENT_GPS_PD_FIX_END
#$clientObject->AddEvent( 411 );       #EVENT_GPS_PD_SESS_END

$clientObject->CommitConfig();

################################################################################
#  Open QXDM Connection by setting the COM port from command line.
################################################################################

  # COM Port value should be supplied as first argument to this script
  $main::qxdm_app->{COMPort} = $main::COMPORT;
 
  # Wait until phone is connected
  while ($main::qxdm_app->{IsPhoneConnected} == 0)
  {
    print main::LOGFILE "Unable to Connect to the Phone. Retrying...\n";
    print "Unable to Connect to the Phone. Retrying...\n";
    sleep(1);
  }

  $main::qxdm_app->SendScript("logging on");
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

  &GpsInit;
  
  printf main::LOGFILE ("\nDeleting All GPS Parameters using SET PARAM\n");

  my $req_coldstart = new pd_DelAll;
  $req_coldstart->{"cmd_code"}        = 75;       # Diag Cmd Code
  $req_coldstart->{"subsys_id"}       = 13;       # Diag Subsystem ID
  $req_coldstart->{"subsys_cmd_code"} = 100;      # Diag Subsystem Cmd Code
  $req_coldstart->{"cgps_cmd_code"}   = 12;       # CGPS Cmd Code
  $req_coldstart->{"version"}         = 1;        # Version Number
  $req_coldstart->{"client_id"}       = $main::client_id_test2;     # TEST2
  $req_coldstart->{"param_type"}      = 4;        # PA_DELETE_PARAMS
  $req_coldstart->{"param_length"}    = 32;       # 8 x (4 byte fields)
  $req_coldstart->{"delete_bitmap"}   = 0x87FF;   # Delete All including XTRA, POS, Eph etc..
                                                  #  (1000|0111|1111|1111
                                                  #  = 0x87FF = 34815)
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

  $req_coldstart->{"time_unc"}        = 0;        # Inject Time Uncertainty
               				                            #  Only applicable if
                                                  #  INJECT_TIME_UNC bit = 1
  $req_coldstart->{"pos_unc"}         = 0;        # Inject Position Uncertainty
				                                          #  Only applicable if
                                                  #  INJECT_POS_UNC bit = 1
  $req_coldstart->{"time_offset"}     = 0;        # Add a Time Offset
				                                          # Only applicable if
                                                  #  INJECT_TIME_OFFSET bit = 1
  $req_coldstart->{"pos_offset"}      = 0;        # Add a Position Offset
				                                          # Only applicable if
                                                  #  INJECT_POS_OFFSET bit = 1
  $req_coldstart->{"eph_sv_mask"}     = 0;        # Ephemeris SV Mask
  $req_coldstart->{"alm_sv_mask"}     = 0;        # Almanac SV Mask
  $req_coldstart->{"delete_celldb_mask"} = 0xFFFFFFFF;  # Delete Cell Database
                                                  #  Mask
                                                  # Only applicable if DELETE_CELLDB_INFO bit = 1

  ############################################################################
  # Construct the DM CGPS_SET_PARAM_CMD capture request packet
  ############################################################################

  my $req_coldstart_pkt = $req_coldstart->SetCaptureCmd();

  ############################################################################
  # Send command request packet, receive command response packet
  ############################################################################

  my $rsp_coldstart_pkt = $main::qxdm_app->SendDmIcdPacketEx($req_coldstart_pkt,2000);

  my $rsp_coldstart_len = length($rsp_coldstart_pkt);
  printf main::LOGFILE ("DM CGPS_SET_PARAM_CMD Response Packet Length: %d\n",
   $rsp_coldstart_len);

  $req_coldstart->ParseData($rsp_coldstart_pkt);

  ############################################################################
  my $status    = $req_coldstart->GetStatus();
  my $client_id = $req_coldstart->GetClientId();
  printf main::LOGFILE ("Status = %d, Client Id = %d\n\n", $status, $client_id);
} # End of sub  cgps_delete


################################################################################
# Main Body of The XTRA diag Client
################################################################################

#binmode(LOGFILE);
print LOGFILE "***************START OF XTRA INJECTOR LOGS *********************\n\n";
  
#Parse the  Config File;
&ParseConfigFile;

$main::COMPORT     = $ARGV[0];

# Allow user input for comport
if (not defined ($main::COMPORT))
{
  print("\nUsage: perl pd_XtraDiagInjector.pl <COM Port>\n\n");
  print ("Please enter the COM port the FFA is connected to\n");
  chomp ($main::COMPORT = <STDIN>);
  print("\n");
}
print  LOGFILE "COMPORT entered by user is: $main::COMPORT \n";

#reset the phone;
&reset_phone;

#Inject Xtra Data;
&InjectXtraData;

print LOGFILE "\n\n**********************     END OF XTRA INJECTOR LOGS   *******************\n";

close LOGFILE;
#################END OF FILE###################


