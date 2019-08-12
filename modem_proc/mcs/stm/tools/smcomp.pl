#!/usr/bin/perl -w
##########################################################################
# ------------------------------------------------------------------------
# State Machine/Group Compiler
#
# Description:
#   This is the state machine compiler for use with the STM service.  For
#   full documentation, see the stm.c/h source code and examples/... directory.
#   A BNF grammar description of what is parsed by this compiler is given below.
#
#
# Copyright (c) 2006 - 2013 Qualcomm Technologies Incorporated.
# All Rights Reserved
# Qualcomm Confidential and Proprietary
#
# Export of this technology or software is regulated by the U.S. Government.
# Diversion contrary to U.S. law prohibited.
#
# All ideas, data and information contained in or disclosed by
# this document are confidential and proprietary information of
# Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
# By accepting this material the recipient agrees that this material
# and the information contained therein are held in confidence and in
# trust and will not be used, copied, reproduced in whole or in part,
# nor its contents revealed in any manner to others without the express
# written permission of Qualcomm Technologies Incorporated.
# ------------------------------------------------------------------------
##########################################################################
#
#                         EDIT HISTORY FOR MODULE
#
# This section contains comments describing changes made to the module.
# Notice that changes are listed in reverse chronological order.
#
# $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/stm/tools/smcomp.pl#1 $
#
# when       who     what, where, why
# --------   ---     -----------------------------------------------------
# 10/10/13   mn      Fix for LLVM compiler warnings.
# 03/21/12   ag      Added support for accepting TCB_PTR
# 06/30/10   hph/trc Be more picky about preprocessor line/file directives
# 04/24/09   trc     Appease lint w/ (stm_input_type) cast in input table
# 03/29/07   trc     Add INT signal abort handler
# 01/17/07   trc     Fix bug when delayed internal command queue == NULL
# 01/11/07   bt      Added delayed internal command queue
# 10/15/06   trc     Initial version
#
##########################################################################

##########################################################################
#
#  BNF Grammar for STM language:
#
#  <state machine group> ::= GROUP <group name> <{group-block}> ...
#
#  <{group-block}> ::= ENTRY                 <function name>;                |
#                      EXIT                  <function name>;                |
#                      WAIT                  <function name>;                |
#                      TCB                   <pointer name>;                 |
#                      INT_CMD_SIG           <signal name>;                  |
#                      INT_CMD_QUEUE         <pointer name>;                 |
#                      DELAYED_INT_CMD_QUEUE <pointer name>;                 |
#                      GLOBAL_GROUP          <group name>;                   |
#                      DEBUG_HOOK            <function name>;                |
#                      HEAP_CTOR             <function name>;                |
#                      HEAP_DTOR             <function name>;                |
#                      MALLOC                <function name>;                |
#                      FREE                  <function name>;                |
#                      SIGNAL_MAPPER         <function name> <{mapper-block}>|
#                      STATE_MACHINE         <sm name>       <{sm-block}> ...
#
#  <{mapper-block}> ::= <signal name>; ...
#
#  <{sm-block}> ::= ENTRY          <function name>;                |
#                   EXIT           <function name>;                |
#                   INITIAL_STATE  <state name>;                   |
#                   START_ACTIVE   <TRUE|FALSE>;                   |
#                   STATE          <state name> <{state-block}> ...
#
#  <{state-block}> ::= ENTRY   <function name>;  |
#                      EXIT    <function name>;  |
#                      INPUTS  <{inputs-block}>
#
#  <{inputs-block}> ::= <input name> [<function name>]; ...
#
##########################################################################

##########################################################################
#
#                            Imported Modules
#
##########################################################################

# Add a little negative reinforcement here to keep things tidy
use strict;
use warnings;

# Allow us to get the current year for copyright generation
use Time::localtime;

# Pull in the cmd line options processor, allow config option bundling
use Getopt::Long qw(:config bundling);


##########################################################################
#
#                            Global Variables
#
##########################################################################

# Global debug level to set verbosity level
my $debuglevel = 1;

# Get the current year, for copyright verbiage
my $currentyear = localtime->year() + 1900;

# Global 'cleanup' variable for INT signal handler
my @files_created = ();


##########################################################################
#
#                              Subroutines
#
##########################################################################

# ------------------------------------------------------------------------
# Name       :  Debug
# Description:  Selectively print debugging messages based upon global
#               debuglevel and the level of the particular message.  If the
#               level of the message is < 0, this is considered fatal, so
#               perform an exit(-msglevel) and terminate.
# Parameters :
# Returns    :
# Notes      :
# ------------------------------------------------------------------------
sub Debug($$) {
  my $msglevel = shift;
  my $msg      = shift;
  print STDERR "$msg\n" unless ( $debuglevel <= $msglevel );
  exit( -$msglevel )    unless ( $msglevel >= 0 );
}

