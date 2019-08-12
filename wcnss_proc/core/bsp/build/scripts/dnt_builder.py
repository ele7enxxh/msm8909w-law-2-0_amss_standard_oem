#===============================================================================
#
# CBSP DnT Builders build rules
#
# GENERAL DESCRIPTION
#    Contains builder(s) defenitions
#
# Copyright (c) 2009-2009 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/core.wcnss/4.2.4/bsp/build/scripts/dnt_builder.py#1 $
#  $DateTime: 2015/07/22 20:31:53 $
#  $Author: pwbldsvc $
#  $Change: 8650711 $
#                      EDIT HISTORY FOR FILE
#                      
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#  
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
#
#===============================================================================
import csv
import os
import subprocess
import string
import re
import hashlib
import fnmatch
from SCons.Script import *
from operator import itemgetter

#------------------------------------------------------------------------------
# Globals
#------------------------------------------------------------------------------
RCINIT_DEPENDENCIES = 'dependencies'
RCINIT_DEPENDENCIES_FILTER = 'dependencies_filter'
RCINIT_INTERNAL_ORDER = '_order'
THREAD_NAME = 'thread_name'
STACK_NAME = 'stack_name'
STACK_SIZE = 'stack_size_bytes'
PRIORITY_AMSS = 'priority_amss_order'
THREAD_ENTRY = 'thread_entry'
CPU_AFFINITY = 'cpu_affinity'
THREAD_TCB = 'tcb_name'
THREAD_INIT = 'init_function'
INIT_NAME = 'init_name'
SEQUENCE_LEVEL = 'sequence_group'
THREAD_TYPE = 'thread_type'
THREAD_PTHREAD = 'RCINIT_THREAD_PTHREAD'
THREAD_REX = 'RCINIT_THREAD_REXTASK'

DEBUG_THREAD_NAME = 'thread_name'
DEBUG_PRIORITY = 'prio'
DEBUG_SEQUENCE_LEVEL = 'sequence_level'
DEBUG_STACK_SIZE = 'stack_size'

RCINIT_NULL = 'RCINIT_NULL'
RCINIT_ENV_DESCRIPTOR = 'RCINIT_INFO_COLLECTOR'
RCINIT_ENABLED_DESCRIPTOR = 'RCINIT_ENABLED'
RCINIT_LEGACY_FILE_PTR = 'RCINIT_LEGACY_FILE_PTR'
RCINIT_PRIO_FILE_PTR = 'RCINIT_PRIO_FILE_PTR'
RCINIT_STKSZ_FILE_PTR = 'RCINIT_STKSZ_FILE_PTR'
RCINIT_GOVERN_FILE_PTR = 'RCINIT_GOVERN_FILE_PTR'

LEGACY_PRIO_DICT = 'LEGACY_PRIO_DICT'
LEGACY_STKSZ_DICT = 'LEGACY_STKSZ_DICT'
LEGACY_PRIO_UNCOUPLED_LIST = 'LEGACY_PRIO_UNCOUPLED_LIST'
LEGACY_FEATURE_DICT = 'LEGACY_FEATURE_DICT'
LEGACY_PRIO_LIST = 'LEGACY_PRIO_LIST'

# REQUIRED FILES
RCINIT_LEGACY_INFO_FILE = "rcinit_legacy_info.csv"
RCINIT_PRIORITY_FILE = "rcinit_task_prio.csv"
RCINIT_GOVERN_FILE = "rcinit_task_govern.csv"

#output log files
RCINIT_PRIORITY_LOG = "rcinit_priority_log.txt"
RCINIT_OUTPUT_LOG = "rcinit_output_log.txt"
RCINIT_DICT = "rcinit.dict"


#index into file pointer array
RCINIT_INFO_INDEX = 0
RCINIT_PRIORITY_INDEX = 1

#dict that defines possible run levels
MAX_GRP_LVL = 8

RCINIT_RUN_LEVEL_CONSTANTS = {
    'RCINIT_GROUP_0' : '0',
    'RCINIT_GROUP_1' : '1',
    'RCINIT_GROUP_2' : '2',
    'RCINIT_GROUP_3' : '3',
    'RCINIT_GROUP_4' : '4',
    'RCINIT_GROUP_5' : '5',
    'RCINIT_GROUP_6' : '6',
    'RCINIT_GROUP_7' : '7',
}





#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
    return True

