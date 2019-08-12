#******************************************************************************
#* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg/mcfg_tools/mcfg_automation/MCFG_Automation_Functions.py#1 $
#* $DateTime: 2016/12/13 07:58:21 $
#*
#* 
#******************************************************************************
#*
#* Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
#* All rights reserved.
#* Qualcomm Technologies, Inc. Confidential and Proprietary.
#*
#******************************************************************************
import datetime
import sys
import os
import smtplib
import sys
import subprocess
import re
from MCFGClient import *
import pdb
import xmltodict
import logging
from optparse import OptionParser
from PrismClient import *
import collections
import time
from PCM import PCM

JIRA_ISSUES_INTERVAL = 1000 # Batch of 100 JIRA issues is considered at a time.
JIRA_FIELDS = "*all"

EFS_ITEM = 0x01
EFS_ITEM_TYPE = 0x02
NV_ITEM  = 0x03


user = None
pw = None
jira = None

#Maps package warehouse to it's carrier policy location
CarrierPolicyMap = {}
#Prefix to all p4 commands. Includes client name e.g p4 -c mcfg.mpss.5.0.CR80000 command
P4Prefix = ""
#Labels that need to be added to this cr
Labels = set()
#Stores the NV items from the ticket
NVItems = []
#Stores the EFS items from the ticket
EFSItems = []
#Stores the package warehouses this ticket involves
PackageWarehouses = set()
#The current CR
APICR = None
#The parsed items
parsedItemsList = []
#This part of the path is constant across all carriers and subtypes
MBN_PATH = "modem_proc\\mcfg\\configs\\mcfg_sw\\generic\\"
XML_PATH = "modem_proc\\mcfg\\mcfg_gen\\generic\\"
POLICY_PATH = "/modem_proc/mmcp/policyman/configurations/..."
EFS_FILE_PATH = "modem_proc\\mcfg\\mcfg_gen\\scripts\\data\\efs_files\\"
#The current pw being processed
CurrentPW = None
#The path of the current view
currentViewPath = None
#Maps the xmls to the id's of the items that were actually changed
ModifiedIDs = {}
#Maps an xml sheet to it's version number
XMLVersions = {}
#Boolean to know if we are in trailor record section for version number editing
trailorRecordSection = False


#Used to store a parsed item. identifier is EFSPath or NVNum etc, type is EFS_ITEM or NV_ITEM etc
ParsedItem = collections.namedtuple("ParsedItem",'identifier,newValue,carrier,subtypes,type,attributes')

#Returns the path to the directory to check out a workspace for the given package warehouse number
def GetPathForWorkspace(pwNum):

    path = os.getcwd()
    pwFolderName = "PW%s"%(pwNum)

    while True:
        if os.path.exists(path+"\\"+pwFolderName):
            path = path+"\\"+pwFolderName
            return path
        elif os.path.exists(path+"\\builds"):
            path = path+"\\builds"

            if os.path.exists(path+"\\"+pwFolderName):
                path = path+"\\"+pwFolderName
            else:
                os.makedirs(path+"\\"+pwFolderName)
                path = path+"\\"+pwFolderName

            break

        parent = os.path.abspath(path+"\\..")



        #There is no builds no directory. Need to create one
        if parent == path:
            os.makedirs(path+"\\builds\\"+pwFolderName)
            path = path+"\\builds\\"+pwFolderName
            break
        else:
            path = parent

    #Gets rid of double backslash from C:\\builds
    path = os.path.normpath(path)
    path += "\\"
    return path

#Checkout a workspace for the given package warehouse
def CreateView(PackageWarehouse,APICR):
    match = re.search(r'(\d).(\d)$',PackageWarehouse)
    pwNum = match.group(1) + '' + match.group(2)
    ViewName = "CR%s_PW%s_%s" % (APICR, pwNum,str(time.time())[:-3])
    path = GetPathForWorkspace(pwNum)
    ClientSpec = None
    os.chdir(path)
    cmd = "vce view --base %s --checkout %s --multicr 2>&1" % (PackageWarehouse, ViewName)
    result = os.popen(cmd).read()
    # print("About to run command:\t" + cmd)
    for line in result.splitlines():

        if re.search(r'Client .* saved.',line):
            ClientSpec = re.search(r'Client (.*) saved.',line).group(1)
        elif re.search(r'Success',line):
            fields = line.split('\\')
            ViewName = fields[-1]

    # ClientSpec = "mmisa.mcfg.mpss.5.0.CR884062_PW50"
    # ViewName = "mcfg.mpss.5.0.CR884062_PW50"
    path += ViewName + "\\" 
    return (ClientSpec, ViewName, path)

#Verifies that we have access to VCE and P4
def CheckPathToVCEandP4():
    global jira
    #Need to create a vce directory??
    Output = ""
    Output = os.popen('vce.exe --version').read()
    if not Output:
        LogPrint("Unable to launch VCE utility. Please check your PATH\n")
        return 0

    P4Version = os.popen('p4.exe').read()
    if not P4Version:
        LogPrint("Unable to launch P4 utility. Please add the Perforce directory to your PATH. Usually C:\\Program Files\\Perforce.\n")
        return 0

    LoggedIn = 0
    cmd = 'echo %s| p4 -u %s login' % (jira.pw, jira.user)
    Output = os.popen(cmd).read()

    if re.search(r'logged in', Output):
        LogPrint("Logged into the Perforce server")
    else:
        LogPrint("Unable to log into the Perforce server")
        LogPrint("Please check your password and run the script again")
        return 0
    return 1


def getJiraTicket(query,jira_release, jira_report):
    try:
        jira = JIRA(user,pw)
    except:
        print "Failed to create JIRA object"
    return jira.getJIRAIssues(query, JIRA_ISSUES_INTERVAL, JIRA_FIELDS, jira_release, jira_report)


def LookupDataFromJira(JiraTicketNum):
    failed = 0
    issue = getJiraTicket("project = MCFG AND key = MCFG-")

