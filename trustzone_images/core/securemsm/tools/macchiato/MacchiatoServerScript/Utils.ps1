# these functions add cr so the transcripts display correctly in notepad
function writeError ($str) {
  Write-Host "ERROR: $str`r" -ForegroundColor Red -BackgroundColor White
}

function writeWarning ($str) {
  Write-Host "WARNING: $str`r" -ForegroundColor Blue -BackgroundColor White
}

function writeInfo ($str) {
  Write-Host "$str`r" -ForegroundColor Yellow
}

function writeCRLF {
  Write-Host "`r"
}

function PsVersion() {
  $PSVersionTable.PSVersion
}

function ErrorBeep() {
#  [console]::beep(1000,2000)
}

function MatchBeep() {
#  [console]::beep(3000,100)
}

function DisplayDevices ($GUIDS) {
  foreach ($g in $GUIDS) {
    writeInfo $g
  }
}

function GetKeyInput {
  while ([console]::KeyAvailable -eq $false) {}
  key = [console]::ReadKey("IncludeKeyUp,NoEcho")
  return $key
}

function WaitForKey {
  If ($psISE) {
    pause
  }
  else {
    while ([console]::KeyAvailable -eq $false) {}
    [console]::ReadKey("IncludeKeyUp,NoEcho")
  }
}

function CheckForKey {
  If ($psISE) {
    Start-Sleep -Seconds 10
    return $true
  }
  else {
    if ([console]::KeyAvailable) {
      [console]::ReadKey("IncludeKeyUp,NoEcho")
      return $true
    } else {
      return $false
    }
  }
}

function FindAttachedDevices([string]$mac) {
  # Kill any running versions of virteth because we want all the output
  $result = stop-process -Name "VirtEth" -EA SilentlyContinue
  start-sleep 1
  del c:\temp\virteth.log
  
  # VirtEth will keep runing in the background and log to the virteth.log file
  $virteth = start-process "C:\\Program Files (x86)\\Microsoft Windows Phone 8 KDBG Connectivity\\bin\\VirtEth.exe" -PassThru -ArgumentList "/autodebug /logfile c:\temp\virteth.log" # -WindowStyle Hidden
  start-sleep -seconds 5
  $myMacs = @()
  $lines = Get-Content "c:\temp\virteth.log" -Encoding Unicode
  foreach ($line in $lines ) {
    if ($line.Contains("NIC UP")) {
      $mac = $line.Substring($line.IndexOf("NIC UP")+8,17)
      # If the mac address is not in current list then add it
      if ($myMacs -notcontains $mac) {
        writeInfo $mac
        $myMacs = $myMacs + $mac
      }
    }
  }
  start-sleep -milliseconds 500

  # return the list of macs
  $myMacs
}

function GetAttachedDevices {
writeCRLF
writeInfo "Wait... looking for connected devices"
writeCRLF

$GUIDS = @()
$GUIDS = FindAttachedDevices 
if ($GUIDS -eq $null) {
  writeError "No devices found, exiting"
  exit
}

writeCRLF
writeInfo "The following devices were found:"
DisplayDevices($GUIDS)
writeCRLF

$GUIDS

}


function MyOpen-Device($GUID) {
  $tlist = $null
  $trycnt = 0
  $status = $true
  while ($tlist -eq $null) {
    Open-Device $GUID
    $tlist = TList-Device
    $trycnt++
    if ($trycnt -gt 5) {
      $status = $false
      break;
    }
  }
  $status
}

function OpenAnyDevice($GUIDS) {
  $status = $false
  foreach ($GUID in $GUIDS) {
    $status = MyOpen-Device $GUID
    if ($status -eq $true) {
      break
    }
  }
  $status
}

function GetHostIpAddr {
  $PcIpAddr = Get-WmiObject Win32_NetworkAdapterConfiguration | Where {$_.DNSDomain -eq "qualcomm.com"} | Where { $_.IPAddress } | Select -Expand IPAddress | Where { $_ -like '*.*.*.*' }
  return $PcIpAddr
}

function MyPause ($pause) {
  if ($pause) {
    pause
  } else {
    Start-Sleep -Seconds 5
  }
}

function SelectDevices ($GUIDS) {
$SelectedGUIDS = $GUIDS
while ($true -and $query) {
  DisplayDevices($SelectedGUIDS)
  $i = Read-Host "enter device GUID as shown to remove it from this setup - <enter> when done, 'r' to start over"
  if ($i -eq 'r') {
    $SelectedGUIDS = $GUIDS
    continue
  } elseif ($i -eq '') {
    break
  }
  $SelectedGUIDS = $SelectedGUIDS -ne $i
  if (-not $SelectedGUIDS) {
    $SelectedGUIDS = $GUIDS
    writeWarning "not allowed to remove all devices"
  }
}
return $SelectedGUIDS
# return variables are very dangerous in powershell since it's just everything on the stack
# make sure everything is captured into variables and nothing else is on this function stack
}

function RegGetREGSZ ($q) {
  $str = "REG_SZ"
  $pos = $q.IndexOf($str)
  $rightPart = $q.Substring($pos+$str.Length)
  $rightPart = $rightPart -replace " ",''
  $rightPart = $rightPart -replace "`r",''
  $rightPart = $rightPart -replace "`n",''
  $rightPart = $rightPart -replace "`t",''
  return $rightPart
}


function MtbfIsRunning {
  $proc_list = tlist-device
  if ($proc_list.ToLower().Contains("mtbfclient.exe")) {
    return $true
  } else {
    return $false
  }
}

function RebootDevice($DeviceIdentifier, [switch] $DoNotSkipFfuModeOnReboot = $false) {
  writeInfo "Rebooting device."
  if (-not $DoNotSkipFfuModeOnReboot) {
    writeInfo "Disable ffu mode for next boot."
    cmdd "bcdedit.exe /set {globalsettings} custom:26000202 yes"
  }

  try
    {        
    cmdd "minshutdown.exe -r"
    }
  catch [Microsoft.WindowsMobile.MIT.Services.DeviceConnectionException]
    {}
  writeInfo "Waiting for device to reboot and reconnect."
  MyOpen-Device $DeviceIdentifier
  cmdd "WaitForShellReady.exe"
}