# 
#$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/standalone_rfv/verif_test/cgps_sa_vt.pl#1 $ $DateTime: 2016/12/13 07:59:45 $ $Author: mplcsds1 $
#*****************************************************************************
#  Copyright (C) 2003 Qualcomm, Inc.
#
#                  Qualcomm, Inc.
#                  675 Campbell Technology Parkway
#                  Campbell, CA  95008
#
# This program is confidential and a trade secret of SnapTrack, Inc.  The
# receipt or possession of this program does not convey any rights to reproduce 
# or disclose its contents or to manufacture, use or sell anything that this 
# program describes in whole or in part, without the express written consent of
# SnapTrack, Inc.  The recipient and/or possessor of this program shall not 
# reproduce or adapt or disclose or use this program except as expressly 
# allowed by a written authorization from SnapTrack, Inc.
#
# *****************************************************************************
# Description:
# This file contains standalone verification test support for the Converged GPS 
# 	
# 
#*****************************************************************************
#*/
#
#!/c:\utils\Cygwin\bin\perl.exe -w
#
# sigma subroutine extract the standard deviation from N samples using the
# sum(x) and sum(x*x)

use Getopt::Long;

$MEAS_STATUS_NULL              = 0x0000; 
$MEAS_STATUS_SM_VALID          = 0x0001;
$MEAS_STATUS_SB_VALID          = 0x0002;
$MEAS_STATUS_MS_VALID          = 0x0004;
$MEAS_STATUS_BE_CONFIRM        = 0x0008;
$MEAS_STATUS_VE_VALID          = 0x0010;
$MEAS_STATUS_VE_FINE           = 0x0020;
$MEAS_STATUS_LP_VALID          = 0x0040;
$MEAS_STATUS_LP_POS            = 0x0080;
$MEAS_STATUS_GOOD_DATA         = 0x0100;
$MEAS_STATUS_FROM_RNG_DIFF     = 0x0200;
$MEAS_STATUS_FROM_VE_DIFF      = 0x0400;
$MEAS_STATUS_XCORR             = 0x0800;
$MEAS_STATUS_TENTATIVE         = 0x1000;
$MEAS_STATUS_DONT_USE          = 0x2000;
$MEAS_STATUS_NEED_SIR_CHECK    = 0x4000;
$MEAS_STATUS_ACQ_OR_PROBATION  = 0x8000;

sub sigma
{
	my( $n, $x, $x2 ) = @_;
	my( $mean ) = $x/$n;
  my( $diff ) = $x2/$n - $mean*$mean;
  if( $diff < 0.000001 )
  {
    $sigma = 0.0;
  }
  else
  {
	  $sigma = sqrt( $diff );
  }

}

# open_file performs the generic file open and checking logic

sub open_file
{
	my( $Handle, $Attrib, $Name ) = @_;
	my( $File ) = sprintf "%s %s", $Attrib, $Name;

	unless( open $Handle, $File )
	{
		die( "Cannot open $Name: $!");
	}
}

# subroutine to read N lines from a specified file

sub read_lines
{
	my( $N_Lines ) = @_;

	$read_lines = '';

	foreach( 1..$N_Lines )
	{
		$line = <STDIN>;
		chomp $line;
		$read_lines .= $line . " ";
	}

	$line_count += $N_Lines;

	# Return the composite line
	$read_lines;
}

%opts = ();
GetOptions( \%opts, 'help');
&usage() if ($opts{'help'});

# Constants
	$PI				= 3.141592654;
	$ERADIUS		= 6378e3;

# Overcome used before initialized warnings
$FCount = 0;
$NPos	= 0;

# Open various output files

&open_file( POSITIONFILE, 	">", "position.txt" );
&open_file( MEASFILE, 		">", "meas.txt" );
&open_file( SUMMARY, 		">", "summary.txt" );

printf( MEASFILE "%% SV FCount CNo MS.SM Speed\n" );

