#    $DateTime: 2016/12/13 07:59:23 $
#    $Author: mplcsds1 $
# 
#   Edit History:
#   YYYY-MM-DD   who   why
#   -----------------------------------------------------------------------------
#   2011-10-06   nr    Support Zipped Tar files and command line options.
#   2011-09-21   wek   Support different OS file write mode.
#   2011-07-13   wek   William Kimberly - first release
# 
#   Copyright 2011 by QUALCOMM, Incorporated.  All Rights Reserved.
# 
# ===============================================================================
#
use strict;
use Win32::OLE;
use Win32::OLE::Variant;
use Getopt::Long;

my $qpst;
my $port_selected;
my $port_list;
my $help;

my @EFS_Subcmd = (0x4B, 19);

#$global_seq - used for sequence number handling
my $global_seq = 0xA55A;

sub get_phone
{
   my $show_gui = shift;
   my $prod_id = "QPSTAtmnServer.Application";

   eval
   {
     $qpst = Win32::OLE->GetActiveObject($prod_id);
   };

   die "$prod_id not installed" if $@;

   # Start a new instance. Call Quit when $qpst set to undef or script exits.
   unless (defined $qpst)
   {
     $qpst = Win32::OLE->new($prod_id, sub {$_[0]->Quit;}) or die "Oops, cannot start $prod_id";
   }

   if($show_gui)
   {
      $qpst->ShowWindow();
      print "press enter to continue";
      <STDIN>;

      $port_selected = $qpst->GetSelectedPort() or die "Selected port unavailable" if(!$port_selected);
   }
   else
   {
      my $port_list = $qpst->GetPortList();

      $port_selected = &pick_a_port($port_list, $qpst);
      if($port_selected == -1)
      {
         die "\nNo phone found!!";
      }
   }
   return $port_selected;
}

  sub DelayMS {
   my $ms=$_[0];
   select undef,undef,undef,$ms/1000;
   }


###############################################################################
##### Subroutine PICK_A_PORT - returns an available port to which phone
#is connected #####
sub pick_a_port
{
   $port_list = $qpst->GetPortList();
   my $phone_count = $port_list->PhoneCount;

   # Translate phone mode to string.
   my %phone_mode_list = qw (
           0 modeNone
           1 modeDataServices
           2 modeDownload
           3 modeDiagnostic
           4 modeDiagnosticOffline
           5 modeReset
           6 modeStreamDownload) ;

   # Translate phone status to string.
   my %phone_status_list = qw (
     0 phoneStatusNone
     1 phoneStatusInitializationInProgress
     2 phoneStatusPortNotPresent
     3 phoneStatusPhoneNotPresent
     4 phoneStatusPhoneNotSupported
     5 phoneStatusReady) ;

   my $count;

   foreach $count (0..$phone_count)
   {
      my $port_id = $port_list->PortId($count);
      my $mode = $phone_mode_list{$port_list->PhoneMode($count)};

      my $qpst_port = $qpst->GetPort($port_id);

      # For all ports that have a running phone get the port object.
      if (($mode eq "modeDiagnostic" || $mode eq "modeDiagnosticOffline") && (defined $qpst_port))
      {
          # Get live data from the port object.
          my $port_name_2 = $qpst_port->PortName;
          my $port_label_2 = $qpst_port->PortLabel;
          my $is_usb_2 = $qpst_port->IsUSB;
          my $port_id_2 = $qpst_port->PortId;
          my $mode_2 = $phone_mode_list{$qpst_port->PhoneMode};
          my $status_2 = $phone_status_list{$qpst_port->PhoneStatus};
          my $hex_esn_2 = sprintf("0x%08X", $qpst_port->ESN);

          # Display the state of the port.
          #print "\nPicking phone: $port_id_2 $port_name_2 $is_usb_2 $hex_esn_2 \042$port_label_2\042 $mode_2 $status_2\n\n";
          print "\nPicking phone on port $port_name_2;\nPortID: $port_id_2\nHEX ESN # $hex_esn_2\nPort Label: $port_label_2\n\n";

          return $qpst_port;
      }
   }
}

sub send_diag_open_filesystem_image
{
  my $seq;
  my $RequestString;
  my $variant;
  my @Rsp;
  my $result;
  my ($fs_img_type, $path) = @_;
  my $token;

  $seq = $global_seq++;
  $RequestString = pack ("CCSSCa128",
                         @EFS_Subcmd, # CC
                         0x36, #S
                         $seq, #S
                         $fs_img_type, #C
                         $path); # a128

  $variant = Variant(VT_ARRAY | VT_UI1, 132);
  $variant->Put($RequestString);

  my $Response = $port_selected->SendCommand($variant, 20000);
  @Rsp = unpack "CCSSII", $Response;
  if ($Rsp[4] != 0)
  {
    printf "Error while opening file system image : %05d          ",$Rsp[4];  # Diag error#  
    $result = -1;
  }
  else
  {
    $result = $Rsp[5];
  }
  return $result;
}