def generate(env):
    rootenv = env.get('IMAGE_ENV')

    #aggregrates all rcinit entries
    rootenv[RCINIT_ENV_DESCRIPTOR] = []
    
    #legacy file pointers
    rootenv[RCINIT_LEGACY_FILE_PTR] = None
    rootenv[RCINIT_PRIO_FILE_PTR] = None
    rootenv[RCINIT_GOVERN_FILE_PTR] = None
    
    #legacy priorities
    rootenv[LEGACY_PRIO_DICT] = {}

    #legacy stack size
    rootenv[LEGACY_STKSZ_DICT] = {}
    
    #legacy features
    rootenv[LEGACY_FEATURE_DICT] = {}
    
    #list of priorities
    rootenv[LEGACY_PRIO_LIST] = []
    
   # add methods to enviroment
    rootenv.AddMethod(add_rcinit_task, "AddRCInitTask")   
    rootenv.AddMethod(add_rcinit_function, "AddRCInitFunc")   
    
    # load builder into enviroment
    rcinit_act = env.GetBuilderAction(rcinit_builder, action_source = None)
    rcinit_scan = env.Scanner(rcinit_scan_func, "rcinit_scan")
    rcinit_bld = env.Builder(action = rcinit_act, target_scanner = rcinit_scan)
    rootenv.Append(BUILDERS = {'RcinitBuilder' : rcinit_bld})
    
    if rootenv.GetUsesFlag('USES_RCINIT') is False:
        return
    
    rcinit_generate(env)

   
def rcinit_scan_func(node, env, path):
    rebuild = False
    rootenv = env.get('IMAGE_ENV')
    
    #rcinit not enabled, null out entries
    if rootenv.GetUsesFlag('USES_RCINIT') is False:
        rootenv[RCINIT_ENV_DESCRIPTOR] = []
        
    #form string from dictionary
    rcinit_string = str(rootenv[RCINIT_ENV_DESCRIPTOR])
    
    #form dict file
    target_full = env.RealPath(str(node))
    rcinit_dict_file = os.path.dirname(target_full) + '\\' + RCINIT_DICT
    
    rcinit_dict_file_ptr = open(rcinit_dict_file, 'w')
    rcinit_dict_file_ptr.write(rcinit_string)
    rcinit_dict_file_ptr.close()
    
    return [
        node, 
        rootenv[RCINIT_LEGACY_FILE_PTR], 
        rootenv[RCINIT_PRIO_FILE_PTR], 
        rootenv[RCINIT_GOVERN_FILE_PTR],
        rcinit_dict_file
        ]

def rcinit_generate(env):
    rootenv = env.get('IMAGE_ENV')
    
    for i in range(MAX_GRP_LVL):
        rootenv[RCINIT_ENV_DESCRIPTOR].append([])
    
        
    #legacy file
    fileItemLegacy = getFileLocation(env, env.get('BUILD_ROOT',None) + "/core/debugtools/", RCINIT_LEGACY_INFO_FILE)
    if fileItemLegacy is None:
        Rcinit_error(env, "rcinit missing " + RCINIT_LEGACY_INFO_FILE)
    #read in features
    rootenv[RCINIT_LEGACY_FILE_PTR] = fileItemLegacy
    featureReader = csv.DictReader(open( rootenv[RCINIT_LEGACY_FILE_PTR], "rb"))
    for row in featureReader:
        rootenv[LEGACY_FEATURE_DICT][row['TASK_LEGACY_NAME'].lower()] = row['TASK_LEGACY_FEATURES']
        
    #prio file
    fileItemPrio = None
    if env.has_key('IMAGE_ROOT'):
        fileItemPrio = getFileLocation(env, env.get('IMAGE_ROOT'), RCINIT_PRIORITY_FILE)
    if fileItemPrio is None:
        fileItemPrio = getFileLocation(env, env.get('BUILD_ROOT',None) + "/core/debugtools/", RCINIT_PRIORITY_FILE)
    if fileItemPrio is None:
        Rcinit_error(env, "rcinit missing " + RCINIT_PRIORITY_FILE)
    rootenv[RCINIT_PRIO_FILE_PTR] = fileItemPrio
    prioReader = csv.DictReader(open( rootenv[RCINIT_PRIO_FILE_PTR], "rb"))
    for row in prioReader:
        rootenv[LEGACY_PRIO_LIST].append( row['TASK_LEGACY_AMSS_PRI_ORDER'] )

    #govern file
    fileItemGovern = None
    if env.has_key('IMAGE_ROOT'):
        fileItemGovern = getFileLocation(env, env.get('IMAGE_ROOT'), RCINIT_GOVERN_FILE)
    if fileItemGovern is None:
        fileItemGovern = getFileLocation(env, env.get('BUILD_ROOT',None) + "/core/debugtools/", RCINIT_GOVERN_FILE)
    if fileItemGovern is None:
        Rcinit_error(env, "rcinit missing " + RCINIT_GOVERN_FILE)
    rootenv[RCINIT_GOVERN_FILE_PTR] = fileItemGovern
    #must read in prios now for input checking
    governReader = csv.DictReader(open( rootenv[RCINIT_GOVERN_FILE_PTR], "rb"))
    for row in governReader:
        rootenv[LEGACY_STKSZ_DICT][row['TASK_LEGACY_NAME']] = row['TASK_LEGACY_STKSZ_KB']
        rootenv[LEGACY_PRIO_DICT][row['TASK_LEGACY_NAME']] = row['TASK_LEGACY_AMSS_PRI_ORDER']

        