while ( <STDIN> )
{
	$pkt_header = $_;
	chomp $pkt_header;


#2006 Oct 11 	21:24:39.002 [1F]	0x1371 Converged GPS Measurement Report
#Version    = 1 
#FCount     = 25560 
#Week       = 1396 weeks
#Msec       = 336279000 ms
#Bias       = -0.314921 ms
#TUnc       = 0.068454 ms
#FreqBias   = 0.0 m/s
#FUnc       = 15.5 m/s
#Number of SV data blocks = 7 
# 
#Sv  Os  Elv  Azi  Stat   Gd   Tot  CNo  Latency  Pre Post     Ms        SubMs      TUnc      Speed   SpdUnc  Multipath   CarrPhase
#----------------------------------------------------------------------------------------------------------------------------------
# 8  5             001F   28    28  4.9    369    20   25 1059116181 0.00000000  0.000489    492.65     0.0      	 	Cyc Slip
#11  5             001F   28    28  4.4    369    20   25 1059771781 0.00000000  0.000489    253.42     0.0      	 	Cyc Slip
#17  5             001F   28    28  5.5    349    20   25 1058402861 0.00000000  0.000489   -143.74     0.0      	 	Cyc Slip
#26  5             001F   28    28  5.0    330    20   25 1051848828 0.00000000  0.000489    122.42     0.0      	 	Cyc Slip
#27  5             001F   28    28  4.0    369    20   25 1061523989 0.00000000  0.000489    585.77     0.0      	 	Cyc Slip
#28  5             001F   28    28  5.1    370    20   25 1048379840 0.00000000  0.000489    221.23     0.0      	 	Cyc Slip
#29  5             001F   28    28  4.9    330    20   25 1024037220 0.00000000  0.000489    160.44     0.0      	 	Cyc Slip


	# Pull the FCount off the first measurement report string
	if( $pkt_header =~ /Converged GPS\s+Measurement\s+Report$/)
	{
    # Verify that the next line is "Version = ...."
		$line = read_lines(1);
		chomp( $line );
		@line = split (' ', $line);
    if( $line[0] eq "Version" )
    {
      # Read the line "FCount = ...."
		  $line = read_lines(1); 
		  chomp( $line );

		  @line = split (' ', $line);

		  $FCount_Meas = $line[2];

      # Skip over the line "Week  = ...."
      # Skip over the line "Msec = ...."
      # Skip over the line "Bias  = ...."
      # Skip over the line "TUnc = ...."
      # Skip over the line "FreqBias  = ...."
      # Skip over the line "FUnc = ...."
		  $line = read_lines(6); 

      # Read the line "Number of SV data blocks = ...."
      $line = read_lines(1); 
      chomp( $line );

      @line = split (' ', $line);

      $SV_Cnt = $line[6];

      # Skip over the line "              "
      # Skip over the line "SV  C  Ht ...."
      # Skip over the line "-----------------------"
      $line = read_lines(3); 

    
		  foreach( 1..$SV_Cnt )
		  {

			  $line = read_lines(1); 
			  @line = split (' ', $line);

			  # When elev/azi are unknown, the fields are blank. Therefore the indexing
			  # of the fields is determined by the formatting of field 12 which is
        # either SubMs with 8 decimal places (when elev/azi are present) or Speed 
        # with 2 decimal places (when elev/azi are blank)
        if( $line[12] =~ /[\d\-]+\.\d{8}/ )
        {
				  $Sv	= $line[0];
          $Os = $line[1];
          $Stat = $line[4];
				  $Speed = $line[14];
				  $Cno = $line[7];
				  $Ms = $line[11];
				  $SubMs = $line[12];
        }
        elsif( $line[12] =~ /[\d\-]+\.\d{2}/ )
        {
				  $Sv	= $line[0];
          $Os = $line[1];
          $Stat = $line[2];
				  $Speed = $line[12];
				  $Cno = $line[5];
				  $Ms = $line[9];
				  $SubMs = $line[10];
        }
        else
        {
          next
        }

			  if( (($Os eq "5") || ($Os eq "7")) &&
            (hex($Stat) & ($MEAS_STATUS_SM_VALID | $MEAS_STATUS_SB_VALID | $MEAS_STATUS_VE_VALID)) &&
            ((hex($Stat) & ($MEAS_STATUS_FROM_RNG_DIFF | 
                            $MEAS_STATUS_FROM_VE_DIFF | 
                            $MEAS_STATUS_XCORR | 
                            $MEAS_STATUS_DONT_USE)) == 0)
          )
			  {

				  printf( MEASFILE "%02s %lu %4.1f %f %s\n", 
						  $Sv, $FCount_Meas, $Cno, $Ms+$SubMs, $Speed );

				  $CnoSum[$Sv] 	+= $Cno;
				  $CnoSum2[$Sv]	+= $Cno * $Cno;

				  $SpeedSum[$Sv]	+= $Speed;
				  $SpeedSum2[$Sv] += $Speed * $Speed;

				  unless( $N[$Sv] )
				  {
					  $MaxCno[$Sv]  	= $Cno;
					  $MinCno[$Sv]  	= $Cno;
					  $MaxSpeed[$Sv]  = $Speed;
					  $MinSpeed[$Sv]  = $Speed;
				  }
				  else
				  {
					  $MaxCno[$Sv]  = $Cno if $Cno > $MaxCno[$Sv];
					  $MinCno[$Sv]  = $Cno if $Cno < $MinCno[$Sv];
		
					  $MaxSpeed[$Sv]  = $Speed if $Speed > $MaxSpeed[$Sv];
					  $MinSpeed[$Sv]  = $Speed if $Speed < $MinSpeed[$Sv];
				  }

          # Look for speed discontinuities, get max point-to-point acceleration
          if( $PriorSpeedFlag[$Sv] )
          {
            $DeltaT = ($FCount_Meas - $LastT[$Sv]) * 0.001;
            $ValidAccel = 0;
            # Avoid divide by zero (or anything very small)
            if( $DeltaT > 0.00001 )
            {
              $ValidAccel = 1;
              $DeltaS = $Speed - $LastS[$Sv];
              $Accel = $DeltaS / $DeltaT;
              if( $Accel < 0.0 )
              {
                $Accel = -$Accel;
              }
              if( $MaxAccel[$Sv] < $Accel )
              {
                $MaxAccel[$Sv] = $Accel;
                $MaxAccelTime[$Sv] = $FCount_Meas * 0.001;  
              }
            }
          }
          if( $ValidAccel || !$PriorSpeedFlag[$Sv] )
          {
            $LastT[$Sv] = $FCount_Meas;
            $LastS[$Sv] = $Speed;
            $PriorSpeedFlag[$Sv] = 1; 
          }

				  $N[$Sv] += 1;
			  }
		  } # foreach
    }
	}


#2006 Oct 11 	21:25:18.018 [07]	0x1370 Converged GPS Position Report
#	Version: 1
#	Pos Vel Flags: 0x0082
#	Pos Source: WLS (Weighted Least Squares)
#
#	GPS Fix Count: 0
#
#	GPS week  = 1396
#	GPS msecs = 336318001
#	FCount msecs = 0
#
#	Latitude  (rad) = 0.650658471  (37.27998429 deg) 
#	Longitude (rad) = -2.128309697  (-121.94316314 deg) 
#	Altitude  (m)   = -1.#QNAN0
#
#	East Vel  (m/s)       = -0.048
#	North Vel (m/s)       = -0.012
#	Up Vel    (m/s)       = 0.000
#	Vert pos sigma (m)    = 0.000
#	East Vel sigma (m/s)  = 2.640
#	North Vel sigma (m/s) = -39164946425679403000000000.000
#	Up Vel sigma (m/s)    = -0.000
#
#	Receiver's clock bias  (m)   = -97066.602
#	Receiver's clock drift (m/s) = 2.031
#
#	Position Dilution of Precision (unitless)   = 2.25
#	Horizontal DOP (unitless)                   = 1.23
#	Vertial DOP (unitless)                      = 1.88
#
#	Filtered altitude (m)         = 34.655182
#	Unfiltered altitude (m)       = 0.000003
#	Filtered altitude sigma (m)   = 0.000003
#	Unfiltered altitude sigma (m) = 34.330002
#
#	Error Ellipse alongside axis     (m) = 2.261
#	Error Ellipse perpendicular axis (m) = 3.278
#	Error Ellipse angle (deg)            = 86.2
#	Total SVs     = 7
#	Number of SVs Used = 7
#
#	Sv Used  IODE       RR(m)      VR(m/s) Weight
#	---------------------------------------------
#	 8  Yes   107     0.00       0.00       0.00
#	11  Yes    14     0.00       0.00       0.00
#	17  Yes    15     0.00       0.00       0.00
#	26  Yes    45     0.00       0.00       0.00
#	27  No 
#	28  Yes    26     0.00       0.00       0.00
#	29  Yes    13     0.00       0.00       0.00
 
	if( $pkt_header =~ /Converged GPS\s+Position\s+Report$/ )
	{
    # Verify that the next line is "Version = ...."
		$line = read_lines(1);
		chomp( $line );
		@line = split (' ', $line);
    if( $line[0] eq "Version" )
    {
		  $line = &read_lines(25);
		  @line = split /[\t\s]+/, $line;

		  $FCount_Pos = $line[28];

		  if( $FCount_Pos != 0 )
		  {
			  $Lat 			= $line[32] * (180./$PI);
			  $Lon 			= $line[38] * (180./$PI);
			  $Alt 			= $line[44] * (180./$PI);

			  $LatSum			+= $Lat;
			  $LonSum			+= $Lon;
			  $AltSum			+= $Alt;

			  $FCount[$NPos]	= $FCount_Pos;
			  $Lat[$NPos]		= $Lat;
			  $Lon[$NPos]		= $Lon;
			  $Alt[$NPos]		= $Alt;

			  $Pdop 			= $line[102];
			  $Pdop[$NPos]	= $Pdop;

			  $NPos++;
		  }
    }
	}

	$line_count++;
}