def LogPrint(Str):
    logging.info(Str)

def VerbosePrint(Str):
    return
    # print()
    #add ability to print to log file
    # logging.info(Str) 

#Leave a commment asking to follow the template
def requestTemplate(jira_release, key, extra = ""):
    comment = "The MCFG Automation script has determined that this ticket is not following the template found at http://qwiki.qualcomm.com/qct-swarchitecture/MPSS/BinaryModem/MCFGCCB.\nPlease follow the guidelines on this page to ensure the ticket is processed correctly.\n\n"
    comment = comment + extra
    creator = jira_release[key]['creator']['name']
    LogPrint("Getting ready to change assignee to " + creator + " and add comment\n" + comment + " ticket " + key)
    # jira.changeAssignee(key,creator)
    # jira.addComment(key,comment)
 
#add the labels to the ticket
def addPLLabels(Labls, ticketLbls, key):
    global jira
    for label in Labls:
        if label not in ticketLbls:
            jira.addLabel(key,label)
            LogPrint("Getting ready to add label " + label + " to ticket: " + key)

#Get the labels to add to the ticket
def getLabels(ReleasePLs):
    for PL in ReleasePLs:
        matches = re.findall(r'AT|TA|JO|TH|DPM|BO', PL)
        for match in matches:
            Labels.add(match)

    return len(Labels)

# Run findbuild command under system process
def getFindBuildInfo(ReleasePL):
    FindBuildArgs = ["findbuild", "-latest", ReleasePL]
    try:
        FindbuildOp = subprocess.check_output(FindBuildArgs)
    except:
        print "Exception issuing cmd with following tokens:" + "', '".join(FindBuildArgs)
        FindbuildOp = None

    return FindbuildOp


def getLatestRelease(ReleasePL):
    CRMRelID = None
    #TODO: add support for when actual release ID is provided
    CRMregex = ReleasePL + '-*-1' # regex with Bash/MSDOS style wildcard
    FindbuildResp = getFindBuildInfo(CRMregex)
    CRMregex = ReleasePL + '-.+-1' # regex change for python equivalent to *

    match = re.search(r"(%s)" % CRMregex, FindbuildResp, re.IGNORECASE)
    if match:
        CRMRelID = match.group(1)

    return CRMRelID

#create a CR for the issue
def CreateAPICR(title,description,JustificationNotes,keys = [],PackageWarehouses=[],ReleasePLs=set(), jira=None):
    Analysis = ','.join(keys)
    ProductLines = ""
    MPSSReleasePLs = []
    MPSSRelease = ""
    for pl in ReleasePLs:
        if not pl:
            continue
        else:
            if not 'MPSS' in pl:
                pl = 'MPSS.' + pl
            MPSSReleasePLs.append(pl)
    ProductLines = ", ".join(MPSSReleasePLs)

    if not ProductLines:
        LogPrint("Unable to find any Product Lines for PRISM CR.\n")
        return 1
    else:
        MPSSRelease = MPSSReleasePLs[0]

    ReleaseID = getLatestRelease(MPSSRelease)

    LogPrint("Found %s for APICR" % (ProductLines))

    Options = collections.namedtuple('Options','user,password,Title,Description,JustificationNotes,Analysis,JiraTicketNo,SoftwareImageReported,Files,RelatedCRs,ReleaseID,ReportedSWImage')
    options = Options(jira.user,jira.pw,title,description,JustificationNotes,Analysis,keys, ProductLines,"","",ReleaseID,MPSSRelease)

    prism = PrismClient(jira.pw)
    #cr = prism.get_changerequest_by_id("938103")
    cr = prism.create_changerequest(options)

    # cr = 884062
    for pw in PackageWarehouses:
        prism.add_changetask(cr,pw)


    prism.requestApprovals(cr)
    return cr

#Get the paths to all the relevant MBN's to check out
def getMBNpaths(carrier,subtypes):
    global MBN_PATH
    global currentViewPath
    carrierFolder = getCarrierFolder(carrier, currentViewPath+XML_PATH)
    paths = []
    if subtypes[0].lower() == 'all':
        subtypes = getSubtypes(carrier, currentViewPath)

    for subtype in subtypes:
        path = ""

        #If the subtype has a dash then there is a folder name in the subtype name
        if re.search(r'-', subtype):
            first = 1
            leaf = None
            for subdir in subtype.split('-'):
                if first:
                    first = 0
                    leaf = subdir
                else:
                    path += subdir + "\\"

            path += leaf + "\\mcfg_sw.mbn"
        else:
            path = subtype + "\\mcfg_sw.mbn"
        absPath = currentViewPath+MBN_PATH+carrierFolder + "\\" + carrier + "\\" + path
        LogPrint("Adding %s to MBNPaths" %(absPath))
        paths.append(absPath)

    return paths 

#Get the paths to all the relevant XMLs to check out
def getSpreadsheetXMLPaths(carrier,subtypes,itemID):
    global XML_PATH
    global currentViewPath
    global ModifiedIDs
    carrierFolder = getCarrierFolder(carrier,currentViewPath+XML_PATH)
    paths = []

    if subtypes[0].lower() == "all":
        subtypes = getSubtypes(carrier,currentViewPath+XML_PATH)


    for subtype in subtypes:
        absPath = currentViewPath + XML_PATH +"%s\\%s\\mcfg_sw_gen_%s.xml" %(carrierFolder,carrier,subtype)
        LogPrint("Adding %s to xml paths" % (absPath))
        if absPath in ModifiedIDs:
            ModifiedIDs[absPath] += ", "+itemID
        else:
            ModifiedIDs[absPath] = itemID
        paths.append(absPath)

    spreadsheetPath = currentViewPath + XML_PATH +"%s\\%s\\MCFG_SW_Items_List_Macro.xlsm" %(carrierFolder,carrier)
    LogPrint("Adding spreadsheet path '%s' to paths")
    paths.append(spreadsheetPath)

    return paths

