#!/usr/bin/perl
#===============================================================================
#                    hitGUI.pl
#  DESCRIPTION:
#     Perl/Tk GUI for HIT
#
#
#  VERSION  DATE       WHO     DESCRIPTION
#  -------  ---------  -------  -----------
#  1.0.0    11/15/2006  vhegde  Generic GUI for HIT test applications
#===============================================================================

#use strict;
use diagnostics;
use warnings;
no  warnings qw(once);
use Data::Dumper;

use Tk;
use Tk::LabFrame;
use Tk::Tree;
use Tk::ROText;
use Tk::TextUndo;

use MetadataConstants;
use APSSubSystem57MetaData;
use APSSubSystem57CmdData;

use ASIA_Main;

#===============================================================================
# Global variables
#===============================================================================
#
our $hit_queue_mode = 0;

our $hitFrame;
our $paramFrame;

our $CmdData={};
our $pathSel="";

our @respToken = ();
our @statusLevel = ( 'LOW', 'MEDIUM', 'HIGH', 'ERROR', 'FATAL' );

our $recText = undef;
our @last10 = ();
our $recHeader = "#!/usr/bin/perl

use ASIA_Main;
use hit;

ASIA_Initialize();

hit::init();

my \@respToken;
my \$token;
my \$resp;

";

our $offline = ($ARGV[0] =~ /^-offline$/) if(defined $ARGV[0]);

#===============================================================================
# ASIA Initialize
#===============================================================================
#
if (!$offline) {
   ASIA_Initialize();
   
   #initialize handle to subsystem 57 (HIT)
   GetResources("APS",      "APS.1",        { Persistence => $PERSISTENT_NEW });
   our $obj57 = $::APS->GetSubSystem ({subsystemid => 57});
}

#===============================================================================
# GUI
#===============================================================================
# 
#Main window
our $mw = MainWindow->new;
$mw->title("HIT - Hardware Integration Test");

#Menubar
$mw->configure(-menu => my $menubar = $mw->Menu);

#Parameter cascade
my $parameter_menu = $menubar->cascade(-label => '~Parameter');
$parameter_menu->command(-label => 'Load', -underline => 0,
                    -command => sub { paramLoad("") });
$parameter_menu->separator;
$parameter_menu->command(-label => 'Save as', -underline => 0,
                    -command => sub { paramSave("") });

#Record cascade
my $record_menu = $menubar->cascade(-label => '~Record');
my $rec_last10 = $record_menu->command(-label => 'Last 10 commands',
                      -command => sub { Last10() } );
$rec_last10->configure(-state=>'disabled') if ($offline);

#Help cascade
our $help = $menubar->cascade(-label => '~Help');

#Dummy label at the bottom - can be used for status bar
our $statusLabel = $mw->Label()->pack(-side=>'bottom');

#Command tree
our $tree = $mw->Scrolled("Tree", -scrollbars=>'ow',
                           -width => 30,
                           -selectmode => "single",
                           -selectbackground => 'grey',
                           #-command => sub { paramEntry() },
                           -browsecmd => sub { paramEntry() }
                          )->pack(-side=>'left', -fill=>'both', -expand =>1);

#Status window
our $statusWin = $mw->Scrolled("ROText", -scrollbars=>'oe',
                             -background => 'black',
                             -foreground => 'red',
                             -state => 'normal',
                             -width => 50
                             )->pack(-side=>'right', -fill=>'both');

$statusWin->tagConfigure('LOW', -foreground=>'green');
$statusWin->tagConfigure('MEDIUM', -foreground=>'cornflowerblue');
$statusWin->tagConfigure('HIGH', -foreground=>'yellow');
$statusWin->tagConfigure('ERROR', -foreground=>'pink');
$statusWin->tagConfigure('FATAL', -foreground=>'red');
$statusWin->tagConfigure('HIT', -foreground=>'blue');
$statusWin->tagConfigure('DEFAULT', -foreground=>'purple');

$mw->Label()->pack(-side=>'right', -pady=>210); #sizer

