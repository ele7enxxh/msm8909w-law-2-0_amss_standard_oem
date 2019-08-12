#!/usr/bin/perl
# ------------------------------------------------------------------------
# Copyright (c) 2012 QUALCOMM Inc. All Rights Reserved.
# $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg/mcfg_gen/build/mcfg_conv_elf_2_mbn.pl#1 $
#
#   Converts mcfg*.elf to mcfg*.mbn 
#
#   The program header section is relocated to immediately following the
#   elf  header.  The program code section is placed at 4K file offset.
#   Sh Hdr, Sh Hdr String Tables etc discarded
#
#   The Usage info is documented in print_usage() below.
#
# ------------------------------------------------------------------------
use strict;

sub print_usage 
{
   my $err_string  = shift;
   print "$err_string\n";
   print "mcfg_conv_elf_2_mbn.pl ELF_FILE -o MBN_FILE\n";
   print "Usage:\n";
   print <<usage;
   ELF_FILE    : Input Q6 ELF file 
   -o MBN_FILE : Output file for converted ELF file to MBN 
usage
   exit;
} 

# Args
my $elf_fname      = undef;
my $mbn_fname      = undef;
my $file_buf;
my $proghdr_offs;
my $sh_hdr_offs;
my $shnum;
my $shstrndx;
my $prog_filesz;
my $bytes_read;
my $count;
my $p_offset;
my $p_filesz;
my $p_filesz_inc;

#
# Command line parsing...
#
$count = 0;
if ($count > $#ARGV) {
   print_usage("");
}
$elf_fname = @ARGV[$count++];

while ($count < $#ARGV) {
   if (@ARGV[$count] eq "-o") {
      $count++;
      $mbn_fname = @ARGV[$count++];
   }
   else {
      print_usage("Invalid args");
   }
}

if (!defined($mbn_fname)) {
   print_usage("Missing -o arg");
}

printf("mcfg_conv_elf_2_mbn: MCFG ELF to MBN Conversion begin ...\n ");
#
# Open ELF file
#
open(elf_file, "<$elf_fname") || die("Error: could not open elf file $elf_fname.");
binmode(elf_file);

open(mbn_file, ">$mbn_fname") || die("Error: could not open mbn file $mbn_fname.");
binmode(mbn_file);

#
# Adjust Elf header to have program header table offset after elf header
# 
sysread(elf_file, $file_buf, 0x1c) == 0x1c || die("Failed to read Elf header");
# Verify Elf header ID pattern
if (0x464c457f != unpack('I', $file_buf)) {
   die ("Missing Elf header in elf file");
}
syswrite(mbn_file, $file_buf, 0x1c) || die("Write failed to mbn_file");

# Read current program header offset
sysread(elf_file, $file_buf, 0x4) == 0x4 || die("Failed to read Elf header");
$proghdr_offs = unpack('I',$file_buf);

# Set program header offset to the next byte after Elf headers
syswrite(mbn_file, pack('I', 0x34), 0x4) || die("Write failed to mbn_file");

# Read current section header offset
sysread(elf_file, $file_buf, 0x4) == 0x4 || die("Failed to read Elf header");
$sh_hdr_offs = unpack('I',$file_buf);

# Set section header offset to 0
syswrite(mbn_file, pack('I', 0x0), 0x4) || die("Write failed to mbn_file");

# Copy the remaining 12 of the Elf hdr as is to the mbn 
sysread(elf_file, $file_buf, 0x0c) == 0x0c || die("Failed to read Elf header");
syswrite(mbn_file, $file_buf, 0x0c) || die("Write failed to mbn_file");

# Read section hdr table entry count
sysread(elf_file, $file_buf, 0x2) == 0x2 || die("Failed to read Elf header");
$shnum = unpack('I',$file_buf);
# Set section hdr table entry count to 0
syswrite(mbn_file, pack('I', 0x0), 0x2) || die("Write failed to mbn_file");

# Read section hdr string table index
sysread(elf_file, $file_buf, 0x2) == 0x2 || die("Failed to read Elf header");
$shstrndx = unpack('I',$file_buf);
# Set section hdr table entry count to 0
syswrite(mbn_file, pack('I', 0x0), 0x2) || die("Write failed to mbn_file");

#
# Adjust program header table section and set it after Elf header
# 
sysseek(elf_file, $proghdr_offs, 0) || die("Failed to seek to prog header");
sysread(elf_file, $file_buf, 0x4) == 0x4 || die("Failed to read Program Header");
syswrite(mbn_file, $file_buf, 0x4) || die("Write failed to mbn_file");

# Adjust poffset to start at 4K boundry
sysread(elf_file, $file_buf, 0x4) == 0x4 || die("Failed to read prog header p_offset");
$p_offset = unpack('I',$file_buf);
# Program code starts at 4K
syswrite(mbn_file, pack('I', 4096), 0x4) || die("Write failed to mbn_file");

# Copy the rest of the prog header (p_vaddr, p_paddr)
sysread(elf_file, $file_buf, 0x8) == 0x8 || die("Failed to read prog header");
syswrite(mbn_file, $file_buf, 0x8) || die("Write failed to mbn_file");

# Ensure filesz value is modulo 4
sysread(elf_file, $file_buf, 0x4) == 0x4 || die("Failed to read prog header prog_filesz");
$p_filesz = unpack('I',$file_buf);
$prog_filesz = $p_filesz;
$p_filesz_inc = (4 - ($prog_filesz % 4));
if ($prog_filesz % 4) {
   printf("mcfg_conv_elf_2_mbn: changing filesz $prog_filesz to mod 4 value\n");
   $prog_filesz += $p_filesz_inc;
}
printf("mcfg_conv_elf_2_mbn: MCFG program code file_sz = %d\n", $prog_filesz);

# Write filesz and mem_size to mbn_file
syswrite(mbn_file, pack('I', $prog_filesz), 0x4) || die("Write failed to mbn_file");

sysread(elf_file, $file_buf, 0x4) == 0x4 || die("Failed to read prog header");
syswrite(mbn_file, pack('I', $prog_filesz), 0x4) || die("Write failed to mbn_file");

# Remaining program header, p_flags - no change
sysread(elf_file, $file_buf, 0x4) == 0x4 || die("Failed to read prog header, p_flags");
syswrite(mbn_file, $file_buf, 0x4) || die("Write failed to mbn_file");

# Read p_align to 4, change it to 4
sysread(elf_file, $file_buf, 0x4) == 0x4 || die("Failed to read prog header");
syswrite(mbn_file, pack('I', 0x4), 0x4) || die("Write failed to mbn_file");

#
# Copy download program code section to 4K offset
# 
sysseek(elf_file, $p_offset, 0) || die("Failed to seek to prog code");
sysseek(mbn_file, 4096, 0);

while ($bytes_read = sysread(elf_file, $file_buf, $p_filesz)) {
   syswrite(mbn_file, $file_buf, $bytes_read) || 
      die("Write failed to mbn_file");
   $p_filesz -= $bytes_read;
}

# fill up the remaining bytes upto $prog_filesz with 0
syswrite(mbn_file, pack('I', 0x0), $p_filesz_inc) || die("Write failed to mbn_file");

close(elf_file);
close(mbn_file);

printf("mcfg_conv_elf_2_mbn: MCFG ELF to MBN Conversion done ...\n");
exit;
