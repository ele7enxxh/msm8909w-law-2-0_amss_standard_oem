#!/usr/bin/perl

# This script does some cleanup on an XML file. It ensures parameter
# values are contained within quotes. It resolves any arithmetic
# expressions in the parameter values. It also removes XML comments
# because at the moment, elfweaver does not handle them.

use strict;

my $path_dir = "";
my $module_xml = "";
my $system_xml = "";
my $module_name = "";
my $prop_c_file = "";
my $found_system_xml = 0;
my $pre_cmd_file ="";

my $input_xml = "";
my $output_xml = "";
my $count;
my $line;
my $file;
my $current_enum;
my @enums = ('');
# Command line parsing...

if( @ARGV == 0)
{
    print "Usage:\n";
	 print " Pre-Process: Auto-gen image/module xml master file\n";
    print "    -pre <path_dir> <out-module/image-xml-file> <system-xml-file> <module-name>\n";
    print "    -pre_cmd_file  <base_path_dir> <xml file containing commands>\n";
	 print " Post-Process: Clean-up XML file and start prop_c_file\n";
    print "    -post <input-xml> <output-xml> <prop_c_file>\n";
	 print " Compatibility Post-Process\n";
    print "    -i <input-xml> -o <output-xml>\n";

	 print "";
    exit(-1);
}

for ($count = 0; $count < @ARGV; $count++) {
  if (@ARGV[$count] eq "-post")
  {
    if(@ARGV == 4)
	 {
		 $input_xml = @ARGV[$count + 1];
		 $count++;
		 $output_xml = @ARGV[$count + 1];
		 $count++;
		 $prop_c_file = @ARGV[$count + 1];
		 $count++;
	 }
  }
  elsif (@ARGV[$count] eq "-pre")
  {
    if(@ARGV == 5){
		 $path_dir = @ARGV[$count + 1];
		 $count++;
		 $module_xml = @ARGV[$count + 1];
		 $count++;
		 $system_xml = @ARGV[$count + 1];
		 $count++;
		 $module_name = @ARGV[$count + 1];
		 $count++;
	 }
  }
  elsif (@ARGV[$count] eq "-pre_cmd_file")
  {
    if(@ARGV == 3){
		 $path_dir = @ARGV[$count + 1];
		 $count++;
		 $pre_cmd_file = @ARGV[$count + 1];
		 $count++;
	 }
  }
  elsif (@ARGV[$count] eq "-i")
  {
    if(@ARGV == 4){
		 $input_xml = @ARGV[$count + 1];
		 $count++;
		 if(@ARGV[$count + 1] eq "-o")
		 {
			 $count++;
			 $output_xml = @ARGV[$count + 1];
			 $count++;
		 }
	 }
  } else {
    print "Unrecognized command line option @ARGV[$count].\n";
    exit(-1);
  }
}


#did we specific command file as an input?
if($pre_cmd_file ne "")
{
	open (pre_cmd_file, $pre_cmd_file) || die("Could not open the pre_cmd_file $pre_cmd_file.");

   # Read input XML in one line at a time.
   while ($line = <pre_cmd_file>)
   {
 		 #remove leading white spaces...
		 $line =~ s/^\s+//;
		 if(substr($line, 0, length("<DAL_PREPROCESS")) eq "<DAL_PREPROCESS")
		 {
				#there has got to be a better way to do this
				$line = substr($line, length("<DAL_PREPROCESS"), length($line));
				my ($sub_dir) = "";
				my ($wCount) = 0;
				my ($wStartIdx) = -1;
				my ($curIdx) = 0;
				my ($curChar) = "";
				while($curIdx < length($line))
				{
					$curChar = substr($line, $curIdx, 1);

					#have we got word start?
					if($wStartIdx >= 0)
					{
						#see if word is done
						if(($curChar eq " ") or ($curChar eq ">"))
						{
								#ending a word
								$wCount++;
								if($wCount == 1)
								{
									$sub_dir = substr($line, $wStartIdx, $curIdx - $wStartIdx);
								}
								elsif ($wCount == 2)
								{
									$module_xml = substr($line, $wStartIdx, $curIdx - $wStartIdx);
								}
								elsif ($wCount == 3)
								{
									$system_xml = substr($line, $wStartIdx, $curIdx - $wStartIdx);
								}
								elsif ($wCount == 4)
								{
									$module_name = substr($line, $wStartIdx, $curIdx - $wStartIdx);
									#update final path strings
									$module_xml = sprintf("%s/%s", $path_dir, $module_xml);
									$path_dir = sprintf("%s/%s", $path_dir, $sub_dir);
	
									#force exit loop we are all done
									$curIdx = 1024;	
								}
								#need to start next word
								$wStartIdx = -1;
						}
						else
						{
							#go to next char
							$curIdx++;
						}
					}
					else
					{
						if($curChar ne " ")
						{
							$wStartIdx = $curIdx;
						}
						#go to next char
						$curIdx++;
					}
				}
		}
	}
	if($module_name eq "")
   {
		die("Invalid pre-process command syntax in cmd_file $pre_cmd_file.");
	}
   close pre_cmd_file;
}