#Checks out the file under the current client
def checkoutFile(filePath):
    global P4Prefix
    cmd = P4Prefix + " edit " + filePath
    result = os.popen(cmd).read()
    LogPrint("Issuing command %s result is:\n %s" % (cmd,result))

#Checks out an array of filepaths under the current client
def checkoutFiles(fileArr):
    for filePath in fileArr:
        checkoutFile(filePath)

#Gets the subtype from the xml path
def getSubtypeFromXMLPath(xmlPath):
    match = re.search(r'mcfg_sw_gen_(.+)\.xml',xmlPath)
    if match:
        return match.group(1)
    return None

#Brings in the carrier policy for the given pw
def getCarrierPolicy():
    global CurrentPW
    global CarrierPolicyMap
    global POLICY_PATH
    global P4Prefix

    Client = re.search('p4 -c (\S+)', P4Prefix).group(1)
    if not CurrentPW in CarrierPolicyMap['PackageWarehouses']:
        print "Can't find this policy..."
        return
    policyLocation = CarrierPolicyMap['PackageWarehouses'][CurrentPW] 
    clientView = "\t+" + policyLocation + " //" + Client+ POLICY_PATH
    result = os.popen('p4 client -o > temp.txt 2>&1').read()
    if os.path.isfile(".\\temp.txt"):
        fd = open('temp.txt','r')
        fdRead = fd.read()
        fd.close()
        fw = open('temp.txt', 'w')
        for line in fdRead.splitlines():
            if re.match(r'View:', line):
                fw.write(line+"\n")
                fw.write(clientView+"\n")
            else:
                fw.write(line+"\n")
        fw.close()
        res = os.popen('type temp.txt | p4 client -i').read()
        if re.search(r'saved', res):
            LogPrint("Succesfully Added Carrier Policy for Package Warehouse " + CurrentPW)
        else:
            LogPrint("Failed to add Carrier Policy for Package Warehouse " + CurrentPW)

        os.popen('p4 sync')

        os.remove('temp.txt')

#builds the mbn
def buildMBN(xmlPath,item):
    global CurrentPW
    global currentViewPath
    
    cwd = os.getcwd()
    carrierFolder = getCarrierFolder(item.carrier, currentViewPath+XML_PATH)
    # carrierFolder = ['Carriers'][item.carrier]
    buildsDir = currentViewPath+"modem_proc\\mcfg\\build"
    os.chdir(buildsDir)
    # getCarrierPolicy()
    subtype = getSubtypeFromXMLPath(xmlPath)
    cmd = "build_mcfgs.exe --force-regenerate --force-rebuild --source-dir=generic/%s/%s --configs=mcfg_sw:%s --xml 2>&1" %(carrierFolder,item.carrier,subtype)
    result = os.popen(cmd).read()
    print(result)
    os.chdir(cwd)
    LogPrint(result)

def removeNV(XMLPaths, item):
    removing = False
    for path in XMLPaths:
        if not re.search(r'xml', path):
            continue
        fr = open(path, 'r')
        fileContent = fr.read()
        fr.close()
        fileWriter = open(path, 'w')
        arr = fileContent.splitlines()
        i = 0

        while i < len(arr):
            line = versionNumber(path, arr, i)
            regex = r'.*<NvItemData id='+re.escape('"'+item.identifier+'"')+r'.*mcfgAttributes="([^"]+)"'
            if re.search(regex, line):
                while not re.search(r'</NvItemData>', line):
                    i+= 1
                    line=arr[i]
                i += 1
                line=arr[i]
            fileWriter.write(line+"\n")
            i += 1
        fileWriter.close()


#Handles the editing of the version number
def versionNumber(path,arr, index):
    global XMLVersions
    global trailorRecordSection
    line = arr[index]
    versionMatch = re.search(r'.*<NvConfigurationData .*version="([^"]+)"', line)
    if versionMatch:
        #Make sure the version number is not incremented multiple times
        if path not in XMLVersions:
            line = re.sub(r'version="[^"]+"','version="' + str(hex(int(versionMatch.group(1),0)+1))+'"', line)
    
    
    elif re.search(r'.*<NvTrlRecord', line):
        trailorRecordSection = True

    while trailorRecordSection:
        index += 1
        v = re.search(r'>(\d+) <', arr[index])
        if v:
            index += 2
            if v.group(1) == '1':
                version = int(re.search(r'>(\d+) <', arr[index]).group(1))
                version+= 1
                arr[index] = re.sub(r'>(\d+) <','>'+str(version)+' <',arr[index])
                print("new version " + str(hex(version)))
                XMLVersions[path] = version
            elif v.group(1) == '5':
                arr[index] = re.sub(r'>(\d+) <','>'+str(XMLVersions[path])+' <',arr[index])
                print("new QC version " + str(hex(XMLVersions[path])))
            #The entry with type 2 has 4 fields instead of 3
            elif v.group(1) == '2':
                index += 1
            elif v.group(1) =='4':
                index += 2

        elif re.search(r'</NvTrlRecord>', arr[index]):
            trailorRecordSection = False

    return line


#Make NV changes
def makeNVChanges(XMLPaths,item):
    found = 0
    values = re.split(r',\s*|\s+', item.newValue)
    XMLMap = None
    for path in XMLPaths:
        found = 0
        changeMade = 0
        #Make sure this is a xml not the spreadsheet
        if not re.search(r'xml',path):
            continue

        fr = open(path, 'r')
        fileContent = fr.read()
        fr.close()
        fileWriter = open(path,'w')
        count = 0
        arr = fileContent.splitlines()
        trailorRecordSection = False
        versionFound = False
        i = 0
        while i < len(arr):
            line = versionNumber(path,arr, i)            
            regex = r'.*<NvItemData id='+re.escape('"'+item.identifier+'"')+r'.*mcfgAttributes="([^"]+)"'
            match = re.search(regex, line)
            #When we find the match grab the attributes
            if match:
                found = 1
                item = item._replace(attributes=match.group(1))
            #We are in the data section of the nv Item
            elif found:
                skipped = False
                for value in values:
                    #Check if it is a multisim item
                    if not skipped and (int(item.attributes,0) & 0x10) != 0:
                        match = re.search(r'<Member[^>]+>(\S+).*</Member>',line)
                        if match:
                            #Check if default sub mask
                            if int(match.group(1),0) == 0x07:
                                fileWrite.write(line+"\n")
                                i += 1
                                line = arr[i]
                        
                        skipped = True
                    #Make the change
                    newLine = re.sub(r'>.*<','>' + value+' <', line)
                    if newLine != line:
                        changeMade = 1
                        line = newLine

                    fileWriter.write(line+"\n")
                    i += 1
                    line = arr[i]

                found = 0
                continue

            fileWriter.write(line+"\n")
            # LogPrint(line+"\n")
            count += 1
            i+=1
        fileWriter.close()


