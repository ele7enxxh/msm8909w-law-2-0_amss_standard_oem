###########################################################################
# Copyright 2008 QUALCOMM Incorporated All Rights Reserved
###########################################################################

use Win32::OLE;
use Win32::OLE::Variant;
use warnings;
#use strict;

#my @command = ( 0x4B, 0x56,0x01,0x00,0x42,0x0,0x0,0x0,0xFF,0x01,0x0,0x0,k,i,n,a,0x00);
my @command = ( 0x4B, 0x56,0x01,0x00,0x42,0x0,0x0,0x0,0xFF,0x01,0x0,0x0,0x68,0x69,0x6E,0x61,0x00);

# AppId for the Automation server.
$prod_id = "QPSTAtmnServer.Application";

# Show (1) the $prod_id GUI or hide (0) it?
$show_gui = 1;

# Attempt to use a running instance.
eval
{
  $qpst = Win32::OLE->GetActiveObject($prod_id)
};

die "$prod_id not installed" if $@;

# Start a new instance. Call Quit when $qpst set to undef or script exits.
unless (defined $qpst)
{
  $qpst = Win32::OLE->new($prod_id, sub {$_[0]->Quit;}) or die "Oops, cannot start $prod_id";
}


##########################################################################
# Start Command Line Processing
# Command Line Options, if valid will skip menu.
##########################################################################

#############################################################
# Show the Automation server GUI. Off by default.
#############################################################
if ($show_gui)
{
  $qpst->ShowWindow();

  print "Select a mobile on the GUI and press enter to continue...";
  <STDIN>;
}

if (defined $qpst)
{
   my $port_list = $qpst->GetPortList();

   #$port_selected = &pick_a_port($port_list, $qpst);

  # Find port selected on Automation server GUI.
  $port_selected = $qpst->GetSelectedPort();

  if (defined $port_selected)
  {
   $port_selected_name = $port_selected->PortName;
   print "Port $port_selected_name selected\n";

  }

  # (1) quit unless we have the correct number of command-line args
  $num_args = $#ARGV + 1;
  if ($num_args != 2) 
  {
    print "\nUsage: ssm_perm_file_commands.pl r/w ssm_perm_file_name \n";
    exit;
  }

  # we got two command line args
  if ( $ARGV[0] eq "r" )
  {
    ssm_read_perm_file( $ARGV[1] );
  }
  else
  {
    if ( $ARGV[0] eq "w" )
    {
      ssm_write_perm_file( $ARGV[1] );
    }
    else
    {
      print "\nUsage: ssm_perm_file_commands.pl r/w ssm_perm_file_name \n";
      exit;
    }
  }
}

# Code the have user input shut down GUI. Only needed if you want to see GUI after loading/reading
#if ($show_gui)
#{
#  print "Press enter to exit script";
#  <STDIN>;
#}

# Sleep for two seconds before releasing QPST Automation
sleep 2;

# Release the automation server.
undef $qpst;
print "Done!\n";

########################################
# Local Subroutines
########################################
# 
# 
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

#################################################################################
##### Subroutine SSM_READ_PERM_FILE - routine to read file in SSM #####

