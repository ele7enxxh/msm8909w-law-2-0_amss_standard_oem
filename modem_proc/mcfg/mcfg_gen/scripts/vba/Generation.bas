Attribute VB_Name = "Generation"
'force declaration of every variable
Option Explicit

'*****************************************************
'Sub/Func  : GetCfgType
'          :
'Purpose   : Determine whether cfg type is hw or sw
'          :
'Arguments : None
'          :
'Returns   : String
'          :
'Comments  : String is of the form "hw" or "sw"
'*****************************************************
Function GetCfgType()
  GetCfgType = Mid(ActiveWorkbook.Name, 6, 2)
  GetCfgType = LCase(GetCfgType)
End Function

Function GetFolder(strPath As String) As String
    Dim fldr As FileDialog
    Dim sItem As String
    Set fldr = Application.FileDialog(msoFileDialogFolderPicker)
    With fldr
        .Title = "Select a Folder"
        .AllowMultiSelect = False
        .InitialFileName = strPath
        If .Show <> -1 Then GoTo NextCode
        sItem = .SelectedItems(1)
    End With
NextCode:
    GetFolder = sItem
    Set fldr = Nothing
End Function

Sub Browse()
    Dim MyPath As String
    Dim MyFolder As String
    Dim Folder As String
    MyPath = Application.ActiveWorkbook.FullName
    MyFolder = Left(MyPath, InStrRev(MyPath, "\"))
    
    
    Folder = GetFolder(MyFolder)
    
    ActiveWorkbook.Sheets.Item("Summary").buildRootTextBox.Text = Folder
End Sub

'Get the directory length but do not include the trailing separator, if any'
Function GetDirLen(Path As String)
   Dim PathLen As String
   
   If (Right(Path, 1) = "\" Or Right(Path, 1) = "/") Then
     PathLen = Len(Path) - 1
   Else
     PathLen = Len(Path)
   End If
   
   GetDirLen = PathLen
End Function

'Returns the base directory name'
Function GetBaseDir(Path As String)
   Dim BasePathLen As String
   Dim FullBasePath As String
   Dim BaseDir As String
         
   'Gets directory length, not including trailing directory separator if present'
   BasePathLen = GetDirLen(Path)
   'Isolate the full base directory path from what was passed in'
   FullBasePath = Left(Path, BasePathLen)
   'Isolate the base directory from the full base directory path'
   BaseDir = Right(FullBasePath, BasePathLen - InStrRev(FullBasePath, "\"))
   
   GetBaseDir = BaseDir
End Function

'Removes the base directory from the path'
Function RemBaseDirFromPath(BasePath As String)
  Dim BasePathLen As String
  Dim FullBasePath As String
  Dim ParentDirPath As String

  'Gets directory length, not including trailing directory separator if present'
  BasePathLen = GetDirLen(BasePath)
  'Isolate the full base directory path from what was passed in'
  FullBasePath = Left(BasePath, BasePathLen)
  'Include only the path up to the last "\" character'
  ParentDirPath = Left(FullBasePath, InStrRev(FullBasePath, "\"))
  
  RemBaseDirFromPath = ParentDirPath
End Function

'Report the PL and chipset on the Summary page. Added to Open event of ThisWorkbook object'
Sub InitializeVariantSummary()
    Dim MyPath As String
    Dim MyFolder As String
    Dim Path As String
    Dim ParentDir As String
    Dim GrandParentDir As String
    Dim GrandParentDir_2 As String
    Dim MCFGGenDir As String
    
    MyPath = Application.ActiveWorkbook.FullName
    MyFolder = Left(MyPath, InStrRev(MyPath, "\"))
    
    'Source and Generated Binaries will now be stored in a "baseline\chipset\" subfolder'
    Path = MyFolder
    ParentDir = GetBaseDir(Path)      'Parent dir will indicate the carrier these configs are for'
    Path = RemBaseDirFromPath(Path)   'Traverse up one directory in path string'
    GrandParentDir = GetBaseDir(Path) 'Grandparent dir will indicate the sw-product these configs are for'
    Path = RemBaseDirFromPath(Path)   'Traverse up one directory in path string'
    GrandParentDir_2 = GetBaseDir(Path) 'Grandparent_2 dir will indicate the image these configs are for'
    Path = RemBaseDirFromPath(Path)   'Traverse up one directory in path string'
    MCFGGenDir = GetBaseDir(Path)
    
    If UCase(MCFGGenDir) = "MCFG_GEN" Then
       Worksheets("Summary").Range("B7").Value = UCase(GrandParentDir_2)
       Worksheets("Summary").Range("B8").Value = UCase(GrandParentDir)
    Else
       Worksheets("Summary").Range("B7").Value = "Undefined: Move worksheet to proper location"
       Worksheets("Summary").Range("B8").Value = "Undefined: Move worksheet to proper location"
    End If
    
    'To suppress prompt to save workbook before generating scripts/mbns. This prompt appears now that InitializeVariantSummary runs at startup'
    ThisWorkbook.Saved = True

    'ActiveWorkbook.Saved = True'
    
End Sub

'*****************************************************
'Sub/Func  : CfgsToBuild
'          :
'Purpose   : Create a string representing the section of the build
'          : command that indicates which configs will be built
'          :
'Arguments : None
'          :
'Returns   : String
'          :
'Comments  : String is of the form 'mcfg_sw:config1,mcfg_sw:config2' etc
'*****************************************************
Function CfgsToBuild() As String
  Dim CfgList() As String
  Dim cfg As Range
  Dim SummaryListings As Range
  Dim i As Integer: i = 0 'index used for list of cfs to build
  Dim cfgType As String   'sw or hw config type
  
  cfgType = GetCfgType
  Set SummaryListings = GetSummaryCfgNames

  
  CfgsToBuild = ""
  ReDim CfgList(SummaryListings.Count)
  
  For Each cfg In SummaryListings
    If GetCfgSelection(cfg.Text) = True Then
      CfgList(i) = "mcfg_" & cfgType & ":" & cfg.Text
      i = i + 1
    End If
  Next cfg
  
  On Error GoTo IndexOutOfRange
  'Redimension array to size of populated elements
  ReDim Preserve CfgList(LBound(CfgList) To i - 1)
    
  CfgsToBuild = Join(CfgList, ",")
  Exit Function
  
IndexOutOfRange:
  If Err = 9 Then
    If i <= 0 Then
      MsgBox "Please select 1 or more configurations to generate.", _
      vbOKOnly, "Selection Error"
    Else
      MsgBox "CfgList array: Index out of range: " & i, _
      vbOKOnly + vbCritical, "Selection Error"
    End If
  Else
      MsgBox Err.Description
  End If
  End
  
End Function

Function GetVariantPath(MyFolder As String) As String
   Dim CommandSubstr As String
   Dim ParentDir As String
   Dim GrandParentDir As String
   Dim GrandParentDir_2 As String
   
   Dim srchToken As String: srchToken = "mcfg_gen"
   Dim Source As String
   Dim PathStart As String
   Dim PathLen As String
       
    'Source and Generated Binaries will now be stored in a "baseline\chipset\" subfolder'
    PathStart = InStrRev(MyFolder, srchToken)
    PathLen = Len(MyFolder) - PathStart - Len(srchToken)
    Source = Right(MyFolder, PathLen)
    
   ' ParentDir = GetBaseDir(Path)      'Parent dir will indicate the chipset these configs are for'
   ' Path = RemBaseDirFromPath(Path)   'Traverse up one directory in path string'
   ' GrandParentDir = GetBaseDir(Path) 'Grandparent dir will indicate the pl these configs are for'
   ' Path = RemBaseDirFromPath(Path)   'Traverse up one directory in path string'
   ' GrandParentDir_2 = GetBaseDir(Path) 'Grandparent_2 dir will indicate the image these configs are for'
   
    CommandSubstr = ""
    CommandSubstr = CommandSubstr & " --source-dir=" & Source

    
    GetVariantPath = CommandSubstr
End Function

Sub GenerateSources()
    Dim oShell
    Dim MyFolder As String
    Dim BuildID As String
    Dim BuildRoot As String
    Dim Command As String
    Dim Result As String
    
    Dim MyPath As String
    Dim cfgType As String   'sw or hw config type
    Dim Ret As String
    
    If ActiveWorkbook.ReadOnly Then
        Call ReadOnlyNotice
        Exit Sub
    End If
        
    Ret = MsgBox("Generating source files", vbOKOnly, "Generating")
    
    'Save workbook to ensure any edits from current session are included in generation
    ThisWorkbook.Save
    
    cfgType = GetCfgType
    
    'BuildID & BuildRoot are deprecated
    'BuildID = Application.ActiveSheet.buildIDTextBox.Text
    'BuildRoot = Application.ActiveSheet.buildRootTextBox.Text
    
    MyPath = Application.ActiveWorkbook.FullName
    MyFolder = Left(MyPath, InStrRev(MyPath, "\"))
    
    If ActiveSheet.Shapes("keepBuildWindowOpen").ControlFormat.Value = xlOn Then
        Command = "%comspec% /k ..\..\..\..\build\build_mcfgs.exe"
    Else
        Command = "%comspec% /c ..\..\..\..\build\build_mcfgs.exe"
    End If
   
    Command = Command & " --build_id=" & BuildID
    Command = Command & " --configs=" & CfgsToBuild
    Command = Command & " --force-regenerate"
    Command = Command & " --sources-only"
    Command = Command & " --" & cfgType & "_cfgs_workbook=" & MyPath
                   
    'Determine which subdirectories the new configs will be placed under'
    Command = Command & GetVariantPath(MyFolder)
    
    If StrComp(BuildRoot, "") <> 0 Then
        Command = Command & " --build_root=" & BuildRoot
    End If

    
    Set oShell = CreateObject("Wscript.shell")
    oShell.CurrentDirectory = MyFolder
    Result = oShell.Run(Command, 1, True)
    Result = Shell("explorer " & MyFolder, 1)
End Sub

Sub GenerateMBNs()
    Dim oShell
    Dim MyPath As String
    Dim MyFolder As String
    Dim BuildID As String
    Dim BuildRoot As String
    Dim Command As String
    Dim Result As String
    
                
    If ActiveWorkbook.ReadOnly Then
        Call ReadOnlyNotice
        Exit Sub
    End If
    
    'Save workbook to ensure any edits from current session are included in generation
    ThisWorkbook.Save

    'BuildID & BuildRoot are deprecated
    'BuildID = Application.ActiveSheet.buildIDTextBox.Text
    'BuildRoot = Application.ActiveSheet.buildRootTextBox.Text
    
    MyPath = Application.ActiveWorkbook.FullName
    MyFolder = Left(MyPath, InStrRev(MyPath, "\"))
    
    If ActiveSheet.Shapes("keepBuildWindowOpen").ControlFormat.Value = xlOn Then
        Command = "%comspec% /k ..\..\..\..\build\build_mcfgs.exe"
    Else
        Command = "%comspec% /c ..\..\..\..\build\build_mcfgs.exe"
    End If
    
    Command = Command & " --build_id=" & BuildID
    Command = Command & " --configs=" & CfgsToBuild
    Command = Command & " --force-regenerate"
    Command = Command & " --force-rebuild"
        
    'Determine which subdirectories the new configs will be placed under'
    Command = Command & GetVariantPath(MyFolder)

        
    If StrComp(BuildRoot, "") <> 0 Then
        Command = Command & " --build_root=" & BuildRoot
    End If
    
    Dim Ret As String
    Ret = MsgBox("Generating source and MBN files", vbOKOnly, "Generating")
    
    Set oShell = CreateObject("Wscript.shell")
    oShell.CurrentDirectory = MyFolder
    Result = oShell.Run(Command, 1, True)
    Result = Shell("explorer " & MyFolder & "..\..\..\..\configs", 1)
End Sub

Sub GenerateSignedMBNs()
    Dim oShell
    Dim MyPath As String
    Dim MyFolder As String
    Dim BuildID As String
    Dim BuildRoot As String
    Dim Command As String
    Dim Result As String
        
        
    If ActiveWorkbook.ReadOnly Then
        Call ReadOnlyNotice
        Exit Sub
    End If
        
    'Save workbook to ensure any edits from current session are included in generation
    ThisWorkbook.Save
 
    'BuildID & BuildRoot are deprecated
    'BuildID = Application.ActiveSheet.buildIDTextBox.Text
    'BuildRoot = Application.ActiveSheet.buildRootTextBox.Text
    
    MyPath = Application.ActiveWorkbook.FullName
    MyFolder = Left(MyPath, InStrRev(MyPath, "\"))
    
    'Determine which subdirectories the new configs will be placed under'
    Command = Command & GetVariantPath(MyFolder)
    
    If ActiveSheet.Shapes("keepBuildWindowOpen").ControlFormat.Value = xlOn Then
        Command = "%comspec% /k ..\..\..\..\build\build_mcfgs.exe"
    Else
        Command = "%comspec% /c ..\..\..\..\build\build_mcfgs.exe"
    End If
    
    Command = Command & " --build_id=" & BuildID
    Command = Command & " --configs=" & CfgsToBuild
    Command = Command & " --force-regenerate"
    Command = Command & " --force-rebuild"
    Command = Command & " --sign-build"
        
    If StrComp(BuildRoot, "") <> 0 Then
        Command = Command & " --build_root=" & BuildRoot
    End If
    
    Dim Ret As String
    Ret = MsgBox("Generating signed MBNs", vbOKOnly, "Generating")
    
    Set oShell = CreateObject("Wscript.shell")
    oShell.CurrentDirectory = MyFolder
    Result = oShell.Run(Command, 1, True)
    Result = Shell("explorer " & MyFolder & "..\..\..\..\configs", 1)
End Sub
