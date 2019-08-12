#============================================================================
#
# GENERAL DESCRIPTION
#    Used to create SSD binaries either in flat-format or image .elf format
#
# Copyright  2013 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#
#  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/ssd/tools/gen_ssd_bin.py#1 $
#  $DateTime: 2016/06/17 14:31:11 $
#  $Author: pwbldsvc $
#  $Change: 10727476 $
#
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who       what, where, why
# --------   ---       ------------------------------------------------------
# 04/10/12    hv       Initial revision
#
#============================================================================
import ConfigParser
import os
import binascii
import shutil
import sys
import getopt
import __main__
import stat
import tempfile
import traceback
import hashlib
from subprocess import PIPE
from subprocess import Popen
import key_config_parser

#----------------------------------------------------------------------------
# GLOBAL VARIABLES BEGIN
#----------------------------------------------------------------------------
PAD_BYTE_1      = 255
IMG_START_ALIGN = 4     #< Image should be word aligned

#----------------------------------------------------------------------------
# Copy key files from remote location if specified
#----------------------------------------------------------------------------
class SSDConfigClass:

   # Tag values
   IMG_ENC_PADDING_TYPE   = "NO_PAD"
   IMG_ENC_OPERATION_MODE = "CBC_MODE"
   IMG_HASH_ALGO          = "SHA-256"
   MD_SIG_DGST_ALGO       = "SHA-256"
   MD_SIG_ALGO            = "RSA-2048"
   MD_SIG_PADDING_TYPE    = "PKCS#1-V1.5"

   # class globals
   CONFIG_BUFF_SIZE                = 4096
   IEK_ENC_PUB_KEY_ID_BUFF_SIZE    =  512
   IEK_CIPHER_VALUE_SIZE           = 1536
   IMG_ENC_IV_BUFF_SIZE            =   32
   MD_SIG_OEM_PUB_KEY_ID_BUFF_SIZE =  512
   MD_SIGNATURE_SIZE               = 1536

   # defined at init
   CONFIG_FILE = ""

   # parsed from INI file
   MD_VERSION_VAL   = ""
   MFG_ID_VAL       = ""
   SW_VERSION_VAL   = ""
   IMG_ENC_ALGO_VAL = ""
   

   IEK_BIN = ""
   IV_BIN  = ""

   # other local variables
   config_p = ConfigParser.ConfigParser()

   # open the file & read the data
   def __init__ (self, tools_dir, keycfg):
      try:
         if keycfg == "":
            keycfg = os.path.join(tools_dir, "key_config.xml")
            print("INFO: --keys option not given.  ")
            print("INFO: Continuing with default configuration file: " + 
                  keycfg)


         config_file = os.path.join(tools_dir, "ssd_bin.cfg")

         # temporary directories 
         tdir = os.path.join(tools_dir, __main__.__file__ + "_temp" ) 
         self.TEMP_F_DIR = tdir

         # temporary files
         self.IV_BIN_FNAME = os.path.join(tdir, "iv.bin")
         self.IEK_BIN_FNAME = os.path.join(tdir, "iek.bin")
         self.SSD_MD_TO_SIGN_FNAME = os.path.join(tdir, "to_sign.xml")

         self.config_p.readfp(open(config_file))

         # Information in the config file is needed for generating the MD
         self.parse_config_file()

         if (self.IEK_ENC_ALGO == "RSA-2048"):
            self.dvc_key_fn = key_config_parser.get_rsa_pub_dvc_key(keycfg)
            self.dvc_key_id = key_config_parser.get_rsa_dvc_key_id_buf(keycfg)
         elif (self.IEK_ENC_ALGO == "AES-128"):
            self.dvc_key_fn = key_config_parser.get_aes_dvc_key(keycfg)
            self.dvc_key_id = key_config_parser.get_aes_dvc_key_id_buf(keycfg)
         else:
            print("Error: Unsupported IEK_ENC_ALGO from config")
            exit(1)

         self.oem_key_fn = key_config_parser.get_rsa_pri_oem_key(keycfg)
         self.oem_key_id = key_config_parser.get_rsa_oem_key_id_buf(keycfg)

         if (self.dvc_key_fn == '' or self.dvc_key_id == '' or
             self.oem_key_fn == '' or self.oem_key_id == ''):
            print("Error: Key config not correct")
            exit(1)

         # Remove previous temporary directory
         if True == os.path.exists(tdir) and True == os.path.isdir(tdir):
            shutil.rmtree(tdir)

         # Create temp directory for storing all temp files
         os.makedirs(tdir)

         # Initialization for encrypting&signing
         self.init_enc();
         self.init_sign('','');

      except:
         print "Failed during init"
         exc_type, exc_value, exc_traceback = sys.exc_info()
         print "*** print_tb:"
         traceback.print_tb(exc_traceback, limit=1, file=sys.stdout)
         print "*** print_exception:"
         traceback.print_exception(exc_type, exc_value, exc_traceback,
                                   limit=2, file=sys.stdout)
         sys.exit(2)         

   def buff_to_file(self, buff_in, out=''):
      # Use for Python 2.6
      # f = tempfile.NamedTemporaryFile(delete=False)
      if out == '':
         fd,fn = tempfile.mkstemp()
         f = os.fdopen(fd,"wb")
      else:
         fn = out
         f = open(out,"wb")
      f.write(buff_in)
      f.close()
      return(fn)
   def file_to_buff(self, fname):
      f = open(fname,"rb")
      buff_out = f.read()
      f.close()
      return(buff_out)
   def output_file(self):
      return(self.buff_to_file(""))

   def clean_file(self, fname):
      os.remove(fname)

   # get the tag from the file
   def get_config_tag_value(self, section, tag):
      try:
         tag_val = self.config_p.get(section, tag)
         return tag_val
      except:
         return ""

   # parse the config file values
   def parse_config_file(self):
      self.MD_VERSION_VAL = self.get_config_tag_value("QC_DATA", "MD_VERSION")
      self.MFG_ID_VAL = self.get_config_tag_value("OEM_CONFIG", "MFG_ID")
      self.SW_VERSION_VAL = self.get_config_tag_value("OEM_CONFIG",
                                                      "SW_VERSION")
      self.IMG_ENC_ALGO_VAL = self.get_config_tag_value("OEM_CONFIG",
                                                        "IMG_ENC_ALGO")
      self.IEK_ENC_ALGO = self.get_config_tag_value("OEM_CONFIG", 
                                                    "IEK_ENC_ALGO")
      self.IEK_ENC_PADDING_TYPE = self.get_config_tag_value("OEM_CONFIG", 
                                                      "IEK_ENC_PADDING_TYPE")
      return

   # TODO: isn't os.urandom good enough for key/iv generation?
   def gen_16_byte_rand_buf(self):

      # openssl weirdness
      randenv = os.environ
      openssl_rnd = os.path.join(self.TEMP_F_DIR, ".rnd")
      randenv["RANDFILE"] = openssl_rnd