def removeEFS(XMLPaths, item):
    global EFS_FILE_PATH
    global currentViewPath
    global P4Prefix

    fileURI = item.identifier
    fileName = getFileNameFromURL(fileURI)
    fileLocation = currentViewPath+EFS_FILE_PATH+item.carrier+"\\"+fileName

    
    cwd = os.getcwd()
    os.chdir(saveLocation)
    if os.path.isfile(filename):
        cmd = P4Prefix + " delete -v " + fileLocation
        print("About to execute command " + cmd)

    

#Make EFS file changes
def makeEFSChanges(XMLPaths,item):
    #grab the file
    #figure out the location to put the file
    #put the file there

    #edit xml version number?
    global jira
    global EFS_FILE_PATH
    global currentViewPath
    global P4Prefix
    addFile = True
    fileURI = item.newValue
    openedAttachment = jira.openAttachment(fileURI)
    saveLocation = currentViewPath+EFS_FILE_PATH+item.carrier+"\\"
    print(saveLocation)
    cwd = os.getcwd()
    os.chdir(saveLocation)
    filename = getFileNameFromURL(fileURI)
    filecontents = openedAttachment.read()
    
    if os.path.isfile(filename):
        checkoutFile(saveLocation+filename)
        addFile = False

    with open(filename, 'w') as fh:
        fh.write(filecontents)


    if addFile:
        cmd = P4Prefix + " add -d " +  re.sub(r'\\','/',saveLocation) + filename
        result = os.popen(cmd).read()
        print(result)
    os.chdir(cwd)


def makeEFSItemChanges(XMLPaths, item):
    found = 0
    values = re.split(r',\s*|\s+', item.newValue)
    XMLMap = None
    for path in XMLPaths:
        found = 0
        changeMade = 0
        #Make sure this is a xml not the spreadsheet
        if not re.search(r'xml',path):
            continue

        fr = open(path, 'r')
        fileContent = fr.read()
        fr.close()
        fileWriter = open(path,'w')
        count = 0
        arr = fileContent.splitlines()
        trailorRecordSection = False
        versionFound = False
        i = 0
        while i < len(arr):
            line = versionNumber(path,arr, i) 
            regex = r'.*<NvEfsItemData mcfgAttributes="([^"]+)" mcfgVariant="([^"]+)" fullpathname="'+ item.identifier + '">'           
            match = re.search(regex, line)
            #When we find the match grab the attributes
            if match:
                found = 1
                item = item._replace(attributes=match.group(1))
            #We are in the data section of the nv Item
            elif found:
                skipped = False
                for value in values:
                    #Check if it is a multisim item
                    if not skipped and (int(item.attributes,0) & 0x10) != 0:
                        match = re.search(r'<Member[^>]+>(\S+).*</Member>',line)
                        if match:
                            #Check if default sub mask
                            if int(match.group(1),0) == 0x07:
                                fileWriter.write(line+"\n")
                                i += 1
                                line = arr[i]
                        
                        skipped = True
                    #Make the change
                    newLine = re.sub(r'>.*<','>' + value+' <', line)
                    if newLine != line:
                        changeMade = 1
                        line = newLine

                    fileWriter.write(line+"\n")
                    i += 1
                    line = arr[i]

                found = 0
                continue

            fileWriter.write(line+"\n")
            # LogPrint(line+"\n")
            count += 1
            i+=1
        fileWriter.close()



#Make the changes for the parsedItem
def makeChanges(SpreadsheetXMLPaths, item):
    global EFS_ITEM
    global NV_ITEM

    if item.type == NV_ITEM:
        if re.search(r'REMOVE',item.newValue):
            removeNV(SpreadsheetXMLPaths, item)
        else:
            makeNVChanges(SpreadsheetXMLPaths, item)
    elif item.type == EFS_ITEM:

        makeEFSChanges(SpreadsheetXMLPaths,item)

    elif item.type == EFS_ITEM_TYPE:
        makeEFSItemChanges(SpreadsheetXMLPaths,item)

    for path in SpreadsheetXMLPaths:
        if not re.search(r'xlsm', path):
            buildMBN(path,item)

#integrate changes
def processCR(crNum,  PackageWarehouses, parsedItems, jiraObject):
    global APICR
    global P4Prefix
    global CurrentPW
    global currentViewPath
    global jira
    global ModifiedIDs
    global XMLVersions
    jira = jiraObject

    APICR = crNum
    for pw in PackageWarehouses:
        CurrentPW = pw
        (client, view, currentViewPath) = CreateView(pw,APICR)
        P4Prefix = "p4 -c " + client

        for item in parsedItems:
            MBNPaths = []
            SpreadsheetXMLPaths = []
            MBNPaths = getMBNpaths(item.carrier,item.subtypes)
            SpreadsheetXMLPaths = getSpreadsheetXMLPaths(item.carrier,item.subtypes,item.identifier)
            checkoutFiles(MBNPaths)
            checkoutFiles(SpreadsheetXMLPaths)
            makeChanges(SpreadsheetXMLPaths,item)

        for path in ModifiedIDs:
            cmd = 'perl C:\mcfg_automation\JIRA_python\\testing.pl --xml '+ path + ' --id "' + ModifiedIDs[path]+'" --version ' + str(hex(XMLVersions[path]))
            result = os.popen(cmd).read()
            # print(result)
        print("Finished with " + pw)