def getFileLocation(env, path, fileToFind):
    for root, dirs, files in os.walk(path):
        for filename in fnmatch.filter(files, fileToFind):
            return os.path.join(root, filename)
    return None
    
            
    
def rcinit_builder(target, source, env):
    # init and fix variebles
    target_full = env.RealPath(str(target[0]))
    RJUST_AMOUNT = 40
    
    rcinit_output_file = open(target_full, 'w')
    rcinit_output_file.write(env.CreateFileBanner(os.path.basename(str(target[0])), style="C"))
    
    rootenv = env.get('IMAGE_ENV')
    
    #rcinit not enabled, dont bother generating rest of file
    if rootenv.GetUsesFlag('USES_RCINIT') is False:
        rcinit_output_file.close()
        return
    
    #generate headers
    rcinit_output_file.write("#include \"customer.h\"\n")
    rcinit_output_file.write("#include \"rex.h\"\n")
    rcinit_output_file.write("#include \"task.h\"\n")
    rcinit_output_file.write("#include \"rcinit.h\"\n")
    rcinit_output_file.write("#include \"rcevt.h\"\n")
    rcinit_output_file.write("\n")
    rcinit_output_file.write("#ifndef REX_SINGLE_CPU_AFFINITY_MASK\n")
    rcinit_output_file.write("#define REX_SINGLE_CPU_AFFINITY_MASK (0)\n")
    rcinit_output_file.write("#define REX_COMMON_SMT_MASK REX_SINGLE_CPU_AFFINITY_MASK\n")
    rcinit_output_file.write("#endif\n")
    rcinit_output_file.write("#ifndef REX_ANY_CPU_AFFINITY_MASK\n")
    rcinit_output_file.write("#define REX_ANY_CPU_AFFINITY_MASK (~0)\n")
    rcinit_output_file.write("#endif\n")
    rcinit_output_file.write("\n\n")
    
    #output priority enum
    rcinit_output_file.write("typedef enum\n")
    rcinit_output_file.write("{\n\n")
    
    #aggregate rcinit names to compare against legacy/rcinit lists
    rcinitNames = []
    for grpList in rootenv[RCINIT_ENV_DESCRIPTOR]:
        grpTasks = [task[THREAD_NAME] for task in grpList if task.has_key(THREAD_NAME)]
        grpFuncs = [task[INIT_NAME] for task in grpList if task.has_key(INIT_NAME)]
        for task in grpTasks:
            rcinitNames.append(task)
        for initFunc in grpFuncs:
            rcinitNames.append(initFunc)
            
    #avoid duplicate priority output
    prevPriority = None
    
    for prio in rootenv[LEGACY_PRIO_LIST]:
        if prio != prevPriority:
            rcinit_output_file.write("".join(["\t", prio, ",\n"]))
            prevPriority = prio
    
    rcinit_output_file.write("\tMAX_PRI_ORDER,\n")
    #rcinit_output_file.write("\tSLEEP_PRI_ORDER = AMSS_MAX_TASK_PRI - 2\n\n")
    rcinit_output_file.write("} rcinit_amss_priority_order_e_type;\n\n")
    
    rcinit_output_file.write("/*******************************************************/")
    rcinit_output_file.write("\n")
    #done outputting priority enum
    
    #output task registers
    registerReader = csv.DictReader(open( rootenv[RCINIT_LEGACY_FILE_PTR], "rb"))
    for row in registerReader:
        if row['TASK_LEGACY_NAME'] not in rcinitNames:
            #save features, its needed later in the builder
            
            rcinit_output_file.write("".join(["#if ", rootenv[LEGACY_FEATURE_DICT][row['TASK_LEGACY_NAME'].lower()], "\n"]))
            if row['TASK_LEGACY_NAME'] not in rootenv[LEGACY_STKSZ_DICT].keys():
                taskStackSz = "0"
            else:
                taskStackSz = rootenv[LEGACY_STKSZ_DICT][row['TASK_LEGACY_NAME']]
            if row['TASK_LEGACY_NAME'] not in rootenv[LEGACY_PRIO_DICT].keys():
                taskPrio = "0"
            else:
                taskPrio = "TASK_PRIORITY(" + rootenv[LEGACY_PRIO_DICT][row['TASK_LEGACY_NAME']].strip() + ")"
            taskEntry = row['TASK_LEGACY_ENTRY'].strip()
            taskTcb = row['TASK_LEGACY_HANDLE'].strip()
            taskStack = ''
            taskExternDeclare = row['TASK_LEGACY_EXTERN'].strip()
            taskCpuAffinity = row['TASK_LEGACY_AFFINITY'].strip()

            if taskEntry != '':
                rcinit_output_file.write("".join(["extern void ", row['TASK_LEGACY_ENTRY'].strip(), "(dword argv);\n"]))
            if taskTcb != '':
                rcinit_output_file.write("".join(["/*static*/ rex_tcb_type ", row['TASK_LEGACY_HANDLE'].strip(), ";\n"]))
            if taskStack != '':
                rcinit_output_file.write("".join(["/*extern rex_stack_word_type ", row['TASK_LEGACY_STACK'].strip(), "[];*/\n"]))
            if taskTcb != '':
                rcinit_output_file.write("".join(["static rcinit_handle ", row['TASK_LEGACY_NAME'], "_rcinit_handle = { RCINIT_TASK_REXTASK, { &", taskTcb, "} };\n"]))
            else:
                rcinit_output_file.write("".join(["static rcinit_handle ", row['TASK_LEGACY_NAME'], "_rcinit_handle = { RCINIT_TASK_REXTASK, { RCINIT_NULL } };\n"]))

            if taskStack != '' and taskEntry != '':
                rcinit_output_file.write("".join(["/*static*/ /*const*/ rcinit_info rcinit_info_", row['TASK_LEGACY_NAME'], " = {\n\t\"", row['TASK_LEGACY_NAME'], "\",\n\t(rcinit_entry_p)", taskEntry, ", ",
                                taskPrio, ", &", row['TASK_LEGACY_NAME'], "_rcinit_handle,\n\t",
                                taskStackSz, ", &", taskStack, "[0],\n\t",
                                taskCpuAffinity, "\n};\n"]))

            elif taskStack == '' and taskEntry != '':
                rcinit_output_file.write("".join(["/*static*/ /*const*/ rcinit_info rcinit_info_", row['TASK_LEGACY_NAME'], " = {\n\t\"", row['TASK_LEGACY_NAME'], "\",\n\t(rcinit_entry_p)", taskEntry, ", ",
                                taskPrio, ", &", row['TASK_LEGACY_NAME'], "_rcinit_handle,\n\t",
                                taskStackSz, ", RCINIT_NULL,\n\t",
                                taskCpuAffinity, "\n};\n"]))

            else:
                rcinit_output_file.write("".join(["/*static*/ /*const*/ rcinit_info rcinit_info_", row['TASK_LEGACY_NAME'], " = {\n \"", row['TASK_LEGACY_NAME'], "\",\n\tRCINIT_NULL, ",
                                taskPrio, ", &", row['TASK_LEGACY_NAME'], "_rcinit_handle,\n\t",
                                taskStackSz, ", RCINIT_NULL,\n\t",
                                taskCpuAffinity, "\n};\n"]))

            rcinit_output_file.write("#endif\n\n")
    #done outputting task registers
    
    #loop through tasks that registered with rcinit
    for grpList in rootenv[RCINIT_ENV_DESCRIPTOR]:
        for task in grpList:
        
            #definitions/externs
            if task.has_key(THREAD_TCB):
                rcinit_output_file.write("".join(["/*static*/ rex_tcb_type ", task[THREAD_TCB], ";\n"]))
                rcinit_output_file.write("".join(["/*extern rex_stack_word_type ", task[STACK_NAME], "[];*/\n"]))

            #output handle if this rcinit struct will be a task (versus an init function)
            if task.has_key(THREAD_NAME):
                rcinit_output_file.write("".join(["static rcinit_handle ", task[THREAD_NAME], "_rcinit_handle = { RCINIT_TASK_REXTASK, { &", task[THREAD_TCB], "} };\n"]))
            if task.has_key(THREAD_ENTRY) is True:
                if task[THREAD_ENTRY] != RCINIT_NULL:
                    rcinit_output_file.write("".join(["extern void ", task[THREAD_ENTRY], "(dword ignored);\n"]))
            else:
                if task[THREAD_INIT] != RCINIT_NULL:
                    rcinit_output_file.write("".join(["extern void ", task[THREAD_INIT], "(void);\n"]))
            
            
            #generate struct field
            if task.has_key(THREAD_NAME):
                rcinit_output_file.write("".join(["/*static*/ /*const*/ rcinit_info rcinit_info_",task[THREAD_NAME], " = { \n"]))
            else:
                rcinit_output_file.write("".join(["/*static*/ /*const*/ rcinit_info rcinit_info_",task[INIT_NAME], " = { \n"]))
                
            #format of output:  first generate struct member string, then comment string
            #name
            if task.has_key(THREAD_NAME):
                nameOutput = " ".join(["".join(["\"", task[THREAD_NAME], "\"", ","]).rjust(RJUST_AMOUNT), "//name\n"])
            else:
                nameOutput = " ".join(["".join(["\"", task[INIT_NAME], "\"", ","]).rjust(RJUST_AMOUNT), "//name\n"])
            rcinit_output_file.write(nameOutput)
        

            #init/entry
            if task.has_key(INIT_NAME) is True:
                rcinit_output_file.write(" ".join(["".join(["(rcinit_entry_p)", task[THREAD_INIT], ","]).rjust(RJUST_AMOUNT), "//thread initializer\n"]))
            else:
                rcinit_output_file.write(" ".join(["".join(["(rcinit_entry_p)", task[THREAD_ENTRY], ","]).rjust(RJUST_AMOUNT), "//thread entry\n"]))
            
            #prio
            if (task[PRIORITY_AMSS] in rootenv[LEGACY_PRIO_LIST]):
                rcinit_output_file.write(" ".join(["".join(["TASK_PRIORITY(", str(task[PRIORITY_AMSS]), "),"]).rjust(RJUST_AMOUNT), "//priority\n"]))
            else:
                rcinit_output_file.write(" ".join(["".join([str(task[PRIORITY_AMSS]), ","]).rjust(RJUST_AMOUNT), "//priority\n"]))
        
            #rcinit handle
            if task.has_key(THREAD_NAME):
                rcinit_output_file.write(" ".join(["".join(["&", task[THREAD_NAME], "_rcinit_handle,"]).rjust(RJUST_AMOUNT), "//rcinit handle\n"]))
            else:
                rcinit_output_file.write(" ".join(["".join(["RCINIT_NULL,"]).rjust(RJUST_AMOUNT), "//rcinit handle\n"]))
        
            #stack size
            rcinit_output_file.write(" ".join(["".join([str(task[STACK_SIZE]), ","]).rjust(RJUST_AMOUNT), "//stack size\n"]))
        
            #stack -- always null for now
            #if task.has_key(THREAD_NAME):
            #    rcinit_output_file.write(" ".join(["".join(["&", task[STACK_NAME], "[0],"]).rjust(RJUST_AMOUNT), "//stack\n"]))
            #else:
            rcinit_output_file.write(" ".join(["RCINIT_NULL,".rjust(RJUST_AMOUNT), "//stack\n"]))

            rcinit_output_file.write(" ".join(["".join([task[CPU_AFFINITY]]).rjust(RJUST_AMOUNT), "//cpu affinity\n"]))

            rcinit_output_file.write(" };\n")
            rcinit_output_file.write("\n")
            rcinit_output_file.write("/*******************************************************/")
            rcinit_output_file.write("\n\n")
        #done generating rcinit structs
        
    #generate task map
    rcinit_output_file.write("const rcinit_internal_name_map_t rcinit_internal_name_map[] = \n")
    rcinit_output_file.write("{\n\n")
    
    featureNameKeys = rootenv[LEGACY_FEATURE_DICT].keys()
    featureNameKeys = list( set(featureNameKeys) | set(rcinitNames) )
    featureNameKeys.sort()
    for key in featureNameKeys:
        if key not in rcinitNames:
            rcinit_output_file.write("".join(["#if ", rootenv[LEGACY_FEATURE_DICT][key], "\n"]))
        rcinit_output_file.write("".join(["\t{\"", key, "\", &rcinit_info_", key, "},\n"]))
        if key not in rcinitNames:
            rcinit_output_file.write("#endif\n\n")
        
    rcinit_output_file.write("\t{ (rcinit_name_p)RCINIT_NULL, (rcinit_info_p)RCINIT_NULL}\n")
    rcinit_output_file.write("};\n\n")
    #done generating task map
    
    
    rcinit_output_file.write("const int rcinit_internal_name_map_size = sizeof(rcinit_internal_name_map) / sizeof(rcinit_internal_name_map_t);\n")
    
    #sort group levels given dependencies
    grpNum = 0
    for grpLvl in rootenv[RCINIT_ENV_DESCRIPTOR]:
        #first generate a list of names at the group level
        taskNames = [ dict_[THREAD_NAME] for dict_ in grpLvl if dict_.has_key(THREAD_NAME)]
        initNames = [ dict_[INIT_NAME] for dict_ in grpLvl if dict_.has_key(INIT_NAME)]
        #only init functions allowed as dependencies
        grpNames = set(initNames)
        
        #form dictionary of vertices for topological sort
        grpDict = {}
        for item in grpLvl:
            filteredDependencies = list(set(item[RCINIT_DEPENDENCIES]) & grpNames)
            item[RCINIT_DEPENDENCIES_FILTER] = filteredDependencies
            if item.has_key(THREAD_NAME):
                grpDict[item[THREAD_NAME]] = filteredDependencies
            else:
                grpDict[item[INIT_NAME]] = filteredDependencies
        
        #sort based on dependencies
        grpSorted = topological_sort(grpDict)
        grpSorted.reverse()
        
        #check if there was a cycle
        if len(grpSorted) != len( set(initNames) | set(taskNames) ):
            Rcinit_error(env, "Cycle in dependencies found")
            
        #append an ordering to each dictionary
        for item in grpLvl:
            if item.has_key(THREAD_NAME):
                item[RCINIT_INTERNAL_ORDER] = grpSorted.index(item[THREAD_NAME])
            else:
                item[RCINIT_INTERNAL_ORDER] = grpSorted.index(item[INIT_NAME])
                
        #sort the dictionaries
        rootenv[RCINIT_ENV_DESCRIPTOR][grpNum] = sorted(grpLvl, key=itemgetter(RCINIT_INTERNAL_ORDER))
        grpNum += 1
    
    #generate rcinit arrays
    rcinit_output_file.write("\n\n")
    for i in range(MAX_GRP_LVL):
        rcinit_output_file.write("".join(["static const rcinit_info_p rcinit_group_", str(i), "[/*", str(len(rootenv[RCINIT_ENV_DESCRIPTOR][i]) + 1) ,"*/] = \n"]))
        rcinit_output_file.write("{\n")
        for rcinitInfo in rootenv[RCINIT_ENV_DESCRIPTOR][i]:
            if rcinitInfo.has_key(THREAD_NAME):
                rcinit_output_file.write("".join(["\t&rcinit_info_", rcinitInfo[THREAD_NAME], ",\n"]))
            else:
                rcinit_output_file.write("".join(["\t&rcinit_info_", rcinitInfo[INIT_NAME], ",\n"]))
        rcinit_output_file.write("\tRCINIT_NULL\n")
        rcinit_output_file.write("};\n")
    
    rcinit_output_file.write("\n\n")
    rcinit_output_file.write("".join(["const rcinit_info_p* rcinit_internal_sequence_groups[/*", str(MAX_GRP_LVL+1), "*/] = \n"]))
    rcinit_output_file.write("{\n")
    for i in range(MAX_GRP_LVL):
        rcinit_output_file.write("".join(["\trcinit_group_", str(i), ",\n"]))
    rcinit_output_file.write("\t(rcinit_info_p*)RCINIT_NULL\n")
    rcinit_output_file.write("};\n")
    #done generating rcinit arrays
    
    #form output log
    debugOutput = []
    debugOutput.append("****************************************\n")
    debugOutput.append("\t\tRCINIT OUTPUT\n\n")
    debugOutput.append("Note: init functions will have a priority and stack size of '0'\n")
    debugOutput.append("****************************************\n")
    
    for i in range(MAX_GRP_LVL):
        debugOutput.append("\n\t GROUP " + str(i) + "\n\n")
        debugOutput.append("\t".join(["NAME".rjust(20), "PRIORITY".rjust(23), "STACK SIZE".rjust(15), "DEPENDENCIES".rjust(23), "\n"]))
        debugOutput.append("\n")
        for threads in rootenv[RCINIT_ENV_DESCRIPTOR][i]:
            if threads.has_key(INIT_NAME):
                debugOutput.append("\t".join([threads[INIT_NAME].rjust(20), str(threads[PRIORITY_AMSS]).rjust(23), threads[STACK_SIZE].rjust(15), ",".join(threads[RCINIT_DEPENDENCIES_FILTER]).rjust(23), "\n"]))
        for threads in rootenv[RCINIT_ENV_DESCRIPTOR][i]:
            if threads.has_key(THREAD_NAME):
                debugOutput.append("\t".join([threads[THREAD_NAME].rjust(20), str(threads[PRIORITY_AMSS]).rjust(23), threads[STACK_SIZE].rjust(15), ",".join(["NA"]).rjust(23), "\n"]))
        debugOutput.append("\n-----------------------------------------------------------------------------------\n")
    
    #form priority output
    priorityOutput = []
    priorityOutput.append("**************************************************************************************\n")
    priorityOutput.append("\t\t\t\t\t\tPRIORITY ORDERING\n")
    priorityOutput.append("**************************************************************************************\n")
    for prio in rootenv[LEGACY_PRIO_LIST]:
        priorityOutput.append("\t".join([prio, "\n"]))
    priority_output_logfile = open(env.get('BUILD_ROOT',None) + "/build/ms/" + RCINIT_PRIORITY_LOG, 'w')
    priority_output_logfile.writelines(priorityOutput)
    priority_output_logfile.close()
    
    #append output log to end of file
    rcinit_output_file.write("\n/* \n")
    for line in debugOutput:
        rcinit_output_file.write(line)
    rcinit_output_file.write("*/ \n")
    rcinit_output_file.close()
    
    #print output log to scons debug
    env.PrintDebugInfo('rcinit', "BEGIN RCINIT DEBUG OUTPUT")
    for line in debugOutput:
        env.PrintDebugInfo('rcinit', line)
        
    #form output log in build/ms
    rcinit_output_logfile = open(env.get('BUILD_ROOT',None) + "/build/ms/" + RCINIT_OUTPUT_LOG, 'w')
    rcinit_output_logfile.writelines(debugOutput)
    rcinit_output_logfile.close()
    return None
    
    
