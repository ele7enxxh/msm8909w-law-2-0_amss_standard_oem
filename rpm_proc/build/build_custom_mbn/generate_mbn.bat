@echo off
REM *=======================================================================================================================================================


REM DESCRIPTION: patch file to create mbn file from elf and then signing the mbn
REM ========================================================================================================================================================*/



REM *=======================================================================================================================================================
REM GENERATE_MBN contains the current path.
REM LOAD_ADDR contains the image destination address
REM ========================================================================================================================================================*/


SET ELF_PATH=%2
SET GENERATE_MBN=%~dp0
SET LOAD_ADDR=0xFC100080

REM --------------------------------------------------------------------------------------------------------------------------------------------------------




REM *=======================================================================================================================================================
REM Main function is used to call different blocks depending on the input from user.
REM If user enters '-c' <$elf path> command, clear_generate_mbn block is executed.
REM If user enters '-h' command, help menu is displayed.
REM If user enters generate_mbn.bat elf <$elf path>, generate_mbn_all block is executed.   

REM ========================================================================================================================================================*/




:Main

	If "%1" == "-c"  					( call :clear_generate_mbn 
	) else If "%1" == "-h"  			( call :help_menu
	) else If "%1" == "elf"			    ( call :generate_mbn_all
	) else call :error_handler 							

goto :eof


REM --------------------------------------------------------------------------------------------------------------------------------------------------------








REM *=======================================================================================================================================================
REM error_handler function is used to print error_handler message for user on wrong usage of the generate_mbn command.
REM It also displays the help menu for reference of the user.
   
REM ========================================================================================================================================================*/

:error_handler
	
	echo Wrong usage of generate_mbn. 
	echo Refer to the help menu given below for reference.
	call :help_menu

goto :eof	


REM --------------------------------------------------------------------------------------------------------------------------------------------------------



REM *=======================================================================================================================================================
REM clear_generate_mbn block clears the generate_mbn. 
REM ========================================================================================================================================================*/


:clear_generate_mbn

	IF EXIST RPM*.* del /q RPM*.* >NUL
	IF EXIST %ELF_PATH%\custom_bin rmdir /s /q %ELF_PATH%\custom_bin >NUL
	echo cleaning is done.
	
goto :eof	
	
	
REM --------------------------------------------------------------------------------------------------------------------------------------------------------	




REM *=======================================================================================================================================================
REM help_menu block displays the correct usage of the generate_mbn command
REM ========================================================================================================================================================*/



:help_menu


	REM =========  Usage ======================================================================================================================================
	@ECHO.
	@ECHO =============== USAGE : generate_mbn.bat  ==================================================================================================================
	@ECHO # This script compiles rpm binaries for all the B family targets  

	@ECHO generate_mbn.bat  -h                        													# Showing this help menu
	@ECHO generate_mbn.bat  elf <Complete path to elf file >   						                    # Compiling properly
	@ECHO generate_mbn.bat  -c  <Complete path to elf file > 											# clean generate_mbn auto-generated files.

	@ECHO =======================================================================================================================================================

goto :eof


REM --------------------------------------------------------------------------------------------------------------------------------------------------------






REM *=======================================================================================================================================================
REM generate_mbn_all block generates the signed .mbn file.
REM ========================================================================================================================================================*/



:generate_mbn_all

	REM *=======================================================================================================================================================
	REM Creation of mbn from elf file.

	REM custom_bin contains unsigned directory which contains unsigned .mbn file.
	REM custom_bin itself contains signed .mbn file.

	REM ========================================================================================================================================================*/
    
	
	echo creating custom_bin directory
	cd %ELF_PATH%
	IF NOT EXIST custom_bin mkdir custom_bin

	echo changing current directory to custom_bin
	cd custom_bin

	echo creating unsigned_mbn directory
	IF NOT EXIST unsigned_mbn mkdir unsigned_mbn

	echo creation of mbn 
	echo changing current directory to unsigned_mbn 
	cd unsigned_mbn 
    	
	python %GENERATE_MBN%\generate_mbn.py %ELF_PATH%\RPM_AAAAANAAR_rpm.elf RPM.mbn %LOAD_ADDR% elf 
	echo returning to generate_mbn
	cd %GENERATE_MBN%
	
	
	REM *=======================================================================================================================================================
	REM Signing of .mbn file.

	REM SECTOOLS_PATH contains the path till sectools. 
	REM signed .mbn is created in secimage_output from where it is copied to custom_bin.

	REM <python_script> secimage -i <source_mbn_file> -g <module_name> -c <($path)secimage.xml> -sa
	REM ========================================================================================================================================================*/

	echo signing of mbn file 
	SET SECTOOLS_PATH=%GENERATE_MBN%\..\..\tools\build\scons\sectools

	python %SECTOOLS_PATH%\sectools.py secimage -i %ELF_PATH%\custom_bin\unsigned_mbn\RPM.mbn -g rpm -c %SECTOOLS_PATH%\config\integration\secimage.xml -sa 
	REM delete IF .mbn file with same name exists to avoid duplication.
	IF EXIST "%ELF_PATH%\custom_bin\RPM.mbn" DEL "%ELF_PATH%\custom_bin\RPM.mbn"

	REM copying signed mbn to generate_mbn
	IF EXIST "%SECTOOLS_PATH%\secimage_output\default\rpm\RPM.mbn" COPY "%SECTOOLS_PATH%\secimage_output\default\rpm\RPM.mbn" "%ELF_PATH%\custom_bin\RPM.mbn" >NUL
	echo Generated mbn is also stored at %ELF_PATH%\custom_bin\RPM.mbn
	

goto :eof




