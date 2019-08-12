#!/usr/bin/python

#NAME : mcfgmmcp_sync
#DESCRIPTION : This script helps copying both mcfg & mmcp folders together while creating
#			   view for mcfg component.
#Submission date: Feb 9th, 2016.

import sys
import subprocess
import shutil
import os

#@###############################+ FUNCTION DEFINITIONS  +######################################@#

########################################################################################
#NAME		: cleanExit
#DESCRIPTION: If any error occurred after the view creation then this function will be
#			  called to delete the view before exit.
########################################################################################
def cleanExit(print_str, *var_tuple):
	print "\nError:\n" + print_str,
	for args in var_tuple:
		print args,
	pipe_delete = subprocess.Popen(["vce", "delete", "--view", sys.argv[2]], stdout=subprocess.PIPE)
	print "\nDeleted the view and exiting :(",
	sys.exit(0)

########################################################################################
#NAME		: disp
#DESCRIPTION: If the debug flag is enabled then this function prints the debug msg on
#			  screen.
########################################################################################
def disp(disp_flag, *var_tuple):
	if(disp_flag != 0):
		print "\n",
		for args in var_tuple:
			print args,
	return;

########################################################################################
#NAME		: prefServ
#DESCRIPTION: This function returns 1 for SD server and 2 for QIPL based server.
########################################################################################
def prefServ(input_loc):
	if ((input_loc == "--location=sd") or (input_loc == "--location=SD")):
		return 1; #This will use the SD servers e.g: snowcone#
	elif ((input_loc == "--location=qipl") or (input_loc == "--location=QIPL")):
		return 2; #This will prefer the Indian servers e.g:holi, diwali#
	else:
		print "\nError:\nEntered Wrong option for Location!\nThe option should have any value from this set = {sd, SD, qipl, QIPL}\
		\nNote: Please don't leave any space around \"=\" in the option!\n\t\teg: --location=QIPL",
		sys.exit(0)

########################################################################################
#NAME		: parseArg
#DESCRIPTION: Parsing the command line arguments to set/reset the flags and variables
#			  to a proper value.
########################################################################################
def parseArg(disp_flag, serv_loc, multicr_flag):
	if((len(sys.argv)) > 6):
		print "\nError:\nThe options are limited.  Please type \"--help\" for usage!",
		sys.exit(0)
	elif (len(sys.argv) == 6):
		for i in range(3,6):
			if((sys.argv[i].find("location")) != -1):
				serv_loc = prefServ(sys.argv[i])
			elif((sys.argv[i].find("multicr")) != -1):
				multicr_flag = "--multicrs"
			elif((sys.argv[i].find("debug")) != -1):
				disp_flag = 1
			elif((sys.argv[i].find("help")) != -1):
				print "\nAs the option contains \"--help\" ignoring all other options and calling only the help function",
				printHelp()
				sys.exit(0)
	elif (len(sys.argv) == 5):
		for i in range(3,5):
			if((sys.argv[i].find("location")) != -1):
				serv_loc = prefServ(sys.argv[i])
			elif((sys.argv[i].find("multicr")) != -1):
				multicr_flag = "--multicrs"
			elif((sys.argv[i].find("debug")) != -1):
				disp_flag = 1
			elif((sys.argv[i].find("help")) != -1):
				print "\nAs the option contains \"--help\" ignoring all other options and calling only the help function",
				printHelp()
				sys.exit(0)
	elif (len(sys.argv) == 4):
		if((sys.argv[3].find("location")) != -1):
			serv_loc = prefServ(sys.argv[3])
		elif((sys.argv[3].find("multicrs")) != -1):
			multicr_flag = "--multicrs"
		elif((sys.argv[3].find("debug")) != -1):
			disp_flag = 1
		elif((sys.argv[3].find("help")) != -1):
				print "\nAs the option contains \"--help\" ignoring all other options and calling only the help function",
				printHelp()
				sys.exit(0)
	return disp_flag, serv_loc, multicr_flag

