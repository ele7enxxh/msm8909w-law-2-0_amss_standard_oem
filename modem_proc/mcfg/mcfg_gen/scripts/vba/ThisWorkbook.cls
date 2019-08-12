VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "ThisWorkbook"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = True
Private Sub Workbook_Open()

    If ActiveWorkbook.ReadOnly Then
      'Call ReadOnlyNotice
    Else
      'Line below ensures application doesn't flicker when activating different sheets/cells
      Application.ScreenUpdating = False
    
      'Make any VB updates before continuing
      VBUpdateOnOpen
              
      'Enable various packages from Tools->Reference list. Requires Trust Access to the VBA Project Object model to be on
      'EnableVBAextensibility
     
      'Update the Summary sheet
      UpdateSummary
        
      'Generate the statistics to be displayed on the revision history tab
      GenerateSpreadsheetStats
        
      'Enable quick keyboard shortcuts to key macros
      AssignShortcuts
       
      'To suppress prompt to save workbook before generating scripts/mbns. This prompt appears now that macros are initializing data after this Workbook.Open event'
      ThisWorkbook.Saved = True
    End If
End Sub