sub ssm_read_perm_file
{
  print " \n\nRead perm file $_[0]\n\n";

  $fd = ssm_open_perm_file($_[0]);
    
  #Try Writing perm file
  #Command structure for writing perm file
  @command = ( 0x4B, 0x56,0x03,0x00 );

  @command_check = @command;
    
  #Location at which file needs to be read
  $offset = 0; 
    
  #Length of buffer to be read
  $buf_len = 99;
    
  #Buffer where file read is copied to
  my @buf;

  #Push FD into command structure
  @arr = ();
  @arr = ssm_int_to_hex_ordering($fd);
    
  #print "\n\narr = @arr\n";

  push ( @command, @arr );

  #print "\n#####Read Command : @command\n";
  
  #Try opening perm file 
  open FILE, ">>", $_[0] or die;
      
  #Read Perm file
  do
  {
    @read_command = @command;

    @arr = ();
    @arr = ssm_int_to_hex_ordering($buf_len);

    #print "\n\noffset_arr = @arr";

    push ( @read_command, @arr );

    @arr = ();
    @arr = ssm_int_to_hex_ordering($offset);
	
    #print "\n Length_arr : @arr ";

    push ( @read_command, @arr );

    print "\n#####Read Command : @read_command "; 
        
    #Calculate length of command structure
    $pack_arg = scalar(@read_command);

    #print "\n Length of command : $pack_arg "; 
	
    #pack command into a string
    $RequestString = pack ( sprintf("C%u",$pack_arg) , @read_command );

    $variant = Variant(VT_ARRAY | VT_UI1, 164);

    $variant->Put($RequestString);

    #Send Command and get Response back
    $Response = $port_selected->SendCommand($variant,20000);
    
    #print "\n\nResponse : $Response\n";

    #Unpack Response packet
    ( $init, $fd, $offset_read, $bytes_read, $error_no, @buf ) = unpack("IiIiiC*", $Response );

    #Keep warnings at bay */
    $offset_read = 0;
    
    my @init_check = ssm_int_to_hex_ordering($init);
    
    if ( ssm_compare_array(\@command_check, \@init_check) == 0 )
    {
      print " \n Read Error response : command_check = @command_check , init_check = @init_check" ;
      return 0;
    }

    #Check if read to file failed
    if ( $error_no != 0 )
    {
      print "\n\nERROR : File read failed : error no = $error_no \n ";
      return 0;
    }

    @buf = map{ chr($_) } @buf;

    print "\nBytes read : @buf ";

    print FILE @buf;

    #Set offset = buf_len and new Buffer length to zero
    $offset = $offset + $bytes_read;

  }while ( $buf_len == $bytes_read );

  
  ssm_close_perm_file( $fd );

  close FILE;

  return 0;
}

#################################################################################
##### Subroutine SSM_WRITE_PERM_FILE - routine to write file in SSM #####