#Send and Record buttons
our $bframe = $mw->Frame(-relief=>'ridge', borderwidth=>2)->pack(-side=>'bottom', -fill=>'both');

#Send command button
our $sendButton = $bframe->Button(-text => " Send Command ",
                                  -state => 'disabled',
                                  -command => sub{ sendCommand("SEND") }
                                 )->pack(-side=>'left', -pady=>6, -ipadx=>12, -padx=>50);

#Record command button
our $recButton = $bframe->Button(-text => 'Record Command',
                                 -state => 'disabled',
                                 -command => sub{ sendCommand("RECORD") }
                )->pack(-side=>'right', -pady=>6, -ipadx=>12, -padx=>50);

# Get command data to form references to the parameter values
paramInit(); #Parameter("");

$tree->autosetmode();

$mw->repeat(2000=> \&getResponse);

MainLoop;

#===============================================================================
# Subroutines used to get params and to send a command to the target
#===============================================================================
#
sub getResponse {

   my $token = shift;
   my $cmd = "";
   my $fmt = "%6s %s: %s\n";

   if (defined($token)) { #called from sendCommand
      $cmd = $token->{cmd};
      $statusWin->insert('end', sprintf($fmt, "HIT", $cmd, "SENT"), 'DEFAULT');
      push(@respToken, $token);
      return;
      #print "DELAYED_RESPONSE_ID: $token->{token}->{DELAYED_RESPONSE_ID}\n";
   } else { #called from repeat
      $token = shift @respToken;
      return if (!defined($token));
      $cmd = $token->{cmd};
   }

   my $err_bak = $ErrorHandling;
   $ErrorHandling = $ERR_NONE;

   my $resp = $obj57->GetDelayedResponse( {token=>$token, timeout=>1000} );

   $ErrorHandling = $err_bak;

   if (!defined($resp)) {
      push(@respToken, $token);
      return;
   }

   my $status = sprintf("0x%.4x",$resp->{status});
   my $level = ($resp->{status}>>12)&0xf;

   if ($level==0xf) {
      $level = 'HIT';
      if (defined $APSSubSystem57CmdData::RspData->{hit}->{status}->{$status}) {
         $status = $APSSubSystem57CmdData::RspData->{hit}->{status}->{$status};
      }
   } elsif ($level < @statusLevel) {
      $level = $statusLevel[$level];
      my ($key1, $key2) = split(/_/,$cmd, 2);
      if (defined $APSSubSystem57CmdData::RspData->{$key2}->{status}->{$status}) {
         $status = $APSSubSystem57CmdData::RspData->{$key2}->{status}->{$status};
      } elsif (defined $APSSubSystem57CmdData::RspData->{$key1}->{status}->{$status}) {
         $status = $APSSubSystem57CmdData::RspData->{$key1}->{status}->{$status};
      }
   } else {
      $level = 'DEFAULT';
   }

   #if ($status !~ /HITCMD_REPLACED/) {
      $statusWin->insert('end', sprintf($fmt,$level,$cmd,$status), $level);
   #}

   if ($obj57->HasMoreResponses({token=>$token})) {
      push(@respToken, $token);
   } elsif ($status !~ /HITCMD_REPLACED/) {
      $statusWin->insert('end', sprintf($fmt,"HIT",$cmd,"DONE"), 'DEFAULT');
   }

   $statusWin->see('end');
} #getResponse

sub validateInput {
   #my ($prop, $char, $curr, $index, $action, @etc) = @_;
   my $bytes = shift;
   my $prop = shift;

   my $len = length(sprintf("%x", ($prop=~/^0/)?oct($prop):$prop ));

   return ($prop =~ /^(0*|0x[0-9a-fA-F]*|[1-9]+[0-9]*)$/ && $len <= 2*$bytes);
} #validateInput

sub invalidInput {
   my $param = shift;
   my $bytes = shift;
   my $prop = shift;

   $mw->messageBox(-title => 'Error',
                   -message => sprintf("$param should be a decimal or a hex value of max %d bytes\n\nProposed value %s is invalid\n", $bytes, $prop),
                   -type => 'OK', -icon=>'error');
} #invalidInput

