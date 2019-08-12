#******************************************************************************
#* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg/mcfg_gen/scripts/mcfg_write.pm#1 $
#* $DateTime: 2016/12/13 07:58:21 $
#*
#* 
#******************************************************************************
#*
#* Copyright (c) 2015 Qualcomm Technologies, Inc.
#* All rights reserved.
#* Qualcomm Technologies, Inc. Confidential and Proprietary.
#*
#******************************************************************************
package MCFGWrite;

# Includes
#use Text::ParseWords; # not needed?
use Time::localtime;
use Math::BigInt;
use File::Find;
use Data::Dumper;
use XML::Parser;
use File::Copy;
use Fcntl 'SEEK_END';
use File::Path qw(mkpath);
use File::Spec qw(canonpath);
use File::Basename qw(fileparse);

our $mcfg_format = 3;


# Global package variables 
our $BUILD_ROOT = "data\\";

# $^O variable contains OS name
my $os = $^O; 

# Sizes are listed in bytes
my $BYTE   = 1;
my $INT_16 = 2;
my $INT_32 = 4;
my $INT_64 = 8;

my $NULL_CHAR_LENGTH   = $BYTE;
my $INDEX_LENGTH       = $BYTE;
my $SUB_ID_LENGTH      = $BYTE;

# GenerateMcfgSegmentFileXml $output_file_types
# These are constants representing output file types
# supported by the merged GenerateMcfgSegmentFileXML subroutine
# in an effort to eliminate some of the duplicated code
$BINARY_OUTPUT_FILE_TYPE = 1;
$DEFAULT_C_OUTPUT_FILE_TYPE = 2;
$GENERIC_C_OUTPUT_FILE_TYPE = 3;
$MULTI_MBN_OUTPUT_FILE_TYPE = 4;

# MCFG_HEADER specific elements
my $MCFG_ITEM_LENGTH   = $INT_32;
my $NV_EFS_TYPE_LENGTH = $BYTE; # Identifies whether the item is NV or EFS
my $ATTRIBUTES_LENGTH  = $BYTE; 
my $EXTRA_BYTE_LENGTH  = $BYTE;
my $NV_NUM_EFS_ITEM_LENGTH = $INT_16;
my $PAYLOAD_SIZE_LENGTH    = $INT_16;
my $CARRIER_INDEX_LENGTH   = $BYTE; # Carrier index is used for multiplexed items, with corresponding data already on target

# header elemnts added for an EFS entry
my $EFS_FILENAME_HEADER_LENGTH = $INT_16; #indicates if there is a filename present
my $EFS_PATH_LENGTH = $INT_16;

#  GENERIC_MCFG_HEADER  = 8 Bytes
my $GENERIC_MCFG_HEADER = $MCFG_ITEM_LENGTH  + $NV_EFS_TYPE_LENGTH + $ATTRIBUTES_LENGTH + 
                          $EXTRA_BYTE_LENGTH + $EXTRA_BYTE_LENGTH;
#  
#  MCFG_HEADER_LENGTH  = 12 Bytes
my $MCFG_HEADER_LENGTH = $GENERIC_MCFG_HEADER + $NV_NUM_EFS_ITEM_LENGTH + $PAYLOAD_SIZE_LENGTH;

#  NV_HEADER_LENGTH  = 12 Bytes
my $NV_HEADER_LENGTH = $MCFG_HEADER_LENGTH;

#  EFS_HEADER_LENGTH  = 17 Bytes
my $EFS_HEADER_LENGTH = $MCFG_HEADER_LENGTH + $EFS_FILENAME_HEADER_LENGTH + $EFS_PATH_LENGTH + $NULL_CHAR_LENGTH;

# TRL related defs
my $TRL_OFFSET_LENGTH    = $INT_32;
my $TRL_MAGIC_NUM_LENGTH = $INT_64; # A magic number is pushed to the front of the TRL payload

#  TRL_HEADER_LENGTH  = 12 Bytes 
my $TRL_HEADER_LENGTH = $MCFG_HEADER_LENGTH;

#  TRL_FOOTER_LENGTH = 4 Bytes
# Note: Padding should not be included in the footer, only the offset. Padding is calculated after 
#      the terminal $mcfg_item->{DataSize} has been added to length of generated array. Then that lenght itself
#      has padding added to it
my $TRL_FOOTER_LENGTH = $TRL_OFFSET_LENGTH;

# PRL related defs
my $NAM_LENGTH           = $BYTE; 
my $PRL_VER_LENGTH       = $INT_16;
my $PRL_SIZE_BITS_LENGTH = $INT_32;
my $PRL_ID = 257;
my $BOOL_LENGTH          = $BYTE; 

#  PRL_SUBHEADER_LENGTH  =  8 bytes
my $PRL_SUBHEADER_LENGTH = $NAM_LENGTH + $PRL_VER_LENGTH + $PRL_SIZE_BITS_LENGTH + $BOOL_LENGTH;

# Common Regexs used 
my $SPACES      = ' +';
my $WHITESPC    = '\s+';
my $STRING      = 'string';
my $INTEGER     = 'int';
my $INT_ARR     = 'int\[\]';
my $INDEXED_ARR = 'int\[\d+\]';
my $SCALAR_EXPR = "^($INTEGER|$INT_ARR|$INDEXED_ARR|$STRING)\$";

# NOTE: May need to fine tune regular expressions
my $UINT           = '\d+';
my $SIGNED_INT     = "-?$UINT";
my $GEN_DELIM      = "($WHITESPC)?(,|-)($WHITESPC)?"; # Generic delimiter for list elements can be currently either a comma or hypen followed by 0 or more spaces
my $COMMA_DELIM    = ",($SPACES)?"; # Comma delimiter for list elements is a comma followed by 0 or more spaces
my $DELIM_SEP_INTS = "$SIGNED_INT((($GEN_DELIM)($SIGNED_INT))+)?"; 
my $COMMA_SEP_INTS = "$SIGNED_INT((($SPACES)?,($SPACES)?$SIGNED_INT)+)?";
my $PLMN           = "$UINT-$UINT";
my $PLMN_LIST      = "$PLMN(($COMMA_DELIM)($PLMN))*";

# Values assigned to different variant types
my $variantValues = {
   "efs" => 2,
   "group" => 2,
   "efs_item" => 2,
   "efs_dir" => 2,
   "slot_nv_item" => 3,
   "slot_efs" => 4,
   "slot_efs_item" => 4,
   "slot_efs_dir" => 4,
   "data_profile" => 5,
   "config" => 6,
   "active_config" => 7,
};

# Names of value types mapped to their column indices in the cooresponding tables
my $COLUMN_INDICES = {
   MCFG_ITEM => 
   {
     SCALAR => {
       "ID"    => 1,
       "Size"  => 3,
       "Value" => 5
     },
     PRL_FILE => {
       "ID"       => 1,
       "Version"  => 0,
       "Filename" => 4
     }
   },
   EFS_FILE => {
     "EFS Item Size" => 4,
     "EFS Item Type" => 3,
     "Filepath"      => 1,
     "Filename"      => 7,
     "Value"         => 5
   },
   GROUP_FILE => {
     "Version"      => 1,
     "Filename"      => 7,
   },
   TRL_RECORD => {
     "Size"   => 4,
     "Type"   => 3,
     "Value"  => 5
   },
   Identifier => 1,
   Type       => 2,
   Attributes => 6
};

# List of items that are not allowed to be included in MCFG configuration. Edit list at own risk
my @blacklisted_storage_items = (
  69, 70, 91, 95, '121-140', 146, '149-159', 194, '199-208', '330-370', '376-397', '431-439', '585-706', '726-812', '856-874',
  '948-1013', '1033-1189', '1210-1301', '1312-1340', '1351-1790', 1877, '1898-1903',
  '2080-2085',
  '3253-3282', '3386-3388', 3515, 3632, 4257, 4548, 6828, 6838, 
  '/nv/item_files/rfnv/...',
  '/nv/item_files/mcfg/mcfg_autoselect_by_uim',
);

# Common defaults used throughout code
my $DFLT_ACTIVE_SUBS = 0x07; # Represents subs 0,1,2 set to active
my $DFLT_SUBS_SIZE = 1; # Represents subs 0,1,2 set to active
my $DFLT_SUBS_TYPE = 'int'; # Represents subs 0,1,2 set to active

# Variables related to MBN generation
my $ELF_BLOCK_ALIGN = 0x1000;
my $Elf32_Ehdr = { 
  e_ident      => '7f 45 4c 46 01 01 01 00 00 00 00 00 00 00 00 00',
  e_type       => '02 00',        # 0x02
  e_machine    => '00 00',        # 0x00
  e_version    => '01 00 00 00', 	# 0x01 =	Current version
  e_entry      => '00 00 00 00', 	# 0x0
  e_phoff      => '34 00 00 00',	# 0x34	program header table's file offset in bytes
  e_shoff      => '00 00 00 00', 	# 0x00	section header table's file offset in bytes
  e_flags      => '05 00 00 00', 	# 0x04
  e_ehsize     => '34 00',        # 0x34	ELF header's size in bytes.
  e_phentsize  => '20 00',        # 0x20	size in bytes of one entry in the file's program header table
  e_phnum      => '03 00',        # 0x03	number of entries in the program header table
  e_shentsize  => '28 00',        # 0x28	section header's size in bytes
  e_shnum      => '00 00',        # 0x00	number of entries in the section header table
  e_shstrndx   => '00 00',        # 0x00	section header table index of the entry associated with the section name string table
};

my $new_phdr = {
  p_type       => '00 00 00 00', # 0x0    This member tells what kind of segment this array element describes or how to interpret the array element's information
  p_offset     => '00 00 00 00', # 0x0000 This member gives the offset from the beginning of the file at which the first byte of the segment resides             
  p_vaddr      => '00 00 00 00', # 0x0    This member gives the virtual address at which the first byte of the segment resides in memory                         
  p_paddr      => '00 00 00 00', # 0x0    On systems for which physical addressing is relevant, this member is reserved for the segment's physical address       
  p_filesz     => '94 00 00 00', # 0x94   This member gives the number of bytes in the file image of the segment                                                 
  p_memsz      => '00 00 00 00', # 0x0    This member gives the number of bytes in the memory image of the segment                                               
  p_flags      => '00 00 00 07', # 0x7000000    This member gives flags relevant to the segment                                                                        
  p_align      => '00 00 00 00', # 0x0    This member gives the value to which the segments are aligned in memory and in the file                                
};

my $hash_phdr = {
  p_type       => '00 00 00 00', # 0x0    This member tells what kind of segment this array element describes or how to interpret the array element's information
  p_offset     => '00 10 00 00', # 0x1000 This member gives the offset from the beginning of the file at which the first byte of the segment resides             
  p_vaddr      => '',            #        Value changes according to MCFG segment size. This member gives the virtual address at which the first byte of the segment resides in memory                         
  p_paddr      => '',            #        Value changes according to MCFG segment size. On systems for which physical addressing is relevant, this member is reserved for the segment's physical address       
  p_filesz     => '88 00 00 00', # 0x88   This member gives the number of bytes in the file image of the segment                                                 
  p_memsz      => '00 10 00 00', # 0x1000  This member gives the number of bytes in the memory image of the segment                                               
  p_flags      => '00 00 20 02', # 0x2200000   This member gives flags relevant to the segment                                                                        
  p_align      => '00 10 00 00', # 0x1000 This member gives the value to which the segments are aligned in memory and in the file                                
};

my $Elf32_Phdr = {
  p_type       => '01 00 00 00', # 0x1    This member tells what kind of segment this array element describes or how to interpret the array element's information
  p_offset     => '00 20 00 00', # 0x2000 This member gives the offset from the beginning of the file at which the first byte of the segment resides             
  p_vaddr      => '00 00 00 00', # 0x0    This member gives the virtual address at which the first byte of the segment resides in memory                         
  p_paddr      => '00 00 00 00', # 0x0    On systems for which physical addressing is relevant, this member is reserved for the segment's physical address       
  p_filesz     => '',            #        (Size of MCFG segment) This member gives the number of bytes in the file image of the segment 
  p_memsz      => '',            #        (Size of MCFG segment) This member gives the number of bytes in the memory image of the segment
  p_flags      => '06 00 00 00', # 0x6    This member gives flags relevant to the segment                                                                        
  p_align      => '04 00 00 00', # 0x04   This member gives the value to which the segments are aligned in memory and in the file                                
};

my $hash_hdr = {
  image_id         => '00 00 00 00',
  flash_parti_ver  => '03 00 00 00',
  image_src        => '00 00 00 00',
  image_dest_ptr   => '',       #        Value changes according to MCFG segment size. 
  image_size       => '60 00 00 00',
  code_size        => '60 00 00 00',
  sig_ptr          => '',       #        Value changes according to MCFG segment size. 
  sig_size         => '00 00 00 00',
  cert_chain_ptr   => '',       #        Value changes according to MCFG segment size. 
  cert_chain_size  => '00 00 00 00',
};
 
sub SetBuildRoot($) {
   ($BUILD_ROOT) = @_;
   $BUILD_ROOT = FixBuildRootString("$BUILD_ROOT");
}

# ----------------- START Debugging Related F() ----------------
# Variables used for logging and debugging
my $DEBUG_ENABLE                 = 0;
my $GLOBAL_LOG_ENABLE            = 1; # Disables all logging if cleared, overriding $FORCE_VERBOSE_LOGGING
my $FORCE_VERBOSE_LOGGING        = 0; # Enables all logging output when set

my $LOG_FATAL_ERROR              = (0 <<  0); 

my $DIR_TRAVERSAL_TRACE_ENABLE   = (0 <<  1); 
my $DATASIZE_TRACE_ENABLE        = (0 <<  2); 
my $FILEPATHS_TRACE_ENABLE       = (0 <<  3); 
my $ITEM_PRINTLIST_ENABLE        = (0 <<  4); 
my $PATH_CONVERSION_TRACE        = (0 <<  5); 
my $LOOP_TRACE_ENABLE            = (0 <<  6); 
my $ARG_TRACE_ENABLE             = (0 <<  7); 
                                          
my $ALL_EXTRACTION_TRACE         = (0 <<  8); 
my $EFS_EXTRACTION_TRACE_ENABLE  = (0 <<  9); 
my $NV_EXTRACTION_TRACE_ENABLE   = (0 << 10); 

my $ITEM_DELETE_TRACE_ENABLE     = (0 << 11); 


sub IsLoggingEnabled {
   my $log_enable = shift;

   return ( ($log_enable || $FORCE_VERBOSE_LOGGING) && $GLOBAL_LOG_ENABLE );
}

sub Log {
   my $log_type  = shift;
   my $statement = shift; 
   my @args = @_;

   if (IsLoggingEnabled( $log_type )) 
   {
     print "$statement:\n";
     foreach (@args) {
        print Dumper($_);  # The dumper module can handle references as well
     }
   }
}

# Traces the number of items, and total size of the mcfg_cust_data_sw[] array
sub TraceMcfgList
{
   my ($length_bytes, $num_mcfg_items, $loop_count) = @_;

   Log($ITEM_PRINTLIST_ENABLE, "\n Loop count: $loop_count, mcfg item tally: '$num_mcfg_items'");
   Log($DATASIZE_TRACE_ENABLE, "current mcfg_cust_data_sw[] size: '$length_bytes'");
}
 
# ----------------- END Debugging Related F() ----------------

# Generate the code to add the passed PRL to the MCFG variable.
sub parse_nv_prl_bin {
   my($prl_filename, $prl_item_attrib, $prl_item_type, $carrier_id) = @_;
   my $prl_buf;
   my $prl_item_length;
   my $prl_length;
   my $nv_prl_length;
   my $nv_prl_size;
   my $prl_byte;
   my $byte_count=0; 
   my $prl_header_length;
   # Open the PRL file. 
   open(PRL_FH, "< $prl_filename") or die ("Could not open PRL file: $!");

   # The PRL is a binary file.
   binmode(PRL_FH);

   # Read the first four bytes from the file. It's the length and version number of the PRL.
   read(PRL_FH, $prl_buf, 4);

   # Close the PRL file.
   close(PRL_FH);

   # Convert the big-endian length into an array of bytes. 
   my (@prl_hdr) = unpack("C*", $prl_buf);

   # Convert the big endian length to little endian.
   $prl_length =  (($prl_hdr[0])<<8) + $prl_hdr[1];

   # If the carrier_id is defined then add 1 byte to the header for its output
   $prl_header_length = $PRL_SUBHEADER_LENGTH + IsDefined( $carrier_id );
   # Output the MCFG item header: 12B for MCFG header, 8 or 9 for the PRL subheader
   $prl_item_length = $MCFG_HEADER_LENGTH + $prl_header_length + $prl_length;

   WriteGenericMCFGHeaderBin($prl_item_length, $prl_item_attrib, 0x01);

   # Output the item type, NV 257 (0x0101).
   if (IsDefined($carrier_id))
   {
     $prl_item_type += 40000; 
   }
   binwrite( pack('I', $prl_item_type), 2);

   # Output the length, add 8 bytes for the PRL header.
   $nv_prl_length = $prl_header_length + $prl_length;
   binwrite( pack('I', $nv_prl_length), 2);

   if (IsDefined($carrier_id))
   {
     binwrite( pack('I', $carrier_id), 1);
   }

   # Output the PRL header. 
   # 1. byte NAM = 0x00
   # 2. word PRL version = the second 16-bit value of the PRL file.
   # Output the version in little endian.
   binwrite( pack('C*', 0x00, $prl_hdr[3], $prl_hdr[2]));

   # 3. dword PRL size in bits.
   binwrite( pack('I', $prl_length * 8), 4);

   # 4. boolean valid = 0x01
   binwrite( pack('C', 0x01));

   # Read the entire PRL contents now and output the roaming list element into MCFG file.
   # Open the PRL file. 
   open(PRL_FH, "< $prl_filename") or die "Could not open prl file $prl_filename: $!";

   # The PRL is a binary file.
   binmode(PRL_FH);

   # Read the entire PRL contents.
   read(PRL_FH, $prl_buf, $prl_length);

   # Output contents from PRL file itself
   binwrite($prl_buf);

   # Close the PRL file.
   close(PRL_FH);
}

sub parse_nv_prl {
   my($prl_filename, $prl_item_attrib, $prl_item_type, $carrier_id) = @_;
   my $prl_buf;
   my $prl_item_length;
   my $prl_length;
   my $nv_prl_length;
   my $nv_prl_size;
   my $prl_byte;
   my $byte_count=0; 
   my $prl_header_length;
   # Open the PRL file. 
   open(PRL_FH, "< $prl_filename") or die ("Could not open PRL file: $!");

   # The PRL is a binary file.
   binmode(PRL_FH);

   # Read the first four bytes from the file. It's the length and version number of the PRL.
   read(PRL_FH, $prl_buf, 4);

   # Close the PRL file.
   close(PRL_FH);

   # Convert the big-endian length into an array of bytes. 
   my (@prl_hdr) = unpack("C*", $prl_buf);

   # Convert the big endian length to little endian.
   $prl_length =  (($prl_hdr[0])<<8) + $prl_hdr[1];

   # If the carrier_id is defined then add 1 byte to the header for its output
   $prl_header_length = $PRL_SUBHEADER_LENGTH + IsDefined( $carrier_id );
   # Output the MCFG item header: 12B for MCFG header, 8 or 9 for the PRL subheader
   $prl_item_length = $MCFG_HEADER_LENGTH + $prl_header_length + $prl_length;
   printf "\n /* PRL */";
   printf "\n ,0x%02X ,0x%02X ,0x%02X ,0x%02X",( $prl_item_length      & 0xFF), (($prl_item_length>>8)  & 0xFF), 
                                               (($prl_item_length>>16) & 0xFF), (($prl_item_length>>24) & 0xFF);

   # Output the item attributes.
   printf " ,0x%02X" , 0x01;
   printf " ,0x%02X" , $prl_item_attrib & 0xFF;
   printf " ,0x%02X" , 0x00;
   printf " ,0x%02X" , 0x00;

   # Output the item type, NV 257 (0x0101).
   if (IsDefined($carrier_id))
   {
     $prl_item_type += 40000; 
   }
   printf "\n ,0x%02X" ,( $prl_item_type     & 0xFF);
   printf " ,0x%02X"   ,(($prl_item_type>>8) & 0xFF);

   # Output the length, add 8 bytes for the PRL header.
   $nv_prl_length = $prl_header_length + $prl_length;
   printf " ,0x%02X" ,( $nv_prl_length     & 0xFF);
   printf " ,0x%02X" ,(($nv_prl_length>>8) & 0xFF);

   if (IsDefined($carrier_id))
   {
     printf " ,0x%02X", $carrier_id;
   }

   # Output the PRL header. 
   # 1. byte NAM = 0x00
   # 2. word PRL version = the second 16-bit value of the PRL file.
   # Output the version in little endian.
   printf " ,0x00 ,0x%02X ,0x%02X" , $prl_hdr[3], $prl_hdr[2];

   # 3. dword PRL size in bits.
   # 4. boolean valid = 0x01
   $nv_prl_size = $prl_length * 8;
   printf " ,0x%02X ,0x%02X ,0x%02X ,0x%02X ,0x01\n" ,( $nv_prl_size      & 0xFF), (($nv_prl_size>>8)  & 0xFF),
                                                      (($nv_prl_size>>16) & 0xFF), (($nv_prl_size>>24) & 0xFF);

   # Read the entire PRL contents now and output the roaming list element into MCFG file.
   # Open the PRL file. 
   open(PRL_FH, "< $prl_filename") or die "Could not open prl file $prl_filename: $!";

   # The PRL is a binary file.
   binmode(PRL_FH);

   # Read the entire PRL contents.
   read(PRL_FH, $prl_buf, $prl_length);

   # Unpack the PRL contents.
   my (@prl_contents) = unpack("C*", $prl_buf);

   # Close the PRL file.
   close(PRL_FH);

   # Output the roaming list element into MCFG file.
   foreach $prl_byte (@prl_contents) 
   {
     printf " ,0x%02X", $prl_byte;
     $byte_count++;
     if ( ( ( $byte_count % 8 ) == 0 ) && ( $byte_count != scalar @prl_contents ) )
     {
       printf "\n";
     }
   }

   printf "\n";
}

sub binwrite
{
  my ($value, $size) = @_;

  # Move filehandle to end of file (in case other fh was opened)
  sysseek(OUT_FILE, 0, SEEK_END);

  if (defined($size))
  {
    if (($size =~ /^$UINT$/) && ($size > 0))
    {
      if(!syswrite(OUT_FILE, $value, $size)) 
      {
        print STDOUT "\nCaller info:\n\n";
        print STDOUT Dumper(caller);
        die("Error: Write failure of $size bytes to output\n");
      }
    }
  }
  else
  {
    if(!syswrite(OUT_FILE, $value)) 
    {
      print STDOUT "\nCaller info:\n\n";
      print STDOUT Dumper(caller);
      die("Error: Write failure to output\n");
    }
  }
}

# Takes a packed value and returns the space 
# delimited string representation of the data
sub unpackToHexString($$)
{
  my ($packed_val, $size) = @_;

  if (!defined($size))
  {
    die ("Error: unpackToHexString needs size of bytes")
  }

  return sprintf("%02X " x $size, unpack('C' x $size, $packed_val));
}

