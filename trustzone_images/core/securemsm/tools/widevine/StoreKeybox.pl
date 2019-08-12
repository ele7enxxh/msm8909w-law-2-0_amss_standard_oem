#*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
#
#                        ParseKeybox.pl Tool
#
#GENERAL DESCRIPTION
#  parse Widevine keybox.
#
#USE:    perl ParseKeybox.pl keybox.xml device_ID
#Output: keybox.bin
#
#Copyright (c) 2012 Qualcomm Incorporated.
#All Rights Reserved.
#Qualcomm Confidential and Proprietary
#
#*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
use strict;
use XML::LibXML;

my $myID = "DeviceID";
my $DeviceID = $ARGV[1];

my $newparser = XML::LibXML->new();
my $xmlfile   = $newparser->parse_file($ARGV[0]);
my $return    = 0;
my $filename  = 'keybox.bin';


#travel all keybox notes
foreach my $keybox ($xmlfile->findnodes('/Widevine/Keybox')) {
    my $Key = $keybox->findnodes('./Key');
    my $ID = $keybox->findnodes('./ID');
    my $Magic = $keybox->findnodes('./Magic');
    my $CRC = $keybox->findnodes('./CRC');
    my @attributelist = $keybox->attributes();
    $DeviceID = $attributelist[0]->to_literal;

    if($DeviceID eq $ARGV[1])
    {
        #found it, print the keybox info
        #check it the Device length
        if (length($DeviceID) > 32){
          print "\nError: The length of DeviceID should NOT larger than 32 bytes! \n";
          last; #break
        }
        $return = 1;
        print "DeviceID: ", $DeviceID, "\n"; 
        print "Key:      ", $Key->to_literal, "\n";
        print "ID:       ", $ID->to_literal, "\n";
        print "Magic:    ", $Magic->to_literal, "\n";
        print "CRC:      ", $CRC->to_literal, "\n";
        print "\n\n";

        #cast string to hexadecima
        $Key = $Key->to_literal;
        $ID = $ID->to_literal;
        $Magic = $Magic->to_literal;
        $CRC = $CRC->to_literal;
        
        #print $Key, "\n";
        open FILE, ">$filename" or die "Unable to create or open the keybox.bin file $!";	
        binmode FILE; #added by Andrew 

        foreach my $i ( 0...(length($DeviceID) - 1 ) )
        { 
          my $char = substr($DeviceID, $i, 1);
          my $ascii = sprintf("%2x", ord($char));
          #print FILE pack("C",$char);
          print FILE pack("H2",$ascii);
          #print pack("H2",$char), "\n";
        }
        foreach my $i ( 0...(32 - length($DeviceID) -1) )
        {
          print FILE "\x00";
        }

        foreach my $i ( 0...(length($Key)/2 - 1 ) )
        { 
          my $char = substr($Key, $i*2, 2);
          print FILE pack("H2",$char);
        }
        
        foreach my $i ( 0...(length($ID)/2 - 1 ) )
        { 
          my $char = substr($ID, $i*2, 2);
          print FILE pack("H2",$char);
        }
        foreach my $i ( 0...(length($Magic)/2 - 1 ) )
        { 
          my $char = substr($Magic, $i*2, 2);
          print FILE pack("H2",$char);
        }
        
        foreach my $i ( 0...(length($CRC)/2 - 1 ) )
        { 
          my $char = substr($CRC, $i*2, 2);
          print FILE pack("H2",$char);
        }
        close FILE or die "Unable to close the keybox.bin file $!";
    }
}
if ($return != 1)
{
  print "ERROR: Parsing keybox file failed. \n";
  die("There is no such a keybox having DeviceID = ", $ARGV[1], "\n");
}
print "Parsing keybox data succeeds! \n";

system("adb start-server");
system("adb push keybox.bin /data/");
print "Pushing keybox data to device succeeds! \n";
system("adb shell StoreKeybox /data/keybox.bin");
system("adb shell rm /data/keybox.bin");
