; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CNewGame
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "Surakarta.h"
LastPage=0

ClassCount=7
Class1=CSurakartaApp
Class2=CSurakartaDoc
Class3=CSurakartaView
Class4=CMainFrame

ResourceCount=3
Resource1=IDR_MAINFRAME
Resource2=IDD_DIALOG1
Class5=CAboutDlg
Class6=StartDlg
Class7=CNewGame
Resource3=IDD_DIALOG2

[CLS:CSurakartaApp]
Type=0
HeaderFile=Surakarta.h
ImplementationFile=Surakarta.cpp
Filter=N

[CLS:CSurakartaDoc]
Type=0
HeaderFile=SurakartaDoc.h
ImplementationFile=SurakartaDoc.cpp
Filter=N

[CLS:CSurakartaView]
Type=0
HeaderFile=SurakartaView.h
ImplementationFile=SurakartaView.cpp
Filter=C
BaseClass=CView
VirtualFilter=VWC
LastObject=CSurakartaView


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
LastObject=CMainFrame
BaseClass=CFrameWnd
VirtualFilter=fWC




[CLS:CAboutDlg]
Type=0
HeaderFile=Surakarta.cpp
ImplementationFile=Surakarta.cpp
Filter=D
LastObject=CAboutDlg

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_BEGIN
Command2=ID_REBEGIN
Command3=ID_BACK
Command4=ID_SET
CommandCount=4

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
CommandCount=14

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
CommandCount=8

[DLG:IDD_DIALOG1]
Type=1
Class=StartDlg
ControlCount=8
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_RED,button,1342308361
Control5=IDC_BLACK,button,1342177289
Control6=IDC_STATIC,static,1342308352
Control7=IDC_COMPUTER,button,1342308361
Control8=IDC_BRAIN,button,1342177289

[CLS:StartDlg]
Type=0
HeaderFile=StartDlg.h
ImplementationFile=StartDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=ID_APP_ABOUT

[DLG:IDD_DIALOG2]
Type=1
Class=CNewGame
ControlCount=7
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_LISTENGINE,listbox,1352728833
Control6=IDC_EDIT1,edit,1350631552
Control7=IDC_PLY,msctls_updown32,1342177334

[CLS:CNewGame]
Type=0
HeaderFile=NewGame.h
ImplementationFile=NewGame.cpp
BaseClass=CDialog
Filter=D
LastObject=CNewGame
VirtualFilter=dWC

