#####################################################################
#
# Modules
# 
#####################################################################  
use fdumpParser;
use File::Basename;
use Getopt::Std;
#use strict;


#####################################################################
#
# Global variables
# 
#####################################################################
##
# Command line parameters
#
my $InputFileName;
my $OutputMetadataFileName;
my $OutputPackUnPackCFileFileName;

##
# Profiling
#
my $profiling_mode                  = 0;        # profiling does NOT allow cmd line parameters, read from file
my %profiling_cmd_param             = (
  InputFileName                   => "temp.cpp.001t.tu",                                        # -d option
  OutputMetadataFileName          => "APSSubsystem68MetaData_New.pm",                           # -m option
  OutputPackUnPackCFileFileName   => "lte_plt_autogen_pack_unpack.foo",                         # -c option
  umidFiles                       => "lte.umid lte_ml1_l1_common.umid fw_lte.umid lte_plt.umid",# -u option
);
                                 
##
# Data structures
#
my $enumMemberList;
my $recordTypeList;
my $recordList;
my $package                         =   "APSSubSystem68Metadata";
my %enum_hash                       =   ();                             # A single has which has all the name,value of constants, used by print_api function
my %umid_hash;                                                          # Hash which has information about all UMID names and their corresponding structures
my $random_number                   =   0;                              # Random number generated to validate metadata
my @globalDependencylist            =   ();                             # Global list which is used in the recursive function which builds dependency list
my %recordNoted                     =   ();

##
# File handles for output
#
my $METADATA;
my $CFILE;
my @XML_FILES;                                                          # List of XML files which contains the UMID information

#####################################################################
#
# Subroutines
# 
#####################################################################
#####################################################################
#
# commandLineProcessing
#     - Read params from profiling_cmd_param because command line 
#       options are not allowed when profiling
#
#####################################################################
sub commandLineFixed
{
    # Check if we missed any fields
    foreach my $key ( keys %profiling_cmd_param)
    {
        if($profiling_cmd_param{$key} eq "") 
        {
            print("\n Missing args $key");
            exit 0;
        }
    }

    # Pack them into where they should go
    $InputFileName                  = $profiling_cmd_param{"InputFileName"};
    $OutputMetadataFileName         = $profiling_cmd_param{"OutputMetadataFileName"};
    $OutputPackUnPackCFileFileName  = $profiling_cmd_param{"OutputPackUnPackCFileFileName"};
    my @xml_file_names              = split(/ /, $profiling_cmd_param{"umidFiles"});

    # Open output files
    open($METADATA, ">$OutputMetadataFileName") || die "Cannot open $OutputMetadataFileName for write\n";
    open($CFILE, ">$OutputPackUnPackCFileFileName") || die "Cannot open $OutputPackUnPackCFileFileName for write\n";

    # Open input files
    foreach my $xml_file (@xml_file_names)
    {
        local *XML_FILE;
        open (XML_FILE, "<$xml_file") || die "Cannot open $file for reading";
        
        push ( @XML_FILES, *XML_FILE);       
    }
}

#####################################################################
#
# commandLineProcessing
#     - Read and parse comamand line options
#     - Open necessary files for read/write
#
#####################################################################
sub commandLineProcessing
{
    my $argCount    =   $#ARGV + 1;
    my %args;

    if($argCount < 9)
    {
        if($argCount == 1)
        {
            getopts('h', \%args);
            
            if($args{h} == "")
            {
                print "\nperl C_Parser.pl -d <<dump.tu>> -m <<APSSubsystem68MetaData_New.pm>> -c <<lte_plt_autogen_pack_unpack.foo>> -u <<UmidFile1>> <<UmidFile2>> <<UmidFile3>> <<UmidFile4>>\n";
                exit 0;
            }
        }
        else
        {
            print "\nIncorrect commandline parameters\nperl C_Parser.pl -d <<dump.tu>> -m <<APSSubsystem68MetaData_New.pm>> -c <<lte_plt_autogen_pack_unpack.foo>>\n";
            exit 0;;
        }
    }

    getopts('d:m:c:u', \%args);
    $InputFileName                  =   $args{d};
    $OutputMetadataFileName         =   $args{m};
    $OutputPackUnPackCFileFileName  =   $args{c};
    
    print "\nCommand line arguments:\n\nTranslation_dump InputFile\t:",$InputFileName,"\nMetadata OutputFile\t\t:",$OutputMetadataFileName,"\nC OutputFile\t\t\t:",$OutputPackUnPackCFileFileName,"\n";

    # Open the files for writing
    open($METADATA, ">$OutputMetadataFileName") || die "Cannot open $OutputMetadataFileName for write\n";
    open($CFILE, ">$OutputPackUnPackCFileFileName") || die "Cannot open $OutputPackUnPackCFileFileName for write\n";
    
    # There must be at least two headers for this to work properly.
    print "XML Files\t\t\t:@ARGV\n\n";

    foreach my $file (@ARGV)
    {
        local *XML_FILE;
        open (XML_FILE, "<$file") || die "Cannot open $file for reading";
        
        push ( @XML_FILES, *XML_FILE);
    }
}

# Consume a line and extract all the field values
sub getUmidFields
{
    my $line            =   shift;
    my @lineContents    =   ();
    my @temp            =   ();
    my %local_fieldHash =   ();
    my $index;
    my $fieldName;
    my $fieldValue;
    
    # First split on space
    @lineContents      =   split(" ", $line);
    
    #If the first element doesn't contain the name "umids" discard the line and return
    if($lineContents[0] !~ /umids/)
    {
        return \%local_fieldHash;
    }
    
    for($index=1 ; $index<@lineContents ; $index++)
    {
        # From the 2nd member of the array split on "=" and get name field and value field
        @temp                           =   split("=",$lineContents[$index]);
                        
        $fieldName                      =   $temp[0];
        $fieldValue                     =   $temp[1];
        
        # fieldValue will have " or ( or ) or * characters, remove them before storing it
        $fieldValue                     =~  s/"|\(|\)|\*//g;
        
        $local_fieldHash{$fieldName}    =   $fieldValue;
    }
    
    return \%local_fieldHash;
}

# This function parses the UMID XML files and stores all UMIDs into umid_hash
sub parse_Umid_XML_Files
{
    my $xml_file_handle     =   shift;
    my $hash_ref            =   shift;
    my %local_umid_hash     =   %{$hash_ref};
    my $line                =   "";
    my %fieldHash           =   ();
    my $buffer              =   "";
   
   for (;$line = <$xml_file_handle>;) 
   {
        # Skip the first and the last lines which have <opt> and </opt> tags
        if($line =~ m/<opt>/ or $line =~ m/<\/opt>/)
        {
            next;
        }
        
        # If the line has both "<" and "/>" then the information is present in a single line
        # Send it to getUmidFiels and extract all the fields
        if($line =~ m/</ and $line =~ m/\/>/)
        {
            $hash_ref                                       =   getUmidFields($line);
            %fieldHash                                      =   %{$hash_ref};
            
            # Process all the interesting fields
            if($fieldHash{struct} ne "" and $fieldHash{sym} ne "")
            {
                # Values are not empty, now check if they are of our interest
                if($fieldHash{struct} =~ /(lte_LL1|lte_cphy|lte_cphy_test|lte_plt|hdrsrch|qtf_run)/)
                {
                    $local_umid_hash{$fieldHash{struct}}    =    $fieldHash{sym};
                }
            }
            
            # Clear the hash
            %fieldHash                                      =   ();
            
            next;
        }
        
        # If the line has only "<", store the line
        if($line =~ m/</ and $line !~ m/\/>/)
        {
            chomp($line);
            $buffer         =   $line;
        }
        
        # If the line has only "/>", append the line to the buffer and process it
        if($line !~ m/</ and $line =~ m/\/>/)
        {
            chomp($line);
            $buffer         .=  $line;

            %fieldHash      =   getUmidFields($buffer);
            
            # Process all the interesting fields
            if($fieldHash{struct} ne "" and $fieldHash{sym} ne "")
            {
                # Values are not empty, now check if they are of our interest
                if($fieldHash{struct} =~ /(lte_LL1|lte_cphy|lte_cphy_test|lte_plt|hdrsrch|qtf_run)/)
                {
                    $local_umid_hash{$fieldHash{struct}}    =    $fieldHash{sym};
                }
            }

            # Clear the hash
            %fieldHash                                      =   ();
            
            # Clear the buffer
            $buffer                                         =   "";
        }
   }
   
   # Return the hash reference
   return \%local_umid_hash;
}

# This function generates a random number and stores it in $random_number
sub generate_random_number 
{
   my $range = 0x7FFFFFFF;

   $random_number = int(rand($range));
}

sub printAllRecords
{
    my $print_rsp    =   shift;
    my $msg_name;
    my %data_struct     =   %{$recordList};
    
    foreach my $record (keys %umid_hash)
    {
        # Print only if the umid structure is in our database
        if(exists($data_struct{$record}))
    {
        $umidName   =   $umid_hash{$record};

        $msg_name   =   $umidName;

        # Convert all upper case to lower case
        $msg_name   =~ tr/[A-Z]/[a-z]/;

        print $METADATA "
     [\'$msg_name\', SUBREC, \'$record\', 1, 0, 0, \'expr \$umid == $enum_hash{$umidName}']," if !$print_rsp;

        print $METADATA "
     [\'$msg_name\', SUBREC, \'$record\', 1, 0, 0, \'expr \$DIAG_RSP_STATUS == 0 \&\& \$umid == $enum_hash{$umidName}']," if $print_rsp;
    }
}
}