#Fix the issue
def processTicket(ticket,Analysis=""):
    global APICR
    global PackageWarehouse
    global P4Prefix
    global CurrentPW
    global currentViewPath
    global ModifiedIDs
    # if not APICR:
    #     APICR = CreateAPICR(ticket,Analysis)
    APICR = 884062
    for pw in PackageWarehouses:
        CurrentPW = pw
        (client, view, currentViewPath) = CreateView(pw,APICR)
        P4Prefix = "p4 -c " + client

        for item in parsedItemsList:
            MBNPaths = []
            SpreadsheetXMLPaths = []
            MBNPaths = getMBNpaths(item.carrier,item.subtypes)
            SpreadsheetXMLPaths = getSpreadsheetXMLPaths(item.carrier, item.subtypes)
            checkoutFiles(MBNPaths)
            checkoutFiles(SpreadsheetXMLPaths)
            makeChanges(SpreadsheetXMLPaths,item)




#Used to parse a prism CR
#Puts PW's corresponding to ReleasePL's in PackageWarehouses
#Parses EFS and NV items and puts it in parsedItems list
def parseCR(description,PackageWarehouses, parsedItems,jiraObject=None ):

    global jira
    global parsedItemsList
    jira = jiraObject

    NVItems = []
    EFSItems = []
    ReleasePLs = set()

    EFSItemStart = 0
    EFSIndex = 0
    ImplementationCRStart = 0
    ReleasePLStart  = 0
    GeneralDescriptionStart = 0
    NVItemStart = 0
    NVIndex = 0
    DataProfileStart = 0
    TrailerRecordStart = 0
    MCFGReviewerStart = 0
    ReviewResultStart = 0
    ReleaseInfoStart = 0
    NotesStart = 0
    DependencyStart = 0
    Index = 0 
    Error = 0
    ErrorMessage = ""  

    newSection = re.compile(r'===')
    for line in description.splitlines():
        if ReleasePLStart:
            if newSection.search(line):
                VerbosePrint("Release PLs ends.")
                # removeBlanklines
                ReleasePLStart = 0
            else:
                VerbosePrint(line)
                if len(ReleasePLs) == 0:
                    ReleasePLs = re.split(r',\s*|\s+',line)
                else:
                    ReleasePLs = ReleasePLs + re.split(r',\s*|\s+',line)
        elif GeneralDescriptionStart:
            if newSection.search(line):
                VerbosePrint("General Description ends.")
                # removeBlankLines
                GeneralDescriptionStart = 0
            else:
                VerbosePrint(line)
                #do something with general description
        elif NVItemStart:
            if newSection.search(line):
                VerbosePrint("NVItem ends")
                #removeBlanklines
                NVItemStart = 0
                NVIndex += 1
            else:
                VerbosePrint(line)
                NVItems[NVIndex].append(line)
        elif EFSItemStart:
            if newSection.search(line):
                VerbosePrint("EFSItem ends")
                #removeBlankLines
                EFSItemStart = 0
                EFSIndex += 1
            else:
                VerbosePrint(line)
                EFSItems[EFSIndex].append(line)
        elif DataProfileStart:
            if newSection.search(line):
                VerbosePrint("Data Profile ends")
                #removeBlankLines
                DataProfileStart = 0
            else:
                VerbosePrint(line)
                #do something with data profile stuff
        elif TrailerRecordStart:
            if newSection.search(line):
                VerbosePrint("Trailor Record ends")
                #removeBlankLines
                TrailerRecordStart = 0
            else:
                VerbosePrint(line)
                #do something with trailor record
        elif MCFGReviewerStart:
            if newSection.search(line):
                VerbosePrint("MCFGReviewer ends")
                #removeBlankLines
                MCFGReviewerStart = 0
            else:
                VerbosePrint(line)
                #do something with MCFGReviewer
        elif ReviewResultStart:
            if newSection.search(line):
                VerbosePrint("ReviewResult ends")
                #removeBlankLines
                ReviewResultStart = 0
            else:
 
                VerbosePrint(line)
                #do something with review result
        elif ImplementationCRStart:
            if newSection.search(line):
                VerbosePrint("ImplementationCR ends")
                #removeBlankLines
                ImplementationCRStart = 0
            else:
                VerbosePrint(line)
                #do something with implementation CR
        elif ReleaseInfoStart:
            if newSection.search(line):
                VerbosePrint("ReleaseInfo ends")
                #removeBlankLines
                ReleaseInfoStart = 0
            else:
                VerbosePrint(line)
                #do something with ReleaseInfo
        elif NotesStart:
            if newSection.search(line):
                VerbosePrint("Notes ends")
                NotesStart = 0
                # break
            else:
                VerbosePrint(line)
                
                #do something with Notes

        if re.search(r'===Release PLs===', line):
            ReleasePLStart = 1
        elif re.search(r'===General Description===', line):
            GeneralDescriptionStart = 1
        elif re.search(r'===NV Item===', line):
            NVItems.append([])
            NVItemStart = 1
        elif re.search(r'===EFS Item===', line):
            EFSItems.append([])
            EFSItemStart = 1
        elif re.search(r'===Data Profile===', line):
            DataProfileStart = 1
        elif re.search(r'===Trailor Record===', line):
            TrailerRecordStart = 1
        elif re.search(r'===MCFG reviewer===', line):
            MCFGReviewerStart = 1
        elif re.search(r'===Review Result===', line):
            ReviewResultStart = 1
        elif re.search(r'===Implementation CR(s)===', line):
            ImplementationCRStart = 1
        elif re.search(r'===Release Info===', line):
            ReleaseInfoStart = 1
        elif re.search(r'===Notes===', line):
            NotesStart = 1
       
        Index += 1

    if len(ReleasePLs) == 0:
        LogPrint("Error: This ticket is not following template: " + key)
        requestTemplate(jira_release,key, "Unable to find any ReleasePLs for this ticket.\n\n")
        return 0

    newPackageWarehouses = set()

    if not getPackageWarehouses(ReleasePLs,newPackageWarehouses):
        ErrorMessage += "Unable to find package warehouse for given PL: " + ', '.join(ReleasePLs) + '\n\n'
        LogPrint("Error: Unable to find package warehouses for ticket: " )

    else:
        if len(PackageWarehouses) == 0:
            for pw in newPackageWarehouses:
                PackageWarehouses.add(pw)
        elif len(PackageWarehouses.intersection(newPackageWarehouses)) != len(PackageWarehouses):
            ErrorMessage += "This ticket, " + key + ", can't be combined with ticket(s) " + correctKeys + " because it does not share the following Package Warehouses"
            ErrorMessage +=  " " + PackageWarehouses.symmetric_difference(newPackageWarehouses) + " \n"




        
    (result, message) = checkNVItems(NVItems)
    Error = Error | result  
    ErrorMessage += message

    (result, message) = checkEFSItems(EFSItems)
    Error = Error | result  
    ErrorMessage += message

    parsedItems += parsedItemsList

    if Error:
        # requestTemplate(jira_release,key,ErrorMessage)
        return 0

    return 1