# Processing complete. Now generate summary information.

printf( "\rDone: %d lines processed\n\n", $line_count );
printf( "Summary written to \"summary.txt\"\n" );
printf( "Measurements written to \"meas.txt\"\n" );
printf( "Positions written to \"position.txt\"\n" );

# Print a summary report 
if( $NPos )
{
	$LatMean		= $LatSum / $NPos;
	$LonMean		= $LonSum / $NPos;
	$AltMean		= $AltSum / $NPos;

	$CosLat			= cos( $LatMean * 3.141592654 / 180.0 );

	for( $I = 0; $I < $NPos; $I++ )
	{
		$X = $CosLat * ($Lon[$I] - $LonMean) * $PI/180.0 * $ERADIUS;
		$Y = ($Lat[$I] - $LatMean) * $PI/180.0 * $ERADIUS;

		$Error[$I] = sqrt( $X*$X + $Y*$Y );
	}

	@Descending = sort { $a <=> $b } @Error;

	$OneSigmaIndex = $NPos * 0.67; 

	$OneSigmaCep = $Descending[ $OneSigmaIndex ];

	printf( POSITIONFILE "%% FCount Lat Lon Alt PDOP Error\n" );

	for( $I = 0; $I < $NPos; $I++ )
	{
		printf( POSITIONFILE "%lu %s %s %s %s %5.3f\n",
					$FCount[$I],
					$Lat[$I],
					$Lon[$I],
					$Alt[$I],
					$Pdop[$I],
					$Error[$I]
					);
	}


	printf( SUMMARY "%ld Positions Average Lat Lon Alt %5.7f %5.7f %5.3f CEP %5.3f\n\n",
					$NPos,
					$LatMean,
					$LonMean,
					$AltMean,
					$OneSigmaCep );
}