sub dumpEnumerators
{
    my $key;
    my $value;
    my $entry;
    my %localHash   =   %{$enumMemberList};
    
    ################# DEBUG Routine #######################
    # Sample enum dump
    #open(my $fd,">enumDump.txt");
    # 
    #foreach $entry (keys %localHash)
    #{
    #    while(($key,$value) = each(%{$localHash{$entry}}))
    #    {
    #        print $fd $keys," ",$value,"\n";
    #    }
    #}
    
    #close($fd);
    #######################################################
    
    # Write the header
    print $METADATA "
#===========================================================================
#  APS Metadata record definitions template file for sub system 68 APPS.
#
#  VERSION  DATE                WHO             DESCRIPTION
#  1.0.000  August 2nd 2013     autogen.pl      This is an autogenerated file
#
# Contact Nirmal Shekar for any query
#===========================================================================
#===========================================================================
package $package;
use MetadataConstants;
use strict;
use warnings;
no  warnings qw(once);

use base 'Exporter';
our \$LTE_CONST;
our \@EXPORT = qw(\$LTE_CONST);
";
    # Dump LTE constants
    print $METADATA "our %LTE_CONSTANTS = (";
    foreach $entry (keys %localHash)
    {
        while(($key,$value) = each(%{$localHash{$entry}}))
        {
            print $METADATA "\n  $key => $value,";
        }
    }
    print $METADATA "\n  LTE_PLT_COOKIE => $random_number";
    print $METADATA "\n);#end of metadata constants
\$LTE_CONST = \\\%LTE_CONSTANTS;
my \$umid = 0;

# ALL THE LOG PACKETS ARE ADDED IN THIS ARRAY.
# FOLLOW THE METADATA SPECIFICATION TO DEFINE THESE
# LOG PACKETS, SUBRECORDS etc.
our \@LOG_68 =
(
[
   [\'LOG_LTE_PLT\'],
],
);

#######################################
# add request responses here
#######################################
our \@CMD_68 =
(
[
   [\'CMD_LTE_PLT\'],
],
#======================================LTE_PLT api/rsp=================================
";
   #Dump Custom Handling Structures
   print $METADATA "
";
}

# Get the size of the union
# We can't directly use the size of a union because compiler automatically pads to bit boundary
# Get the size of each individual member and select the highest size and return
sub getUnionSize
{
    my $union                           =   shift;
    my $maxSize                         =   0;
    my $size                            =   0;
    my %data_struct                     =   %{$recordList};
    
    foreach my $unionMember (@{$data_struct{$union->{size}}})
    {
        my $totalSize       =   0;
        
        $size               =   getSizeOfMember($unionMember);
        
        $totalSize          =   $size * $unionMember->{array_length};
        
        if($totalSize > $maxSize)
        {
            $maxSize                    =   $totalSize;
        }
    }
    
    return $maxSize;
}

# Recursive function which gets the size of a member
sub getSizeOfMember
{
    my $record                          =   shift;
    my $size                            =   0;
    my %data_struct                     =   %{$recordList};
    
    if($record->{memberType} eq "enum")
    {
        $size                           +=  $record->{size};
    }
    elsif($record->{memberType} eq "union")
    {
        $size                           +=  getUnionSize($record);
    }
    elsif($record->{memberType} eq "struct")    # struct, we need to recursively go through all the elements
    {
        foreach my $subrecord (@{$data_struct{$record->{size}}})
        {
            my $single_element_size;
            
            $single_element_size        =   getSizeOfMember($subrecord);
            
            # Now we have the size of a single element
            # If this was a primitive type, the returned size considers the array size also
            # If this is a struct or a union we only get the primitive size and we need to consider the array size
            
            if($subrecord->{memberType} eq "struct" or $subrecord->{memberType} eq "union")
            {
                
                if($subrecord->{arrayDimension} == 2)
                {
                    # Check if its a 1-D array or 2-D array or a simple variable
                    # array_length for 2-D arrays will be of the form "x,y" 
                    # where is x is the inner dimension and y is the outer dimension
                
                    my $length1;
                    my $length2;
                    my $finalLength;
                    my @temp;
                    
                    # Here we need to convert a 2-D array into 1-D array to compute the size
                    # array_length for 2-D arrays will be of the form "x,y" where is x is the inner dimension and y is the outer dimension
                    @temp   =   split(",", $subrecord->{array_length});
                    
                    $finalLength                =   $temp[0] * $temp[1];
                    
                    $size                       +=   $single_element_size * $finalLength;
                }
                elsif($subrecord->{arrayDimension} == 1)
                {
                    # 1-D array
                    $size                       +=  $single_element_size * $subrecord->{array_length};
                }
                else
                {
                    # Primitive size - Array size taken care in the routine getSizeOfMember
                    $size                       +=  $single_element_size;
                }
            }
            else
            {
                # Primitive size - Array size taken care in the routine getSizeOfMember
                $size                       +=  $single_element_size;
            }
        }
    }
    else
    {
        if($record->{arrayDimension} == 2)
        {
            # Check if its a 1-D array or 2-D array or a simple variable
            # array_length for 2-D arrays will be of the form "x,y" 
            # where is x is the inner dimension and y is the outer dimension
        
            my $length1;
            my $length2;
            my $finalLength;
            my @temp;
            
            # Here we need to convert a 2-D array into 1-D array to compute the size
            # array_length for 2-D arrays will be of the form "x,y" where is x is the inner dimension and y is the outer dimension
            @temp   =   split(",", $record->{array_length});
            
            $finalLength                    =   $temp[0] * $temp[1];
            
            $size                           +=   $finalLength * $record->{size};
        }
        else
        {
            # 1-D or a simple data type, array_length will have correct value
            # It can also be a array so while returning the size, return (unit_size * array_size)
            $size                           +=  ($record->{size} * $record->{array_length});
        }
    }
    
    return $size;
}

sub dumpRecords
{
    my $record;
    my $subrec;
    my $lastFoundEnumerator     =   "";
    my $enum_cntr               =   0;
    my $memberSize              =   0;
    my %hashRef                 =   %{$recordTypeList};
    
    # Instead of using the original hash reference, use the one got form XMLIn
    
    #$xml_read                   =   new XML::Simple;
    #$xml_read                   =   new XML::Simple->new(ForceArray => 1, KeepRoot => 1, XMLDecl => +1);
    
    #my $data                    =   $xml_read->XMLin("C:/Users/nmahabal/LTE.MPSS.2.1.ScheduleAnalyzer/modem_proc/lte/ML1/schdlr/tools/ScheduleAnalyzer/PLT_Parser/xmloutput.xml");
    #my %data_struct             =   %{$data};
    
    my %data_struct             =   %{$recordList};
    
    # Don't loop through all the entries but only the entries in the dependency list
    foreach $record (@globalDependencylist)
    {
        # Dump only structures
        if($hashRef{$record} eq "struct")
    {
        print $METADATA "
[
  [\'$record\'],
  [" ; #end print
  
        foreach $subrec (@{$data_struct{$record}})
        {
            # Some variables don't have identifier names, don't record them - Hack
            if($subrec->{name} eq "")
            {
                next;
            }
            
            if($subrec->{memberType} eq "enum")
            {
                print $METADATA "
    [\'$subrec->{name}\', $subrec->{type}, $subrec->{size}, $subrec->{array_length}, DEPEND],";
     
                $lastFoundEnumerator    =   $subrec->{name};
            }
            elsif($subrec->{memberType} eq "struct")
            {
                # Check if its a 2-D array
                if($subrec->{arrayDimension} == 2)
                {
                    my $length1;
                    my $length2;
                    my @temp;
                    my $finallength;
                    
                    # Here we need to convert a 2-D array into 1-D array to compute the size
                    # array_length for 2-D arrays will be of the form "x,y" where is x is the inner dimension and y is the outer dimension
                    @temp   =   split(",", $subrec->{array_length});
                    
                    $finalLength    =   $temp[0] * $temp[1];
                    
                    print $METADATA "
    [\'$subrec->{name}\', $subrec->{type}, '$subrec->{size}', $finalLength],";
                }
                else
                {
                    # 1-D or simple variable
                    print $METADATA "
    [\'$subrec->{name}\', $subrec->{type}, '$subrec->{size}', $subrec->{array_length}],";
                }
            }
            elsif($subrec->{memberType} eq "union")
            {
                $enum_cntr              =   0;
                my @padList             =   ();
                my $unionSize           =   getUnionSize($subrec);
                
                # Go through each member which will be a structure
                foreach my $union_member (@{$data_struct{$subrec->{size}}})
                {
                    my $totalSize       =   0;
                    
                    print $METADATA "
    [\'$union_member->{name}\', SUBREC, \'$union_member->{size}\', $union_member->{array_length}, 0, 0, \'expr \$$lastFoundEnumerator == $enum_cntr\'],";
    
                    # Get the size of the member
                    # Use a recursive function which will return the size of each member
                    $memberSize         =   getSizeOfMember($union_member);
                    
                    $totalSize          +=  $memberSize * $union_member->{array_length};
                    
                    push(@padList, ($unionSize-$totalSize));
                    
                    $enum_cntr++;
                }
                
                # Reset the counter
                $enum_cntr              =   0;
                
                # We have expanded all the members of the union
                # Now we have to add pad corresponding to union member if required
                foreach my $paddingBytes (@padList)
                {
                    if($paddingBytes > 0)
                    {
                        print $METADATA "
    [\'pad\', BYTE, 1, $paddingBytes, 0, 0, \'expr \$$lastFoundEnumerator == $enum_cntr\'],";
                    }
    
                    $enum_cntr++;
                }
            }
            elsif($subrec->{memberType} eq "bitfield")
            {
                # In future we can remove this, we already have the correct size of the bitfield in {size} field of the record
                # We can use {size} field in the future
    
                print $METADATA "
    [\'$subrec->{name}\', $subrec->{type}, 4, $subrec->{array_length}],";
            }
            elsif($subrec->{memberType} eq "pointer")
            {
                # A pointer needs 4 bytes
                print $METADATA "
    [\'$subrec->{name}\', $subrec->{type}, 4, $subrec->{array_length}],";
            }
            elsif($subrec->{memberType} eq "default")
            {
                # Check if its a 2-D array
                if($subrec->{arrayDimension} == 2)
                {
                    my $length1;
                    my $length2;
                    my @temp;
                    my $finallength;
                    my $size;
                    
                    # Here we need to convert a 2-D array into 1-D array to compute the size
                    # array_length for 2-D arrays will be of the form "x,y" where is x is the inner dimension and y is the outer dimension
                    @temp   =   split(",", $subrec->{array_length});
                    
                    $finalLength    =   $temp[0] * $temp[1];
                    
                    $size           =   $subrec->{size} * $finalLength;
                    
                    print $METADATA "
    [\'$subrec->{name}\', $subrec->{type}, $subrec->{size}, $finalLength],";
                }
                else
                {
                    # 1-D or simple variable
                    $size           =   $subrec->{size};
                    
                    # Check if the size is > 4 bytes, if yes split the member into 2, _lo and _hi
                    if($size == 8)
                    {
                        $size   /=  2;
                        print $METADATA "
    [\'$subrec->{name}_lo\', $subrec->{type}, $size, $subrec->{array_length}],";
     
                        print $METADATA "
    [\'$subrec->{name}_hi\', $subrec->{type}, $size, $subrec->{array_length}],";
                    }
                    else
                    {
                        print $METADATA "
    [\'$subrec->{name}\', $subrec->{type}, $subrec->{size}, $subrec->{array_length}],";
                    }
                }
            }
        }
        print $METADATA "
  ],
],"; #end print
    }
    }

    # Some custom functions - Hard coded in the script
    print $METADATA "
[
   [\'LTE_PLT_DIAG_CMD_ECHO\'],
   [
     [\'CMD_CODE\', BYTE, 1, 1, HIDDEN|READONLY, 75],
     [\'SUBSYS_ID\', BYTE, 1, 1, HIDDEN|READONLY, 68],
     [\'SUBSYS_CMD_CODE\', BYTE, 2, 1, HIDDEN|READONLY, 0],
     [\'cookie_value\', BYTE, 4, 1, HIDDEN|READONLY, $random_number],
     [\'umid\', BYTE, 4, 1, DEPEND],";#end print

     #print "LTE_PLT_DIAG_CMD_ECHO\n";
   &printAllRecords(0);
   print $METADATA "      
   ],

   [\'LTE_PLT_DIAG_CMD_ECHO Response Message\'],
   [
     [\'CMD_CODE\', BYTE, 1, 1, HIDDEN|READONLY, 75],
     [\'SUBSYS_ID\', BYTE, 1, 1, HIDDEN|READONLY, 68],
     [\'SUBSYS_CMD_CODE\', BYTE, 2, 1, HIDDEN|READONLY, 0],
     [\'cookie_value\', BYTE, 4, 1, HIDDEN|READONLY, $random_number],
     [\'DIAG_RSP_STATUS\', BYTE, 4, 1, DEPEND],
     [\'TIME\', BYTE, 4, 1, 0, 0, \'expr \$DIAG_RSP_STATUS == 0\'],
     [\'umid\', BYTE, 4, 1, DEPEND, 0, \'expr \$DIAG_RSP_STATUS == 0\'],
     [\'SFN\', BYTE, 2, 1, DEPEND, 0, \'expr \$DIAG_RSP_STATUS == 0\'],
     [\'Sub_FN\', BYTE, 2, 1, DEPEND, 0, \'expr \$DIAG_RSP_STATUS == 0\'],";#end print

   &printAllRecords(1);
   print $METADATA "     
   ],
],
[
   [\'LTE_PLT_DIAG_CMD_STORE\'],
   [
     [\'CMD_CODE\', BYTE, 1, 1, HIDDEN|READONLY, 75],
     [\'SUBSYS_ID\', BYTE, 1, 1, HIDDEN|READONLY, 68],
     [\'SUBSYS_CMD_CODE\', BYTE, 2, 1, HIDDEN|READONLY, 1],
     [\'cookie_value\', BYTE, 4, 1, HIDDEN|READONLY, $random_number],
     [\'umid\', BYTE, 4, 1, DEPEND],";#end print

   &printAllRecords(0);
   print $METADATA "     
   ],

   [\'LTE_PLT_DIAG_CMD_STORE Response Message\'],
   [
     [\'CMD_CODE\', BYTE, 1, 1, HIDDEN|READONLY, 75],
     [\'SUBSYS_ID\', BYTE, 1, 1, HIDDEN|READONLY, 68],
     [\'SUBSYS_CMD_CODE\', BYTE, 2, 1, HIDDEN|READONLY, 1],
     [\'cookie_value\', BYTE, 4, 1, HIDDEN|READONLY, $random_number],
     [\'DIAG_RSP_STATUS\',BYTE, 4, 1],
   ],
],
[
   [\'LTE_PLT_DIAG_CMD_RUN\'],
   [
     [\'CMD_CODE\', BYTE, 1, 1, HIDDEN|READONLY, 75],
     [\'SUBSYS_ID\', BYTE, 1, 1, HIDDEN|READONLY, 68],
     [\'SUBSYS_CMD_CODE\', BYTE, 2, 1, HIDDEN|READONLY, 2],
     [\'cookie_value\', BYTE, 4, 1, HIDDEN|READONLY, $random_number],
   ],

   [\'LTE_PLT_DIAG_CMD_RUN Response Message\'],
   [
     [\'CMD_CODE\', BYTE, 1, 1, HIDDEN|READONLY, 75],
     [\'SUBSYS_ID\', BYTE, 1, 1, HIDDEN|READONLY, 68],
     [\'SUBSYS_CMD_CODE\', BYTE, 2, 1, HIDDEN|READONLY, 2],
     [\'cookie_value\', BYTE, 4, 1, HIDDEN|READONLY, $random_number],
     [\'DIAG_RSP_STATUS\',BYTE, 4, 1],
   ],
],
[
   [\'LTE_PLT_DIAG_CMD_FETCH\'],
   [
     [\'CMD_CODE\', BYTE, 1, 1, HIDDEN|READONLY, 75],
     [\'SUBSYS_ID\', BYTE, 1, 1, HIDDEN|READONLY, 68],
     [\'SUBSYS_CMD_CODE\', BYTE, 2, 1, HIDDEN|READONLY, 3],
     [\'cookie_value\', BYTE, 4, 1, HIDDEN|READONLY, $random_number],
   ],

   [\'LTE_PLT_DIAG_CMD_FETCH Response Message\'],
   [
     [\'CMD_CODE\', BYTE, 1, 1, HIDDEN, 75],
     [\'SUBSYS_ID\', BYTE, 1, 1, HIDDEN, 68],
     [\'SUBSYS_CMD_CODE\', BYTE, 2, 1, HIDDEN, 3],
     [\'cookie_value\', BYTE, 4, 1, HIDDEN|READONLY, $random_number],
     [\'DIAG_RSP_STATUS\', BYTE, 4, 1, DEPEND],
     [\'time\', BYTE, 4, 1, 0, 0, \'expr \$DIAG_RSP_STATUS == 0\'],
     [\'umid\', BYTE, 4, 1, DEPEND, 0, \'expr \$DIAG_RSP_STATUS == 0\'],
     [\'SFN\', BYTE, 2, 1, DEPEND, 0, \'expr \$DIAG_RSP_STATUS == 0\'],
     [\'Sub_FN\', BYTE, 2, 1, DEPEND, 0, \'expr \$DIAG_RSP_STATUS == 0\'],";#end print

   &printAllRecords(1);
   print $METADATA "     
   ],
],
[
   ['LTE_PLT_DIAG_CMD_VERSION'],
   [
     ['CMD_CODE', BYTE, 1, 1, HIDDEN|READONLY, 75],
     ['SUBSYS_ID', BYTE, 1, 1, HIDDEN|READONLY, 68],
     ['SUBSYS_CMD_CODE', BYTE, 2, 1, HIDDEN|READONLY, 4],
   ],

   ['LTE_PLT_DIAG_CMD_VERSION Response Message'],
   [
     ['CMD_CODE', BYTE, 1, 1, HIDDEN, 75],
     ['SUBSYS_ID', BYTE, 1, 1, HIDDEN, 68],
     ['SUBSYS_CMD_CODE', BYTE, 2, 1, HIDDEN, 4],
     ['cookie_value', BYTE, 4, 1, HIDDEN|READONLY, $random_number],
     ['diag_rsp_status', BYTE, 4, 1, DEPEND], 
     ['version', BYTE, 2, 1, DEPEND],     
     ['plt_enabled_flag', BYTE, 2, 1, DEPEND],     
   ],
],
[
   ['LTE_PLT_DIAG_CMD_EFS_STORE'],
   [
     ['CMD_CODE', BYTE, 1, 1, HIDDEN|READONLY, 75],
     ['SUBSYS_ID', BYTE, 1, 1, HIDDEN|READONLY, 68],
     ['SUBSYS_CMD_CODE', BYTE, 2, 1, HIDDEN|READONLY, 6],
   ],

   ['LTE_PLT_DIAG_CMD_EFS_STORE Response Message'],
   [
     ['CMD_CODE', BYTE, 1, 1, HIDDEN, 75],
     ['SUBSYS_ID', BYTE, 1, 1, HIDDEN, 68],
     ['SUBSYS_CMD_CODE', BYTE, 2, 1, HIDDEN, 6],
     ['cookie_value', BYTE, 4, 1, HIDDEN|READONLY, $random_number],
     ['diag_rsp_status', BYTE, 4, 1, DEPEND], 
   ],
],
);

1;

"; #end print

}

# Routine which builds a single list enum hash which will be used by printAllRecords
# Prepare enum_hash which will form a flat enum hash from the array of enum hashes
    # $enumMemberList will have the below format
    # {
    #       enum1Member1=Value, enum1Member2=Value, enum1Member3=Value......
    #       enum2Member1=Value, enum2Member2=Value, enum2Member3=Value......
    #       .
    #       .
    # }
    # The below function will transform into a single list like below
    # {
    #       enum1Member1=Value
    #       enum1Member2=Value
    #       enum1Member3=Value
    #       .
    #       .
    #       .
    #       enum2Member1=Value
    #       enum2Member2=Value
    #       enum2Member3=Value
    # }
    # It is easier dumping the values like this
sub prepareEnumHash
{
    my $key;
    my $value;
    my $entry;
    my $subEntry;
    my %localHash   =   %{$enumMemberList};
    
    foreach $entry (keys %localHash)
    {
        while(($key,$value) = each(%{$localHash{$entry}}))
        {
            $enum_hash{$key}     =       $value;
        }
    }
}

# Function to dump the packed structure given a structure name
sub dumpStructure
{
    my $record          =   shift;
    my $pack			=   shift;
    my %data_struct     =   %{$recordList};
    my %hashRef         =   %{$recordTypeList};     # Hash reference to record types
    
    if($pack == 1)
    {
        print $CFILE "\ntypedef PACK($hashRef{$record})\n{\n";
    }
    else
    {
         print $CFILE "\ntypedef $hashRef{$record}\n{\n";
    }
    
    foreach $subrecord (@{$data_struct{$record}})
    {
        # Some variables don't have identifier names, don't record them - Hack
        if($subrecord->{name} eq "")
        {
            next;
        }
            
        # If the member is a primitive type then use the normal name
        # Else it should start with "packed_" as we are defining
        # all non-primitive types as packed structures
        if($subrecord->{memberType} eq "struct" or $subrecord->{memberType} eq "union")
        {
            # Check the array dimension
            if($subrecord->{arrayDimension} == 2)
            {
                # 2-D array
                my $length1;
                my $length2;
                my $finalLength;
                my @temp;
                
                # Here we need to convert a 2-D array into 1-D array to compute the size
                # array_length for 2-D arrays will be of the form "x,y" where is x is the inner dimension and y is the outer dimension
                @temp   =   split(",", $subrecord->{array_length});
                
                # Now we have both the dimension of the 2-D array
                print $CFILE "  packed_$subrecord->{displayTypeName} $subrecord->{name}\[$temp[0]\]\[$temp[1]\];\n";
            }
            elsif($subrecord->{arrayDimension} == 1)
            {
                # 1-D array
                print $CFILE "  packed_$subrecord->{displayTypeName} $subrecord->{name}\[$subrecord->{array_length}\];\n";
            }
            else
            {
                # Simple variable
                print $CFILE "  packed_$subrecord->{displayTypeName} $subrecord->{name};\n";
            }
        }
        elsif($subrecord->{memberType} eq "pointer")
        {
            # If function pointer - Don't record it, skip the entry
            if($subrecord->{displayTypeName} eq "function")
            {
                next;
            }
            
            # Handle pointer type
            # It might point to struct type - Use packed_ prefix for name
            # Else not required
            if($subrecord->{type} eq "SUBREC")
            {
                print $CFILE "  packed_$subrecord->{displayTypeName} *$subrecord->{name};\n";
            }
            else
            {
                print $CFILE "  $subrecord->{displayTypeName} *$subrecord->{name};\n";
            }
        }
        else
        {
            # Primitive type
            
            # Check the array dimension
            if($subrecord->{arrayDimension} == 2)
            {
                # 2-D array
                my $length1;
                my $length2;
                my $finalLength;
                my @temp;
                
                # Here we need to convert a 2-D array into 1-D array to compute the size
                # array_length for 2-D arrays will be of the form "x,y" where is x is the inner dimension and y is the outer dimension
                @temp   =   split(",", $subrecord->{array_length});
                
                # Now we have both the dimension of the 2-D array
                print $CFILE "  $subrecord->{displayTypeName} $subrecord->{name}\[$temp[0]\]\[$temp[1]\];\n";
            }
            elsif($subrecord->{arrayDimension} == 1)
            {
                # 1-D array
                print $CFILE "  $subrecord->{displayTypeName} $subrecord->{name}\[$subrecord->{array_length}\];\n";
            }
            else
            {
                # Simple variable
                print $CFILE "  $subrecord->{displayTypeName} $subrecord->{name};\n";
            }
        }
    }
    
	if($pack == 1)
	{
           print $CFILE "}packed_$record;\n";
        }
        else
        {
            print $CFILE "}$record;\n";
        }
}

# Helper functions
# This function returns 1 if the current structure has a union/struct member or else 0
sub isStructUnionPresent
{
    my $record          =   shift;
    my %data_struct     =   %{$recordList};
    
    foreach my $subrec (@{$data_struct{$record}})
    {
        if($subrec->{memberType} eq "struct" or $subrec->{memberType} eq "union")
        {
            return 1;
        }
    }
    
    return 0;
}

# This function returns 0 if there are no members which have arrays
# Returns 1 if there is a 1-D array present
# Returns 2 if there is a 2-D array present
sub isArrayPresent
{
    my $record          =   shift;
    my %data_struct     =   %{$recordList};
    my $dimension       =   0;
    
    foreach my $subrec (@{$data_struct{$record}})
    {
        # 2-D array, for this type array_length will be of the type "x,y" where
        # x is the first dimension and y is the second dimension
        if($subrec->{arrayDimension} == 2)
        {
            # Once we have found a 2-D array we can safely return the value of 2
            # as this will be the highest value which will be returned
            return 2;
        }
        
        # 1-D array
        if($subrec->{arrayDimension} == 1)
        {
            $dimension =  1;
        }
    }
    
    return $dimension;
}

# Dumping the Unpack routine
sub dumpUnpackRoutine
{
    my $record          =   shift;
    my %data_struct     =   %{$recordList};
    my %hashRef         =   %{$recordTypeList};     # Hash reference to record types
    my $dimension;
    my $structPresent;
    my $lastEnumName;
    my $lastEnumType;
    my $structIndex     =   0;
    my $enumIndex       =   0;
    my %localEnumHash   =   ();
    
    print $CFILE "\nstatic lte_plt_autogen_result_enum_type unpack_$record(uint32 packed_msg_len, void* packed_msg_ptr, uint32 *unpacked_msg_len, void* unpacked_msg_ptr)\n{";
    
    # Check if a struct or a union is present we need 3 additional local variables
    if(isStructUnionPresent($record) == 1)
    {
        print $CFILE "
  uint32 pass_pack_msg_len;
  uint32 ret_unpack_msg_len;
  lte_plt_autogen_result_enum_type status;
     ";
    }
    
    # Check if an array is present, if we have a 1-D array we need one variable for the loop
    # If we have a 2-D array we need 2 variables
    $dimension  =   isArrayPresent($record);
    
    if($dimension == 1)
    {
        print $CFILE "
  uint32 index0;
        ";
    }
    
    if($dimension == 2)
    {
        print $CFILE "
  uint32 index0;
  uint32 index1;
        ";
    }
    
    # Declare the unpacked and packed message pointers
    print $CFILE "
  $record *unpacked_msg;
  packed_$record *packed_msg;
    ";
    
    # Typecast and store the input message pointers
    print $CFILE "
  unpacked_msg = ($record *) unpacked_msg_ptr;
  packed_msg = (packed_$record *) packed_msg_ptr;
    ";
    
    # Check the message size with the input paramter
    print $CFILE "
  if(packed_msg_len != sizeof(packed_$record))
  {
    return AUTOGEN_RESULT_BAD_MSG_LEN;
  }";
    
    # Comment
    print $CFILE "\n\n  /* Assign unpacked structure elements to packed */\n";
    
    # Assign/copy each member of the packed structure to the corresponding unpacked structure
    foreach my $subrec (@{$data_struct{$record}})
    {
        # Some variables don't have identifier names, don't record them - Hack
        if($subrec->{name} eq "")
        {
            next;
        }
        
        if($subrec->{memberType} eq "struct")
        {
            # Non-primitive type - struct
            # Check if its an 1-D array, 2-D array or a simple variable
            if($subrec->{arrayDimension} == 2)
            {
                # 2-D array
                # Extract information about both the dimension of the 2-D array
                my @temp;
                
                # array_length for 2-D arrays will be of the form "x,y" where is x is the inner dimension and y is the outer dimension
                @temp   =   split(",", $subrec->{array_length});
                
                print $CFILE "\n
  for(index0 = 0 ; index0 < $temp[0] ; index0++)
  {
    for(index1 = 0 ; index1 < $temp[1] ; index1++)
    {
      pass_pack_msg_len = sizeof(packed_$subrec->{size});
      status = unpack_$subrec->{size}(pass_pack_msg_len,(void *)&packed_msg->$subrec->{name}\[index0\]\[index1\],&ret_unpack_msg_len,(void *)&unpacked_msg->$subrec->{name}\[index0\]\[index1\]);

      if (status != AUTOGEN_RESULT_SUCCESS)
      {
        return status;
      }
    }
  }";
            }
            elsif($subrec->{arrayDimension} == 1)
            {
                # 1-D array
                print $CFILE "\n
  for(index0 = 0 ; index0 < $subrec->{array_length} ; index0++)
  {
    pass_pack_msg_len = sizeof(packed_$subrec->{size});
    status = unpack_$subrec->{size}(pass_pack_msg_len,(void *)&packed_msg->$subrec->{name}\[index0\],&ret_unpack_msg_len,(void *)&unpacked_msg->$subrec->{name}\[index0\]);

    if (status != AUTOGEN_RESULT_SUCCESS)
    {
      return status;
    }
  }";
            }
            else
            {
                print $CFILE "
  pass_pack_msg_len = sizeof(packed_$subrec->{size});
  status = unpack_$subrec->{size}(pass_pack_msg_len,(void *)&packed_msg->$subrec->{name},&ret_unpack_msg_len,(void *)&unpacked_msg->$subrec->{name});

  if (status != AUTOGEN_RESULT_SUCCESS)
  {
    return status;
  }
  ";
            }
        }
        elsif($subrec->{memberType} eq "union")
        {
            my %enumHash;
            my $memberStructureTypeName;
            my $memberStructureVariableName;
            
            # Non-primitive type - union
            print $CFILE "\n
  switch(packed_msg->$lastEnumName)
  {";
            
            # Loop through all the enumerator values and use the corresponding structure and its unpack function
            $structIndex    =   0;
            
            # Using the enum name, get the index to the list which has information about all enumerator members
            #$enumIndex      =   getEnumIndex($lastEnumType);
            
            #if($enumIndex == -1)
            #{
            #    print "ERROR: enum $lastEnumName not found\n";
            #    die();
            #}
            
            %enumHash       =   %{$enumMemberList};
            
            %localEnumHash  =   %{$enumHash{$lastEnumType}};
            
            #foreach my $enum (sort values %localEnumHash)
            foreach my $enum (sort { ($localEnumHash{$a} cmp $localEnumHash{$b}) } keys %localEnumHash)
            {
                # $data_struct{$subrec->{size}} gives information about the member structure
                # From this point we need to know the structure's type name and variable name
                # This is got using the fields 
                #             a. {size} -> Type name
                #             b. {name} -> Variable name
                # PS: $data_struct{} is an array of entries
                #     Here we use $structIndex to index one of the entries
                #     The assumption is that for every enumType we have a corresponding structure member in the union
                #     Hence a straightforward indexing works.
                $memberStructureTypeName        =   $data_struct{$subrec->{size}}[$structIndex]->{size};
                $memberStructureVariableName    =   $data_struct{$subrec->{size}}[$structIndex]->{name};
                
                if($memberStructureTypeName eq "" or $memberStructureVariableName eq "")
                {
                    # There are no more structures present, exit from the for loop
                    last;
                }
                
                print $CFILE "
    case $enum:
                pass_pack_msg_len = sizeof(packed_$memberStructureTypeName);
                status = unpack_$data_struct{$subrec->{size}}[$structIndex]->{size}(pass_pack_msg_len,(void *)&packed_msg->$subrec->{name}.$memberStructureVariableName,&ret_unpack_msg_len,(void *)&unpacked_msg->$subrec->{name}.$memberStructureVariableName);

                if (status != AUTOGEN_RESULT_SUCCESS)
                {
                  return status;
                }
                
                break;\n";
                
                # We are assuming that for every enumerator value we have a corresponding structure in the enumerator in the same order
                $structIndex++;
            }
  
            # End the switch case with a break statement
            print $CFILE "
    default:
      break;/* Error handling */
  }";
            
        }
        elsif($subrec->{memberType} eq "enum")
        {
            $lastEnumName   =   $subrec->{name};
            $lastEnumType   =   $subrec->{displayTypeName};
            
            # Check if its an array of enumerator
            if($subrec->{arrayDimension} == 1)
            {
                # 1-D array
                print $CFILE "\n
  for(index0 = 0 ; index0 < $subrec->{array_length} ; index0++)
  {
    unpacked_msg->$subrec->{name}\[index0\] = packed_msg->$subrec->{name}\[index0\];  
  }";
            }
            else
            {
                print $CFILE "
  unpacked_msg->$subrec->{name} = packed_msg->$subrec->{name};";
            }
        }
        else
        {
            # Primtive type
            
            # Check if its an 1-D array, 2-D array or a simple variable
            if($subrec->{arrayDimension} == 2)
            {
                # 2-D array
                # Extract information about both the dimension of the 2-D array
                my @temp;
                
                # array_length for 2-D arrays will be of the form "x,y" where is x is the inner dimension and y is the outer dimension
                @temp   =   split(",", $subrec->{array_length});
                
                print $CFILE "\n
  for(index0 = 0 ; index0 < $temp[0] ; index0++)
  {
    for(index1 = 0 ; index1 < $temp[1] ; index1++)
    {
      unpacked_msg->$subrec->{name}\[index0\]\[index1\] = packed_msg->$subrec->{name}\[index0\]\[index1\];
    }
  }";
            }
            elsif($subrec->{arrayDimension} == 1)
            {
                # 1-D array
                print $CFILE "\n
  for(index0 = 0 ; index0 < $subrec->{array_length} ; index0++)
  {
    unpacked_msg->$subrec->{name}\[index0\] = packed_msg->$subrec->{name}\[index0\];  
  }";
            }
            else
            {
                print $CFILE "
  unpacked_msg->$subrec->{name} = packed_msg->$subrec->{name};";
            }
        }
    }
    
    # Assign size of the unpacked structure to the input parameter
    print $CFILE "
  \n  *unpacked_msg_len = sizeof($record);
    ";
    
    # Return success status
    print $CFILE "
  return AUTOGEN_RESULT_SUCCESS;";
    
    print $CFILE "\n}\n";
}

# Dumping the Pack routine
sub dumpPackRoutine
{
    my $record          =   shift;
    my %data_struct     =   %{$recordList};
    my %hashRef         =   %{$recordTypeList};     # Hash reference to record types
    my $dimension;
    my $structPresent;
    my $lastEnumName;
    my $lastEnumType;
    my $structIndex     =   0;
    my $enumIndex       =   0;
    my %localEnumHash   =   ();
    
    print $CFILE "\nstatic lte_plt_autogen_result_enum_type pack_$record(uint32 unpacked_msg_len, void* unpacked_msg_ptr, uint32 *packed_msg_len, void* packed_msg_ptr)\n{";
    
    # Check if a struct or a union is present we need 3 additional local variables
    if(isStructUnionPresent($record) == 1)
    {
        print $CFILE "
  uint32 pass_unpack_msg_len;
  uint32 ret_pack_msg_len;
  lte_plt_autogen_result_enum_type status;
     ";
    }
    
    # Check if an array is present, if we have a 1-D array we need one variable for the loop
    # If we have a 2-D array we need 2 variables
    $dimension  =   isArrayPresent($record);
    
    if($dimension == 1)
    {
        print $CFILE "
  uint32 index0;
        ";
    }
    
    if($dimension == 2)
    {
        print $CFILE "
  uint32 index0;
  uint32 index1;
        ";
    }
    
    # Declare the unpacked and packed message pointers
    print $CFILE "
  $record *unpacked_msg;
  packed_$record *packed_msg;
    ";
    
    # Typecast and store the input message pointers
    print $CFILE "
  unpacked_msg = ($record *) unpacked_msg_ptr;
  packed_msg = (packed_$record *) packed_msg_ptr;
    ";
    
    # Check the message size with the input paramter
    print $CFILE "
  if(unpacked_msg_len != sizeof($record))
  {
    return AUTOGEN_RESULT_BAD_MSG_LEN;
  }";
    
    # Comment
    print $CFILE "\n\n  /* Assign packed structure elements to unpacked */\n";
    
    # Assign/copy each member of the packed structure to the corresponding unpacked structure
    foreach my $subrec (@{$data_struct{$record}})
    {
        # Some variables don't have identifier names, don't record them - Hack
        if($subrec->{name} eq "")
        {
            next;
        }
        
        if($subrec->{memberType} eq "struct")
        {
            # Non-primitive type - struct
            # Check if its an 1-D array, 2-D array or a simple variable
            if($subrec->{arrayDimension} == 2)
            {
                # 2-D array
                # Extract information about both the dimension of the 2-D array
                my @temp;
                
                # array_length for 2-D arrays will be of the form "x,y" where is x is the inner dimension and y is the outer dimension
                @temp   =   split(",", $subrec->{array_length});
                
                print $CFILE "\n
  for(index0 = 0 ; index0 < $temp[0] ; index0++)
  {
    for(index1 = 0 ; index1 < $temp[1] ; index1++)
    {
      pass_unpack_msg_len = sizeof($subrec->{size});
      status = pack_$subrec->{size}(pass_unpack_msg_len,(void *)&unpacked_msg->$subrec->{name}\[index0\]\[index1\],&ret_pack_msg_len,(void *)&packed_msg->$subrec->{name}\[index0\]\[index1\]);

      if (status != AUTOGEN_RESULT_SUCCESS)
      {
        return status;
      }
    }
  }";
            }
            elsif($subrec->{arrayDimension} == 1)
            {
                # 1-D array
                print $CFILE "\n
  for(index0 = 0 ; index0 < $subrec->{array_length} ; index0++)
  {
    pass_unpack_msg_len = sizeof($subrec->{size});
    status = pack_$subrec->{size}(pass_unpack_msg_len,(void *)&unpacked_msg->$subrec->{name}\[index0\],&ret_pack_msg_len,(void *)&packed_msg->$subrec->{name}\[index0\]);

    if (status != AUTOGEN_RESULT_SUCCESS)
    {
      return status;
    }
  }";
            }
            else
            {
                print $CFILE "
  pass_unpack_msg_len = sizeof($subrec->{size});
  status = pack_$subrec->{size}(pass_unpack_msg_len,(void *)&unpacked_msg->$subrec->{name},&ret_pack_msg_len,(void *)&packed_msg->$subrec->{name});

  if (status != AUTOGEN_RESULT_SUCCESS)
  {
    return status;
  }
  ";
            }
        }
        elsif($subrec->{memberType} eq "union")
        {
            my %enumHash;
            my $memberStructureTypeName;
            my $memberStructureVariableName;
            
            # Non-primitive type - union
            print $CFILE "\n
  switch(unpacked_msg->$lastEnumName)
  {";
            
            # Loop through all the enumerator values and use the corresponding structure and its unpack function
            $structIndex    =   0;
            
            # Using the enum name, get the index to the list which has information about all enumerator members
            #$enumIndex      =   getEnumIndex($lastEnumType);
            
            #if($enumIndex == -1)
            #{
            #    print "ERROR: enum $lastEnumName not found\n";
            #    die();
            #}
            
            %enumHash       =   %{$enumMemberList};
            
            %localEnumHash  =   %{$enumHash{$lastEnumType}};
            
            my @enumArray = sort { ($localEnumHash{$a} cmp $localEnumHash{$b}) } keys %localEnumHash;
            my $defaultEnum = $enumArray[0];
            
            my $defaultMemberStructureTypeName        =   $data_struct{$subrec->{size}}[0]->{size};
            my $defaultMemberStructureVariableName    =   $data_struct{$subrec->{size}}[0]->{name};
            
            foreach my $enum (@enumArray)
            {
                # $data_struct{$subrec->{size}} gives information about the member structure
                # From this point we need to know the structure's type name and variable name
                # This is got using the fields 
                #             a. {size} -> Type name
                #             b. {name} -> Variable name
                # PS: $data_struct{} is an array of entries
                #     Here we use $structIndex to index one of the entries
                #     The assumption is that for every enumType we have a corresponding structure member in the union
                #     Hence a straightforward indexing works.
                $memberStructureTypeName        =   $data_struct{$subrec->{size}}[$structIndex]->{size};
                $memberStructureVariableName    =   $data_struct{$subrec->{size}}[$structIndex]->{name};
                
                if($memberStructureTypeName eq "" or $memberStructureVariableName eq "")
                {
                    # There are no more structures present, exit from the for loop
                    last;
                }
                
                print $CFILE "
    case $enum:
                pass_unpack_msg_len = sizeof($memberStructureTypeName);
                status = pack_$data_struct{$subrec->{size}}[$structIndex]->{size}(pass_unpack_msg_len,(void *)&unpacked_msg->$subrec->{name}.$memberStructureVariableName,&ret_pack_msg_len,(void *)&packed_msg->$subrec->{name}.$memberStructureVariableName);

                if (status != AUTOGEN_RESULT_SUCCESS)
                {
                  return status;
                }
                
                break;\n";
                
                # We are assuming that for every enumerator value we have a corresponding structure in the enumerator in the same order
                $structIndex++;
            }
  
            # End the switch case with a break statement
            print $CFILE "
    default:
                /* In case that we have uninitialized data which happens do be invalid, we want to avoid crashing
                   ASIA. We recursively set any Enums to 'default' value and any remaining data set to 0xBA */
                memset(packed_msg, 0xBA, sizeof($defaultMemberStructureTypeName));
                packed_msg->$lastEnumName = $defaultEnum;
                pass_unpack_msg_len = sizeof($defaultMemberStructureTypeName);
                status = pack_$defaultMemberStructureTypeName(pass_unpack_msg_len,(void *)&unpacked_msg->$subrec->{name}.$defaultMemberStructureVariableName,&ret_pack_msg_len,(void *)&packed_msg->$subrec->{name}.$defaultMemberStructureVariableName);

                if (status != AUTOGEN_RESULT_SUCCESS)
                {
                  return status;
                }
      break;/* Error handling */
  }";

        }
        elsif($subrec->{memberType} eq "enum")
        {
            $lastEnumName   =   $subrec->{name};
            $lastEnumType   =   $subrec->{displayTypeName};
            
            # Check if its an array of enumerator
            if($subrec->{arrayDimension} == 1)
            {
                # 1-D array
                print $CFILE "\n
  for(index0 = 0 ; index0 < $subrec->{array_length} ; index0++)
  {
    packed_msg->$subrec->{name}\[index0\] = unpacked_msg->$subrec->{name}\[index0\];  
  }";
            }
            else
            {
                print $CFILE "
  packed_msg->$subrec->{name} = unpacked_msg->$subrec->{name};";
            }
        }
        else
        {
            # Primtive type
            
            # Check if its an 1-D array, 2-D array or a simple variable
            if($subrec->{arrayDimension} == 2)
            {
                # 2-D array
                # Extract information about both the dimension of the 2-D array
                my @temp;
                
                # array_length for 2-D arrays will be of the form "x,y" where is x is the inner dimension and y is the outer dimension
                @temp   =   split(",", $subrec->{array_length});
                
                print $CFILE "\n
  for(index0 = 0 ; index0 < $temp[0] ; index0++)
  {
    for(index1 = 0 ; index1 < $temp[1] ; index1++)
    {
      packed_msg->$subrec->{name}\[index0\]\[index1\] = unpacked_msg->$subrec->{name}\[index0\]\[index1\];
    }
  }";
            }
            elsif($subrec->{arrayDimension} == 1)
            {
                # 1-D array
                print $CFILE "\n
  for(index0 = 0 ; index0 < $subrec->{array_length} ; index0++)
  {
    packed_msg->$subrec->{name}\[index0\] = unpacked_msg->$subrec->{name}\[index0\];  
  }";
            }
            else
            {
                print $CFILE "
  packed_msg->$subrec->{name} = unpacked_msg->$subrec->{name};";
            }
        }
    }
    
    # Assign size of the unpacked structure to the input parameter
    print $CFILE "
  \n  *packed_msg_len = sizeof(packed_$record);
    ";
    
    # Return success status
    print $CFILE "
  return AUTOGEN_RESULT_SUCCESS;";
    
    print $CFILE "\n}\n";
}

# Function which creates the pack/unpack routines in the file lte_plt_autogen_pack_unpack.foo
sub createPackUnpackRoutines
{
    my $record;
    my %data_struct             =   %{$recordList};
    my %hashRef                 =   %{$recordTypeList};     # Hash reference to record types
    my $C_PACK                  =   "pack";
    my $C_UNPACK                =   "unpack";
    
    print $CFILE "/*!
  @file
  lte_plt_autogen_pack_unpack.c

  @brief
  This file contains LTE PLT autogenerated pack/ unpack routines

  @detail
  description of functions in this file
 
*/

/*===========================================================================

  Copyright (c) 2008 QUALCOMM Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/13/08   Autogenerated      Do  not modify this file
===========================================================================*/\n\n
/*lint -save -e641*/\n\n"; #end print

   print $CFILE "#include \"lte_plt_autogen_pack_unpack.h\" \n"; #end print

   print $CFILE "\nvoid lte_plt_autogen_get_rsp_umid (int *num_umids_ptr, msgr_umid_type *umids )\n{
  *num_umids_ptr = 0;";

   foreach my $structure(keys %umid_hash) 
   {
      my $umid = $umid_hash{$structure};
      
      # Check if the UMID is present in your constants
      if(exists $enum_hash{$umid})
      {
        print $CFILE "\n  *umids = $umid;\n  umids++;\n  (*num_umids_ptr)++;\n";
      }
   }
   print $CFILE "}\n";

   my $r_n = sprintf("%x",$random_number);
   #print cookie function
   print $CFILE "\n\nint lte_plt_autogen_get_cookie ( void )\n{
   return 0x$r_n;\n}\n"; #end print
   
   # Go through the dependency list and dump packed structure/pack/unpack routines in that order
   foreach $record (@globalDependencylist)
                {
                    # Dump the packed structure
        dumpStructure($record, 1);
        if ($record =~ /autogen/)
        {
             dumpStructure($record, 0);
        }
                    
                    # We need to create the pack/unpack functions for only structs and not for unions
        if($hashRef{$record} eq "struct")
                    {
                        # Dump the unpack routine for this structure
            dumpUnpackRoutine($record);
                        
                        # Dump the pack routine for this structure
            dumpPackRoutine($record);
        }
   }
   
   # Dump the main pack/unpack routines
   print $CFILE "
lte_plt_autogen_result_enum_type lte_plt_autogen_unpack
( 
  msgr_umid_type umid, 
  uint32 packed_msg_len, 
  void *packed_msg_ptr, 
  uint32 *unpacked_msg_len, 
  void *unpacked_msg_ptr
)
{
  lte_plt_autogen_result_enum_type status = AUTOGEN_RESULT_SUCCESS;

  switch (umid)
  {"; #end print

   &printAllPackUnPackRoutines($C_UNPACK);

   print $CFILE "
     default:
             if (packed_msg_len != 0)
             {
               status = AUTOGEN_RESULT_UNKNOWN_UMID;
             }

             break;
  }

  return status;
}

"; #end print

    print $CFILE "
lte_plt_autogen_result_enum_type lte_plt_autogen_pack
( 
  msgr_umid_type umid, 
  uint32 unpacked_msg_len, 
  void *unpacked_msg_ptr, 
  uint32 *packed_msg_len, 
  void *packed_msg_ptr
)
{
  lte_plt_autogen_result_enum_type status = AUTOGEN_RESULT_SUCCESS;

  switch (umid)
  {"; #end print

   &printAllPackUnPackRoutines($C_PACK);

   print $CFILE "
     default:
             if (packed_msg_len != 0)
             {
               status = AUTOGEN_RESULT_UNKNOWN_UMID;
             }

             break;
  }

  return status;
}

"; #end print
}

# This function reads all the XML files and builds a hash for all umid's
sub getUmids
{
    my $hash_ref;
    
    # The below function is borrowed from Autogen.pl
    foreach my $xml_file (@XML_FILES)
    {
        $hash_ref   =   &parse_Umid_XML_Files($xml_file, \%umid_hash);   
        %umid_hash  =   %{$hash_ref};
    }
}

# Update record list
# We need 3 structures which have bitfields and we are not handling bitfields currently
# So once we have the list of all the structures, delete those 3 structures
# Add 3 new structures in desired format in their place
# The 3 structures in question are
# a. "lte_LL1_sys_time_struct"
# b. "ack_ri_payload_struct"
# c. "lte_l1_gfn_s"
sub sanitizeRecordList
{
    my $replaceRecord   =   0;
    my @elements;
    my %data_struct     =   %{$recordList};
    my %hashRef         =   %{$recordTypeList};
    
    foreach my $record (keys %data_struct) 
    {
        if($record eq "lte_LL1_sys_time_struct")
        {
            # Delete the entry and create a new entry
            delete $data_struct{$record};
            
            # Even though we are storing a record, we are specifying it as a default type so that we don't expand its contents
            $data_struct{$record}  =   [{name => "value", type => "BYTE", size => 2, array_length => 1, memberType => "default", displayTypeName => "uint16"}];
        }
        elsif($record eq "ack_ri_payload_struct")
        {
            @elements       =       ();
            
            # Delete the entry and create a new entry
            delete $data_struct{$record};
            
            push(@elements,{name => "length", type => "BYTE", size => 4, array_length => 1, memberType => "default", displayTypeName => "uint32"});
            
            push(@elements,{name => "payload", type => "BYTE", size => 4, array_length => 1, memberType => "default", displayTypeName => "uint32"});
            
            # Even though we are storing a record, we are specifying it as a default type so that we don't expand its contents
            $data_struct{$record}  =   [@elements];
        }
        elsif($record eq "lte_l1_gfn_s")
        {
            @elements       =       ();
            
            # Delete the entry and create a new entry
            delete $data_struct{$record};
            
            push(@elements,{name => "hyperframe", type => "BYTE", size => 4, array_length => 1, memberType => "default", displayTypeName => "uint32"});
            
            push(@elements,{name => "frame", type => "BYTE", size => 4, array_length => 1, memberType => "default", displayTypeName => "uint32"});
            
            $data_struct{$record}   =   [@elements];
        }
        
        # Change all instances of the union "lte_LL1_sys_time_struct" are changed to default
        foreach my $subrecord (@{$data_struct{$record}})
        {
            if($subrecord->{size} eq "lte_LL1_sys_time_struct")
            {
                # If the type is "lte_LL1_sys_time_struct" then change the type from "union" to "default"
                $subrecord->{memberType}    =   "struct";
            }
        }
    }
    
    ############################################## HACK ##############################################
    # Change the type of "lte_LL1_sys_time_struct" from "union" to "struct" so that it is dumped
    $hashRef{lte_LL1_sys_time_struct}       =   "struct";
    $recordTypeList                         =   \%hashRef;
    ############################################## HACK ##############################################
    
    ############################################## HACK ##############################################
    # Some structure should not be dumped as it was not parsed earlier
    # Remove them from data_struct hash so that it won't be dumped
    ############################################## HACK ##############################################
    delete $data_struct{lte_cphy_hrpd_meas_results_cnf_s};
    delete $umid_hash{lte_cphy_hrpd_meas_results_cnf_s};
    
    return \%data_struct;
}

# Function to format the .tu file so that we can easily parse it
sub processFile
{
    ######################################################################################
    # Place to pre process the file to make sure that we have only (fieldName:fieldValue)
    # pairs in the source file. If there is a new identifier introduced which breaks
    # our assumption, modify the original file here and create a new file
    # $fileName_Processed which will be used for further parsing
    ######################################################################################
    
    # Hack to parse (name,value) pairs in each node
    # The only exception is the (name,value) pair => bitfield
    # So search for all occurrences of bitfield and convert it to (name,value) by replace them with "bitfield: 1" to make our lives easy
    open(inputFile,$InputFileName) or die("ERROR: $InputFileName file open failed\n");
    open($fd, ">".$InputFileName."_processed");
    
    while (my $line = <inputFile>)
    {
        $line =~ s/bitfield/bitfield: 1/g;          # bitfield -> bitfield:
        $line =~ s/chain:/chan:/g;                  # Hexagon-5.1 uses chain: instead of chan:, which was used by older compiler
        $line =~ s/\._/autogen_/g;                    # Anonymous strcuts start with ._, change it to autogen_
        print $fd $line;                            # Print to the modified file
    }

    close(inputFile);
    close($fd);
    
    # Add a '@' in the end as that's the delimiter for our parser - HACK
    # We process nodes when we encounter a "@" symbol. Hence adding a "@" in the end
    # to process the last record
    open($fd, $InputFileName."_processed");
    print $fd "\n@";
    close($fd);
}

sub dumpInXMLFormat
{
    my $XML_FILE;
    my $XML_File    =   "xmloutput.xml";
    
    # XML Output
    # Before dumping it, dump an XML output
    my $xs  = new XML::Simple();
    my $xml = $xs->XMLout($recordList);
     
    open($XML_FILE,">".$XML_File) or die("Cannot open file $XML_File for write\n");
    
    print $XML_FILE $xml;
     
    close($XML_FILE);
}

# Debug function which dumps all the UMID names read from the XML files
sub dumpHashList
{
    open(DEBUG_FILE, ">debug_hash_New.txt");
    # Debug function to dump all the api hash
    foreach $debug_key (keys %umid_hash)
    {
        print DEBUG_FILE $debug_key,"\n";
    }
    close(DEBUG_FILE);
}

sub buildDependencyList
{
    my $record      =   shift;
    
    # Get a reference to the master hahs which has information about all the structures
    my %data_struct =   %{$recordList};
    
    foreach my $subrec (@{$data_struct{$record}})
    {
        if(($subrec->{memberType} eq "struct" or $subrec->{memberType} eq "union") and !(exists($recordNoted{$subrec->{size}})))
        {
            $recordNoted{$subrec->{size}}   =   1;
            
            buildDependencyList($subrec->{size});
        }
    }
    
    # Push the current record
    push(@globalDependencylist, $record);
}

# Function which prints all the structures and their dependencies
sub getStructureNames
{
    my %data_struct =   %{$recordList};
    
    foreach my $record (keys %data_struct)
    {
        print "Parent Record: ",$record,"\n";
        
        buildDependencyList($record);
        
        foreach my $member (@globalDependencylist)
        {
            print $member,"\n";
        }
        
        print "\n";
    }
}

##################################################################
# DEBUG Function
##################################################################
sub check
{
    my %data_struct =   %{$recordList};
    my %fdump;
    
    # First record the all the record names
    #Read in the fdump strings
    open(HANDLE_OLD, "<debug_hash_Original.txt") or die("File open failed");
    
    my @values;
    
    foreach $line (<HANDLE_OLD>) 
    {
    chomp($line);
    
    $fdump{$line} = 1;
    }
    close(HANDLE_OLD);
    
    foreach my $record (keys %fdump)
    {
        foreach my $subrec (@{$data_struct{$record}})
        {
            if($subrec->{memberType} eq "union")
            {
                print $record,"\n";
            }
        }
    }
}

sub printAllPackUnPackRoutines
{
    my $routine    =   shift;
    my $msg_name;
    
    foreach my $record (keys %{$recordList})
    {
        if( exists $umid_hash{$record})
        {
            $msg_name   =   $umid_hash{$record};
            
            if($routine eq "pack")
            {
                # Dump all pack routines
                print $CFILE "
    case $msg_name:
                   status = pack_$record(unpacked_msg_len, unpacked_msg_ptr, packed_msg_len, packed_msg_ptr);
                   break;
";
            }
            else
            {
                # Dump all pack routines
                print $CFILE "
    case $msg_name:
                   status = unpack_$record(packed_msg_len, packed_msg_ptr, unpacked_msg_len, unpacked_msg_ptr);
                   break;
";
            }
        }
    }
}

# This creates unit test routines
sub createUnitTestRoutines
{
    my $structName;
    my $umidName;
    my %data_struct     =   %{$recordList};
    
    print $CFILE "\ntypedef enum
{
   AUTOGEN_PACK_UNPACK_SUCCESS,
   AUTOGEN_PACK_UNPACK_FAILED
}autogen_pack_unpack_result_type_enum;

static autogen_pack_unpack_result_type_enum verify_pack_unpack(uint8 *reference_api, uint8 *packed_unpacked_api, uint32 length)
{
  uint32 index;
  uint32 index1;

  for (index = 0;index < length; index ++) 
  {
    if (reference_api[index] != packed_unpacked_api[index]) 
    {
      for (index1 = 0; index1 < length; index1++) 
      {
         printf(\"\\n ref_api is %d packed_unpacked is :%d \",(int)reference_api[index1],(int)packed_unpacked_api[index1]);
      }
      printf(\"\\n Length is :%d\",(int)length);
      printf(\"\\n Index failed at :%d\",(int)index);
      printf(\"\\n ref_api was %d packed_unpacked was :%d \",(int)reference_api[index],(int)packed_unpacked_api[index]);
      return AUTOGEN_PACK_UNPACK_FAILED;
    }
  }
  return AUTOGEN_PACK_UNPACK_SUCCESS;
}

/* Returns the number of tests failed */

uint8 ref_msg[1024];
uint8 unpacked_msg[1024];
uint8 packed_msg[1024];
uint8 passed_msg[1024];\n";

print $CFILE "\nint lte_plt_autogen_pack_unpack_unit_test( void )
{
  int tests_failed = 0;
  int tests_passed = 0;
  
  uint32 index;
  uint32 umid;
  lte_plt_autogen_result_enum_type status;
  uint32 unpacked_length = 0;
  uint32 packed_length = 0;
  autogen_pack_unpack_result_type_enum pack_unpack_status = AUTOGEN_PACK_UNPACK_SUCCESS;

  for (index = 0; index < 1024; index ++) 
  {
    ref_msg[index] = passed_msg[index] = (uint8)index;
    unpacked_msg[index] = packed_msg[index] = 0;
  }
   ";
   
   #while(($structName,$umidName) = each(%umid_hash))
   foreach my $struct (keys %umid_hash)
   {
        #if(exists($umid_hash{$umidName}))
        if( exists($data_struct{$struct}))
        {
            print $CFILE "\n
  umid = $umid_hash{$struct};
  
  if(umid == LTE_CPHY_CONN_MEAS_CFG_REQ)
  {
    tests_passed++;
  }
  else
  {
    status = lte_plt_autogen_unpack(umid, sizeof(packed_$struct), (void *) &passed_msg[0], &unpacked_length, (void *) &unpacked_msg[0]);
    
    if(status == AUTOGEN_RESULT_SUCCESS)
    {
      status = lte_plt_autogen_pack(umid,unpacked_length,(void *)&unpacked_msg[0],&packed_length,(void *)&packed_msg[0]);
    }
    else
    {
      printf(\"\\n For $umid_hash{$struct}, unpacking failed\");
    }
    
    if(status == AUTOGEN_RESULT_SUCCESS) 
    {
      pack_unpack_status = verify_pack_unpack(&ref_msg[0],&packed_msg[0],packed_length);
    }
    else
    {
      printf(\"\\n For $umid_hash{$struct}, packing failed\");
    }
    
    if(pack_unpack_status == AUTOGEN_PACK_UNPACK_FAILED) 
    {
      printf(\"\\n For $umid_hash{$struct}, verify failed\");
      tests_failed++;
    }
    else if(pack_unpack_status == AUTOGEN_PACK_UNPACK_SUCCESS)
    {
       tests_passed++;
    }
    else
    {
      printf(\"\\n Error with status\");
    }
  }";
        }
   }
   
   print $CFILE "
  printf(\"\\n Tests passed: %d, tests failed: %d\",tests_passed,tests_failed);
  return tests_failed;
}\n
/*lint -restore*/
";
}

sub main
{
    my $numOfEntries = 0;
    my $ix;
    my $fd;
    
    # Read the command line arguments and open necessary files required 
    # for read and write from command line 
    if ( $profiling_mode == 1 )
    {
        commandLineFixed();
    }
    else
    {
    commandLineProcessing();
    }
   
   # Rountine to get a list of all UMID's
    getUmids();

    # Generate the random number which is used to validate the metadata
    &generate_random_number();
    
    # Function to format the .tu file so that we can easily parse it
    processFile();
    
    # This is the main function which parses the .tu dump and creates a data structure
    # which has information about all the interested nodes
    $numOfEntries = fdumpParser::parseDumpFile($InputFileName."_processed");
    
    print "INFO: Done preparing the entries table, numOfEntries: ",$numOfEntries,"\n";
    
    # This function parses the earlier created data structure and extracts all the enumerators
    # It returns a hash reference
    $enumMemberList     =   fdumpParser::getEnumerators($numOfEntries);
    
    # Changing the enum hash into a flat format which is easier to dump
    # Check function definition for more details
    prepareEnumHash();
    
    # Here we are dumping all the constants into APSSubSystem68Metadata_New.pm file
    dumpEnumerators();
    
    print "INFO: Done dumping all the enumerators\n";
    
    # This function parses the earlier created data structure and extracts all the structures/unions
    # It returns a hash reference
    $recordList         =   fdumpParser::getRecords($numOfEntries);
    
    # We get a reference to a list of hash which has information about all records and their types, struct or union
    $recordTypeList     =   fdumpParser::getRecordTypeList();
    
    # Dumping the received data in XML format which can be used in future
    # Currently the problem is forming a hash structure back from the XML file
    #dumpInXMLFormat();
    
    # Updating record list to handle structures/unions with bitfields
    $recordList         =   sanitizeRecordList();
    
    ################# DEBUG Routine #######################
    #dumpHashList();
    #######################################################
    
    my %data_struct     =   %{$recordList};
    
    # Build the dependency list and dump only those records
    # Clear the list array, this is a global list which will hold all the dependency
    @globalDependencylist           =   ();
    
    # For each UMID present
    foreach my $record (keys %umid_hash)
    {
        # Check if we have the record information in our database and not record
        if(exists($data_struct{$record}) and !(exists($recordNoted{$record})))
        {
            # This recursively parses a structure/union and fills the globalDependencyList with all
            # member structures and unions
            buildDependencyList($record);
        }
    }
    
    #########################################################
    # DEBUG - print all the dependency list
    #foreach my $record (@globalDependencylist)
    #{
    #    print $record,"\n";
    #}
    #########################################################
    
    # Here we append to the APSSubSystem68Metadata_New.pm file with structure information
    dumpRecords();
    
    print "INFO: Done dumping all the structures and unions\n";
    
    # Creates the lte_plt_autogen_pack_unpack.foo file which has C routines for pack/unpack functionality
    createPackUnpackRoutines();
    
    print "INFO: Done creating the C file which has pack/unpack routines\n";
    
    # Dump the unit test function
    createUnitTestRoutines();
    
    print "INFO: Done creating unit test routines\n";
    
    # Close file
    close($METADATA);
    close($CFILE);
}

main();
