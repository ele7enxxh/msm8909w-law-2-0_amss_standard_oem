# this is running in a new process, so the device will need to be opened again
# global variables are in a new scope

# http://ss64.com/ps/
# http://technet.microsoft.com/en-us/library/bb978526.aspx
# http://technet.microsoft.com/en-us/library/ee176880.aspx
# http://ss64.com/ps/syntax-regex.html
# https://groups.google.com/forum/#!forum/powershell-users
# http://blogs.technet.com/b/heyscriptingguy/archive/2010/02/11/hey-scripting-guy-february-11-2010.aspx
# http://rkeithhill.wordpress.com/2007/11/24/effective-powershell-item-10-understanding-powershell-parsing-modes/
# http://www.powershellmagazine.com/2013/08/19/mastering-everyday-xml-tasks-in-powershell/

param (
#  [parameter(Mandatory=$true)][string]$Test,            # 
#  [switch]$test1
)

$Date = "06-Nov-2014"

$WorkingDir = Split-Path $MyInvocation.MyCommand.Path -Parent
Write-Host "WorkingDir: $WorkingDir"
$Transcript = "$WorkingDir\qcMacchiatoWpTest.log"
Write-Host "Transcript: $Transcript"

. $WorkingDir\Utils.ps1

#$FileListName = "FileList.txt"
$challengeLocalFile = ""
$challengeOnDeviceFile = ""
$messageLocalFile = ""
$messageOnDeviceFile = ""
$keywraptLocalFile = ""
$keywrapOnDeviceFile = ""
$unwrappedKeyFile = "c:\test\unwrapped_key.macc"
$svc_data = "c:\test\svc_data.macc"
$signed_svc_data = "c:\test\signed_svc_data.macc"
$qcmacapptest = "qcMacchiatoWpTest.exe"
$macchiato_mbn = "macchiato_sample.mbn"
$qcScmmacTest_UM_log = "qcScmMacTest_UM.log"
$deviceIdFile = "deviceId.txt"


function OpenDeviceIdFromFile ([string]$deviceIdFile) {
    $GUID = Get-Content $deviceIdFile
	writeInfo "deviceId: $GUID"
    MyOpen-Device $GUID
}

function ReadInputFileList ([string]$FileListName) {
    $content = Get-Content $FileListName
    $i = 0
    foreach ($line in $content) {
        $i++
        if ($i -eq 1) {$Global:challengeLocalFile = $line}
        if ($i -eq 2) {$Global:challengeOnDeviceFile = $line}
        if ($i -eq 3) {$Global:messageLocalFile = $line}
        if ($i -eq 4) {$Global:messageOnDeviceFile = $line}
        if ($i -eq 5) {$Global:keywrapLocalFile = $line}
        if ($i -eq 6) {$Global:keywrapOnDeviceFile = $line}
    }
}


function AuthTest ([string]$FileListName) {
	Start-Transcript -path $Transcript -Append
    writeInfo "AuthTest: Enter"
    writeInfo "FileListName: $FileListName"

    ReadInputFileList($FileListName)

	WriteCRLF
    writeInfo "Files used:"
	writeInfo "service data created on device and copied to host: $svc_data"
	writeInfo "signed service data copied from device to host: $signed_svc_data"
	
    Exec-Device $qcmacapptest 'sign'

    Get-Device *.log
	Get-Device $svc_data
	Get-Device $signed_svc_data
    writeInfo "AuthTest: Exit"
    writeInfo "qcMacchiatoWpTest.ps1: Exit"
    Stop-Transcript
}

function ChallengeTest ([string]$FileListName) {
	Start-Transcript -path $Transcript -Append
    writeInfo "ChallengeTest: Enter"
    writeInfo "FileListName: $FileListName"

    ReadInputFileList($FileListName)

	WriteCRLF
    writeInfo "Files used:"
    writeInfo "challengeLocalFile: $challengeLocalFile"
	writeInfo "copied to device as"
    writeInfo "challengeOnDeviceFile: $challengeOnDeviceFile"
	WriteCRLF
    writeInfo "messageOnDeviceFile: $messageOnDeviceFile"
	writeInfo "copied to host as"
    writeInfo "messageLocalFile: $messageLocalFile"
#	writeInfo "and" 
#    writeInfo "messageOnDeviceFile: $messageOnDeviceFile"

	WriteCRLF
    WriteInfo "Put-Device $challengeLocalFile $challengeOnDeviceFile"
    Put-Device $challengeLocalFile $challengeOnDeviceFile

    Exec-Device $qcmacapptest 'challenge'

    Get-Device *.log
#    Get-Device $challengeOnDeviceFile
#    Get-Device $messageOnDeviceFile
    Get-Device $messageOnDeviceFile $messageLocalFile
    writeInfo "ChallengeTest: Exit"
    writeInfo "qcMacchiatoWpTest.ps1: Exit"
    Stop-Transcript
}

function ProvisionKeyTest ([string]$FileListName) {
	Start-Transcript -path $Transcript -Append

    writeInfo "ProvisionKeyTest: Enter"
    writeInfo "FileListName: $FileListName"

    ReadInputFileList($FileListName)

	writeCRLF
    writeInfo "Files used:"
    writeInfo "keywrapLocalFile: $keywrapLocalFile"
	writeInfo "copied to device as"
    writeInfo "keywrapOnDeviceFile: $keywrapOnDeviceFile"
	writeCRLF
    writeInfo "unwrappedKeyFile: $unwrappedKeyFile"
	writeInfo "copied from device as same"
	
	writeCRLF
    writeInfo "Put-Device $keywrapLocalFile $keywrapOnDeviceFile"
    Put-Device $keywrapLocalFile $keywrapOnDeviceFile

    Exec-Device $qcmacapptest 'provision'

    Get-Device *.log
	Get-Device $unwrappedKeyFile
    writeInfo "ProvisionKeyTest: Exit"
    writeInfo "qcMacchiatoWpTest.ps1: Exit"
    Stop-Transcript
}

function StartTest ([string]$FileListName) {
	Start-Transcript -path $Transcript
	writeInfo "qcMacchiatoWpTest.ps1:StartTest: Enter"
	writeInfo "Date: $Date"
	writeInfo "WorkingDir: $WorkingDir"

    Del-Device $qcScmmacTest_UM_log
    Del-Device *.macc

	WriteCRLF
    WriteInfo "Put-Device $qcmacapptest"
    Put-Device $qcmacapptest

	WriteCRLF
    WriteInfo "Put-Device $macchiato_mbn"
    Put-Device $macchiato_mbn

    Exec-Device $qcmacapptest 'start'

    writeInfo "StartTest: Exit"
    writeInfo "qcMacchiatoWpTest.ps1: Exit"
    Stop-Transcript
}

function Close ([string]$FileListName) {
	Start-Transcript -path $Transcript -Append
    writeInfo "Close: Enter"

    Exec-Device $qcmacapptest 'unload'

    writeInfo "Close: Exit"
    writeInfo "qcMacchiatoWpTest.ps1: Exit"
    Stop-Transcript
}

#GetDevices
OpenDeviceIdFromFile($deviceIdFile)
CD-Device c:\test