# ------------------------------------------------------------------------
# Name       :
# Description:
# Parameters :
# Returns    :
# Notes      :
# ------------------------------------------------------------------------
sub TokenizeFile($$) {
  my ( $infilename, $tokendataref ) = @_;

  # Try slurping in the file we were given
  open( my $fh, "<$infilename" )
    || Debug( -1, "Error, can't open file: $infilename" );
  my @infile = <$fh>;
  close( $fh );
  Debug( 2,
         "Processing " . scalar( @infile ) . " lines from file: $infilename" );

  # Spin through the file and tokenize it (keeping track of filename/line #)
  my $incomment = 0;
  my $filename  = $infilename;
  my $linenum   = 1;
  for ( ; $_ = shift( @infile ) ; $linenum++ ) {

    #
    # Strip/skip any comments - the preprocessor should have done this, but
    # we'll just be redundant in-case someone is running this on a file
    # that doesn't need preprocessing (for #include, #ifdef, ...) and wants
    # to check the output or verify correct input syntax.
    #

    # If inside a multi-line comment
    if ( $incomment ) {

      # ... and no end of comment in sight, proceed to next
      next unless ( m|\*/| );

      # Otherwise, strip off the commented portion and continue processing
      s|^.*?\*/(.*)$|$1|;

      # We are now out of the multiline comment
      $incomment = 0;
    }

    # Remove C-style single-line-bound comments: /* xxx */
    s|/\*.*?\*/||g;

    # Check if beginning of a multi-line comment
    if ( m|/\*.*[^*][^/]| ) {

      # Nuke the comment part, but keep the preceding text
      s|/\*.*$||;

      # Keep track of the fact that we are inside a multiline comment
      $incomment = 1;
    }

    # Skip purely blank lines, now that all the comments have been stripped
    next if ( m/^\s*$/ );

    #
    # Handle the preprocessor insertions  '#[line] <line #> "filename"'
    #
    if ( my ( $ppins ) = $_ =~ m/^\s*#\s*(.*)$/ ) {

      Debug( 4, "Preprocessor annotation:  $ppins" );

      # GNU cpp just does '# <number> <filename>', ARM does '#line <number> <filename>'
      # be selective in the regex, such that we only match a non-angle bracketed filename
      # and not just a path or built-in
      if ( my ( $line, $file ) =
           $ppins =~ m/^(?:line)?\s*(\d+)\s+\"([^<].+[^>\\\/])\"/ )
      {
        # line # specified is the NEXT line in the file, so predecrement
        $linenum  = $line - 1;
        # Only grab the basename of the file, no need for paths
        # and the possible slash/backslash complications
        ( $filename ) = $file =~  m|([^/\\]+)$|;
        Debug( 3, "Using preprocessor line/file info:  $line/$filename" );
      } elsif ($ppins =~ m/^(?:line)?\s*(\d+)\s+\"(.+)\"/ ) {
        # Minor debug info if we hit a preprocessor line/file directive we can't use
        Debug( 1 , "$filename:$linenum:Warning, preprocessor directive filename '$2' ignored.\n" );
      } else {
        # Complain/warn if we hit a preprocessor directive we CAN'T handle
        Debug( 0 , "$filename:$linenum:Warning, preprocessor directive '$ppins' ignored.\n   " .
                   "Please use C preprocessor on input file prior to compilation" );
      }

      # Nothing more to do with this line, skip to next
      next;
    }

    # Remove C++ -style single-line-bound comments: //.* until EOL$
    # Doing this here after preprocessor directives to prevent operating on paths
    s|//.*$||;

    # Pad any delimiters with whitespace so we can split them apart from
    # any adjacent words
    s/([,;{}\[\]])/ $1 /g;

    # Save the tokens and their file/line # info
    # File/line # info is cheesy, but it's needed so the lexer can report
    # roughly where syntax errors occur.
    foreach ( split ) {
      push( @{ $tokendataref }, "$_ $filename $linenum" );
    }

  }

  # Push an 'end of file' token so the lexer knows when there's no more
  push( @{ $tokendataref }, "_EOT_ $filename $linenum" );

  Debug( 2, "Extracted " . scalar( @$tokendataref ) . " tokens" );

  # Just return - lexer will catch/flag any syntax errors
}

# ------------------------------------------------------------------------
# Name       :
# Description:
# Parameters :
# Returns    :
# Notes      :
# ------------------------------------------------------------------------
sub ExtractToken($$) {
  my ( $tokendataref, $index ) = @_;
  my $token = "";

  # Check num of tokens left
  if ( scalar( @$tokendataref ) > $index ) {
    $token = ( split( / /, $$tokendataref[ $index ] ) )[ 0 ];
  }

  return ( $token );
}

# ------------------------------------------------------------------------
# Name       :
# Description:
# Parameters :
# Returns    :
# Notes      :
# ------------------------------------------------------------------------
sub ExtractFilename($$) {
  my ( $tokendataref, $index ) = @_;
  my $filename = "";

  # Check num of tokens left
  if ( scalar( @$tokendataref ) > $index ) {
    $filename = ( split( / /, $$tokendataref[ $index ] ) )[ 1 ];
  }

  return ( $filename );
}

# ------------------------------------------------------------------------
# Name       :
# Description:
# Parameters :
# Returns    :
# Notes      :
# ------------------------------------------------------------------------
sub ExtractLineNumber($$) {
  my ( $tokendataref, $index ) = @_;
  my $linenum = "";

  # Check num of tokens left
  if ( scalar( @$tokendataref ) > $index ) {
    $linenum = ( split( / /, $$tokendataref[ $index ] ) )[ 2 ];
  }

  return ( $linenum );
}

# ------------------------------------------------------------------------
# Name       :
# Description:
# Parameters :
# Returns    :
# Notes      :
# ------------------------------------------------------------------------
sub ConsumeTokens($$) {
  my ( $tokendataref, $num ) = @_;

  # Consume $num tokens or as many as are left (ie. all of them)
  if ( scalar( @$tokendataref ) >= $num ) {
    splice( @$tokendataref, 0, $num );
  }
  else {
    @$tokendataref = ();
  }
}

# ------------------------------------------------------------------------
# Name       :
# Description:
# Parameters :
# Returns    :
# Notes      :
# ------------------------------------------------------------------------
sub ErrorMsgTok($$) {
  my ( $prefixtxt, $tokenstr ) = @_;
  my $msg;

  my ( $token, $filename, $linenum ) = split( / /, $tokenstr );
  if ( $token eq "_EOT_" ) {
    $msg = "$filename:$linenum:$prefixtxt: ";
  }
  else {
    $msg = "$filename:$linenum:$prefixtxt near '$token': ";
  }

  return ( $msg );
}

# ------------------------------------------------------------------------
# Name       :
# Description:
# Parameters :
# Returns    :
# Notes      :
# ------------------------------------------------------------------------
sub ErrorMsgTokIndex($$$) {
  my ( $prefixtxt, $tokendataref, $index ) = @_;
  my $msg = "";

  # Check num of tokens left
  if ( scalar( @$tokendataref ) > $index ) {
    my ( $token, $filename, $linenum ) = split( / /, $$tokendataref[ $index ] );
    if ( $token eq "_EOT_" ) {
      $msg = "$filename:$linenum:$prefixtxt: ";
    }
    else {
      $msg = "$filename:$linenum:$prefixtxt near '$token': ";
    }
  }

  return ( $msg );
}

# ------------------------------------------------------------------------
# Name       :
# Description:
# Parameters :
# Returns    :
# Notes      :
# ------------------------------------------------------------------------
sub IsEOT($) {
  my $tokendataref = shift;
  my $result       = 0;

  # Check if we are at the _EOT_ marker or not in the tokenstream
  if ( ExtractToken( $tokendataref, 0 ) eq "_EOT_" ) {
    $result = 1;
  }

  return ( $result );
}

# ------------------------------------------------------------------------
# Name       :
# Description:
# Parameters :
# Returns    :
# Notes      :
# ------------------------------------------------------------------------
# Parameters: keyword array ref, token data ref, base hash ref, error text ref
# Return code: -1 on match but error, 0 on no match, 1 on success
#
sub ParseKeywordAssignment($$$$) {
  my ( $keywordsref, $tokendataref, $basehashref, $errortextref ) = @_;
  my $status;

  # Check that we were passed an existing hash reference (internal error)
  if ( !defined $basehashref ) {
    die( "Undefined hash value passed in for assignment" );
  }

  do {
    {

      # Check for keyword match, bail if not
      my $keyword = ExtractToken( $tokendataref, 0 );
      if ( 0 == grep $keyword eq $_, @$keywordsref ) {
        $$errortextref =
          "No (" . join( "|", @$keywordsref ) . ") assignment found";
        $status = 0;
        last;
      }

      # Check for valid argument
      my $argument = ExtractToken( $tokendataref, 1 );
      if ( $argument !~ m/^(\w+)$/ ) {
        $status = -1;
        $$errortextref =
          ErrorMsgTokIndex( "Syntax error", $tokendataref, 1 )
          . "Illegal argument";
        last;
      }

      # Check for semicolon terminator
      if ( ExtractToken( $tokendataref, 2 ) ne ";" ) {
        $status = -1;
        $$errortextref =
          ErrorMsgTokIndex( "Syntax error", $tokendataref, 1 )
          . "Expected trailing semicolon";
        last;
      }

      # Check that we can save off to the hash without duplication
      if ( defined $basehashref->{ $keyword } ) {
        $status = -1;
        $$errortextref =
          ErrorMsgTokIndex( "Syntax error", $tokendataref, 0 )
          . "Duplicate $keyword declaration inside same block";
        last;
      }

      # Eat the tokens we just used
      ConsumeTokens( $tokendataref, 3 );

      # Add the keyword to the 'ordered keys' array
      push( @{ $basehashref->{ _KEYS } }, $keyword );

      # Save the value into the hash and declare success
      $basehashref->{ $keyword } = $argument;
      $status = 1;

      Debug( 2, "Parsed: Keyword Assignment: $keyword = $argument" );

    }
  } while ( 0 );

  # Return our status
  return ( $status );
}

# ------------------------------------------------------------------------
# Name       :
# Description:
# Parameters :
# Returns    :
# Notes      :
# ------------------------------------------------------------------------
# Parameters: token data ref, base hash ref, error text ref
# Return code: -1 on match but error, 0 on no match, 1 on success
#
sub ParseGenericName($$$) {
  my ( $tokendataref, $basehashref, $errortextref ) = @_;
  my $status;

  # Check that we were passed an existing hash reference (internal error)
  if ( !defined $basehashref ) {
    die( "Undefined hash value passed in for assignment" );
  }

  do {
    {

      # The first token should be the variable
      my $name = ExtractToken( $tokendataref, 0 );

      # Check for semicolon terminator
      if ( ExtractToken( $tokendataref, 1 ) ne ";" ) {

        # Bail out now with a non-match, since we can't
        # declare a syntax error inside a generic name (due to no
        # precise list of keywords to compare against)
        $$errortextref = "No generic name found";
        $status        = 0;
        last;
      }

      # Check that we can save off to the hash without duplication
      if ( defined $basehashref->{ $name } ) {
        $status = -1;
        $$errortextref =
          ErrorMsgTokIndex( "Syntax error", $tokendataref, 0 )
          . "Duplicate $name declaration inside same block";
        last;
      }

      # Eat the tokens we just used
      ConsumeTokens( $tokendataref, 2 );

      # Add the variable to the 'ordered keys' array
      push( @{ $basehashref->{ _KEYS } }, $name );

      # Save the empty scalar value into the hash and declare success
      $basehashref->{ $name } = "";
      $status = 1;

      Debug( 2, "Parsed: Generic Name: $name" );

    }
  } while ( 0 );

  # Return our status
  return ( $status );
}

# ------------------------------------------------------------------------
# Name       :
# Description:
# Parameters :
# Returns    :
# Notes      :
# ------------------------------------------------------------------------
# Parameters: token data ref, base hash ref, error text ref
# Return code: -1 on match but error, 0 on no match, 1 on success
#
sub ParseGenericAssignment($$$) {
  my ( $tokendataref, $basehashref, $errortextref ) = @_;
  my $status;

  # Check that we were passed an existing hash reference (internal error)
  if ( !defined $basehashref ) {
    die( "Undefined hash value passed in for assignment" );
  }

  do {
    {

      # The first token should be the variable
      my $variable = ExtractToken( $tokendataref, 0 );

      # The second token should be the assigned value

      # Check for valid argument or semicolon
      my $argument = ExtractToken( $tokendataref, 1 );
      if ( $argument !~ m/^(\w+)$/ ) {

        # Bail out now with a non-match, since we can't
        # declare a syntax error inside a generic assigment (due to no
        # precise list of keywords to compare against)
        $$errortextref = "No generic assignment found";
        $status        = 0;
        last;
      }

      # Check for semicolon terminator (if not given above)
      if ( ExtractToken( $tokendataref, 2 ) ne ";" ) {

        # Bail out now with a non-match, since we can't
        # declare a syntax error inside a generic assigment (due to no
        # precise list of keywords to compare against)
        $$errortextref = "No generic assignment found";
        $status        = 0;
        last;
      }

      # Check that we can save off to the hash without duplication
      if ( defined $basehashref->{ $variable } ) {
        $status = -1;
        $$errortextref =
          ErrorMsgTokIndex( "Syntax error", $tokendataref, 0 )
          . "Duplicate $variable assignment inside same block";
        last;
      }

      # Eat the tokens we just used
      ConsumeTokens( $tokendataref, 3 );

      # Add the variable to the 'ordered keys' array
      push( @{ $basehashref->{ _KEYS } }, $variable );

      # Save the value into the hash and declare success
      $basehashref->{ $variable } = $argument;
      $status = 1;

      Debug( 2, "Parsed: Generic Assignment: $variable = $argument" );

    }
  } while ( 0 );

  # Return our status
  return ( $status );
}

# ------------------------------------------------------------------------
# Name       :
# Description:
# Parameters :
# Returns    :
# Notes      :
# ------------------------------------------------------------------------
# Parameters: keyword array ref, token data ref, output hash ref, output hash ref,error text ref
# Return code: -1 on match but error, 0 on no match, 1 on success
#
sub ParseBlock($$$$$) {
  my ( $keywordsref, $tokendataref, $basehashref, $outhashref, $errortextref ) =
    @_;
  my $status;

  # Check that we were passed an existing hash reference (internal error)
  if ( !defined $basehashref ) {
    die( "Undefined hash value passed in for assignment" );
  }

  do {
    {

      # Check for keyword match, bail if not
      my $keyword = ExtractToken( $tokendataref, 0 );
      if ( 0 == grep $keyword eq $_, @$keywordsref ) {
        $$errortextref = "No (" . join( "|", @$keywordsref ) . ") block found";
        $status = 0;
        last;
      }

      # Check for block begin
      if ( ExtractToken( $tokendataref, 1 ) ne "{" ) {
        $status = -1;
        $$errortextref =
          ErrorMsgTokIndex( "Syntax error", $tokendataref, 1 )
          . "Expected beginning of block '{'";
      }

      # Check that we can save off to the hash without duplication
      if ( defined $basehashref->{ $keyword } ) {
        $status = -1;
        $$errortextref =
          ErrorMsgTokIndex( "Syntax error", $tokendataref, 1 )
          . "Duplicate $keyword block";
        last;
      }

      # Eat the tokens we just used
      ConsumeTokens( $tokendataref, 2 );

      # Create the hash ref,
      # copy the hash ref outward, and declare success
      $basehashref->{ $keyword } = {};
      $$outhashref               = $basehashref->{ $keyword };
      $status                    = 1;

      Debug( 2, "Parsed: Unnamed Block: $keyword" );

    }
  } while ( 0 );

  # Return our status
  return ( $status );
}

# ------------------------------------------------------------------------
# Name       :
# Description:
# Parameters :
# Returns    :
# Notes      :
# ------------------------------------------------------------------------
# Parameters: keyword array ref, token data ref, output hash ref,
#             output hash ref,enforce unique,error text ref
# Return code: -1 on match but error, 0 on no match, 1 on success
#
sub ParseNamedBlock($$$$$$$) {
  my (
       $keywordsref, $tokendataref,   $basehashref, $outhashref,
       $nameref,     $enforce_unique, $errortextref
       ) = @_;
  my $status;

  # Check that we were passed an existing hash reference (internal error)
  if ( !defined $basehashref ) {
    die( "Undefined hash value passed in for assignment" );
  }

  do {
    {

      # Check for keyword match, bail if not
      my $keyword = ExtractToken( $tokendataref, 0 );
      if ( 0 == grep $keyword eq $_, @$keywordsref ) {
        $status = 0;
        $$errortextref = "No (" . join( "|", @$keywordsref ) . ") block found";
        last;
      }

      # Check for valid argument
      my $argument = ExtractToken( $tokendataref, 1 );
      if ( $argument !~ m/^(\w+)$/ ) {
        $status = -1;
        $$errortextref =
          ErrorMsgTokIndex( "Syntax error", $tokendataref, 1 )
          . "Illegal argument";
        last;
      }

      # Check for block begin
      if ( ExtractToken( $tokendataref, 2 ) ne "{" ) {
        $status = -1;
        $$errortextref =
          ErrorMsgTokIndex( "Syntax error", $tokendataref, 1 )
          . "Expected beginning of block '{'";
      }

      # Save the name of the block
      $$nameref = $argument;

      # Check if uniqueness is enforced (only one of these block types per hash)
      if ( $enforce_unique
           && defined $basehashref->{ $keyword } )
      {
        $status = -1;
        $$errortextref =
          ErrorMsgTokIndex( "Syntax error", $tokendataref, 0 )
          . "Only one $keyword block allowed";
        last;
      }

      # Check that we can save off to the hash without duplication
      if ( defined $basehashref->{ $keyword }->{ $argument } ) {
        $status = -1;
        $$errortextref =
          ErrorMsgTokIndex( "Syntax error", $tokendataref, 1 )
          . "Duplicate $keyword block: $argument";
        last;
      }

      # Eat the tokens we just used
      ConsumeTokens( $tokendataref, 3 );

      # Add the argument to the 'ordered keys' array
      push( @{ $basehashref->{ $keyword }->{ _KEYS } }, $argument );

      # Create the hash ref initialized with some source file info,
      # copy the hash ref outward, and declare success
      $basehashref->{ $keyword }->{ $argument } = {
                            FILENAME   => ExtractFilename( $tokendataref,   0 ),
                            LINENUMBER => ExtractLineNumber( $tokendataref, 0 )
                            };
      $$outhashref = $basehashref->{ $keyword }->{ $argument };
      $status      = 1;

      Debug( 2, "Parsed: Named Block: $keyword = $argument" );

    }
  } while ( 0 );

  # Return our status
  return ( $status );
}

# ------------------------------------------------------------------------
# Name       :
# Description:
# Parameters :
# Returns    :
# Notes      :
# ------------------------------------------------------------------------
# Parameters: token data ref
# Return code: 0 on no match, 1 on success
#
sub ParseEndBlock($$) {
  my ( $tokendataref, $errortextref ) = @_;
  my $status;

  do {
    {

      # Check for keyword match, bail if not
      my $keyword = ExtractToken( $tokendataref, 0 );
      my $tokencount = 1;
      if ( $keyword ne "}" ) {
        $status        = 0;
        $$errortextref = "No end of block found";
        last;
      }

      # Check for optional semicolon terminator
      if ( ExtractToken( $tokendataref, 1 ) eq ";" ) {
        $tokencount++;
      }

      # Eat the tokens we just used
      ConsumeTokens( $tokendataref, $tokencount );

      # Declare success
      $status = 1;

      Debug( 2, "Parsed: End of block" );

    }
  } while ( 0 );

  # Return our status
  return ( $status );
}

# ------------------------------------------------------------------------
# Name       :
# Description:
# Parameters :
# Returns    :
# Notes      :
# ------------------------------------------------------------------------
sub ParseInputs($$$) {
  my ( $tokendataref, $inputsref, $errortextref ) = @_;
  my ( $status, $inputshashref, $blocktoken );

  do {
    {

      # We're looking for start of inputs block
      $status = ParseBlock( [ "INPUTS" ],    $tokendataref, $inputsref,
                            \$inputshashref, $errortextref );

      # Bail upon unsuccessful parse
      last if ( $status != 1 );

      # Save the start of block token for some error reporting
      $blocktoken = $tokendataref->[ 0 ];

      # If the above succeeded, parse the inputs.
      # The following can be in any order, so spin until
      # we've exhausted all possibilities
      while ( 1 ) {

        # Look for an input -> transition assignment
        $status =
          ParseGenericAssignment( $tokendataref, $inputshashref,
                                  $errortextref );
        if ( $status == -1 ) {
          last;
        }
        elsif ( $status == 1 ) {
          next;
        }

        # Look for an bare input assignment (ie. ignore this input)
        $status =
          ParseGenericName( $tokendataref, $inputshashref, $errortextref );
        if ( $status == -1 ) {
          last;
        }
        elsif ( $status == 1 ) {
          next;
        }

        # Look for an end block
        $status = ParseEndBlock( $tokendataref, $errortextref );
        if ( $status == 1 ) {
          last;
        }

        # Check if we have exhausted the token stream
        if ( IsEOT( $tokendataref ) ) {
          $status = -1;
          $$errortextref =
            ErrorMsgTokIndex( "End of file error", $tokendataref, 0 )
            . "Unterminated inputs block";
          last;
        }

        # If we get here, it's invalid syntax (for this context)
        $status = -1;
        $$errortextref =
          ErrorMsgTokIndex( "Syntax error", $tokendataref, 0 )
          . "Invalid syntax";
        last;
      }

    }
  } while ( 0 );

  # If we successfully parsed an INPUTS block, check that there are
  # > 0 entries in it.  Fail if not.
  if ( $status == 1 ) {
    if ( !defined $inputshashref->{ _KEYS } ) {
      $status = -1;
      $$errortextref =
        ErrorMsgTok( "Syntax error", $blocktoken ) . "Empty INPUTS block";
    }
  }

  # Let the caller know how things went
  return ( $status );
}

# ------------------------------------------------------------------------
# Name       :
# Description:
# Parameters :
# Returns    :
# Notes      :
# ------------------------------------------------------------------------
sub ParseState($$$) {
  my ( $tokendataref, $stateref, $errortextref ) = @_;
  my ( $status, $statehashref, $statename, $blocktoken );

  do {
    {

      # We're looking for start of state block
      $status = ParseNamedBlock( [ "STATE" ],
                                 $tokendataref, $stateref, \$statehashref,
                                 \$statename, 0, $errortextref );

      # Bail upon unsuccessful parse
      last if ( $status != 1 );

      # Save the start of block token for some error reporting
      $blocktoken = $tokendataref->[ 0 ];

      # If the above succeeded, parse the state's contents.
      # The following can be in any order, so spin until
      # we've exhausted all possibilities
      while ( 1 ) {

        # Look for an entry/exit function definition
        $status = ParseKeywordAssignment( [ "ENTRY", "EXIT" ],
                                  $tokendataref, $statehashref, $errortextref );
        if ( $status == -1 ) {
          last;
        }
        elsif ( $status == 1 ) {
          next;
        }

        # Look for an inputs section
        $status = ParseInputs( $tokendataref, $statehashref, $errortextref );
        if ( $status == -1 ) {
          last;
        }
        elsif ( $status == 1 ) {
          next;
        }

        # Look for an end block
        $status = ParseEndBlock( $tokendataref, $errortextref );
        if ( $status == 1 ) {
          last;
        }

        # Check if we have exhausted the token stream
        if ( IsEOT( $tokendataref ) ) {
          $status = -1;
          $$errortextref =
            ErrorMsgTokIndex( "End of file error", $tokendataref, 0 )
            . "Unterminated state block";
          last;
        }

        # If we get here, it's invalid syntax (for this context)
        $status = -1;
        $$errortextref =
          ErrorMsgTokIndex( "Syntax error", $tokendataref, 0 )
          . "Invalid syntax";
        last;
      }

    }
  } while ( 0 );

  # If we successfully parsed a STATE block, check a number of criteria
  if ( $status == 1 ) {

    # Make sure we had an INPUTS block
    if ( !defined $statehashref->{ INPUTS } ) {
      $status = -1;
      $$errortextref =
        ErrorMsgTok( "Syntax error", $blocktoken )
        . "Missing INPUTS block in state: $statename";
    }

    # Emit warnings for defaulted NULL ENTRY/EXIT functions
    if ( !defined $statehashref->{ ENTRY } ) {
      $statehashref->{ ENTRY } = "NULL";
      Debug( 0, "Warning: State: $statename, entry function implicitly NULL" );

    }
    if ( !defined $statehashref->{ EXIT } ) {
      $statehashref->{ EXIT } = "NULL";
      Debug( 0, "Warning: State: $statename, exit function implicitly NULL" );
    }
  }

  # Let the caller know how things went
  return ( $status );
}

# ------------------------------------------------------------------------
# Name       :
# Description:
# Parameters :
# Returns    :
# Notes      :
# ------------------------------------------------------------------------
sub ParseSM($$$) {
  my ( $tokendataref, $smref, $errortextref ) = @_;
  my ( $status, $smhashref, $smname, $blocktoken );

  do {
    {

      # We're looking for start of state machine block
      $status = ParseNamedBlock( [ "STATE_MACHINE" ],
                                 $tokendataref, $smref, \$smhashref, \$smname,
                                 0, $errortextref );

      # Bail upon unsuccessful parse
      last if ( $status != 1 );

      # Save the start of block token for some error reporting
      $blocktoken = $tokendataref->[ 0 ];

      # If the above succeeded, parse the state machine's contents.
      # The following can be in any order, so spin until
      # we've exhausted all possibilities
      while ( 1 ) {

        # Look for an entry, exit, initial_state, or start_active definition
        $status =
          ParseKeywordAssignment(
          [ "ENTRY", "EXIT", "INITIAL_STATE", "START_ACTIVE" ],
          $tokendataref, $smhashref, $errortextref );
        if ( $status == -1 ) {
          last;
        }
        elsif ( $status == 1 ) {
          next;
        }

        # Look for a STATE block
        $status = ParseState( $tokendataref, $smhashref, $errortextref );
        if ( $status == -1 ) {
          last;
        }
        elsif ( $status == 1 ) {
          next;
        }

        # Look for an end block
        $status = ParseEndBlock( $tokendataref, $errortextref );
        if ( $status == 1 ) {
          last;
        }

        # Check if we have exhausted the token stream
        if ( IsEOT( $tokendataref ) ) {
          $status = -1;
          $$errortextref =
            ErrorMsgTokIndex( "End of file error", $tokendataref, 0 )
            . "Unterminated state machine block";
          last;
        }

        # If we get here, it's invalid syntax (for this context)
        $status = -1;
        $$errortextref =
          ErrorMsgTokIndex( "Syntax error", $tokendataref, 0 )
          . "Invalid syntax";
        last;
      }

    }
  } while ( 0 );

  # If we successfully parsed a STATE_MACHINE block, check a number of criteria
  if ( $status == 1 ) {

    # clear the error string
    $$errortextref = "";

    # Make sure we had at least one STATE block
    if ( !defined $smhashref->{ STATE }->{ _KEYS } ) {
      $status = -1;
      $$errortextref .=
        ErrorMsgTok( "Syntax error", $blocktoken )
        . "No STATE blocks in state machine: $smname\n";
    }

    # Make sure INITIAL_STATE is specified and valid
    if ( !defined $smhashref->{ INITIAL_STATE } ) {
      $status = -1;
      $$errortextref .=
        ErrorMsgTok( "Syntax error", $blocktoken )
        . "No INITIAL_STATE specified in state machine: $smname\n";
    }
    elsif ( !defined $smhashref->{ STATE }->{ $smhashref->{ INITIAL_STATE } } )
    {
      $status = -1;
      $$errortextref .=
          ErrorMsgTok( "Syntax error", $blocktoken )
        . "Invalid initial state: $smhashref->{INITIAL_STATE} "
        . "specified in state machine: $smname\n";
    }

    # Make sure START_ACTIVE is specified and valid
    if ( !defined $smhashref->{ START_ACTIVE } ) {
      $status = -1;
      $$errortextref .=
        ErrorMsgTok( "Syntax error", $blocktoken )
        . "No START_ACTIVE specified in state machine: $smname\n";
    }
    elsif (    ( $smhashref->{ START_ACTIVE } ne "TRUE" )
            && ( $smhashref->{ START_ACTIVE } ne "FALSE" ) )
    {
      $status = -1;
      $$errortextref .=
          ErrorMsgTok( "Syntax error", $blocktoken )
        . "START_ACTIVE must be TRUE or FALSE "
        . "in state machine: $smname\n";
    }

    # Clean any trailing newlines
    chomp( $$errortextref );

    # Emit warnings for defaulted NULL ENTRY/EXIT functions
    if ( !defined $smhashref->{ ENTRY } ) {
      $smhashref->{ ENTRY } = "NULL";
      Debug( 0,
            "Warning: State machine: $smname, entry function implicitly NULL" );

    }
    if ( !defined $smhashref->{ EXIT } ) {
      $smhashref->{ EXIT } = "NULL";
      Debug( 0,
             "Warning: State machine: $smname, exit function implicitly NULL" );
    }
  }

  # Let the caller know how things went
  return ( $status );
}

# ------------------------------------------------------------------------
# Name       :
# Description:
# Parameters :
# Returns    :
# Notes      :
# ------------------------------------------------------------------------
sub ParseSigMappingBlock($$$) {
  my ( $tokendataref, $sigmapperref, $errortextref ) = @_;
  my ( $status, $sigmapperhashref, $sigmapfn, $blocktoken );

  do {
    {

      # We're looking for start of the sig mapper block
      $status = ParseNamedBlock(
                                 [ "SIGNAL_MAPPER" ], $tokendataref,
                                 $sigmapperref,       \$sigmapperhashref,
                                 \$sigmapfn,          1,
                                 $errortextref
                                 );

      # Bail upon unsuccessful parse
      last if ( $status != 1 );

      # Save the start of block token for some error reporting
      $blocktoken = $tokendataref->[ 0 ];

      # If the above succeeded, parse the sigmapper.
      # The following can be in any order, so spin until
      # we've exhausted all possibilities
      while ( 1 ) {

        # Look for an name declaration
        $status =
          ParseGenericName( $tokendataref, $sigmapperhashref, $errortextref );
        if ( $status == -1 ) {
          last;
        }
        elsif ( $status == 1 ) {
          next;
        }

        # Look for an end block
        $status = ParseEndBlock( $tokendataref, $errortextref );
        if ( $status == 1 ) {
          last;
        }

        # Check if we have exhausted the token stream
        if ( IsEOT( $tokendataref ) ) {
          $status = -1;
          $$errortextref =
            ErrorMsgTokIndex( "End of file error", $tokendataref, 0 )
            . "Unterminated SIGNAL_MAPPER block";
          last;
        }

        # If we get here, it's invalid syntax (for this context)
        $status = -1;
        $$errortextref =
          ErrorMsgTokIndex( "Syntax error", $tokendataref, 0 )
          . "Invalid syntax";
        last;
      }

    }
  } while ( 0 );

  # If we successfully parsed a SIGNAL_MAPPER block, check that there are
  # > 0 entries in it.  Fail if not.
  if ( $status == 1 ) {
    if ( !defined $sigmapperhashref->{ _KEYS } ) {
      $status = -1;
      $$errortextref =
        ErrorMsgTok( "Syntax error", $blocktoken )
        . "Empty SIGNAL_MAPPER block: $sigmapfn";
    }
  }

  # Let the caller know how things went
  return ( $status );
}

# ------------------------------------------------------------------------
# Name       :
# Description:
# Parameters :
# Returns    :
# Notes      :
# ------------------------------------------------------------------------
sub ParseGroup($$$) {
  my ( $tokendataref, $groupref, $errortextref ) = @_;
  my ( $status, $grouphashref, $groupname, $blocktoken );

  do {
    {

      # We're looking for start of group block
      $status = ParseNamedBlock( [ "GROUP" ],
                                 $tokendataref, $groupref, \$grouphashref,
                                 \$groupname, 0, $errortextref );

      # Bail upon unsuccessful parse
      last if ( $status != 1 );

      # Save the start of block token for some error reporting
      $blocktoken = $tokendataref->[ 0 ];

      # If the above succeeded, parse the group's contents.
      # The following can be in any order, so spin until
      # we've exhausted all possibilities
      while ( 1 ) {

        # Look for one of the many possible assignment keywords
        $status = ParseKeywordAssignment(
                                          [
                                            "ENTRY",                 "EXIT",
                                            "WAIT",                  "TCB", "TCB_PTR",
                                            "INT_CMD_SIG",           "INT_CMD_QUEUE",
                                            "DELAYED_INT_CMD_QUEUE", "GLOBAL_GROUP",
                                            "DEBUG_HOOK",            "HEAP_CTOR",
                                            "HEAP_DTOR",             "MALLOC",
                                            "FREE",                  "USER_SIGNAL_FN",
                                            "USER_HANDLER_FN"
                                          ],
                                          $tokendataref,
                                          $grouphashref,
                                          $errortextref
                                          );
        if ( $status == -1 ) {
          last;
        }
        elsif ( $status == 1 ) {
          next;
        }

        # Look for a SIGNAL_MAPPER block
        $status =
          ParseSigMappingBlock( $tokendataref, $grouphashref, $errortextref );
        if ( $status == -1 ) {
          last;
        }
        elsif ( $status == 1 ) {
          next;
        }

        # Look for a STATE_MACHINE block
        $status = ParseSM( $tokendataref, $grouphashref, $errortextref );
        if ( $status == -1 ) {
          last;
        }
        elsif ( $status == 1 ) {
          next;
        }

        # Look for an end block
        $status = ParseEndBlock( $tokendataref, $errortextref );
        if ( $status == 1 ) {
          last;
        }

        # Check if we have exhausted the token stream
        if ( IsEOT( $tokendataref ) ) {
          $status = -1;
          $$errortextref =
            ErrorMsgTokIndex( "End of file error", $tokendataref, 0 )
            . "Unterminated state machine group block";
          last;
        }

        # If we get here, it's invalid syntax (for this context)
        $status = -1;
        $$errortextref =
          ErrorMsgTokIndex( "Syntax error", $tokendataref, 0 )
          . "Invalid syntax";
        last;
      }

    }
  } while ( 0 );

  # If we successfully parsed a GROUP block, check a number of criteria
  if ( $status == 1 ) {

    # clear the error string
    $$errortextref = "";

    # Make sure we had at least one STATE_MACHINE block
    if ( !defined $grouphashref->{ STATE_MACHINE }->{ _KEYS } ) {
      $status = -1;
      $$errortextref .=
        ErrorMsgTok( "Syntax error", $blocktoken )
        . "No STATE_MACHINE blocks in group: $groupname\n";
    }

    # Make sure we have some things explicitly defined
    if ( !defined $grouphashref->{ WAIT } ) {
      $status = -1;
      $$errortextref .=
        ErrorMsgTok( "Syntax error", $blocktoken )
        . "No WAIT function defined in group: $groupname\n";
    }
    if ( !defined $grouphashref->{ TCB } ) {
      $status = -1;
      $$errortextref .=
        ErrorMsgTok( "Syntax error", $blocktoken )
        . "No TCB defined in group: $groupname\n";
    }
    if ( !defined $grouphashref->{ INT_CMD_SIG } ) {
      $status = -1;
      $$errortextref .=
        ErrorMsgTok( "Syntax error", $blocktoken )
        . "No INT_CMD_SIG signal defined in group: $groupname\n";
    }
    if ( !defined $grouphashref->{ INT_CMD_QUEUE } ) {
      $status = -1;
      $$errortextref .=
        ErrorMsgTok( "Syntax error", $blocktoken )
        . "No INT_CMD_QUEUE defined in group: $groupname\n";
    }
    if ( !defined $grouphashref->{ MALLOC } ) {
      $status = -1;
      $$errortextref .=
        ErrorMsgTok( "Syntax error", $blocktoken )
        . "No MALLOC function defined in group: $groupname\n";
    }
    if ( !defined $grouphashref->{ FREE } ) {
      $status = -1;
      $$errortextref .=
        ErrorMsgTok( "Syntax error", $blocktoken )
        . "No FREE function defined in group: $groupname\n";
    }

    # Clean any trailing newlines
    chomp( $$errortextref );

    # Emit warnings for defaulted NULL definitions
    if ( !defined $grouphashref->{ ENTRY } ) {
      $grouphashref->{ ENTRY } = "NULL";
      Debug( 0, "Warning: Group: $groupname, entry function implicitly NULL" );

    }
    if ( !defined $grouphashref->{ EXIT } ) {
      $grouphashref->{ EXIT } = "NULL";
      Debug( 0, "Warning: Group: $groupname, exit function implicitly NULL" );
    }
    if ( !defined $grouphashref->{ DEBUG_HOOK } ) {
      $grouphashref->{ DEBUG_HOOK } = "NULL";
      Debug( 0,
            "Warning: Group: $groupname, debug hook function implicitly NULL" );
    }
    if ( !defined $grouphashref->{ HEAP_CTOR } ) {
      $grouphashref->{ HEAP_CTOR } = "NULL";
      Debug( 0,
             "Warning: Group: $groupname, heap ctor function implicitly NULL" );
    }
    if ( !defined $grouphashref->{ HEAP_DTOR } ) {
      $grouphashref->{ HEAP_DTOR } = "NULL";
      Debug( 0,
             "Warning: Group: $groupname, heap dtor function implicitly NULL" );
    }
    if ( !defined $grouphashref->{ GLOBAL_GROUP } ) {
      $grouphashref->{ GLOBAL_GROUP } = "NULL";
      Debug( 0,
           "Warning: Group: $groupname, global group pointer implicitly NULL" );
    }
    # optional feature. No warning necessary
    if ( !defined $grouphashref->{ DELAYED_INT_CMD_QUEUE } ) {
      $grouphashref->{ DELAYED_INT_CMD_QUEUE } = "NULL";
    }
    # USER_* stuff is deprecated, so no warning necessary
    if ( !defined $grouphashref->{ USER_SIGNAL_FN } ) {
      $grouphashref->{ USER_SIGNAL_FN } = "NULL";
    }
    if ( !defined $grouphashref->{ USER_HANDLER_FN } ) {
      $grouphashref->{ USER_HANDLER_FN } = "NULL";
    }
  }

  # Let the caller know how things went
  return ( $status );
}

# ------------------------------------------------------------------------
# Name       :
# Description:
# Parameters :
# Returns    :
# Notes      :
# ------------------------------------------------------------------------
sub GenerateSimple16BitHash($) {
  my $instring  = shift;
  my $upperbyte = 0;
  my $lowerbyte = 0;

  # Spin through the string and add the odd indexed character values
  # to the lower byte, and the even indexed character values to the upper byte
  #
  # Note that this routine is BROKEN intentionally for backwards compatibility
  # in that the _last_ character of the string is also ignored in the original
  # smcomp.cpp routine.  Lame! and should read:
  # for (my $index = 0; $index < length($instring); $index++) {
  #
  for ( my $index = 0 ; $index < length( $instring ) - 1 ; $index++ ) {
    my $char = substr( $instring, $index, 1 );
    if ( ( $index % 2 ) == 0 ) {
      $upperbyte += ord( $char );
    }
    else {
      $lowerbyte += ord( $char );
    }
  }

  # Return the 16 bit value formed by concatenating the two 8 bit parts
  return ( ( ( $upperbyte & 0xff ) << 8 ) + ( $lowerbyte & 0xff ) );
}

# ------------------------------------------------------------------------
# Name       :
# Description:
# Parameters :
# Returns    :
# Notes      :
# ------------------------------------------------------------------------
sub GenerateSMCode($$$) {
  my ( $SMhashref, $SMprefix, $errortextref ) = @_;

  # Check that we were passed an existing hash reference (internal error)
  if ( !defined $SMhashref ) {
    die( "Undefined hash value passed in for assignment" );
  }

  my $status = 0;

  # Create the state enum
  my $stateenum = "enum\n{\n";
  foreach my $statename ( @{ $SMhashref->{ STATE }->{ _KEYS } } ) {
    $stateenum .= "  $statename,\n";
  }
  $stateenum .= "};";

  # Create the #define for the number of states
  my $numstates       = scalar( @{ $SMhashref->{ STATE }->{ _KEYS } } );
  my $numstatesdefine = "#define ${SMprefix}_NUMBER_OF_STATES $numstates";

  # Create the state name / entry / exit table
  # and create unique lists of all our inputs, transition functions, and
  # state entry/exit fns
  my $statetable = "static const stm_state_array_type\n";
  $statetable .= "  ${SMprefix}_states[ ${SMprefix}_NUMBER_OF_STATES ] =\n{\n";
  my @allinputs            = ();
  my @alltransfns          = ();
  my @allstateentryexitfns = ();
  my $uses_ignore_fn       = "";

  # Some hashes to figure out uniqueness
  my %allinputshash            = ();
  my %alltransfnshash          = ();
  my %allstateentryexitfnshash = ();
  foreach my $statename ( @{ $SMhashref->{ STATE }->{ _KEYS } } ) {

    # Get the state's subhash
    my $state = $SMhashref->{ STATE }->{ $statename };

    # Get the entry fn name (defaults to NULL)
    my $stateentry = "NULL";
    if (    ( defined $state->{ ENTRY } )
         && ( $state->{ ENTRY } ne "" )
         && ( $state->{ ENTRY } ne "NULL" ) )
    {
      $stateentry = $state->{ ENTRY };

      # Store the function if unique, and mark it as stored
      push( @allstateentryexitfns, $stateentry )
        unless defined $allstateentryexitfnshash{ $stateentry };
      $allstateentryexitfnshash{ $stateentry }++;
    }

    # Get the exit fn name (defaults to NULL)
    my $stateexit = "NULL";
    if (    ( defined $state->{ EXIT } )
         && ( $state->{ EXIT } ne "" )
         && ( $state->{ EXIT } ne "NULL" ) )
    {
      $stateexit = $state->{ EXIT };

      # Store the function if unique, and mark it as stored
      push( @allstateentryexitfns, $stateexit )
        unless defined $allstateentryexitfnshash{ $stateexit };
      $allstateentryexitfnshash{ $stateexit }++;
    }

    # Add our line of source
    $statetable .= "  {\"$statename\", $stateentry, $stateexit},\n";

    # While we're in here, accumulate the total set of unique inputs
    # and transition functions
    foreach my $inputname ( @{ $state->{ INPUTS }->{ _KEYS } } ) {

      # Store the input if unique, and mark it as stored
      push( @allinputs, $inputname )
        unless defined $allinputshash{ $inputname };
      $allinputshash{ $inputname }++;

      # Store the transition fn if unique, and mark it as stored
      if ( defined( $state->{ INPUTS }->{ $inputname } ) ) {

        # If the transition function is left blank, assign it the ignore fn
        if ( $state->{ INPUTS }->{ $inputname } eq "" ) {
          $state->{ INPUTS }->{ $inputname } = "${SMprefix}_ignore_input";
          $uses_ignore_fn = "${SMprefix}_ignore_input";
        }

        # Now check for uniqueness and store
        push( @alltransfns, $state->{ INPUTS }->{ $inputname } )
          unless defined $alltransfnshash{ $state->{ INPUTS }->{ $inputname } };
        $alltransfnshash{ $state->{ INPUTS }->{ $inputname } }++;
      }
    }
  }
  $statetable .= "};";

  # Create the list of prototypes for the transition functions
  my $transfnprotos = "";
  foreach my $transfn ( @alltransfns ) {
    $transfnprotos .= "static stm_state_type ${transfn}(void *);\n";
  }

  # Tack on the dummy ignore function if it's used
  if ( $uses_ignore_fn ne "" ) {
    $transfnprotos .= <<DONE;

/* Input ignoring transition function that returns STM_SAME_STATE */
static stm_state_type ${uses_ignore_fn}(void *payload)
{
  /* Suppress lint/compiler warnings about unused payload variable */
  if(payload){}
  return(STM_SAME_STATE);
}
DONE
  }

  # Create the list of prototypes for state entry/exit functions
  my $stateentryexitprotos = "";
  foreach my $entryexitfn ( @allstateentryexitfns ) {
    $stateentryexitprotos .=
      "static void ${entryexitfn}(void *payload, stm_state_type previous_state);\n";
  }

  # Create the #define for the number of unique inputs
  my $numinputs       = scalar( @allinputs );
  my $numinputsdefine = "#define ${SMprefix}_NUMBER_OF_INPUTS $numinputs";

  # Create the input / input name table
  my $inputtable = "static const stm_input_array_type\n";
  $inputtable .= "  ${SMprefix}_inputs[ ${SMprefix}_NUMBER_OF_INPUTS ] =\n{\n";
  foreach my $inputname ( @allinputs ) {
    $inputtable .= "  {(stm_input_type)$inputname, \"$inputname\"},\n";
  }
  $inputtable .= "};";

  # Create the transition table
  my $transtable = "static const stm_transition_fn_type\n";
  $transtable .= "  ${SMprefix}_transitions[ ${SMprefix}_NUMBER_OF_STATES * ";
  $transtable .= " ${SMprefix}_NUMBER_OF_INPUTS ] =\n{\n";
  foreach my $statename ( @{ $SMhashref->{ STATE }->{ _KEYS } } ) {

    # Get the state's subhash
    my $state = $SMhashref->{ STATE }->{ $statename };

    # Add a comment
    $transtable .= "  /* Transition functions for state $statename */\n";

    # Spin through all possible inputs
    foreach my $inputname ( @allinputs ) {
      $transtable .= "    ";
      if ( !defined $state->{ INPUTS }->{ $inputname } ) {
        $transtable .= "NULL,";
      }
      elsif ( $state->{ INPUTS }->{ $inputname } eq "" ) {
        $transtable .= "${SMprefix}_ignore,";
      }
      else {
        $transtable .= "$state->{INPUTS}->{$inputname},";
      }
      $transtable .= "    /* $inputname */\n";
    }
    $transtable .= "\n";
  }
  $transtable .= "};";

  # Create the unique numerical hash for TLA/debug purposes
  my $SMid = GenerateSimple16BitHash( $SMprefix );

  # Build the SM entry exit prototypes
  my $SMentryexitprotos = "";

  # Get the SM entry fn name (defaults to NULL)
  my $SMentry = "NULL";
  if (    ( defined $SMhashref->{ ENTRY } )
       && ( $SMhashref->{ ENTRY } ne "" )
       && ( $SMhashref->{ ENTRY } ne "NULL" ) )
  {
    $SMentry = $SMhashref->{ ENTRY };
    $SMentryexitprotos .= "static void ${SMentry}(stm_group_type *group);\n";
  }

  # Get the SM exit fn name (defaults to NULL)
  my $SMexit = "NULL";
  if (    ( defined $SMhashref->{ EXIT } )
       && ( $SMhashref->{ EXIT } ne "" )
       && ( $SMhashref->{ EXIT } ne "NULL" ) )
  {
    $SMexit = $SMhashref->{ EXIT };
    $SMentryexitprotos .= "static void ${SMexit}(stm_group_type *group);\n";
  }

  # Get the initial state
  my $SMinitstate = $SMhashref->{ INITIAL_STATE };

  # Get the start active flag
  my $SMstartactive = $SMhashref->{ START_ACTIVE };

  # Build the H source
  $SMhashref->{ Hsource } = <<DONE;
/* Begin machine generated code for state machine: ${SMprefix} */

/* External reference to state machine structure */
extern stm_state_machine_type ${SMprefix};

/* End machine generated code for state machine: ${SMprefix} */
DONE

  # Build the C source
  $SMhashref->{ Csource } = <<DONE;
/* Begin machine generated code for state machine: ${SMprefix} */

/* Transition function prototypes */
$transfnprotos

/* State Machine entry/exit function prototypes */
$SMentryexitprotos

/* State entry/exit function prototypes */
$stateentryexitprotos

/* Total number of states and inputs */
$numstatesdefine
$numinputsdefine


/* State enumeration */
$stateenum


/* State name, entry, exit table */
$statetable


/* Input value, name table */
$inputtable


/* Transition table */
$transtable


/* State machine definition */
stm_state_machine_type ${SMprefix} =
{
  "${SMprefix}", /* state machine name */
  $SMid, /* unique SM id (hash of name) */
  $SMentry, /* state machine entry function */
  $SMexit, /* state machine exit function */
  $SMinitstate, /* state machine initial state */
  $SMstartactive, /* state machine starts active? */
  ${SMprefix}_NUMBER_OF_STATES,
  ${SMprefix}_NUMBER_OF_INPUTS,
  ${SMprefix}_states,
  ${SMprefix}_inputs,
  ${SMprefix}_transitions,
};

/* End machine generated code for state machine: ${SMprefix} */
DONE

  # Create the TLA transition list
  my $TLAsource = "";
  my @colors =
    ( "black", "blue", "maroon", "magenta", "red", "navy", "olive", "purple" );
  my $bgcolor = "white";
  my $fgcolor = $colors[ int( rand( scalar( @colors ) ) ) ];

  # Pick debug fg color
  # Spin through all possible 'current' states
  for ( my $currstateidx = 0 ;
        $currstateidx < scalar( @{ $SMhashref->{ STATE }->{ _KEYS } } ) ;
        $currstateidx++ )
  {

    # Spin through all possible inputs
    for ( my $inputidx = 0 ; $inputidx < scalar( @allinputs ) ; $inputidx++ ) {

      # Spin through all possible 'next' states
      for ( my $nextstateidx = 0 ;
            $nextstateidx < scalar( @{ $SMhashref->{ STATE }->{ _KEYS } } ) ;
            $nextstateidx++ )
      {

        # Create the TLA text string
        my $tlastring .= "${SMprefix}:";
        $tlastring    .= "${$SMhashref->{STATE}->{_KEYS}}[$currstateidx]:";
        $tlastring    .= "$allinputs[$inputidx]:";
        $tlastring    .= "${$SMhashref->{STATE}->{_KEYS}}[$nextstateidx]";

        # Do a little compression on the TLA text string
        # by removing some common suffixes
        $tlastring =~ s/(_SM|_STATE|_CMD)\b//ig;

        # Format the output line
        $TLAsource .= sprintf(
                               "%-60s %04x %x %02x %x    @%s @%s\n",
                               $tlastring, $SMid,         $currstateidx,
                               $inputidx,  $nextstateidx, $fgcolor,
                               $bgcolor
                               );
      }

      # Put a blank line between current states
      $TLAsource .= "\n";
    }
  }

  # Build the TLA source
  $SMhashref->{ TLAsource } = <<DONE;
# Begin machine generated TLA code for state machine: ${SMprefix}
# State machine:current state:input:next state                      fgcolor bgcolor

$TLAsource

# End machine generated TLA code for state machine: ${SMprefix}
DONE

  # Return with what we've got
  return ( $status );
}

# ------------------------------------------------------------------------
# Name       :
# Description:
# Parameters :
# Returns    :
# Notes      :
# ------------------------------------------------------------------------
sub GenerateSMGroupCode($$$) {
  my ( $SMGhashref, $SMGprefix, $errortextref ) = @_;

  # Check that we were passed an existing hash reference (internal error)
  if ( !defined $SMGhashref ) {
    die( "Undefined hash value passed in for assignment" );
  }

  my $status = 0;

  # First, try to generate the code for each SM in the group
  # Upon failure, bail out (return) right away
  foreach my $smname ( @{ $SMGhashref->{ STATE_MACHINE }->{ _KEYS } } ) {
    my $sm = $SMGhashref->{ STATE_MACHINE }->{ $smname };
    $status = GenerateSMCode( $sm, $smname, $errortextref );
    return ( $status ) if ( $status != 0 );
  }

  # Construct the array of the state machines in this group
  my $numsms      = scalar( @{ $SMGhashref->{ STATE_MACHINE }->{ _KEYS } } );
  my $smtablename = "${SMGprefix}_sm_list";
  my $smtable =
    "static stm_state_machine_type *" . $smtablename . "[ $numsms ] =\n{\n";
  foreach my $smname ( @{ $SMGhashref->{ STATE_MACHINE }->{ _KEYS } } ) {
    $smtable .= "  &$smname,\n";
  }
  $smtable .= "};";

  # Generate the signal list (if any) specified in the signal mapping block
  my $sigtable     = "/* No signals handled */";
  my $sigtablename = "NULL";
  my $sigmapperfn  = "NULL";
  my $numsigs      = 0;
  if ( defined( $SMGhashref->{ SIGNAL_MAPPER } )
       && ( scalar( @{ $SMGhashref->{ SIGNAL_MAPPER }->{ _KEYS } } ) == 1 ) )
  {

    # Grab the signal mapper function
    $sigmapperfn = ${ $SMGhashref->{ SIGNAL_MAPPER }->{ _KEYS } }[ 0 ];

    # Grab the number of signals
    $numsigs =
      scalar(
             @{ $SMGhashref->{ SIGNAL_MAPPER }->{ $sigmapperfn }->{ _KEYS } } );

    # Create the signal table name
    $sigtablename = "${SMGprefix}_sig_list";

    # Create the signal table code
    $sigtable = "static rex_sigs_type " . $sigtablename . "[ $numsigs ] =\n{\n";
    foreach my $sig (
              @{ $SMGhashref->{ SIGNAL_MAPPER }->{ $sigmapperfn }->{ _KEYS } } )
    {
      $sigtable .= "  $sig,\n";
    }
    $sigtable .= "};";
  }

  # Extract other params for the group structure
  my $waitfn         = $SMGhashref->{ WAIT };

  # If TCB SELF is defined, set tcb to NULL else use the value in TCB.
  # One is guaranteed to exist and they are mutually exclusive
  my $tcb = $SMGhashref->{ TCB };
  if ( $tcb =~ m/^SELF$/i)
  {
    $tcb = "NULL";
  }
  else
  {
    $tcb =  "&".$tcb;
  }
  
  my $intcmdsig      = $SMGhashref->{ INT_CMD_SIG };
  my $intcmdq        = "&" . $SMGhashref->{ INT_CMD_QUEUE };
  my $delayedintcmdq = $SMGhashref->{ DELAYED_INT_CMD_QUEUE };
  if ( $delayedintcmdq ne "NULL" ) {
    $delayedintcmdq = "&" . $delayedintcmdq;
  }

  my $entryfn        = $SMGhashref->{ ENTRY };
  my $exitfn         = $SMGhashref->{ EXIT };
  my $globalgroup    = $SMGhashref->{ GLOBAL_GROUP };
  if ( $globalgroup ne "NULL" ) {
    $globalgroup = "&" . $globalgroup;
  }

  my $debugfn       = $SMGhashref->{ DEBUG_HOOK };
  my $heapctorfn    = $SMGhashref->{ HEAP_CTOR };
  my $heapdtorfn    = $SMGhashref->{ HEAP_DTOR };
  my $mallocfn      = $SMGhashref->{ MALLOC };
  my $freefn        = $SMGhashref->{ FREE };
  my $usersigfn     = $SMGhashref->{ USER_SIGNAL_FN };
  my $userhandlerfn = $SMGhashref->{ USER_HANDLER_FN };

  # Create the entry/exit prototypes, if needed
  my $SMGentryexitprotos = "";
  if ( $entryfn ne "NULL" )
  {
    $SMGentryexitprotos .= "static void ${entryfn}(void);\n";
  }
  if ( $exitfn ne "NULL" )
  {
    $SMGentryexitprotos .= "static void ${exitfn}(void);\n";
  }

  # Create the signal mapper prototype, if needed
  my $sigmapperproto = "";
  if ( $sigmapperfn ne "NULL" ) {
    $sigmapperproto = "static boolean ${sigmapperfn}";
    $sigmapperproto .= "(rex_tcb_type *,rex_sigs_type, stm_sig_cmd_type *);\n"
  }


  # Build the H source
  $SMGhashref->{ Hsource } = <<DONE;
/* Begin machine generated code for state machine group: ${SMGprefix} */

/* External reference to state machine group structure */
extern stm_group_type ${SMGprefix};

/* End machine generated code for state machine group: ${SMGprefix} */
DONE

  # Build the C source
  $SMGhashref->{ Csource } = <<DONE;
/* Begin machine generated code for state machine group: ${SMGprefix} */

/* State machine group entry/exit function prototypes */
$SMGentryexitprotos


/* State machine group signal mapper function prototype */
$sigmapperproto


/* Table of state machines in group */
$smtable


/* Table of signals handled by the group's signal mapper */
$sigtable


/* State machine group definition */
stm_group_type ${SMGprefix} =
{
  "${SMGprefix}", /* state machine group name */
  $sigtablename, /* signal mapping table */
  $numsigs, /* number of signals in mapping table */
  $sigmapperfn, /* signal mapper function */
  $smtablename, /* state machine table */
  $numsms, /* number of state machines in table */
  $waitfn, /* wait function for group's signals */
  $tcb, /* TCB of task that processes group's signals */
  $intcmdsig, /* internal command queue signal */
  $intcmdq, /* internal command queue */
  $delayedintcmdq, /* delayed internal command queue */
  $entryfn, /* group entry function */
  $exitfn, /* group exit function */
  $debugfn, /* debug hook function */
  $heapctorfn, /* group heap constructor fn */
  $heapdtorfn, /* group heap destructor fn */
  $mallocfn, /* group malloc fn */
  $freefn, /* group free fn */
  $globalgroup, /* global group */
  $usersigfn, /* user signal retrieval fn */
  $userhandlerfn /* user signal handler fn */
};

/* End machine generated code for state machine group: ${SMGprefix} */
DONE

  # Return with what we've got
  return ( $status );
}


# ------------------------------------------------------------------------
# Name       : abort_handler
# Description: This subroutine is run when an INT signal is caught.  It is
#              responsible for cleaning up any generated files.
# Parameters :
# Returns    :
# Notes      :
# ------------------------------------------------------------------------
sub abort_handler {
  # Let the user know we were interrupted
  Debug(0,"SIGINT received!");

  # Let curious users know what we're trying to delete
  if (scalar(@files_created) > 0) {
    Debug(1,"Deleting generated file/s: " . join " ",@files_created);
    unlink @files_created;
  }
}


# ------------------------------------------------------------------------
# Name       :
# Description:
# Parameters :
# Returns    :
# Notes      :
# ------------------------------------------------------------------------
# Print out usage and exit with passed-in status.
sub Usage($) {
  my $status = shift;
  my ( $prgname ) = ( $0 =~ m|([^/\\]+)$| );
  print STDERR <<EOF;

Usage: $prgname [-h] [-v] [-t] [-b basename] [-d dirname] [-[sea] ext] [input filename]
State Machine/Group Compiler, version 1.1.2
Copyright (c) Qualcomm Technologies Incorporated, $currentyear

Options:
  -h | -? | --help      : Display this information
  -v | -vv | -vvv | ... : Verbosity/debugging level
  -t | --tla            : Generate logic analyzer file
  -b basename |         : Use basename for source code output files
  --basename basename
  -d dirname |          : Directory in which to put source code output files
  --dirname dirname
  -s ext |              : C source output suffix, (default: smt)
  --sourceext ext
  -e ext |              : C header output suffix, (default: smh)
  --headerext ext
  -a ext |              : TLA output suffix, (default: tss)
  --tlaext ext


Notes:
  If input filename is omitted, input is taken from STDIN.
  Error and debug information is directed to STDERR.
  Exit code of 0 indicates success.

EOF
  exit( $status );
}

# ------------------------------------------------------------------------
# Name       :
# Description:
# Parameters :
# Returns    :
# Notes      :
# ------------------------------------------------------------------------
# Main routine
# Called upon program startup to parse args and do stuff
{
  # Set up a signal handler to catch Ctrl-C
  $SIG{INT} = \&abort_handler;

  my $wanttla  = 0;
  my $basename = "";
  my $dirname = "";
  my $Cext = "smt";
  my $Hext = "smh";
  my $TLAext = "tss";

  # Parse the command-line options, printing usage upon failure
  GetOptions(
              "help|h|?"      => sub { Usage( 0 ) },
              "tla|t"         => \$wanttla,
              "basename|b=s"  => \$basename,
              "dirname|d=s"   => \$dirname,
              "sourceext|s=s" => \$Cext,
              "headerext|e=s" => \$Hext,
              "tlaext|a=s"    => \$TLAext,
              "v+"            => \$debuglevel
              ) || Usage( 1 );

  # Check what's left in ARGV, should be 0 or 1 'filename'
  # Bail if > 1 filename specified
  if ( scalar( @ARGV ) > 1 ) {
    Debug( 0, "Greater than one filename argument!" );
    Usage( 1 );
  }
  elsif ( scalar( @ARGV ) == 0 ) {

    # No filename argument, make it "-" for STDIN
    push( @ARGV, "-" );
  }

  # If we were provided an output directory, ensure that it exists
  if ( ( $dirname ne "" ) &&
       ( ! -d $dirname ) ) {
    Debug( -1, "No such directory: $dirname");
  }

  # Tokenized file data
  my @tokendata;

  # Tokenize the file
  TokenizeFile( $ARGV[ 0 ], \@tokendata );

  # Make sure we got some tokens (guard against possible empty/blank file)
  if ( IsEOT( \@tokendata ) ) {
    Debug( -1, "Empty or blank input file");
  }

  # Get the true source filename from the token stream
  # (works w/ cpp'ed data or plain data, thanks to tokenizer)
  my $sourcefilename = ExtractFilename( \@tokendata, 0 );

  # Make sure we have a sane-looking source filename (ie. not "-")
  if ( $sourcefilename eq "-" ) {
    Debug( -1, "Ambiguous input filename, please preprocess STDIN or specify an input filename");
  }

  my $datatree     = {};
  my $errortext    = "";
  my $status       = 0;
  my $gotsmg       = 0;
  my $outCsource   = "";
  my $outHsource   = "";
  my $outTLAsource = "";

  # Let's be flexible and try to figure out what kind of file we were given
  #
  # Try to parse a set of groups first
  do {
    $status = ParseGroup( \@tokendata, $datatree, \$errortext );
  } while ( $status == 1 );

  # Bail out if there was an error
  if ( $status != 0 ) {
    Debug( -1, "$errortext" );
  }

  # If we didn't find any groups, try to find some SM's
  if ( !defined $datatree->{ GROUP }
       || ( scalar( @{ $datatree->{ GROUP }->{ _KEYS } } ) == 0 ) )
  {
    do {
      $status = ParseSM( \@tokendata, $datatree, \$errortext );
    } while ( $status == 1 );

    # Bail out if there was an error
    if ( $status != 0 ) {
      Debug( -1, "$errortext" );
    }

    # And also bail out if we didn't fetch any state machines, since that's
    # all we can do at this point
    if ( !defined $datatree->{ STATE_MACHINE }
         || ( scalar( @{ $datatree->{ STATE_MACHINE }->{ _KEYS } } ) == 0 ) )
    {
      Debug( -1, "Error: No state machines or groups parsed in $ARGV[0]" );
    }

    # Generate SM code for what we parsed
    foreach my $smk ( @{ $datatree->{ STATE_MACHINE }->{ _KEYS } } ) {
      my $sm = $datatree->{ STATE_MACHINE }->{ $smk };
      $status = GenerateSMCode( $sm, $smk, \$errortext );
      last if ( $status != 0 );
    }

  }
  else {

    # If we get here, we found state machine group/s
    $gotsmg = 1;

    # Generate SM group code for what we parsed
    foreach my $groupk ( @{ $datatree->{ GROUP }->{ _KEYS } } ) {
      my $group = $datatree->{ GROUP }->{ $groupk };
      $status = GenerateSMGroupCode( $group, $groupk, \$errortext );
      last if ( $status != 0 );
    }

  }

  # Handle any errors in the code generation
  if ( $status != 0 ) {
    Debug( -1, "$errortext" );
  }

  # Build the files to be generated for a state machine group
  if ( $gotsmg ) {
    # Spin through all groups parsed
    foreach my $groupk ( @{ $datatree->{ GROUP }->{ _KEYS } } ) {
      my $group = $datatree->{ GROUP }->{ $groupk };
      # We only care about the groups in the file we parsed, not any that
      # may have been #included from the parsed file
      if ( $group->{ FILENAME } eq $sourcefilename ) {
        Debug(
               1,
               "Including C source for SM Group: $groupk, "
                 . "filename: $group->{FILENAME} == $sourcefilename"
                 );
        # Spin through all the state machines in the group
        foreach my $smk ( @{ $group->{ STATE_MACHINE }->{ _KEYS } } ) {
          my $sm = $group->{ STATE_MACHINE }->{ $smk };

          # Header and TLA files can have info from all state machines,
          # regardless of whether they were in the parsed file or not
          $outHsource   .= $sm->{ Hsource };
          $outTLAsource .= $sm->{ TLAsource };

          # However, the C source to be generated should only be for the
          # parsed file, and not anything it #includes
          if ( $sm->{ FILENAME } eq $sourcefilename ) {
            Debug(
                   1,
                   "Including C source for SM: $smk, "
                     . "filename: $sm->{FILENAME} == $sourcefilename"
                     );
            $outCsource   .= $sm->{ Csource };
          }
          else {
            Debug(
                   1,
                   "Excluding C source for SM: $smk, "
                     . "filename: $sm->{FILENAME} != $sourcefilename"
                     );
          }

        }

        # Lastly, add the C source from the parsed group
        $outCsource .= $group->{ Csource };
      }
      else {
        Debug(
               1,
               "Excluding C source for SM Group: $groupk, "
                 . "filename: $group->{FILENAME} != $sourcefilename"
                 );
      }

      # Headers for all groups parsed can be exposed in the header file
      $outHsource .= $group->{ Hsource };

    }
  }
  else {
    # Build the files for one or more state machine
    foreach my $smk ( @{ $datatree->{ STATE_MACHINE }->{ _KEYS } } ) {
      my $sm = $datatree->{ STATE_MACHINE }->{ $smk };

      # Header and TLA files can have info from all state machines,
      # regardless of whether they were in the parsed file or not
      $outHsource   .= $sm->{ Hsource };
      $outTLAsource .= $sm->{ TLAsource };

      # However, the C source to be generated should only be for the
      # parsed file, and not anything it #includes
      if ( $sm->{ FILENAME } eq $sourcefilename ) {
        Debug(
               1,
               "Including C source for SM: $smk, "
                 . "filename: $sm->{FILENAME} == $sourcefilename"
                 );
        $outCsource   .= $sm->{ Csource };

      }
      else {
        Debug(
               1,
               "Excluding C source for SM: $smk, "
                 . "filename: $sm->{FILENAME} != $sourcefilename"
                 );
      }

    }
  }

  # Let's re-use the disclaimer as much as possible
  my $disclaimer = <<DONE;
###############################################################################
# Copyright (c) $currentyear
# Qualcomm Technologies Incorporated.
# All Rights Reserved
# Qualcomm Confidential and Proprietary
#
# Export of this technology or software is regulated by the U.S. Government.
# Diversion contrary to U.S. law is prohibited.
#
# All ideas, data and information contained in or disclosed by
# this document are confidential and proprietary information of
# Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
# By accepting this material the recipient agrees that this material
# and information contained therin are held in confidence and in
# trust and will not be used, copied, reproduced in whole or in part,
# nor its contents revealed in any manner to others without the express
# written permission of Qualcomm Technologies Incorporated.
###############################################################################
DONE

  # Get our basename from the source filename unless explicitly specified
  if ( $basename eq "" ) {
    # grab the non-path part
    ( $basename ) = $sourcefilename =~  m|([^/\\]+)$|;
    # chop off any extension
    $basename =~ s|\.\w*$||;
  }

  # Create our output base filename
  my $outfilename = $basename;

  # Prepend the directory name to outfilename if it was specified
  if ( $dirname ne "" ) {
    $outfilename = $dirname . "/" . $outfilename;
  }

  # Now build the actual C, H, and TLA files and write them out
  # using the provided (or implied) path-prefixed basename
  my $filehandle;

  # Write out the header file
  Debug( 1, "Writing header file: ${outfilename}.${Hext}");
  push @files_created,"${outfilename}.${Hext}";
  open( $filehandle, ">${outfilename}.${Hext}" )
    || Debug( -1, "Error, can't write file: ${outfilename}.${Hext}" );
  print $filehandle <<EOF;
/*=============================================================================

  ${basename}.${Hext}

Description:
  This file contains the machine generated header file for the state machine
  and/or state machine group specified in the file:
  ${sourcefilename}

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

$disclaimer
=============================================================================*/

/* Include STM framework header */
#include "stm.h"

$outHsource
EOF
  close($filehandle);

  # Write out the C source file
  Debug( 1, "Writing C source file: ${outfilename}.${Cext}");
  push @files_created,"${outfilename}.${Cext}";
  open( $filehandle, ">${outfilename}.${Cext}" )
    || Debug( -1, "Error, can't write file: ${outfilename}.${Cext}" );
  print $filehandle <<EOF;
/*=============================================================================

  ${basename}.${Cext}

Description:
  This file contains the machine generated source file for the state machine
  and/or state machine group specified in the file:
  ${sourcefilename}

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

$disclaimer
=============================================================================*/

/* Include STM framework header */
#include "stm.h"

$outCsource
EOF
  close($filehandle);

  # Write out the TLA file (if desired)
  if ( $wanttla ) {
    Debug( 1, "Writing TLA file: ${outfilename}.${TLAext}");
    push @files_created,"${outfilename}.${TLAext}";
    open( $filehandle, ">${outfilename}.${TLAext}" )
      || Debug( -1, "Error, can't write file: ${outfilename}.${TLAext}" );
    print $filehandle <<EOF;
###############################################################################
#
#    ${basename}.${TLAext}
#
# Description:
#   This file contains the machine generated state machine TLA info from the
#   file:  ${sourcefilename}
#
#
$disclaimer

$outTLAsource
EOF
    close($filehandle);
  }

  # Exit with success here
  exit( 0 );
}

