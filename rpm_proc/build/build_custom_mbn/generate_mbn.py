#============================================================================
#
#/** @file mbn_builder.py
#
# GENERAL DESCRIPTION
#   Creates mbn file for the RPM elf passed as argument
#
#============================================================================
import os
import sys
sys.path.append(os.path.dirname(os.path.abspath(__file__))+'/'+ '../..' + '/tools/build/scons/sectools/sectools/features/isc/parsegen')
import mbn_tools




if len(sys.argv) < 5:
  print "Incorrect Usage: generate_mbn.py <source file> <target file> <image_destination> <mbn type> <opt: elf_type>"
  print "Example: generate_mbn.py ..\..\core\bsp\rpm\build\RPM_AAAAANAAR.elf RPM.mbn 0xFC100080 elf"
  raise RuntimeError, "Usage: generate_mbn.py <source file> <target file> <image_destination> <mbn type> <opt: elf_type>" 

gen_dict = {}
source_full = sys.argv[1]
target_full = sys.argv[2]
hash_seg_size = None
last_phys_addr = None
mbn_type = sys.argv[4]

# Attempt to evaluate value
try:
   image_destination = eval(sys.argv[3])
# Catch exceptions and do not evaluate
except:
   raise RuntimeError, "Invalid image destination address"


source_base = os.path.splitext(str(source_full))[0]
target_base = os.path.splitext(str(target_full))[0]
header_format = 'reg' 
gen_dict['IMAGE_KEY_IMAGE_ID'] = mbn_tools.ImageType.RPM_IMG 
gen_dict['IMAGE_KEY_IMAGE_SOURCE'] = 0
gen_dict['IMAGE_KEY_IMAGE_DEST'] = image_destination
gen_dict['IMAGE_KEY_MBN_TYPE'] = mbn_type
image_header_secflag = 'non_secure'
image_size = os.stat(source_full).st_size



#----------------------------------------------------------------------------
# Generate  mbn
#----------------------------------------------------------------------------

# MBN is generated in ELF format 
if mbn_type == 'elf':

    target_hash = target_base + ".hash"
    target_hash_hd = target_base + "_hash.hd"
    target_cert_chain = target_base + "_hash_cert_chain.mbn"
    target_hash_full = target_base + "_hash_sec.mbn"
    target_phdr_elf = target_base + "_phdr.pbn"
    target_add_hash_elf = target_phdr_elf
    target_pre_encrypt_elf = target_base + "_pre_encrypt.pbn"
    target_encrypt_xml = target_base + "_enc_md.xml"

    # Create elf header for RPM
    rv = mbn_tools.pboot_gen_elf([], source_full, target_hash, 
                                 elf_out_file_name = target_phdr_elf,
                                 secure_type = image_header_secflag)        
    if rv:
       raise RuntimeError, "Failed to run pboot_gen_elf" 
      
	  
	  
    if not rv:
       rv = mbn_tools.image_header([], gen_dict, target_hash, target_hash_hd, 
                                   image_header_secflag, elf_file_name = source_full)
       target_nonsec = target_base + "_combined_hash.mbn"
       target_nonsec_xml = target_base + "_combined_hash_xml.mbn"
       files_to_cat_in_order = [target_hash_hd, target_hash]
       mbn_tools.concat_files (target_nonsec, files_to_cat_in_order)

       #Add the hash segment into the ELF 
       mbn_tools.pboot_add_hash([], target_add_hash_elf, target_nonsec, target_full)
else :
    
	print "Enter mbn_type as elf"
