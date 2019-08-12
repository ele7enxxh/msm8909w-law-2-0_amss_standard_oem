#******************************************************************************
#* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg/mcfg_tools/mcfg_automation/PrismClient.py#1 $
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
# DESCRIPTION
#   PrismChangeRequest

# COPYRIGHT
#   All Rights Reserved.  QUALCOMM Proprietary

#   Export of this technology or software is regulated by the U.S. Government.
#   Diversion contrary to U.S. law prohibited.

#   Copyright (c) 2014, QUALCOMM Technologies, Inc.

#   All data and information contained in or disclosed by this document is
#   confidential and propriety information of QUALCOMM Technologies, Inc. and all
#   rights therein are expressly reserved. By accepting this material the
#   recipient agrees that this material and the information contained
#   therein is held in confidence and in trust and will not be used
#   copied, reproduced in whole or in part, nor its contents revealed
#   in any manner to others without the express written permission
#   of QUALCOMM Technologies, Inc.
#
#   Authors: Monte Misa, Phil Mao, and Simon Bang (srbang@qti.qualcomm.com)

from Prism import Prism
import logging
import base64
import datetime
from datetime import timedelta
import re
import sys


class PrismClient(Prism):

    def __init__(self, passwd = None):
        Prism.__init__(self,passwd)
        # logging.basicConfig(filename='suds_messages.log',level=logging.DEBUG)
        # logging.getLogger('suds.client').setLevel(logging.DEBUG)




    def get_changerequest_by_id(self, crid):
        """
        Retrieves ChangeRequest by ChangeRequestId
        """

        request = Prism.client.factory.create("ns1:CRRequest")

        request["ChangeRequestId"] = crid
        request["ApplicationSource"] = Prism.app_source

        response = Prism.client.service.GetChangeRequestById(request)

        return response["ChangeRequest"]


    def save_changerequest(self, changerequest):
        """
        Saves ChangeRequest
        """

        request = Prism.client.factory.create("ns1:SaveCRRequest")

        request["ChangeRequest"] = changerequest
        request["ApplicationSource"] = Prism.app_source
        request["ChangeComments"] = "Put your change comments here and review the comments in PRISM Change Log"
        response = Prism.client.service.SaveChangeRequest(request)

        if response.IsSuccess is True:
            return response["ChangeRequestId"]
        else:
            raise response["ValidationErrors"] # Inspect "ValidationErrors" in case of any errors


    def add_analysis(self, crid):
        """
        Adds analysis text in the CR
        """

        request = Prism.client.factory.create("ns1:AppendAnalysisTextRequest")

        request["Text"] = "PRISM API Analysis Text"
        request["ChangeRequestId"] = crid
        request["ApplicationSource"] = Prism.app_source

        response = Prism.client.service.AppendAnalysisText(request)

        if response.IsSuccess is False:
            raise response["ValidationErrors"]


    # def create_changerequest(self):
    # 	"""
    # 	Creates a CR
    # 	"""

    # 	cr = self._get_changerequest()

    # 	crid = self.save_changerequest(cr)

    # 	return crid

    def create_changerequest(self,options):

        SaveCRRequest = Prism.client.factory.create('ns1:SaveCRRequest')

        SaveCRRequest['UserName'] = options.user
        SaveCRRequest['ChangeRequest'].AssigneeUserName = options.user
        SaveCRRequest['ChangeRequest'].Id = 0
        #SaveCRRequest['ChangeRequest'].ChangeRequestType = "Internal"
        SaveCRRequest['ChangeRequest'].ChangeRequestType = "Bug"
        SaveCRRequest['ChangeRequest'].ReleaseId = options.ReleaseID
        SaveCRRequest['ChangeRequest'].Title = options.Title
        SaveCRRequest['ChangeRequest'].Description = "".join(options.Description['EntireTicketDesc'])
        SaveCRRequest['ChangeRequest'].ThirdPartyNotes = "https://jira.qualcomm.com/jira/browse/" +  "\nhttps://jira.qualcomm.com/jira/browse/".join(options.JiraTicketNo)
        SaveCRRequest['ChangeRequest'].Analysis.Text = "https://jira.qualcomm.com/jira/browse/" +  "\nhttps://jira.qualcomm.com/jira/browse/".join(options.JiraTicketNo)
        SaveCRRequest['ChangeRequest'].Analysis.AstSRAnalysis = "NotABug"

        # Reported Software Image
        ReportedSoftwareImagesArray = options.SoftwareImageReported.split(',')
        ArrayOfReportedSoftwareImage = Prism.client.factory.create('ns1:ArrayOfReportedSoftwareImage')
        index = 0
        while index < len(ReportedSoftwareImagesArray):
            ReportedSoftwareImage = Prism.client.factory.create('ns1:ReportedSoftwareImage')
            ReportedSoftwareImage.SoftwareImageName = ReportedSoftwareImagesArray[index]
            index += 1
            ArrayOfReportedSoftwareImage.ReportedSoftwareImage.append(ReportedSoftwareImage)

        SaveCRRequest['ChangeRequest'].ReportedSoftwareImages = ArrayOfReportedSoftwareImage

        # Participant
        Participant = Prism.client.factory.create('ns1:Participant')
        Participant.Area = "Modem"
        Participant.SubSystem = "Architecture"
        Participant.SwFunctionality = "Configuration"
        Participant.IsPrimary = True
        ArrayOfParticipant = Prism.client.factory.create('ns1:ArrayOfParticipant')
        ArrayOfParticipant.Participant = [ Participant ]
        SaveCRRequest['ChangeRequest'].Participants = ArrayOfParticipant

        #Added for creating bug CR
        SaveCRRequest['ChangeRequest'].IsCRMReleaseId = True
        SaveCRRequest['ChangeRequest'].IsCrash = False
        SaveCRRequest['ChangeRequest'].NoFoundOnSoftwareProductDuringCreation = True
        SaveCRRequest['ChangeRequest'].SoftwareImageName = options.ReportedSWImage
        SaveCRRequest['ChangeRequest'].TargetName = "NA"
        SaveCRRequest['ChangeRequest'].TestPhase = "MST Data Feature Testing"
        #SaveCRRequest['ChangeRequest'].
        SaveCRRequest['ChangeRequest'].HardwarePlatform = "NA"
        SaveCRRequest['ChangeRequest'].Repeatability = "NotAttempted"
        SaveCRRequest['ChangeRequest'].Severity = "Medium"
        SaveCRRequest['ChangeRequest'].Status = "Open"
        SaveCRRequest['ChangeRequest'].TargetGroupStatus = "Open"

        # Reporter
        Reporter = Prism.client.factory.create('ns1:Reporter')
        Reporter.UserName = options.user
        Reporter.TestTeam = 'MST-Modem Architecture'
        Reporter.TestOrganization = "MST"
        # Reporter.TestArea = "SanDiego"
        SaveCRRequest['ChangeRequest'].Reporter = Reporter

        #Analysis section notes
        ChangeRequestNote = Prism.client.factory.create('ns1:ChangeRequestNote')
        ChangeRequestNote.Extension = None
        ChangeRequestNote.Id = 0
        ChangeRequestNote.Text = options.JustificationNotes
        # Notes.TestArea = "SanDiego"
        SaveCRRequest['ChangeRequest'].ChangeRequestNotes = ChangeRequestNote


        # Release Notes
        ReleaseNotes = Prism.client.factory.create('ns1:ReleaseNotes')
        #if 'ProblemDescription' in options.Description['GeneralDescription']:
        #    ReleaseNotes.ProblemDescription = options.Description['GeneralDescription']['ProblemDescription']
        #if 'ProblemImpact' in options.Description['GeneralDescription']:
        #    ReleaseNotes.ProblemScenario = options.Description['GeneralDescription']['ProblemImpact']
        ReleaseNotes.ProblemRecovery = ""
        ReleaseNotes.ProblemOccurrence = "Internal testing"
        ReleaseNotes.ChangeDescription = "MCFG workbook(s) updated with changes requested in CR description. Spreadsheet(s) then used to generate MCFG configs."
        ReleaseNotes.ChangeSideEffects = "None"
        ReleaseNotes.ChangeVerification = "Binary differences done on the before and after mbn configurations to verify only bytes related CR item(s) had been updated."
        ReleaseNotes.FailureFrequency = "Every Time"
        ReleaseNotes.ImpactCategory = "NV Item"

        ReleaseNotes.ScenarioLikelihood = "Unknown"
        ReleaseNotes.CertificationImpacts = "None"
        SaveCRRequest['ChangeRequest'].ReleaseNotes = ReleaseNotes

        #RCA
        RootCauseAnalysis = Prism.client.factory.create('ns1:RootCauseAnalysis')
        RootCauseAnalysis.RCACode = "Configuration Management Error"
        SaveCRRequest['ChangeRequest'].RootCauseAnalysis = RootCauseAnalysis

        #print SaveCRRequest
        SaveCRResponse = Prism.client.service.SaveChangeRequest(SaveCRRequest)
        if SaveCRResponse.IsSuccess == False:
            print "IsSuccess = %s " % str(SaveCRResponse['IsSuccess'])
            print str(SaveCRResponse['ValidationErrors'])
            sys.exit(0)

        return SaveCRResponse['ChangeRequestId']
        # crr = self._get_changerequest()

        # return self.save_changerequest(crr)


    def add_relatedCRs(self,crid,relatedCRs):
        # Related CRs
         # Only if related CRs are identified
        CRRequest = Prism.client.factory.create('ns1:CRRequest')
        CRRequest['ChangeRequestId'] = crid
        CRResponse = Prism.client.service.GetChangeRequestById([CRRequest])
        SaveCRRequest = Prism.client.factory.create('ns1:SaveCRRequest')
        SaveCRRequest['ChangeRequest'] = CRResponse['ChangeRequest']

        RelatedCRsArray = RelatedCRs.split(',')
        ArrayOfChangeRequestRelationship = Prism.client.factory.create('ns1:ArrayOfChangeRequestRelationship')
        index = 0
        while index < len(RelatedCRsArray):
            ChangeRequestRelationship = Prism.client.factory.create('ns1:ChangeRequestRelationship')
            ChangeRequestRelationship.ChangeRequestId = int(RelatedCRsArray[index])
            index += 1
            ArrayOfChangeRequestRelationship.ChangeRequestRelationship.append(ChangeRequestRelationship)
        SaveCRRequest['ChangeRequest'].RelatedCRs = ArrayOfChangeRequestRelationship

        SaveCRResponse = Prism.client.service.SaveChangeRequest([SaveCRRequest])
        if SaveCRResponse.IsSuccess == False:
            print "ChangeRequestId = %s " % str(SaveCRResponse['ChangeRequestId'])
            print "IsSuccess = %s " % str(SaveCRResponse['IsSuccess'])
            sys.exit(0)

    def move_CRStatus(self,crid,status):
        # Move CR to Analysis
        CRRequest = Prism.client.factory.create('ns1:CRRequest')
        CRRequest['ChangeRequestId'] = crid
        CRResponse = Prism.client.service.GetChangeRequestById([CRRequest])
        SaveCRRequest = Prism.client.factory.create('ns1:SaveCRRequest')
        # SaveCRRequest['UserName'] = options.user
        SaveCRRequest['ChangeRequest'] = CRResponse['ChangeRequest']
        SaveCRRequest['ChangeRequest'].Status = status
        SaveCRRequest['ChangeRequest'].PriorityId = 3
        SaveCRRequest['ChangeRequest'].AnalysisDueDate = datetime.datetime.now() + timedelta(days=1)

        SaveCRResponse = Prism.client.service.SaveChangeRequest([SaveCRRequest])
        if SaveCRResponse.IsSuccess == False:
            print "ChangeRequestId = %s " % str(SaveCRResponse['ChangeRequestId'])
            print "IsSuccess = %s " % str(SaveCRResponse['IsSuccess'])
            sys.exit(0)

    def add_release_notes(self,crid,title, description, category = "NV Item", verification = "None",
                            sideEffects = "None", impacts = "None"):
        # Add release notes
        CRRequest = Prism.client.factory.create('ns1:CRRequest')
        CRRequest['ChangeRequestId'] = SaveCRResponse.ChangeRequestId
        CRResponse = Prism.client.service.GetChangeRequestById([CRRequest])
        SaveCRRequest = Prism.client.factory.create('ns1:SaveCRRequest')
        # SaveCRRequest['UserName'] = options.user
        SaveCRRequest['ChangeRequest'] = CRResponse['ChangeRequest']
        SaveCRRequest['ChangeRequest'].ReleaseNotes.Title = options.Title
        SaveCRRequest['ChangeRequest'].ReleaseNotes.ChangeDescription = options.Description
        SaveCRRequest['ChangeRequest'].ReleaseNotes.ImpactCategory = category
        SaveCRRequest['ChangeRequest'].ReleaseNotes.ChangeVerification = verification
        SaveCRRequest['ChangeRequest'].ReleaseNotes.ChangeSideEffects = sideEffects
        SaveCRRequest['ChangeRequest'].ReleaseNotes.CertificationImpacts = impacts
        SaveCRRequest['ChangeRequest'].ReleaseNotes.UserSpecifiedFiles = True
        # SaveCRRequest['ChangeRequest'].ReleaseNotes.FilesAffected = options.Files

        SaveCRResponse = Prism.client.service.SaveChangeRequest([SaveCRRequest])
        if SaveCRResponse.IsSuccess == False:
            print "ChangeRequestId = %s " % str(SaveCRResponse['ChangeRequestId'])
            print "IsSuccess = %s " % str(SaveCRResponse['IsSuccess'])
            sys.exit(0)

    def review_release_notes(self,crid):
        # Review release notes
        CRRequest = Prism.client.factory.create('ns1:CRRequest')
        CRRequest['ChangeRequestId'] = crid
        CRResponse = Prism.client.service.GetChangeRequestById([CRRequest])
        SaveCRRequest = Prism.client.factory.create('ns1:SaveCRRequest')
        # SaveCRRequest['UserName'] = options.user
        SaveCRRequest['ChangeRequest'] = CRResponse['ChangeRequest']
        SaveCRRequest['ChangeRequest'].ReleaseNotes.ReleaseNotesStatus = 'Review'
        SaveCRResponse = Prism.client.service.SaveChangeRequest([SaveCRRequest])

        print "ChangeRequestId = %s " % str(SaveCRResponse['ChangeRequestId'])
        print "IsSuccess = %s " % str(SaveCRResponse['IsSuccess'])



    def _get_changerequest(self):
        """
        These are the minimum ChangeRequest fields that need to be filled in to create a CR.
        Some fields need to be adjusted for your needs.
        """

        ## Do this to see what the ChangeRequest class looks like
        #cr_info = Prism.client.factory.create("ns1:ChangeRequest")
        #print cr_info


        # You should modify this initial data based on your environment.
        # For instance, SoftwareImageName can go EOL'ed in the future.
        # Or TestTeam, TestOrganization, etc. should be modified according to your configuration.
        # ValidationErrors in the response object should tell you what the problems are.
        cr = {
            "PriorityId" : 2,
            "ChangeRequestType" : "Bug",
            "Title" : "PRISM API Sample Code in Python",
            "Description" : "PRISM API Sample Code in Python",
            "Repeatability" : "NotAttempted",
            "Severity" : "Critical",
            "ReleaseId" : "M8610AAAAAAAZL201005.2",
            "IsCRMReleaseId" : True,
            "SoftwareImageName" : "ADSP.BF.2.0",
            "TargetName" : "NA",
            "HardwarePlatform" : "CUSTOMER",
            "IsCrash" : False,
            "Reporter": {
                "UserName" : Prism.userid,
                "TestOrganization" : "MST",
                "TestTeam" : "IMS",
                "TestArea" : "Austin"
            },
            "TestPhase" : "Development Test",
            "NoFoundOnSoftwareProductDuringCreation" : True,
            "Participants" : {
                "Participant" : [ {
                    "SwFunctionality" : "1X_MUX",
                    "Area" : "Modem",
                    "SubSystem" : "1x",
                    "IsPrimary" : True
                } ]
            }
        }

        ## Another way of generating the Participants array
        #array_of_participants = Prism.client.factory.create("ns1:ArrayOfParticipant")
        #array_of_participants.Participant = [{
        #    "SwFunctionality" : "1X_MUX",
        #    "Area" : "Modem",
        #    "SubSystem" : "1x",
        #    "IsPrimary" : True }]
        #cr["Participants"] = array_of_participants

        ## Do this to see what the Participant members are
        #participant = Prism.client.factory.create("ns1:Participant")
        #print participant

        return cr

    def get_softwareimagerecords_by_crid(self,crid):
        request = Prism.client.factory.create("ns1:GetSoftwareImageRecordsRequest")
        request["ChangeRequestId"] = crid
        request["ApplicationSource"] = Prism.app_source

        response = Prism.client.service.GetSoftwareImageRecords(request)

        return response["SoftwareImageRecords"]


    def edit_softwareimagerecord_priority(self,crid,priority):
        """
        Modifies the first software image record's priority
        """

        sirs = self.get_softwareimagerecords_by_crid(crid)
        sirs.SoftwareImageRecord[0]["Priority"] = priority

        request = Prism.client.factory.create("ns1:SaveSoftwareImageRecordsRequest")
        request["ChangeRequestId"] = crid
        request["ApplicationSource"] = Prism.app_source
        request["SoftwareImageRecords"] = sirs

        response = Prism.client.service.SaveSoftwareImageRecords(request)


    def get_changetasks_by_crid(self, crid):
        """
        Retrieves all the change tasks in the given CR
        """

        request = Prism.client.factory.create("ns1:GetChangeTasksRequest")
        request["ChangeRequestId"] = crid
        request["ApplicationSource"] = Prism.app_source

        response = Prism.client.service.GetChangeTasks(request)

        return response["ChangeTasks"]


    def save_changetasks(self, crid, changetask):
        """
        Saves Change Tasks
        """

        array_of_changetasks = Prism.client.factory.create("ns1:ArrayOfChangeTask")
        array_of_changetasks.ChangeTask = [changetask]

        request = Prism.client.factory.create("ns1:SaveChangeTasksRequest")
        request["ChangeTasks"] = array_of_changetasks
        request["ChangeRequestId"] = crid
        request["ApplicationSource"] = Prism.app_source

        response = Prism.client.service.SaveChangeTasks(request)

        if response.IsSuccess is False:
            if re.search(r"Change Task Name must be unique", response["ValidationErrors"][0][0].ErrorMessage):
                print("Change task already added")
            else:
                raise response["ValidationErrors"]


    def create_changetask(self, taskName):
        image_based_changetask = Prism.client.factory.create("ns1:ComponentChangeTask")
        image_based_changetask["Type"] = "Component"
        image_based_changetask["ChangeTaskName"] = taskName
        image_based_changetask["ReadyDate"] = str(datetime.date.today())
        image_based_changetask["Status"] = "Open"
        image_based_changetask["Assignee"] = Prism.userid

        return image_based_changetask
    def add_changetask(self, crid, taskName):
        """
        Adds a change task to a CR
        """

        image_based_changetask = Prism.client.factory.create("ns1:ComponentChangeTask")
        image_based_changetask["Type"] = "Component"
        image_based_changetask["ChangeTaskName"] = taskName
        # image_based_changetask["Version"] = base64.b64encode("")
        image_based_changetask["ReadyDate"] = str(datetime.date.today())
        image_based_changetask["Status"] = "Open"
        image_based_changetask["Assignee"] = Prism.userid

        # image_based_changetask["SoftwareImageRecords"] = self.get_softwareimagerecords_by_crid(crid)


        self.save_changetasks(crid, image_based_changetask)


    def move_changetask_status(self, crid, status):
        """
        Move Change Task Status to InProgress
        """

        array_of_tasks = self.get_changetasks_by_crid(crid)
        task = array_of_tasks["ChangeTask"][0]

        task["Status"] = status
        task["ReadyDate"] = str(datetime.date.today())

        self.save_changetasks(crid, task)

    def requestApprovals(self, crid):
        CRRequest = Prism.client.factory.create('ns1:CRRequest')
        CRRequest['ChangeRequestId'] = crid
        CRRes = Prism.client.service.GetChangeRequestById([CRRequest])

        SaveCRRequest = Prism.client.factory.create('ns1:SaveCRRequest')
        SaveCRRequest['ChangeRequest'] = CRRes['ChangeRequest']

        for sir in SaveCRRequest['ChangeRequest']['SoftwareImageRecords'][0]:
            FA = sir['FixAllowed']
            if FA == "TBD" or FA == "No":
                print("Setting FixAllowed to Requested Approval for " + sir['SoftwareImageName'])
                sir['FixAllowed'] = "Requested Approval"

        SaveCRResponses = Prism.client.service.SaveChangeRequest([SaveCRRequest])

        if SaveCRResponses.IsSuccess == False:
            print "ChangeRequestId = %s " % str(SaveCRResponses['ChangeRequestId'])
            print "IsSuccess = %s " % str(SaveCRResponses['IsSuccess'])
            print SaveCRResponses





# prism = PrismClient()
# prism.add_changetask(847264)
# cr = prism.get_changerequest_by_id(847264)

# print(cr)
# print("done")