Attribute VB_Name = "Selectors"
'*****************************************************
'History   : 5/5/14 (asandstr) - Created.
'*****************************************************


'*****************************************************
'Global options / variables

'Options   : Explicit - force declaration of every variable
'          :
'Variables : ConfigListings - represents configuration list
'          :
'Comments  : Globals will need be reinitialized after any
'          : untrapped errors have occurred. So, always
'          : check a global is set before using it

'*****************************************************
Option Explicit


Public ConfigListings() As String
'*****************************************************
'Sub/Func  : GetConfigList
'          :
'Purpose   : Sets the string list of configurations.
'          :
'Arguments : List of cells representing the range
'          :
'Returns   : Boolean based on whether list provided was empty
'          :
'Comments  : If config listing is cleared due to workbook error
'          : then this function will have the list reinitialized
'*****************************************************
Function GetConfigList() As String()

  If IsArrayEmpty(ConfigListings) Then
    SetConfigList
  End If
  
  GetConfigList = ConfigListings
End Function

'*****************************************************
'Sub/Func  : SetConfigList
'          :
'Purpose   : Sets the string list of configurations.
'          :
'Arguments : List of cells representing the range
'          :
'Returns   : Boolean based on whether list provided was empty
'          :
'Comments  : If config listing is cleared due to workbook error
'          : then this function will have the list reinitialized
'*****************************************************
Function SetConfigList() As Boolean

  ConfigListings = getCfgWorksheetList()
  
  If IsArrayEmpty(ConfigListings) Then
    'todo: print an error message stating no configs found
    SetConfigList = False
    Exit Function
  End If
  
  'Set ConfigListings = cfgList
  SetConfigList = True
End Function

'*****************************************************
'Sub/Func  : InitSelectors
'          :
'Purpose   : Main initialization fucntion to be called
'          : when workbook opens
'          :
'Arguments : None
'          :
'Returns   : None
'          :
'Comments  : Called within ThisWorkBook.Open
'*****************************************************
Sub InitSelectors()

  If SetConfigList = False Then
    MsgBox "Config selectors not initialized"
    Exit Sub
  End If
  
  'Generate checkboxes if needed
  CreateCfgSelectors
End Sub

'*****************************************************
'Sub/Func  : getFirstCfgIndex
'Purpose   : Returns the index of the first config
'          : in the workbook
'          :
'Arguments : None
'          :
'Returns   : Integer Value
'          :
'Comments  : Index refers to the worksheet index
'*****************************************************
Function getFirstCfgIndex() As Integer
   getFirstCfgIndex = Sheets("Summary").index + 1
End Function

'*****************************************************
'Sub/Func  : getLastCfgIndex
'Purpose   : Returns the index of the last cfg
'          : in the workbook
'          :
'Arguments : None
'          :
'Returns   : Integer Value
'          :
'Comments  : Index refers to the worksheet index
'*****************************************************
Function getLastCfgIndex() As Integer
   getLastCfgIndex = Sheets("Revision History").index - 1
End Function

'*****************************************************
'Sub/Func  : InitSummaryCfgNames
'Purpose   : Generates a list of cells whose values
'          : represent the configurations to be generated
'          :
'Arguments : None
'          :
'Returns   : Range of cells
'          :
'Comments  : If workbook has a chage of state due to any error
'          : then InitSummaryCfgNames will need to be called again
'*****************************************************
Function InitSummaryCfgNames() As Range
  
  'Establish names of carriers from list of worksheet names
  Call SetConfigList
  
  'List the config names in the summary table
  Call SetSummaryCfgNames
                      
  'Get the cell range that encompasses all the config names
  Set InitSummaryCfgNames = GetSummaryCfgNames
End Function

Sub CreateWorksheetLink(ws As Worksheet, wsListing As Range)
    With ws
      .Hyperlinks.Add _
        Anchor:=wsListing, _
        Address:="", _
        SubAddress:="'" & Sheets(wsListing.Value2).Name & "'!A5", _
        ScreenTip:="Jump to worksheet"
    End With
End Sub

Sub SetSummaryCfgNames()
  Dim currCell As Range
  Dim CfgList() As String
  Dim i As Integer
  
  CfgList = GetConfigList()
  'Worksheets("Summary").Cells(B11) = Worksheets("Summary").Cells(11, 2)
  For i = 0 To UBound(CfgList)
    Set currCell = Worksheets("Summary").Cells(11, 2 + i)
    currCell.Value = CfgList(i)
    
    Call CreateWorksheetLink(Worksheets("Summary"), currCell)
  Next i
  
End Sub

