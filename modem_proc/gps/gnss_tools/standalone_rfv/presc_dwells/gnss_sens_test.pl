#========================================================================================
#$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/standalone_rfv/presc_dwells/gnss_sens_test.pl#1 $ $DateTime: 2016/12/13 07:59:45 $ $Author: mplcsds1 $
#========================================================================================
#  Copyright (C) 2004 Qualcomm, Inc.
#
#                  Qualcomm, Inc.
#                  675 Campbell Technology Parkway
#                  Campbell, CA  95008
#
# This program is confidential and a trade secret of Qualcomm, Inc.  The
# receipt or possession of this program does not convey any rights to reproduce 
# or disclose its contents or to manufacture, use or sell anything that this 
# program describes in whole or in part, without the express written consent of
# Qualcomm, Inc.  The recipient and/or possessor of this program shall not 
# reproduce or adapt or disclose or use this program except as expressly 
# allowed by a written authorization from Qualcomm, Inc.
#
#========================================================================================
# Description:
# This script starts QXDM, sets up a connection to the phone and runs "StartPrescDwell.pl"
# which in turn issues one dwell cmd per call. Upon completion, the isf file is parsed 
# using APEX. The resulting .txt file is parsed using "ProcDwellResults.pl" script.
#
# Usage:
# perl StartSensTest.pl <Com Port Number>
#
#========================================================================================

use vars qw(%opts);
use Cwd;
use Win32::OLE;
use Win32::OLE::Variant;
use Getopt::Long;
use HelperFunctions;

$C_MSTOL1HZ =(1.0/(299792458.0/(1540.0*1000.0*1023.0)));

my $com_port = int $ARGV[0]; 
# Default dwell params
my $numdwells = 3;
my $gps_enable = 1;
my $gps_rxd_flag = 0;
my $glo_enable = 1;
my $gps_dopp_hz = 900*5.25;
my $glo_dopp_hz = 900*5.25;
my $gps_cp_win_sz_chips = 32;
my $glo_cp_win_sz_chips = 32;


GetOptions(
           'gps_enable=i'   => \$gps_enable,
           'gps_rxd_flag=i' => \$gps_rxd_flag,
           'gps_dopp_hz=f'  => \$gps_dopp_hz,
           'gps_cp_win=i' => \$gps_cp_win_sz_chips,

           'glo_enable=i'   => \$glo_enable,
           'glo_dopp_hz=f'  => \$glo_dopp_hz,
           'glo_cp_win=i' => \$glo_cp_win_sz_chips,

           'help', \$help); 

&usage() if ($help);

my %acquire_dwell = (
            gps_enable    => $gps_enable,
            gps_rxd_flag  => $gps_rxd_flag,
            gps_dopp_hz   => $gps_dopp_hz,
            glo_enable    => $glo_enable,
            glo_dopp_hz   => $glo_dopp_hz,
);

if($gps_enable > 0)
{
  $gps_enable = 1;
}

if($glo_enable > 0)
{
  $glo_enable = 1;
}

#===========================================================================
# Create QXDM instance
#===========================================================================
my $qxdm_app = new Win32::OLE 'QXDM.Application';
if(!$qxdm_app)
{
   print "ERROR: Unable to invoke the QXDM application.\n";
   die;
}

#===========================================================================
# Open QXDM Connection by setting the com port. 
#===========================================================================
$qxdm_app->{COMPort} = $com_port;

# Wait until phone is connected
while( $qxdm_app->{IsPhoneConnected} == 0 ){
   print "Unable to Connect to the Phone. Retrying.\n";
   sleep(1);
}

SetQXDM ( $QXDM );

my $cwd = getcwd();
# Replace forward slashes with back slashes since "save as text" feature in APEX fails with forward slashes. APEX will fix this
# in future.
$cwd =~ s/\//\\/g;

# Get file name from script folder path
my $isf_filename = GenerateFileName( "", "" );

