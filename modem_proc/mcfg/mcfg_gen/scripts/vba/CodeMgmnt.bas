Attribute VB_Name = "CodeMgmnt"
'*****************************************************
'History   : 5/22/14 (asandstr) - Created
'*****************************************************
'File Purpose:
'  Provides general purpose handling of VBA modules themselves
'  Key functions include the ability to import and export VBA modules in the workbook

'*****************************************************
'ExportAllComponents code found at experts-exchange.com
'site covering how to program the VBA editor itself: http://www.cpearson.com/excel/vbe.aspx
'*****************************************************

Option Explicit

'*****************************************************
'        GLOBALS
'*****************************************************

'Determines whether import/export of modules is done based on file versions
Public Const VerControl As Boolean = False
'Source directory that contains where vba code is located
Public Const VBSrcDir As String = "\..\..\..\scripts\vba"
'Controls whether VBA modules are automatically updated when spreadsheet is opened
Public Const AutoVBUpdate As Boolean = False
'*****************************************************
'Dictionary / hash table of the various versions of each file
Private versionDict As Dictionary


'Certain settings / functions need to be enabled/present for automatic import/export of VB modules
'This function makes this possible
Sub BootStrapCodeMgmnt()
   Call EnableVBAextensibility
   ImportAllComponents
End Sub


Sub UpdateVBModules(direction As String)
  If direction = "import" Then
    ImportAllComponents
  ElseIf direction = "export" Then
    ExportAllComponents
  Else
    MsgBox "Invalid direction passed to UpdateVBModules. No action. Direction: " & direction
  End If
End Sub
'*****************************************************
'Sub/Func  : ExportAllComponents
'          :
'Purpose   : Export all vb components that are not empty
'          :
'Arguments : None
'          :
'Returns   : Null
'          :
'Comments  : Exports all VBA project components containing code to a folder in the same directory as this spreadsheet.
'          : Requires add-in Microsoft Visual Basic for Applications Extensibility
'          : To add, select Microsoft Visual Basic for Applications Extensibility under Tools->Reference
'*****************************************************
Public Sub ExportAllComponents()
    Dim VBComp As VBIDE.VBComponent
    Dim destDir As String, fName As String, ext As String
    'Create the directory where code will be created.
    'Alternatively, you could change this so that the user is prompted
    If ActiveWorkbook.Path = "" Then
        MsgBox "You must first save this workbook somewhere so that it has a path.", , "Error"
        Exit Sub
    End If
    destDir = ActiveWorkbook.Path & VBSrcDir
    If Dir(destDir, vbDirectory) = vbNullString Then MkDir destDir
    
    'Export all non-blank components to the directory
    For Each VBComp In ActiveWorkbook.VBProject.VBComponents
        If VBComp.CodeModule.CountOfLines > 0 Then
            'Determine the standard extension of the exported file.
            'These can be anything, but for re-importing, should be one of the following:
            Select Case VBComp.Type
                Case vbext_ct_ClassModule: ext = ".cls"
                Case vbext_ct_Document: ext = ".cls"
                Case vbext_ct_StdModule: ext = ".bas"
                Case vbext_ct_MSForm: ext = ".frm"
                Case Else: ext = vbNullString
            End Select
            If ext <> vbNullString Then
                fName = destDir & "\" & VBComp.Name & ext
                'this will still try to export a file if the
                If VerControl = False Or IsFileVerNewer(VBComp.Name, "external") = False Then
                  'Overwrite the existing files if not set to read only
                  If Dir(fName) <> vbNullString Then
                    If GetAttr(fName) And vbReadOnly Then
                      MsgBox "Cannot overwrite read only files in " & destDir
                      Exit Sub
                    End If
                    
                    Kill (fName)
                  End If
                  VBComp.Export (fName)
                End If
            End If
        End If
        'Ensure previous commands complete before proceeding
        DoEvents
    Next VBComp
End Sub

Function IsVBModule(filename As String) As Boolean
   Dim ext As String
   
   ext = Right(filename, 4)
   Select Case ext
       Case ".cls": IsVBModule = True
       Case ".bas": IsVBModule = True
       Case ".frm": IsVBModule = True
       Case Else: IsVBModule = False
   End Select