'*****************************************************
'Sub/Func  : GetSummaryCfgNames
'          :
'Purpose   : Wrapper around InitSummaryCfgNames.
'          :
'Arguments : None
'          :
'Returns   : Range of cells
'          :
'Comments  : If config listing is cleared due to workbook error
'          : then this function will have the list reinitialized
'*****************************************************
Function GetSummaryCfgNames() As Range
  Dim cfg1 As Range, cfgN As Range

  '1st config listed at this location
  Set cfg1 = Worksheets("Summary").Range("B11")
  'Last config: Find first empty cell and select cell just before it
  Set cfgN = cfg1.EntireRow.Find(What:="", _
                                 LookIn:=xlValues, _
                                 LookAt:=xlWhole, _
                                 SearchOrder:=xlByColumns) _
                            .offset(, -1)
                       
  Set GetSummaryCfgNames = Range(cfg1, cfgN)
End Function

Function getCfgWorksheetList() As String()
  Dim CfgList() As String
  Dim offset As Integer, i As Integer
  Dim firstIndex As Integer, lastIndex As Integer
  
  'Resize the array to the number of configs present in the worksheet
  firstIndex = getFirstCfgIndex
  lastIndex = getLastCfgIndex
  ReDim CfgList(lastIndex - firstIndex) As String
  
  'Array indices start at 0. Use offset map config indixes to array
  offset = firstIndex
  For i = firstIndex To lastIndex
    CfgList(i - offset) = Sheets(i).Name
  Next
  
  getCfgWorksheetList = CfgList()
End Function

'Function GetSummaryCfgNames() As String'
'
'  If ConfigListings Is Nothing Then
'    InitSummaryCfgNames
'  End If
'
'  Set GetSummaryCfgNames = ConfigListings
'End Function

Sub FormatNewTableEntry(cell As Range)
  Worksheets("Summary").Activate
  Range("A10:A15").Select
  Selection.Copy
  Range(cell.offset(-1, 0), cell.offset(4, 0)).Select
  Selection.PasteSpecial Paste:=xlPasteFormats, Operation:=xlNone, _
      SkipBlanks:=False, Transpose:=False
  Application.CutCopyMode = False
  ActiveWindow.SmallScroll ToRight:=-13
End Sub

Sub RefreshSummary()
  Dim CfgList As Range
  Dim cell As Range
  Application.ScreenUpdating = False
  
  Worksheets("Summary").Activate
  'Perform garbage collection for config selectors...
  DeleteAllCfgSelectors
  
  'Reset the list of available config names
  Set CfgList = InitSummaryCfgNames
  For Each cell In CfgList
    Call FormatNewTableEntry(cell)
  Next cell
  
  Call CreateCfgSelectors
End Sub
'*****************************************************
'Sub/Func  : SetCfgSelection
'          :
'Purpose   : Marks a configuration for generation inclusion/exculsion
'          :
'Arguments : Selected - bool value used to indicate if
'          : option should be selected
'          : cfgName - Name of the configuration to be selected
'          :
'Returns   : None
'          :
'Comments  : None
'*****************************************************
Sub SetCfgSelection(cfgName As String, Selected As Boolean)
  Dim Setting As Integer:  Setting = 0
  
  If Selected Then
    Setting = 1
  End If
  
  On Error GoTo MissingSelector
  ActiveSheet.Shapes(cfgName).OLEFormat.Object.Value = Setting
  Exit Sub
  
MissingSelector:
  If Err = -2147024809 Then
    MsgBox "Checkbox not found for " & cfgName & vbCr & _
    "Please save and reopen spreadsheet"
  End If
  End
  
End Sub

'*****************************************************
'Sub/Func  : GetCfgSelection
'          :
'Purpose   : Return a check box state as a boolean
'          :
'Arguments : cfgName - Name of the configuration
'          :
'Returns   : Bool representing check box state
'          :
'Comments  :
'*****************************************************
Function GetCfgSelection(cfgName As String) As Boolean
  Dim Selection As Integer
  
  On Error GoTo MissingSelector
  Selection = ActiveSheet.Shapes(cfgName).OLEFormat.Object.Value
  
  If Selection = 1 Then
    GetCfgSelection = True
  Else
    GetCfgSelection = False
  End If
  Exit Function
  
MissingSelector:
  If Err = -2147024809 Then
    MsgBox "Checkbox not found for " & cfgName & vbCr & _
    "Please save and reopen spreadsheet"
  End If
  End
End Function

'*****************************************************
'Sub/Func  : SetAllSelectors
'          :
'Purpose   : Mark all configurations as included/excluded
'          : based on the value of Selected
'          :
'Arguments : Selected - value indicated if box will be checked/unchecked
'          :
'Returns   : None
'          :
'Comments  :
'*****************************************************

Sub SetAllSelectors(Selected As Boolean)
  Dim cfg As Range
  Dim CfgList As Range
    
  Set CfgList = GetSummaryCfgNames
  
  For Each cfg In CfgList
    Call SetCfgSelection(cfg.Text, Selected)
  Next cfg
  
End Sub

'*****************************************************
'Sub/Func  : SelAllCfgs
'          :
'Purpose   : Mark all configs for generation
'          :
'Arguments : None
'          :
'Returns   : None
'          :
'Comments  :
'*****************************************************
Sub SelAllCfgs()
  Call SetAllSelectors(True)