my $isf_handle = $qxdm_app->CreateItemStore($isf_filename . ".isf");
# Save current item store. We will over-write this file later. This is to simply trigger start of a fresh isf collect.
SaveISF( );
$qxdm_app->CloseItemStore($isf_handle);

my $isf_handle = $qxdm_app->CreateItemStore($cwd . $isf_filename . ".isf");
# says "run the perl program CGPS_ModeSwitch.pl, additionally checking the ..\CGPS_ModeSwitch directory for modules"
my $result = qx{perl -I ..\\cgps_mode_switch ..\\cgps_mode_switch\\CGPS_ModeSwitch.pl $com_port ON};
chomp $result;
printf( "$result \n" );

if( $result =~ /SARF mode entered!+/ )
{
}
else
{
      # Make one more attempt
      my $result = qx{perl -I ..\\CGPS_ModeSwitch ..\\CGPS_ModeSwitch\\CGPS_ModeSwitch.pl $com_port ON};
      chomp $result;
      printf( "$result \n" );
      if( $result =~ /SARF mode entered!+/ )
      {
      }
      else
      {
       printf( "*********Unsucessful Mode Switch. Aborting." );
       exit();
      }
}

acquire_signal();
$gps_dwell_cnt = $numdwells;
$glo_dwell_cnt = $numdwells;

while( ( $gps_dwell_cnt > 0 && $gps_enable == 1 ) || ( $glo_dwell_cnt > 0 && $glo_enable == 1 ) )
{
	if( $gps_dwell_cnt )
	{
		$gps_sens_dwells_pending = 1;
	}
	else
	{
		$gps_sens_dwells_pending = 0;
	}
	
	if( $glo_dwell_cnt )
	{
		$glo_sens_dwells_pending = 1;
	}
	else
	{
		$glo_sens_dwells_pending = 0;
	}

      #For Sensitivty tests need to randomize the starting bin
      my $dopp_randomizer = rand(25) - 12.5; #Allow for neg doppler randomization
      
      $gps_dopp_adjusted_hz = $gps_dopp_hz + $dopp_randomizer;
      $glo_dopp_adjusted_hz = $glo_dopp_hz + $dopp_randomizer;      
      
	printf ("\nStarting Dopp (Hz): %.2f\n", $gps_dopp_adjusted_hz);

	($gps_dwell_status, $glo_dwell_status) = start_sens_dwell($gps_sens_dwells_pending, $glo_sens_dwells_pending);
	printf "($gps_dwell_status, $glo_dwell_status)";

	if( $gps_dwell_status eq "GPS Dwell Done" )
	{
		$gps_dwell_cnt--;
		printf "\nGPS Dwell #" . ($numdwells - $gps_dwell_cnt) . " of " . $numdwells . " Done";
	}
	elsif( $gps_dwell_status eq "GPS Dwell Tunc" )
	{
		$need_to_acquire = 1;
		printf "\nGPS Dwell #" . ($numdwells - $gps_dwell_cnt) . " of " . $numdwells . ": Tunc too big";
	}
	elsif( $gps_dwell_status eq "GPS Dwell NA" )
	{
		# Nothing to do
		printf "\nGPS Dwell not requested";
	}
	elsif( $gps_dwell_status eq "GPS Dwell Rejected" )
	{
		# Nothing to do
		printf "\nGPS Dwell #" . ($numdwells - $gps_dwell_cnt) . " of " . $numdwells . " request rejected";
	}

	if( $glo_dwell_status eq "GLO Dwell Done" )
	{
		$glo_dwell_cnt--;
		printf "\nGLO Dwell #" . ($numdwells - $glo_dwell_cnt) . " of " . $numdwells . " Done";
	}
	elsif( $glo_dwell_status eq "GLO Dwell Tunc" )
	{
		$need_to_acquire = 1;
		printf "\nGLO Dwell #" . ($numdwells - $glo_dwell_cnt) . " of " . $numdwells . ": Tunc too big";
	}
	elsif( $glo_dwell_status eq "GLO Dwell NA" )
	{
		# Nothing to do
		printf "\nGLO Dwell not requested";
	}
	elsif( $glo_dwell_status eq "GLO Dwell Rejected" )
	{
		# Nothing to do
		printf "\nGLO Dwell #" . ($numdwells - $glo_dwell_cnt) . " of " . $numdwells . " request rejected";
	}

	if( $need_to_acquire == 1 )
	{
		acquire_signal();
		$need_to_acquire = 0;
	}
}