#      dump = open(os.devnull, "w")
      dump = sys.stderr

      # TODO: tempfile doesn't work for this on windows.  It causes
      # errors in "openssl rand" 
      seed_fn = os.path.join(self.TEMP_F_DIR, "rand.bin")
      with open(seed_fn, "wb") as seed_fo:
         seed_fo.write(os.urandom(1024))

      p = Popen(["openssl", "rand", "16", "-rand", seed_fn], 
                stdout = PIPE, stderr = dump, env=randenv )
      rand = p.communicate()[0]

      self.clean_file(seed_fn)
      self.clean_file(openssl_rnd)

      return rand[:16]


   def gen_aes_128_key(self):
      return self.gen_16_byte_rand_buf()

   def gen_aes_128_iv(self):
      return self.gen_16_byte_rand_buf()


   def gen_xml_to_sign_file(self):
      fd = open(self.SSD_MD_TO_SIGN_FNAME, 'w')

      fd.write("<MD_SIGN>")
      fd.write("<MD_VERSION>" + self.MD_VERSION_VAL + "</MD_VERSION>")
      fd.write("<MFG_ID>" + self.MFG_ID_VAL + "</MFG_ID>")
      fd.write("<SW_VERSION>" + self.SW_VERSION_VAL + "</SW_VERSION>")
      fd.write("<IMG_ENC_INFO>")
      fd.write("<IMG_ENC_ALGO>" + self.IMG_ENC_ALGO_VAL + "</IMG_ENC_ALGO>")

      # Skip everything if the IMG is not encrypted
      if "NULL" == self.IMG_ENC_ALGO_VAL:
         fd.write("</IMG_ENC_INFO></MD_SIGN>")
         fd.close()
         return

      fd.write("<IMG_ENC_PADDING_TYPE>" + self.IMG_ENC_PADDING_TYPE +
               "</IMG_ENC_PADDING_TYPE>")

      fd.write("<IMG_ENC_OPERATION_MODE>" + self.IMG_ENC_OPERATION_MODE +
               "</IMG_ENC_OPERATION_MODE>")

      fd.write("<IMG_ENC_IV>" +  binascii.b2a_base64(self.IV_BIN).rstrip() +
               "</IMG_ENC_IV>")

      fd.write("</IMG_ENC_INFO>")

      fd.write("<IEK_ENC_INFO>")
      fd.write("<IEK_ENC_ALGO>" + self.IEK_ENC_ALGO + "</IEK_ENC_ALGO>")
      fd.write("<IEK_ENC_PADDING_TYPE>" + self.IEK_ENC_PADDING_TYPE +
               "</IEK_ENC_PADDING_TYPE>")
      fd.write("<IEK_ENC_PUB_KEY_ID>" + 
               binascii.b2a_base64(self.dvc_key_id).rstrip() + 
               "</IEK_ENC_PUB_KEY_ID>")

      # encrypt the IEK with Device public key
      iek_fn = self.buff_to_file(self.IEK_BIN)