if ($module_xml ne "") {
  open (module_xml, "+>$module_xml") || die("Could not create the module XML file $module_xml.");

  print "Created module XML file: $module_xml\n";

  print module_xml "\#include \"DALPropDef.h\"\n";
  print module_xml "\#include \"DALDeviceId.h\"\n";
  print module_xml "\#include \"dalconfig.h\"\n";
  print module_xml "\n";

  print module_xml "<?xml version=\"1.0\"?>\n";
  print module_xml "<dal>\n";
  print module_xml "<module name=\"$module_name\">\n";
  print module_xml "   #include \"$path_dir/$system_xml\"\n";

  opendir(DIR, $path_dir);
  foreach $file (readdir(DIR))
  {
     if(($file ne $system_xml) && ($file ne $module_xml))
	  {
			if(substr($file, length( $file )-4 ) eq ".xml")
			{
				print module_xml  "   #include \"$path_dir/$file\"\n";
  		   }
	  }
	  else
	  {
	 		$found_system_xml = 1;
	  }
  }

  close(DIR);

  if($found_system_xml == 0)
  {
    print "ERROR: Unable to locate system_xml file $system_xml\n";
    exit(-1);
  }

  print module_xml  "</module>\n";
  print module_xml "</dal>\n";
  close module_xml;

  exit(0);
}




# Make sure we have required parameters.
if (($input_xml eq "") || ($output_xml eq "")) {
  print "You must specify an input XML file and an output XML file.\n";
  exit(-1);
}

open (input_xml, $input_xml) || die("Could not open the input XML file $input_xml.");
open (output_xml, "+>$output_xml") || die("Could not open the output XML file $output_xml.");
if ($prop_c_file ne "")
{
	open (prop_c_file, "+>$prop_c_file") || die("Could not open the output prop_c_file $prop_c_file.");
}

