#******************************************************************************
#* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg/mcfg_tools/mcfg_automation/MCFG_CreateCR.py#1 $
#* $DateTime: 2016/12/13 07:58:21 $
#*
#* 
#******************************************************************************
#*
#* Copyright (c) 2015- Qualcomm Technologies, Inc.
#* All rights reserved.
#* Qualcomm Technologies, Inc. Confidential and Proprietary.
#*
#******************************************************************************
# optparse deprecated from 2.7. replace with argparse
from optparse import OptionParser
import re
from MCFGClient import *
from PrismClient import *
import logging
import xmltodict
import collections
import pdb
from MCFG_Automation_Functions import *

parser = OptionParser()
# parser.add_option("--lo", dest="labelOnly", action="store_true", default=False, help="Running with this option will only add labels to the tickets")
# parser.add_option("--query", dest="query", help="Supply a custom query. Default is 'project = MCFG AND component = MBN")
# parser.add_option("--groupCR", dest="isGroup", action="store_true", help="All JIRA tickets returned from the query will be grouped into one CR")
parser.add_option("--keys", dest="keys", help="Comma separated list of keys to be combined in this CR. e.g --keys \"MCFG-201, MCFG-213\"  or --keys \"201, 213\"")
parser.add_option("--force", dest="forceCreate", action="store_true", default=False, help="This flag will create a CR using the PL's found in the labels of the JIRA ticket.")
NULL_CR = 1;
def VerbosePrint(line):
    return

def LogPrint(Str):
    logging.info(Str)


def executed():
    # global combinedDescription
    readXMLs()


    (options, args) = parser.parse_args()

    if options.keys == None:
        print("Error! JIRA ticket keys must be specified...")

    keyList = re.split(r',?\s*', options.keys)

    if 'MCFG' in keyList[0]:
        query = "project = MCFG AND key = '%s'" % (keyList[0])
    else:
        query = "project = MCFG AND key = 'MCFG-%s'" % (keyList[0])

    for key in keyList[1:]:
        if 'MCFG' in keyList[0]:
            query += " OR key = '%s'" % (key)
        else:
            query += " OR key = 'MCFG-%s'" % (key)

    # TODO: create session so user doesn't have to repeatedly log in
    # between invocations
    user, pw = login()
    logging.basicConfig(filename='log_messages-'+user+'.log',level=logging.ERROR)
    try:
        jira = JIRA(user,pw)
    except:
        print "Failed to create JIRA object"

    JIRA_QUERY = query
    JIRA_ISSUES_INTERVAL = 1000 # Batch of 100 JIRA issues is considered at a time.
    JIRA_FIELDS = "*all"
    jira_release = dict()
    jira_report = list()

    PackageWarehouses = set()
    correctKeys = set()
    ReleasePLs = set()

    jira.getJIRAIssues(JIRA_QUERY, JIRA_ISSUES_INTERVAL, JIRA_FIELDS, jira_release, jira_report)
    combinedDescription = {}
    JustificationNotes = []
    # pdb.set_trace()
    title = ""
    forcedLabels = set()
    temp = set()
    forceError = False
    forceErrorMessage = ""
    forcedDescription = {}
    forcedDescription['EntireTicketDesc'] = []
    for key in jira_release.keys():
        for label in jira_release[key]['labels']:
            forcedLabels.add(label)

        forcedDescription['EntireTicketDesc'] += ''.join(jira_release[key]['description'])
        # pdb.set_trace()
        parseTicket(jira_release, key, combinedDescription,PackageWarehouses,ReleasePLs, correctKeys,jira, JustificationNotes)
        forcedTitle = jira_release[key]['summary']
        title = jira_release[key]['summary']

    # cr = "888833"
    print("Product Lines ")
    print(ReleasePLs)
    if len(correctKeys) == 0:
        cr = NULL_CR
    else:
        cr = CreateAPICR(title,combinedDescription,JustificationNotes,correctKeys,PackageWarehouses,ReleasePLs,jira)

    if cr != NULL_CR:
        link = "http://prism/CR/" + str(cr)
        for k in correctKeys:
            res = addImplementationCR(jira_release[k]['description'],link )
            jira.addLabel(k, "CR"+str(cr))
            retVal = jira.updateDescription(k,res)
            LogPrint("Updated description\n\n" + res)
            # LogPrint("Return Value\n\n" + retVal)

        print ("CR Number: " + str(cr))

    elif cr == NULL_CR:

        if options.forceCreate:
            if len(forcedLabels) == 0:
                print("Force Error! No PL labels were specified..")
                return

            forcedPWs = set()
            #pdb.set_trace()
            getPackageWarehouses(forcedLabels, forcedPWs)

            print("Getting ready to force create a CR with the following PWs: \n" + '\n'.join(forcedPWs))
            if combinedDescription:
                forcedDescription = combinedDescription
            #print(forcedDescription)
            LogPrint("\n\nThis is the forced description\n\n")
            LogPrint(forcedDescription['EntireTicketDesc'])

            forcedCR = CreateAPICR(title, forcedDescription, JustificationNotes, jira_release.keys(), forcedPWs,forcedLabels, jira)

            if  forcedCR:
                for k in jira_release.keys():
                    jira.addLabel(k, "CR"+str(forcedCR))
            print("CR Number: " + str(forcedCR))
            return
        else:
            print("Parsing Error: No CR was created...")


    LogPrint("CR Number: " + str(cr))
    LogPrint("\n\nThis is combined description\n\n")
    LogPrint(combinedDescription['EntireTicketDesc'])
    LogPrint(PackageWarehouses)
    LogPrint(correctKeys)


executed()