#      dump = open(os.devnull, "w")
      dump = sys.stderr

      enc_iek = ''

      if (self.IEK_ENC_ALGO == "RSA-2048"):
         # TODO: update "-pkcs" to "-oaep" (must also update the
         # iek_enc_padding_type)
         p = Popen(["openssl", "rsautl", "-encrypt", "-pkcs", "-in",
                    iek_fn, "-inkey", self.dvc_key_fn, "-pubin",
                    "-keyform", "DER"], stdout = PIPE) #, stderr = dump)
         enc_iek = p.communicate()[0]
      elif (self.IEK_ENC_ALGO == "AES-128"):
         key = ''
         with open(self.dvc_key_fn, "rb") as key_fo:
            key = key_fo.read().rstrip()
         iv = self.gen_aes_128_iv()
         keyhex = binascii.hexlify(key)
         ivhex = binascii.hexlify(iv)

         p = Popen(["openssl", "enc", "-aes-128-cbc", "-e", "-K", 
                    keyhex, "-iv", ivhex, "-nopad", "-in", iek_fn], 
                   stdout = PIPE) #, stderr = dump)
         enc_iek = p.communicate()[0]

         fd.write("<IEK_ENC_IV>" + 
                  binascii.b2a_base64(iv).rstrip() + 
                  "</IEK_ENC_IV>")

      fd.write("<IEK_CIPHER_VALUE>" + 
               binascii.b2a_base64(enc_iek).rstrip() + "</IEK_CIPHER_VALUE>")

      fd.write("</IEK_ENC_INFO>")

      fd.write("<MD_SIG_INFO>")

      fd.write("<MD_SIG_ALGO>" + self.MD_SIG_ALGO + "</MD_SIG_ALGO>")
      fd.write("<MD_SIG_DGST_ALGO>" + self.MD_SIG_DGST_ALGO +
               "</MD_SIG_DGST_ALGO>")
      fd.write("<MD_SIG_PADDING_TYPE>" + self.MD_SIG_PADDING_TYPE +
               "</MD_SIG_PADDING_TYPE>")
      fd.write("<MD_SIG_OEM_PUB_KEY_ID>" + 
               binascii.b2a_base64(self.oem_key_id).rstrip() +
               "</MD_SIG_OEM_PUB_KEY_ID>")


      fd.write("</MD_SIG_INFO>")
      fd.write("</MD_SIGN>")
      fd.close()

   def gen_signed_ssd_xml(self, op_fn):
      self.gen_xml_to_sign_file()
      try:
         fd_final = open(op_fn, 'wb')

         # write the XML header tag
         fd_final.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>")

         # Write top level SSD Metadata tag
         fd_final.write("<SSD_METADATA>")

         # read the XML & write it to Final XML
         fd_to_sign = open(self.SSD_MD_TO_SIGN_FNAME, 'r')

         # write the signed XML file to final XML file
         fd_final.write(fd_to_sign.read())

         # close the signed file so it can be used for signing
         fd_to_sign.close()

         fd_final.write("<MD_SIGNATURE>")

         # generate the signature over the intermediate XML
         # call the RSA sign function with the OEM private key
         #
         # run SHA-256 hash over the intermediate XML file
         digest = ''

         with open(self.SSD_MD_TO_SIGN_FNAME, "rb") as input_fo:
            digest = hashlib.sha256(input_fo.read()).digest()

         der_oem_key_fn = "oem.der"

         # convert the oem key before signing
         p = Popen(["openssl", "pkcs8", "-inform", "DER", "-outform", "DER", 
                    "-nocrypt", "-in", self.oem_key_fn, "-out", der_oem_key_fn])
         p.wait()

         # sign the Hash
         p = Popen(["openssl", "rsautl", "-pkcs", "-sign", "-keyform", "DER",
                    "-inkey", der_oem_key_fn], 
                   stdout = PIPE, stdin = PIPE)

         sig = p.communicate(digest)[0]

         # Remove the intermediate key file
         self.clean_file(der_oem_key_fn)
         
         # covert the Signature to b64 format and write to output
         fd_final.write(binascii.b2a_base64(sig).rstrip())

         # Finish the XML
         fd_final.write("</MD_SIGNATURE>")
         fd_final.write("</SSD_METADATA>")

         # Insert alignment after metadata to align the image on word
         # boundary
         ssd_xml_len = fd_final.tell()

         if (ssd_xml_len % IMG_START_ALIGN > 0):
            bytes_to_pad = IMG_START_ALIGN - ssd_xml_len % IMG_START_ALIGN;
         else: 
            bytes_to_pad = 0

         while bytes_to_pad > 0:
            fd_final.write('%c' % PAD_BYTE_1)
            bytes_to_pad -= 1

         # close output file
         fd_final.close()
         return

      except:
         print"Error in gen_signed_ssd_xml()"
         sys.exit(2)

   def _to_hex_string(self, int_):

      hex_values = { 0: '0', 1: '1', 2: '2', 3: '3',
                     4: '4', 5: '5', 6: '6', 7: '7',
                     8: '8', 9: '9',10: 'a',11: 'b',
                    12: 'c',13: 'd',14: 'e',15: 'f'}

      MSB = int_/16
      LSB = int_%16

      return hex_values.get(MSB) + hex_values.get(LSB)

   def enc_segment(self, 
                   seg_no,
                   pt_fn,
                   op_fn):

      op_fp = open(op_fn, "wb")
      pt_fp = open(pt_fn, "rb")

      pt_buf = pt_fp.read()
      pt_buf_len = os.path.getsize(pt_fn)

      # how much data are we going to encrypt
      data_to_enc = pt_buf_len - (pt_buf_len % 16)

      tip_fn = os.path.join(self.TEMP_F_DIR, "temp_ip.dat")
      top_fn = os.path.join(self.TEMP_F_DIR, "temp_op.dat")

      temp_ip = open(tip_fn, "wb")
      temp_ip.write(pt_buf[:data_to_enc])
      temp_ip.close()

      # Accept segment numbers up to 32 bits, force endian
      seg_no_1 =  seg_no      & 0x000000ff
      seg_no_2 = (seg_no>>8)  & 0x000000ff
      seg_no_3 = (seg_no>>16) & 0x000000ff
      seg_no_4 = (seg_no>>24) & 0x000000ff
      seg_iv = hashlib.sha256(self.IV_BIN + 
                              chr(seg_no_4) + 
                              chr(seg_no_3) + 
                              chr(seg_no_2) + 
                              chr(seg_no_1)).digest()[:16]


      os.system("openssl enc -aes-128-cbc -in " + tip_fn + " -K " +
                binascii.hexlify(self.IEK_BIN).rstrip() + " -iv " +
                binascii.hexlify(seg_iv).rstrip() +
                " -out " + top_fn + " -nopad" )

      temp_op = open(top_fn, "r+b")
      temp_op.seek(0, os.SEEK_END)
      temp_op.write(pt_buf[data_to_enc:])
      temp_op.seek(0,0)
      temp_op_size = os.path.getsize(top_fn)
      ct_buf = temp_op.read(temp_op_size)
      temp_op.close()

      op_fp.write(ct_buf)
      op_fp.close()

      return
   def init_enc(self):
      pass

   def init_sign(self, iek_fn, iv_fn):
      # Load IEK&IV BINs from files if specified
      if (iek_fn != '' and iv_fn != ''):
         self.IEK_BIN = self.file_to_buff(iek_fn)
         self.IV_BIN = self.file_to_buff(iv_fn)
      else:
         self.IEK_BIN = self.gen_aes_128_key()
         self.IV_BIN = self.gen_aes_128_iv()

   def deinit(self, save_iv_and_key):
      iek_fn  = os.path.join(os.getcwd(),
                            os.path.basename(self.IEK_BIN_FNAME))
      iv_fn = os.path.join(os.getcwd(),
                            os.path.basename(self.IV_BIN_FNAME))

      if save_iv_and_key:
         self.buff_to_file(self.IV_BIN, iv_fn)
         self.buff_to_file(self.IEK_BIN, iek_fn)
      else:
         try:
            os.remove(iv_fn)
            os.remove(iek_fn)
         except OSError:
            pass

      shutil.rmtree(self.TEMP_F_DIR)

      return

   def copy_keys(self, key_conf, copy_oem):
      return