# Save current item store
SaveISF( );
$qxdm_app->CloseItemStore($isf_handle);
print "\nSaved item store to:\n.\\" . $isf_filename . ".isf\n";
$qxdm_app = NULL;

$isf_Full_Path = $isf_filename . ".isf";
system("\nperl parseQCAT.pl $isf_Full_Path");

$isf_txt_Full_Path = $isf_filename . ".txt";
system("\nperl ProcDwellResults.pl 0 < $isf_txt_Full_Path");

# Save the curent ISF
sub SaveISF
{
   $qxdm_app->SaveItemStore( $cwd . "\\" . $isf_filename . ".isf" );
   $RC = true;
   return $RC;
}

# METHOD:
#   usage
#
# DESCRIPTION:
#   Display usage information.
#
# RETURN VALUE:
#   None
#
sub usage
{
   my $usage = <<END;

Start Sensitivity Test.

Syntax:   perl StartSensTest.pl COM_PORT [OPTIONS]

          COM_PORT:         QPST Connection Port Number.

OPTIONS:  -h                Display this help message.
          -numdwells        Number of dwells. Default = 100
          -sv:              SV id (0-32, 0=codeless). Default = 1
          -sm:              Search Mode (0/1/2 => 10/20/160 ms). Default = 1
          -postd:           PostDets. Default = 600
          -dopp:            Doppler (m/s). Default = 0 m/s

Default values from perl script take effect when value is not provided.
 
Example 1:  perl StartSensTest.pl 7 -numdwells 50 -sv 2 -sm 2 -postd 50
This does 50 presc dwells on SV 2, with search mode 2 and postdet 50. All other fields are default.

Example 2:  perl StartSensTest.pl 7

END

   print "$usage";
   exit (0);
}

sub acquire_signal()
{
# Disable cmd line display of dwell results. 
	printf "\nStarting peak acquisition...\n";
	my $as_presc_done = 1;
	my $string1 = "-gps_enable $gps_enable ";

	my $string2 = "-gps_rxd_flag $gps_rxd_flag ";
	my $string3 = "-gps_dopp_hz $gps_dopp_hz ";
	my $string4 = "-glo_enable $glo_enable ";
	my $string5 = "-glo_dopp_hz $glo_dopp_hz ";

	my $string6 = "-gps_ctrl_flag 0 -gps_cp_win 1023 -gps_srch_mode 3 -gps_postd 100 ";
	my $string7 = "-glo_ctrl_flag 0 -glo_cp_win 511 -glo_srch_mode 18 -glo_postd 100 ";

	my $as_presc_string = "perl gnss_presc_dwell.pl $com_port 0 " . $string1 . $string2 . $string3 . $string4 . $string5 . $string6 . $string7;

	my $result = qx{$as_presc_string};

#	printf $as_presc_string;
	chomp $result;
	printf( "$result \n" );
	if( $result =~ /GPS_PRESC_DWELL_DONE/ )
	{
	     printf("\nGPS \"As prescribed\" dwell done!\n");
	     $as_presc_done = $as_presc_done & 1;
	}
	else
	{
	     $as_presc_done = $as_presc_done & 0;
	}

	if( $glo_enable == 1)
	{
		if( $result =~ /GLO_PRESC_DWELL_DONE/ )
		{
		     printf("GLO \"As prescribed\" dwell done!\n");
		     $as_presc_done = $as_presc_done & 1;
		}
		else
		{
		     $as_presc_done = $as_presc_done & 0;
		}
	}

	if($as_presc_done != 1)
	{
		$as_presc_done = 1;
		printf("\nWarning: Dwell Complete Event NOT received for either GPS or GLO. Trying again. Attempt 2 of 2\n");
		my $result = qx{$as_presc_string};

		printf $as_presc_string;
		chomp $result;
		printf( "$result \n" );
		if( $result =~ /GPS_PRESC_DWELL_DONE/ )
		{
		     printf("\nGPS \"As prescribed\" dwell done!\n");
		     $as_presc_done = $as_presc_done & 1;
		}
		else
		{
		     $as_presc_done = $as_presc_done & 0;
		}

		if( $glo_enable == 1)
		{
			if( $result =~ /GLO_PRESC_DWELL_DONE/ )
			{
			     printf("GLO \"As prescribed\" dwell done!\n");
			     $as_presc_done = $as_presc_done & 1;
			}
			else
			{
			     $as_presc_done = $as_presc_done & 0;
			}
		}
		if($as_presc_done != 1)
             {
                   printf( "\n*********Error: Unsucessful second attempt. Aborting test.\n" );
                   exit();
             }
	}
}