sub paramEntry {

   $paramFrame->destroy if Tk::Exists($paramFrame);
   $hitFrame->destroy if Tk::Exists($hitFrame);

   $pathSel = $tree->selectionGet;

   my ($testapp, $cmd);

   if ($pathSel =~ /(.*)\.(.*)/) {
      ($testapp, $cmd) = ($1, $2);
      $sendButton->configure(-state=>'normal') if (!$offline);
      $recButton->configure(-state=>'normal');
   } else {
      $sendButton->configure(-state=>'disabled');
      $recButton->configure(-state=>'disabled');
      return;
   }

   my $cmd_desc = $APSSubSystem57CmdData::CmdData->[$tree->infoData($pathSel)]->{cmdspec}->{paramspec}->{params}->[1];
   my $row=0;
   my $column=0;

   $paramFrame = $mw->LabFrame(-label=>$cmd, -labelside=>'acrosstop')->pack(-side=>'top', -fill=>'both');

   foreach my $param_desc (@{$cmd_desc}) {

      my $param = $param_desc->[0];
      my $width = 2*$param_desc->[2] + 2;
      my $bytes = $param_desc->[2];

      if( @$param_desc>4 && $param_desc->[4]==HIDDEN ) {
         next;
      } elsif ($param=~/^hit_cmd_delay$/) {
         if ($hit_queue_mode!=0) {
            $hitFrame = $mw->Frame()->pack(-side=>'top', -before=>$paramFrame);
            $hitFrame->Label()->grid(-row=>0,-column=>0);
            $hitFrame->Label(-text=> "$param:")
                        ->grid(-sticky=>'e',-row=>1,-column=>0);

            $hitFrame->Entry(-textvariable => \$CmdData->{$cmd}->{$param},
                                -validate => 'key', -width=>$width, 
                                -validatecommand => sub { validateInput($bytes, @_); }, 
                                -invalidcommand => sub { invalidInput($param, $bytes, @_); } )
                        ->grid(-sticky=>'e', -row=>1, -column=>1);
         }

         next;
      }

      if ($param_desc->[3] == 1) {

         $paramFrame->Label(-text=> "$param:")
                     ->grid(-sticky=>'e',-row=>$row,-column=>$column);

         $paramFrame->Entry(-textvariable => \$CmdData->{$cmd}->{$param},
                             -validate => 'key', -width=>$width, 
                             -validatecommand => sub { validateInput($bytes, @_); }, 
                             -invalidcommand => sub { invalidInput($param, $bytes, @_); } )
                     ->grid(-sticky=>'e', -row=>$row, -column=>$column+1);
      } else {
         my $max = $param_desc->[3] - 1;

         $paramFrame->Label(-text=> "$param\[0-$max\]:")
                     ->grid(-sticky=>'e', -row=>$row, -column=>$column);

         for (my $i=0, my $col=$column+1; $i<=$max; $i++, $col++) {
            if ( ($col-$column) > 6) {
               $col = $column+1;
               $row++;
            }

            $paramFrame->Entry(-textvariable => \$CmdData->{$cmd}->{$param}->[$i],
                                -validate => 'key', -width=>$width, 
                                -validatecommand => sub { validateInput($bytes, @_); }, 
                                -invalidcommand => sub { invalidInput("$param\[$i\]", $bytes, @_); }
                                )->grid(-sticky=>'e', -row=>$row, -column=>$col);

         }
      }

      if ( ++$row >= 19) {
         $row = 0;
         $column += 2;
         $paramFrame->Label(-text=>"      ")->grid(-row=>0, -column=>$column++);
      }
   } #foreach $param_desc

} #paramEntry