# The measurement summary captures how each SV did across the observation interval

for( $Sv = 1; $Sv <= 32; $Sv++ )
{
	if( $N[$Sv] )
	{
		printf( SUMMARY "Sv %2d Num of meas %5d\n", $Sv, $N[$Sv] );

		$CnoMean = $CnoSum[$Sv] / $N[$Sv];
		$CnoSigma = &sigma( $N[$Sv], $CnoSum[$Sv], $CnoSum2[$Sv] );

		printf( SUMMARY "C/No Avg %5.2f Sigma %5.2f Max %5.2f Min %5.2f\n",
						$CnoMean,
						$CnoSigma,
						$MaxCno[$Sv],
						$MinCno[$Sv] );

		$SpeedMean = $SpeedSum[$Sv] / $N[$Sv];
		$SpeedSigma = &sigma( $N[$Sv], $SpeedSum[$Sv], $SpeedSum2[$Sv] );
		printf( SUMMARY "Speed Avg %5.1f Sigma %5.1f Max %5.1f Min %5.1f\n",
						$SpeedMean, 
						$SpeedSigma,
						$MaxSpeed[$Sv],
						$MinSpeed[$Sv] );

    printf( SUMMARY "Max Acceleration %5.3f m/s/s at %5.1f\n",
            $MaxAccel[$Sv],
            $MaxAccelTime[$Sv] );

    printf( SUMMARY "System Processing Loss %3.1f dB\n",
            173.9 - 130.0 - $CnoMean );

		printf( SUMMARY "\n" );
	}
}

close (SUMMARY);