#Used to parse a jira ticket
#Puts PW's corresponding to ReleasePL's in PackageWarehouses
#Ensures required values are there and adds to the combinedDesc and correct keys if it is correct
def parseTicket(jira_release,key,combinedDesc = {},PackageWarehouses=set(),ProductLines=set(),correctKeys = set(), jiraObject = None, JustificationNotes = []):
    global jira
    NVItems = []
    EFSItems = []
    Notes = []

    jira = jiraObject
    combinedDescription = []
    if not 'GeneralDescription' in combinedDesc:
        combinedDesc["GeneralDescription"] = {}
    GeneralDescription = combinedDesc["GeneralDescription"]
    if not 'ProblemImpact' in GeneralDescription:
        GeneralDescription['ProblemImpact'] = []

    GeneralDescriptionRaw = []
    subSection = None

    ticket = jira_release[key]
    if not ticket['description']:
        LogPrint("Error: This ticket has no description: " + key)
        return 0
    lines = ''.join(ticket['description'])
    description = lines.splitlines()
    ReleasePLs = set()

    EFSItemStart = 0
    EFSIndex = 0
    ImplementationCRStart = 0
    ReleasePLStart  = 0
    GeneralDescriptionStart = 0
    NVItemStart = 0
    NVIndex = 0
    DataProfileStart = 0
    TrailerRecordStart = 0
    MCFGReviewerStart = 0
    ReviewResultStart = 0
    ReleaseInfoStart = 0
    NotesStart = 0
    DependencyStart = 0
    Index = 0 
    Error = 0
    ErrorMessage = ""  

    newSection = re.compile(r'===')
    for line in description:
        combinedDescription.append(line + "\n")

        if ReleasePLStart:
            # replace the often used ':' or ' ' with .
            # between the letters and numbers of PL
            formattedLine = re.sub('([a-zA-Z]{2})[\s+:](\d)',r'\1.\2', line)
            if newSection.search(formattedLine):
                VerbosePrint("Release PLs ends.")
                # removeBlanklines
                ReleasePLStart = 0
            else:
                VerbosePrint(formattedLine)
                if len(ReleasePLs) == 0:
                    ReleasePLs = re.split(r',\s*|\s+',formattedLine)
                else:
                    ReleasePLs = ReleasePLs + re.split(r',\s*|\s+',formattedLine)
        elif GeneralDescriptionStart:
            if newSection.search(line):
                VerbosePrint("General Description ends.")
                if not 'GeneralDescriptionRaw' in GeneralDescription:
                        GeneralDescription['GeneralDescriptionRaw'] = []
                #Remove blank lines from description
                GeneralDescription['GeneralDescriptionRaw'] = filter(None, GeneralDescription['GeneralDescriptionRaw'])
                # Append separator in case multiple key descriptions are included
                GeneralDescriptionRaw.append('-' * 15)
                GeneralDescription['GeneralDescriptionRaw'] += (GeneralDescriptionRaw)
                # removeBlankLines
                GeneralDescriptionStart = 0
                subSection = None
            else:
                VerbosePrint(line)
                GeneralDescriptionRaw.append(line)
                if IsSubSection(line):
                    subSection = SelectSubSection(line)
                    # remove subsection heading from line
                    line = re.sub('=.*=:\s+', '', line)
                if subSection:
                    if not subSection in GeneralDescription:
                        GeneralDescription[subSection] = []
                    GeneralDescription[subSection].append(line)
        elif NVItemStart:
            if newSection.search(line):
                VerbosePrint("NVItem ends")
                #removeBlanklines
                NVItemStart = 0
                NVIndex += 1
            else:
                VerbosePrint(line)
                NVItems[NVIndex].append(line)
        elif EFSItemStart:
            if newSection.search(line):
                VerbosePrint("EFSItem ends")
                #removeBlankLines
                EFSItemStart = 0
                EFSIndex = 0
            else:
                VerbosePrint(line)
                EFSItems[EFSIndex].append(line)
        elif DataProfileStart:
            if newSection.search(line):
                VerbosePrint("Data Profile ends")
                #removeBlankLines
                DataProfileStart = 0
            else:
                VerbosePrint(line)
                #do something with data profile stuff
        elif TrailerRecordStart:
            if newSection.search(line):
                VerbosePrint("Trailor Record ends")
                #removeBlankLines
                TrailerRecordStart = 0
            else:
                VerbosePrint(line)
                #do something with trailor record
        elif MCFGReviewerStart:
            if newSection.search(line):
                VerbosePrint("MCFGReviewer ends")
                #removeBlankLines
                MCFGReviewerStart = 0
            else:
                VerbosePrint(line)
                #do something with MCFGReviewer
        elif ReviewResultStart:
            if newSection.search(line):
                VerbosePrint("ReviewResult ends")
                #removeBlankLines
                ReviewResultStart = 0
            else:
                VerbosePrint(line)
                #do something with review result
        elif ImplementationCRStart:
            if newSection.search(line):
                VerbosePrint("ImplementationCR ends")
                #removeBlankLines
                ImplementationCRStart = 0
            else:
                VerbosePrint(line)
                #do something with implementation CR
        elif ReleaseInfoStart:
            if newSection.search(line):
                VerbosePrint("ReleaseInfo ends")
                #removeBlankLines
                ReleaseInfoStart = 0
            else:
                VerbosePrint(line)
                #do something with ReleaseInfo
        elif NotesStart:
            if newSection.search(line):
                VerbosePrint("Notes ends")
                #Remove empty list elements
                Notes = filter(None, Notes)
                NotesStart = 0
            else:
                VerbosePrint(line)
                Notes.append(line)
                break
                #do something with Notes

        if re.search(r'===Release PLs===', line):
            ReleasePLStart = 1
        elif re.search(r'===General Description===', line):
            GeneralDescriptionStart = 1
        elif re.search(r'===NV Item===', line):
            NVItems.append([])
            NVItemStart = 1
            combinedDescription.append("Request: " + key + "\n")
        elif re.search(r'===EFS Item===', line):
            EFSItems.append([])
            EFSItemStart = 1
            combinedDescription.append(   "Request: " + key + "\n")
        elif re.search(r'===Data Profile===', line):
            DataProfileStart = 1
        elif re.search(r'===Trailor Record===', line):
            TrailerRecordStart = 1
        elif re.search(r'===MCFG reviewer===', line):
            MCFGReviewerStart = 1
        elif re.search(r'===Review Result===', line):
            ReviewResultStart = 1
        elif re.search(r'===Implementation CR(s)===', line):
            ImplementationCRStart = 1
        elif re.search(r'===Release Info===', line):
            ReleaseInfoStart = 1
        elif re.search(r'===Notes===', line):
            NotesStart = 1
       
        Index += 1


    combinedDescription.append("\n\n")
    if not 'EntireTicketDesc' in combinedDesc:
        combinedDesc['EntireTicketDesc'] = []
    combinedDesc['EntireTicketDesc'] += combinedDescription

    JustificationNotes.append(str(datetime.date.today()) + "\n=Justification=:  " + PopulateJustification(Notes, GeneralDescription['ProblemImpact']))
    combinedDesc['GeneralDescription'] = GeneralDescription

    if len(ReleasePLs) == 0:
        LogPrint("Error: This ticket is not following template: " + key)
        requestTemplate(jira_release,key, "Unable to find any ReleasePLs for this ticket.\n\n")
        return 0

    newPackageWarehouses = set()
    if not getPackageWarehouses(ReleasePLs,newPackageWarehouses):
        Labels.clear()
        if getLabels(ReleasePLs):
            addPLLabels(Labels, jira_release[key]['labels'], key)
        ErrorMessage += "Unable to find package warehouse for given PL: " + ', '.join(ReleasePLs) + '\n\n'
        LogPrint("Error: Unable to find package warehouses for ticket: " + key)
    
    else:
        addPLLabels(Labels, jira_release[key]['labels'], key)
        if len(PackageWarehouses) == 0:
            for pw in newPackageWarehouses:
                PackageWarehouses.add(pw)
        elif len(PackageWarehouses.intersection(newPackageWarehouses)) != len(PackageWarehouses):
            ErrorMessage += "This ticket, " + key + ", can't be combined with ticket(s) " + correctKeys + " because it does not share the following Package Warehouses"
            ErrorMessage +=  " " + PackageWarehouses.symmetric_difference(newPackageWarehouses) + " \n"

        
    (result, message) = checkNVItems(NVItems)
    Error = Error | result  
    ErrorMessage += message

    (result, message) = checkEFSItems(EFSItems,key)
    Error = Error | result  
    ErrorMessage += message

    if Error:
        requestTemplate(jira_release,key,ErrorMessage)
        return 0

    for pl in ReleasePLs:
        ProductLines.add(pl)
    correctKeys.add(key)
    return 1


