Attribute VB_Name = "KeyboardShortcuts"
'*****************************************************
'History   : 11/06/14 (asandstr) - Created
'*****************************************************
'File Purpose:
'  Provide quick access to various macros by assigning
'  keyboard shortcuts

'*****************************************************

Option Explicit
'*****************************************************

Sub AssignShortcuts()
' Shortcuts need to be initiated outside of VBA editor
' See http://msdn.microsoft.com/en-us/library/ff197461.aspx
' for complete listing of key codes

    ' ALT = % | CTRL = ^ | SHIFT = +
    Application.OnKey "^+%E", "ExportAllComponents"         'Export all the VB components to VBA source directory
    Application.OnKey "^+%I", "ImportAllComponents"         'Import all the VB components from VBA source directory
    Application.OnKey "^+%C", "SetTRLCarrierNames"          'Set the TRL carrier name in each config worksheet
    Application.OnKey "^+%X", "EnableVBAextensibility"      'Turn on various Excel capabilities via Tools->References
    Application.OnKey "^+%D", "DeleteAllCfgSelectors"       'Removes checkboxes from the summary table
    'Application.OnKey "^+%R", "Macro7"                      'Add refresh button to the summary sheet
End Sub