sub start_sens_dwell()
{
	printf "\nStarting sens dwell...\n";
	my($gps_enable, $glo_enable) = @_;
	my $auto_center_done = 1;
	my $string1 = "-gps_enable $gps_enable ";

	my $string2 = "-gps_rxd_flag $gps_rxd_flag ";
	my $string3 = "-gps_dopp_hz $gps_dopp_adjusted_hz ";
	my $string4 = "-glo_enable $glo_enable ";
	my $string5 = "-glo_dopp_hz $glo_dopp_adjusted_hz ";

	my $string6 = "-gps_ctrl_flag 0 -gps_cp_win $gps_cp_win_sz_chips -gps_srch_mode 3 -gps_postd 600 ";
	my $string7 = "-glo_ctrl_flag 0 -glo_cp_win $glo_cp_win_sz_chips -glo_srch_mode 18 -glo_postd 600 ";

	my $sens_dwell_cmd_string = "perl gnss_presc_dwell.pl $com_port 0 " . $string1 . $string2 . $string3 . $string4 . $string5 . $string6 . $string7;

#	printf $sens_dwell_cmd_string;
	my $result = qx{$sens_dwell_cmd_string};

	chomp $result;
	printf( "$result \n" );

	# No prints included here. Caller can print whats necessary and add more info to it as necessary (Eg., Dwell cnt etc.)
	if( $result =~ /GPS_PRESC_DWELL_DONE/ )
	{
	     $gps_dwell_status = "GPS Dwell Done";
	}
	elsif( $result =~ /GPS_PRESC_DWELL_TUNC_TOO_BIG/ )
	{
	     $gps_dwell_status = "GPS Dwell Tunc";
	}
	elsif( $result =~ /GPS_PRESC_DWELL_NOT_APPLICABLE/ )
	{
	     $gps_dwell_status = "GPS Dwell NA";
	}
	elsif( $result =~ /GPS_PRESC_DWELL_REJECTED/ )
	{
	     $gps_dwell_status = "GPS Dwell Rejected";
	}

	if( $result =~ /GLO_PRESC_DWELL_DONE/ )
	{
	     $glo_dwell_status = "GLO Dwell Done";
	}
	elsif( $result =~ /GLO_PRESC_DWELL_TUNC_TOO_BIG/ )
	{
	     $glo_dwell_status = "GLO Dwell Tunc";
	}
	elsif( $result =~ /GLO_PRESC_DWELL_NOT_APPLICABLE/ )
	{
	     $glo_dwell_status = "GLO Dwell NA";
	}
	elsif( $result =~ /GLO_PRESC_DWELL_REJECTED/ )
	{
	     $glo_dwell_status = "GLO Dwell Rejected";
	}
	return($gps_dwell_status, $glo_dwell_status);
}