sub sendCommand {
   my $mode = shift;
   my $value=0;
   my %param_data = ();
   my ($testapp, $cmd);

   if ($pathSel =~ /(.*)\.(.*)/) {
      ($testapp, $cmd) = ($1, $2);
   } else {
      return;
   }

   my $record_line = "\$token = hit::$cmd ( {";

   my $cmd_desc = $APSSubSystem57CmdData::CmdData->[$tree->infoData($pathSel)]->{cmdspec}->{paramspec}->{params}->[1];

   foreach my $param_desc (@{$cmd_desc}) {

      next if( @$param_desc>4 && $param_desc->[4]==HIDDEN ); 

      my $param = $param_desc->[0];

      $record_line .= "\n\t\t$param => ";

      if ($param_desc->[3] == 1) {
         $value = $CmdData->{$cmd}->{$param};
         $param_data{$param} = $value=~/^0/ ? oct($value):$value;
         $record_line .= "$value,";
      } else {
         for (my $i=0; $i<$param_desc->[3]; $i++) {
            $value = $CmdData->{$cmd}->{$param}->[$i];
            $param_data{$param}->[$i] = $value=~/^0/ ? oct($value):$value;
         }
         $record_line .= "[". join(",",@{$CmdData->{$cmd}->{$param}}) . "],";
      } 

   } #foreach $param

   $record_line .= "},\n\t\t{blocking => $FALSE}\n\t\t);\n";
   $record_line .= "push(\@respToken, \$token);\n";

   if ($hit_queue_mode == 0) {
      $record_line .= "\$resp = hit::PrintResponse({token=>\$token});\n\n";
   } else {
      print "\n";
   }

   if ($mode =~ /^SEND$/) {
      my $token = $obj57->$cmd({%param_data}, {blocking => $FALSE});

      getResponse($token);

      push(@last10, $record_line);

      if (@last10 >= 10) {
         shift @last10;
      }
   } elsif ($mode =~ /^RECORD$/) {
      if (!Tk::Exists($recText)) {
         $rw = MainWindow->new;
         $rw->title("Record Command");
         $recText = $rw->Scrolled("TextUndo", -scrollbars=>'osoe',
                                   -background => 'white',
                                   -width => 60
                                   )->pack();
         $recText->insert('end', "$recHeader\n");
      }

      $recText->insert('end', "$record_line\n");
   }

} #sendCommand


sub paramInit {
   my ($testapp, $cmd, $param, $value);

   for( my $i=1; $i < @APSSubSystem57Metadata::CMD_57; $i++ ){
   
      $testapp = $APSSubSystem57Metadata::CMD_57[$i]->[0]->[0];

      $testapp =~ s/^hit_generic_icd_//;
      $testapp =~ s/_command$//;

      $tree->add( $testapp, -text=>$testapp,
                  -data=>$APSSubSystem57Metadata::CMD_57[$i]->[1]->[2]->[5]  #testapp ID/SUBSYS_CMD_CODE
                 );
    }
         
   for( my $cmd_idx=0; $cmd_idx < @{$APSSubSystem57CmdData::CmdData} ; $cmd_idx++) {
   
      my $cmd_desc = $APSSubSystem57CmdData::CmdData->[$cmd_idx]->{cmdspec}->{paramspec}->{params};
      my $cmd = $cmd_desc->[0]->[0];

      $CmdData->{$cmd} = 0;
      $testapp = $APSSubSystem57CmdData::CmdData->[$cmd_idx]->{diagcmd};
      $testapp =~ s/^hit_generic_icd_//;
      $testapp =~ s/_command$//;
      $tree->add( "$testapp.$cmd", -text=>$cmd, -data=>$cmd_idx );
         
      foreach my $param_desc (@{$cmd_desc->[1]}) {
   
         $param = $param_desc->[0];
   
         next if( @$param_desc>4 && $param_desc->[4]==HIDDEN ); 

         if ($param_desc->[3] == 1) {
            $CmdData->{$cmd}->{$param} = "0";
         } else {
            #$CmdData->{$cmd}->{$param} = 0;
            for (my $i=0; $i<$param_desc->[3]; $i++) {
               $CmdData->{$cmd}->{$param}->[$i] = "0";
            }
         }      
      } #foreach $param
   } #foreach $cmd

   paramLoad("last.hit");
} #paramInit

our @param_file_type = (["Param Files", '.hit'], ["All Files", "*"]);

