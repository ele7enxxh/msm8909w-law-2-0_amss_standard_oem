from glob import glob
import re
import os

#======================================================================
# ConfigError
#======================================================================

class DynError(Exception):
   def __init__(self, value):
      self.value = "Dynamic Memory Reclaim Error: " + value
   def __str__(self):
      return repr(self.value)
# End of class DynError

#get the directory name of the card
def dynrec_get_dir (card_dir):
    card_absdir = os.path.abspath(card_dir)
    #print(card_absdir)

    dynrec_dir = re.search(r'(modem_proc.*)', card_absdir, re.M|re.I)
    #print(dynrec_dir.group())

    if (dynrec_dir):
        dynrec_card_dir = dynrec_dir.group().replace('\\', '/')
        return(dynrec_card_dir)
#end of dynrec_get_dir

#write the text header of the dynrec.list file
def dynrec_write_text_header ():
    f_dynrec.write("/******************************************************************************\n")
    f_dynrec.write("                                TEXT and RODATA\n")
    f_dynrec.write("******************************************************************************/\n")
#end of dynrec_write_text_header

#write the data header of the dynrec.list file
def dynrec_write_data_header ():
    f_dynrec.write("/******************************************************************************\n")
    f_dynrec.write("                                DATA\n")
    f_dynrec.write("******************************************************************************/\n")
#end of dynrec_write_data_header

#write a text entry in the dynrec.list file
def dynrec_write_text_entry (card_id, card_dir):
    text_start = "__dynrec_text_start_" + str(card_id) + " = .;\n"
    f_dynrec.write(text_start)
    dynrec_dir = dynrec_get_dir(card_dir)
    f_dynrec.write("*" + dynrec_dir + "/?*:(.text .text.* .rodata .rodata.*)\n")
    f_dynrec.write(". = ALIGN(4K);\n")
    text_end = "__dynrec_text_end_" + str(card_id) + " = .;\n"
    f_dynrec.write(text_end)
    f_dynrec.write("\n")
    #print(text_start)
#end of dynrec_write_text_entry

#write a data entry in the dynrec.list file
def dynrec_write_data_entry (card_id, card_dir):
    data_start = "__dynrec_data_start_" + str(card_id) + " = .;\n"
    f_dynrec.write(data_start)
    dynrec_dir = dynrec_get_dir(card_dir)
    f_dynrec.write("*" + dynrec_dir + "/?*:(.data .data.*)\n")
    f_dynrec.write(". = ALIGN(4K);\n")
    data_end = "__dynrec_data_end_" + str(card_id) + " = .;\n"
    f_dynrec.write(data_end)
    f_dynrec.write("\n")
    #print(data_start)
#end of dynrec_write_text_entry

#get the directory name of the card
def dynrec_find_hwid (card_fold_name, rfc_ids):
    dynrec_card_dir = card_fold_name.replace('\\', '/')
    searchObj = re.search(r'rfc_(wtr.*?)/', dynrec_card_dir, re.M|re.I)
    if searchObj:
        rfc_card = searchObj.group(1)

        #print ("card_fold_name:"+card_fold_name)
        #print ("rfc_card:"+rfc_card)
        rfc_card_defined_hwid = 0

        # whether the rf_card exits in hwid file
        for rfc_id_def in rfc_ids:
            #print ("rfc_id_def:"+rfc_id_def)
            rfc_card_exist = re.search(r'(rf.*) = \(uint8\)(\d+)', rfc_id_def, re.M|re.I)
            if rfc_card_exist:
                #print rfc_card_exist.group()
                rfc_idname_s = rfc_card_exist.group(1)
                rfc_idname = rfc_idname_s.strip()

                rfc_hwid_defined = re.search(r'%s$'%rfc_card, rfc_idname, re.M|re.I);

                if rfc_hwid_defined:
                    #print rfc_hwid_defined.group()
                    rfc_hwid = rfc_card_exist.group(2)
                    #print rfc_hwid
                    rfc_card_defined_hwid = 1
                #else:
                    #raise DynError("rfc card: " + rfc_idname + " hwid not defined in rfc_hwid.h file\n")

        if rfc_card_defined_hwid == 1:
            #print rfc_card
            rfc_card_id = "rfc_" + rfc_hwid
            return rfc_card_id
        else:
            return 0
#end dynrec_find_hwid

#main function
def dynamic_mem_list ( ):
    rfc_card_fold_list = []
    rfc_card_id_list = []

    f_hwid = open('../../api/rfc_hwid.h', 'r')
    rfc_ids = f_hwid.readlines()
    f_hwid.close()

    paths = glob('../rfc_*/')

    print paths

    #check every single folder
    for tfc_c_fold in paths:
        #print tfc_c_fold
        #print(os.path.abspath(tfc_c_fold))

        dynrec_id = dynrec_find_hwid(tfc_c_fold, rfc_ids)
        if dynrec_id != 0:
            rfc_card_id_list.append([dynrec_id, tfc_c_fold])
            print "dynrec_id:" + str(dynrec_id)

    #add unspecified at the end
    rfc_card_id_list.append(["unspecified", "../"])

    #generate the dynrec.lst under modem_proc/build/ms
    global f_dynrec
    f_dynrec = open('../../../build/ms/dynrec.lst', 'w')

    dynrec_write_text_header ()
    for rfc_card_id_item in rfc_card_id_list:
        rfc_card_id = rfc_card_id_item[0]
        fold_path = rfc_card_id_item[1]
        dynrec_write_text_entry(rfc_card_id, fold_path)

    dynrec_write_data_header ()
    for rfc_card_id_item in rfc_card_id_list:
        rfc_card_id = rfc_card_id_item[0]
        fold_path = rfc_card_id_item[1]
        dynrec_write_data_entry(rfc_card_id, fold_path)

    f_dynrec.close()