# Generate the code to add the passed EFS file to the MCFG variable.
sub parse_efs_file_bin
{
   my($efs_filename, $attributes, $efs_full_path, $efs_payload_length, 
     $mcfg_output_length, $item_variant, $efs_elements) = @_;
   my $file_path_index=0;
   my $byte_count=0;
   my $efs_file_buf;
   my @efs_file_contents = ();
   my $efs_read_size = 0;
   my $filename_length = length($efs_full_path) + $NULL_CHAR_LENGTH;
   my $item_length;

   # For an item marked delete or SW only the efs_read_size = 0  
   if ( ! ItemIsDeleteOrSWOnly($attributes)) {
      # Open the EFS file from the efs_files subdirectory.
      open(UNDP_EFS_FH, "< $efs_filename")
          or die "Could not open or create temporary file $efs_filename $!";

      # Assume the EFS file is a binary file.
      binmode(UNDP_EFS_FH);

      # Read the entire EFS file, up to 64K. Save the size of the file.
      $efs_read_size = read(UNDP_EFS_FH, $efs_file_buf, 65535);

      # Close the EFS file.
      close(UNDP_EFS_FH);
   }

   # Output the MCFG header. The item length is the MCFG header size (12), plus
   # the length of the null-terminated file path, plus the EFS header size (4), 
   # plus the length of the EFS file.
   WriteEFSHeaderBin($efs_read_size, $mcfg_output_length, $attributes, $efs_full_path, $item_variant);
#  # Output the item data header. EFS file data = 0x0002.
   binwrite(pack('C*', 0x02, 0x00));
 
   # Output the length of the file data. efs_read_size isn't used becuase
   # there may be metadata added to the efs payload
   binwrite(pack('I', $efs_payload_length), 2);

   #print STDOUT "parse_efs_file_bin: \n\t" . Dumper($efs_elements);
   # Output any corresponding metadata. Loop not entered if none exists
   foreach my $datum (@{ $efs_elements })
   {
     if(IsMetaData($datum))
     {
       binwrite(pack('C*', $datum->{Value}->[0]), $datum->{Size});
     }
   }

   # Output the file data itself. Do not attempt to write 
   binwrite($efs_file_buf, $efs_read_size);
   
   return ($byte_count);
}

sub parse_efs_file
{
   my($efs_filename, $attrib, $efs_full_path, $efs_payload_length, 
     $mcfg_output_length, $item_variant, $efs_elements) = @_;
   my $file_path_index=0;
   my $byte_count=0;
   my $efs_file_buf;
   my @efs_file_contents = ();
   my $efs_read_size = 0;
   my $filename_length = length($efs_full_path) + $NULL_CHAR_LENGTH;
   my $item_length;

   # Output the EFS file comment header.
   printf "\n /* EFS file: ";
   printf " " . GetFilenameFromPath($efs_filename, "\\") .  " , Size = ";

   # For an item marked delete or SW only the efs_read_size = 0  
   if ( ! ItemIsDeleteOrSWOnly($attrib)) {
      # Open the EFS file from the efs_files subdirectory.
      open(UNDP_EFS_FH, "< $efs_filename")
          or die "Could not open or create temporary file $efs_filename $!";

      # Assume the EFS file is a binary file.
      binmode(UNDP_EFS_FH);

      # Read the entire EFS file, up to 64K. Save the size of the file.
      $efs_read_size = read(UNDP_EFS_FH, $efs_file_buf, 65535);

      # Close the EFS file.
      close(UNDP_EFS_FH);
   }

   # Ouput the length of the file to the EFS file comment header.
   print $efs_read_size;
   print " bytes ";

   # Outut the full path of the file in the EFS to the EFS file comment header.
   print "\n   Path on Disk: ";
   print $efs_full_path;   
   print " */ \n";

   # Output the code to include the EFS file in the MCFG souce file.
   # Output the MCFG header. The item length is the MCFG header size (12), plus
   # the length of the null-terminated file path, plus the EFS header size (4), 
   # plus the length of the EFS file.
   printf "\n ,0x%02X ,0x%02X ,0x%02X ,0x%02X",( $mcfg_output_length      & 0xFF), (($mcfg_output_length>>8)  & 0xFF), 
                                               (($mcfg_output_length>>16) & 0xFF), (($mcfg_output_length>>24) & 0xFF);

   # Output the passed attributes of the EFS file.
   printf " ,0x%02X ,0x%02X ,0x%02X ,0x%02X", $item_variant, ($attrib & 0xFF), 0x00, 0x00;
   $byte_count+=8;

   # Output the file name header. EFS file name = 0x0001.
   print "\n ,0x01, 0x00";

   # Output the length of the filename (filename + 1 for null char terminator).
   printf " ,0x%02X" ,( $filename_length     & 0xFF);
   printf " ,0x%02X" ,(($filename_length>>8) & 0xFF);
   $byte_count+=4;

   # Ouptput the filename plus the null character terminator.
   my (@efs_filepath_details) = unpack("C*", $efs_full_path);
   foreach $efs_filepath_byte (@efs_filepath_details) 
   {
     printf " ,0x%02X", $efs_filepath_byte;
     $byte_count++;
     if ( ($byte_count % 8) == 0 ) 
     {
       printf "\n";
     }
   }
   print " ,0x00";
   $byte_count++;
   
   # Output the item data header. EFS file data = 0x0002.
   print "\n ,0x02, 0x00";

   # Output the length of the file data.
   printf " ,0x%02X" ,( $efs_payload_length     & 0xFF);
   printf " ,0x%02X" ,(($efs_payload_length>>8) & 0xFF);
   $byte_count = 4;

   if ( ! ItemIsDeleteOrSWOnly($attrib)) {
      # Unpack the file data.
      (@efs_file_contents) = unpack("C*", $efs_file_buf);
   }

   foreach my $datum (@{ $efs_elements })
   {
     if(IsMetaData($datum))
     {
       #assume metadata elements are all 1B ea
       $byte_count++;
       printf " ,0x%02X", $datum->{Value}->[0];
     }
   }

   # Output the file data. 
   foreach $efs_file_byte (@efs_file_contents) 
   {
     printf " ,0x%02X", $efs_file_byte;
     $byte_count++;
     if ( ( ( $byte_count % 8 ) == 0 ) && ( $byte_count != (scalar @efs_file_contents) + 4 ) )
     {
       printf "\n";
     }
   }

   printf "\n";

   return ($byte_count);
}

# efs_index -- not used
# efs_type -- 1,2,4,8, 5,6 indicating which value type is contained in the value
# efs_item_value -- the value
# attrib -- attributes
# efs_filename -- the name
# efs_full_path -- the full path
sub create_efs_file
{
   my($efs_index, $efs_type, $efs_item_value, $attrib, $efs_filename, $efs_full_path, $item_variant) = @_;
   my $file_path_index=0;
   my $efs_file_buf;
   my $efs_read_size;
   my $mcfg_item_length;
   my $byte_count = 0;
   # Output the EFS file comment header.
   printf "\n /* EFS file: ";
   printf " " . GetFilenameFromPath($efs_filename, "\\") . " , Size = "; 

   # Ouput the length of the file to the EFS file comment header.
   print $efs_type;
   print " bytes ";

   # Outut the full path of the file in the EFS to the EFS file comment header.
   print "\n   Path on Disk: ";
   print $efs_full_path;   
   print " */ \n";

   my $efs_path_length = length($efs_full_path); # Length of NULL terminator already calculated into EFS_HEADER_LENGTH
   # Output the code to include the EFS file in the MCFG souce file.
   $mcfg_item_length = $EFS_HEADER_LENGTH + $efs_path_length + $efs_type; 
   printf "\n ,0x%02X ,0x%02X ,0x%02X ,0x%02X", ($mcfg_item_length       & 0xFF), (($mcfg_item_length>>8)  & 0xFF), 
                                                (($mcfg_item_length>>16) & 0xFF), (($mcfg_item_length>>24) & 0xFF);

   # Output the passed attributes of the EFS file.
   printf " ,0x%02X ,0x%02X ,0x%02X ,0x%02X", $item_variant, $attrib & 0xFF, 0x00, 0x00;

   # Output the file name header. EFS file name = 0x0001.
   print "\n ,0x01, 0x00";

   # Output the length of the filename + 1 for null char terminator.
   printf " ,0x%02X" ,( $efs_path_length     & 0xFF);
   printf " ,0x%02X" ,(($efs_path_length>>8) & 0xFF);

   # Set the byte count to allow for the header type and length.
   $byte_count = 4;

   # Ouptput the filename plus the null character terminator.
   my (@efs_filepath_details) = unpack("C*", $efs_full_path);
   foreach $efs_filepath_byte (@efs_filepath_details) 
   {
     printf " ,0x%02X", $efs_filepath_byte;
     $byte_count++;
     if ( ($byte_count % 8) == 0 ) 
     {
       printf "\n";
     }
   }
   print " ,0x00";
   
   # Output the item data header. EFS file data = 0x0002.
   print "\n ,0x02, 0x00";

   # Output the length of the file data.
   printf " ,0x%02X" ,($efs_type & 0xFF);
   printf " ,0x%02X" ,(($efs_type>>8) & 0xFF);

   my $temp_val;

   # If the number is in hexidecimal format...
   if ($efs_item_value =~/^0x/) 
   {
     # ...Read the field as a hexidecimal number.
     $temp_val = hex($efs_item_value);
   }
   else
   {
     # ...Read the field and let Perl decide the format.
     $temp_val = $efs_item_value;
   }

   # If the field type is 1, the payload of this NV item is a 8-bit value.
   if ($efs_type == 1) 
   {
     printf " ,0x%02X" ,($temp_val & 0xFF);
   }

   # If the field type is 2, the payload of this NV item is a 16-bit value.
   if ($efs_type == 2) 
   {
     printf " ,0x%02X ,0x%02X", ($temp_val & 0xFF), (($temp_val >> 8) & 0xFF);
   }

   # If the field type is 4, the payload of this NV item is a 32-bit value.
   if ($efs_type == 4) 
   {
     printf " ,0x%02X ,0x%02X ,0x%02X ,0x%02X",
      ($temp_val & 0xFF), (($temp_val >> 8) & 0xFF), (($temp_val >> 16) & 0xFF), (($temp_val >> 24) & 0xFF);
   }

   # If the field type is 8, the payload of this NV item is a 64-bit value.
   if ($efs_type == 8) 
   {
     # Values larger than unit32 are stored as a reference to an array of bytes
     if (ref($efs_item_value) eq 'ARRAY') {
      # First array element equals '0x'. This pushes the last index out to size instead of size - 1, 
      # with respect to the size of the data being stored in the array.
      for my $index (1..$#{ $efs_item_value }) {
         printf " ,0x%02X", hex($efs_item_value->[$index]);
      }
     }
     else {
       use Math::BigInt;
       $temp_val = Math::BigInt->new($efs_item_value);
       printf " ,0x%02X ,0x%02X ,0x%02X ,0x%02X",
        ($temp_val & 0xFF), (($temp_val >> 8) & 0xFF), (($temp_val >> 16) & 0xFF), (($temp_val >> 24) & 0xFF);
       $temp_val = Math::BigInt->new($efs_item_value) >> 32;
       printf " ,0x%02X ,0x%02X ,0x%02X ,0x%02X",
        ($temp_val & 0xFF), (($temp_val >> 8) & 0xFF), (($temp_val >> 16) & 0xFF), (($temp_val >> 24) & 0xFF);
     }
   }

   printf "\n";

   return ($byte_count);
}

sub WriteELFHeaderBin() {
  my $hdr = '';
  my $hdr_sz, $pad_sz;

  # Concatenate the ELF header bytes. 
  foreach my $item ($Elf32_Ehdr, @Elf32_Phdr)
  {
    my ($key, $value) = each %$item;
    $hdr .= " $value"; 
  }
  # calculate sizes for headers and associated padding to 4K boundary
  $hdr_sz = scalar split(/\s+/, Trim($hdr));
  $pad_sz = 0x1000 - $hdr_sz;
  # Remove spaces in header string
  $hdr =~ s/\s+//g;

  # Write entire header
  binwrite( pack('H*', $hdr));
  # NULL pad the rest of the memory block
  binwrite( pack('H2' x $pad_sz, 0x00));
}

sub WriteELFSegmentBin($) {
  my ($seg) = @_;

  $seg =~ s/\s+//g;
  binwrite( pack('H*', $seg));
}

sub WriteEFSHeaderBin($$$$$) {
   my ($payload_length, $item_length, $attributes, $filepath, $item_variant) = @_;
   my $filename_length = length($filepath) + $NULL_CHAR_LENGTH; 

   # Make the default item_variant type a regular efs file. Check that item is defined just to be safe
   if (!defined($item_variant) || $item_variant == 0) {
      $item_variant = 2;
   }

   # Output the MCFG header. 
   WriteGenericMCFGHeaderBin($item_length, $attributes, $item_variant);
   # The item length is the MCFG header size (12), plus
   # the length of the null-terminated file path, plus the EFS header size (4), 
   # plus the length of the EFS file.
 
   # Output the file name header. EFS file name = 0x0001.
   binwrite( pack('C*', 0x01, 0x00));
 
   # Output the length of the filename (filename + 1 for null char terminator).
   binwrite( pack('I', $filename_length), 2);
 
   # Ouptput the filename plus the null character terminator.
   binwrite( $filepath);
   binwrite( pack('C', 0x00));
}

sub WriteEFSHeader($$$$$) {
   my ($payload_length, $item_length, $attributes, $filepath, $item_variant) = @_;
   my $filename_length = length($filepath) + $NULL_CHAR_LENGTH; 

   # Make the default item_variant type a regular efs file. Check that item is defined just to be safe
   if (!defined($item_variant) || $item_variant == 0) {
      $item_variant = 2;
   }

   # Output the EFS file comment header including file size
   printf "\n /* EFS file:  " . GetFilenameFromPath($filepath, "/") . " , Size = $payload_length bytes ";

   # Outut the full path of the file in the EFS to the EFS file comment header.
   print "\n   Path on Disk: $filepath */ \n";

   # Output the code to include the EFS file in the MCFG souce file.
   # Output the MCFG header. The item length is the MCFG header size (12), plus
   # the length of the null-terminated file path, plus the EFS header size (4), 
   # plus the length of the EFS file.
   printf "\n ,0x%02X ,0x%02X ,0x%02X ,0x%02X", ByteNOf($item_length, 1), ByteNOf($item_length, 2), 
                                                ByteNOf($item_length, 3), ByteNOf($item_length, 4);

   # Output the passed attributes of the EFS file.
   printf " ,0x%02X ,0x%02X ,0x%02X ,0x%02X", $item_variant, $attributes, 0x00, 0x00;

   # Output the file name header. EFS file name = 0x0001.
   print "\n ,0x01, 0x00";

   # Output the length of the filename + 1 for null char terminator.
   printf " ,0x%02X" , ByteNOf($filename_length, 1);
   printf " ,0x%02X" , ByteNOf($filename_length, 2);

   # Set the byte count to allow for the header type and length.
   my $byte_count = 4;

   # Ouptput the filename plus the null character terminator.
   my (@efs_filepath_details) = unpack("C*", $filepath);
   foreach my $efs_filepath_byte (@efs_filepath_details) 
   {
     printf " ,0x%02X", $efs_filepath_byte;
     $byte_count++;
     if ( ($byte_count % 8) == 0 ) 
     {
       printf "\n";
     }
   }
   print " ,0x00\n";
}

sub WriteScalarHeader($$$) {
   my ($item_length, $attributes, $item_variant) = @_;

   # write item length
   printf " ,0x%02X ,0x%02X ,0x%02X ,0x%02X", ByteNOf($item_length, 1), ByteNOf($item_length, 2), 
                                              ByteNOf($item_length, 3), ByteNOf($item_length, 4);
   # write item type, attributes, and two extra bytes
   printf " ,0x%02X ,0x%02X ,0x%02X ,0x%02X\n", $item_variant, ByteNOf($attributes, 1), 0, 0;

}

sub WriteGenericMCFGHeaderBin($$$)
{
  my ($item_length, $attributes, $item_variant) = @_;

  # write item length
  binwrite( pack('I', $item_length), 4);

  # Output the passed variant, attributes and spare bytes 
  binwrite( pack('C*', $item_variant, ($attributes & 0xFF), 0x00, 0x00));

}

sub WriteScalarHeaderBin($$$) {
   my ($item_length, $attributes, $item_variant) = @_;

   WriteGenericMCFGHeaderBin($item_length, $attributes, $item_variant);
}

# Trims leading and trailing whitespace from variable contents. 
sub Trim {
   my ($val) = @_;

   $val =~ s/^\s+|\s+$//g;
   return $val;
}

# Determines if a field is blank (not including whitespace)
sub IsBlank {
   my ($field) = @_;

   if(defined($field)) {
     $field = Trim($field);
   }

   return $field eq '';
}

sub IsSubsMaskPresent
{
  my ($field) = @_;

  if(defined($field)) {
    $field = Trim( lc($field) );
  }

  return ($field =~ m/sub(s)?_mask/);
}

# given a value name and the hash cooresponding to the table type,
# this returns the index of the column
sub GetFieldIndex($$) {
   my ($fieldName, $fieldMap) = @_;

   return $fieldMap->{$fieldName};
}

# a helper function that returns the Nth byte (starting from 1)
# of an integer value
sub ByteNOf($$) {
   my ($num, $n) = @_;
   my $shift = (0, 0, 8, 16, 24, 32, 40, 48, 56)[$n];
   my $bigInt = Math::BigInt->new($num);

   return (($bigInt >> $shift) & 0xFF);
}

# a helper function that determines which file to read in for an efs entry
sub GetEFSFilename($$) {
   my ($full_path, $given_filename) = @_;

   $given_filename and return $given_filename;

   return GetFilenameFromPath($full_path, "/");
}

# returns the name of a file from its path. a unix or windows style separator can be
# specified (/ or \)
sub GetFilenameFromPath($$) {
   my ($path, $seperator) = @_;

   my $last_forward_slash_index = rindex($path, $seperator);
   my $filename = substr($path , $last_forward_slash_index + 1);

   return $filename;
}

# Purpose: Convert a hex string to a byte array with size equal to 'size' attribute
# in spreadsheet/database + 1 (for 0x prefix).
sub HexStringToArrayRef($$){
  my ($hex_string, $size) = @_;

  # The unpack function is set up to rely on an even number of characters.
  # If there are an odd number of characters (i.e. nibbles) then prefix string with a '0'
  $hex_string =~ s/^0x/0x0/ if ( (length($hex_string) % 2 ) != 0 );

  my @hex_bytes = reverse unpack("a2" x (length($hex_string) / 2), $hex_string);

  # Return 0x prefix to front of array by popping it from the back and then unshifting it to the front
  unshift (@hex_bytes, pop @hex_bytes);
  
  # If needed, extend array size up to 'size' given in excel sheet (plus 1 for 0x prefix), and pad with bytes of 0x00.
  $#hex_bytes = $size;
  my @hex_bytes_resized = map { defined($_) ? $_ : '0x00' } @hex_bytes;

  return \@hex_bytes_resized;
}

# 

# verify string format for a comma separated list of digits
sub IsDigitList($) {
   my $value_string = shift;

   return $value_string =~ m/^$DELIM_SEP_INTS$/s; 
}

# verify string format for a comma separated list of MCC/MNC combinations 
sub IsPLMNList($) {
   my $value_string = shift;

   {
     #use re 'debug';
     return $value_string =~ m/^$PLMN_LIST$/; 
   }
}

# This function takes the value string and type specifier and returns a perl value
# based on that. It can handle hex formatted numbers as well as normal base ten numbers
# it can also handle fixed sized and variable sized int arrays.
sub ParseValue($$$) {
   my ($value_string, $type, $size) = @_;

   $value_string = Trim($value_string);

   if($type =~ /^int$/) {
     # need to compensate for 2s complement negative numbers
     if(IsHexString($value_string)) {
       # Using unpack when size > 4B since perl's hex() function does not work for numbers > uint32. 
       if ( $size > 4 ) {

         # Example: '0x0102030405060708' -> [ 08, 07, 06, 05, 04, 03, 02, 01, 0x ]
         my $value_ref = HexStringToArrayRef($value_string, $size);
         return $value_ref;
       }
       else { # size <= 4 Bytes
         return hex($value_string);
       }
     }
     elsif($value_string =~ /^\d+\s*$/) {
       # This will not work for numbers > 4B in size
       return int($value_string);
     }
     elsif($value_string =~ /^\-\d+$/) {
       # This will not work for numbers > 4B in size
       my $value = int($value_string) * -1;
       my $final_value = Math::BigInt->new(0);

       for my $which_byte (1 .. $size) {
         my $byte = ~(ByteNOf($value, $which_byte)) & 0xFF;
         $byte = Math::BigInt->new($byte);
         $byte = $byte->blsft(($which_byte-1)*8);
         $final_value = Math::BigInt->new($final_value->badd($byte));
       }

       $final_value = $final_value->badd(1);

       return $final_value;
     }
   }
   elsif($type =~ /int\[\d+\]/ || $type =~ /int\[\]/) {
     my $open_bracket_index  = index($type, '[');
     my $close_bracket_index = index($type, ']');
     my $num_array_elements  = int(substr($type, $open_bracket_index+1, $close_bracket_index - $open_bracket_index));
     my $array_value = [];

     if( IsDigitList($value_string) ) { # verify string format
       my @numbers = ();
       if (IsPLMNList($value_string))
       {
         @numbers = split(/[^\d]+/ ,$value_string);
       }
       else
       {
         @numbers = split(/,\s*/ ,$value_string);
       }

       @numbers = grep { $_ ne '' } @numbers;
       my $last_number = 0;

       for my $number (@numbers) {
         $last_number = ParseValue($number, "int", $size);
         push(@$array_value, $last_number);
       }

       # if it is a fixed buffer size then pad the rest of the values with the last given value
       # check to see if this behavior is necessary -- or whether it would be better to just pad with 0's
       if($type =~ /int\[\d+\]/) {
         my $array_length = scalar @$array_value;
   
         # fill array with the rest of the values
         while($array_length < $num_array_elements) {
            push(@$array_value, $last_number);
            $array_length++;
         }
       }
     }

     return $array_value;
   }
   else {
     return $value_string;
   }
}

# from the attributes of an item, determine if it's software only
sub ItemIsSoftwareOnly($) {
   my ($attributes) = @_;

   $attributes = hex($attributes);

   return not ($attributes & 0x01);
}

sub ItemIsDelete($) {
   my ($attributes) = @_;

   return ($attributes & 0x40);
}

sub ItemIsDeleteOrSWOnly {
   my ($attributes) = @_;

   if ( ItemIsDelete( $attributes ) || ItemIsSoftwareOnly( $attributes )) {
      return 1;
   }
   else {
      return 0;
   }
}

sub ItemIsMuxd($) {
   my ($attributes) = @_;

   return ($attributes & 0x02);
}

sub ItemIsIndexed($) {
   my ($attributes) = @_;

   return ($attributes & 0x20);
}

sub ItemIsSubscriber($) {
   my ($attributes) = @_;

   return ($attributes & 0x10);
}

sub VariantCanRepeat($) {
   my ($variant) = @_;

   # Variant types 3 & 4 correspond to variant types that may be listed multiple times in the spreadsheet
   if ($variant == $variantValues->{slot_nv_item} || $variant == $variantValues->{slot_efs_item})
   {
      return 1;
   }
   else 
   {
      return 0;
   }
}