End Sub

'*****************************************************
'Sub/Func  : DeselAllCfgs
'          :
'Purpose   : Mark all configs for generation
'          :
'Arguments : None
'          :
'Returns   : None
'          :
'Comments  :
'*****************************************************
Sub DeselAllCfgs()
  Call SetAllSelectors(False)
End Sub

'*****************************************************
'Sub/Func  : DeleteAllCfgSelectors
'          :
'Purpose   : Removes all config related checkboxes
'          :
'Arguments : None
'          :
'Returns   : None
'          :
'Comments  : Used for debugging purposes when boxes
'          : need to be removed
'*****************************************************
Sub DeleteAllCfgSelectors()
  Dim cfg As Variant
  Dim CfgList As Range
  
  Set CfgList = GetSummaryCfgNames
  
  For Each cfg In CfgList
    On Error Resume Next
    ActiveSheet.CheckBoxes(cfg.Text).Delete
  Next cfg
End Sub

'*****************************************************
'Sub/Func  : IsCheckboxPresent
'          :
'Purpose   : Determine if a checkbox is already present
'          : for a given config
'          :
'Arguments : cfg - Cell representing a configuration listing
'          :
'Returns   : Bool indicating whether a checkbox selector exists
'          :
'Comments  :
'*****************************************************
Function IsCheckboxPresent(cfg As Range) As Boolean
  Dim cfgName As String
  Dim boxID As Integer
  
  IsCheckboxPresent = False
  cfgName = cfg.Text
  
  'If a checkbox is not present, attempts at accessing ID will throw an error
  'Resume to next statement after error
  On Error Resume Next
  boxID = ThisWorkbook.Sheets("Summary").Shapes(cfgName).ID
  If boxID > 0 Then
    IsCheckboxPresent = True
  End If
  
End Function

'*****************************************************
'Sub/Func  : createCheckbox
'          :
'Purpose   : Create a checkbox for a given configuration
'          :
'Arguments : cfg  - a cell representing a configuration
'          :
'Returns   : None
'          :
'Comments  :
'*****************************************************
Sub createCheckbox(cfg As Range)
  Dim genSelect As Range
  Dim position As Range
  
  Application.ScreenUpdating = False
  'Box position relative to config listing
  Set position = cfg.offset(4, 0)
    
  With Worksheets(1).CheckBoxes.Add(position.Left, position.Top, position.Width, position.Height)
      'Name the checkbox after the cfg name (e.g. Carrier)
      .Name = cfg.Text
      'clear the caption
      .Text = ""
      'TODO: Find way to center checkbox and enable only checkbox area as clickable
      .Left = position.Left
  End With
End Sub

'*****************************************************
'Sub/Func  : CreateCfgSelectors
'          :
'Purpose   : Upon initialization, generate any missing checkboxes
'          : in case new sheets have been added
'          :
'Arguments : None
'          :
'          :
'Returns   : None
'          :
'Comments  :
'*****************************************************
Sub CreateCfgSelectors()

   Dim cfg As Range
   Dim CfgList As Range
   
   Set CfgList = GetSummaryCfgNames

   For Each cfg In CfgList
     If IsCheckboxPresent(cfg) <> True Then
       Call createCheckbox(cfg)
     End If
   Next cfg
End Sub

'Utility functions
Public Function IsArrayEmpty(Arr As Variant) As Boolean
'*****************************************************
'Sub/Func  : IsArrayEmpty
'          :
'Purpose   : Tests whether an array is empty
'          :
'Arguments : Arr - Array variable
'          :
'          :
'Returns   : Boolean - T/F
'          :
'Comments  :
'*****************************************************
    Dim lower_bound As Long
    Dim upper_bound As Long
    Dim size As Long
    
    size = 0
    lower_bound = 0
    upper_bound = 0
    IsArrayEmpty = True
    
    If Not IsMissing(Arr) Then
      On Error GoTo ArrSize_Err_handl:
      If IsArray(Arr) = True Then
        On Error GoTo ArrSize_Err_handl:
        upper_bound = UBound(Arr, 1)
        On Error GoTo ArrSize_Err_handl:
        lower_bound = LBound(Arr, 1)
        
        size = upper_bound - lower_bound
        If (size > 0) Then
            IsArrayEmpty = False
        End If
      End If
    End If
    Exit Function
    
ArrSize_Err_handl:
    'Assume array is uninitialized if there are issues
    'accessing its contents
    IsArrayEmpty = True
    Err.Clear
End Function

Sub UpdateSummary()

    'ActiveWorkbook.Sheets.Item("Summary").buildRootTextBox.Text = ""
    
    'List the image and sw-product for generated configs
    'InitializeVariantSummary
    
    'create the checkboxes used for selecting which configs to generate
    RefreshSummary
End Sub