def add_rcinit_task(env, targets, thread_input_dict):

    rootenv = env.get('IMAGE_ENV')
    if rootenv.GetUsesFlag('USES_RCINIT') is False:
        return
    
    newDict = {}
    
    
    #read in name
    if thread_input_dict.has_key(THREAD_NAME) is False:
        Rcinit_error(env, THREAD_NAME + " required")
    newDict[THREAD_NAME] = thread_input_dict[THREAD_NAME]
    
    for grpList in rootenv[RCINIT_ENV_DESCRIPTOR]:
        for threads in grpList:
            if threads.has_key(THREAD_NAME) and newDict[THREAD_NAME] == threads[THREAD_NAME]:
                Rcinit_error(env, "Duplicate thread name found: " + newDict[THREAD_NAME])
            if threads.has_key(INIT_NAME) and newDict[THREAD_NAME] == threads[INIT_NAME]:
                Rcinit_error(env, "Init function shares name with task: " + newDict[THREAD_NAME])
    
    #read in sequence level
    if thread_input_dict.has_key(SEQUENCE_LEVEL) is False:
        Rcinit_error(env, SEQUENCE_LEVEL + " required for task " + newDict[THREAD_NAME])
    if thread_input_dict[SEQUENCE_LEVEL] not in RCINIT_RUN_LEVEL_CONSTANTS.keys():
        Rcinit_error(env, SEQUENCE_LEVEL + " invalid for task " + newDict[THREAD_NAME])
    newDict[SEQUENCE_LEVEL] = RCINIT_RUN_LEVEL_CONSTANTS[thread_input_dict[SEQUENCE_LEVEL]]
    
    #read in thread type
    if thread_input_dict.has_key(THREAD_TYPE):
        newDict[THREAD_TYPE] = thread_input_dict[THREAD_TYPE]
    else:
        newDict[THREAD_TYPE] = THREAD_REX
    
    #read in stack name
    if thread_input_dict.has_key(STACK_NAME):
        newDict[STACK_NAME] = thread_input_dict[STACK_NAME]
    else:
        newDict[STACK_NAME] = newDict[THREAD_NAME] + '_stack'
    
    #read in stack size
    if newDict[THREAD_NAME] in rootenv[LEGACY_STKSZ_DICT].keys():
        newDict[STACK_SIZE] = rootenv[LEGACY_STKSZ_DICT][newDict[THREAD_NAME]]
    elif thread_input_dict.has_key(STACK_SIZE) is True:
        newDict[STACK_SIZE] = thread_input_dict[STACK_SIZE]
    else:
        newDict[STACK_SIZE] = ""    
    
    #read in priority
    if newDict[THREAD_NAME] in rootenv[LEGACY_PRIO_DICT].keys():
        newDict[PRIORITY_AMSS] = rootenv[LEGACY_PRIO_DICT][newDict[THREAD_NAME]]
    elif thread_input_dict.has_key(PRIORITY_AMSS) is True:
        newDict[PRIORITY_AMSS] = thread_input_dict[PRIORITY_AMSS]
    else:
        newDict[PRIORITY_AMSS] = ""

        
    #read in thread entry
    if thread_input_dict.has_key(THREAD_ENTRY):
        newDict[THREAD_ENTRY] = thread_input_dict[THREAD_ENTRY]
    else:
        newDict[THREAD_ENTRY] = newDict[THREAD_NAME] + "_task"
    
    #read in thread tcb
    if thread_input_dict.has_key(THREAD_TCB):
        newDict[THREAD_TCB] = thread_input_dict[THREAD_TCB]
    else:
        newDict[THREAD_TCB] =  newDict[THREAD_NAME] + "_tcb"
        
    #read in dependencies (not allowed for tasks)
    newDict[RCINIT_DEPENDENCIES] = []

    #read in cpu affinity
    if thread_input_dict.has_key(CPU_AFFINITY):
        newDict[CPU_AFFINITY] = thread_input_dict[CPU_AFFINITY]
    else:
        newDict[CPU_AFFINITY] =  "REX_ANY_CPU_AFFINITY_MASK"

   # check to see if this environment will be use for this target
    if env.IsTargetEnable(targets):
        rootenv[RCINIT_ENV_DESCRIPTOR][int(newDict[SEQUENCE_LEVEL])].append(newDict)
    
    
    
    return None
    