sub ssm_write_perm_file
{
  print " \n\nWrite perm file $_[0]\n\n";

  #Try opening perm file 
  open FILE, $_[0] or die;

  $fd = ssm_open_perm_file($_[0]);

  #Try Writing perm file
  #Command structure for writing perm file
  @command = ( 0x4B, 0x56,0x04,0x00 );

  @command_check = @command;
    
  #Location at which file needs to be written
  $offset = 0; 
    
  #Length of buffer
  $buf_len = 0;
    
  #Number of bytes read from the file
  my $n;
    
  #Buffer where file read is copied to
  my @buf;

  #Push FD into command structure
  @arr = ();
  @arr = ssm_int_to_hex_ordering($fd);
    
  #print "\n\narr = @arr\n";

  push ( @command, @arr );

  #print "\n#####WRITE Command : @command\n";
      
  #Write Perm file
  while( ($n = read FILE, $data, 1) != 0)
  {
    $buf_len = $buf_len + $n;
    push(@buf, ord($data));

    #Check for buffer length .. it is limited to 99 Bytes
    if ( $buf_len >= 0x63 )
    {
      @write_command = @command;
      #print "\n Offset : $offset Length :$buf_len \n";

      #Push offset to write command
      @arr = ();
      @arr = ssm_int_to_hex_ordering($offset);
      push ( @write_command, @arr );

      #Push buffer length to write command
      @arr = ();
      @arr = ssm_int_to_hex_ordering($buf_len);
      push ( @write_command, @arr );

      #Push write buffer into command structure
      push ( @write_command, @buf);

      print "\n#####Write Command : @write_command "; 
        
      #Calculate length of command structure
      $pack_arg = scalar(@write_command);

      #print "\n Length of command : $pack_arg "; 
	
      #pack command into a string
      $RequestString = pack ( sprintf("C%u",$pack_arg) , @write_command );

      $variant = Variant(VT_ARRAY | VT_UI1, 164);

      $variant->Put($RequestString);

      #Send Command and get Response back
      $Response = $port_selected->SendCommand($variant,20000);
      #print "\n\nResponse : $Response\n";

      #Unpack Response packet
      ( $init, $fd, $offset_written, $bytes_written, $error_no ) = unpack("IiIii", $Response );

      my @init_check = ssm_int_to_hex_ordering($init);
    
      if ( ssm_compare_array(\@command_check, \@init_check) == 0 )
      {
        print " \n Write Error response : command_check = @command_check , init_check = @init_check" ;
        return 0;
      }

      #Check if write to file failed
      if ( $error_no != 0 )
      {
        print "\n\nERROR : File write failed : error no = $error_no \n ";
        return 0;
      }

      #print " \nAfter write : offset_written = $offset_written ";
      #print " \nAfter write : bytes_written  = $bytes_written ";
      #print " \nAfter write : buf_len = $buf_len ";
      #print " \nAfter write : error_no = $error_no ";

      #Increment offset with buf_len and new Buffer length to zero
      $offset = $offset + $buf_len;
      $buf_len = 0;
      @buf = ();
    }
  }
  if ( $buf_len > 0 )
  {
    @write_command = @command;
    #print "\n Offset : $offset Length :$buf_len \n";

    #Push offset to write command
    @arr = ();
    @arr = ssm_int_to_hex_ordering($offset);
    push ( @write_command, @arr );

    #Push buffer length to write command
    @arr = ();
    @arr = ssm_int_to_hex_ordering($buf_len);
    push ( @write_command, @arr );

    #Push write buffer into command structure
    push ( @write_command, @buf);

    print "\n#####Write Command : @write_command "; 
        
    #Calculate length of command structure
    $pack_arg = scalar(@write_command);

    #print "\n Length of command : $pack_arg "; 
	
    #pack command into a string
    $RequestString = pack ( sprintf("C%u",$pack_arg) , @write_command );

    $variant = Variant(VT_ARRAY | VT_UI1, 164);

    $variant->Put($RequestString);

    #Send Command and get Response back
    $Response = $port_selected->SendCommand($variant,20000);
    #print "\n\nResponse : $Response\n";

    #Unpack Response packet
    ( $init, $fd, $offset_written, $bytes_written, $error_no ) = unpack("IiIii", $Response );

    my @init_check = ssm_int_to_hex_ordering($init);
    
    if ( ssm_compare_array(\@command_check, \@init_check) == 0 )
    {
      print " \n Write Error response : command_check = @command_check , init_check = @init_check" ;
      return 0;
    }

    #Check if write to file failed
    if ( $error_no != 0 )
    {
      print "\n\nERROR : File write failed : error no = $error_no \n ";
      return 0;
    }
  }
  
  #print " \nAfter write : offset_written = $offset_written ";
  #print " \nAfter write : bytes_written  = $bytes_written ";
  #print " \nAfter write : buf_len = $buf_len ";
  #print " \nAfter write : error_no = $error_no ";

  ssm_close_perm_file($fd);

  close FILE;

  return 0;
}

#################################################################################
##### Subroutine SSM_INT_TO_HEX_ORDERING - routine to convert integer to diag commad format#####

sub ssm_int_to_hex_ordering
{
  my @hex_ordering=();
  
  $temp = $_[0]; 
  for ($i=0 ; $i < 4 ; $i++ )
  {
    push( @hex_ordering, $temp%0x100 );
    $temp=$temp/0x100;
  }

  return @hex_ordering;  
} 
#ssm_int_to_hex_ordering