def usage ():
   print(
"===============================================================================\n"
"Parameters:\n"
"--help (-h)     Prints this help.\n"
"--pt_file       File to encrypt.\n"
"--ct_file       Encrypted file.\n"
"--op_file       Output file.\n"
"--tools_dir     Location of the SSD tools folder.  Required if running script\n"
"                from outside of SSD folder.\n"
"--keys          Location of key configuration file to use for encryption.\n"
"--enconly       Used to encrypt a binary and generate an IV and IEK. Use \n"
"                before --signonly.\n"
"--signonly      Signed XML and combine with encrypted image.  Use after \n"
"                --enconly\n"
"--iek_fname     Image encryption key to use for encrypting.  Only used with \n"
"                --signonly parameter.  File is deleted after use.\n"
"--iv_fname      IV to use when encrypting.  Only used with --signonly parameter.\n"
"                File is deleted after use.\n"
"--seg_no        Segment number to use for the encryption (debugging only)\n\n"
"Usage:\n"
"Generate metadata, sign and encrypt in one step:\n"
 "$ " + __main__.__file__ + " --pt_file=<file_to_encrypt> " +
 "--op_file=<output_file_name> --tools_dir=<ssd_tools_root_dir> " +
 "--keys=<path_to_key_config>\n\n"

"Generate IV and IEK, then encrypt binary:\n"

"$ " + __main__.__file__ + " --enconly --pt_file=<file_to_encrypt> "
"--op_file=<output_of_script> --keys=<path_to_key_config>\n\n"


"Generate MD, sign MD, and append binary to signed metadata:\n"

"$ " + __main__.__file__ + " --signonly --ct_file=<metadata_file_name> "
"--iv_fname=<iv_file_name> "
"--iek_fname=<iek_file_name> --op_file=<output_file_name>\n\n"

"Examples:\n"

"$ " + __main__.__file__ + " --pt_file=c:\\common\\builds\\NON-HLOS.bin" 
" --op_file=c:\\common\\builds\\NON-HLOS.enc "
"--tools_dir=c:\common\\tools\\ssd\\tools\"\n\n"

"=============================================================================="
)