End Function
'*****************************************************
'Sub/Func  : ImportAllComponents
'          :
'Purpose   : Import all vb components from specified directory
'          :
'Arguments : None
'          :
'Returns   : Null
'          :
'Comments  : Exports all VBA project components containing code to a folder in the same directory as this spreadsheet.
'          : Requires add-in Microsoft Visual Basic for Applications Extensibility
'          : To add, select Microsoft Visual Basic for Applications Extensibility under Tools->Reference
'*****************************************************
Public Sub ImportAllComponents()
    Dim sourceDir As String
    Dim file As Variant
    Dim fPath As String
    Dim fName As String
    Dim vbCompName As String
    
    'Alternatively, you could change this so that the user is prompted
    If ActiveWorkbook.Path = "" Then
        MsgBox "You must first save this workbook somewhere so that it has a path.", , "Error"
        Exit Sub
    End If
    
    'Verify source directory existence
    'Todo: see if trailing \ can be included in VBSrcDir in other locations
    sourceDir = ActiveWorkbook.Path & VBSrcDir & "\"
    If Dir(sourceDir) = "" Then
        MsgBox "vba source directory not found: " & sourceDir
        Exit Sub
    Else
      'loop through the directory, importing ea. file.
      'options --> general --> compile on demand must be checked,
      'otherwise copies of each module are made before the modules are removed
      fName = Dir(sourceDir)
      Do
        fPath = sourceDir & fName
        
        If IsVBModule(fPath) Then
          vbCompName = RemoveExt(fName)
          
          If VerControl = False Or IsFileVerNewer(vbCompName, "internal") = True Then
            Call ImportComponent(fPath, vbCompName)
            'Ensure commands complete before moving on...
            DoEvents
          End If
        End If
        
        fName = Dir
      Loop While fName <> ""
    End If
End Sub

Sub ImportComponent(fPath As String, vbCompName As String)
   Dim VBComp As VBIDE.VBComponent
  
   If vbCompName = "CodeMgmnt" Then
   'ToDO: find way to programmatically update codeMgmnt while it's being executed
   ElseIf Instring(fPath, ".cls") Then
     'Since cls files cannot be removed, they must be edited inline
     Call UpdateWorkbookEventCode(vbCompName, fPath)
   Else
     'unlike .cls files, .bas files can be removed
     If ModuleExists(vbCompName) Then
       Set VBComp = ActiveWorkbook.VBProject.VBComponents(vbCompName)
       ActiveWorkbook.VBProject.VBComponents.Remove VBComp
     End If
     'Replace the missing component with the updated version in VBSrcDir
     Set VBComp = ActiveWorkbook.VBProject.VBComponents.Import(fPath)
   End If
End Sub

'Found syntax for opening a file cumbersome. So created openfile function
Function OpenFile(file As String, Optional mode As String = "read") As Integer
  Dim iFileNumber As Integer: iFileNumber = 0
  Dim FSO As Scripting.FileSystemObject
  Dim FolderPath As String
  
  If file = "" Then
    MsgBox "Empty string passed to OpenFile"
    Exit Function
  End If
  
  
  Set FSO = New Scripting.FileSystemObject
  If FSO.FileExists(file) = True Then
    'Handle error msgs outside of function in case OpenFile is used in a loop
    iFileNumber = FreeFile()
    
    If mode = "read" Then
      Open file For Input As iFileNumber
      
    ElseIf mode = "write" Then
      If GetAttr(file) And vbReadOnly Then
        MsgBox "File is Read-only. Cannot write to: " & vbCrLf & file
        Exit Function
      Else
        'Clears contents of file upon successful open
        Open file For Output As iFileNumber
      End If
    End If
    
  Else 'file does not exist, ok to create new file
    If mode = "write" Then
      'Clears contents of file upon successful open
      Open file For Output As iFileNumber
    End If
  End If
  
  OpenFile = iFileNumber
End Function


Sub ReadOnlyNotice()
   MsgBox "File is Read-only. " & vbCrLf & _
   "Please set write privelages before using workbook for carrier generation."
End Sub

Sub VBUpdateOnOpen()
  If AutoVBUpdate = True Then
    UpdateVBModules ("import")
  End If
