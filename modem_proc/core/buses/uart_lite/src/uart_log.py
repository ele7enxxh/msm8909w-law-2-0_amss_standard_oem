"""
/*===================================================================================================

FILE: Uart_log.py

DESCRIPTION: This module provides the script to decode the UART log file.

USAGE:
    python Uart_log.py <log file> <decoded_txt_file>

                           Copyright (c) 2014 QUALCOMM Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================
Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/uart_lite/src/uart_log.py#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
07/15/13   VV      Initial revision.

==================================================================================================*/
"""

import re
import os
import sys
import fnmatch



def get_byte(log_buf):
    byte = int(log_buf.pop(0),16)
    return byte

def get_word(log_buf):
    word = int(log_buf.pop(0),16)
    word = word | int(log_buf.pop(0),16) << 8
    return word

def get_dword(log_buf):
    dword = int(log_buf.pop(0),16)
    dword = dword | int(log_buf.pop(0),16) << 8
    dword = dword | int(log_buf.pop(0),16) << 16
    dword = dword | int(log_buf.pop(0),16) << 24
    return dword

def get_raw_msg(log_buf):
    msg = {}
    msg_start = ["0xcd","0xab","0x34","0x12"]
    i = 0
    while (i < 4) and (log_buf != []):
        if msg_start[i] != hex(get_byte(log_buf)):
            i = 0
            continue
        else:
            i = i+1

    if i!=4:
        return msg
    msg['file_id']  = get_word(log_buf)
    msg['line']     = get_word(log_buf)
    msg['num_args'] = get_word(log_buf)
    msg['msg_seq']  = get_word(log_buf)
    msg['log_level']= "INVALID"
    return msg

def get_line_content(msg,files):

    file_content = files[msg['file_id']][1]
    file_content = file_content.split("\n")
    args = msg['args']

    try:
        source_line = file_content[msg['line'] - 1]
    except:
        print "Line number error. Line number decoded: " + str(msg['line'] - 1)
        print "File ID : " + str(msg['file_id'])
       

    try:
        msg_string = re.search("(?<=\")(.*)(?=\")",source_line).group(0)
    except:
        msg['log_level'] = "INVALID"
        msg_string = "---------------"
        return msg_string

    try:
        msg['log_level'] = re.search("INFO|ERROR",source_line).group(0)
    except:
        msg['log_level'] = "INVALID"
        msg_string = "---------------"
        return msg_string

    for i in range(0,msg['num_args']):
        msg_string = re.sub("%d",str(args[i]),msg_string,1)
    return  msg_string

def parse_contents(log_buf,files,msgs):

    while log_buf != []:
        raw_msg = get_raw_msg(log_buf)
        if raw_msg == {}:
            return

        args = []
        decoded_msg = {}

        for i in range(0,raw_msg['num_args']):
            args.append(get_dword(log_buf))
        raw_msg['args'] = args

        file_name = re.search("(\w)*\.c",files[raw_msg['file_id']][0],re.I).group(0)

        decoded_msg['msg']          = get_line_content(raw_msg,files)
        decoded_msg['seq']          = raw_msg['msg_seq']
        decoded_msg['file_name']    = file_name
        decoded_msg['line_number']  = raw_msg['line']
        decoded_msg['log_level']    = raw_msg['log_level']
        msgs.append(decoded_msg)

def parse_files(files):
    curr_dir = os.getcwd()
    par_dir = re.search(r"(.*)uart\\",curr_dir).group(0)

    src_files = []
    for root, dirnames, filenames in os.walk(par_dir):
      for filename in fnmatch.filter(filenames, '*.c'):
          src_files.append(os.path.join(root, filename))
    for src in src_files:
        file_content = open(src,"r").read()
        try:
            file_trace_id = int(re.search("UART_LOGGING_FILE_ID (\d*)",file_content).group(1))
            files[file_trace_id] = [src,file_content]
        except:
            file_trace_id = 0
    return files

msgs = []
files = {}
log_file_name = sys.argv[1]
log_file = open(log_file_name,"r")

file_content = log_file.read()
log_file.close()
log_buf = re.search(r"(?<=log = \()(.*?)(?=\))",file_content,re.S)


log_buf = str(log_buf.group(0))
log_buf = log_buf.replace(" ","")
log_buf = log_buf.replace("\n","")
log_buf = log_buf.split(",")

parse_files(files)
parse_contents(log_buf,files,msgs)
sorted(msgs,key = lambda k: k['seq'] )

out_file = open(sys.argv[2],"w")
out_file.write("{:<5} {:^15} {:<15} {:<5} {:<30} \n" .format("Msg Seq.No"," Log Level", "File ", "Line", "Message "))
for i in msgs:
   out_file.write("{:<5} {:^15} {:<15} {:<5} {:<30} \n" .format(str(i['seq']) , i['log_level'] , i['file_name'] , str(i['line_number']) , i['msg']))

out_file.close()