def main():
   pt_fn     = ""
   op_fn     = ""
   ct_fn     = ""
   iek_fn    = ""
   iv_fn     = ""
   seg_no    = 0
   key_conf  = ""
   tools_dir = ""
   do_sign   = True
   do_enc    = True

   try:
      opts, args = getopt.getopt(sys.argv[1:], "h",
                                 ["help", "pt_file=", "ct_file=", "op_file=",
                                  "tools_dir=", "keys=", "signonly", "enconly",
                                  "iek_fname=", "iv_fname=", "seg_no="])
   except getopt.GetoptError, err:
      # print help information and exit:
      print str(err) # will print something like "option -a not recognized"
      usage()
      sys.exit(2)

   if len(sys.argv) < 2:
      usage()
      sys.exit(2)

   for o, a in opts:
      if o in ("-h", "--help"):
         usage()
         sys.exit()
      elif o in ("--pt_file"):
         pt_fn = a
      elif o in ("--ct_file"):
         ct_fn = a
      elif o in ("--op_file"):
         op_fn = a
      elif o in ("--iek_fname"):
         iek_fn = a
      elif o in ("--iv_fname"):
         iv_fn = a
      elif o in ("--seg_no"):
         seg_no = a
      elif o in ("--tools_dir"):
         tools_dir = a
      elif o in ("--keys"):
         key_conf = a
      elif o in ("--signonly"):
         do_enc = False
      elif o in ("--enconly"):
         do_sign = False


   print("========================================" +
         "=======================================")

   if do_enc:
      if pt_fn == "":
         print("ERROR: Missing --pt_file parameter")
         usage()
         sys.exit(2)
      elif not os.path.exists(pt_fn):
         print("ERROR: " + pt_fn + " does not exist.")
         sys.exit(2)

   if op_fn == "":
      print("ERROR: Missing --op_file parameter")
      usage()
      sys.exit(2)

   if not do_enc and not do_sign:
      print("ERROR: Both --signonly and --enconly parameters passed")
      usage()
      sys.exit(2)

   elif do_sign and not do_enc:
      # check usage
      if iek_fn == "" or iv_fn == "":
         print("ERROR: Both --iek_fname and --iv_fname parameters are "
               "required when using --signonly parameter.")
         sys.exit(2)
      elif not os.path.exists(iek_fn):
         print("ERROR: " + iek_fn + " does not exist.")
         sys.exit(2)
      elif not os.path.exists(iv_fn):
         print("ERROR: " + iv_fn + " does not exist.")
         sys.exit(2)

      if ct_fn == "":
         print("ERROR: Missing --ct_file parameter")
         usage()
         sys.exit(2)

      if not os.path.exists(ct_fn):
         print("ERROR: " + ct_fn + " does not exist.")
         sys.exit(2)

      if pt_fn != "":
         print("INFO: --pt_file parameter ignored for sign only option")

   elif do_enc and not do_sign:
      # for encrypt only option remove any previous existing file with
      # same name as output file otherwise the encrypted data will
      # append to it
      if os.path.exists(op_fn):
         os.remove(op_fn)


   if tools_dir == "":
      tools_dir = os.getcwd()

   # Initialize SSD Config Pointer
   ssd_p = SSDConfigClass(tools_dir, key_conf)

   # Make a local copy of the keys
