package PD_FILE_DLOAD;

use Win32::OLE;
use Win32::OLE::Variant;

my %fields = (
       cmd => undef,
       id => undef,
       sub_cmd => undef,
	   fileoperation => undef,
	   filenamelength => undef,
       reserved => undef,
       filesize => undef,
       filepayload => undef,
);

# METHOD:
#   new
#
# DESCRIPTION:
#   Constructor
#
# RETURN VALUE:
#   Instance
#
sub new
{
   my $that = shift;
   my $class = ref($that) || $that;
   my $self = { %fields,
   };
   bless $self, $class;
   return $self;
}

# METHOD:
#   AUTOLOAD
#
# DESCRIPTION:
#   Performs object validataion.
#
# RETURN VALUE:
#   reference object
#
sub AUTOLOAD
{
   my $self = shift;
   my $type = ref($that) || die "$self is not an object";
   my $name = $AUTOLOAD;
   $name =~ s/.*://;   #strip fully-qualified portion
   unless( exists $self->{$name} ) {
      die "Can't access '$name' field in object of class $type";
   }
   if( @_) {
      return $self->{$name} = shift;
   }else{
      return $self->{$name};
   }
}

sub uint32toBytes {
   my ($arg) = @_;
   my($byte1, $byte2, $byte3, $byte4);

   # printf ("%d", $arg);
    
   $byte1 = $arg & 0xFF; $arg = $arg >> 8;
   $byte2 = $arg & 0xFF; $arg = $arg >> 8;
   $byte3 = $arg & 0xFF; $arg = $arg >> 8;
   $byte4 = $arg & 0xFF;

   #printf ("  %d %d %d %d\n", $byte1, $byte2, $byte3, $byte4);

   return($byte1, $byte2, $byte3, $byte4);

}

# METHOD:
#   SetCaptureCmd
#
# DESCRIPTION:
#   Initialize HS bitmap capture command packet.
#
# RETURN VALUE:
#   None
#
sub SetCaptureCmd
{
   my $self = shift;
   my ($arg) = @_;
   my $type = ref($self) || die "$self is not an object";

   # Header = 4 bytes; FileOperation = 1 byte;
   # FileNameLength = 1 byte; Reserved = 4 bytes;
   # FileSize = 4 bytes; FilePayload = Variable Length
   # => 14 bytes fixed + length of file name + size of file
   
   my $length =  14 + length($self->{filepayload});

   my $req_pkt = Variant(VT_ARRAY | VT_UI1, $length);
   printf("Variant Array Length: %d\n", $length);
   $req_pkt->Put(0, $self->{"cmd"});
   $req_pkt->Put(1, $self->{"subsys_id"});
   $req_pkt->Put(2, 0xFF & $self->{"sub_cmd"});

   my $upper = 0xFF & ($self->{"sub_cmd"} >> 8);
   $req_pkt->Put(3, $upper);

   $req_pkt->Put(4, $self->{"fileoperation"});
   $req_pkt->Put(5, $self->{"filenamelength"});

   # 4 bytes for "reserved" field (Bytes 6,7,8,9) 
   my @argsTopass = uint32toBytes($self->{"reserved"}); 
   my $i = 6;
   foreach (@argsTopass)
   {
      $req_pkt->Put($i, $_);
      $i++;
   }

   # 4 bytes for "filesize" (Bytes 10,11,12,13)
   my @argsTopass = uint32toBytes($self->{"filesize"}); 
   my $i = 10;
   foreach (@argsTopass)
   {
      $req_pkt->Put($i, $_);
      $i++;
   }

   my $i = 14;
   foreach (split(//, $self->{"filepayload"})) 
   {
     $req_pkt->Put($i, ord($_));
	 #printf("Data: %d\n", ord($_));
     $i++;
   }
   return $req_pkt;
}