#################################################################################
##### Subroutine SSM_COMPARE_ARRAY - routine to compare two arrays #####
sub ssm_compare_array
{
  
  ($array1, $array2) = @_;

  if ( scalar(@{$array1}) != scalar (@{$array2}) )
  {
    return 0;
  }
  $a = scalar(@{$array1});
  
  for ( $i =0; $i< $a ; $i++ )
  {
    if( $array1->[$i] != $array2->[$i] )
    {
      return 0;
    }
  }

  #keep warnings at bay 
  $array2 = 0;
  return 1;

} 
#ssm_compare_array

#################################################################################
##### Subroutine SSM_OPEN_PERM_FILE - routine to open perm file on efs #####

sub ssm_open_perm_file
{
  #Initial Command to open SSM perm file 
  my @command = ( 0x4B, 0x56,0x01,0x00 );
    
  my @open_o_flag = (0x42,0x0,0x0,0x0 );
    
  my @open_mode = ( 0xFF,0x01,0x0,0x0 );

  my @command_check = @command;

  #Push open_o_flag and mode in commands structure
  push( @command, @open_o_flag );
  push( @command, @open_mode );
       
  #Append file name to command 
  my @arr = split(//,$_[0]);

  @arr = map{ ord($_) } @arr;
   
  #Push file name to command structure
  push(@command, @arr);

  #Length of argument 
  my $pack_args = scalar(@command);

  print "\n#####OPEN Command : @command\n";
    
  #Pack the command into a structure
  my $RequestString = pack( sprintf( "C%u", $pack_args ), @command);

  my $variant = Variant(VT_ARRAY | VT_UI1, 164);

  $variant->Put($RequestString);

  #Send the command and get response
  my $Response = $port_selected->SendCommand($variant,20000);
    
  #print "\n\nResponse : $Response \n";

  #Unpack Response packet
  my ( $init, $fd_open, $error_no ) = unpack("IiI", $Response );

  my @init_check = ssm_int_to_hex_ordering($init);
    
  if ( ssm_compare_array(\@command_check, \@init_check) == 0 )
  {
    print " \n Open Error response : command_check = @command_check , init_check = @init_check" ;
    exit;
  }

  #Check if file opening failed
  if ( $error_no != 0 )
  {
    print "\nERROR : File opening failed : error no = $error_no \n ";
    exit;
  }

  #Check if fd is correct
  if ( $fd_open < 0 )
  {
    print "\nERROR : File opening failed : fd = $fd_open \n ";
    exit;
  }

  #print "\n\n fd = $fd_open\n";
  #print "\n\n error_no = $error_no\n";

  return fd_open;

} 
# ssm_open_perm_file

#################################################################################
##### Subroutine SSM_CLOSE_PERM_FILE - routine to close perm file on efs #####

sub ssm_close_perm_file
{
  #Command Structure to close the file 
  @command = (0x4B, 0x56,0x02,0x00);

  @command_check = @command;

  #Push FD into command structure
  @arr = ();
  @arr = ssm_int_to_hex_ordering($fd);
  push ( @command, @arr );

  print "\n\n#####CLOSE Command : @command\n";

  #Calculate length of command structure
  $pack_arg = scalar(@command);
    
  #pack command into a string
  $RequestString =  pack ( sprintf("C%u",$pack_arg) , @command );

  $variant = Variant(VT_ARRAY | VT_UI1, 164);

  $variant->Put($RequestString);
  #Send command and get response
  $Response = $port_selected->SendCommand($variant,20000);

  #Unpack Response packet
  my ( $init, $error_no ) = unpack("Ii", $Response );

  my @init_check = ssm_int_to_hex_ordering($init);
    
  if ( ssm_compare_array(\@command_check, \@init_check) == 0 )
  {
    print " \n Close Error response : command_check = @command_check , init_check = @init_check" ;
    exit;
  }

  #Check if closing of file failed
  if ( $error_no != 0 )
  {
    print "\n\nERROR : File close failed : error no = $error_no \n ";
    exit;
  }
    
  #print "\n\nResponse : $Response \n";
  return 0;

}
#ssm_close_perm_file
