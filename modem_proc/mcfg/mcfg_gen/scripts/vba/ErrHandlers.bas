Attribute VB_Name = "ErrHandlers"
'*****************************************************
'History   : 10/25/14 (asandstr) - Created
'*****************************************************
'File Purpose:
'  Provide error Handling routines

'*****************************************************
Option Explicit
'*****************************************************
Public Function dhError( _
  Optional strProc As String = "<unknown>", _
  Optional fRespond As Boolean = False, _
  Optional objErr As ErrObject) _
As Boolean

  Dim strMessage As String
  Dim strTitle As String
  Dim intStyle As Integer
  
  'if the user didn't pass an ErrOjbect, use Err
  If objErr Is Nothing Then
    Set objErr = Err
  Else
    'Build title and message
    strTitle = "Error " & objErr.Number & " in " & strProc
    strMessage = "Error has occurred: " & vbCrLf & vbCrLf & objErr.Desctiption
    
    'set the icon and buttons for MsgBox
    intStyle = vbExclamation
    If fRespond Then
      intStyle = intStyle Or vbOKCancel
    End If
    
    'display message and return result
    dhError = (MsgBox(strMessage, intStyle, strTitle) = vbOK)
  End If
End Function

