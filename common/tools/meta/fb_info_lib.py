#!/usr/bin/env python

'''
Name:
   fb_info_lib.py

   Get findbuild info of a build

Description:
   This script takes in a Build ID as input and returns 
   the full findbuild information as a dictionary.

'''

import os, sys, shlex, subprocess


class fbInfo:

   #==============================================================================
   #  Constructor
   #==============================================================================
   def __init__(self, on_linux):
      self.fbPath = None
      self.on_linux = on_linux
   
   
   #==============================================================================
   #  get_fbInfo(buildID)
   #
   #   Input:
   #      buildID: Required build ID to search
   #   Output:
   #      fbInfo: Dictionary with information from findbuild search
   #==============================================================================
   def get_fbInfo(self, buildID, throw_ex=True):
      '''
      C:\Users\c_bandar>findbuild M8974AAAAANAAM002127.2 -lo
      Build:          M8974AAAAANAAM002127.2
      Location:       \\fosters\builds435\INTEGRATION\M8974AAAAANAAM002127.2
      Comments:       Build completed, labeled, and marked for retention.
      Status:         Approved  [Approved for Testing]
      BuildDate:      08/09/2013 16:25:19
      Type:           INTEGRATION
      ProductLine:    MPSS.DI.1.0 OSVer:  HWPlatform: MSM8974
      MainMake:       modem_proc/build/ms/build.sh 8974.gps.test 8974.gen.test BUILD_VER=002127 -k
      OSFilerPath:    \\fosters\builds435\INTEGRATION\M8974AAAAANAAM002127.2
      LinuxPath:      /prj/qct/asw/crmbuilds/fosters/builds435/INTEGRATION/M8974AAAAANAAM002127.2
      Compiler:       QDSP6gcc
      PLF:            target_main.plf
      PLFP4Location:  //source/qcom/qct/mpss/integration/Dime/1.0/main/latest/target_main.plf#3043
      Submitter:      qctecmdr
      Approver:       kbevara
      Archived:       False
      Shippable:      True
      IsSbaStrict:    False
      SiteLocation:   SanDiego
      '''
      
      # GET FINDBUILD OUTPUT
      #---------------------
      fb_output = ""
      
      # If it's first time, run the subroutine to set findbuild path
      if not self.fbPath:
         fb_output = self.__set_fbPath(self.on_linux, buildID)
         # If still path is not found abort
         if not self.fbPath and throw_ex:
            raise Exception("Findbuild location to use was not initialized")
      
      # If it's not the first time, findbuild path is already set. Call directly.
      else:
         fb_output = runCmd([self.fbPath, buildID, "-lo"])
      
      
      # PROCESS FINDBUILD OUTPUT
      #-------------------------
      fbInfo = {}
      print "Findbuild returned:\n" + fb_output    #Debug message
      
      # Check if build id is valid
      if "No Record Found!" in fb_output:
         return None
         
      fb_output = fb_output.split("\n")
      
      # Parse the output and split it into key value pairs
      for each_line in fb_output:
         if "MainMake:" in each_line:
            each_line = each_line.split(': ',1)
            fbInfo[each_line[0].strip()] = each_line[-1].strip()
         else:
            each_line = each_line.split()
            new_line = []
            for word in each_line:
               if ':' in word or len(new_line) == 0:
                  new_line.append(word)
               else:
                  new_line[-1] = new_line[-1] + " " + word
            for each_pair in new_line:
               each_pair = each_pair.split(':',1)
               fbInfo[each_pair[0].strip()] = each_pair[-1].strip()
          
      return fbInfo
   
   
   #==============================================================================
   #  __set_fbPath(on_linux, buildID)
   #      Private function to set findubild path first time
   #
   #   Input:
   #      on_linux: Platform
   #   Output:
   #      findbuild output
   #==============================================================================
   def __set_fbPath(self, on_linux, buildID):
      try:
         print("\nfb_info_lib: Calling FindBuild from local environment")  #Debug Statement - Print FindBuild Command
         self.fbPath = "Findbuild"
         stuff = runCmd([self.fbPath, buildID, "-lo"])
         #print("FindBuild returned: " + ' '.join(stuff)+"\n")  #Debug Statement - Print FindBuild Output
         if len(stuff) == 0 and throw_ex:
            raise Exception("Findbuild returned empty on stdout")
      except:
         print("fb_info_lib: Could not use FindBuild from environment.")
         if on_linux:
            print("fb_info_lib: Trying /prj/qct/asw/qctss/linux/bin/FindBuild") #Debug Statement - Print FindBuild Command
            self.fbPath = "/prj/qct/asw/qctss/linux/bin/FindBuild"
         else:
            print("fb_info_lib: Trying \\\\stone\\aswcrm\\smart\\nt\\bin\\FindBuild") #Debug Statement - Print FindBuild Command
            self.fbPath = "\\\\stone\\aswcrm\\smart\\nt\\bin\\FindBuild"
         try:
            stuff = runCmd([self.fbPath, buildID, "-lo"])
            #print("FindBuild returned: " + ' '.join(stuff)+"\n") #Debug Statement - Print FindBuild Output
         except:
            print("FindBuild could not be found.")
            self.fbPath = None
      
      return stuff


#==============================================================================
#  runCmd function
#
#   Input:
#      command: String of command that needs to be entered in the command prompt
#   Output:
#      cmd_out: The result of running the command
#==============================================================================
def runCmd(command):   
   # print "Executing:", command, "..."
   # Call process
   if type(command) is not list:
      command = shlex.split(command);
   subproc = subprocess.Popen(command,stdout=subprocess.PIPE)
   stdout, stderr = subproc.communicate()
   if not stderr:
      return stdout
   else:
      print command + " failed! : " + stderr
      return None
