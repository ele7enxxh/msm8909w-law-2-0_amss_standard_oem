# 
#$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss_tools/standalone_rfv/bus_bw_test/ProcBusBwResults.pl#1 $ $DateTime: 2016/12/13 07:59:45 $ $Author: mplcsds1 $
######################################################################
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
######################################################################
# Description:
# This file takes as input the APEX output file txt file containing the dwell results.
# This script outputs busbw_summary.txt which contains the summary results based on user-supplied
# CNo threshold.
#
# Usage:
# perl ProcBusBwResults.pl < <APEX_Output_Filename>
# 
######################################################################

#!/c:\utils\Cygwin\bin\perl.exe -w
use Getopt::Long;
# Constants
use constant TEST_TYPE_GPS_BW        => 1;
use constant TEST_TYPE_MAX_BW_DMA    => 2;
use constant TEST_TYPE_MAX_BW_SYS    => 3;

# sigma subroutine extracts the standard deviation from N samples using the
# sum(x) and sum(x*x)

sub sigma
{
	my( $n, $x, $x2 ) = @_;
	my( $mean ) = $x/$n;

	$sigma = sqrt( $x2/$n - $mean*$mean );
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


# Initialize to be safe
$N		 = 0;
$CnoSum  = 0;
$CnoSum2 = 0;

$type_of_test = 0;

$Total_for_avg = 0;
$Cnt = 0;

$Total_for_avg_srch = 0;
$Total_for_avg_srch_et = 0;
$Total_for_avg_gacc = 0;
$Cnt_srch = 0;
$Cnt_gacc = 0;
$SrchEt_Over_Nominal = 0;
$SrchCt_Over_1ms = 0;
$GaccCt_Over_30ms = 0;
$StallCntTot = 0;

$SrchMin  = 999999;  # Some large unrealistic value
$SrchEtMin = 999999;
$GaccMin  = 999999;  # Some large unrealistic value

$SrchCtMax  = 0;  
$SrchEtMax = 0;
$GaccMax  = 0; 

$SrchAvg  = 0;  
$GaccAvg  = 0;  

$StallCntMin  = 999999;  # Some large unrealistic value
$StallCntMax  = 0;  

$MaxBw_Min  = 999999;  
$MaxBw_Max  = 0;  
$MaxBw_Avg  = 0;  
my $ColCnt; # Count of columns that preceded the actual F3 msg.
my $found_start_col_flag = 0;


# Open various output files

&open_file( SUMMARY, 		">", "cgps_busbw_summary.txt" );
&open_file( EXCEL_DUMP, 		">", "cgps_busbw_dump.xls" );
$start_file_dump = 0;
printf( EXCEL_DUMP "Srch_Ct Min (chipx1)\t Srch_Ct Max (chipx1)\t gps_rtc_max\t Srch_Ct Accum (chipx1)\t Srch_Et Min (chipx1)\t Srch_Et Max (chipx1)\t gps_rtc_max\t Srch_Et Accum (chipx1)\t Completion Time Over-run\t Execution Time Over-run\t Stall Cnt Min\t Stall Cnt Max\t Stall Cnt Accum\t Srch Violation Cnt\t Gacc Min(us)\tGacc Max(us)\t Gacc Accum (chipx1)\tGacc Violation Cnt\t" );


while ( <STDIN> )
{
	$line = $_;
	chomp $line;

# QXDM F3 msg screen's columns are configurable. This requires the code to determine the # of cols before busbw F3 msg begins. Col# is constant
# for a given file. We trap the first occurrence of "cc_busbwtest.c" and loop through each word of the line to determine the col#. This is then
# used to index into required elements.

       if( $found_start_col_flag == 0 )
       {
	       $line2 = $_;
      	       if( $line2 =~ /cc_busbwtest.c+/ )
      	       {

                    $found_start_col_flag = 1;
                    @line2 = split /[\t\s]+/, $line2;
                    $col_index = @line2 -1;# @ on an array returns # of elements.
                    
                    while( $col_index != 0 )
                    {
                          if( $line2[$col_index] eq "cc_busbwtest.c" )
                          {
                                  $ColCnt = $col_index + 2;# +1 for the source-code line# col; +1 for index->cnt
                          }
                          $col_index--;
                    }
             }
       }
# 21:09:05.281         cc_busbwtest.c  01402  Srch Stall Count:   Min 0  Max 0  Accum 0 (Clock cycles)
# 21:09:05.281         cc_busbwtest.c  01407  Srch Violation Count:   0 / 1000 (Violations / Attempts)
# 21:09:06.245         cc_busbwtest.c  01442  Gacc Completion Time: Min 5737 chipx1, Max 12575 chipx1, Accum 424687 Chipx1
# 21:09:06.245         cc_busbwtest.c  01447  Gacc Violation Count: 0 / 50 (Violations / Attempts)
# 21:09:06.281         cc_busbwtest.c  01389  Srch Completion Times: Min 854 chipx1, Max 854 chipx1 @ gps_rtc 2207752 us, Total 854000 chipx1
# 21:09:06.281         cc_busbwtest.c  01393  Srch Execution Times: Min 854 chipx1, Max 854 chipx1 @ gps_rtc 2207752 us, Total 854000 chipx1
# 21:09:06.281         cc_busbwtest.c  01400  Srch Nominal Completion/Execution Time Over-run: 0 / 0 (Count)
# 21:09:06.281         cc_busbwtest.c  01402  Srch Stall Count:   Min 0  Max 0  Accum 0 (Clock cycles)
# 21:09:06.282         cc_busbwtest.c  01407  Srch Violation Count:   0 / 1000 (Violations / Attempts)
# 21:09:07.248         cc_busbwtest.c  01442  Gacc Completion Time: Min 5773 chipx1, Max 24431 chipx1, Accum 436430 Chipx1
# 21:09:07.248         cc_busbwtest.c  01445  Gacc Violation Count: 1 / 50 (Violations / Attempts)**********
# 21:09:07.280         cc_busbwtest.c  01389  Srch Completion Times: Min 854 chipx1, Max 854 chipx1 @ gps_rtc 3231752 us, Total 854000 chipx1
# 21:09:07.280         cc_busbwtest.c  01393  Srch Execution Times: Min 854 chipx1, Max 854 chipx1 @ gps_rtc 3231752 us, Total 854000 chipx1
# 21:09:07.280         cc_busbwtest.c  01400  Srch Nominal Completion/Execution Time Over-run: 0 / 0 (Count)

	# Pull the data from the test results
	if( $line =~ /Srch Completion Times:+/ )
	{
	       $type_of_test = TEST_TYPE_GPS_BW;
              @line = split /[\t\s]+/, $line;
              
              if( $line[$ColCnt + 7] > $SrchCtMax )
              {
              $SrchCtMax = $line[$ColCnt + 7];
              }
              if( $line[$ColCnt + 4] < $SrchMin )
              {
              $SrchMin = $line[$ColCnt + 4];
              }
              $Total_for_avg_srch += $line[$ColCnt + 10];
              $Cnt_srch++;
              printf( EXCEL_DUMP "\n$line[$ColCnt + 4]\t$line[$ColCnt + 7]\t$line[$ColCnt + 11]\t$line[$ColCnt + 14]\t" );
              $start_file_dump = 1;
	}

	if( $line =~ /Srch Execution Times:+/ )
	{
	       $type_of_test = TEST_TYPE_GPS_BW;
              @line = split /[\t\s]+/, $line;
              
              if( $line[$ColCnt + 7] > $SrchMax )
              {
              $SrchEtMax = $line[$ColCnt + 7];
              }
              if( $line[$ColCnt + 4] < $SrchMin )
              {
              $SrchEtMin = $line[$ColCnt + 4];
              }
              $Total_for_avg_srch_et += $line[$ColCnt + 10];
              printf( EXCEL_DUMP "$line[$ColCnt + 4]\t$line[$ColCnt + 7]\t$line[$ColCnt + 11]\t$line[$ColCnt + 14]\t" );
	}

	if( $line =~ /Srch Nominal +/ )
	{
	       $type_of_test = TEST_TYPE_GPS_BW;
              @line = split /[\t\s]+/, $line;
              $SrchEt_Over_Nominal = $SrchEt_Over_Nominal + $line[$ColCnt + 7];

              printf( EXCEL_DUMP "$line[$ColCnt + 5]\t$line[$ColCnt + 7]\t" );
	}

	if( $line =~ /Gacc Completion Time:+/ )
	{
	       $type_of_test = TEST_TYPE_GPS_BW;
              @line = split /[\t\s]+/, $line;
              
              if( $line[$ColCnt + 7] > $GaccMax )
              {
              $GaccMax = $line[$ColCnt + 7];
              }
              if( $line[$ColCnt + 4] < $GaccMin )
              {
              $GaccMin = $line[$ColCnt + 4];
              }
              $Total_for_avg_gacc += $line[$ColCnt + 10];
              $Cnt_gacc++;
              if( $start_file_dump == 1 )
              {
                    printf( EXCEL_DUMP "$line[$ColCnt + 4]\t$line[$ColCnt + 7]\t$line[$ColCnt + 10]\t" );
              }
	}

	if( $line =~ /Srch Stall Count:+/ )
	{
	       $type_of_test = TEST_TYPE_GPS_BW;
              @line = split /[\t\s]+/, $line;
              
              if( $line[$ColCnt + 6] > $StallCntMax )
              {
              $StallCntMax = $line[$ColCnt + 6];
              }
              if( $line[$ColCnt + 4] < $StallCntMin )
              {
              $StallCntMin = $line[$ColCnt + 4];
              }

              $StallCntTot += $line[$ColCnt + 8];
              
              if( $start_file_dump == 1 )
              {
                    printf( EXCEL_DUMP "$line[$ColCnt + 4]\t$line[$ColCnt + 6]\t$line[$ColCnt + 8]\t" );
              }
	}

	if( $line =~ /Srch Violation Count:+/ )
	{
	       $type_of_test = TEST_TYPE_GPS_BW;
              @line = split /[\t\s]+/, $line;
              
              $SrchCt_Over_1ms = $SrchCt_Over_1ms + $line[$ColCnt + 3];
              if( $start_file_dump == 1 )
              {
                    printf( EXCEL_DUMP "$line[$ColCnt + 3]\t" );
              }

	}

	if( $line =~ /Gacc Violation Count:+/ )
	{
	       $type_of_test = TEST_TYPE_GPS_BW;
              @line = split /[\t\s]+/, $line;

              $GaccCt_Over_30ms += $line[$ColCnt + 3];
              if( $start_file_dump == 1 )
              {
                    printf( EXCEL_DUMP "$line[$ColCnt + 3]\t" );
              }

	}

	if( $line =~ /Data Transfer per Interrupt:+/ )
	{
              @line = split /[\t\s]+/, $line;
              $MaxBw_TransferPerIntr_Bytes = $line[$ColCnt + 4];
	}
	if( $line =~ /SYS Bus Transfers+/ )
	{
	       $type_of_test = TEST_TYPE_MAX_BW_SYS;
              @line = split /[\t\s]+/, $line;
              
              if( $line[$ColCnt + 9] > $MaxBw_Max )
              {
              $MaxBw_Max = $line[$ColCnt + 9];
              }
              if( $line[$ColCnt + 6] < $MaxBw_Min )
              {
              $MaxBw_Min = $line[$ColCnt + 6];
              }
              $Total_for_avg +=$line[$ColCnt + 12];
              $Cnt++;
	}
	if( $line =~ /DMA Bus Transfers+/ )
	{
	       $type_of_test = TEST_TYPE_MAX_BW_DMA;
              @line = split /[\t\s]+/, $line;

              if( $line[$ColCnt + 9] > $MaxBw_Max )
              {
              $MaxBw_Max = $line[$ColCnt + 9];
              }
              if( $line[$ColCnt + 6]< $MaxBw_Min )
              {
              $MaxBw_Min = $line[$ColCnt + 6];
              }
              $Total_for_avg +=$line[$ColCnt + 12];

              $Cnt++;
	}
}

# Processing complete. Now generate summary information.

printf( "Summary written to \"busbw_summary.txt\"\n" );
printf( "ALL Bus BW related data written to \"cgps_busbw_dump.xls\"\n" );


if( $type_of_test == TEST_TYPE_GPS_BW )
{
          printf( SUMMARY "BUS BW TEST (GPS BW) RESULTS SUMMARY:\n\n" );
          printf( SUMMARY "SEARCHER SUMMARY:-\n\n" );
          printf( SUMMARY "Test Duration:            %d sec\n", $Cnt_srch);
#         printf( SUMMARY "Fastest Srch Transfer over 1ms:     %d us\n", $SrchMin/1.023 );  
          printf( SUMMARY "Max Completion:           %d us\n",  $SrchCtMax/1.023 );
          printf( SUMMARY "Executions above 837 us:  %d \n",    $SrchEt_Over_Nominal );
          printf( SUMMARY "Completions above 1ms:    %d\n\n",   $SrchCt_Over_1ms );
#          printf( SUMMARY "Smallest Stall Count over 1ms: %d\n", $StallCntMin );  
          printf( SUMMARY "Max Stall in 1ms:         %d\n",     $StallCntMax );
          printf( SUMMARY "Total Stalls:             %d\n\n",   $StallCntTot );
#          printf( SUMMARY "Fastest Gacc Transfer over 20ms:    %f us\n", $GaccMin/1.023 );  

          printf( SUMMARY "GACC SUMMARY:-\n\n" );
          printf( SUMMARY "Test Duration:            %d sec\n",   $Cnt_gacc);
          printf( SUMMARY "Max Completion:           %5.3f ms\n", $GaccMax/1023 );
          printf( SUMMARY "Completions above 20/30ms:%d       (Regular/UBL)\n\n",     $GaccCt_Over_30ms );

#          printf( SUMMARY "Average Srch Transfer:    %d us\n", $Total_for_avg_srch/($Cnt_srch * 1.023) );
#          printf( SUMMARY "Average Gacc Transfer:    %d us\n", $Total_for_avg_gacc/($Cnt_gacc * 1.023) );
}

if( $type_of_test == TEST_TYPE_MAX_BW_SYS )
{
          printf( SUMMARY "BUS BW TEST (SYS BUS, MAX BW) RESULTS SUMMARY:\n\n" );
          printf( SUMMARY "Data Transferred:         %d bytes\n",        $MaxBw_TransferPerIntr_Bytes);
          printf( SUMMARY "Slowest SYS Transfer:     %d us\n",           $MaxBw_Max );  
          printf( SUMMARY "Fastest SYS Transfer:     %d us\n",           $MaxBw_Min );
          printf( SUMMARY "Highest Bandwidth:        %.4f Mbytes/sec\n", $MaxBw_TransferPerIntr_Bytes/$MaxBw_Min);
          printf( SUMMARY "Lowest Bandwidth:         %.4f Mbytes/sec\n", $MaxBw_TransferPerIntr_Bytes/$MaxBw_Max);
          printf( SUMMARY "Average Transfer:         %d us\n",           $Total_for_avg/$Cnt );
}

if( $type_of_test == TEST_TYPE_MAX_BW_DMA )
{
          printf( SUMMARY "BUS BW TEST (DMA BUS, MAX BW) RESULTS SUMMARY:\n\n" );
          printf( SUMMARY "Data Transferred:         %d bytes\n", $MaxBw_TransferPerIntr_Bytes);
          printf( SUMMARY "Slowest DMA Transfer:    %d us\n", $MaxBw_Max );  
          printf( SUMMARY "Fastest DMA Transfer:    %d us\n", $MaxBw_Min );  
          printf( SUMMARY "Highest Bandwidth:        %.4f Mbytes/sec\n", $MaxBw_TransferPerIntr_Bytes/$MaxBw_Min);
          printf( SUMMARY "Lowest Bandwidth:         %.4f Mbytes/sec\n", $MaxBw_TransferPerIntr_Bytes/$MaxBw_Max);
          printf( SUMMARY "Average Transfer:        %d us\n", $Total_for_avg/$Cnt );
}


printf( SUMMARY "\n" );

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

Process QXDM-generated F3 text file containing Bus BW test result obtained from a SINGLE test.
Creates two output files. cgps_busbw_dump.xls is a column-formatted dump of all Bus BW data in F3. 
cgps_busbw_summary.txt contains a summary of the results.

Syntax:   perl ProcBusBwResults.pl < F3_TEXT_FILE

          F3_TEXT_FILE:  QXDM-generated text file containing Bus BW F3 messages.
          -h:            Display this help message.

Example:  perl ProcBusBwResults.pl < BusBw_results.txt

END

   print "$usage";
   exit (0);
}