# Parsing summary.txt for PASS/FAIL checking
printf ("\nparsing summary.txt\n");
open(SUMMARY, "summary.txt") || die "cannot open summary.txt";
$i = 0;
while( <SUMMARY>)
{
	$array[$i] = $_;
	$i++;
}
close (SUMMARY);
$size = @array;
open(SUMMARY, ">>summary.txt");
printf SUMMARY "------------------------------ PASS/FAIL checking ------------------------------\n";
for ($i = 0; $i <= $size; $i++)
{
	$line = $array[$i];
	#print("line = $line\n");
	chomp $line;
	if( $line =~ /C\/No/ ) {
		@Cno = split(/\s+/,$line);
		$CnoStability = $Cno[4];
		if ($CnoStability < 0.3 ) {
			printf SUMMARY "\n\nPASS\t";
		}
		else {
			printf SUMMARY "\n\nFAIL\t";;
		}
		printf SUMMARY "CnoStability [C\/No Sigma] = %2.2f", $CnoStability; 
		printf SUMMARY "\t\t|Pass requirement < 0.3dB\/HZ\n";
		printf "\nCnoStability [C\/No Sigma] = $CnoStability \t\t|| Pass requirement < 0.3dB\/HZ\n";
	}
	if( $line =~ /System/ ) {
		@System = split(/\s+/,$line);
		$SystemProcessingLoss = $System[3];
		if ($SystemProcessingLoss < 5.5 ) {
			printf SUMMARY "PASS\t";
		}
		else {
			printf SUMMARY "FAIL\t";;
		}
		printf SUMMARY "SystemProcessingLoss = %2.2f", $SystemProcessingLoss;
		printf SUMMARY "\t\t\t|Pass requirement < 5.5dB\n";
		printf "SystemProcessingLoss = $SystemProcessingLoss \t\t\t|| Pass requirement < 5.5dB\n";
	}
	if( $line =~ /Speed/ ) {
		@Speed = split(/\s+/,$line);
		$LoBias = $Speed[2];
		$LoStability = $Speed[6] - $Speed[8];
		if ($LoBias < 1500 ) {
			printf SUMMARY "PASS\t";
		}
		else {
			printf SUMMARY "FAIL\t";;
		}
		printf SUMMARY "LoBias [Speed Avg]  = %6.2f", $LoBias;
		printf SUMMARY "\t\t\t|Pass requirement < 5ppm (1500m\/s)\n";
		printf "LoBias [Speed Avg] = $LoBias \t\t\t|| Pass requirement < 5ppm (1500m\/s)\n";
		if ($LoStability < 300 ) {
			printf SUMMARY "PASS\t";
		}
		else {
			printf SUMMARY "FAIL\t";;
		}
		printf SUMMARY "LoStability [Speed Max - Speed Min] = %3.2f", $LoStability;
		printf SUMMARY "\t|Pass requirement < 1ppm (300m\/s)\n";
		printf "LoStability [Speed Max - Speed Min] = %3.2f", $LoStability;
		printf " \t|| Pass requirement < 1ppm (300m\/s)\n";
	}
	if( $line =~ /Acceleration/ ) {
		@Acceleration = split(/\s+/,$line);
		$LoDiscontinuity = $Acceleration[2];
		if ($LoDiscontinuity < 3) {
			printf SUMMARY "PASS\t";
		}
		else {
			printf SUMMARY "FAIL\t";;
		}
		printf SUMMARY "LoDiscontinuity [Max Acceleration] = %2.2f", $LoDiscontinuity;
		printf SUMMARY "\t|Pass requirement < 3m\/s\/s\n";
		printf "LoDiscontinuity [Max Acceleration] = $LoDiscontinuity \t|| Pass requirement < 3m\/s\/s\n";
	}
}
if ($CnoStability < 0.3 && $SystemProcessingLoss < 5.5 && $LoBias < 1500 && $LoStability < 300 && $LoDiscontinuity < 3) {
	print "\n******************************************* PASS *************************************\n";
	printf SUMMARY "\n------------------------------ 4.1 C\/No Test PASS ------------------------------\n";
}
else {
	print "\n******************************************* FAIL *************************************\n";
	printf SUMMARY "\n------------------------------ 4.1 C\/No Test FAIL ------------------------------\n";
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

Process CGPS logs.

Syntax:   perl cgps_sa_vt.pl < <Text File>

Options:  -help               Display this help message.
 
Example:  perl cgps_sa_vt.pl < M20070926_165839.txt

END

   print "$usage";
   exit (0);
}