sub DuplicateItemAllowed($$) {
   my ($item_attrib, $item_variant) = @_;
   my $allowed = 0;

   if (ItemIsIndexed($item_attrib) || ItemIsSubscriber($item_attrib) || VariantCanRepeat($item_variant))
   {
      $allowed = 1;
   }

   return $allowed;
}

# calculates the length of an efs file
sub CalculateEfsElementLength($$$$) {
   my ($efs_full_path, $efs_filename, $value, $size) = @_;

   my  $efs_read_size = 0;  

   if($value) {
     #print "arrived in if value statement\n";
     return $size + length($efs_full_path) + 1 + 4;
   }
   else {
     my $file_path_index=0;
     my $byte_count=0;
     my $efs_file_buf;
   
     # Open the EFS file from the efs_files subdirectory.
     open(UNDP_EFS_FH, "<", "$efs_filename")
         or die "Could not open or create temporary file '$efs_filename' $!";
     binmode(UNDP_EFS_FH);
     $efs_read_size = read(UNDP_EFS_FH, $efs_file_buf, 65535);
     close(UNDP_EFS_FH);

     return ($efs_read_size);
   }
}

# calculates the length of a prl file
sub CalculatePrlElementLength($) {
   my ($prl_filename) = @_;
   my $prl_buf;
   my $prl_length;

   open(PRL_FH, "< $prl_filename") or die "Could not open prl file: " . $prl_filename;
   binmode(PRL_FH);
   read(PRL_FH, $prl_buf, 2);
   close(PRL_FH);

   # Convert the big-endian length into an array of bytes. 
   my (@prl_hdr) = unpack("C*", $prl_buf);

   # Convert the big endian length to little endian. 
   $prl_length =  ((($prl_hdr[0])<<8) + $prl_hdr[1]);

   return $prl_length;
}

# calculates the length of a string, int or int array
sub CalculateScalarElementLength($$$) {
   my ($type, $size, $value) = @_;

   my $row_data_size = 0;
   if($type =~ /^int$/) { # integer value
     $row_data_size = $size;
     # print "++ Calculating size of int to be: $row_data_size\n";
   }
   elsif($type =~ /int\[\]/) {
     $row_data_size = (scalar @$value)*$size;
     #print "++ Calculating size of dynamic int arr to be: $row_data_size\n";
   }
   elsif($type =~ /int\[\d+\]/) { # integer array
     my $array_element_size = $size;

     my $open_bracket_index = index($type, '[');
     my $close_bracket_index = index($type, ']');

     my $num_array_elements = int(substr($type, $open_bracket_index+1, $close_bracket_index - $open_bracket_index));

     $row_data_size = $num_array_elements * $array_element_size;
     #print "++ Calculating size of static int arr to be: $row_data_size\n";
   }
   elsif($type =~ /^string/) { # string
     my $buffer_size = $size;

     if($buffer_size) {
       $row_data_size = int($buffer_size);
     }
     else {
       my $string_value = $value;

       $row_data_size = length($string_value) + 1;
       #print "++ Calculating size of string to be: $row_data_size\n";
     }
   }
   
   return $row_data_size;
}

# used to validate input. makes sure the value
# string matches the value type 
sub TypeMatchesValue($$) {
   my ($type, $value) = @_;

   if($type =~ /^int$/) {
     return ($value =~ /^(\-)?\d+$/ || IsHexString($value));
   }
   elsif($type =~ /^int\[(\d+)?\]/) {
     return ($value =~ /^(\-)?\d+((,(\-)?\d+)+)?$/);
   }
   elsif($type =~ /^string$/) {
   }

   return $type;
}

sub SizeIsValid($) {
   my ($size) = @_;

   return ($size == 1) || ($size == 2) || ($size == 4) || ($size == 8);
}

sub IsValidItemType
{
  my ($variant) = @_;

  return (defined($variantValues->{$variant}));
}

# validates the fields in a row to make sure it follows the proper format
sub CheckRowInputFormat {
   my @fields = @_;

   my $row_type = @fields[GetFieldIndex("Type", $COLUMN_INDICES)];
   my $error_message = "";

   if($row_type =~ /^int$/ || $row_type =~ /^int\[(\d+)?\]/) {
     my $row_value = $fields[GetFieldIndex("Value", $COLUMN_INDICES->{MCFG_ITEM}->{SCALAR})];
     my $size = int($fields[GetFieldIndex("Size", $COLUMN_INDICES->{MCFG_ITEM}->{SCALAR})]);

     if(!SizeIsValid($size)) {
       $error_message = "Value size ($size) is not valid";
     }
     elsif(!TypeMatchesValue($row_type, $row_value)) {
       $error_message = "Value ($row_value) does not match specified type ($row_type)";
     }
   }
   elsif($row_type =~ /^efs$/) {
     my $efs_type = Trim($fields[GetFieldIndex("EFS Item Type", $COLUMN_INDICES->{EFS_FILE})]);
     my $efs_value = Trim($fields[GetFieldIndex("Value", $COLUMN_INDICES->{EFS_FILE})]);
     my $efs_size = Trim($fields[GetFieldIndex("EFS Item Size", $COLUMN_INDICES->{EFS_FILE})]);

     my $efs_source_file = $fields[GetFieldIndex("Filename", $COLUMN_INDICES->{EFS_FILE})];

     if($efs_type || $efs_value || $efs_size) {
       $error_message = "\n'efs' type items can only use the 'EFS Item Type', 'Value', or 'EFS Item Size' columns under special circumstances (e.g. multiSIM 'efs'). 'efs_item' type should be used if data for the target file is listed in the spreadsheet."
     }
     elsif(!$efs_source_file) {
       $error_message = "EFS input file path must be specified";
     }
   }
   elsif($row_type =~ /^prl$/) {
   }
   elsif($row_type =~ /^string$/) {
   }
   elsif($row_type ne '' && !IsValidItemType($row_type))
   {
     die "Error: NV item type not supported: $row_type\n" .
         "Found on line with following data: " . join(',', @fields) ."\n";
   }

   return $error_message;
}

# writes the header that contains the top comment and array name
sub WriteMcfgHeaderBin($$$$$) {
   my ($mcfg_output_file_name, $num_mcfg_items, $length_bytes, $carrier_index, $configuration_type) = @_;

   my $HW_CONFIG = 0;
   my $SW_CONFIG = 1;

   if (($configuration_type != $HW_CONFIG) && ($configuration_type != $SW_CONFIG))
   {
     die ("\nErr: Invalid configuration_type listed: %d\n", $configuration_type);
   }

   # Add the magic number to the MCFG variable.
   binwrite( pack('a*', 'MCFG'));

   # Add the format number to the MCFG variable.
   binwrite( pack('I', $mcfg_format), 2);

   # CFG Type 0 means hardware 1 means software (this is a constant for now. changed later)
   binwrite( pack('I', $configuration_type), 2);
   
   # Add the number MCFG items to the MCFG variable.
   binwrite( pack('I', $num_mcfg_items), 4);

   # the muxd index info -- need to find out where this comes from as well. set to 0 for now
   binwrite( pack('I', $carrier_index), 2);

   # two spare bytes
   binwrite( pack('I', 0x0), 2);

}

# writes the header that contains the top comment and array name
sub WriteMcfgHeader($$$$$$) {
   my ($mcfg_output_file_name, $num_mcfg_items, $length_bytes, $carrier_index, $configuration_type, $input_file_name) = @_;

   my $HW_CONFIG = 0;
   my $SW_CONFIG = 1;

   # Fill in comment section at start of MCFG source file.
   printf "/*========================================================================  \n";
   printf "\n             ";
   print GetFilenameFromPath($mcfg_output_file_name, "\\");
   printf "\n";
   printf "\n DESCRIPTION\n";
   printf "  This is an autogenerated file and contains the MCFG Item defintions used\n";
   printf "  in the MPSS image.\n";
   printf "\n Autogenerated on :";
   $tm = localtime;
   printf("%04d-%02d-%02d   \n",$tm->year+1900,($tm->mon)+1, $tm->mday, );
   printf("\n MCFG XML Source File : $input_file_name");
   printf "\n MCFG Format Ver : ";
   printf("%02d  \n",$mcfg_format);
   printf "\n Copyright (c) 2007-%04d by QUALCOMM, Incorporated.  All Rights Reserved. \n",$tm->year+1900; 
   printf "=========================================================================*/ \n";

   # Setup the MCFG variable name and size.
   if ($configuration_type == $HW_CONFIG) {
     printf "\n\nunsigned char mcfg_cust_data_hw[%d] __attribute__ ((section (\".mcfg_seg\"))) = {",   $length_bytes;
   }
   elsif ($configuration_type == $SW_CONFIG) {
     printf "\n\nunsigned char mcfg_cust_data_sw[%d] __attribute__ ((section (\".mcfg_seg\"))) = {",   $length_bytes;
   }
   else {
     die ("\nErr: Invalid configuration_type listed: %d\n", $configuration_type);
   }

   # Add the magic number to the MCFG variable.
   printf "\n /* MCFG Header */";
   printf "\n  'M'  ,'C'  ,'F'  ,'G'  " ;

   # Add the format number to the MCFG variable.
   printf ",0x%02X " ,($mcfg_format & 0xFF);
   printf ",0x%02X " ,(($mcfg_format>>8) & 0xFF);

   # CFG Type 0 means hardware 1 means software (this is a constant for now. changed later)
   printf ",0x%02X " ,($configuration_type & 0xFF);
   printf ",0x%02X \n " ,(($configuration_type>>8) & 0xFF);
   
   # Add the number MCFG items to the MCFG variable.
   printf ",0x%02X " ,( $num_mcfg_items     & 0xFF);
   printf ",0x%02X " ,(($num_mcfg_items>>8)  & 0xFF);
   printf ",0x%02X " ,(($num_mcfg_items>>16) & 0xFF);
   printf ",0x%02X " ,(($num_mcfg_items>>24) & 0xFF);

   # the muxd index info -- need to find out where this comes from as well. set to 0 for now
   printf ",0x%02X " , ($carrier_index & 0xFF);
   printf ",0x%02X " , (($carrier_index >> 8) & 0xFF);

   # two spare bytes
   my $spare_count = 2;
   for (my $spare = 0; $spare < $spare_count; $spare++ ) 
   {
     printf ",0x%02X ", 0;
   }
   printf "\n";

}

# writes the header that contains the top comment and array name
sub WriteMcfgDefaultHeader($$$$$$) {
   my ($mcfg_output_file_name, $num_mcfg_items, $length_bytes, $carrier_index, $configuration_type, $input_file_name) = @_;

   my $HW_CONFIG = 0;
   my $SW_CONFIG = 1;

   # Fill in comment section at start of MCFG source file.
   printf "/*========================================================================  \n";
   printf "\n             ";
   print GetFilenameFromPath($mcfg_output_file_name, "\\");
   printf "\n";
   printf "\n DESCRIPTION\n";
   printf "  This is an autogenerated file and contains the DEFAULT MCFG Item defintions used\n";
   printf "  in the MPSS image and weaved into the main image using DevCfg approach.\n";
   printf "\n Autogenerated on :";
   $tm = localtime;
   printf("%04d-%02d-%02d   \n",$tm->year+1900,($tm->mon)+1, $tm->mday, );
   printf("\n MCFG XML Source File : $input_file_name");
   printf "\n MCFG Format Ver : ";
   printf("%02d  \n",$mcfg_format);
   printf "\n Copyright (c) 2007-%04d by QUALCOMM, Incorporated.  All Rights Reserved. \n",$tm->year+1900; 
   printf "=========================================================================*/ \n";

   # Setup the MCFG variable name and size.

   if ($configuration_type == $HW_CONFIG) {
    printf "\nconst unsigned int  mcfg_cust_data_hw_size = %d ;\n", $length_bytes;
     printf "\nconst unsigned char mcfg_cust_data_hw[] = { ";
   }
   elsif ($configuration_type == $SW_CONFIG) {
     printf "\nconst unsigned int  mcfg_cust_data_sw_size = %d ;\n", $length_bytes;
     printf "\nconst unsigned char mcfg_cust_data_sw[] = { ";
   }
   else {
     die ("\nErr: Invalid configuration_type listed: %d\n", $configuration_type);
   }

   # Add the magic number to the MCFG variable.
   printf "\n /* MCFG Header */";
   printf "\n  'M'  ,'C'  ,'F'  ,'G'  " ;

   # Add the format number to the MCFG variable.
   printf ",0x%02X " ,($mcfg_format & 0xFF);
   printf ",0x%02X " ,(($mcfg_format>>8) & 0xFF);

   # CFG Type 0 means hardware 1 means software (this is a constant for now. changed later)
   printf ",0x%02X " ,($configuration_type & 0xFF);
   printf ",0x%02X \n " ,(($configuration_type>>8) & 0xFF);
   
   # Add the number MCFG items to the MCFG variable.
   printf ",0x%02X " ,($num_mcfg_items & 0xFF);
   printf ",0x%02X " ,(($num_mcfg_items>>8) & 0xFF);
   printf ",0x%02X " ,(($num_mcfg_items>>16) & 0xFF);
   printf ",0x%02X " ,(($num_mcfg_items>>24) & 0xFF);

   # the muxd index info -- need to find out where this comes from as well. set to 0 for now
   printf ",0x%02X " , ($carrier_index & 0xFF);
   printf ",0x%02X " , (($carrier_index >> 8) & 0xFF);

   # two spare bytes
   my $spare_count = 2;
   for (my $spare = 0; $spare < $spare_count; $spare++ ) 
   {
     printf ",0x%02X ", 0;
   }
   printf "\n";

}


# writes the header for mutliMBN
sub WriteMcfgMultiMBNHeader($$$$$) {
   my ($mcfg_output_file_name, $num_mcfg_items, $length_bytes, $carrier_index, $configuration_type) = @_;

   my $max_multimbn_size = 524288;

   # Fill in comment section at start of MCFG source file.
   printf "/*========================================================================  \n";
   printf "\n             ";
   print GetFilenameFromPath($mcfg_output_file_name, "\\");
   printf "\n";
   printf "\n DESCRIPTION\n";
   printf "  This is an autogenerated file and contains the MultiMBN MCFG Item defintions used\n";
   printf "  in the MPSS image\n";
   printf "\n Autogenerated on :";
   $tm = localtime;
   printf("%04d-%02d-%02d   \n",$tm->year+1900,($tm->mon)+1, $tm->mday, );
   printf "\n MCFG Format Ver : ";
   printf("%02d  \n",$mcfg_format);
   printf "\n Copyright (c) 2007-%04d by QUALCOMM, Incorporated.  All Rights Reserved. \n",$tm->year+1900; 
   printf "=========================================================================*/ \n";

   if (($length_bytes+4) >= $max_multimbn_size) {
     die ("\nErr: MultiMBN is too large to fit: %d\n", $length_bytes);
   }

   printf "\nunsigned char mcfg_multi_MBN[";
   printf "%d", ($max_multimbn_size);
   printf "] = { ";

   # Add the length to the MCFG variable.
   printf "\n /* MCFG Length */";
   printf "\n 0x%02X " ,($length_bytes & 0xFF);
   printf ",0x%02X " ,(($length_bytes>>8) & 0xFF);
   printf ",0x%02X " ,(($length_bytes>>16) & 0xFF);
   printf ",0x%02X \n" ,(($length_bytes>>24) & 0xFF);

   # Add the magic number to the MCFG variable.
   printf "\n /* MCFG Header */";
   printf "\n ,'M'  ,'C'  ,'F'  ,'G'  " ;

   # Add the format number to the MCFG variable.
   printf ",0x%02X " ,($mcfg_format & 0xFF);
   printf ",0x%02X " ,(($mcfg_format>>8) & 0xFF);

   # CFG Type 0 means hardware 1 means software (this is a constant for now. changed later)
   printf ",0x%02X " ,($configuration_type & 0xFF);
   printf ",0x%02X \n " ,(($configuration_type>>8) & 0xFF);
   
   # Add the number MCFG items to the MCFG variable.
   printf ",0x%02X " ,($num_mcfg_items & 0xFF);
   printf ",0x%02X " ,(($num_mcfg_items>>8) & 0xFF);
   printf ",0x%02X " ,(($num_mcfg_items>>16) & 0xFF);
   printf ",0x%02X " ,(($num_mcfg_items>>24) & 0xFF);

   # the muxd index info -- need to find out where this comes from as well. set to 0 for now
   printf ",0x%02X " , ($carrier_index & 0xFF);
   printf ",0x%02X " , (($carrier_index >> 8) & 0xFF);

   # two spare bytes
   my $spare_count = 2;
   for (my $spare = 0; $spare < $spare_count; $spare++ ) 
   {
     printf ",0x%02X ", 0;
   }
   printf "\n";

}

# writes the bytes of an integer of some size
# in little endian form 
sub WriteIntegerPayload($$$) {
   my ($value, $size, $horizontal_position) = @_;

   # For ints > MAX_INT_32 values have been stored in an anonymous array by ParseValue()
   if (ref($value) eq 'ARRAY') {

     # First array element equals '0x'. This pushes the last index out to size instead of size - 1
     for my $index (1..$#{ $value }) {
       printf " ,0x%02X", hex($value->[$index]);

       if($horizontal_position++ == 7) {
         printf "\n";
         $horizontal_position = 0;
       }
     }
   }
   else {
     for my $byte (1 .. $size) {
       printf " ,0x%02X", ByteNOf($value, $byte);

       if($horizontal_position++ == 7) {
         printf "\n";
         $horizontal_position = 0;
       }
     }
   }

   return $horizontal_position;
}

# writes the bytes of an integer of some size
# in little endian form 
sub WriteIntegerPayloadBin($$$) {
   my ($value, $size, $horizontal_position) = @_;

   # For ints > MAX_INT_32 values have been stored in an anonymous array by ParseValue()
   if (ref($value) eq 'ARRAY') {

     # First array element equals '0x'. This pushes the last index out to size instead of size - 1
     for my $index (1..$#{ $value }) {
       binwrite( pack('I', hex($value->[$index])), 1);
     }
   }
   else {
     binwrite( pack('I', $value), $size);
     if ($size > $INT_32)
     {
       #print STDOUT "size $size\n";
       my $null_pad = $size - $INT_32;
       binwrite( pack('C*', (0x00) x $null_pad));
     }
   }

   return $horizontal_position;
}

# writes the id, length, and value of an item
# this is common for most item types, so it was made into its own function
sub WriteTLV($$$) {
   my ($nv_item_number, $payload_length, $item_elements) = @_;

   printf " ,0x%02X ,0x%02X", ByteNOf($nv_item_number, 1), ByteNOf($nv_item_number, 2);
   printf " ,0x%02X ,0x%02X", ByteNOf($payload_length, 1), ByteNOf($payload_length, 2);

   my $horizontal_position = 4;

   for my $element (@$item_elements) {
     my $element_type = $element->{Type};
     my $element_value = $element->{Value};
     my $element_length = $element->{Size};

     if($element_type =~ /^int$/) {
       $horizontal_position = WriteIntegerPayload($element_value, $element_length, $horizontal_position);
     }
     elsif($element_type =~ /int\[\d+\]/ || $element_type =~ /int\[\]/) {
       my $array = $element_value;

       for my $el (@$array) {
         $horizontal_position = WriteIntegerPayload($el, $element_length, $horizontal_position);
       }
     }
     elsif($element_type =~ /string/) {
       my $horizontal_position = 4;
       my @int_array = unpack("C*", $element_value);

       if($element_length > 0) { # fixed buffer size string

         for(scalar(@int_array) .. $element_length - 1) {
            push(@int_array, 0);
         }

         for my $char_value (@int_array) {
            printf " ,0x%02X", $char_value;

            if($horizontal_position++ == 7) {
              printf "\n";
              $horizontal_position = 0;
            }
         }
       }
       else { # null terminated string
         push(@int_array, 0);

         for my $char_value (@int_array) {
            printf " ,0x%02X", $char_value;

            if($horizontal_position++ == 7) {
              printf "\n";
              $horizontal_position = 0;
            }
         }
       }
     }
   }
}

# writes the id, length, and value of an item
# this is common for most item types, so it was made into its own function
sub WriteTLVBin($$$) {
   my ($nv_item_number, $payload_length, $item_elements) = @_;

   binwrite( pack('I', $nv_item_number), 2);
   binwrite( pack('I', $payload_length), 2);

   my $horizontal_position = 4;

   for my $element (@$item_elements) {
     my $element_type = $element->{Type};
     my $element_value = $element->{Value};
     my $element_length = $element->{Size};

     if($element_type =~ /^int$/) {
       $horizontal_position = WriteIntegerPayloadBin($element_value, $element_length, $horizontal_position);
     }
     elsif($element_type =~ /int\[\d+\]/ || $element_type =~ /int\[\]/) {
       my $array = $element_value;

       for my $el (@$array) {
         $horizontal_position = WriteIntegerPayloadBin($el, $element_length, $horizontal_position);
       }
     }
     elsif($element_type =~ /string/) {
       my @int_array = unpack("C*", $element_value);

       if($element_length > 0)  # fixed buffer size string
       { 
         for(scalar(@int_array) .. $element_length - 1) {
            push(@int_array, 0);
         }

         for my $char_value (@int_array) {
           binwrite( pack('C', $char_value));
         }
       }
       else # null terminated string
       { 
         push(@int_array, 0);

         for my $char_value (@int_array) {
           binwrite( pack('C', $char_value));
         }
       }
     }
   }
}

sub WriteMcfgVersionBin($$) {
   my ($mcfg_output_file_name, $version_number) = @_;
   my $version_item_number = 4995;

   # Output the version information.
   WriteTLVBin($version_item_number, 4, [{Value => $version_number, Type => "int", Size => 4}]);
}

sub WriteMcfgVersion($$) {
   my ($mcfg_output_file_name, $version_number) = @_;
   my $version_item_number = 4995;

   # Output the version information.
   printf "\n /* NV_CUST_MCFG_VER */\n";

   WriteTLV($version_item_number, 4, [{Value => $version_number, Type => "int", Size => 4}]);

   printf "\n";
}

# a general function for determining the length
# of an arbitrary item (efs, prl, etc.)
sub CalculateItemPayloadLength {
   my ($elements, $attributes) = @_;
 
   my $load_length = 0;
   my $length_bytes = 0;

   foreach my $element (@$elements) {
     my $type = $element->{Type};

     if($type =~ /prl/) {
       my $prl_file_name = $element->{Filename};
       $length_bytes += CalculatePrlElementLength($BUILD_ROOT . $prl_file_name);
     }
     elsif($type =~ /^efs$/) {
       my $efs_full_path = $element->{Filepath};
       my $efs_file_name = GetEFSFilename($efs_full_path, $element->{Filename});
       my $value = $element->{EfsValue};
       my $size = $element->{EfsSize};

       if ( ! ItemIsDeleteOrSWOnly($attributes)) {
          $load_length = CalculateEfsElementLength($efs_full_path, $BUILD_ROOT . $efs_file_name, $value, $size);
       }
       else {
          $load_length = 0;
       }
       $length_bytes += $load_length;
     }
     elsif($type =~ /^efs_item$/) {
        # efs_item size is calculated by having its element list passed to CalculateItemPayloadLength()
     }
     else { # Scalar...usually an NV item
       my $size = $element->{Size};
       my $value = $element->{Value};

       $length_bytes += CalculateScalarElementLength($type, $size, $value);
     }
   }

   return $length_bytes;
}