sub paramLoad {
   my $file = shift;

   my ($cmd, $param);
   my @arr = ();

   if ($file=~/^$/) {
     $file = $mw->getOpenFile(-title => 'Load',
                              -filetypes => \@param_file_type,
                              -defaultextension => '.hit');
     if (!defined($file)) {
        return;
     }
   }

   if (!open (FILE, "<$file")) {
      $mw->messageBox(-title => 'Error',
                      -message => "Unable to open file $file",
                      -type => 'OK', -icon=>'error');
      return;
   }

   while ($line = <FILE>) {
      chop $line;
      if ($line =~ s/^-//) {
         ($param,$value) = split(/=/, $line);
         if ($value =~ /,/) {
            @arr = split(",", $value);
            for (my $i=0; $i<@arr; $i++) {
               if ($arr[$i]!~/^$/ && defined($CmdData->{$cmd}->{$param}->[$i])) {
                  $CmdData->{$cmd}->{$param}->[$i] = $arr[$i];
               }
            }
         } elsif ($value!~/^$/ && defined($CmdData->{$cmd}->{$param})) {
            $CmdData->{$cmd}->{$param} = $value;
         }
      } else {
         $cmd = $line;
      }
   } #while

   close(FILE);
} #paramLoad

sub paramSave {
   my $file = shift;

   my ($cmd, $param);

   if ($file=~/^$/) {
      $file = $mw->getSaveFile(-title => 'Save as',
                               -filetypes => \@param_file_type,
                               -defaultextension => '.hit');
      if (!defined($file)) {
         return;
      }
   }

   if (!open (FILE, ">$file")) {
      $mw->messageBox(-title => 'Error',
                      -message => "Unable to write to file $file",
                      -type => 'OK', -icon=>'error');
      return;
   }

   for( my $cmd_idx=0; $cmd_idx < @{$APSSubSystem57CmdData::CmdData} ; $cmd_idx++) {

      my $cmd_desc = $APSSubSystem57CmdData::CmdData->[$cmd_idx]->{cmdspec}->{paramspec}->{params};
      $cmd = $cmd_desc->[0]->[0];
      print FILE "$cmd\n";

      foreach my $param_desc (@{$cmd_desc->[1]}) {

         next if( @$param_desc>4 && $param_desc->[4]==HIDDEN ); 

         $param = $param_desc->[0];

         if ($param_desc->[3] == 1) {
            print FILE "-$param=$CmdData->{$cmd}->{$param}\n";
         } else {
            print FILE "-$param=".join(",",@{$CmdData->{$cmd}->{$param}})."\n";
         }      
      } #foreach $param
   } #foreach $cmd

   close(FILE);
} #paramSave


sub Last10 {
   my $rw = MainWindow->new;
   $rw->title("Last 10 Commands");
   my $rec_text = $rw->Scrolled("TextUndo", -scrollbars=>'osoe',
                             -background => 'white',
                             -width => 60
                             )->pack();
   $rec_text->insert('end', "$recHeader\n");

   foreach my $record_line (@last10) {
      $rec_text->insert('end', "$record_line\n\n");
   }   
} #Last10


#==============================================================================
#  Exit Test Case
#==============================================================================
END {
   paramSave ("last.hit");
   ExitTestCase();                  # required - DO NOT REMOVE
}


1;

#==========================================================================
#
# QUALCOMM Proprietary
#
# Export of this software and/or technology may be controlled
# by the U.S. Government.  Diversion contrary to U.S. law
# prohibited.
#
# Copyright (c) 2004 by QUALCOMM Incorporated. All Rights Reserved.
#
# All data and information contained in or disclosed by this document
# is confidential and proprietary information of QUALCOMM Incorporated
# and all rights therein are expressly reserved.  By accepting this
# material the recipient agrees that this material and the information
# contained therein is held in confidence and in trust and will not be
# used, copied, reproduced in whole or in part, nor its contents
# revealed in any manner to others without the express written
# permission of QUALCOMM Incorporated.
#
# End of Module
#
# The following is the module terminator - this statement is required
#==========================================================================