End Sub

'*****************************************************
'Sub/Func  : ModuleExists
'          :
'Purpose   : Identify whether a module exists with the given name
'          :
'Arguments : vbCompName - name of the module to check
'          :
'Returns   : boolean
'          :
'Comments  : None
'*****************************************************
Function ModuleExists(vbCompName As String) As Boolean
   Dim module As Object
   
   ModuleExists = False
   For Each module In ActiveWorkbook.VBProject.VBComponents
     If module.Name = vbCompName Then
       ModuleExists = True
       Exit For
     End If
   Next
  
End Function

'*****************************************************
'Sub/Func  : RemoveExt
'          :
'Purpose   : Removed the file extension from a filename
'          :
'Arguments : fname - filename string
'          :
'Returns   : filename string without the extension
'          :
'Comments  : Removed the .XXX extension (4 char)
'*****************************************************
Function RemoveExt(fName As String) As String
  Dim extLen As Integer: extLen = 4
  
  RemoveExt = Left(fName, Len(fName) - extLen)
End Function

'*****************************************************
'Sub/Func  : UpdateWorkbookEventCode
'          :
'Purpose   : Updates the code a single module contains
'          :
'Arguments : vbCompName - module name
'          : fPath - full filepath, i.e. directory/filename
'          :
'Returns   : none
'          :
'Comments  : none
'*****************************************************
Sub UpdateWorkbookEventCode(vbCompName As String, fPath As String)
  Call DeleteWorkbookEventCode(vbCompName)
  Call ImportWorkbookEventCode(vbCompName, fPath)
End Sub

'*****************************************************
'Sub/Func  : DeleteWorkbookEventCode
'          :
'Purpose   : Deltes all the code a module contains,
'          : but not the module itself
'          :
'Arguments : vbCompName - module name
'          :
'Returns   : none
'          :
'Comments  : Currently just used for ThisWorkbook
'          : Requires add-in Microsoft Visual Basic for Applications Extensibility
'          : To add, select Microsoft Visual Basic for Applications Extensibility under Tools->Reference
'*****************************************************
Sub DeleteWorkbookEventCode(vbComponentName As String)
    With ThisWorkbook.VBProject.VBComponents(vbComponentName).CodeModule
            .DeleteLines 1, .CountOfLines
    End With
End Sub

'Import WorkbookEvent Code
Public Sub ImportWorkbookEventCode(module As String, fPath As String)
   Dim iFileNumber As Integer
   Dim sBuf As String
   Dim lineNum As Integer
 
   Application.VBE.ActiveVBProject.VBComponents.Item(module) _
     .CodeModule.AddFromFile (fPath)
 
   If Instring(fPath, ".cls") Then
      RemoveHeader (module)
   End If
'   iFileNumber = FreeFile()
'   Open file For Input As iFileNumber
''
   'Skip module header listed in exported version of ThisWorkbook
'   Do While Not EOF(iFileNumber)
'     Line Input #iFileNumber, sBuf
'     If Instring(sBuf, "Sub") Then
'       Exit Do
'     End If
'   Loop
'
'   lineNum = 1
'   Do While Not EOF(iFileNumber)
'      Application.VBE.ActiveVBProject.VBComponents.Item("ThisWorkBook") _
'      .CodeModule.AddFromString (sBuf)
'      'Read in lines to be copied to ThisWorkbook class
'      Line Input #iFileNumber, sBuf
'      lineNum = lineNum + 1
'
'  '    ActiveWorkbook.VBProjet.VBComponents("ThisWorkbook").CodeModule.InsertLines lineNum, sBuf
'   Loop
'   Close iFileNumber
   'open file
   'for ea line in file
   'insert line into thisworkbook'
End Sub
'Remove the header created from the initial export of the module code
Sub RemoveHeader(vbComponentName As String)
  Dim lineCnt As Integer: lineCnt = 4
  
  With ThisWorkbook.VBProject.VBComponents(vbComponentName).CodeModule
    .DeleteLines 1, lineCnt
  End With
End Sub

Public Function Instring(sBuf As String, searchString As String) As Boolean
  Instring = InStr(sBuf, searchString)
End Function