def add_rcinit_function(env, targets, thread_input_dict):

    rootenv = env.get('IMAGE_ENV')
    if rootenv.GetUsesFlag('USES_RCINIT') is False:
        return
    
    newDict = {}
    
    #read in name
    if thread_input_dict.has_key(INIT_NAME) is False:
        Rcinit_error(env, INIT_NAME + " required")
        
    newDict[INIT_NAME] = thread_input_dict[INIT_NAME]
    for grpList in rootenv[RCINIT_ENV_DESCRIPTOR]:
        for threads in grpList:
            if threads.has_key(INIT_NAME) and newDict[INIT_NAME] == threads[INIT_NAME]:
                Rcinit_error(env, "Duplicate init function name found: " + newDict[INIT_NAME])
            if threads.has_key(THREAD_NAME) and newDict[INIT_NAME] == threads[THREAD_NAME]:
                Rcinit_error(env, "Init function shares name with task: " + newDict[INIT_NAME])
    
    #read in init function if applicable
    if thread_input_dict.has_key(THREAD_INIT) is True:
        newDict[THREAD_INIT] = thread_input_dict[THREAD_INIT]
    else:
        newDict[THREAD_INIT] = newDict[INIT_NAME] + "_init"
    
    #read in sequence level
    if thread_input_dict.has_key(SEQUENCE_LEVEL) is False:
        Rcinit_error(env, SEQUENCE_LEVEL + " required for init function " + newDict[INIT_NAME])
    if thread_input_dict[SEQUENCE_LEVEL] not in RCINIT_RUN_LEVEL_CONSTANTS.keys():
        Rcinit_error(env, SEQUENCE_LEVEL + " invalid for init function " + newDict[INIT_NAME])
    newDict[SEQUENCE_LEVEL] = RCINIT_RUN_LEVEL_CONSTANTS[thread_input_dict[SEQUENCE_LEVEL]]
    
    
    #read in thread type
    if thread_input_dict.has_key(THREAD_TYPE):
        newDict[THREAD_TYPE] = thread_input_dict[THREAD_TYPE]
    else:
        newDict[THREAD_TYPE] = THREAD_REX
    
    #read in stack
    newDict[STACK_NAME] = ''
    
    #read in stack size
    newDict[STACK_SIZE] = '0'
    
    #read in priority
    newDict[PRIORITY_AMSS] = '0'
    
    #read in cpu affinity
    newDict[CPU_AFFINITY] =  "REX_ANY_CPU_AFFINITY_MASK"
    
    #read in dependencies
    if thread_input_dict.has_key(RCINIT_DEPENDENCIES):
        if isinstance(thread_input_dict[RCINIT_DEPENDENCIES], list) == False:
            Rcinit_error(env, RCINIT_DEPENDENCIES + " must be python list")
        newDict[RCINIT_DEPENDENCIES] = thread_input_dict[RCINIT_DEPENDENCIES]
    else:
        newDict[RCINIT_DEPENDENCIES] = []
    
   # check to see if this environment will be use for this target
    if env.IsTargetEnable(targets):
        rootenv[RCINIT_ENV_DESCRIPTOR][int(newDict[SEQUENCE_LEVEL])].append(newDict)
    
    
    return None
    

def Rcinit_error(env, Info):
    env.PrintError("Rcinit error found: " + Info)
    raise
    
def Rcinit_checkInt(env, checkInt, paramName, name):
    try:
        int(checkInt)
    except ValueError:
        Rcinit_error(env, paramName + " must be an integer for task or init function " + name)
        
def topological_sort(graph):
    count = { }
    for node in graph:
        count[node] = 0
    for node in graph:
        for successor in graph[node]:
            count[successor] += 1

    ready = [ node for node in graph if count[node] == 0 ]
    
    result = [ ]
    while ready:
        node = ready.pop(-1)
        result.append(node)
        
        for successor in graph[node]:
            count[successor] -= 1
            if count[successor] == 0:
                ready.append(successor)
    
    return result