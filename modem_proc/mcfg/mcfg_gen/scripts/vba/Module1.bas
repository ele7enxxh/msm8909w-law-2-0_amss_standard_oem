Attribute VB_Name = "Module1"
Sub createQCVersion()
'
' createQCVersion Macro
'

'
    Rows("433:435").Select
    Selection.Copy
    Rows("448:448").Select
    Selection.Insert Shift:=xlDown
    Range("F448").Select
    Application.CutCopyMode = False
    ActiveCell.FormulaR1C1 = "0x05"
    Range("A448").Select
    ActiveCell.FormulaR1C1 = "QC_version_type"
    Range("A449").Select
    ActiveCell.FormulaR1C1 = "QC_version_len"
    Range("A450").Select
    ActiveCell.FormulaR1C1 = "QC_Version"
    Range("A451").Select
End Sub

Sub Macro1()
Attribute Macro1.VB_ProcData.VB_Invoke_Func = "T\n14"
'
' Macro2 Macro
'
' Keyboard Shortcut: Ctrl+Shift+T
'
    Selection.Copy
    Range("B43").Select
    Selection.PasteSpecial Paste:=xlPasteValues, Operation:=xlNone, SkipBlanks _
        :=False, Transpose:=False
    Rows("44:44").Select
    Application.CutCopyMode = False
    Selection.Delete Shift:=xlUp
    Range("B43").Select
    ActiveWindow.ScrollColumn = 2
    ActiveWindow.ScrollColumn = 3
    ActiveWindow.ScrollColumn = 4
    ActiveWindow.ScrollColumn = 5
    ActiveWindow.ScrollColumn = 7
    ActiveWindow.ScrollColumn = 8
    Range("B43:Q44").Select
    Selection.Copy
    ActiveWindow.ScrollColumn = 7
    ActiveWindow.ScrollColumn = 5
    ActiveWindow.ScrollColumn = 4
    ActiveWindow.ScrollColumn = 3
    ActiveWindow.ScrollColumn = 2
    ActiveWindow.ScrollColumn = 1
    Range("B46").Select
    Selection.PasteSpecial Paste:=xlPasteAll, Operation:=xlNone, SkipBlanks:= _
        False, Transpose:=True
    Rows("43:44").Select
    Application.CutCopyMode = False
    Selection.Delete Shift:=xlUp
    Range("B44").Select
    ActiveWindow.SmallScroll Down:=6
    Range("B44:C59").Select
    Selection.Copy
End Sub

Sub Increment_cell_value()
'
' Increment_cell_value Macro
'
' Keyboard Shortcut: Ctrl+Shift+I
'
   ' Range("F28").Value = Range("F28").Value + 1
   For Each c In Selection
     c.Value = c.Value + 1
   Next
End Sub

Sub Macro7()
Attribute Macro7.VB_ProcData.VB_Invoke_Func = "R\n14"
'
' Macro7 Macro
'
' Keyboard Shortcut: Ctrl+Shift+R
'
    ActiveSheet.Buttons.Add(44.25, 300, 105.75, 15.75).Select
    Selection.OnAction = "RefreshSummary"
    Selection.Characters.Text = "Refresh Summary List"
    With Selection.Characters(Start:=1, Length:=20).Font
        .Name = "Arial"
        .FontStyle = "Regular"
        .size = 10
        .Strikethrough = False
        .Superscript = False
        .Subscript = False
        .OutlineFont = False
        .Shadow = False
        .Underline = xlUnderlineStyleNone
        .ColorIndex = 1
    End With
    Range("B17").Select
End Sub

Sub sumUpdate()
'
' sumUpdate Macro
'
' Keyboard Shortcut: Ctrl+Shift+U
'
    Rows("19:19").Select
    ActiveSheet.Buttons.Add(44.25, 302.25, 109.5, 18).Select
    Selection.OnAction = "RefreshSummary"
    Selection.Characters.Text = "Refresh Summary Table"
    With Selection.Characters(Start:=1, Length:=21).Font
        .Name = "Arial"
        .FontStyle = "Regular"
        .size = 10
        .Strikethrough = False
        .Superscript = False
        .Subscript = False
        .OutlineFont = False
        .Shadow = False
        .Underline = xlUnderlineStyleNone
        .ColorIndex = 1
    End With
    Range("C18").Select
    ActiveSheet.OLEObjects.Add(ClassType:="Forms.CommandButton.1", Link:=False _
        , DisplayAsIcon:=False, Left:=435.882352941176, Top:=390, Width:= _
        72.3529411764706, Height:=23.8235294117647).Select
    ActiveSheet.Shapes.Range(Array("buildVersionTextBox")).Select
    Selection.Delete
    Selection.Cut
    ActiveSheet.Shapes.Range(Array("CommandButton1")).Select
    Selection.Delete
    Selection.Cut
    Range("A24").Select
    Selection.Cut
    Range("A24").Select
    Application.CutCopyMode = False
    Selection.ClearContents
End Sub
 
'If you have more then one occurrence of the value this will select the last occurrence.
Function Find_Last_Entry(cfg As String, Optional FindString As String = "") As Range
    Dim rng As Range
    
    If FindString = "" Then
       FindString = InputBox("Enter a Search value")
    End If
    If Trim(FindString) <> "" Then
         With Sheets(cfg).Range("A:A")
             Set rng = .Find(What:=FindString, _
                             After:=.Cells(1), _
                             LookIn:=xlValues, _
                             LookAt:=xlWhole, _
                             SearchOrder:=xlByRows, _
                             SearchDirection:=xlPrevious, _
                             MatchCase:=False)
         End With
    End If
    
    If Not rng Is Nothing Then
       Set Find_Last_Entry = rng
    Else
       Set Find_Last_Entry = Nothing
    End If
    
End Function

'Carrier names in trailer records are changing a lot. This function makes it easy to modify all of them
Sub SetTRLCarrierNames()
  Dim prefix As String
  Dim currCell As Range
  Dim cfg As String
  Dim i As Integer
  
  'Carrier name located at B9 on Summary page
  prefix = Sheets("Summary").Cells(9, "B").Value
  
  'Make sure config list is up to date
  SetConfigList
  
  For i = LBound(ConfigListings) To UBound(ConfigListings)
    Set currCell = Find_Last_Entry(ConfigListings(i), "carrier_name")
    If Not currCell Is Nothing Then
      currCell.offset(0, 5).Value = ConfigListings(i) & "-" & prefix
    End If
  Next
    
End Sub