'*****************************************************
'*****************************************************
   ' To ensure no modules need to be renamed after import sections of others have been removed and placed below this section.
   ' Otherwise, accessing code from other modules resulted in a lock on them that prevented them from being removed
   ' E.g. FileVersioning was left and FileVersioning1 was added.
   ' Will figure out how to get around this later
'*****************************************************
'*****************************************************


Sub SetVersionDict()
  Set versionDict = New Dictionary
  'For any new file add to this version table
  'Whenever a file is updated also update the corresponding version
  versionDict("CodeMgmnt") = 0.1
  versionDict("ErrHandlers") = 0.1
  'Update FileVersioning any time another file is updated as well
  versionDict("FileVersioning") = 0.1
  versionDict("Generation") = 0.1
  versionDict("Globals") = 0.1
  versionDict("KeyboardShortcuts") = 0.1
  versionDict("Module1") = 0.1
  versionDict("Reports") = 0.1
  versionDict("Selectors") = 0.1
  versionDict("WkBkMgmnt") = 0.1
End Sub

Function GetModuleVersion(module As String) As Double
  
  If module = "" Then
    'see if GetVersion automatically returns 0#
    'GetModuleVersion = 0#
    Exit Function
  End If
  
  If versionDict Is Nothing Then
    SetVersionDict
  End If
  
  GetModuleVersion = versionDict(module)
End Function

Function GetInternalFileVer(module As String) As Double
  
  GetInternalFileVer = GetModuleVersion(module)
End Function

Function GetExternalFileVer(module As String) As Double
  Dim sBuf As String
  Dim ver As Double: ver = 0# 'Init version in case ext. module version not obtained
  Dim verFile As String
  Dim iFileNumber As Integer
  
  verFile = GetVBVerFile
  iFileNumber = OpenFile(verFile)
  
  If iFileNumber > 0 Then
    Do While Not EOF(iFileNumber)
      Line Input #iFileNumber, sBuf
      If sBuf Like "*versionDict*" & module & "*" Then
        ver = ExtractVerFrmString(sBuf)
        Exit Do
      End If
    Loop
    Close iFileNumber
  End If
  
  GetExternalFileVer = ver
End Function
Function GetVBVerFile() As String

  GetVBVerFile = ActiveWorkbook.Path & VBSrcDir & "\" & "FileVersioning.bas"
End Function

Function IsFileVerNewer(module As String, destFile As String) As Boolean
  
  If destFile = "internal" Then
    If GetExternalFileVer(module) > GetInternalFileVer(module) Then
      IsFileVerNewer = True
    Else
      IsFileVerNewer = False
    End If
  ElseIf destFile = "external" Then
    If GetInternalFileVer(module) > GetExternalFileVer(module) Then
      IsFileVerNewer = True
    Else
      IsFileVerNewer = False
    End If
  End If
  
End Function

Function ExtractVerFrmString(sBuf As String) As Double
  Dim ver As Double: ver = 0#
  Dim strVer As String
  Dim position As Integer
  Dim vEnd As Integer, vStart As Integer
  
  If sBuf = "" Then
    ExtractVerFrmString = ver
    Exit Function
  End If
  
  'Extract substring representing version
  vStart = InStrRev(sBuf, " ")
  strVer = Mid(sBuf, vStart + 1, Len(sBuf))
  
  If IsNumeric(strVer) Then
    ver = strVer
  End If
  
  ExtractVerFrmString = ver
End Function

'*****************************************************
' Belongs in WkBkMgmnt
'*****************************************************
'enables VBA references under Tools->Reference
Sub EnableVBAextensibility()
    
    'An error is thrown if this is already enabled. Just continue
    On Error Resume Next
    'Microsoft Visual Basic for Applications Extensibility
    ThisWorkbook.VBProject.References.AddFromGuid _
      GUID:="{0002E157-0000-0000-C000-000000000046}", _
      Major:=5, Minor:=3

    'An error is thrown if this is already enabled. Just continue
    On Error Resume Next
    'Microsoft Scripting Runtime - For using Dictionary object
    ThisWorkbook.VBProject.References.AddFromGuid _
      GUID:="{420B2830-E718-11CF-893D-00A0C9054228}", _
      Major:=1, Minor:=0
      
End Sub