########################################################################################
#NAME		: printHelp
#DESCRIPTION: Print usage and options of this script on screen.
########################################################################################
def printHelp():
	print "\nMCFGMMCP_SYNC, Version 2.0",
	print "\nUsage: python mcfgmmcp_sync.py <component_name> <view_name> [options]",
	print "\nOPTIONS:-",
	print "\n\t--help\n\t\tGives the list of options & help(No need of <component_name> & <view_name> for this option)",
	print "\n\t--location=SD|QIPL\n\t\tTo prefer QIPL/SD location(Required <component_name> & <view_name>)",
	print "\n\t--multicrs\n\t\tTo enable the VCE++ mode for the view(Required <component_name> & <view_name>)",
	print "\n\t--debug\n\t\tTo print some debugging messages while running the script(Required <component_name> & <view_name>)",
	print "\n\n",

########################################################################################
#NAME		: validateArgs
#DESCRIPTION: Validating the entered arguments before processing then command line
#			  Arguments.
########################################################################################
def validateArgs():
	if (len(sys.argv) <= 2):
		if(sys.argv[1] == "--help"):
			printHelp()
		else:
			print "\nError:\nEntered inputs are not sufficient. Run with \"--help\" for usage!",
		sys.exit(0)
	if((sys.argv[1].find("mcfg.mpss")) == -1):
		print "\nError:\nThe first arguments should be the component name. Please check the arguments entered!\
			\nNote: This Script currently works only for mcfg.mpss components!",
		sys.exit(0)
	if((sys.argv[2].find("--")) != -1):
		print "\nError:\nThe second argument should be the name of your view. Please check the arguments entered!",
		sys.exit(0)

########################################################################################
#NAME		: findPL
#DESCRIPTION: Find the PL name from given string and return the same.
########################################################################################
def findPL(variants_output):
	index = variants_output.find("lnx_pro")
	if(index == -1):
		cleanExit("There is no lnx_pro variants for the given component", sys.argv[1])
	index1 = variants_output.find("CRM Reference Base => ", index)
	index1 += (len("CRM Reference Base => "))
	index = variants_output.find("MPSS", index1, index1+5)
	if(index == -1):
		cleanExit("There is no PLs listed for this component's lnx_pro variant")
	index = variants_output.find("-", index1)
	return variants_output[index1:index];

########################################################################################
#NAME		: selectServ
#DESCRIPTION: It selects the user preferred server(QIPL/SD) from the list of available
#			  servers.  It will select any server based on availability when there
#			  is no user preferred server location available.
########################################################################################
def selectServ(disp_flag, serv_loc, ntwrk_list):
	if(serv_loc == 2):
		for arg in ntwrk_list:
			if((arg.find("diwali") != -1) or (arg.find("holi") != -1)):
				disp(disp_flag, "Selected QIPL server!")
				return arg;
		disp(disp_flag, "As there is no QIPL server available selecting any available server!")
		return ntwrk_list[0]
	else:
		for arg in ntwrk_list:
			if(arg.find("snowcone") != -1):
				disp(disp_flag, "Selected SD server!")
				return arg;
		disp(disp_flag, "As there is no SD server available selecting any available server!")
		return ntwrk_list[0]

########################################################################################
#NAME		: findNetPath
#DESCRIPTION: Lists the available server from the findbuild output and selects any
#			  server based on the availability and user preference.
########################################################################################
def findNetPath(disp_flag, serv_loc, findbuild_output):
	ntwrk_list = []
	index = findbuild_output.find("Location:       ", 0)
	while (index  != -1):
		index += (len("Location:       "))
		index1 = findbuild_output.find("No Location", index, (index + len("No Location")))
		if(index1 != -1):
			index = findbuild_output.find("Location:       ", index)
			continue
		index1 = findbuild_output.find("\n", index)
		index1 -= 1
		if (findbuild_output.find("\\crm-ubuntu", index, index1) == -1):
			ntwrk_list.append(findbuild_output[index:index1])
		index = findbuild_output.find("Location:       ", index)
	if len(ntwrk_list) == 0:
		cleanExit("Currently no network path is listed for this given variant of the component", sys.argv[1], "!")
	return selectServ(disp_flag, serv_loc, ntwrk_list);