my $bKeepLines = 0;
my $lastLine = "";
my $line_copy = "";
my $inside_enum = 0;

   # Read input XML in one line at a time.
   while ($line = <input_xml>)
   {
     chop $line;
     # We only care about lines that have XML tags in them. These lines
     # must begin with any combination of spaces or tabs ( ,\t)* followed
     # by < , any number of characters, > , then more spaces or tabs.
     if ($line =~ m/( ,\t)*<.*>( ,\t)*/ && $line !~ m/<!--.*>/
         && $line !~ m/( ,\t)*#.*/ ) {

       #If line starts with a space or tab make sure that it is always followed by a tag.
       if ($line =~ m/^([ \t]+)/) {
         if ($line !~ m/^([ \t]+<)/) {
           next;
         }
       }

       # This line has an XML tag in it.

       # Make sure all parameter values are in quotes. Since parameter values
       # may be arithmetic expressions, this gets a little complex. An explanation
       # of this regular expression is in order...
       #
       # (\w+)= is the name of the parameter and the equals sign following it.
       #
       # ([\w\s()+-]+?) is the value of the key, which may be an arithmetic expression.
       #   It possibly contains alphanumeric characters, parenthesis, spaces, and + or
       #   - signs. We make it not greedy, because if this parameter is followed by another,
       #   a greedy match would steal all but one character of the following parameter name.
       #
       # (\s*\w+=|\s*>|\s*\/>) Things that can follow the parameter value. There can be
       #   0 or more whitespace characters, then either the name of another parameter or
       #   the end of the tag (specified as either > or />).
       #
       # We must run this over and over until there are no more matches. Doing a single
       # global match doesn't work, probably because a match consumes part of an
       # adjacent parameter (meaning that parameter then won't be matched).
       while($line =~ s/ (\w+)=([\w\s()\+\-\*]+?)(\s*\w+=|\s*>|\s*\/>)/ $1=\"$2\"$3/g) {
       }

       # Resolve any arithmetic expressions.
       $line =~ s/ (\w+)=\"\s*\(([\w\s()\+\-\*]+)\)\"/ " " . $1 ."=\"" . sprintf("0x%x",eval($2)) . "\""/ge;
       print output_xml "$line\n";

		 #determine if <driver or </driver so we can start or end discarding other lines
		 $line_copy = $line;
		 #remove leading white spaces
		 $line_copy =~ s/^\s+//;
		 if(substr($line_copy,1,6) eq "driver")
		 {
  			$bKeepLines = 1;
		 }
		 if(substr($line_copy,1,7) eq "/driver")
		 {
  			$bKeepLines = 0;
		 }
     }else
     {
	  	 $line_copy = $line;

		 #check to see if line is empty
		 #remove leading whitespaces, if line is empty do not print it.
		 $line_copy =~ s/^\s+//;
		if(substr($line_copy, 0, 1) eq "#")
		{
	 		  #if we starting prop_c file only...
			  if ($prop_c_file ne "")
			  {
				  #if its a #include ...h grab it and throw it in the prop_c_file

                                  if((substr($line_copy, length($line_copy)-4, 3) eq ".h\"") or (substr($line_copy, length($line_copy)-3, 3) eq ".h\""))
				  {
						#there has got to be a better way to do this
						my ($NotDone) = 1;
						while($NotDone == 1)
						{
							#position to first "
							if(substr($line_copy, 0, 1) ne "\"")
							{
								$line_copy = substr($line_copy, 1, length($line_copy));
							}
							else
							{
								 $line_copy =~ s/\\\\/\\/g;
								 $line_copy =~ s/\/\//\//g;							
								 if($lastLine ne $line_copy)
								 {
									 #print prop_c_file "#include $line_copy\n";
									 $lastLine = $line_copy;
								 }
								 $NotDone = 0;
							}
						}
				  }
			  }
		 }
		 elsif($line_copy ne "")
		 {

			if ($line =~ m/^((\s)*typedef(\s)*enum)/)
			{
				$current_enum = "";
				$inside_enum=1;
				$current_enum = "$current_enum$line_copy\n";
			}
			elsif($inside_enum == 1 and $line =~ m/(\s)*;/)
			{
				$current_enum = "$current_enum$line_copy\n";
				# get the name:
				my $enum_names = substr($current_enum,index($current_enum, '}'), index($current_enum, ';')-index($current_enum, '}'));
				$enum_names =~ s/(\s)*//g;
				# make sure it is unique 
				if (!grep (/$enum_names/, @enums)){
					print prop_c_file "$current_enum\n";
					push (@enums, $enum_names);
				}
				$current_enum = "";
				$inside_enum=0;
			}
			elsif($inside_enum == 1)
			{
					$current_enum = "$current_enum$line_copy\n";
			}		  
 		      if($bKeepLines == 1)
		      {
					print output_xml "$line\n";
		      }
		 }
     }
   }

	

if ($prop_c_file ne "")
{
	close prop_c_file;
	print "Post-processing complete: $output_xml   Prop file: $prop_c_file\n";
}
else
{
	print "Post-processing complete: $output_xml\n";
}
close input_xml;
close output_xml;
