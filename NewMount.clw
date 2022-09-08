; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMountView
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "newmount.h"
LastPage=0

ClassCount=7
Class1=CChildFrame
Class2=CMainFrame
Class3=CMountDoc
Class4=CMountView
Class5=CMountApp
Class6=CAboutDlg
Class7=CProgressDlg

ResourceCount=5
Resource1=IDR_MAINFRAME
Resource2=IDD_ABOUTBOX
Resource3=IDR_MOUNTTYPE
Resource4=CG_IDD_PROGRESS_Monitor
Resource5=CG_IDD_PROGRESS

[CLS:CChildFrame]
Type=0
BaseClass=CMDIChildWnd
HeaderFile=ChildFrm.h
ImplementationFile=ChildFrm.cpp

[CLS:CMainFrame]
Type=0
BaseClass=CMDIFrameWnd
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp

[CLS:CMountDoc]
Type=0
BaseClass=CDocument
HeaderFile=MountDoc.h
ImplementationFile=MountDoc.cpp

[CLS:CMountView]
Type=0
BaseClass=CScrollView
HeaderFile=MountView.h
ImplementationFile=MountView.cpp
Filter=C
VirtualFilter=VWC
LastObject=ID_116_SR

[CLS:CMountApp]
Type=0
BaseClass=CWinApp
HeaderFile=NewMount.h
ImplementationFile=NewMount.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=NewMount.cpp
ImplementationFile=NewMount.cpp
LastObject=CAboutDlg

[CLS:CProgressDlg]
Type=0
BaseClass=CDialog
HeaderFile=ProgDlg.h
ImplementationFile=ProgDlg.cpp
Filter=D
VirtualFilter=dWC
LastObject=CG_IDC_PROGDLG_PERCENT

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

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
Command8=ID_TEST_SR
Command9=ID_APP_ABOUT
Command10=ID_CONTEXT_HELP
CommandCount=10

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_PRINT_SETUP
Command4=ID_FILE_MRU_FILE1
Command5=ID_APP_EXIT
Command6=ID_VIEW_TOOLBAR
Command7=ID_VIEW_STATUS_BAR
Command8=ID_HELP_FINDER
Command9=ID_APP_ABOUT
CommandCount=9

[MNU:IDR_MOUNTTYPE]
Type=1
Class=CMountView
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_CLOSE
Command4=ID_FILE_SAVE
Command5=ID_FILE_SAVE_AS
Command6=ID_FILE_PRINT
Command7=ID_FILE_PRINT_PREVIEW
Command8=ID_FILE_PRINT_SETUP
Command9=ID_FILE_MRU_FILE1
Command10=ID_APP_EXIT
Command11=ID_EDIT_UNDO
Command12=ID_EDIT_CUT
Command13=ID_EDIT_COPY
Command14=ID_EDIT_PASTE
Command15=ID_Test_Equation
Command16=ID_TEST_ShotPar
Command17=ID_TEST_SR
Command18=ID_TEST_SvSys
Command19=ID_Test_Survey
Command20=ID_TEST_FSTBRK
Command21=ID_Test_initial
Command22=ID_TEST_RESOLVE
Command23=ID_Test4_Project
Command24=ID_Test4_SvSys
Command25=ID_Test4_ShotPar
Command26=ID_Test4_ShotRecieve
Command27=ID_Test4_FstBrk
Command28=ID_Test4_Initial
Command29=ID_Test4_Resolve
Command30=ID_Test_Cac
Command31=ID_116_Pro
Command32=ID_116_Sys
Command33=ID_116_SR
Command34=ID_CALC_STATIC
Command35=ID_VIEW_TOOLBAR
Command36=ID_VIEW_STATUS_BAR
Command37=ID_WINDOW_NEW
Command38=ID_WINDOW_CASCADE
Command39=ID_WINDOW_TILE_HORZ
Command40=ID_WINDOW_ARRANGE
Command41=ID_HELP_FINDER
Command42=ID_APP_ABOUT
CommandCount=42

[ACL:IDR_MAINFRAME]
Type=1
Class=?
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
Command15=ID_CONTEXT_HELP
Command16=ID_HELP
CommandCount=16

[DLG:CG_IDD_PROGRESS_Monitor]
Type=1
Class=CProgressDlg
ControlCount=5
Control1=IDCANCEL,button,1342242817
Control2=CG_IDC_PROGDLG_PROGRESS,msctls_progress32,1342177281
Control3=CG_IDC_PROGDLG_PERCENT,static,1342308352
Control4=CG_IDC_PROGDLG_STATUS,static,1342308353
Control5=IDC_Monitor,edit,1353779396

[DLG:CG_IDD_PROGRESS]
Type=1
Class=?
ControlCount=5
Control1=IDCANCEL,button,1342242817
Control2=CG_IDC_PROGDLG_PROGRESS,msctls_progress32,1342177281
Control3=CG_IDC_PROGDLG_PERCENT,static,1342308352
Control4=CG_IDC_PROGDLG_STATUS,static,1342308353
Control5=IDC_Monitor,edit,1353779396