#Returns a set of Package Warehouses that contain the given ReleasePLs
def getPackageWarehouses(ReleasePLs,PackageWarehouses = set()):
    #pdb.set_trace()
    pcm = PCM(jira.pw)
    removed = set()
    for pl in ReleasePLs:
        origPL = pl
        if not pl:
            continue

        if not 'MPSS' in pl:
            pl = 'MPSS.'+pl
        print "Searching for component for " + pl
        PW = pcm.getPackageWarehouse(pl.upper())
        if PW:
            print "Adding component %s for pl %s" %(PW,pl)
            PackageWarehouses.add(PW)
        else:
            print "Couldn't find a component for " + pl
            print "Removing %s from ReleasePLs" % pl
            removed.add(origPL)

    if len(PackageWarehouses) == 0:
        print "Unable to find any package warehouses.."
        sys.exit(0)
    for p in removed:
        ReleasePLs.remove(p)

    return 1


#Return the name of the attached file from the files uri
def getFileNameFromURL(url):
    match = url.split('/')
    return match[-1]

#--------------------------------------------------------------------
#Returns a list of values tuples 
#One item may be split into multiple value tuples if there are multiple carriers
#listed in it.
def getRequiredValues(item,jiraKey=None):
    global jira
    Values = collections.namedtuple("Values", 'PathOrNV, NewValue, Carrier,Subtype,efsType,jiraKey')
    PathOrNV = None
    NewValue = None
    Carrier = None
    Subtype = None
    efsType = None
    itemLst = []
    carriers = []
    subtypes = []
    for line in item:
        if re.search(r'\s*NV Number:',line):
            PathOrNV = re.search(r'\s*NV Number:+\s*(.*)',line).group(1)
        elif re.search(r'\s*EFS Path:', line):
            PathOrNV = re.search(r'\s*EFS Path:+\s*(.*)',line).group(1)
        elif re.search(r'\s*New [^:]*:', line):
            NewValue = re.search(r'\s*New [^:]*:+\s*(.*)',line).group(1)
        elif re.search(r'\s*SW-Carrier.*:', line):
            Carrier = re.search(r'\s*SW-Carrier.*:+\s*(.*)',line).group(1)
            if Carrier:
                carriers = Carrier.split(',')
        elif re.search(r'\s*Config-Subtype:', line):
            Subtype = re.search(r'\s*Config-Subtype:+\s*(.*)',line).group(1)
            if Subtype:
                subtypes = Subtype.split(';')
                # Subtype = re.split(r',\s*|\s+',Subtype)
        elif re.search(r'\s*Request:',line):
            jiraKey = re.search(r'\s*Request:+\s*(\S*)',line).group(1)

    if NewValue and re.search(r'attach', NewValue, flags = re.IGNORECASE):
        attachments = jira.getAttachments(jiraKey)
        #The efs path for this item should contain the name of the attached file
        fileName = getFileNameFromURL(PathOrNV)
        fileLocation = None
        for attachment in attachments:
            #Assign the url of the attachment to NewValue
            if getFileNameFromURL(attachment['content']) == fileName:
                NewValue = attachment['content']
        efsType = EFS_ITEM
    else:
        efsType = EFS_ITEM_TYPE


    if len(carriers) != len(subtypes):
        return [Values(PathOrNV, NewValue, None, None, efsType, jiraKey)]
    else:
        for i in range(len(carriers)):
            itemLst.append(Values(PathOrNV, NewValue, carriers[i], re.split(r',\s*|\s+',subtypes[i]), efsType, jiraKey))
        return itemLst