#@########################################+ FUNCTION DEFINITIONS END +#####################################@#


disp_flag = 0 #by default debugging flag will be false#
serv_loc = 1 #by default server location will be SD#
multicr_flag = None #by default multicr_flag will be empty#

validateArgs()#Validating the Command line arguments#

#Parsing the command line arguments#
disp_flag, serv_loc, multicr_flag = parseArg(disp_flag, serv_loc, multicr_flag)
disp(disp_flag, "disp_flag = ", disp_flag)
disp(disp_flag, "multicr_flag = ", multicr_flag)

if(serv_loc == 1):
	disp(disp_flag, "serv_loc = SD")
else:
	disp(disp_flag, "serv_loc = QIPL")

#A log file to write the simple logs about the process#
logfile = open("mcfgmmcp_log.txt", 'w')

print "\nCreating view...",
if (multicr_flag != None):
	print "\nVCE++(--multicrs) enabled in this view!\n",
	pipe_view = subprocess.Popen(["vce", "view", "--base", sys.argv[1], "--checkout", sys.argv[2], multicr_flag], stdout=subprocess.PIPE)
	logfile.write("vce view --base " + sys.argv[1] + " --checkout " + sys.argv[2] + " " + multicr_flag + "\n")
else:
	print "\nVCE++(--multicrs) not enabled in this view!\n",
	pipe_view = subprocess.Popen(["vce", "view", "--base", sys.argv[1], "--checkout", sys.argv[2]], stdout=subprocess.PIPE)
	logfile.write("vce view --base " + sys.argv[1] + " --checkout " + sys.argv[2] + "\n")

output_view = pipe_view.stdout.read()
logfile.write(output_view)

if((output_view.find("Successfully synced")) == -1):
	print "\nError:\nProblem with the view creation.  Check whether the given component name exist in PW or not!",
	logfile.close()
	sys.exit(0)
else:
	print "\nSuccessfully created view with the name " + sys.argv[1] + "." + sys.argv[2],

print "\nCopying MMCP folder...",
disp(disp_flag, "Executing the vce variants to get the CRM Reference Base & PL name...")
pipe_variants = subprocess.Popen(["vce", "variants", "--base", sys.argv[1]], stdout=subprocess.PIPE)
variants_output = pipe_variants.stdout.read()
logfile.write("vce variants --base " + sys.argv[1] + "\n")
logfile.write(variants_output)
pl = findPL(variants_output)
disp(disp_flag, "\nThe PL is : ", pl)

disp(disp_flag, "Triggering findbuild -pl to get the network path")

logfile.write("findbuild -pl " + pl + "\n")
pipe_findbuild = subprocess.Popen(["findbuild", "-pl", pl], stdout=subprocess.PIPE)

findbuild_output = pipe_findbuild.stdout.read()
logfile.write(findbuild_output)

ntwrk_path = findNetPath(disp_flag, serv_loc, findbuild_output)

disp(disp_flag, "Copying the MMCP folder from the Location: " + ntwrk_path + "\modem_proc\mmcp\policyman\configurations.")
print "\nPlease be patient till it copies the MMCP file!!!",
current_dir = os.path.dirname(os.path.abspath(__file__))
shutil.copytree(ntwrk_path+r'\modem_proc\mmcp\policyman\configurations', current_dir + "\\" + sys.argv[1] + "." + sys.argv[2] + r'\modem_proc\mmcp\policyman\configurations')
disp(disp_flag, "View created successfully! :)")

#Open the folder at last#
subprocess.Popen(["explorer", current_dir + "\\" + sys.argv[1] + "." + sys.argv[2] + "\\modem_proc"])
