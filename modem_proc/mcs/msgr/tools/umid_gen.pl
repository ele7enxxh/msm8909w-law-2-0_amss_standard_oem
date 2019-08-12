#!/usr/bin/env perl

use English;
use strict;
use Getopt::Long;    # Parsing command-line args
use Time::localtime; # localtime for copyright
use XML::Simple;     # XML "compilation" output
use Data::Dumper;
use bytes;
use File::Basename;

my $VERSION = "1.4";
my $HELP = "UMID Generator, version $VERSION";

my $MAXLINES = 10;

my $g_umids;
my $g_tech_mods;
my $g_defines;
my $g_curfile;
my @g_preproc;

my $opt_help;
my $opt_debug;
my $opt_compile;
my $opt_link;
my $opt_macro;
my $opt_output;
my $opt_osource;
my $opt_odocs;
my @opt_inputs = ();
my $opt_tech;
my @opt_techs = ();
my @opt_paths = ();
my @opt_preprocs = ();


# Get the current year, for copyright verbiage
my $currentyear = localtime->year() + 1900;

# Used to match nested parens, ref bottom of http://dev.perl.org/perl6/doc/design/exe/E05.html
our $parens_re;
$parens_re = qr/ \(  (?: (?>[^()]+) | (??{$parens_re}) )*  \) /x;

# Call the main subroutine
main();

# ------------------------------------------------------------------------
# Name       : usage()
# Description: Starting point for program execution
# Parameters : none
# Returns    : none
# Notes      :
# ------------------------------------------------------------------------
sub usage()
{
  print $HELP;
  print "\n";
  exit(0);
}


# ------------------------------------------------------------------------
# Name       : main()
# Description: Starting point for program execution
# Parameters : none
# Returns    : none
# Notes      : will process @ARGV
# ------------------------------------------------------------------------
sub main()
{
  GetOptions( 'help'      => sub { usage(); },
              'debug'     => \$opt_debug,
              'compile'   => \$opt_compile,
              'link'      => \$opt_link,
              'output=s'  => \$opt_output,
              'input=s'   => \@opt_inputs,
              'os|output_source=s'  => \$opt_osource,
              'od|output_docs=s'    => \$opt_odocs,
              'macro=s'   => \$opt_macro,
              'tech=s'    => \$opt_tech,
              'paths=s'   => \@opt_paths,
              'doti=s'    => \@opt_preprocs,
              );
  dprint( "Parsed command-line options\n" );

  # Fixup opt_paths
  foreach my $dir (@opt_paths) {
    # Replace \ with /
    $dir =~ s/\\/\//g;
    # Make sure the path has a trailing slash
    $dir =~ s/([^\/])$/$1\//
  }

  # Fixup osource and odocs
  $opt_osource =~ s/\\/\//g if( defined($opt_osource) );
  $opt_odocs =~ s/\\/\//g if( defined($opt_odocs) );

  # Parse through to find the following pairs:
  #   --start-inputs  and --end-inputs
  my $in_input_block   = 0;
  foreach my $arg (@ARGV) {
    $in_input_block  = 0 if( $arg =~ /^--end-inputs$/ );

    push( @opt_inputs,  $arg ) if( $in_input_block );

    $in_input_block  = 1 if( $arg =~ /^--start-inputs$/ );
  }

  #   --start-preprocs  and --end-preprocs
  my $in_input_block   = 0;
  foreach my $arg (@ARGV) {
    $in_input_block  = 0 if( $arg =~ /^--end-preprocs$/ );

    push( @opt_preprocs,  $arg ) if( $in_input_block );

    $in_input_block  = 1 if( $arg =~ /^--start-preprocs$/ );
  }

  # Add paths for each input location if no preprocs
  if( @opt_preprocs == 0 && $opt_compile )
  {
    foreach my $input (@opt_inputs)
    {
      if( $input =~ /(.*)\/(.*?)/ )
      {
        my $path = $1."/";
        my $file = $2;

        # See if the path is already in our list
        my $found = 0;
        foreach my $p (@opt_paths) 
        {
          if( $p eq $path )
          {
            $found = 1;
            last;
          }
        }
        # If it's not found, add it to our list
        if( !$found )
        {
          dprint("Adding input path to list of paths: $path\n");
          push( @opt_paths, $path );
        }
      }
      else
      {
        die "Bad input: $input";
      }
    }
  }

  if( (!defined($opt_compile) && !defined($opt_link)) ||
      (defined($opt_compile) && defined($opt_link)) ) {
    print "Error: Must specify one and only one of --compile/--link\n\n";
    usage();
  }
  if( $opt_compile ) {
    die "Must specify -m <#define tag>" if( !defined($opt_macro) );
    die "Must specify -t <tech list>" if( !defined($opt_tech) );
    die "Must specify -o <output file>" if( !defined($opt_output) );

    dprint( "Output = $opt_output\n" );
  }
  if( $opt_link ) {
    die "Must specify -os <output file>" if( !defined($opt_osource) );

    dprint( "Output Source = $opt_osource\n" );
    dprint( "Output Docs   = $opt_odocs\n" );
  }

  @opt_techs    = split( /,/, $opt_tech );

  # What mode are we in?  Compilation or Linking?
  if( $opt_compile ) 
  {
    # If the header files were passed in, then just parse them.  
    # Otherwise we need to browse the tree
    foreach my $header (@opt_inputs) 
    {
      if( @opt_preprocs > 0 ) 
      {
        my $found = 0;
        my @suffixes = (".h", ".i");

        # Capture the base filename
        my $base = basename($header, @suffixes);
        dprint( "header: $header -> $base\n" );
        # Iterate through the preprocs to find a matching filename
        foreach my $preproc (@opt_preprocs)
        {
          my $preproc_base = basename($preproc, @suffixes);
          dprint( "preproc: $preproc -> $preproc_base\n" );
          if( $base eq $preproc_base )
          {
            dprint( "match: $header <-> $preproc\n" );
            # Process the header along w/ the preproc
            process_file($header, $preproc);
            # Done
            $found = 1;
            last;
          }
        }
        die "Unable to find matching preproc: $header" if( !$found );
      }
      else
      {
        process_file($header);
      }
    }

    print_xml_file( $g_umids, $g_tech_mods, $opt_output );
  } elsif( $opt_link ) {
    foreach my $xml (@opt_inputs) {
      load_xml_file($xml);
    }

    my $path;
    my $short_file;
    if( $opt_osource =~ /^(.*)\/(.*?)$/ ) {
      $path       = $1;
      $short_file = $2;
    } else {
      die "Output must go in a directory: $opt_osource";
    }

    my $hdr_def = uc($short_file)."__";
    $hdr_def =~ s/\./_/g;

    # Generate the output source file
    print_source_file( $path, $short_file );

    # Generate the output documentation header file
    if( $opt_odocs =~ /^(.*)\/(.*?)$/ ) {
      $path       = $1;
      $short_file = $2;
      
      print_header_file( $path, $short_file, $hdr_def );
    }
  } else {
    die "Internal Error";
  }
}