sub WriteMcfgItemComment($$$$) {
   my ($item_id, $item_type, $attributes, $payload_length) = @_;

   if($item_type eq "efs") {
   }
   elsif($item_type eq "prl") {
   }
   elsif($item_type eq "scalar") {
     if (!ItemIsSoftwareOnly($attributes)) 
     {
       printf "\n /* MCFG Item(carrier specific): %d , Item size: %d */\n", $item_id, $payload_length;
     }
     else
     {
       printf "\n /* MCFG Item(Muxd Sw-only): %d , Item size: %d */\n", $item_id, $payload_length;
     }
   }
}

# write comment, header and payload of item
sub WriteMcfgItemBin($) {
   my ($mcfg_item) = @_;

   my $attributes     = $mcfg_item->{Attributes};
   my $id             = $mcfg_item->{ID};
   my $elements       = $mcfg_item->{Elements};
   my $payload_length = $mcfg_item->{Payload};
   my $item_length    = $mcfg_item->{DataSize};
   my $item_type      = $mcfg_item->{ItemType};
   my $item_variant   = $mcfg_item->{Variant};
                     
   if($item_type eq "efs") 
   { 
     #efs item(s) stored as file or directory of efs files. Metadata (i.e. subsMask)
     #can also exist in the beginning of the item(s) list
     my $efs_element  = IsMetaData($mcfg_item->{Elements}->[0]) ? $mcfg_item->{Elements}->[1] : $mcfg_item->{Elements}->[0];
     my $efs_elements = $mcfg_item->{Elements};
     my $efs_value    = $mcfg_item->{EfsValue};
     my $efs_type     = $mcfg_item->{EfsType};
     my $efs_size     = $mcfg_item->{EfsSize};
     my $efs_filepath = ItemIsDeleteOrSWOnly( $attributes ) ? $mcfg_item->{Filepath} : $efs_element->{Filepath};
     my $efs_filename = ItemIsDeleteOrSWOnly( $attributes ) ? $mcfg_item->{Filename} : $efs_element->{Filename};
                  
     if($efs_value) { 
       # inserting the size as the type assumes that only numeric values will be used. need to check if this is the case
       #create_efs_file(0, $efs_size, $efs_value, $attributes, 
       #           GetEFSFilename($efs_filepath, $efs_filename), $efs_filepath, $item_variant);
     }
     else {
        if (ItemIsIndexed($attributes)) 
        {
           # Not implemented yet
           Log ($LOG_FATAL_ERROR, "Error on item", $item);
           die "EFS File type: Indexed attribute has not been implemented yet in generation script\n";
        }

        #print STDOUT "WriteMcfgItemBin: \n\t" . Dumper($mcfg_item);
        parse_efs_file_bin($BUILD_ROOT . GetEFSFilename($efs_filepath, $efs_filename), 
                       $attributes, $efs_filepath, $payload_length, $item_length, $item_variant, $efs_elements);
     }                                    
   }
   elsif($item_type eq "efs_item") 
   { #efs item: this type is listed and modified directly in the excel sheet
     # [NV ITEM LENGTH -- 4] [0x02] [ATTRIBUTES] [0x00] [0x00]
     # [0x0001] [FILE PATH LENGTH -- 2] [FILE PATH]
     WriteEFSHeaderBin($payload_length, $item_length, $attributes, $mcfg_item->{Filepath}, $item_variant);
     WriteTLVBin(0x0002, $payload_length, $elements);
   }
   elsif($item_type eq "prl") 
   {
     my $prl_filename    = $mcfg_item->{Elements}->[0]->{Filename};
     my $prl_item_attrib = $mcfg_item->{Attributes};
     my $prl_item_id     = $mcfg_item->{ID};
     my $carrier_id      = $mcfg_item->{CarrierID};

     parse_nv_prl_bin($BUILD_ROOT . $prl_filename, $prl_item_attrib, $prl_item_id, $carrier_id);
   }
   elsif($item_type eq "scalar") 
   {
     WriteScalarHeaderBin($item_length, $attributes, $item_variant);
     WriteTLVBin($id, $payload_length, $elements);
   }
}

sub WriteMcfgItem($) {
   my ($mcfg_item) = @_;

   my $attributes     = $mcfg_item->{Attributes};
   my $id             = $mcfg_item->{ID};
   my $elements       = $mcfg_item->{Elements};
   my $payload_length = $mcfg_item->{Payload};
   my $item_length    = $mcfg_item->{DataSize};
   my $item_type      = $mcfg_item->{ItemType};
   my $item_variant   = $mcfg_item->{Variant};
                     
   WriteMcfgItemComment($id, $item_type, $attributes, $payload_length);

   if($item_type eq "efs") 
   { 
     #efs item(s) stored as file or directory of efs files. 
     #Metadata (i.e. subsMask) can also exist in the beginning of the item(s) list
     my $efs_element  = IsMetaData($mcfg_item->{Elements}->[0]) ? $mcfg_item->{Elements}->[1] : $mcfg_item->{Elements}->[0];
     my $efs_elements = $mcfg_item->{Elements};
     my $efs_value    = $mcfg_item->{EfsValue};
     my $efs_type     = $mcfg_item->{EfsType};
     my $efs_size     = $mcfg_item->{EfsSize};
     my $efs_filepath = ItemIsDeleteOrSWOnly( $attributes ) ? $mcfg_item->{Filepath} : $efs_element->{Filepath};
     my $efs_filename = ItemIsDeleteOrSWOnly( $attributes ) ? $mcfg_item->{Filename} : $efs_element->{Filename};
                  
     if($efs_value) { 
       # inserting the size as the type assumes that only numeric values will be used. need to check if this is the case
       create_efs_file(0, $efs_size, $efs_value, $attributes, 
                   GetEFSFilename($efs_filepath, $efs_filename), $efs_filepath, $item_variant);
     }
     else {

        if (ItemIsIndexed($attributes)) 
        {
           # Not implemented yet
           Log ($LOG_FATAL_ERROR, "Error on item", $item);
           die "EFS File type: Indexed attribute has not been implemented yet in generation script\n";
        }

        parse_efs_file($BUILD_ROOT . GetEFSFilename($efs_filepath, $efs_filename), 
                       $attributes, $efs_filepath, $payload_length, $item_length, $item_variant, $efs_elements);
     }                                    
   }
   elsif($item_type eq "efs_item") 
   { #efs item: this type is listed and modified directly in the excel sheet
     # [NV ITEM LENGTH -- 4] [0x02] [ATTRIBUTES] [0x00] [0x00]
     # [0x0001] [FILE PATH LENGTH -- 2] [FILE PATH]
     WriteEFSHeader($payload_length, $item_length, $attributes, $mcfg_item->{Filepath}, $item_variant);
     WriteTLV(0x0002, $payload_length, $elements);
   }
   elsif($item_type eq "prl") 
   {
     my $prl_filename    = $mcfg_item->{Elements}->[0]->{Filename};
     my $prl_item_attrib = $mcfg_item->{Attributes};
     my $prl_item_id     = $mcfg_item->{ID};
     my $carrier_id      = $mcfg_item->{CarrierID};

     parse_nv_prl($BUILD_ROOT . $prl_filename, $prl_item_attrib, $prl_item_id, $carrier_id);
   }
   elsif($item_type eq "scalar") 
   {
     WriteScalarHeader($item_length, $attributes, $item_variant);
     WriteTLV($id, $payload_length, $elements);
   }

   printf "\n\n";
}

sub IsHexString($) {
   my ($string) = @_;

   return $string =~ /^0x[0-9a-fA-F]+$/;
}

# Purpose: Returns the field that can be used to determine whether an NV or EFS item is unique
sub GetItemIdentifier($) {
   my ($mcfg_item) = @_;
   
     if ( $mcfg_item->{ItemType} =~ /scalar/ ) {
       return $mcfg_item->{ID};
     }
     elsif ($mcfg_item->{ItemType} eq 'efs_dir') {
       return "efs_dir";
     }
     elsif ($mcfg_item->{ItemType} eq 'efs') {
       return $mcfg_item->{Filepath};
     }
     elsif ($mcfg_item->{ItemType} eq 'group') {
       return $mcfg_item->{Filename};
     }
     elsif ($mcfg_item->{ItemType} eq 'efs_item') {
       return $mcfg_item->{Filepath};
     }
     elsif ($mcfg_item->{ItemType} =~ /prl/) {
       return $mcfg_item->{Elements}[0]->{Filename};
     }
     elsif ($mcfg_item->{ItemType} =~ /trl/) {
       return "trl"; #There's no ID to return for a trailer record
     }
     else {
       printf "Error: Invalid item type: '$mcfg_item->{ItemType}'\n"
     }
}

{
   # Outer scope used so $uniq_mcfg_items can be a "static" function variable 
   my $uniq_mcfg_items = {};

# Purpose: Tests whether the current NV or EFS element has already been processed 
   sub IsItemUnique($) {
     my ($item) = @_;
     my $item_id = GetItemIdentifier($item);
     my $item_attrib = $item->{Attributes};
     my $item_variant = $item->{Variant};

     if ($item_id ne '') {
       if ((exists $uniq_mcfg_items->{"$item_id"}) && 
            !DuplicateItemAllowed($item_attrib, $item_variant))
       {
         return 0; # Non-repeatable item ID already exists. Test Fails.
       }
       else {
         $uniq_mcfg_items->{"$item_id"} = 1;
         return 1;
       }
     }
   }

   #Purpose: Clear the hash tracking unique item entries
   sub ResetUniqueItemList() {
    %$uniq_mcfg_items = ();
   }
}

# Indicates if argument is a reference to an array of integers
sub IsIntArrayRef {
   my $element_value = shift;
   my $bool = 0;

   if (( ref( $element->{Value} ) eq "ARRAY" )) {
     my $first_arr_element = ${ $element->{Value} }[0];
     # XXX: change the condition to match an integer, as opposed to "not a hex"
     if ( $first_arr_element ne '0x' ) {
       $bool = 1;
     }
     else {
       # Array reference is to list of hex bytes. since the array 
       # represents one number case element_size remains 
       # the same value as size listed in the excel spreadsheet
     }
   }
   else {
     # Parsevalue returned either a regular integer or a string
   }

   return bool;
}

# Name:  IsNextItem
#
# Params: $item_id   -- Item identifier, e.g. NV # or target EFS filepath
#         $attrib    -- Attributes, e.g. 0x09,0x2B
# 
# Return: bool
# 
# Purpose: 
#    Identifies if next NV/EFS item listed has been reached. Call this function AFTER moving 
#    beyond the current item's attributes row
sub IsNextItem {
   my (@properties) = @_;
   my $next_item = 0;

   foreach my $property (@properties) {
      if (defined $property) {
         $property = Trim($property);
      }
      if ($property ne '') {
         $next_item = 1;
      }
   }

   return ($next_item);
}

# Creates the container for a new NV item
sub newElement {
   my ($row_type, $size, $value) = @_;

   my $element = { 
              Type  => $row_type,
              Size  => $size,
              Value => $value,
             };

   return $element;
}
# make this generic. it will know the type of item being parsed so it can 
# call newElement with teh correct parameters
sub CreateNewElement {
   my ($item_type, $element_type, $row_ref) = @_;

   my $element;
   my $element_size;
   my $element_value;

   if ( $item_type =~ /scalar/ ) 
   {
     $element_size  = $row_ref->[GetFieldIndex("Size" , $COLUMN_INDICES->{MCFG_ITEM}->{SCALAR})];
     $element_value = ParseValue($row_ref->[GetFieldIndex("Value", $COLUMN_INDICES->{MCFG_ITEM}->{SCALAR})], $element_type, $element_size);
      
   }
   elsif ($item_type =~ /efs/) 
   {
     $element_size  = $row_ref->[ GetFieldIndex( "EFS Item Size" , $COLUMN_INDICES->{EFS_FILE} ) ];
     $element_value = ParseValue($row_ref->[ GetFieldIndex("Value", $COLUMN_INDICES->{EFS_FILE} ) ], $element_type, $element_size);
   }
#  elsif ($item_type =~ /prl/) {
#  }
#  elsif ($item_type =~ /trl/) {
#  }
#  else {
#  }

   $element = newElement( $element_type, $element_size, $element_value ); 
                                         
   return $element;
}


# Determines which variant type value should be used, given the row type. 
# This value will be included in the header prefix, and used when processing 
# items in MCFG
sub  GetVariantType($) {
   my ($row) = @_;
   my $row_type = $row->[GetFieldIndex("Type", $COLUMN_INDICES)];
   my $variant = 0 ;

   if (defined( $variantValues->{$row_type} )) {
      $variant = $variantValues->{$row_type};
   }
   else {
      my $efs_filepath = $row->[GetFieldIndex("Filepath", $COLUMN_INDICES->{EFS_FILE})];
      print "Err: GetVariantType: Variant not found for '$efs_filepath'\n";
   }

   return $variant;
}

sub ExtractNVItem($$$) {
   my ($rows_ref, $row, $attributes) = @_;
   my $ID = $row->[GetFieldIndex("ID", $COLUMN_INDICES->{MCFG_ITEM}->{SCALAR})];
   my $item = { 
            ID => $ID,
            Attributes => $attributes,
            ItemType   => "scalar",
            Payload    => 0, # Contains total size of all data elements within specific NV item
            DataSize   => 0, # Contains Payload + Header
            Elements   => [], 
            Variant    => 1,
           };
  
   # erase the attributes field
   $row->[ GetFieldIndex("Attributes", $COLUMN_INDICES) ] = "";
   $attributes = "";

   my $element; 
   my $element_type = $row->[GetFieldIndex("Type", $COLUMN_INDICES)];

   if ($ID > 7232)
   {
     die ("Err: Invalid NV# for NV listing '$ID'. NVs beyond 7232 should be listed as efs or efs_item types\n");
   }
   elsif ($ID < 0 )
   {
     die ("Err: Invalid NV# '$ID' for NV listing. NV values are always positive\n");
   }

   # Getting to this point means the attributes and ID fields are populated
   # If element/item type is populated as well then we have the entire item listed on one row
   if ( !IsBlank($element_type) )
   {
     CheckElementFormat($row, $item);

     # If single line item is multiSIM then include defualt subs_mask
     if (ItemIsSubscriber($item->{Attributes})) 
     {
       $element = newElement( $DFLT_SUBS_TYPE, $DFLT_SUBS_SIZE, $DFLT_ACTIVE_SUBS );
       push @{ $item->{Elements} }, $element;
     }

     $element = CreateNewElement( $item->{ItemType}, $element_type, $row );
     push( @{$item->{Elements}}, $element );
   }
   # element_type is blank, therefore item is listed on multiple lines
   else 
   { 
     $row = GetNextRow( $rows_ref );
     $element_type = $row->[GetFieldIndex("Type", $COLUMN_INDICES)];
     $ID  = $row->[GetFieldIndex("ID", $COLUMN_INDICES->{MCFG_ITEM}->{SCALAR})];

     # Filepath is where subs_mask will reside; special case of !IsNextItem
     if (ItemIsSubscriber($item->{Attributes})) 
     {
       if (!IsSubsMaskPresent($ID))
       {
         $element = newElement( $DFLT_SUBS_TYPE, $DFLT_SUBS_SIZE, $DFLT_ACTIVE_SUBS );
         push @{ $item->{Elements} }, $element;
       }
       else
       {
         # clear 'subs_mask' from $ID so false positive for a new NV/EFS item isn't found 
         $ID = '';
       }
     }

     while($row)
     {

       # If next item has been reached then we are done processing current item
       last if (IsNextItem( $ID, $attributes ));

       CheckElementFormat($row, $item);

       if($element_type =~ /^int$/ || $element_type =~ /int\[\d+\]/ || $element_type =~ /int\[\]/ || $element_type =~ /^string$/) 
       {
          my $element = CreateNewElement( $item->{ItemType}, $element_type, $row );
          push @{ $item->{Elements} }, $element;
       }
       $row = GetNextRow( $rows_ref );
       $ID  = $row->[ GetFieldIndex("ID", $COLUMN_INDICES->{MCFG_ITEM}->{SCALAR}) ];
       $element_type = $row->[GetFieldIndex("Type", $COLUMN_INDICES)];
       $attributes   = $row->[GetFieldIndex("Attributes", $COLUMN_INDICES)];
     }
   }

   if(@$row) {
     unshift(@$rows_ref, $row);
   }
 
   $item->{Payload}  = CalculateItemPayloadLength($item->{Elements}, $item->{Attributes});
   $item->{DataSize} = $NV_HEADER_LENGTH + $item->{Payload};
 
   Log($NV_EXTRACTION_TRACE_ENABLE | $ALL_EXTRACTION_TRACE, "\nNV item extracted: '$item->{ID}'", $item );
   Log($DATASIZE_TRACE_ENABLE, "Size: '$item->{DataSize}'");

   return $item;
}

sub newEFSFile {
   my ($type, $filepath, $filename) = @_;

   return {  
         Type     => "$type",
         Filepath => $filepath,
         Filename => $filename, 
         Payload    => 0, # of just the file item. will not include header size
         DataSize   => 0, # will include both the payload and the header size
        };
}

# 
# Name:  IsEFSItemData
#
# Params: $rows_ref -- arr of rows extracted from spreadsheet
# 
# Return: bool
# 
# Purpose: 
#    Identifies if next row in the list contains data listed 
#    in the style of an efs_item type
sub IsEFSItemData
{
  my ($rows_ref) = @_;
  my @fields = @{ $rows_ref->[0] }; # just copy. leave $rows_ref unmodified

  # mark item type as "efs" to make use of CheckRowInputFormat
  # returns error if it finds data listed for "efs_item" type
  $fields[GetFieldIndex("Type", $COLUMN_INDICES)] = 'efs'; 

  my $error = CheckRowInputFormat(@fields);
  if ($error ne '' &&
      $error !~ /EFS input file/)
  {
    print STDOUT $error;
    return 1;
  }

  return 0;
}

sub ExtractEFSFile {
   my ( $rows_ref, $next_row, $attributes, 
        # When ExtractEFSFile is called during directory extraction, these extra arguments are passed to the function.
        $extracting_from_dir, $efs_filepath, $efs_filename, $variant, $metaData,
      ) = @_;

   Log($ARG_TRACE_ENABLE, "\nExtractEFSFile: arguments passed", \@_  );

   # If ExtractEFSFile is not called during a directory extraction filepaths will need to be set
   if ( ! $extracting_from_dir ) 
   { 
     $efs_filepath = $next_row->[GetFieldIndex("Filepath", $COLUMN_INDICES->{EFS_FILE})];
     $efs_filename = $next_row->[GetFieldIndex("Filename", $COLUMN_INDICES->{EFS_FILE})];
   }
   $efs_filepath = Trim($efs_filepath);
   $efs_filename = Trim($efs_filename);

   my $item = { 
            Attributes => $attributes,
            ItemType   => "efs",
            Filepath   => $efs_filepath, # This datum is stored in both the item and element subhash for an EFS file
            Filename   => $efs_filename, # EFS filename attribute is used for testing by mcfg_verify.pm
            Elements   => [],
            Payload    => 0,
            DataSize   => 0,
            Variant    => $variant,      # Variant is only passed as function parameter during directory extraction
           };
          
   if ( ! $extracting_from_dir ) 
   { 
      ExtractItemMetaData($rows_ref, $item);
      #print STDOUT "ExtractEFSFile (not from dir): \n\t" . Dumper($item);
      $item->{Variant} = GetVariantType($next_row);
   }
   else # $metadata collected during directory extraction
   {
     if (defined($metaData))
     {
       push( @{$item->{Elements}}, $metaData );
     }
   }

   if (IsEFSItemData($rows_ref))
   {
     die "Error: extra row of data listed for 'efs' style item type: $efs_filepath\n";
   }

   if ( ! $extracting_from_dir ) 
   { 
     Log($FILEPATHS_TRACE_ENABLE, "\nEFS extraction"                    );
     Log($FILEPATHS_TRACE_ENABLE, "\t efs path target: '$efs_filepath'" );
     Log($FILEPATHS_TRACE_ENABLE, "\t efs local path:  '$efs_filename'" );
   }
   else 
   {
     Log($FILEPATHS_TRACE_ENABLE, "\nDir extraction: within EFS extraction"  );
     Log($FILEPATHS_TRACE_ENABLE, "\t efs path target: '$efs_filepath'"  );
     Log($FILEPATHS_TRACE_ENABLE, "\t efs local path:  '$efs_filename'"  );
   }

   my $element = newEFSFile( "efs", $efs_filepath, $efs_filename );
   push( @{$item->{Elements}}, $element );
   $item->{Payload}  = CalculateItemPayloadLength($item->{Elements}, $item->{Attributes});
   $item->{DataSize} = $EFS_HEADER_LENGTH + length($efs_filepath) + $item->{Payload};

   Log($EFS_EXTRACTION_TRACE_ENABLE | $ALL_EXTRACTION_TRACE, "\nEFS file extracted: $item->{Filename}", $item  );

   return $item;
}

