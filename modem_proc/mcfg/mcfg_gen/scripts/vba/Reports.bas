Attribute VB_Name = "Reports"
'force declaration of every variable
Option Explicit

'Find the number of configs in this workbook'
Public Function Get_Carrier_Total() As Integer
    Dim non_carrier As Integer, carrier_count As Integer
    
    'number of spreadsheets that are not mcfg configurations'
    non_carrier = 2
    
    Get_Carrier_Total = Worksheets.Count - non_carrier
End Function

'Tally the total number of EFS and NV items between 2 sheets
Function Sum_NV_EFS_items(StartFlag As String, StopFlag As String)

    Dim num_item As Integer
    Dim activeIndex As Integer
    Dim sheetname As String
    Dim num_header As Integer
    Dim index As Integer
    
    ' determine current bounds
    Dim StartIndex As Integer, EndIndex As Integer, LoopIndex As Integer
    StartIndex = Sheets(StartFlag).index + 1
    EndIndex = Sheets(StopFlag).index - 1

    'Store which sheet is initially active
    activeIndex = ActiveSheet.index
    
    num_item = 0
    For index = StartIndex To EndIndex
        sheetname = Sheets(index).Name
        
        'Select the worksheet and cell from the column to be tallied
        Worksheets(sheetname).Activate
        ActiveSheet.Range("B1").Select
        
        'Add count of all cells from active column that are not blank
        num_item = num_item + WorksheetFunction.CountA(ActiveCell.EntireColumn)
    Next index
    
    'Reactivate originally active sheet
    Worksheets(activeIndex).Activate
    
    'On every carrier spreadsheet there are 2 header listings in column B, which need to be removed from the tally
    num_item = num_item - (Get_Carrier_Total() * 2)
    
    Sum_NV_EFS_items = num_item
End Function

'Set the total number of items for all carriers
Sub Set_total_workbook_item_count()

   Worksheets("Revision History").Range("E6").Value = Sum_NV_EFS_items("Summary", "Revision History")
End Sub

'Set the total number of carriers
Sub Set_Carrier_Total()

   Worksheets("Revision History").Range("E5").Value = Get_Carrier_Total()
End Sub

'function to be called upon the open event in ThisWorkbook module
'add any new statistics to this generate function
Sub GenerateSpreadsheetStats()
    Set_total_workbook_item_count
    Set_Carrier_Total
End Sub