sub send_diag_read_filesystem_image
{
  my $seq;
  my $RequestString;
  my $variant;
  my @Rsp;
  my $result;
  my ($handle) = @_;
  my $token;

  $seq = $global_seq++;
  $RequestString = pack ("CCSSI",
                         @EFS_Subcmd, # CC
                         0x37, #S
                         $seq, #S
                         $handle); # I

  $variant = Variant(VT_ARRAY | VT_UI1, 10);
  $variant->Put($RequestString);

  my $Response = $port_selected->SendCommand($variant, 20000);
  @Rsp = unpack "CCSSIIa*", $Response;
  if ($Rsp[5] != 0)
  {
    printf "Error while reading file system image: %05d          ",$Rsp[5];  # Diag error#  
    $result = -1;
  }
  else
  {
    $result = $Rsp[4];
  }

  return ($result, $Rsp[6]);
}

sub send_diag_close_filesystem_image
{
  my $seq;
  my $RequestString;
  my $variant;
  my @Rsp;
  my $result = 0;
  my ($handle) = @_;
  my $token;

  $seq = $global_seq++;
  $RequestString = pack ("CCSSI",
                         @EFS_Subcmd, # CC
                         0x38, #S
                         $seq, #S
                         $handle); # I

  $variant = Variant(VT_ARRAY | VT_UI1, 10);
  $variant->Put($RequestString);

  my $Response = $port_selected->SendCommand($variant, 20000);
  @Rsp = unpack "CCSSI", $Response;
  if ($Rsp[4] != 0)
  {
    printf "Error while closing file system image : %05d          ",$Rsp[4];  # Diag error#  
    $result = -1;
  }

  return $result;
}

sub set_help_sub
{
  print "Unknown arguments detected \n";
  $help = 1;
}

sub help_sub
{  
  print "\n  This module generates File system images in format specified\n";
  print "\n  -t|--tar ********************** Tar format (DEFAULT).";
  print "\n  -z|--ziptar ******************* Gzipped tar format.";
  print "\n  -o|--output [FILENAME] ******** Specify output file name.";
  print "\n  -p|--path [PATH] ************** Specify path on phone to extract";
  print "\n  -h|-?|--help ****************** Print this help message and exit";
  print "\n";
  print "\n  NOTE : -t|--tar and -z|--ziptar cannot be used together.";
  print "\n";
  die "\n";
}


###############################################################################
################################ START OF EXECUTION ###########################


###############################Command Line Options ###########################

my $path = "/";
my $result = 0;
my $handle = 0;
my $data;
my $file_name = 0;
my $tar_diag = 0;
my $zip_tar_diag = 1;
my $diag_code = -1;
my $file_type_tar = 0;
my $file_type_zip_tar = 0;
$help = 0;

Getopt::Long::Configure ("no_auto_abbrev");
$result = GetOptions ("tar|t" => \$file_type_tar, # file type is a tar (default)
            "ziptar|z" => \$file_type_zip_tar, # file type is a zipped tar
            "output|o=s" => \$file_name, # Optional output file name
            "path|p=s" => \$path, #The path to extract
            "help|h|?" => \$help, #help flag superceeds other options
            "<>" => \&set_help_sub, # Call help for unknown args
            ); 

            
if($help || $result != 1)
{
  help_sub();
}         
if($file_type_tar && $file_type_zip_tar)
{
  help_sub();
}          

if(!$file_type_tar && !$file_type_zip_tar)
{
  print "\n No Command line options give assuming defaults and generating tar";
  print "\n use (-h|-?|--help) for help\n";
  $file_type_tar = 1;
}

if($file_type_tar)
{
  $diag_code = $tar_diag;
  if($file_name eq 0)
  {    
    $file_name = "fs_image.tar";
    print "\n No filename given using default file name ".$file_name. " \n";
  }
}
else
{
  $diag_code = $zip_tar_diag;
  if($file_name eq 0)
  {
    $file_name = "fs_image.tar.gz";
    print "\n No filename given using default file name ".$file_name. " \n";
  }
}



#################################### Main Script ##############################

#Call get_phone to obtain a port object for the script
#Set $gui to 1 to pick a specific port from the QPST Gui or leave it as zero to pick the first available phone

my $gui = 0;
get_phone($gui);

$handle = send_diag_open_filesystem_image ($diag_code, $path); #create a image based on diag packet , on $path

if ($handle < 0)
{
  die "Failed to open the file system image through diag.";
}

open (FSIMAGE, "> ".$file_name);
binmode (FSIMAGE);

my @pretty_output = ("|","/","-","\\");
my $pretty_out_len = @pretty_output;
my $pretty_index = 0;

print "\n Extracting  Image :  ";
do
{
  ($result, $data) = send_diag_read_filesystem_image ($handle);
  if ($result < 0)
  {
    printf "Failed not read file system image chunk, image may be invalid.";
  }  
  syswrite FSIMAGE, $data;  
  printf "\b".$pretty_output[++$pretty_index % $pretty_out_len];   
}while ($result > 0);

printf "\b DONE!\n";
close (FSIMAGE);

$result = send_diag_close_filesystem_image ($handle);
if ($result < 0)
{
  die "Failed to close the file system image through diag.";
}

printf "Done extracting file system image to file ".$file_name."\n"