#Checks if all the required EFS values are there and deteremines
#if it is an EFS file type or and EFS item type based on if 
#newValue is 'attached'
def checkEFSItems(EFSItems,key=None):
    global parsedItemsList
    Error = 0
    ErrorMessage = ""
    efsType = None
    for item in EFSItems:
        ValueList  = getRequiredValues(item,key)
        for Values in ValueList:
            (EFSPath,NewValue,Carrier,Subtype,efsType) = (Values.PathOrNV,Values.NewValue,Values.Carrier,Values.Subtype,Values.efsType)
            All = EFSPath and NewValue and Carrier and Subtype
            Nothing = not(EFSPath or NewValue or Carrier or Subtype)
            if not (All or Nothing):
                Error = 1 
                ErrorMessage += "Unable to find required value(s) in this EFSItem:\n" + '\n'.join(item)
                break
            
            if All:
                parsedItemsList.append(ParsedItem(EFSPath,NewValue,Carrier,Subtype, efsType,0))

    return (Error,ErrorMessage)

#Checks whether a new subsection has been encountered while parsing description
# returns true if a line starts with '=' and contains '=:', false otherwise
def IsSubSection(line):
    return bool(re.match(r'=.*=:', line))

# Returns a subsection label to be used when parsing ticket fields
# subsection is intended to be used as a dictionary key
def SelectSubSection(line):
    subSection = None

    if re.match(r'=Build tested=:',line):
        subSection = 'BuildTested'
    elif re.match(r'=Problem Description=:',line):
        subSection = 'ProblemDescription'
    elif re.match(r'=Problem Impact=:',line):
        subSection = 'ProblemImpact'
    else:
        VerbosePrint('No ticket subSection identified.\n')
    return subSection

# Takes a variable number of arguments and uses the first list
# encountered with at least 1 item. This allows for the priority
# for a note source to be determined based on where it is in the list
def PopulateJustification(*args):
    retval = ''
    for arg in args:
        if isinstance(arg, list) and len(arg) > 0:
            try:
                retval = "\n".join(arg)
                return retval
            except:
                print "not a list"
    return retval

#Checks if all the nv items have all the required values they need
def checkNVItems(NVItems):
    Error = 0
    ErrorMessage = ""
    for item in NVItems:
        ValueList = getRequiredValues(item)
        for Values in ValueList:
            (NVNum,NewValue,Carrier,Subtype,efsType) = (Values.PathOrNV,Values.NewValue,Values.Carrier,Values.Subtype,Values.efsType)
            All = NVNum and NewValue and Carrier and Subtype
            Nothing = not(NVNum or NewValue or Carrier or Subtype)
            if not (All or Nothing):
                Error = 1 
                ErrorMessage += "Unable to find required value(s) in this NVItem:\n" + '\n'.join(item)
                break

            if All:
                parsedItemsList.append(ParsedItem(NVNum,NewValue,Carrier,Subtype, NV_ITEM,0))

    return (Error,ErrorMessage)


#Returns the folder that a carrier is in
def getCarrierFolder(carrier, path):
    for root, dirs, files  in os.walk(path):
        if carrier in dirs:
            return re.split(r'\\',root)[-1]


def getSubtypes(carrier, path):
    lst = []
    for root, dirs, files in os.walk(path):
        if carrier in root:
            for f in files:
                match = re.search(r'mcfg_sw_gen_(.+)\.xml', f)
                if match:
                    lst.append(match.group(1))
    return lst



#Reads the xml files and puts them into their global map variables
def readXMLs():

    #xml_path = join(os.path.dirname(os.path.realpath(__file__))
    with open('.\CarrierPolicy.xml') as fd:
        global CarrierPolicyMap
        CarrierPolicyMap = xmltodict.parse(fd.read())

#returns a string that is the description updated with the CR
#to be used in an update call to the JIRA api
def addImplementationCR(description,link):
    result = ""
    ImplementationCRStart = 0
    for line in description.splitlines():

        if ImplementationCRStart:
            result += link + "\n"
            ImplementationCRStart = 0

        if "Implementation CR(s)" in line:
            ImplementationCRStart = 1

        result += line + "\n"

    return result