#   ssd_p.copy_keys(key_path, do_sign)

   # Encryption will generate ct_fn 
   # Signing will generate md_fn
   # outputs will be combined at the end
   if do_enc:
      print("========================================" +
            "=======================================")
      ssd_p.init_enc()
      # If not ciphertext output file, specified use a temp file
      if(ct_fn == ''):
         ct_fn = ssd_p.output_file()

   if do_sign:
      print("========================================" +
            "=======================================")
      print("Generating or Loading IEK and IV")
      print("========================================" +
            "=======================================")
      # If iek_fn and iv_fn are not blank, keys are loaded in
      ssd_p.init_sign(iek_fn, iv_fn)
      md_fn = ssd_p.output_file()


   # Need to encrypt after init_sign since that is where the
   # IEK/IVs are generated or loaded from file
   if do_enc:
      print("========================================" +
            "=======================================")
      print("Encrypting: " + pt_fn + " with seg_no = " + str(seg_no))
      print("========================================" +
            "=======================================")
      ssd_p.enc_segment(int(seg_no), pt_fn, ct_fn)

   if do_sign:
      print("========================================" +
            "=======================================")
      print("Generating Metadata and Signing")
      print("========================================" +
            "=======================================")
      # Generate the SSD Metadata that will be signed and place it in a file
      # and sign the generated xml
      ssd_p.gen_signed_ssd_xml(md_fn)


   op_fp = open(op_fn, 'wb')
   ct_fp = open(ct_fn, 'rb')
   if do_enc and not do_sign:
      # Only dump the ciphertext if not signing
      op_fp.write(ct_fp.read())
      ct_fp.close()
      op_fp.close()
   elif do_sign:
      # Write MD followed by CT if doing both
      md_fp = open(md_fn, 'rb')
      op_fp.write(md_fp.read())
      op_fp.write(ct_fp.read())
      md_fp.close()
      os.remove(md_fn)
   ct_fp.close()
   op_fp.close()
   # if enc&sign, this removes the temp file
   # if signonly, this removes the given ct_file
   os.remove(ct_fn)

   # clean up temporary files
   ssd_p.deinit(do_enc and not do_sign)

   if do_enc:
      print("Success.")
      print("Encrypted Image: " + op_fn)
      if not do_sign:
         print("Image Encryption Key File: " + 
               os.path.basename(ssd_p.IEK_BIN_FNAME))
         print("IV File: " + os.path.basename(ssd_p.IV_BIN_FNAME))
      print("========================================" +
            "=======================================")
   elif do_sign:
      print("========================================" +
            "=======================================")
      print("Success.")
      print("Signed Metadata: " + op_fn)
      if not do_enc:
         print("Intermediate files deleted: " + os.path.basename(ct_fn) + 
               ", " + os.path.basename(ssd_p.IEK_BIN_FNAME) + ", and " + 
               os.path.basename(ssd_p.IV_BIN_FNAME) + ".");
      print("========================================" +
            "=======================================")

if __name__ == "__main__":
    main()