# ------------------------------------------------------------------------
# Name       : dprint()
# Description: Debug print routine.  Prints out the specified string only if 
#              debugging is enabled.
# Parameters : String to print
# Returns    : none
# Notes      : 
# ------------------------------------------------------------------------
sub dprint($)
{
  my $str = shift;  # string to print

  if( $opt_debug ) {
    print $str;
  }
}

# ------------------------------------------------------------------------
# Name       : check_literal
# Description: 
# Parameters : 
# Returns    : none
# Notes      : 
# ------------------------------------------------------------------------
sub check_literal( $ )
{
  my $val = shift;

  if( defined $val
      && ( $val =~ /^\d+$/ || $val =~ /^0x[0-9a-fA-F]+$/ ) )
  {
    # TRUE - is a literal
    return 1;
  }

  # FALSE - not a literal
  return 0;
}

# ------------------------------------------------------------------------
# Name       : Get Decimal Value
# Description: 
# Parameters : 
# Returns    : none
# Notes      : 
# ------------------------------------------------------------------------
sub get_value( $ )
{
  my $input = shift;
  my $value;

  if( $input =~ s/0x// )
  {
    $value = hex($input);
  }
  else
  {
    die "$input is not a hex value";
  }

  return $value;
}

# ------------------------------------------------------------------------
# Name       : lookup_define_from_preproc()
# Description: Searches for the specified define in the specified 
#              preprocessed header file
# Parameters : Define for which to search
# Returns    : Returns whether the define was found
# Notes      : Modifies $g_defines
# ------------------------------------------------------------------------
sub lookup_define_from_preproc($$)
{
  my $symbol   = shift;   # UMID Symbol
  my $define   = shift;   # Define to push into $g_defines

  my $found    = 0;
  # Search the preproc file looking for the specified UMID
  foreach my $line (@g_preproc)
  {
    # use "parens" to group everything inside the outter-most parens (including "inside" parens)
    if( $line =~ /$symbol\s+=\s+($parens_re)/ ) 
    {
      my $rhs = $1;
      if( $rhs =~ /\(\(msgr_umid_type\) (.*)/ )
      {
        my $rhs_strip = $1;
        # The following regexp MUST match the macro defined in msgr_umid.h
        # Any change in bitfield pattern here must also be accompanied by changes in another location below.
        if( $rhs_strip =~ /\(\(\(\(\(uint16\) \(($parens_re)\ << 8\) \| ($parens_re)\)\) & 0xFFFF\) \<\< 16\) \| \(($parens_re) \<\< 8\) \| ($parens_re)/ )
        {
          my $tech = $1;
          my $mod  = $2;
          my $type = $3;
          my $id   = $4;

          dprint("lookup: $symbol: $tech, $mod, $type, $id\n");
          my $value_re = q/\(\((0x[\da-fA-F]{1,2})\)\s\&\s0x[\da-fA-F]{2}\)/;
          if( $define =~ /MSGR_TECH/ )
          {
            if( $tech =~ /$value_re/ )
            {
              my $val = $1;
              dprint("lookup: $symbol: $define = $val\n");
              $g_defines->{$define} = $val;
            }
            else
            {
              die "Internal error: mismatch in umid definition: $tech";
            }
          }
          elsif( $define =~ /MSGR_MODULE/ )
          {
            if( $mod =~ /$value_re/ )
            {
              my $val = $1;
              dprint("lookup: $symbol: $define = $val\n");
              $g_defines->{$define} = $val;
            }
            else
            {
              die "Internal error: mismatch in umid definition: $mod";
            }
          }
          elsif( $define =~ /MSGR_TYPE/ )
          {
            if( $type =~ /$value_re/ )
            {
              my $val = $1;
              dprint("lookup: $symbol: $define = $val\n");
              $g_defines->{$define} = $val;
            }
            else
            {
              die "Internal error: mismatch in umid definition: $type";
            }
          }
          else
          {
            my $value_id_re = q/\(\((\d+|0x[\da-fA-F]{1,2})\)\s\&\s0x[\da-fA-F]{2}\)/;
            if( $id =~ /$value_id_re/ )
            {
              my $val = $1;
              dprint("lookup: $symbol: $define = $val\n");
              $g_defines->{$define} = $val;
            }
            elsif( $id =~ /\(\((.*)\) \& 0x[\da-fA-F]{2}\)/ )
            {
              # The ID was not a #define, so look for enums
              my $id_sym = $1;
              # The name of the preproc file was stored at the end of the g_preproc array
              my $preproc_file = @g_preproc[@g_preproc - 1];
              my $fh;
              open( $fh, $preproc_file ) || die "unable to open preproc file: $preproc_file";
              while( my $enum_line = <$fh> )
              {
                if( $enum_line =~ /$id_sym\s*=\s*([^,\s]+)/ )
                {
                  my $raw_val = $1;
                  dprint("lookup: $symbol: $id_sym = $raw_val(raw)\n");
                  my $val;
                  if( $raw_val =~ /0x[\da-fA-F]+/ )
                  {
                    $val = $raw_val;
                    dprint("lookup: $symbol: $id_sym = $raw_val(hex)\n");
                  }
                  elsif( $raw_val =~ /\d+/ )
                  {
                    $val = sprintf("0x%x", $raw_val);
                    dprint("lookup: $symbol: $id_sym = $raw_val(dec) = $val\n");
                  }
                  else
                  {
                    die "unable to match value type of decimal or hex: $raw_val";
                  }
                  dprint("lookup: $symbol: $define = $val\n");
                  $g_defines->{$define} = $val;
                  last;
                }
              }
              close( $fh );
            }
            else
            {
              die "Internal error: mismatch in umid definition: $id";
            }
          }
        }
      }
      else
      {
        die "Internal error: mismatch in umid definition: $rhs";
      }
      # Mark us as found and exit the loop
      $found = 1;
      last;
    }
  }

  # Return if we found the umid
  return $found;
}


# ------------------------------------------------------------------------
# Name       : lookup_define()
# Description: Searches for the specified define in header files located in 
#              directories contained in @opt_paths.
# Parameters : Define for which to search
# Returns    : none (see notes)
# Notes      : Modifies $g_defines
# ------------------------------------------------------------------------
sub lookup_define($)
{
  my $define = shift;  # Define to lookup

  
  if ( check_literal($define) || check_literal($g_defines->{$define}) )
  {
    dprint("--short--\n");
    return;
  }

  my @search_files;

  # We can search in our current file
  push( @search_files, $g_curfile );

  # We can also search all of the dirs passed in
  foreach my $dir (@opt_paths) {
    my $dh;
    opendir( $dh, $dir ) || die "Unable to search in dir: $dir";
    my @files = readdir( $dh );
    foreach my $file (@files) {
      if( $file =~ /\.h$/ ) {
        push( @search_files, $dir.$file );
      }
    }
    closedir( $dh );
  }

  my $found = 0;
  foreach my $file (@search_files) {
    dprint("Looking up $define in $file\n");
    my $fh;
    open( $fh, $file ) || die "Unable to open file: $file";

    while( my $line = <$fh> ) {
      # Look for a #define
      if( $line =~ /^\s*\#define\s+$define\s+(.*)$/ ) {
        my $def = $1;
        # Remove comments
        $def =~ s/\/\/.*$//;
        $def =~ s/\/\*.*?\*\///;

        # Remove any whitespace around the remaining definition
        $def =~ s/\s//g;

        dprint("Located Definition: $define -> $def\n");
        $g_defines->{$define} = $def;
        $found = 1;
        last;
      }

      # Look for an enum w/ an '=' assignment
      if( $line =~ /^\s*$define\s*=\s*(.*?)[\s,]+.*$/ ) {
        my $def = $1;
        # Remove comments
        $def =~ s/\/\/.*$//;
        $def =~ s/\/\*.*?\*\///;

        dprint("Located Enumeration: $define -> $def\n");
        $g_defines->{$define} = $def;
        $found = 1;
        last;
      }
    }
    close( $fh );
    # If we've found the definition, bail out now
    last if( $found );
  }

  if( !$found ) {
    print "Error: unable to find '$define' used in $g_curfile\n";
    die;
  }
}

# ------------------------------------------------------------------------
# Name       : recursive_lookup_define
# Description: 
# Parameters : 
# Returns    : none
# Notes      : 
# ------------------------------------------------------------------------
sub recursive_lookup_define( $ )
{
  my $define = shift;

  if ( check_literal($define) || check_literal($g_defines->{$define}) )
  {
    return;
  }

  my $def = $define;

  my $ctr = 0;
  while( 1 )
  {
    $ctr++;

    # if $def is a literal (e.g. decimal or hex number), then we're done
    if( check_literal($def) )
    {
      last;
    }

    # Otherwise, assume it's a #define and look it up
    lookup_define( $def );

    dprint("Recurse: $def  -> $g_defines->{$def}\n");
    $def = $g_defines->{$def};
  }

  $g_defines->{$define} = $def;
  dprint("  Set: $define  -> $g_defines->{$define}\n");

  if ( ! check_literal( $g_defines->{$define} ) )
  {
    print "Error:  Definition  $define  - literal not found\n";
    die;
  }
}

# ------------------------------------------------------------------------
# Name       : add_umid()
# Description: 
# Parameters : 
# Returns    : none
# Notes      : 
# ------------------------------------------------------------------------
sub add_umid($$$$$$)
{
  my $tech       = shift;   # 'Tech'   portion of UMID symbol
  my $mod        = shift;   # 'Module' portion of UMID symbol
  my $type       = shift;   # 'Type'   portion of UMID symbol
  my $id         = shift;   # 'ID'     portion of UMID symbol
  my $id_literal = shift;   # 'ID'     portion of UMID value as a string
  my $struct     = shift;   # Payload structure definition

  my $tech_define   = "MSGR_TECH_".$tech;
  my $mod_define    = "MSGR_MODULE_".$mod;
  my $type_define   = "MSGR_TYPE_".$type;

  my $tech_val;
  my $mod_val; 
  my $type_val;
  my $idnum;

  ########################################################
  # New method: Use preprocessor used by MBS 2.0 (SCons) #
  ########################################################
  if( @g_preproc )
  {
    # Construct the symbol
    my $symbol = $tech."_".$mod."_".$id."_".$type;

    ##
    #  Tech
    # The preproc buffer is full, so lookup from preproc
    my $umid_exists = lookup_define_from_preproc( $symbol, $tech_define );
    # If the UMID doesn't exist in the preproc, then it was actually #ifdef'd out
    # If so, do nothing and just return now
    return if( !$umid_exists );

    $tech_val = get_value($g_defines->{$tech_define});

    ##
    #  Module
    # The preproc buffer is full, so lookup from preproc
    lookup_define_from_preproc( $symbol, $mod_define );
    $mod_val = get_value($g_defines->{$mod_define});

    ##
    #  Type
    # The preproc buffer is full, so lookup from preproc
    lookup_define_from_preproc( $symbol, $type_define );
    $type_val = get_value($g_defines->{$type_define});

    ##
    # Identifier
    $idnum = $id_literal;
    if ( ! check_literal( $id_literal ) )
    {
      # The preproc buffer is full, so lookup from preproc
      lookup_define_from_preproc( $symbol, $id_literal );

      $idnum = $g_defines->{$id_literal};
    }
  } 
  ###################################################
  # Old method: search through all headers used by Qbuild
  ###################################################
  else
  {
    my $tech_mod_define = "MSGR_".$tech."_".$mod;

    ##
    #  Tech and Module (together)
    # Lookup by browsing the include paths, but don't do this recursively yet
    dprint("Looking up tech/mod: $tech_mod_define\n");
    lookup_define( $tech_mod_define );
    # Lookup the value that was parsed
    my $tech_mod = $g_defines->{$tech_mod_define};
    if( $tech_mod =~ /MSGR_TECH_MODULE\(\s*(\S+),\s*(\S+)\)/ )
    {
      my $inner_tech_define = $1;
      my $inner_mod_define  = $2;

      die "Tech values don't match: $tech_define != $inner_tech_define" if( $tech_define ne $inner_tech_define );
      dprint("Decomposing $tech_mod_define: $inner_tech_define, $inner_mod_define\n");
      # Recursively lookup the inner tech by browsing the include paths
      recursive_lookup_define( $inner_tech_define );
      $tech_val = get_value($g_defines->{$inner_tech_define});

      # The inner module define might be a literal, so account for that
      if( check_literal($inner_mod_define) )
      {
        # Just use the value since we're already at a literal
        $mod_val = get_value($inner_mod_define);
      }
      else
      {
        # Recursively lookup the inner tech by browsing the include paths
        recursive_lookup_define( $inner_mod_define );
        $mod_val = get_value($g_defines->{$inner_mod_define});
      }
    } 
    else
    {
      die "Tech/Module ($tech_mod_define) not built using MSGR_TECH_MODULE() macro";
    }

    ##
    #  Type
    # Lookup by browsing the include paths
    recursive_lookup_define( $type_define );
    $type_val = get_value($g_defines->{$type_define});

    ##
    # Identifier
    $idnum = $id_literal;
    if ( ! check_literal( $id_literal ) )
    {
      # Lookup by browsing the include paths
      recursive_lookup_define( $id_literal );

      $idnum = $g_defines->{$id_literal};
    }
  }
  #####################
  # Done w/ 2 methods
  #####################

  ##
  # Create a new entry
  my $entry = {
    tech   => $tech,
    mod    => $mod,
    type   => $type,
    id     => $id,
    idnum  => $idnum,
    struct => $struct,
    func   => "NULL",
  };
  # Convert idnum to decimal
  if( $entry->{idnum} =~ /0x/ ) {
    $entry->{idnum} =~ s/0x//;
    $entry->{idnum} = hex($entry->{idnum});
  }
  # Set the UMID symbol
  $entry->{sym} = $tech."_".$mod."_".$id."_".$type;
  # Set the UMID field
  $entry->{umid_val} = 
    ( (($tech_val & 0xFF) << 24) +
      (($mod_val  & 0xFF) << 16) +
      (($type_val & 0xFF) <<  8) +
      (($entry->{idnum} & 0xFF)) );
  $entry->{umid_hex} = sprintf("%08x", $entry->{umid_val});

  # Save the UMID
  insert_umid_entry( $entry );

  # Strip off and generate the TECH/MODULE portion
  my $tech_mod = {
    sym => "MSGR_".$tech."_".$mod,
    tech_val => ($tech_val & 0xFF),
    mod_val  => ($mod_val & 0xFF),
  };
  # Set the val field
  $tech_mod->{val} = ( (($tech_val & 0xFF) << 8) + ($mod_val & 0xFF) );
  $tech_mod->{val_hex} = sprintf("%04x", $tech_mod->{val});

  # Save the TECH/MODULE entry
  insert_module_entry( $tech_mod );

  return $entry;
}


# ------------------------------------------------------------------------
# Name       : insert_umid_entry
# Description: Inserts the given UMID entry into the global umid table
# Parameters : $entry 
# Returns    : none
# Notes      : 
# ------------------------------------------------------------------------
sub insert_umid_entry($)
{
  my $entry  = shift;  # Entry to insert into global umid table

  if( !defined($entry->{umid_hex}) ||
      !defined($entry->{sym})
    )
  {
    print "Malformed umid entry: \n";
    print Dumper($entry);
    die;
  }

  dprint("adding umid: {\n");
  dprint("  sym     -> $entry->{sym}\n");
  dprint("  tech    -> $entry->{tech}\n");
  dprint("  mod     -> $entry->{mod}\n");
  dprint("  type    -> $entry->{type}\n");
  dprint("  id      -> $entry->{id}\n");
  dprint("  idnum   -> $entry->{idnum}\n");
  dprint("  struct  -> $entry->{struct}\n");
  dprint("  func    -> $entry->{func}\n");
  dprint("  umid    -> $entry->{umid_val}\n");
  dprint("  umid_h  -> $entry->{umid_hex}\n");
  dprint("}\n");

  my $key = $entry->{umid_hex};
  if( defined( $g_umids->{$key} ) ) {
    my $errstr = "Duplicate UMID value defined: 0x$key\n";
    $errstr   .= "  First def: $g_umids->{$key}->{sym}\n";
    $errstr   .= "  Second def: $entry->{sym}\n";
    die $errstr;
  }
  $g_umids->{$key} = $entry;
}


# ------------------------------------------------------------------------
# Name       : insert_module_entry()
# Description: Inserts a module entry into the global table
# Parameters : $entry
# Returns    : none
# Notes      : 
# ------------------------------------------------------------------------
sub insert_module_entry($)
{
  my $entry = shift;   # Module entry to insert

  my $key = $entry->{val_hex};
  if( !defined( $g_tech_mods->{$key} ) ) {
    dprint("adding module: {\n");
    dprint("  sym    -> $entry->{sym}\n");
    dprint("  tech   -> $entry->{tech_val}\n");
    dprint("  mod    -> $entry->{mod_val}\n");
    dprint("  val    -> $entry->{val}\n");
    dprint("  val_h  -> $entry->{val_hex}\n");
    dprint("}\n");

    # Insert the entry
    $g_tech_mods->{$key} = $entry;
  } else {
    if( $g_tech_mods->{$key}->{sym} ne $entry->{sym} ) {
      die "Error: Conflicting modules: $g_tech_mods->{$key}->{sym}, $entry->{sym}";
    }
  }
}

# ------------------------------------------------------------------------
# Name       : add_umid_2()
# Description: 
# Parameters : 
# Returns    : none
# Notes      : 
# ------------------------------------------------------------------------
sub add_umid_2($$$$$$$)
{
  my $tech       = shift;   # 'Tech'   portion of UMID symbol
  my $mod        = shift;   # 'Module' portion of UMID symbol
  my $type       = shift;   # 'Type'   portion of UMID symbol
  my $id         = shift;   # 'ID'     portion of UMID symbol
  my $id_literal = shift;   # 'ID'     portion of UMID value as a string
  my $struct     = shift;   # Payload structure definition
  my $func       = shift;   # Function used in testing to create a string 
                            # representation of the UMID
  
  my $entry = add_umid($tech, $mod, $type, $id, $id_literal, $struct);
  $entry->{func} = $func;

  dprint("Modifying func:\n");
  dprint("  func   -> $entry->{func}\n");

  return $entry;
}

# ------------------------------------------------------------------------
# Name       : process_file
# Description: 
# Parameters : 
# Returns    : none
# Notes      : 
# ------------------------------------------------------------------------
sub process_file($;$)
{
  my $preproc;
  my $file;

  $file      = shift;   # Name of file to process
  $preproc   = shift;   # Preprocessed version of $file if( @_ > 0 );

  dprint( "Searching $file\n" );
  dprint( "  with preproc: $preproc\n" ) if( defined($preproc) );

  # preload the preproc file into ram
  if( defined($preproc) )
  {
    my $f_preproc;
    open( $f_preproc, $preproc ) || die "Unable to open preproc file: $preproc";
    @g_preproc = reverse <$f_preproc>;
    close( $f_preproc );
    # Append the name of the preproc file to the end of the list
    # It will be needed for enums
    push( @g_preproc, $preproc );
  }

  $g_curfile = $file;
  open( H_FILE, $file ) || die "Unable to open file: $file";

  
  while( my $line = <H_FILE> ) {
    if( $file =~ /foo/ ) {
      dprint( "line: $line" );
    }
    if( $line =~ /$opt_macro/ ) {
      # Remember where we left off
      my $pos = tell(H_FILE);
      # Read up to MAXLINES and look for our definition
      my $i;
      for( $i = 0; $i < $MAXLINES; $i++ ) {
        if( $line =~ /$opt_macro\s*\(.*?\)/ ) {
          last;
        }
        $line .= <H_FILE>;
        # Remove comments
        $line =~ s/\/\/.*$//;
        $line =~ s/\/\*.*?\*\///g;
        # Remove newlines
        $line =~ s/\n//g;
      }
      if( $i >= $MAXLINES ) {
        dprint("Too many lines, backing up\n");
        # Move back to where we left off
        seek( H_FILE, $pos, 0 );
        # and start again
        next;
      }
      # See if this is the master define
      if( $line =~ /^\s*\#define\s+$opt_macro\s*\(/ ) {
        dprint( "skipping master define: $line\n" );
        next;
      }

      # Otherwise we have a definition, remove whitespace
      $line =~ s/\s//g;

      dprint( "Found UMID in $file\n" );
      dprint( "umid: $line\n" );

      # use "parens" to group everything inside the outter-most parens (including "inside" parens)
      $line =~ m/$opt_macro($parens_re)/;

      # Parse the UMID definition
      if( $1 =~ /\((.+?),(.+?),(.+?),(.+?),(.+?),(.+?),(.+)\)/ ) {
        # Support for function to create a better name for mscgen
        add_umid_2( $1, $2, $3, $4, $5, $6, $7 );
      } elsif( $1 =~ /\((.+?),(.+?),(.+?),(.+?),(.+?),(.+)\)/ ) {
        # Standard args for umid defintion
        add_umid( $1, $2, $3, $4, $5, $6 );
      } else {
        print "Warning: Bad definition in $file: $line\n";
      }
      dprint( "end\n\n" );
    }
  }

  close( H_FILE );
  undef( $g_curfile );
}

# ------------------------------------------------------------------------
# Name       : print_header_file()
# Description: Open and write the header file
# Parameters : $path    - Path for the output file
#              $file    - Name of header file to write (w/o path)
#              $hdr_def - Feature define protecting multiple inclusion
# Returns    : none
# Notes      : 
# ------------------------------------------------------------------------
sub print_header_file($$$)
{
  my $path    = shift;   # Path to output directory
  my $file    = shift;   # Name of header file to write
  my $hdr_def = shift;   # Feature define protecting multiple inclusion

  my $hdr_heading = <<DONE;
/*!

  \@file   $file

  \@brief Auto-generated file containing UMID Documentation

  \@detail
  This file is AUTO-GENERATED.  Do not modify!
  This auto-generated header file declares the message router table mapping
  UMIDs to their string equivalent

*/

/*===========================================================================

  Copyright (c) $currentyear QUALCOMM Incorporated. All Rights Reserved

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

\#ifndef $hdr_def
\#define $hdr_def

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

DONE

  my $H_HDR_OUT;
  my $hdr_out = $path."/".$file;
  open( $H_HDR_OUT, ">$hdr_out" ) || die "Unable to open output file: $hdr_out";

  # Spit out the heading
  print $H_HDR_OUT $hdr_heading;

  print $H_HDR_OUT "\#error \"$file is not created for header inclusion\"\n";
  print $H_HDR_OUT "\n";

  my $modules;
  foreach my $umid (keys(%$g_umids)) {
    $modules->{$g_umids->{$umid}->{mod}} = 1;
  }
  # Print out the external UMID's
  print $H_HDR_OUT "/*!\n";
  print $H_HDR_OUT "  \@page umids Message Router UMID Definitions\n";
  foreach my $mod (sort(keys(%$modules))) {
    print $H_HDR_OUT "  \@par ".uc($mod)."\n";
    print $H_HDR_OUT "  <table>\n";
    print $H_HDR_OUT "    <tr><th>UMID</th><th>Hex Value</th><th>Payload</th></tr>\n";
    foreach my $umid (sort(keys(%$g_umids))) {
      my $type = $g_umids->{$umid}->{type};
      if( $g_umids->{$umid}->{mod} eq $mod && !($type =~ /I$/) )
      {
        print $H_HDR_OUT "    <tr>";
        print $H_HDR_OUT "<td>$g_umids->{$umid}->{sym}</td>";
        print $H_HDR_OUT "<td>0x$g_umids->{$umid}->{umid_hex}</td>";
        print $H_HDR_OUT "<td>$g_umids->{$umid}->{struct}</td>";
        print $H_HDR_OUT "</tr>\n";
      }
    }
    print $H_HDR_OUT "  </table>\n";
    print $H_HDR_OUT "\n";
  }
  print $H_HDR_OUT "\n  \@ref int_umids\n";
  print $H_HDR_OUT "\n  \@ref all_umids\n";
  print $H_HDR_OUT "*/\n";
  print $H_HDR_OUT "\n";

  # Print out the internal UMID's
  print $H_HDR_OUT "/*!\n";
  print $H_HDR_OUT "  \@page int_umids Internal Message Router UMID Definitions\n";
  foreach my $mod (sort(keys(%$modules))) {
    print $H_HDR_OUT "  \@par ".uc($mod)."\n";
    print $H_HDR_OUT "  <table>\n";
    print $H_HDR_OUT "    <tr><th>UMID</th><th>Hex Value</th><th>Payload</th></tr>\n";
    foreach my $umid (sort(keys(%$g_umids))) {
      my $type = $g_umids->{$umid}->{type};
      if( $g_umids->{$umid}->{mod} eq $mod && $type =~ /I$/ )
      {
        print $H_HDR_OUT "    <tr>";
        print $H_HDR_OUT "<td>$g_umids->{$umid}->{sym}</td>";
        print $H_HDR_OUT "<td>0x$g_umids->{$umid}->{umid_hex}</td>";
        print $H_HDR_OUT "<td>$g_umids->{$umid}->{struct}</td>";
        print $H_HDR_OUT "</tr>\n";
      }
    }
    print $H_HDR_OUT "  </table>\n";
    print $H_HDR_OUT "\n";
  }
  print $H_HDR_OUT "  </table>\n";
  print $H_HDR_OUT "\n  \@ref umids\n";
  print $H_HDR_OUT "\n  \@ref all_umids\n";
  print $H_HDR_OUT "*/\n";
  print $H_HDR_OUT "\n";

  # Create doxygen comments to create a table of links for the UMID's
  print $H_HDR_OUT "/*!\n";
  print $H_HDR_OUT "  \@page all_umids All MSGR UMID's (sorted by number)\n";
  print $H_HDR_OUT "  <table>\n";
  print $H_HDR_OUT "    <tr><th>Hex Value</th><th>UMID</th><th>Payload</th></tr>\n";
  foreach my $umid (sort(keys(%$g_umids)))
  {
    print $H_HDR_OUT "    <tr>";
    print $H_HDR_OUT "<td>0x$g_umids->{$umid}->{umid_hex}</td>";
    print $H_HDR_OUT "<td>$g_umids->{$umid}->{sym}</td>";
    print $H_HDR_OUT "<td>$g_umids->{$umid}->{struct}</td>";
    print $H_HDR_OUT "</tr>\n";
  }
  print $H_HDR_OUT "  </table>\n";
  print $H_HDR_OUT "\n  \@ref umids\n";
  print $H_HDR_OUT "\n  \@ref int_umids\n";
  print $H_HDR_OUT "*/\n";
  print $H_HDR_OUT "\n";

  # End the header def
  print $H_HDR_OUT "#endif /* $hdr_def */\n";
  print $H_HDR_OUT "\n";

  # Close the header file
  close( $H_HDR_OUT );
}


# ------------------------------------------------------------------------
# Name       : print_source_file()
# Description: Prints out the auto-generated source file
# Parameters : $path - path to output directory
#              $file - name of file to write (w/o path)
# Returns    : none
# Notes      : 
# ------------------------------------------------------------------------
sub print_source_file($$)
{
  my $path  = shift;   # Path to output directory
  my $file  = shift;   # Name of file to write output to

  my $src_heading = <<DONE;
/*!
  \@file   $file

  \@brief Auto-generated file containing UMID mapping tables

  \@detail
  This file is AUTO-GENERATED.  Do not modify!
  This auto-generated header file declares the message router table mapping
  UMIDs to their string equivalent

*/

/*===========================================================================

  Copyright (c) $currentyear QUALCOMM Incorporated. All Rights Reserved

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

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

DONE

  # Open the output source file
  my $H_OUT;
  my $outfile = $path."/".$file;
  open( $H_OUT, ">$outfile" ) || die "Unable to open output file: $outfile";

  # Spit out the heading
  print $H_OUT $src_heading;

  # Error checking for the header file
  print $H_OUT "#ifndef MSGR_UMID_GEN\n";
  print $H_OUT "#error \"$file may only be included from msgr_umid.c\"\n";
  print $H_OUT "#endif /* MSGR_UMID_GEN */\n";
  print $H_OUT "\n";

  # Macro to define UMID table entries according to TF_MSCGEN or TF_UMID_NAMES
  print $H_OUT "/* \@brief Populate table entries based on flags set */\n";
  print $H_OUT "#ifdef TF_MSCGEN\n";
  print $H_OUT "  #ifndef TF_UMID_NAMES\n";
  print $H_OUT "    #error \"Incorrect configuration - TF_UMID_NAMES must be defined\"\n";
  print $H_OUT "  #else\n";
  print $H_OUT "    /* TF_MSCGEN & TF_UMID_NAMES is defined, so define complete table */\n";
  print $H_OUT "    #define MSGR_UMID_TABLE_ENTRY(hex_id, name, func_ptr) ";
  print $H_OUT "{ hex_id, name, func_ptr }\n";
  print $H_OUT "  #endif\n";
  print $H_OUT "#elif defined TF_UMID_NAMES\n";
  print $H_OUT "  /* Only TF_UMID_NAMES is defined, so define UMID names only */\n ";
  print $H_OUT "  #define MSGR_UMID_TABLE_ENTRY(hex_id, name, func_ptr) ";
  print $H_OUT "{ hex_id, name, NULL }\n";
  print $H_OUT "#else\n";
  print $H_OUT "  /* TF_MSCGEN/TF_UMID_NAMES are not defined, so no definitions required\n";
  print $H_OUT "     This case is for final release*/\n";
  print $H_OUT "#endif\n";
  print $H_OUT "\n";

  # Create prototypes for the string functions
  print $H_OUT "#ifdef TF_MSCGEN\n";
  foreach my $umid (sort(keys(%$g_umids))) {
    if( $g_umids->{$umid}->{func} ne "NULL" ) {
      print $H_OUT "/* Returns a string representing $g_umids->{$umid}->{sym} */\n";
      print $H_OUT "const char * ".$g_umids->{$umid}->{func};
      print $H_OUT "(msgr_hdr_struct_type*, const char *, void*);\n";
      print $H_OUT "\n";
    }
  }
  print $H_OUT "#endif /* TF_MSCGEN */\n";
  print $H_OUT "\n";

  # Create the UMID table
  print $H_OUT "/*! \@brief Table mapping UMID to strings */\n";
  print $H_OUT "static const msgr_umid_name_entry_s msgr_umid_name_table[] = {\n";
  print $H_OUT "#if ((!defined TF_MSCGEN) && (!defined TF_UMID_NAMES))\n";
  print $H_OUT "  /* TF_MSCGEN/TF_UMID_NAMES are not defined, so no ";
  print $H_OUT "definitions */\n";
  print $H_OUT "  { 0x00000000, \"\", NULL },\n";
  print $H_OUT "#else\n";
  foreach my $umid (sort(keys(%$g_umids))) {
    print $H_OUT "  /* $g_umids->{$umid}->{sym}, payload: $g_umids->{$umid}->{struct} */\n";
    print $H_OUT "  MSGR_UMID_TABLE_ENTRY ( 0x".$g_umids->{$umid}->{umid_hex}.",";
    print $H_OUT " \"".$g_umids->{$umid}->{sym}."\", ".$g_umids->{$umid}->{func}." ),\n";
    print $H_OUT "\n";
  }
  print $H_OUT "#endif /* ((!defined TF_MSCGEN) && (!defined TF_UMID_NAMES)) */\n";
  print $H_OUT "};\n";
  print $H_OUT "\n";

  # Create the TECH/MOD table
  print $H_OUT "/*! \@brief Table mapping TECH/MODULE to strings */\n";
  print $H_OUT "static const msgr_module_name_entry_s  msgr_module_name_table[] = {\n";
  print $H_OUT "#if ((!defined TF_MSCGEN) && (!defined TF_UMID_NAMES))\n";
  print $H_OUT "  /* TF_MSCGEN & TF_UMID_NAMES are not defined, so no ";
  print $H_OUT "definitions */\n";
  print $H_OUT "  { 0x0000, \"\" },\n";
  print $H_OUT "#else\n";
  foreach my $tech_mod (sort(keys(%$g_tech_mods))) {
    my $sym = $g_tech_mods->{$tech_mod}->{sym};
    $sym =~ s/^MSGR_//;
    print $H_OUT "  /* $sym */\n";
    print $H_OUT "  { 0x".$g_tech_mods->{$tech_mod}->{val_hex}.", ";
    print $H_OUT "\"".$g_tech_mods->{$tech_mod}->{sym}."\" },\n";
    print $H_OUT "\n";
  }
  print $H_OUT "#endif /* ((!defined TF_MSCGEN) && (!defined TF_UMID_NAMES)) */\n";
  print $H_OUT "};\n\n";

  print_jump_table($H_OUT);

  # Close the file
  close( $H_OUT );
}


# ------------------------------------------------------------------------
# Name       : print_jump_table()
# Description: Prints out the message router jump tables
# Parameters : $fh - file handle
# Returns    : none
# Notes      : 
# ------------------------------------------------------------------------
sub print_jump_table($)
{
  my $fh   = shift;  # File handle to write the output to

  my $str;
  my $num_msgs = 0;
  my $total_mods;

  my $techs;

  for( my $i = 0; $i < @opt_techs; $i += 2 ) {
    my $name = @opt_techs[$i+0];
    my $val  = @opt_techs[$i+1];

    $techs->{$name} = { tech => $val, max_mods => 0 };
  }

  my @types = ( "SPR",  "CMD",  "REQ", "RSP",  "IND",  "FLM",  "OTA", "DLM", "CNF", "TMR", 
                "CMDI", "REQI", "RSPI", "INDI", "CNFI", "TMRI", "INTI" );

  # below used to be used when the given types overlapped in msgr_umid.h
  # the overlapping types also need to then be removed from @types above
  my $trans = {
  #  "REQ" => "CMD",
  #  "OTA" => "FLM",
  };
  
  my $num_tech_mod_type  = 0; # To Compute Number of Unique Combinations of Tech/Mod/Type
  my $num_reg_nodes_slot = 0; # To Compute Number of Reg Nodes Slots Needed for Known UMIDS
  
  # slot_array - Used for Counting the Number of Unique UpperBits for a specific Tech/Mod/Type
  # The Values Are Treated as Boolean. So If its 1, Then a Unique Combination of 
  # Tech/Mod/Type/IdUpperNibble Exists in the System at Compile Time   
  my @slot_array = ( [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0] ); 
                                                          
  foreach my $tech (keys(%$techs)) {
    dprint("Tech = $tech\n");
    my $max_mod = -1;
    foreach my $tech_mod (sort(keys(%$g_tech_mods))) {
      next if( $g_tech_mods->{$tech_mod}->{tech_val} ne $techs->{$tech}->{tech} );
      next if( $g_tech_mods->{$tech_mod}->{mod_val} eq 0xFF );
      if( $g_tech_mods->{$tech_mod}->{mod_val} > $max_mod ) {
        $max_mod = $g_tech_mods->{$tech_mod}->{mod_val};
      }
    }
    if( $max_mod >= 0 ) {
      $max_mod++;
      $max_mod = $max_mod + 2; # Provide 2 Extra Modules
      $techs->{$tech}->{max_mods} = $max_mod;
      $total_mods += $max_mod;

      print $fh "\#define MSGR_MAX_".uc($tech)."_MODULES ".$max_mod."\n";
      print $fh "static uint16 msgr_".lc($tech)."_jump_table";
      print $fh "[MSGR_MAX_".uc($tech)."_MODULES][MSGR_NUM_TYPES] = \n";

      my %modules;
      foreach my $umid (sort(keys(%$g_umids))) {
        next if( $g_umids->{$umid}->{tech} ne $tech );
        my $module = $g_umids->{$umid}->{mod};
        my $mod_val = ($g_umids->{$umid}->{umid_val} >> 16) & 0xFF;
        %modules->{$mod_val} = $module;
      }

      print $fh "{ /*";
      foreach my $type (@types) {
        printf $fh ( "%6s, ", $type);
      }
      print $fh " */\n";
      for( my $i = 0; $i < $max_mod; $i++ ) {
        if( !exists( %modules->{$i} ) ) {
          $str = sprintf( "  /* %10s (0x%04x)*/\n", "Unused", $i );
          print $fh $str;
          print $fh "  { ";
          foreach my $unused (@types) {
            $str = sprintf( "0x%04x, ", 0xFFFF ); # Initialize to Default Value of 0xFFFF,
                                                  # Helps save some cycles at Initialization                   
            print $fh $str;
          }
          print $fh "},\n";
          next;
        }

        my $module = %modules->{$i};
        $str = sprintf( "  /* %10s (0x%04x) */\n", $module, $i );
        print $fh $str;
        print $fh "  { ";
        foreach my $type (@types) {

          my $i = 0;
          my $max_id = -1;
          for ( $i = 0 ; $i < 16 ; $i++){
            $slot_array[$i] = 0; # Re-Initialize the Slot Array
          }
          foreach my $umid (sort(keys(%$g_umids))) {
            if( $g_umids->{$umid}->{tech} eq $tech &&
                $g_umids->{$umid}->{mod}  eq $module &&
                ( $g_umids->{$umid}->{type} eq $type ||
                  $trans->{$g_umids->{$umid}->{type}} eq $type ) ) {
              my $umid_val = $g_umids->{$umid}->{umid_val};  
              #Parse the Upper Nibble of the ID
              $umid_val = ($umid_val & 0xF0) >> 4;
              # Set it, confirming it exists  
              $slot_array[$umid_val] = 1;
              if( $g_umids->{$umid}->{idnum} > $max_id ) {
                $max_id = $g_umids->{$umid}->{idnum};
              }
            }
          }
          for ( $i = 0 ; $i < 16 ; $i++){
            # Count the Number of Unique Upper Bits Set
            $num_reg_nodes_slot += $slot_array[$i];
          }
          $max_id++;
          
          if( $max_id != 0 ) {
            # If Non-Zero, Tech/Mod/Type Combination Exists
            $num_tech_mod_type = $num_tech_mod_type + 1; 
          $str = sprintf( "0x%04x, ", $max_id );
          } else {
            # Else, Initialize it to Default Value
            $str = sprintf( "0x%04x, ", 0xFFFF );
          }
          print $fh $str;
          $num_msgs += $max_id;
        }
        print $fh "},\n";
      }
      print $fh "};\n";
      print $fh "\n\n";
    }
  }

  my $max_techs = 0;
  foreach my $t (keys(%$techs)) {
    $max_techs = $techs->{$t}->{tech} if( $techs->{$t}->{tech} > $max_techs );
  }
  $max_techs++;
  print $fh "\#define MSGR_NUM_TECHS ".$max_techs."\n";
  print $fh "static msgr_tech_jump_entry_s msgr_gen_jump_table[MSGR_NUM_TECHS] = \n";
  print $fh "{\n";
  # Iterate through the tech values
  for( my $i = 0; $i < $max_techs; $i++ ) {
    my $tech;
    # Find the tech entry that matches
    foreach my $t (keys(%$techs)) {
      if( $techs->{$t}->{tech} eq $i ) {
        $tech = $t;
        last;
      }
    }
    if( defined( $tech ) && ($techs->{$tech}->{max_mods} > 0) ) {
      print $fh "  { (uint16*)msgr_".lc($tech)."_jump_table, ";
      print $fh $techs->{$tech}->{max_mods}." },\n";
    } else {
      print $fh "  { NULL, 0 },/* $tech (no messages) */\n";
    }
  }

  print $fh "};\n\n";
  print $fh "\#define MSGR_NUM_TOTAL_MSGS $num_msgs\n";
  print $fh "\n\n";
  print $fh "/* Defines Number of Unique Combinations of Tech/Mod/Type in the system \n";
  print $fh "   when MSGR gets compiled */\n\n";
  print $fh "\#define MSGR_NUM_TECH_MOD_TYPE $num_tech_mod_type\n\n";
  print $fh "/* Defines Number of Reg Nodes Slot Needed for Known UMIDS \n";
  print $fh "   when MSGR gets compiled */\n\n";
  print $fh "\#define MSGR_NUM_REGULAR_SLOTS $num_reg_nodes_slot\n\n";


  print $fh "/* The size of the top-level jump table,\n";
  print $fh "   which points to the correct tech-specific table.*/\n";
  print $fh "#define MSGR_JUMP_TABLE_SIZE  (MSGR_NUM_TECHS * sizeof(msgr_tech_jump_entry_s))\n\n";

  print $fh "/* The total size of all the tech-specific tables.*/\n";
  print $fh "#define MSGR_TECH_JUMP_TABLE_SIZE  ($total_mods * MSGR_NUM_TYPES * sizeof(uint16))\n\n";
}


# ------------------------------------------------------------------------
# Name       : print_xml_file()
# Description: Prints out the message definitions in XML format
# Parameters : $msgs - message definition hash to output in XML format
#            : $modules - module definition has to output in XML format
#            : $file - name of output file
# Returns    : none
# Notes      : 
# ------------------------------------------------------------------------
sub print_xml_file($$$)
{
  my  $msgs    = shift;  # Message definition hash
  my  $modules = shift;  # Module definition hash
  my  $file    = shift;  # Name of output file

  my $hash = {
    umids => $msgs,
    modules => $modules,
  };

  my $output = XMLout($hash, 
                      KeepRoot => 1, 
                      KeyAttr => { umids => 'key', modules => 'key' });
  my $fh;
  open( $fh, ">$file" ) || die "unable to open output file: $file";
  print $fh $output;
  close( $fh );
}


# ------------------------------------------------------------------------
# Name       : load_xml_file()
# Description: Loads in the message definitions in XML format
# Parameters : $file - name of input file
# Returns    : none
# Notes      : modifies $g_umids
# ------------------------------------------------------------------------
sub load_xml_file($)
{
  my  $file  = shift;    # Name of input file

  my $fh;
  my $buffer;

  dprint( "Opening xml: $file\n" );
  open( $fh, $file ) || die "unable to open input file: $file";
  while( my $line = <$fh> ) {
    $buffer .= $line;
  }
  close( $fh );

  my $hash = XMLin($buffer, 
                   KeyAttr => { umids => 'key', modules => 'key' },
                   ForceArray => 1);

  if( $hash->{umids} ne "" ) {
    foreach my $key (keys(%{$hash->{umids}})) {
      insert_umid_entry( $hash->{umids}->{$key} );
    }

    foreach my $key (keys(%{$hash->{modules}})) {
      insert_module_entry( $hash->{modules}->{$key} );
    }
  }
}