sub CheckForBlacklisting
{
  my ($item_id) = @_;
  my $err_msg = '';
  my @matches = ();

  if ($item_id =~ /trl|efs_dir|prl|group/)
  {
    return;
  }

  if ($item_id =~ m#^/.*#) # item_id is an efs path in root folder (/)
  {
    foreach my $blacklisted_storage_item (@blacklisted_storage_items)
    {
      my $cmp_w_file = 1;
      my $bl_item = $blacklisted_storage_item;

      if ($bl_item =~ m#/...$#)
      {
        $cmp_w_file = 0;
        $bl_item =~ s#/...$##;
      }

      if ($cmp_w_file)
      {
        if ($bl_item eq $item_id)
        {
          $err_msg = "Storage item '$item_id' is blacklisted from being included in MCFG MBNs. Please remove from configuration";
        }
      }
      else # comparing generic path 
      {
        if (index($item_id, $bl_item) != -1)
        {
          $err_msg = "NV item '$item_id' is under blacklisted directory '$bl_item'. Please remove from configuration";
        }
      }
    }
  }
  elsif ($item_id =~ /^$UINT$/)
  {
    foreach my $blacklisted_storage_item (@blacklisted_storage_items)
    {
      my $is_range = 1;
      my $bl_item = $blacklisted_storage_item;
      if ($bl_item =~ /^(\d|-)+$/)
      {
        my ($nv_start, $nv_stop) = split('-', $bl_item);
        # range could start from 0 so use defined()
        if (defined($nv_start) && defined($nv_stop)) # bl_item is an NV range 
        {
          if ($item_id >= $nv_start && $item_id <= $nv_stop)
          {
            $err_msg = "Storage item '$item_id' falls within blacklist range '$bl_item'. Values within this range are restricted from being included in MCFG MBNs. Please remove this entry from configuration";
          }
        }
        else # entry is a specific NV
        {
          if ($item_id eq $bl_item)
          {
            $err_msg = "Storage item '$item_id' is blacklisted from being included in MCFG MBNs. Please remove from configuration";
          }
        }
      }
    }
  }
  else
  {
    $err_msg = "Unknown storage item id '$item_id'. Please verify destination path or nv# is correct";
  }

  die("\n== >> Error: $err_msg \n\n") if ($err_msg);
}

sub ExtractGroupFileListing {
   my ( $rows_ref, $next_row, $attributes, 
        # When ExtractEFSFile is called during directory extraction, these extra arguments are passed to the function.
        $extracting_from_dir, $efs_filepath, $efs_filename, $variant, $metaData,
      ) = @_;

   Log($ARG_TRACE_ENABLE, "\nExtractGroupFile: arguments passed", \@_  );


   #$efs_filepath = $next_row->[GetFieldIndex("Filepath", $COLUMN_INDICES->{EFS_FILE})];
   $group_version = $next_row->[GetFieldIndex("Version", $COLUMN_INDICES->{GROUP_FILE})]; 
   $group_filename = $next_row->[GetFieldIndex("Filename", $COLUMN_INDICES->{GROUP_FILE})];

   #$efs_filepath = Trim($efs_filepath);
   $group_version = Trim($group_version);
   $group_filename = Trim($group_filename);

   my $item = { 
            Attributes => $attributes,
            ItemType   => "group",
            #Filepath   => $efs_filepath, 
            Version    => $group_version,
            Filename   => $group_filename,
            Elements   => [],
            Payload    => 0,
            DataSize   => 0,
            Variant    => GetVariantType($next_row),  
           };

   ExtractItemMetaData($rows_ref, $item);  # This subroutine call may not be needed, need to check
   #$rows_ref and $item the same before/after call?

   # This check may not be needed, need to check
   if (IsEFSItemData($rows_ref))  # you would rarely try to list a group like an efs item
   {
     die "Error: extra row of data listed for 'efs' style item type: $group_filename\n";
   }

   Log($FILEPATHS_TRACE_ENABLE, "\nGroup extraction");
   Log($FILEPATHS_TRACE_ENABLE, "\t group local path:  '$group_filename'" );

   my $element = newEFSFile( "group", $efs_filepath, $group_filename );
   push( @{$item->{Elements}}, $element );
   $item->{Payload}  = CalculateItemPayloadLength($item->{Elements}, $item->{Attributes});
   $item->{DataSize} = $EFS_HEADER_LENGTH + length($efs_filepath) + $item->{Payload};

   Log($EFS_EXTRACTION_TRACE_ENABLE | $ALL_EXTRACTION_TRACE, "\nGroup file extracted: $item->{Filename}", $item  );
   return $item;

}


sub ExtractGroupFileContents {
   my ( $rows_ref, $next_row, $attributes, 
        # When ExtractEFSFile is called during directory extraction, these extra arguments are passed to the function.
        $extracting_from_dir, $efs_filepath, $efs_filename, $variant, $metaData,
      ) = @_;

   #$efs_filepath = $next_row->[GetFieldIndex("Filepath", $COLUMN_INDICES->{EFS_FILE})];
   $group_version = $next_row->[GetFieldIndex("Version", $COLUMN_INDICES->{GROUP_FILE})]; 
   $group_filename = $next_row->[GetFieldIndex("Filename", $COLUMN_INDICES->{GROUP_FILE})];

   #$efs_filepath = Trim($efs_filepath);
   $group_version = Trim($group_version);
   $group_filename = Trim($group_filename);

   chdir $BUILD_ROOT;  # why is this necessary?

   # read xml group file
   open my $fh, '<', $group_filename or die "Can't open file '$group_filename'!";
   my $xml_group_content = do { local $/; <$fh> };
   #print "XML: " . $xml_group_content . "\n";

   # parse xml
   my $parser = new XML::Parser(Style => 'Tree');
   my $xmlParseTree = $parser->parse($xml_group_content);
   $xmlParseTree = XmlStripBlanks($xmlParseTree);
   my $top = shift(@{$xmlParseTree->[1]}); # remove top level attributes hash
   $xmlParseTree = $xmlParseTree->[1];
   my $group_sheet_data = $xmlParseTree->[1]->[0];
   my $actual_group_version = $$group_sheet_data{'version'};

   my @mcfg_items;  # array of hash references to mcfg_items
   # Need to convert xmlParseTree to array of mcfg item hash references

   my $mcfg_sheet_data = ExtractCarrierData($xmlParseTree);
   my @parsed_mcfg_items  = ExtractNextMcfgItemXML($xmlParseTree);  # array of mcfg_item hash references (always one or more)
   while (!isEmptyArray(\@parsed_mcfg_items)) {
      foreach $mcfg_item (@parsed_mcfg_items) {
         my %mcfg_item_hash = %$mcfg_item;
         my $ref = ref($mcfg_item);
         my $payload = $mcfg_item_hash{'Payload'};
         if ($payload == 0) {
            next;
         }
         push(@mcfg_items, $mcfg_item);  
      }
      @parsed_mcfg_items  = ExtractNextMcfgItemXML($xmlParseTree);
   }

   return @mcfg_items;
}


# Instead of reading through the directory, this just reads the data
# given in the spreadsheet into a hash.
sub ExtractEFSDirInfo {
   my ( $rows_ref, $next_row, $attributes) = @_;

   Log($ARG_TRACE_ENABLE, "\nExtractEFSDirInfo: arguments passed", \@_  );

   $efs_filepath = $next_row->[GetFieldIndex("Filepath", $COLUMN_INDICES->{EFS_FILE})];
   $efs_filename = $next_row->[GetFieldIndex("Filename", $COLUMN_INDICES->{EFS_FILE})];

   my $item = { 
            Attributes => $attributes,
            ItemType   => "efs_dir",
            Filepath   => $efs_filepath,
            Filename   => $efs_filename,
            Elements   => [],
            Payload    => 0,
            DataSize   => 0,
            Variant    => 0,
           };

   $item->{Variant} = GetVariantType($next_row);
         
   Log($FILEPATHS_TRACE_ENABLE, "\nEFS extraction"                    );
   Log($FILEPATHS_TRACE_ENABLE, "\t efs path target: '$efs_filepath'" );
   Log($FILEPATHS_TRACE_ENABLE, "\t efs local path:  '$efs_filename'" );

   my $element = newEFSFile( "efs_dir", $efs_filepath, $efs_filename );
   # element passed as an anon array ref since CalculateItemPayloadLength expects an arr_ref
   push( @{$item->{Elements}}, $element );

   return $item;
}

# Determines whether a file item is a directory based on the string format
# If there is a trailing '/' or '\' then consider then flag file as a dir
sub IsDirectory {
   my $file = shift;

   return $file =~ m#(/|\\)$#;
}
      
# Function: BuildPath
# Params:   Filepath portions to be concatenated with a directory separator in between. 
#           Components will need to be listed in the array in the order they are to be appended
# Return:   A full path. Can be either absolute or relative
sub BuildPath {
   my @path_component = @_;
   my $path = '';

   foreach (@path_component) {
      # Append each path_component to the path unless $_ is blank. If this weren't done then 
      # '\\' would be incorrectly appended whenever a blank $_ is encountered
      $path = File::Spec->catfile( "$path", "$_") unless /^$/; 
   }
   $path =~ s#^\\+##; # Strip path of leading '\' characters. 

   Log($PATH_CONVERSION_TRACE, "\nBuilding path with following components");
      foreach (@path_component) {
      Log($PATH_CONVERSION_TRACE, "\t Concatenated Path: '$_'" );
   }
   Log($PATH_CONVERSION_TRACE, "\t Full Path: '$path'" );

   return $path;
}

# Format a path to adhere to the rules of Windows paths
sub CanonicalizePathWindows {
   my $path = shift;
   my $orig_path = $path;

   $path = File::Spec->canonpath( $path );

   Log($PATH_CONVERSION_TRACE, "\nCanonicalizing path to Windows");
   Log($PATH_CONVERSION_TRACE, "\t Path before: '$orig_path'" );
   Log($PATH_CONVERSION_TRACE, "\t Path after: '$path'" );
   return $path;
}

# Format a path to adhere to the rules of Unix paths
sub CanonicalizePathUnix {
   my $path = shift;
   my $orig_path = $path;

   $path =~ s#\s+#_#g; #replace spaces in target directory with underscores
   $path =~ s#\\+#/#g; #Convert windows style directory delimiters to Unix style
   $path = '/' . $path unless ( $path =~ m#^/# ); # Prepend the root symbol to the target path unless one is already present

   Log($PATH_CONVERSION_TRACE, "\nCanonicalizing path to Unix (for target EFS)");
   Log($PATH_CONVERSION_TRACE, "\t Path before: '$orig_path'" );
   Log($PATH_CONVERSION_TRACE, "\t Path after: '$path'" );

   return $path;
}

# Purpose: Gather a listing of the contents within a folder; equivalent to typing 'ls' at the command prompt
# Params:  absolute_directory_path, i.e. relative to nothing but the root directory
# Return: Returns a pointer to a list of all the files found in the directory tree
sub ListDirContents {
   my ($dir_abs) = @_;
   my @dir_contents;

   Log($DIR_TRAVERSAL_TRACE_ENABLE, "\nOpening directory '$dir_abs'");
   Log($DIR_TRAVERSAL_TRACE_ENABLE, "Directory contents list: should be empty", \@dir_contents);
    
   opendir( my $EFS_DIR, "$dir_abs") or die "Could not open dir '$dir_abs': $!";
   # Each perl function operates on the one below; read stacked operations from bottom up, i.e. readdir, grep, grep, map
   @dir_contents  = map  { -d "$dir_abs/$_" ? "$_\\" : "$_" } # Add a trailing '\' to directories
                    grep { !/^\.{1,2}$/ } # Filter out '.' and '..' directories
                    grep { -f "$dir_abs/$_" or -d "$dir_abs/$_" } # Filter out everything but files and directories
                    readdir($EFS_DIR);
   closedir( $EFS_DIR ) or die "Could not close directory '$dir_abs': $!";
 
   Log($DIR_TRAVERSAL_TRACE_ENABLE, "\nContents read from directory '$dir_abs'", \@dir_contents );
   
 
   # Return a reference in case the list is large
   return \@dir_contents;
}

# Purpose: Extract the contents of a directory to two lists: directories in one list, files in the other
# Params:
# Return: Returns a pointer to a list of all the files found within this directory (not including subdirectories)
sub ExtractEFSDir {
   my ( $dir_list_ref, $file_list_ref, $src_dir_root, 
     $target_dir, $attributes, $variant, $metaData ) = @_;

   my $subdir = shift @{ $dir_list_ref }; # subdir = subdirectory path relative to the source root
   my $dirlevel;

   if ("$subdir" ne "$src_dir_root") {
      $dirlevel = 1; # We are not at the root source directory
   }
   else {
      $dirlevel = 0; # Starting point of directory search, subdir == source root directory
   }

   Log($DIR_TRAVERSAL_TRACE_ENABLE, "\nDirectory level: $dirlevel for item : '$subdir'" );
    
   my $dir_abs = BuildPath( $BUILD_ROOT, $src_dir_root ); # Absolute path: /buildroot/top_level_source_dir 

   if ($dirlevel != 0) {
      $dir_abs = BuildPath( $dir_abs, $subdir ); # Absolute path: /buildroot/top_level_source_dir/subdir_path 
   }

   # Open directory and store a list of its contents 
   my $dir_contents = ListDirContents($dir_abs);

   foreach $dir_item (@{ $dir_contents }) {
      if ( IsDirectory("$dir_item") ) 
      {
         Log($LOOP_TRACE_ENABLE, "\nExtractEFSDir loop: pushing directory: '$dir_item'" );
          
         # Prepend subdir path to item and add to directory search list.
         # Src root is already included above, so make sure not to prepend here
         $dir_item = BuildPath( $subdir, "$dir_item" ) if ($dirlevel != 0);
         push( @{$dir_list_ref}, $dir_item );

         Log($LOOP_TRACE_ENABLE, "\nExtractEFSDir loop: finished pushing dir: '$dir_item'" );
         Log($LOOP_TRACE_ENABLE, "\tNew directory search list:", $dir_list_ref );
          
      }
      else # directory item is a file
      {
         $dir_item = BuildPath( $subdir, "$dir_item" ) if ($dirlevel != 0);

         Log($LOOP_TRACE_ENABLE, "\nExtractEFSDir loop: extracting file: '$dir_item'" );
          

         my $full_target_path = BuildPath( $target_dir, "$dir_item" );
            $full_target_path = CanonicalizePathUnix( "$full_target_path" );
                          
         Log($FILEPATHS_TRACE_ENABLE, "\nPaths sent to buildPath", 
                 [
                   { "src_dir_root" => $src_dir_root  },
                   { "subdir"       => $subdir        },
                   { "dir_item"     => $dir_item      },
                 ]
            );
          
         # Sbudir path was prepended at the top of this else...no need to include it here
         my $full_local_path  = BuildPath( "$src_dir_root", "$dir_item" );
            $full_local_path  = CanonicalizePathWindows( "$full_local_path" );

          
         Log($FILEPATHS_TRACE_ENABLE, "\nDirectory level: sending paths to ExtractEFSFile" );
         Log($FILEPATHS_TRACE_ENABLE, "\t full_path_target: '$full_target_path'" );
         Log($FILEPATHS_TRACE_ENABLE, "\t full_path_local : '$full_local_path'"  );
          
         my $file_item = ExtractEFSFile( "0",
                                         "0",
                                         $attributes,
                                         "efs_dir", 
                                         $full_target_path, 
                                         $full_local_path,
                                         $variant,
                                         $metaData,
                                       );
         push( @{ $file_list_ref }, $file_item);
         #print STDOUT "ExtractEFSdir file_item\n\t" . Dumper($file_list_ref);

         Log($LOOP_TRACE_ENABLE, "\nExtractEFSDir loop: finished extraction: '$dir_item'" );
         Log($LOOP_TRACE_ENABLE, "New directory file list:", $file_list_ref );
      }
   }
}

sub ExtractItemMetaData
{
   my ($rows_ref, $item) = @_;
   my $item_type = $item->{ItemType};
   
   if (ItemIsSubscriber( $item->{Attributes} ))
   {
     my $row = GetNextRow( $rows_ref );
     my $subs_label  = $row->[ GetFieldIndex("Filepath", $COLUMN_INDICES->{EFS_FILE}) ];
     # my $row_type  = $row->[ GetFieldIndex("EFS Item Type", $COLUMN_INDICES->{EFS_FILE}) ];
     # my $row_size  = $row->[ GetFieldIndex("EFS Item Size", $COLUMN_INDICES->{EFS_FILE}) ];
     # my $row_value = $row->[ GetFieldIndex("Value"        , $COLUMN_INDICES->{EFS_FILE}) ];     
     my $row_type;
     my $row_size;
     my $row_value;
     my @spare_row; # used to fill gap between 2 efs file entries if 1st is multiSIM

     # Note: This check is no longer valid since efs listings can occur back to back 
     # without a subs_mask in between them
     # These fields should be blank when entering metadata
     # my @blank_flds = ( $row->[ GetFieldIndex("Type"      , $COLUMN_INDICES) ],
     #                    # $row->[ GetFieldIndex("Identifier", $COLUMN_INDICES) ], # subs_mask can now exist in identifier column
     #                    $row->[ GetFieldIndex("Attributes", $COLUMN_INDICES) ],
     #                  );
     # foreach my $field (@blank_flds)
     # {
     #   if (! IsBlank($field)) # Cells should be blank
     #   {
     #     die "Error: Subscription listing must only contain data between (and not including) " . 
     #         "the item type and the attributes columns (i.e. only in columns D-F)";
     #   }
     # }

     # Assign default values if no multiSIM information listed
     if (! IsSubsMaskPresent($subs_label))
     {
       # put $row back into $rows_ref. Since no multiSIM information listed row likely contains another NV/EFS item
       unshift(@{ $rows_ref}, $row);
       
       # Create a spare row to populate with default subs_mask values and process
       @spare_row = ("") x scalar(@{ $row });
       $row = \@spare_row;

       $row->[ GetFieldIndex("EFS Item Type", $COLUMN_INDICES->{EFS_FILE}) ] = $DFLT_SUBS_TYPE;
       $row->[ GetFieldIndex("EFS Item Size", $COLUMN_INDICES->{EFS_FILE}) ] = $DFLT_SUBS_SIZE;
       $row->[ GetFieldIndex("Value"        , $COLUMN_INDICES->{EFS_FILE}) ] = $DFLT_ACTIVE_SUBS;     
       
     } 
    
     $row_type  = $row->[ GetFieldIndex("EFS Item Type", $COLUMN_INDICES->{EFS_FILE}) ];
     $row_size  = $row->[ GetFieldIndex("EFS Item Size", $COLUMN_INDICES->{EFS_FILE}) ];
     $row_value = $row->[ GetFieldIndex("Value"        , $COLUMN_INDICES->{EFS_FILE}) ];     
     
     # Data must be present in these cells
     if (IsBlank($row_type) || IsBlank($row_size) || IsBlank($row_value))
     {
       die "Error: Missing either subsmask type, size or value in multiSIM listing: $item->{Filepath}\n";
     }

     if($row_type =~ /^int$/ || $row_type =~ /int\[\d+\]/ || $row_type =~ /int\[\]/ || $row_type =~ /^string$/) 
     {
       my $element = CreateNewElement( $item_type, $row_type, $row );
       if ($item_type eq 'efs_dir')
       {
         # $item->{Elements} is used for a file list for efs_dir 
         # so metaData info is pushed to different section in this case
         push @{ $item->{MetaData} }, $element;
       }
       else
       {
         push @{ $item->{Elements} }, $element;
       }
       #print STDOUT "ExtractItemMetaData item\n\t" . Dumper($item);
     }
   }
}

# Name:     ExtractEFSDirTree
#
# Purpose:  Loops around ExtractEFSDir, which gathers a list of all directories and files in the directory tree.
#           Each directory read is pushed onto the directory list to be processed during another loop iteration. 
#           Each file read is extracted and added to the file elements list
#
# Return:   returns an mcfg_item. The item's element list, however, is a listing of all the files found in the
#           directory tree. 
sub ExtractEFSDirTree {
   my ($rows_ref, $next_row, $attributes) = @_;
   my $dir_tree_size  = 0;
   my @dir_search_list = ();
   my $dir;
   my $variant = GetVariantType($next_row);
   my $item = { 
            Attributes => $attributes,
            ItemType   => "efs_dir",      
          # Filepath   => filepath is used for directory files, but not the directory itself
          # Filename   => filename is used for directory files, but not the directory itself
            Elements   => [],
            MetaData   => [], # will be used to store data about the data, e.g. subscription mask
            DataSize   => 0,
            Variant    => $variant,
           };

   my $target_dir = $next_row->[ GetFieldIndex("Filepath" , $COLUMN_INDICES->{EFS_FILE}) ]; # Path on target
   my $src_dir_root = $next_row->[ GetFieldIndex("Filename" , $COLUMN_INDICES->{EFS_FILE}) ]; # Path on local PC

   ExtractItemMetaData($rows_ref, $item);
   #print STDOUT "ExtractEFSdirtree \n\t" . Dumper($item);
   # Push the top-level directory to initialize the search list 
   push (@dir_search_list, $src_dir_root);

   while ( @dir_search_list ) 
   {
      # A 'shift' is called on the directory list within ExtractEFSDir. This front 'pop' prevents this loop = inifinity
      ExtractEFSDir( \@dir_search_list, 
                      $item->{Elements}, 
                      $src_dir_root, 
                      $target_dir,
                      $item->{Attributes},
                      $item->{Variant},
                      $item->{MetaData},
                    );
   }

   foreach my $file ( @{ $item->{Elements} } )
   {
      Log( $DATASIZE_TRACE_ENABLE, "\nAdding size from file: '$file->{Filename}'", 
           [ 
             { "Curr tree Size"          => $dir_tree_size                     },
             { "File Size with MCFG hdr" => $file->{DataSize}                  },
             { "Total Size"              => $file->{DataSize} + $dir_tree_size },
           ]
         );

      $dir_tree_size += $file->{DataSize};
   }                                                       

   $item->{DataSize} = $dir_tree_size;
   #print STDOUT "ExtractEFSdirtree exit\n\t" . Dumper($item);
   return $item;
}

sub IsProbablyElement
{
  my (@el_attributes) = @_;
  # Require at least one item to be present before tracking whether data is missing. 
  # Blank lines would be flagged as errors otherwise.
  my $threshold = 1; 
  my $tally = 0;

  foreach my $data (@el_attributes)
  {
    if (! IsBlank($data))
    {
      $tally++;
    }
  }
  return ($tally >= $threshold);
}

sub CheckElementFormat
{
  my ($row, $item) = @_;
  my $type = $item->{ItemType};
  my @el_attributes = (); # These element attributes cannot be blank
  my $id = '';

  if ($type =~ /efs/)
  {
    $id = $item->{Filepath};
    @el_attributes = ( $row->[GetFieldIndex("EFS Item Type", $COLUMN_INDICES->{EFS_FILE}) ],
                       $row->[GetFieldIndex("EFS Item Size", $COLUMN_INDICES->{EFS_FILE})], 
                       #$row->[GetFieldIndex("Value", $COLUMN_INDICES->{EFS_FILE})], 
                     );
  }
  elsif ($type =~ /scalar/)
  {
    $id = $item->{ID};
    @el_attributes = ( $row->[GetFieldIndex("Type", $COLUMN_INDICES) ],
                       $row->[GetFieldIndex("Size", $COLUMN_INDICES->{MCFG_ITEM}->{SCALAR})], 
                       #$row->[GetFieldIndex("Value", $COLUMN_INDICES->{MCFG_ITEM}->{SCALAR})], 
                     );
  }

  if (IsProbablyElement(@el_attributes))
  {
    foreach my $el_attrib (@el_attributes)
    {
      if (IsBlank($el_attrib))
      {
        die "Error: Listing for $id is missing data of type, length, or value for one of its elements\n";
      }
    }
  }
}

sub ExtractEFSItem($$$) {
   my ($rows_ref, $row, $attributes) = @_;

   my $filepath = Trim($row->[ GetFieldIndex("Filepath"    , $COLUMN_INDICES->{EFS_FILE}) ]);
   my $item = { 
            Attributes => $attributes,
            ItemType   => "efs_item",      
            Filepath   => $filepath, 
           # Filename   => filenames are not used for EFS_items; data is obtained from the spreadsheet itself
            Elements   => [],
            Payload    => 0, 
            DataSize   => 0, 
            Variant    => 0,
           };

   $item->{Variant} = GetVariantType($row);

   # erase the attributes field
   $row->[GetFieldIndex("Attributes", $COLUMN_INDICES)] = "";
   $attributes = "";

   # Getting to this point means the attributes and ID fields are populated
   $row = GetNextRow( $rows_ref );
   my $row_type = $row->[ GetFieldIndex("EFS Item Type", $COLUMN_INDICES->{EFS_FILE}) ];
   my $row_value = $row->[GetFieldIndex("Value", $COLUMN_INDICES->{EFS_FILE})];
   my $row_size = $row->[GetFieldIndex("EFS Item Size", $COLUMN_INDICES->{EFS_FILE})];
   $filepath    = $row->[ GetFieldIndex("Filepath"     , $COLUMN_INDICES->{EFS_FILE}) ];

   
   # Filepath is where subs_mask will reside; special case of !IsNextItem
   if (ItemIsSubscriber($item->{Attributes})) 
   {
     if (!IsSubsMaskPresent($filepath))
     {
       $element = newElement( $DFLT_SUBS_TYPE, $DFLT_SUBS_SIZE, $DFLT_ACTIVE_SUBS );
       push @{ $item->{Elements} }, $element;
     }
     else
     {
       # clear filepath so false positive isn't found for a new NV/EFS item
       $filepath = '';
     }
   }

   # Item is listed on multiple lines
   while($row && (scalar(@$row) > 1) && (!IsNextItem($filepath, $attributes)))
   {
     #print Dumper($row);
     CheckElementFormat($row, $item);
     if($row_type =~ /^int$/ || $row_type =~ /int\[\d+\]/ || $row_type =~ /int\[\]/ || $row_type =~ /^string$/) 
     {
       my $element = CreateNewElement( $item->{ItemType}, $row_type, $row );
       push @{ $item->{Elements} }, $element;
     }
     
     $row = GetNextRow( $rows_ref );
     $filepath = $row->[ GetFieldIndex("Filepath"     , $COLUMN_INDICES->{EFS_FILE}) ];
     $row_type = $row->[ GetFieldIndex("EFS Item Type", $COLUMN_INDICES->{EFS_FILE}) ];
     $row_value = $row->[GetFieldIndex("Value", $COLUMN_INDICES->{EFS_FILE})];
     $row_size = $row->[GetFieldIndex("EFS Item Size", $COLUMN_INDICES->{EFS_FILE})];
     $attributes = $row->[GetFieldIndex("Attributes"  , $COLUMN_INDICES)];
   }

   if(@$row) {
     unshift(@$rows_ref, $row);
   }
 
   $item->{Payload}  = CalculateItemPayloadLength($item->{Elements}, $item->{Attributes});

   if ( $item->{Variant} == $variantValues->{slot_nv_item} ) 
   {
      $item->{ItemType} = 'scalar';
      $item->{DataSize} = $NV_HEADER_LENGTH + $item->{Payload};
   }
   else 
   {
      $item->{DataSize} = $EFS_HEADER_LENGTH + length($item->{Filepath}) + $item->{Payload};
   }

   Log($EFS_EXTRACTION_TRACE_ENABLE | $ALL_EXTRACTION_TRACE, "\nEFS file extracted: $item->{Filename}", $item  );
   Log($DATASIZE_TRACE_ENABLE, "Payload: '$item->{Payload}, Size: '$item->{DataSize}'");

   return $item;
}

# Parameters: Any single variable
# Returns:   Boolean value indicating whether value is present or not
sub IsDefined {
   my $variable = shift;

   return ( (defined $variable) ? 1 : 0 );
}

# Parameters: Hash representing data for an item's element
# Primarily used for determining whether efs types have
# addtnl data listed beneath them, e.g. subs mask
# Returns: boolean value 
sub IsMetaData($)
{
  my ($member) = @_;

  # hashes with filename define represent the usual 
  # 1 line listing for an efs type
  return (! defined($member->{Filename}));
}

# Compares two strings without respect to upper/lower case
# Returns true if they match
sub strmatch($$)
{
  (my $a, $b) = @_;

  return (lc $a eq lc $b);
}

sub ExtractPrlItem($$$) {
   my ($rows_ref, $next_row, $attributes) = @_;
   my $item = { 
                Attributes => $attributes,
                ID         => undef,
                ItemType   => "prl",
                Elements   => [],
                Payload    => undef,
                DataSize   => undef,
                CarrierID  => undef,
              };

   $item->{ID}      = $next_row->[ GetFieldIndex("ID"      , $COLUMN_INDICES->{MCFG_ITEM}->{PRL_FILE}) ];
   my $prl_filename = $next_row->[ GetFieldIndex("Filename", $COLUMN_INDICES->{MCFG_ITEM}->{PRL_FILE}) ];
   my $prl_version  = $next_row->[ GetFieldIndex("Version" , $COLUMN_INDICES->{MCFG_ITEM}->{PRL_FILE}) ];

   if (ItemIsDeleteOrSWOnly( $item->{Attributes} )) {
     $item->{DataSize} = $MCFG_HEADER_LENGTH + IsDefined( $item->{CarrierID} ) + $PRL_SUBHEADER_LENGTH;
      # print STDOUT "prl item $item->{ID} marked for deletion\n";
      # print STDOUT "size for prl item delete: $item->{DataSize}\n";
      return $item;
   }

   my $element = { 
                  Type     => "prl",
                  Filename => $prl_filename,
                  Version  => $prl_version,
                 };

   push(@{$item->{Elements}}, $element);
   
   $item->{Payload}  = CalculateItemPayloadLength($item->{Elements}, $item->{Attributes});
   # If the carrier_id is defined then add 1 byte to the header for its output
   $item->{DataSize} = $MCFG_HEADER_LENGTH + IsDefined( $item->{CarrierID} ) + 
                       $PRL_SUBHEADER_LENGTH + $item->{Payload};
   return $item;
}

sub ExtractTrlItem($$$) {
   my ($rows_ref, $next_row, $attributes) = @_;
   my $item = { 
            Attributes => $attributes,
            ItemType   => "trl",
            Elements   => [],
            Payload    => undef,
            DataSize   => undef,
           };
   my $row_type = $next_row->[GetFieldIndex("Type", $COLUMN_INDICES)];

   # erase the attributes field
   $next_row->[GetFieldIndex("Attributes", $COLUMN_INDICES)] = "";
   $attributes = "";

   # until we find the next item
   while(length($attributes) == 0) {
   # find the next row with a value on it
     while(length($attributes) == 0 and length($row_type) == 0 and @$next_row) {
       $next_row = GetNextRow( $rows_ref );
       $attributes = $next_row->[ GetFieldIndex("Attributes", $COLUMN_INDICES) ];
       $row_type   = $next_row->[ GetFieldIndex("Type"      , $COLUMN_INDICES->{TRL_RECORD}) ];
     }

     if(!@$next_row) {
       last;
     }

     if(length($attributes) == 0) {
       if($row_type =~ /^int$/ || $row_type =~ /int\[\d+\]/ || $row_type =~ /int\[\]/ || $row_type =~ /^string$/) {
         my $element_size  = $next_row->[GetFieldIndex("Size", $COLUMN_INDICES->{TRL_RECORD})];
         my $element_value = ParseValue($next_row->[GetFieldIndex("Value", $COLUMN_INDICES->{TRL_RECORD})], $row_type, $element_size);

         if ($row_type =~ /^string$/)
         {
           $element_value =~ s/\s+//g;
           $element_size = length($element_value);

           # TRL record follows TLV format (type length value). Types with a value containing string will need 
           # corresponding length element adjusted if spaces are removed. 
           # "length" is stored in last element in $item->{Elements} array ref (hence index -1)
           $item->{Elements}[-1]->{Value} = $element_size;
         }

         my $element = { 
                     Type  => $row_type,
                     Size  => $element_size,
                     Value => $element_value
                    };

         push(@{$item->{Elements}}, $element);
       }

       $next_row = GetNextRow( $rows_ref );
       $attributes = $next_row->[ GetFieldIndex("Attributes", $COLUMN_INDICES) ];
       $row_type   = $next_row->[ GetFieldIndex("Type"     , $COLUMN_INDICES->{TRL_RECORD}) ];
     }
   }

   if(@$next_row) {
     unshift(@$rows_ref, $next_row);
   }

   
   $item->{Payload} = $TRL_MAGIC_NUM_LENGTH + CalculateItemPayloadLength($item->{Elements}, 0x09);
   # Although padding is printed within this record it is not included in DataSize sum. Once DataSize from
   # all elements has been tallied, padding is calculated and then included
   $item->{DataSize} = $TRL_HEADER_LENGTH + $item->{Payload} + $TRL_FOOTER_LENGTH;

   return $item;
}

# Parameters: rows_ref, which is a references to an array of array references
# Returns:   an element (i.e. row) of rows_ref after removing leading and trailing 
#            whitespace from each element
sub GetNextRow { 
   my ($rows_ref) = @_;

   my $row = shift @$rows_ref;
   @$row = map { Trim($_) } @$row;

   return  $row;
}

# name: PostProcessItem
# 
# args: $item  hash ref representing an mcfg NV item
# 
# Notes: 
# Certain attributes affect the size and values listed in the final output 
# of an MCFG NV listing. Specifically, the following:  mcfg structure DataSize, reported Payload size, and actual payload. 
# Below is a brief table listing how attributes affect the MCFG structure of an NV / EFS item. 
#
# |  Attribute Settings   |              Affects on MCFG structure         |
# | Del/SW Only | Indexed | Reported MCFG DataSize | Reported Payload Size |
# --------------------------------------------------------------------------
#       0           x     |   Unchanged                  Unchanged        
#       1           0     |   Size of Headers            0        
#       1           1     |   Hdr_size + Indx_len        Index_len                          

sub PostProcessItem {
   my ($item) = @_;

   my $indexedItem = ItemIsIndexed($item->{Attributes});
   my $swOnlyItem  = ItemIsSoftwareOnly($item->{Attributes});
   my $muxedItem   = ItemIsMuxd($item->{Attributes});
   my $subItem     = ItemIsSubscriber($item->{Attributes});
   my $elements    = $item->{Elements};
   my $temp_element;

   # No post processing done on trl or efs_directory items
   if (($item->{ItemType} eq 'trl') || ($item->{ItemType} eq 'efs_dir')) {
      return $item; 
   }

   if( ItemIsDeleteOrSWOnly( $item->{Attributes} ) ) 
   {
      if ($subItem) {
         if ($indexedItem) {
            # The header size is gotten by subtracting the payload size from the total size. Index and sub_id lengths are then added back in. 
            # This ensures the correct size is calculated for NV and EFS items, which have a varying path length in their header
            $item->{DataSize} = $item->{DataSize} - $item->{Payload} + $SUB_ID_LENGTH + $INDEX_LENGTH;
            $item->{Payload}  = $SUB_ID_LENGTH + $INDEX_LENGTH;
            $temp_element = [(@$elements[0], @$elements[1])]; # save the first and 2nd elements (sub_id and index respectively)
         }
         else { # multi-subscription/non-indexed/SW only item
            $item->{DataSize} = $item->{DataSize} - $item->{Payload} + $SUB_ID_LENGTH;
            $item->{Payload}  = $SUB_ID_LENGTH; # A subscriber/non-indexed/sw-only item will have just the sub_id as the payload
            $temp_element = [shift(@$elements)]; # save the first element (the sub_id)
         }
         # Replace payload contents with temp_element (equates to index or nothing)
         $item->{Elements} = $temp_element;
      }
      else { # Not a multiple subscription item
         if ($indexedItem) {
            # The header size is gotten by subtracting the payload size from the total size. Index length is then added back in. 
            # This ensures the correct size is calculated for NV and EFS items, which have a varying path length in their header
            $item->{DataSize} = $item->{DataSize} - $item->{Payload} + $INDEX_LENGTH;
            $item->{Payload}  = $INDEX_LENGTH;
            $temp_element = [shift(@$elements)]; # save the first element (the index)
         }
         else { # non-indexed SW only item
            $item->{DataSize} = $item->{DataSize} - $item->{Payload};
            $item->{Payload}  = 0;# A non-indexed sw-only item will have no payload
            $temp_element = (); # no index to save so create an empty arry
         }
         # Replace payload contents with temp_element (equates to index or nothing)
         $item->{Elements} = $temp_element;
      }
   }
   return $item;
}

sub ExtractCarrierData($) {
   my ($xmlParseTree) = @_;
   my $mcfg_carrier_data = {
      "Carrier Index" => undef,
      "Configuration Type" => undef,
      "Full MCFG Version" => undef,
   };

   my $index = 0;
   while($index < scalar(@$xmlParseTree)) {
      my $next_item_tag = $xmlParseTree->[$index];
      my $next_item_body = $xmlParseTree->[$index+1];

      if($next_item_tag eq "NvConfigurationData") {
         my $attributes = shift(@{$next_item_body});
         $mcfg_carrier_data->{"Carrier Index"} = $attributes->{carrierIndex};
         $mcfg_carrier_data->{"Configuration Type"} = $attributes->{type};
         $mcfg_carrier_data->{"Full MCFG Version"} = $attributes->{version};
         splice(@$xmlParseTree, $index, 2);
         last;
      }

      $index += 2;
   }

   if (!defined($mcfg_carrier_data->{"Configuration Type"}) || IsBlank($mcfg_carrier_data->{"Configuration Type"}))
   {
     die("Error: Type value in source XML is blank. This must be present to identify configuration type (e.g. SW or HW type)\n");
   }

   return $mcfg_carrier_data;
}

# alternative to ExtractNextMcfgItem. This is used
# to process an MCFG xml format instead of the usual
# 2D grid of data obtained from a spreadsheet
#
# Inputs:
#     $xmlParseTree -> parsed xml tree created by XML::Parser module
# Returns:
#     an mcfg item hash just like ExtractNextMcfgItem
sub ExtractNextMcfgItemXML($) {
   my ($xmlParseTree) = @_;

   # NV itemss are considered scalars
   my $handler  = \&ExtractNVItemXML;
   my $handlers = {
      "NvPrlFile"     => \&ExtractPrlItemXML,
      "NvEfsFile"     => \&ExtractEFSFileXML,
      "NvEfsDir"      => \&ExtractEFSDirTreeXML,
      "NvEfsItemData" => \&ExtractEFSItemXML,
      "NvTrlRecord"   => \&ExtractTrlItemXML,
      "GroupFile"     => \&ExtractGroupFileXML, 
   };
   my $next_item_tag = shift(@$xmlParseTree);
   my $next_item_body = shift(@$xmlParseTree);

   if(not defined $next_item_tag) {
      return {};
   }

   if(defined $handlers->{$next_item_tag}) {
      $handler = $handlers->{$next_item_tag};
   }

   my @items = &{$handler}($next_item_body);  # handlers return a reference
   foreach $item (@items) {  # DOUBLE CHECK THIS, ref being passed in, but new value being saved? should just modify ref
      # blacklisted item testing is not required for group/prl/trl/efs_dir type of mcfg items.
	  if (!($next_item_tag =~ /NvEfsDir|NvPrlFile|NvTrlRecord|GroupFile/)){
		# Verify item is not a blacklisted item
		CheckForBlacklisting(GetItemIdentifier($item));
	  }
      $item = PostProcessItem($item);
   }
   
   return @items;
}

# Reads the list of members for NV items
# and certain EFS items. A member is just
# one of the data structures inside of an
# NV Item
#
# Inputs:
#     $xmlMembers - array of members as created by the xml parser
# Returns:
#     a list of nv item elements ($item->{Elements} array)
sub ExtractMembersList($) {
   my ($xmlMembers) = @_;
   my @membersList = ();

   while(scalar(@$xmlMembers)) {
      shift(@$xmlMembers); # remove Member tag element
      push(@membersList, shift(@$xmlMembers));
   }

   return map { MemberToElementFormat($_); } @membersList;
}

# converts XML member to an element as contained in
# the $item->{Elements} array of each mcfg item
sub MemberToElementFormat($) {
   my ($member) = @_;
   my $xmlSize = $member->[0]->{"sizeOf"};
   my $xmlType = $member->[0]->{"type"};
   my $xmlValue = $member->[2];
   my ($size, $type, $value) = XMLSizeTypeValueTranslate($xmlSize, $xmlType, $xmlValue);

   return newElement($type, $size, $value);
}

# translates from XML type, size and value data
# to that used in spreadsheets
sub XMLSizeTypeValueTranslate($$$) {
   my ($xmlSize, $xmlType, $xmlValue) = @_;
   my @valueArray = split(/\s+/, $xmlValue);
   my ($size, $type, $value);

   $type = "int[$xmlSize]";

   # remove leading u if any
   $xmlType = substr($xmlType, 1) if($xmlType =~ /^u/);
   # remove int part so only size remains
   $xmlType = substr($xmlType, 3);

   # size in bytes of each member
   $size = int($xmlType)/8;

   # pad with 0s
   while($xmlSize > scalar(@valueArray)) {
      push(@valueArray, 0);
   }

   @valueArray = map { ParseValue($_, "int", $size) } @valueArray;

   $value = [@valueArray];

   return ($size, $type, $value);
}

# just calculates the total size of an mcfg xml item
# by adding up the size of each of its members (elements)
sub CountBytesInMemberList($) {
   my ($elements) = @_;
   my $sizeInBytes = 0;
   for my $element (@$elements) {
      my $bytesPerItem = $element->{Size};
      my $itemsInArray = scalar(@{$element->{Value}});

      $sizeInBytes += $bytesPerItem*$itemsInArray;
   }

   return $sizeInBytes;
}

# Each of the below functions with names that look like
# Extract[something]XML take an XML parse tree of a single
# mcfg item which might be of various types (prl, trl, efs,
# efs_dir, efs_item, scalar). Each function handles one of
# these types. It reads the data from the parse tree and
# creates an mcfg item hash which is just like the hash
# created by the extract functions for spreadsheet data.
sub ExtractNVItemXML($) {
   my ($itemParsedXML) = @_;
   my $xmlAttributes = shift(@$itemParsedXML);
   my $item = { 
      ID => $xmlAttributes->{id},
      Attributes => hex($xmlAttributes->{mcfgAttributes}),
      Variant    => $xmlAttributes->{mcfgVariant},
      ItemType   => "scalar",
      Payload    => 0, # Contains total size of all data elements within specific NV item
      DataSize   => 0, # Contains Payload + Header
      Elements   => [], 
   };

   if ($item->{ID} > 7232)
   {
     die ("Err: Invalid NV# for NV listing '$ID'. NVs beyond 7232 should be listed as efs or efs_item types\n");
   }
   elsif ($item->{ID} < 0)
   {
     die ("Err: Invalid NV# '$ID' for NV listing. NV values are always positive\n");
   }

   # $itemParsedXML 
   # [{"id" => "6", "mcfgAttributes" => "0x09"}, "Member", [...], "Member", [...]]
   my @members = ExtractMembersList($itemParsedXML);

   $item->{Elements} = \@members;

   # get payload and datasize fields
   my $sizeInBytes = CountBytesInMemberList($item->{Elements});

   $item->{Payload} = $sizeInBytes;
   $item->{DataSize} = $NV_HEADER_LENGTH + $sizeInBytes;

   Log($NV_EXTRACTION_TRACE_ENABLE | $ALL_EXTRACTION_TRACE, "\nNV item extracted: '$item->{ID}'", $item );
   Log($DATASIZE_TRACE_ENABLE, "Size: '$item->{DataSize}'");

   return $item;
}

sub ExtractPrlItemXML($) {
   my ($itemParsedXML) = @_;
   my $xmlAttributes = shift(@$itemParsedXML);
   #print "ASDF: " . $xmlAttributes->{mcfgAttributes} . "\n";
   my $item = { 
      Attributes => hex($xmlAttributes->{mcfgAttributes}),
      ID         => undef,
      ItemType   => "prl",
      Elements   => [],
      Payload    => undef,
      DataSize   => undef,
      CarrierID  => undef,
   };

   $item->{ID} = $PRL_ID;
   my $prl_filename = $xmlAttributes->{buildPath};
   my $prl_version  = $xmlAttributes->{prlVersion};

   if (ItemIsDeleteOrSWOnly( $item->{Attributes} )) {
      $item->{DataSize} = $MCFG_HEADER_LENGTH + IsDefined( $item->{CarrierID} ) + $PRL_SUBHEADER_LENGTH;
      return $item;
   }

   my $element = { 
      Type     => "prl",
      Filename => $prl_filename,
      Version  => $prl_version,
   };

   push(@{$item->{Elements}}, $element);

   $item->{Payload} = CalculateItemPayloadLength($item->{Elements}, $item->{Attributes});
   # If the carrier_id is defined then add 1 byte to the header for its output
   $item->{DataSize} = $MCFG_HEADER_LENGTH + IsDefined( $item->{CarrierID} ) + 
                  $PRL_SUBHEADER_LENGTH + $item->{Payload};

   return $item;
}

sub ExtractEFSFileXML($) {
   my ($itemParsedXML) = @_;
   my $xmlAttributes = shift(@$itemParsedXML);
   my $item = { 
      Attributes => hex($xmlAttributes->{mcfgAttributes}),
      ItemType   => "efs",
      Filepath   => $xmlAttributes->{targetPath}, # This datum is stored in both the item and element subhash for an EFS file
      Filename   => $xmlAttributes->{buildPath}, # EFS filename attribute is used for testing by mcfg_verify.pm
      Variant    => $xmlAttributes->{mcfgVariant},
      Elements   => [],
      Payload    => 0,
      DataSize   => 0,
   };
   my $member_sz = 0;

   # Any data still present in $itemParsedXML should reflect metaData
   if (scalar(@{ $itemParsedXML }) > 0)
   {
     my @members = ExtractMembersList($itemParsedXML);
     $item->{Elements} = \@members;
     # get payload and datasize fields
     $member_sz = CountBytesInMemberList($item->{Elements});
   }

   my $efs_filepath = $item->{Filepath};
   my $efs_filename = $item->{Filename};
   Log($FILEPATHS_TRACE_ENABLE, "\nEFS extraction"                    );
   Log($FILEPATHS_TRACE_ENABLE, "\t efs path target: '$efs_filepath'" );
   Log($FILEPATHS_TRACE_ENABLE, "\t efs local path:  '$efs_filename'" );

   my $element = newEFSFile( "efs", $item->{Filepath}, $item->{Filename});

   # element passed as an anon array ref since CalculateItemPayloadLength expects an arr_ref
   $element->{Payload} = CalculateItemPayloadLength( [ $element ], $item->{Attributes} ); 

   push( @{$item->{Elements}}, $element );

   $item->{Payload}  = $member_sz + $element->{Payload};
   $item->{DataSize} = $EFS_HEADER_LENGTH + length($item->{Filepath}) + $item->{Payload};

   return $item;
}

# needs to return an array of hash references of valid mcfg_items in this group
sub ExtractGroupFileXML($) {
   my ($itemParsedXML) = @_;
   my $xmlAttributes = shift(@$itemParsedXML);
   my $efs_filename = $xmlAttributes->{buildPath};
   my $efs_filepath = $xmlAttributes->{targetPath}; 
   my $expected_group_version = $xmlAttributes->{version}; 
   $efs_filename = $BUILD_ROOT . GetEFSFilename($efs_filepath, $efs_filename);

   # read xml group file
   open my $fh, '<', $efs_filename or die "Can't open file '$filename'!";
   my $xml_group_content = do { local $/; <$fh> };
   # print $xml_group_content;

   # parse xml
   my $parser = new XML::Parser(Style => 'Tree');
   my $xmlParseTree = $parser->parse($xml_group_content);
   $xmlParseTree = XmlStripBlanks($xmlParseTree);
   my $top = shift(@{$xmlParseTree->[1]}); # remove top level attributes hash
   $xmlParseTree = $xmlParseTree->[1];
   my $group_sheet_data = $xmlParseTree->[1]->[0];
   my $actual_group_version = lc($$group_sheet_data{'version'});

   my @mcfg_items;  # array of hash references to mcfg_items

   if (! strmatch($expected_group_version, $actual_group_version)) {
      print "==================================================================\n";
      print "Version mismatch in: $efs_filename\n";
      print "Main config expected group version: $expected_group_version\n";
      print "Actual group version: $actual_group_version\n";
      print "==================================================================\n";
      die("Version mismatch\n");
      return {};
   }

   my $mcfg_sheet_data = ExtractCarrierData($xmlParseTree);
   my @parsed_mcfg_items  = ExtractNextMcfgItemXML($xmlParseTree);  # array of mcfg_item hash references (always one or more)
   while (!isEmptyArray(\@parsed_mcfg_items) and !$version_mismatch) {
      foreach $mcfg_item (@parsed_mcfg_items) {
         push(@mcfg_items, $mcfg_item);  
      }
      @parsed_mcfg_items  = ExtractNextMcfgItemXML($xmlParseTree); 
   }

   return @mcfg_items;
}



sub ExtractEFSDirTreeXML($) {
   my ($itemParsedXML) = @_;
   my $xmlAttributes = shift(@$itemParsedXML);
   my $dir_tree_size  = 0;
   my @dir_search_list = ();
   my $dir;
   my $item = { 
            Attributes => hex($xmlAttributes->{mcfgAttributes}),
            Variant    => $xmlAttributes->{mcfgVariant},
            ItemType   => "efs_dir",      
            Elements   => [],
            DataSize   => 0,
           };

   my $target_dir = $xmlAttributes->{targetPath};
   my $src_dir_root = $xmlAttributes->{buildPath};

   # Push the top-level directory to initialize the search list 
   push (@dir_search_list, $src_dir_root);

   while ( @dir_search_list ) 
   {
      # A 'shift' is called on the directory list within ExtractEFSDir. This front 'pop' prevents this loop = inifinity
      ExtractEFSDir( \@dir_search_list, 
                      $item->{Elements}, 
                      $src_dir_root, 
                      $target_dir,
                      $item->{Attributes},
                      $item->{Variant},
                    );
   }

   foreach my $file ( @{ $item->{Elements} } )
   {
      Log( $DATASIZE_TRACE_ENABLE, "\nAdding size from file: '$file->{Filename}'", 
           [ 
             { "Curr tree Size"          => $dir_tree_size                     },
             { "File Size with MCFG hdr" => $file->{DataSize}                  },
             { "Total Size"              => $file->{DataSize} + $dir_tree_size },
           ]
         );

      $dir_tree_size += $file->{DataSize};
   }                                                       

   $item->{DataSize} = $dir_tree_size;
   return $item;
}

sub ExtractEFSItemXML($) {
   my ($itemParsedXML) = @_;
   my $xmlAttributes = shift(@$itemParsedXML);
   my $item = { 
      Attributes => hex($xmlAttributes->{mcfgAttributes}),
      ItemType   => "efs_item",      
      Filepath   => $xmlAttributes->{fullpathname}, 
      Variant    => $xmlAttributes->{mcfgVariant},
      Elements   => [],
      Payload    => 0, 
      DataSize   => 0, 
   };

   my @members = ExtractMembersList($itemParsedXML);
   $item->{Elements} = \@members;

   # get payload and datasize fields
   my $sizeInBytes = CountBytesInMemberList($item->{Elements});

   $item->{Payload} = $sizeInBytes;
   $item->{DataSize} = $EFS_HEADER_LENGTH + length($item->{Filepath}) + $sizeInBytes;
   
   Log($EFS_EXTRACTION_TRACE_ENABLE | $ALL_EXTRACTION_TRACE, "\nEFS file extracted: $item->{Filepath}", $item  );
   Log($DATASIZE_TRACE_ENABLE, "Payload: '$item->{Payload}, Size: '$item->{DataSize}'");

   return $item;
}

sub ExtractTrlItemXML($) {
   my ($itemParsedXML) = @_;
   my $xmlAttributes = shift(@$itemParsedXML);
   my $item = { 
      Attributes => hex($xmlAttributes->{mcfgAttributes}),
      ItemType   => "trl",
      Elements   => [],
      Payload    => undef,
      DataSize   => undef,
   };

   $item->{Elements} = [ExtractMembersList($itemParsedXML)];

   # get payload and datasize fields
   my $sizeInBytes = CountBytesInMemberList($item->{Elements});

   $item->{Payload} = $TRL_MAGIC_NUM_LENGTH + CalculateItemPayloadLength($item->{Elements}, 0x09);
   # Although padding is printed within this record it is not included in DataSize sum. Once DataSize from
   # all elements has been tallied, padding is calculated and then included
   $item->{DataSize} = $TRL_HEADER_LENGTH + $item->{Payload} + $TRL_FOOTER_LENGTH;

   return $item;
}

# Replaces ExtractNextMcfgItem. This differs from
# ExtractNextMcfgItem in that it will not remove the
# {Elements} list from an mcfg item. This is useful
# for the mcfg translate script with software only items
# because it needs to know not only the size but the elements
# that exist within that mcfg item
sub ExtractNextMcfgItemFull($) {
   my ($rows_ref) = @_;

   # NV items are considered scalars
   my $handler  = \&ExtractNVItem;
   my $handlers = {
     "prl"      => \&ExtractPrlItem,
     "efs"      => \&ExtractEFSFile,
     "efs_item" => \&ExtractEFSItem,
     "efs_dir"  => \&ExtractEFSDirInfo,
     "trl"      => \&ExtractTrlItem,
     "data_profile"  => \&ExtractEFSFile,
     "slot_efs_item" => \&ExtractEFSItem,
     "slot_efs"      => \&ExtractEFSFile,
     "slot_efs_dir"  => \&ExtractEFSDirInfo,
     "slot_nv_item"  => \&ExtractEFSItem,
     "nv_item"       => \&ExtractEFSItem,
     "config"        => \&ExtractEFSFile,
     "active_config" => \&ExtractEFSFile,
     "group"      => \&ExtractGroupFileListing,
   };
   my $row;
   my $attributes = "";
   my $item_id   = "";

   # Looks at attributes to find next row with a valid element
   while(!IsHexString( $attributes ))
   {
     # rows_ref fails when pointing to an empty array
     if (scalar(@$rows_ref) != 0) {
       $row = GetNextRow($rows_ref);
     }
     else { # No rows are left in $rows_ref 
       return {};
     }

     $item_id    = $row->[GetFieldIndex("Identifier", $COLUMN_INDICES)];
     $attributes = $row->[GetFieldIndex("Attributes", $COLUMN_INDICES)];
   }
   if(not ItemIsSoftwareOnly($attributes)) {
     my $error = CheckRowInputFormat(@{$row});
     if($error) {
        die "Input file could not be parsed for item '$item_id'\n\tReason: $error\n";
     }
   }
   my $row_type = $row->[GetFieldIndex("Type", $COLUMN_INDICES)];

   if(defined $handlers->{$row_type}) {
     $handler = $handlers->{$row_type};
   }

   my $item = &{$handler}($rows_ref, $row, hex($attributes));
   my @tempElements = @{$item->{Elements}}; # save the element list that could be removed by PostProcessItem

   # Verify item is not a blacklisted item
   CheckForBlacklisting(GetItemIdentifier($item));
   # Adjust reported sizes and payloads if needed
   $item = PostProcessItem($item);

   $item->{Elements} = \@tempElements; # restore element list

   return $item;
}

# reads next rows of spreadsheet to extract the next item
# as a data structure usable by the other function
sub ExtractNextMcfgItem($$) {
   my ($rows_ref, $flatten_groups) = @_;

   # NV itemss are considered scalars
   my $handler  = \&ExtractNVItem;
   my $handlers = {
     "prl"      => \&ExtractPrlItem,
     "efs"      => \&ExtractEFSFile,
     "efs_item" => \&ExtractEFSItem,
     "efs_dir"  => \&ExtractEFSDirTree,
     "trl"      => \&ExtractTrlItem,
     "data_profile" => \&ExtractEFSFile,
     "slot_efs_item" => \&ExtractEFSItem,
     "slot_efs" => \&ExtractEFSFile,
     "slot_efs_dir" => \&ExtractEFSDirTree,
     "slot_nv_item" => \&ExtractEFSItem,
   };

   if ($flatten_groups) {
      $handlers->{"group"} = \&ExtractGroupFileContents;
      # will return an array of mcfg hash item references
      # Each hash represents an item listed inside of the group xml file
   } else {
      $handlers->{"group"} = \&ExtractGroupFileListing;
      # will return a single hash item that represents the 
      # listing for the group 
   }

   my $row;
   my $attributes = "";
   my $item_id   = "";

   # Looks at attributes to find next row with a valid element
   while(!IsHexString( $attributes ))
   {
     # rows_ref fails when pointing to an empty array
     if (scalar(@$rows_ref) != 0) {
       $row = GetNextRow($rows_ref);
     }
     else { # No rows are left in $rows_ref 
       return {};
     }

     $item_id    = $row->[GetFieldIndex("Identifier", $COLUMN_INDICES)];
     $attributes = $row->[GetFieldIndex("Attributes", $COLUMN_INDICES)];
   }

   if(not ItemIsSoftwareOnly($attributes)) {
     my $error = CheckRowInputFormat(@{$row});
     if($error) {
        die "Input file could not be parsed for item '$item_id'\n\tReason: $error\n";
     }
   }

   my $row_type = $row->[GetFieldIndex("Type", $COLUMN_INDICES)];

   if(defined $handlers->{$row_type}) {
     $handler = $handlers->{$row_type};
   }

   # Notice that this is returned in an array context
   # Should always return 1 or more mcfg item hash references
   my @items = &{$handler}($rows_ref, $row, hex($attributes));

   foreach $item (@items) {
      # Adjust reported sizes and payloads if needed
      $item = PostProcessItem($item);
   }


   return @items;
}
sub WriteMCFGVersionInTrailerRecord {
   my ($trailerRecord, $hash, $crm_buildid) = @_;
   my $record_length = $trailerRecord->{DataSize};
   my $mcfgversion_size = length($hash);
   my $mcfgbuildversion_size = length($crm_buildid);
   my $length_bytes = 0;
   my $mcfgversiontype = { 
            Type  => "int[1]",
            Size  => 1,
            Value => [0x08],
           };
   my $mcfgversionsize = { 
            Type  => "int[1]",
            Size  => 2,
            Value => [$mcfgversion_size],
           };
   my @unpacked = unpack("C*", $hash);
   my $mcfgversionArray = \@unpacked;
   my $mcfgversion = { 
            Type  => "int[$mcfgversion_size]",
            Size  => 1,
            Value => $mcfgversionArray,
           };
   my $mcfgbuildversiontype = {   
            Type  => "int[1]",
            Size  => 1,
            Value => [0x09],
           };
   my $mcfgbuildversionsize = {   
            Type  => "int[1]",
            Size  => 2,
            Value => [$mcfgbuildversion_size],
           };
   my @unpacked = unpack("C*", $crm_buildid);
   my $mcfgbuildversionArray = \@unpacked;
   my $mcfgbuildversion = {   
            Type  => "int[$mcfgbuildversion_size]",
            Size  => 1,
            Value => $mcfgbuildversionArray,
           };
   push(@{$trailerRecord->{Elements}}, $mcfgversiontype);
   push(@{$trailerRecord->{Elements}}, $mcfgversionsize);
   push(@{$trailerRecord->{Elements}}, $mcfgversion);
   push(@{$trailerRecord->{Elements}}, $mcfgbuildversiontype);
   push(@{$trailerRecord->{Elements}}, $mcfgbuildversionsize);
   push(@{$trailerRecord->{Elements}}, $mcfgbuildversion);
   $length_bytes += $mcfgversion_size+3;
   $length_bytes += $mcfgbuildversion_size+3;
   $trailerRecord->{DataSize} += $mcfgversion_size+3;
   $trailerRecord->{DataSize} += $mcfgbuildversion_size+3;
   return $length_bytes;   
}

sub WriteTrailerRecordBin($$) {
   my ($trailerRecord, $paddingBytes) = @_;
   my $attribtes = $trailerRecord->{Attributes};
   my $record_length = $trailerRecord->{DataSize};
   my $payload_length = $record_length - $TRL_HEADER_LENGTH - $TRL_FOOTER_LENGTH; # Note: trailer footer = offset
   my $offset = $record_length + $paddingBytes;
   my $elements = $trailerRecord->{Elements};

   # write item length
   binwrite( pack('I', $record_length), 4);

   # Output the item type, attributes (set to 0) and spare bytes 
   binwrite( pack('C*', 0x0A, 0x00, 0x00, 0x00));

   # write item length
   my $magic = { Type  => "string",
                 Size  => 8,
                 Value => "MCFG_TRL",
               };
   unshift(@$elements, $magic);

   WriteTLVBin(0xA1, $payload_length, $elements);

 # Write number of bytes needed to make trailer record word aligned
   binwrite( pack('I', 0x0), $paddingBytes);
 
 # write offset (equals total size of trailer record in bytes)
   binwrite( pack('I', $offset));
}

sub WriteTrailerRecord($$) {
   my ($trailerRecord, $paddingBytes) = @_;
   my $attribtes = $trailerRecrod->{Attributes};
   my $record_length = $trailerRecord->{DataSize};
   my $payload_length = $record_length - $TRL_HEADER_LENGTH - $TRL_FOOTER_LENGTH; # Note: trailer footer = offset
   my $offset = $record_length + $paddingBytes;
   my $elements = $trailerRecord->{Elements};

   printf "/* Trailer Record */\n";
   # write item length
   printf " ,0x%02X, 0x%02X, 0x%02X, 0x%02X", ByteNOf($record_length, 1), ByteNOf($record_length, 2), 
                                              ByteNOf($record_length, 3), ByteNOf($record_length, 4);
   # write item type, attributes, and two extra bytes
   printf " ,0x%02X, 0x%02X, 0x%02X, 0x%02X\n", 0x0A, 0x00, 0, 0;

   my $magic = { Type  => "string",
                 Size  => 8,
                 Value => "MCFG_TRL",
               };
   unshift(@$elements, $magic);

   WriteTLV(0xA1, $payload_length, $elements);

   printf "\n";

   while($paddingBytes) {
     printf " ,0x00";
     $paddingBytes -= 1;
   }

   printf ",0x%02X, 0x%02X, 0x%02X, 0x%02X\n", ByteNOf($offset, 1), ByteNOf($offset, 2), 
                                               ByteNOf($offset, 3), ByteNOf($offset, 4);
}

sub WriteMcfgFooter {
   printf "\n} ; ";
}

# makes sure the build root string given ends with a slash
sub FixBuildRootString($) {
   my ($build_root) = @_;

   # Remove all directory separators from end of build root
   $build_root =~ s#(/|\\)+$##;

   # Replace with OS dependent separator
   if ($os =~ /Win/) 
   {
     $build_root .= '\\';
   }
   else
   {
     $build_root .= '/';
   }

   return $build_root;
}

# this removes all whitespace text elements
# from the parse tree. This essentially 
# makes a parse tree that ignores XML formatting
sub XmlStripBlanks($) {
   my ($parseTree) = @_;
   my $xmlStripBlanks;
   $xmlStripBlanks = sub {
      my ($parseTree) = @_;
      my $index = 1;

      while($index < scalar(@$parseTree)) {
         if($parseTree->[$index] eq "0") { # text nodes
            if($parseTree->[$index+1] =~ /^\s+$/) {
               # remove next two items for whitespace text node
               splice(@$parseTree, $index, 2);
            }
            else {
               $index += 2;
            }
         }
         else { # non-text node. recursively remove whitespace
            $xmlStripBlanks->($parseTree->[$index+1]);
            $index += 2;
         }
      }
   };

   unshift(@$parseTree, {}); # put in empty attributes hash for consistency
   $xmlStripBlanks->($parseTree);
   shift(@$parseTree);       # take out the empty attributes hash

   return $parseTree;
}

# Update the ELF & program headers with changes based on the MCFG segement size
sub UpdateHdrs($)
{
  my ($mcfg_seg_sz) = @_;
  my $aligned_addr = ($mcfg_seg_sz & ~($ELF_BLOCK_ALIGN -1)) + $ELF_BLOCK_ALIGN;
  my $image_dest_ptr;
  my $MI_BOOT_IMG_HDR_SIZE = 0x28;
  my $IMAGE_SIZE = 0x60; # Size of hash contents (96 bytes)

  if ($mcfg_seg_sz < 1)
  {
    die ("Error: MCFG segment size < 1B");
  }
  else
  {
    # Hash_phdr updates
    $hash_phdr->{p_vaddr} = unpackToHexString(pack('I', $aligned_addr), 4);
    $hash_phdr->{p_paddr} = $hash_phdr->{p_vaddr};

    # Elf32_Phdr updates
    $Elf32_Phdr->{p_filesz} = unpackToHexString(pack('I', $mcfg_seg_sz), 4);
    $Elf32_Phdr->{p_memsz} = $Elf32_Phdr->{p_filesz};

    # hash_hdr updates
    $image_dest_ptr = $aligned_addr + $MI_BOOT_IMG_HDR_SIZE;
    $hash_hdr->{image_dest_ptr} = unpackToHexString(pack('I', $image_dest_ptr), 4);
    $hash_hdr->{sig_ptr} = unpackToHexString(pack('I', $image_dest_ptr + $IMAGE_SIZE), 4);
    $hash_hdr->{cert_chain_ptr} = $hash_hdr->{sig_ptr};
  }
}

sub WriteHashHdr()
{
  #my ($out_file) = @_;
  my $hdr_str = '';
  my $hdr_sz;

  $hdr_str = $hash_hdr->{image_id}        . " " .
             $hash_hdr->{flash_parti_ver} . " " . 
             $hash_hdr->{image_src}       . " " .
             $hash_hdr->{image_dest_ptr}  . " " .
             $hash_hdr->{image_size}      . " " .
             $hash_hdr->{code_size}       . " " .
             $hash_hdr->{sig_ptr}         . " " .
             $hash_hdr->{sig_size}        . " " .
             $hash_hdr->{cert_chain_ptr}  . " " .
             $hash_hdr->{cert_chain_size} . " " ;

  # calculate total size for headers. This is needed later for calculating NULL padding
  $hdr_sz = scalar split(/\s+/, Trim($hdr_str));
  # Remove spaces in header string
  $hdr_str =~ s/\s+//g;

  # Write headers
  binwrite( pack('H*', $hdr_str));

  return $hdr_sz
}

sub WriteElfHdrs()
{
  #my ($out_file) = @_;
  my $hdrs = '';
  my $hdr_sz;

  # Build the string representation of binary header values
  # Order must be in 
  # 1) Elf32_Ehdr, 2) new_phdr, 3) hash_phdr, 4) Elf32_Phdr
  $hdrs = $Elf32_Ehdr->{e_ident}    . " " .
          $Elf32_Ehdr->{e_type}     . " " .
          $Elf32_Ehdr->{e_machine}  . " " .
          $Elf32_Ehdr->{e_version}  . " " .
          $Elf32_Ehdr->{e_entry}    . " " .
          $Elf32_Ehdr->{e_phoff}    . " " .
          $Elf32_Ehdr->{e_shoff}    . " " .
          $Elf32_Ehdr->{e_flags}    . " " .
          $Elf32_Ehdr->{e_ehsize}   . " " .
          $Elf32_Ehdr->{e_phentsize}. " " .
          $Elf32_Ehdr->{e_phnum}    . " " .
          $Elf32_Ehdr->{e_shentsize}. " " .
          $Elf32_Ehdr->{e_shnum}    . " " .
          $Elf32_Ehdr->{e_shstrndx} . " " ;

  for my $phdr ($new_phdr, $hash_phdr, $Elf32_Phdr)
  {
    $hdrs .= $phdr->{p_type}   . " " . 
             $phdr->{p_offset} . " " .
             $phdr->{p_vaddr}  . " " .
             $phdr->{p_paddr}  . " " .
             $phdr->{p_filesz} . " " .
             $phdr->{p_memsz}  . " " .
             $phdr->{p_flags}  . " " .
             $phdr->{p_align}  . " " ;
  }

  # calculate total size for headers. This is needed later for calculating NULL padding
  $hdr_sz = scalar split(/\s+/, Trim($hdrs));
  # Remove spaces in header string
  $hdrs =~ s/\s+//g;

  # Write headers
  binwrite( pack('H*', $hdrs));

  return $hdr_sz
}

sub WriteFileToTarget($$)
{
  my ($ip, $op) = @_;
  my $ip_sz = 0;
  my $ttl_sz = 0;
  my $file_buf;

  open(ip_fh, "<", $ip) || die "Could not open file for read: '$ip'\n";
  open(op_fh, ">>", $op) || die "Could not open file for append: '$op'\n";
  binmode(ip_fh);
  binmode(op_fh);

  $ip_sz = -s $ip;

  if ($ip_sz > 0)
  {
    #print STDOUT sprintf("0x%02X", $ip_sz) . "\n";
    while ($ttl_sz < $ip_sz)
    {
      $sz_read = sysread(ip_fh, $file_buf, $ip_sz);
      if ($sz_read == 0)
      {
        die "Error: Failure to read $ip_sz bytes from file: '$ip_fh'";
      }
      else
      {
        $ttl_sz += $sz_read;
      }
    }
    syswrite(op_fh, $file_buf, $ip_sz)|| die "Error: Failure to write $ip_sz bytes to file: '$op_fh'";
  }
  else
  {
    die "Error: ip file is less than 1B: '$ip'";
  }

  close(ip_fh);
  close(op_fh);

  return $ip_sz;
}

sub GenerateHash($$$)
{
  my ($target_hash, $target_phdr_elf, $target_seg_mcfg) = @_;
  my $sha;
  my $digest;
  my $dig_sz = 32;
  my $file_sz = 0;
  my $read_sz = 0;
  my $ttl_sz = 0;

  open(op_fh, ">", $target_hash) || die "Error: could not open '$target_hash' for hash creation";
  binmode(op_fh);

  # Include SHA module only when needed 
  # This ensures legacy building does not fail 
  # when generating default mbns
  require Digest::SHA;
  SHA->import(256);

  # Create a sha object to perform hash according to algorithm given (e.g. 256)
  $sha = Digest::SHA->new(256);

  # Create hash of Elf / program headers
  $sha->addfile($target_phdr_elf, "b");
  # Note that the digest method is a read-once operation. Once it has been performed, the Digest::SHA object is automatically 
  # reset in preparation for calculating another digest value. Call $sha->clone->digest if it's necessary to preserve the original digest state.
  $digest = $sha->digest;
  syswrite(op_fh, $digest, $dig_sz) || die "Could not write to output file";

  # Pad 32Bytes between hashes
  syswrite(op_fh, pack('C*', (0x00) x $dig_sz)) || die "Could not write to output file";

  # Open MCFG seg file and slurp data since $sha->addfile was not working well with large files 
  open(ip_fh, "<", $target_seg_mcfg) || die "Error: could not open '$target_phdr_elf' for hash creation";
  binmode(ip_fh);
  local $/ = undef;
  $f_buf = <ip_fh>;
  close(ip_fh);
  
  # Create hash of MCFG segment
  $sha->add($f_buf);

  # Note that the digest method is a read-once operation. Once it has been performed, the Digest::SHA object is automatically 
  # reset in preparation for calculating another digest value. Call $sha->clone->digest if it's necessary to preserve the original digest state.
  $digest = $sha->digest;
  syswrite(op_fh, $digest, $dig_sz) || die "Could not write to output file";

  close(op_fh);
}

sub GenerateMCFGTargetFrmSourceXML
{
  my ($xml, $input_file_name, $target_mbn, $build_root, $crm_buildid) = @_;

  my $target_file; 
  my $target_dir; 

  #cleanup target path and use it for filename / dir info
  $target_mbn = File::Spec->canonpath($target_mbn);
  ($target_file, $target_dir) = fileparse($target_mbn);
  # Create the destination directory if needed
  if (! -d $target_dir)
  {
    mkpath($target_dir) or die "Error creating dir '$target_dir': $!";
  }

  # Target files used during mbn creation
  my $target_base = File::Spec->catfile( $target_dir, $target_file );
  my $target_seg_mcfg = $target_base . "_seg.mcfg";
  my $target_hash = $target_base . ".hash";
  my $target_phdr_elf = $target_base . "_phdr.pbn";
  # Files may be needed to support signed mbns
  #my $target_cert_chain = $target_base . "_hash_cert_chain.mbn";
  #my $target_hash_full = $target_base . "_hash_sec.mbn";
  #my $target_pre_encrypt_elf = $target_base . "_pre_encrypt.pbn";
  #my $target_encrypt_xml = $target_base . "_enc_md.xml";

  my $hdr_sz = 0; 
  my $pad_sz = 0; 
  my $file_sz = 0; 

  # Generate the ELF segment for MCFG data
  my ($mcfg_seg_sz, $num_mcfg_items) = GenerateMcfgSegmentFileXML($xml, $input_file_name, $target_seg_mcfg, $build_root, $BINARY_OUTPUT_FILE_TYPE, 0, undef, $crm_buildid);

  # Update ELF headers for final mbn; updates must occur before the hash diegest is done 
  UpdateHdrs($mcfg_seg_sz);

  open(OUT_FILE, ">", $target_mbn) || die ("Error: !$ \n Could not open file: $target_mbn");
  select(OUT_FILE);
  binmode(OUT_FILE);

  # Write headers to mbn file
  $hdr_sz = WriteElfHdrs();

  # copy hdrs to separate file for easy hash generation
  copy $target_mbn, $target_phdr_elf;

  # Generate 2 hashes: one of the headers and the other of MCFG segment
  GenerateHash($target_hash, $target_phdr_elf, $target_seg_mcfg);

  ## Null Pad to 4k (0x1000)
  $pad_sz = $ELF_BLOCK_ALIGN - $hdr_sz;
  binwrite( pack('H2' x $pad_sz, 0x00));

  # Write hash hdr to mbn
  $hdr_sz = WriteHashHdr();

  # Write hash itself to mbn
  $file_sz = WriteFileToTarget($target_hash, $target_mbn);

  ## Null Pad hash segment to 8k (0x2000)
  $pad_sz = $ELF_BLOCK_ALIGN - ($hdr_sz + $file_sz);
  binwrite( pack('H2' x $pad_sz, 0x00));

  #Write MCFG segment to file
  WriteFileToTarget($target_seg_mcfg, $target_mbn);

  close(OUT_FILE);
  select(STDOUT);

  unless ($DEBUG_ENABLE) 
  {
    unlink $target_seg_mcfg || "Warn: Could not remove temp file: '$target_seg_mcfg'\n";
    unlink $target_hash     || "Warn: Could not remove temp file: '$target_hash'\n";
    unlink $target_phdr_elf || "Warn: Could not remove temp file: '$target_phdr_elf'\n";
  }

  return $mcfg_seg_sz, $num_mcfg_items;
}

# Input: Array of one more references to hashes
# Output: 1 for array with empty hashes, 0 for array with non empty hashes
sub isEmptyArray($) {
   my $mcfg_items = shift;  # reference to array of references to hashes
   my @mcfg_items = @$mcfg_items;  # array of references to hashes
   my $first_hash_ref = @mcfg_items[0];  # 1st hash reference
   my %first_hash = %$first_hash_ref;  # 1st hash
   if (%first_hash) {
      return 0;
   } else {
      return 1;
   }
}

sub GetPadding
{
  my ($boundary_sz, $data_len) = @_;
  my $padding = $boundary_sz - $data_len % $boundary_sz;

  return $padding;
}

sub AlignMCFGItems
{
  my ($mcfg_items, $length_bytes, $boundary_sz) = @_;
  my $aligned_item = [];
  my $padding = 0; 

  if ($boundary_sz =~ /^\d+$/ && $boundary_sz > 0)
  {
    foreach my $mcfg_item (@$mcfg_items) {
      $padding = 0;

      if ($mcfg_item->{'ItemType'} =~ /efs/)
      {
        $padding = GetPadding( $boundary_sz, ($EFS_HEADER_LENGTH + length( $mcfg_item->{'Elements'}->[0]->{'Filepath'})) );
        $length_bytes += $padding;
        $mcfg_item->{'DataSize'} += $padding; 
        $mcfg_item->{'Elements'}->[0]->{'Filepath'} .= "Z" x $padding ;
        push(@$aligned_item, $mcfg_item);  
      }
      else
      {
        die "Padding only supported for efs item types\n";
      }
    }
  }
  else
  {
    die ("Flag to align MCFG entries is set but alignment boundary is not set. boundary size = '$boundary_sz'\n");
  }

  return ($aligned_item, $length_bytes);
}

# Generic function for reading an xml and writing it to a file.
# This function should be preferred over duplicated branches of code
# Supports nv/efs groups
# $output_type = 1 for writing to a binary file (mbn)
# $output_type = 2 for writing to a default .c file
# $output_type = 3 for writing to a generic .c file (from mcfg_generate.pl)
# $output_type = 3 for writing a multi mbn
sub GenerateMcfgSegmentFileXML {
   my ($xml, $mcfg_input_file_name, $mcfg_output_file_name, $build_root, $output_type, $is_byte_aligned, $boundary_sz, $crm_buildid) = @_;
   # $mcfg_input_file_name is needed for a comment in .c file generation
   my $parser = new XML::Parser(Style => 'Tree');
   my $xmlParseTree = $parser->parse($xml);

   $xmlParseTree = XmlStripBlanks($xmlParseTree);
   
   my $temp = shift(@{$xmlParseTree->[1]}); # remove top level attributes hash
   $xmlParseTree = $xmlParseTree->[1];
   #print STDOUT Dumper($xmlParseTree);

   if($build_root) {
     $BUILD_ROOT = FixBuildRootString($build_root); #set the global build root
   }

   # There will always be at least 24 bytes in the array:
   # 16B for the MCFG header
   #  8B for the NV Customer and mcfg file version numbers
   my ($length_bytes, $num_mcfg_items) = (24, 1);
   TraceMcfgList($length_bytes, $num_mcfg_items);

   my $mcfg_sheet_data = ExtractCarrierData($xmlParseTree);
   my @parsed_mcfg_items  = ExtractNextMcfgItemXML($xmlParseTree);  # array of mcfg_item hash references (always one or more)
   my $mcfg_items = [];
   my $trailerRecord = {};

   # Make sure has hash of unique items is emptied before processing carrier NV items
   ResetUniqueItemList();

   my $loop = 0;
   while(!isEmptyArray(\@parsed_mcfg_items)) {
      $loop++;
      #print Dumper(@parsed_mcfg_items);
      #last;
      foreach $mcfg_item (@parsed_mcfg_items) {
        my %mcfg_item = %$mcfg_item;   #dereference hash
        my $item_id = GetItemIdentifier($mcfg_item);

        if($item_id eq "trl") {
          $length_bytes += $mcfg_item->{DataSize};
          $trailerRecord = $mcfg_item;
         }
        # Only include unique NV or EFS listings
        else {
          my @items_found;
          if($mcfg_item->{ItemType} eq 'efs_dir') {
            # assign list of EFS files found in the directory
            @items_found = ( @{ $mcfg_item->{Elements} } );
            }
          else {
            # assign just the single EFS file, but in list form
            @items_found = ($mcfg_item);
          }

          foreach my $item (@items_found) {
            if( IsItemUnique($item) ) {
              $length_bytes += $item->{DataSize};

               # if an invalid type is specified for an nv item it can result in
               # an empty item structure. it should only be read in if DataSize is
               # not zero
               if( ($mcfg_item->{DataSize} != 0) || ItemIsDeleteOrSWOnly($mcfg_item->{Attributes}) ) {
                 $num_mcfg_items++;
                 push(@$mcfg_items, $item);

                 TraceMcfgList($length_bytes, $num_mcfg_items, $loop);
               }
            }
            else { # Item has already been collected, and is not unique
               if ($item->{ItemType} =~ /efs/) {
                 printf "Skipping duplicate entry with filepath:\t'%s'\n", $item->{Filepath};
               }
               else {
                 #print Dumper($item);
                 printf "Skipping duplicate entry with ID:\t'%s'\n", $item->{ID};
               }
            }
          }
        }
      }
      @parsed_mcfg_items = ExtractNextMcfgItemXML($xmlParseTree);
   }

   # if(%$trailerRecord) {
   
      # open(OUT_FILE, ">", $mcfg_output_file_name) || die ("Could not open file: '$mcfg_output_file_name'\n");
      # select(OUT_FILE);   
      # binmode(OUT_FILE);
      # my @mcfg_items_copy =();
      # push @mcfg_items, @mcfg_items_copy;
      # my $trailerRecord_copy += $trailerRecord;
     
   # #  print "size used in mcfg header: length bytes = $length_bytes\n";
      # WriteMcfgHeaderBin(  $mcfg_output_file_name, $num_mcfg_items, $length_bytes,
                        # $mcfg_sheet_data->{"Carrier Index"}, int( $mcfg_sheet_data->{"Configuration Type"} )
                     # );
      # WriteMcfgVersionBin($mcfg_output_file_name, hex($mcfg_sheet_data->{"Full MCFG Version"}));

      # while(my $next_item = shift(@$mcfg_items_copy)) 
      # {
        # WriteMcfgItemBin($next_item);
      # }
      # WriteTrailerRecord($trailerRecord_copy, 4 - ($length_bytes % 4));
      # close(OUT_FILE);
      # select(STDOUT);
      # require Digest::SHA;
      # SHA->import(256);
      # $sha = Digest::SHA->new(256);
      # $sha->addfile($mcfg_output_file_name, "b");
      # $digest = $sha->digest;
      # $length_bytes += WriteMCFGVersionInTrailerRecord($trailerRecord, $digest, $crm_buildid);
   # }
   
   
   my $padding = 4 - ($length_bytes % 4);
   $length_bytes += $padding;

   open(OUT_FILE, ">", $mcfg_output_file_name) || die ("Could not open file: '$mcfg_output_file_name'\n");
   select(OUT_FILE);
   if ($output_type == $BINARY_OUTPUT_FILE_TYPE) 
   {
      binmode(OUT_FILE);

   #  print "size used in mcfg header: length bytes = $length_bytes\n";
      WriteMcfgHeaderBin( $mcfg_output_file_name, $num_mcfg_items, $length_bytes,
                          $mcfg_sheet_data->{"Carrier Index"}, int( $mcfg_sheet_data->{"Configuration Type"} ));
      WriteMcfgVersionBin($mcfg_output_file_name, hex($mcfg_sheet_data->{"Full MCFG Version"}));

      while(my $next_item = shift(@$mcfg_items)) 
      {
        WriteMcfgItemBin($next_item);
      }

      if(%$trailerRecord) {
        WriteTrailerRecordBin($trailerRecord, $padding);
      }

   } 
   elsif ($output_type == $DEFAULT_C_OUTPUT_FILE_TYPE or $output_type == $GENERIC_C_OUTPUT_FILE_TYPE) 
   {
      if ($output_type == $DEFAULT_C_OUTPUT_FILE_TYPE) 
      {
         WriteMcfgDefaultHeader($mcfg_output_file_name, $num_mcfg_items, $length_bytes, 
                                $mcfg_sheet_data->{"Carrier Index"}, int($mcfg_sheet_data->{"Configuration Type"}), $mcfg_input_file_name);
      } 
      elsif ($output_type == $GENERIC_C_OUTPUT_FILE_TYPE) 
      {
         WriteMcfgHeader($mcfg_output_file_name, $num_mcfg_items, $length_bytes, 
                         $mcfg_sheet_data->{"Carrier Index"}, int($mcfg_sheet_data->{"Configuration Type"}), $mcfg_input_file_name);
      }

      WriteMcfgVersion($mcfg_output_file_name, hex($mcfg_sheet_data->{"Full MCFG Version"}));

      while(my $next_item = shift(@$mcfg_items)) {
        WriteMcfgItem($next_item);
      }

      if(%$trailerRecord) {
        WriteTrailerRecord($trailerRecord, $padding);
      }

      WriteMcfgFooter();
   } 
   elsif ($output_type == $MULTI_MBN_OUTPUT_FILE_TYPE) 
   {
      if ($is_byte_aligned)
      {
        ($mcfg_items, $length_bytes) = AlignMCFGItems($mcfg_items, $length_bytes, $boundary_sz);
      }

      WriteMcfgMultiMBNHeader($mcfg_output_file_name, $num_mcfg_items, $length_bytes, $mcfg_sheet_data->{"Carrier Index"}, int($mcfg_sheet_data->{"Configuration Type"}));
      WriteMcfgVersion($mcfg_output_file_name, hex($mcfg_sheet_data->{"Full MCFG Version"}));

      while(my $next_item = shift(@$mcfg_items)) {
        WriteMcfgItem($next_item);
      }

      if(%$trailerRecord) {
        WriteTrailerRecord($trailerRecord, $padding);
      }

      WriteMcfgFooter();
   } else {
      die ("Unsupported GenerateMcfgSegmentFileXML argument for \$output_type = $output_type\n");
   }

   close(OUT_FILE);
   select(STDOUT);

   return ($length_bytes, $num_mcfg_items);
}

sub GenerateMcfgSourceFile($$$$$) {
   my ($rows_ref, $mcfg_sheet_data, $mcfg_input_file_name, $mcfg_output_file_name, $build_root) = @_;
   if($build_root) {
     $BUILD_ROOT = FixBuildRootString($build_root); #set the global build root
   }

   # There will always be at least 24 bytes in the array:
   # 16B for the MCFG header
   #  8B for the NV Customer and mcfg file version numbers
   my ($length_bytes, $num_mcfg_items) = (24, 1);
   TraceMcfgList($length_bytes, $num_mcfg_items);

   my $flatten_groups = 1;
   my @parsed_mcfg_items = ExtractNextMcfgItem($rows_ref, $flatten_groups);
   my $mcfg_items = [];
   my $trailerRecord = {};

   # Make sure has hash of unique items is emptied before processing carrier NV items
   ResetUniqueItemList();

   my $loop = 0;
   while(!isEmptyArray(\@parsed_mcfg_items)) {
      $loop++;
      foreach $mcfg_item (@parsed_mcfg_items) {
         my $item_id = GetItemIdentifier($mcfg_item);

         if($item_id eq "trl") 
         {
           $length_bytes += $mcfg_item->{DataSize};
           $trailerRecord = $mcfg_item;
         }
         # Only include unique NV or EFS listings
         else 
         {
           my @items_found;
           if($mcfg_item->{ItemType} eq 'efs_dir') 
           {
             # assign list of EFS files found in the directory
             @items_found = ( @{ $mcfg_item->{Elements} } );
           }
           else {
             # assign just the single EFS file, but in list form
             @items_found = ($mcfg_item);
           }

           foreach my $item (@items_found) 
           {
             if( IsItemUnique($item) ) 
             {
               $length_bytes += $item->{DataSize};

                # if an invalid type is specified for an nv item it can result in
                # an empty item structure. it should only be read in if DataSize is
                # not zero
                if( ($mcfg_item->{DataSize} != 0) || ItemIsDeleteOrSWOnly($mcfg_item->{Attributes}) )
                {
                  $num_mcfg_items++;
                  push(@$mcfg_items, $item);

                  TraceMcfgList($length_bytes, $num_mcfg_items, $loop);
                }
             }
             else 
             { # Item has already been collected, and is not unique
                if ($item->{ItemType} =~ /efs/) {
                  printf "Skipping duplicate entry with filepath:\t'%s'\n", $item->{Filepath};
                }
                else {
                  printf "Skipping duplicate entry with ID:\t'%s'\n", $item->{ID};
                }
             }
           }
         }
      }

     @parsed_mcfg_items = ExtractNextMcfgItem($rows_ref, $flatten_groups);
   }

   my $padding = 4 - ($length_bytes % 4);
   $length_bytes += $padding;

   open(OUT_FILE, ">", $mcfg_output_file_name) or return ();
   select(OUT_FILE);

#   print "size used in mcfg header: length bytes = $length_bytes\n";
   WriteMcfgHeader(  $mcfg_output_file_name, $num_mcfg_items, $length_bytes,
                     $mcfg_sheet_data->{"Carrier Index"}, int( $mcfg_sheet_data->{"Configuration Type"}),
                     $mcfg_input_file_name
                  );
   WriteMcfgVersion($mcfg_output_file_name, hex($mcfg_sheet_data->{"Full MCFG Version"}));

   while(my $next_item = shift(@$mcfg_items)) {
     WriteMcfgItem($next_item);
   }

   if(%$trailerRecord) {
     WriteTrailerRecord($trailerRecord, $padding);
   }

   WriteMcfgFooter();

   select(STDOUT);
   close(OUT_FILE);

   return ($length_bytes, $num_mcfg_items);
}

sub GenerateMcfgDefaultSourceFile($$$$$) {
   my ($rows_ref, $mcfg_sheet_data, $mcfg_input_file_name, $mcfg_output_file_name, $build_root) = @_;
   if($build_root) {
     $BUILD_ROOT = FixBuildRootString($build_root); #set the global build root
   }
   
   my ($length_bytes, $num_mcfg_items) = (24, 1);

   my $flatten_groups = 1;
   my @parsed_mcfg_items = ExtractNextMcfgItem($rows_ref, $flatten_groups);
   my $mcfg_items = [];
   my $trailerRecord = {};

   # This makes sure has hash of unique items is empty before processing carrier NV items
   # Otherwise, items from prior carrier configs will clobber this hash's key space
   ResetUniqueItemList();

   my $count = 1;
   while(!isEmptyArray(\@parsed_mcfg_items)) {
      for $mcfg_item (@parsed_mcfg_items) {
         my $item_id = GetItemIdentifier($mcfg_item);

         if($mcfg_item->{ItemType} eq "trl") {
           $length_bytes += $mcfg_item->{DataSize};
           $trailerRecord = $mcfg_item;
         } 
         # Only include unique NV or EFS listings
         elsif (IsItemUnique($mcfg_item)) {
           if( ItemIsIndexed($mcfg_item->{Attributes}) )
           {
             print "Item ID = $mcfg_item->{ID}\tDataSize = $mcfg_item->{DataSize}\n";
             $length_bytes += 1;
             #$length_bytes += 1 if ItemIsSubscriber($item->{Attributes});
           }
           else 
           {
             $length_bytes += $mcfg_item->{DataSize};
           }

           # if an invalid type is specified for an nv item it can result in
           # an empty item structure. it should only be read in if DataSize is
           # not zero. Exception: 2 instances in which DataSize will be 0 are when the item
           # is either mar ked for deletion or is a Software Only item
           if( ($mcfg_item->{DataSize} != 0) || ItemIsDeleteOrSWOnly($mcfg_item->{Attributes}) )
           {
             $num_mcfg_items++;
             push(@$mcfg_items, $mcfg_item);
           }
         }
         else { # Item has already been collected, and is not unique
           if ($mcfg_item->{ItemType} =~ /efs/) {
             printf "Skipping duplicate entry with filepath:\t'%s'\n", $mcfg_item->{Filepath};
           }
           else {
             printf "Skipping duplicate entry with ID:\t'%s'\n", $mcfg_item->{ID};
           }
         } 
      }
      @parsed_mcfg_items = ExtractNextMcfgItem($rows_ref, flatten_groups);
      $count = $count + 1;
   }

   my $padding = 4 - ($length_bytes % 4);
   $length_bytes += $padding;

   open(OUT_FILE, ">", $mcfg_output_file_name) or return ();
   select(OUT_FILE);

   WriteMcfgDefaultHeader($mcfg_output_file_name, $num_mcfg_items, $length_bytes, $mcfg_sheet_data->{"Carrier Index"}, int($mcfg_sheet_data->{"Configuration Type"}), $mcfg_input_file_name);
   WriteMcfgVersion($mcfg_output_file_name, hex($mcfg_sheet_data->{"Full MCFG Version"}));

   while(my $next_item = shift(@$mcfg_items)) {
     WriteMcfgItem($next_item);
   }

   if(%$trailerRecord) {
     WriteTrailerRecord($trailerRecord, $padding);
   }

   WriteMcfgFooter();

   select(STDOUT);
   close(OUT_FILE);

   return ($length_bytes, $num_mcfg_items);
}


# Deprecated - call GenerateMcfgSegmentFileXML($xml, $mcfg_input_file_name, $mcfg_output_file_name, $build_root, $output_type=4)
sub GenerateMcfgMultiMBNSourceFileXML($$$) {
   my ($xml, $mcfg_output_file_name, $build_root) = @_;
   #GenerateMcfgSegmentFileXML($xml, $mcfg_input_file_name, $mcfg_output_file_name, $build_root, $output_type=4)
   my $parser = new XML::Parser(Style => 'Tree');
   my $xmlParseTree = $parser->parse($xml);

   $xmlParseTree = XmlStripBlanks($xmlParseTree);
   shift(@{$xmlParseTree->[1]}); # remove top level attributes hash
   $xmlParseTree = $xmlParseTree->[1];

   if($build_root) {
     $BUILD_ROOT = FixBuildRootString($build_root); #set the global build root
   }
   
   my ($length_bytes, $num_mcfg_items) = (24, 1);

   my $mcfg_sheet_data = ExtractCarrierData($xmlParseTree);
   my $mcfg_item = ExtractNextMcfgItemXML($xmlParseTree);
   my $mcfg_items = [];
   my $trailerRecord = {};

   # This makes sure has hash of unique items is empty before processing carrier NV items
   # Otherwise, items from prior carrier configs will clobber this hash's key space
   ResetUniqueItemList();

   while(%$mcfg_item) {
     my $item_id = GetItemIdentifier($mcfg_item);

     if($mcfg_item->{ItemType} eq "trl") {
       $length_bytes += $mcfg_item->{DataSize};
       $trailerRecord = $mcfg_item;
     }
     # Only include unique NV or EFS listings
     elsif (IsItemUnique($mcfg_item)) {
       if( ItemIsIndexed($item->{Attributes}) )
       {
         print "Item ID = $mcfg_item->{ID}\tDataSize = $mcfg_item->{DataSize}\n";
         $length_bytes += 1;
       }
       else 
       {
         $length_bytes += $mcfg_item->{DataSize};
       }

       # if an invalid type is specified for an nv item it can result in
       # an empty item structure. it should only be read in if DataSize is
       # not zero. Exception: 2 instances in which DataSize will be 0 are when the item
       # is either marked for deletion or is a Software Only item
       if( ($mcfg_item->{DataSize} != 0) || ItemIsDeleteOrSWOnly($mcfg_item->{Attributes}) )
       {
         $num_mcfg_items++;
         push(@$mcfg_items, $mcfg_item);
       }
     }
     else { # Item has already been collected, and is not unique
       if ($mcfg_item->{ItemType} =~ /efs/) {
         printf "Skipping duplicate entry with filepath:\t'%s'\n", $mcfg_item->{Filepath};
       }
       else {
         printf "Skipping duplicate entry with ID:\t'%s'\n", $mcfg_item->{ID};
       }
     }

     $mcfg_item = ExtractNextMcfgItemXML($xmlParseTree);
   }

   my $padding = 4 - ($length_bytes % 4);
   $length_bytes += $padding;

   open(OUT_FILE, ">", $mcfg_output_file_name) or return ();
   select(OUT_FILE);

   WriteMcfgMultiMBNHeader($mcfg_output_file_name, $num_mcfg_items, $length_bytes, $mcfg_sheet_data->{"Carrier Index"}, int($mcfg_sheet_data->{"Configuration Type"}));
   WriteMcfgVersion($mcfg_output_file_name, hex($mcfg_sheet_data->{"Full MCFG Version"}));

   while(my $next_item = shift(@$mcfg_items)) {
     WriteMcfgItem($next_item);
   }

   if(%$trailerRecord) {
     WriteTrailerRecord($trailerRecord, $padding);
   }

   WriteMcfgFooter();

   select(STDOUT);
   close(OUT_FILE);

   return ($length_bytes, $num_mcfg_items);
}

