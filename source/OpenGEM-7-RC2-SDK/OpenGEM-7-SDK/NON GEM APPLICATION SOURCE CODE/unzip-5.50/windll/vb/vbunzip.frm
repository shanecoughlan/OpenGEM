VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.1#0"; "comdlg32.ocx"
Begin VB.Form VBUnzFrm 
   AutoRedraw      =   -1  'True
   Caption         =   "VBUnzFrm"
   ClientHeight    =   4785
   ClientLeft      =   780
   ClientTop       =   525
   ClientWidth     =   9375
   BeginProperty Font 
      Name            =   "Fixedsys"
      Size            =   9
      Charset         =   0
      Weight          =   400
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   LinkTopic       =   "VBUnzFrm"
   ScaleHeight     =   4785
   ScaleWidth      =   9375
   StartUpPosition =   1  'Fenstermitte
   Begin VB.TextBox ZipFName 
      BeginProperty Font 
         Name            =   "Courier New"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   4440
      TabIndex        =   5
      Top             =   240
      Width           =   4335
   End
   Begin VB.TextBox ExtractRoot 
      BeginProperty Font 
         Name            =   "Courier New"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   4440
      TabIndex        =   4
      Top             =   960
      Width           =   4335
   End
   Begin VB.CommandButton StartUnz 
      Caption         =   "Start"
      Height          =   495
      Left            =   240
      TabIndex        =   3
      Top             =   1800
      Width           =   3255
   End
   Begin VB.TextBox MsgOut 
      BeginProperty Font 
         Name            =   "Courier New"
         Size            =   9
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   2175
      Left            =   240
      Locked          =   -1  'True
      MultiLine       =   -1  'True
      ScrollBars      =   3  'Beides
      TabIndex        =   2
      Top             =   2520
      Width           =   8895
   End
   Begin VB.CommandButton QuitVBUnz 
      Caption         =   "Quit"
      Height          =   495
      Left            =   6240
      TabIndex        =   1
      Top             =   1800
      Width           =   2895
   End
   Begin VB.CommandButton SearchZfile 
      Caption         =   "..."
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   8760
      TabIndex        =   0
      Top             =   240
      Width           =   375
   End
   Begin MSComDlg.CommonDialog CommonDialog1 
      Left            =   4800
      Top             =   1800
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   327681
   End
   Begin VB.Label Label1 
      Caption         =   "Complete path-name of Zip-archive:"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   240
      TabIndex        =   7
      Top             =   240
      Width           =   3855
   End
   Begin VB.Label Label2 
      Caption         =   "Extract archive into directory:"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   240
      TabIndex        =   6
      Top             =   960
      Width           =   3855
   End
End
Attribute VB_Name = "VBUnzFrm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

'---------------------------------------------------
'-- Please Do Not Remove These Comment Lines!
'----------------------------------------------------------------
'-- Sample VB 5 code to drive unzip32.dll
'-- Contributed to the Info-ZIP project by Mike Le Voi
'--
'-- Contact me at: mlevoi@modemss.brisnet.org.au
'--
'-- Visit my home page at: http://modemss.brisnet.org.au/~mlevoi
'--
'-- Use this code at your own risk. Nothing implied or warranted
'-- to work on your machine :-)
'----------------------------------------------------------------
'--
'-- This Source Code Is Freely Available From The Info-ZIP Project
'-- Web Server At:
'-- ftp://ftp.info-zip.org/pub/infozip/infozip.html
'--
'-- A Very Special Thanks To Mr. Mike Le Voi
'-- And Mr. Mike White
'-- And The Fine People Of The Info-ZIP Group
'-- For Letting Me Use And Modify Their Orginal
'-- Visual Basic 5.0 Code! Thank You Mike Le Voi.
'-- For Your Hard Work In Helping Me Get This To Work!!!
'---------------------------------------------------------------
'--
'-- Contributed To The Info-ZIP Project By Raymond L. King.
'-- Modified June 21, 1998
'-- By Raymond L. King
'-- Custom Software Designers
'--
'-- Contact Me At: king@ntplx.net
'-- ICQ 434355
'-- Or Visit Our Home Page At: http://www.ntplx.net/~king
'--
'---------------------------------------------------------------
'--
'-- Modified August 17, 1998
'-- by Christian Spieler
'-- (added sort of a "windows oriented" user interface)
'--
'---------------------------------------------------------------

Private Sub StartUnz_Click()

    Dim MsgTmp As String
    
    Cls
    MsgOut.Text = ""
    
    '-- Init Global Message Variables
    uZipInfo = ""
    uZipNumber = 0   ' Holds The Number Of Zip Files
    
    '-- Select UNZIP32.DLL Options - Change As Required!
    uPromptOverWrite = 1  ' 1 = Prompt To Overwrite
    uOverWriteFiles = 0   ' 1 = Always Overwrite Files
    uDisplayComment = 0   ' 1 = Display comment ONLY!!!
    
    '-- Change The Next Line To Do The Actual Unzip!
    uExtractList = 1       ' 1 = List Contents Of Zip 0 = Extract
    uHonorDirectories = 1  ' 1 = Honour Zip Directories
    
    '-- Select Filenames If Required
    '-- Or Just Select All Files
    uZipNames.uzFiles(0) = vbNullString
    uNumberFiles = 0
    
    '-- Select Filenames To Exclude From Processing
    ' Note UNIX convention!
    '   vbxnames.s(0) = "VBSYX/VBSYX.MID"
    '   vbxnames.s(1) = "VBSYX/VBSYX.SYX"
    '   numx = 2
    
    '-- Or Just Select All Files
    uExcludeNames.uzFiles(0) = vbNullString
    uNumberXFiles = 0
    
    '-- Change The Next 2 Lines As Required!
    '-- These Should Point To Your Directory
    uZipFileName = ZipFName.Text
    uExtractDir = ExtractRoot.Text
    If uExtractDir <> "" Then uExtractList = 0 ' unzip if dir specified
    
    
    '-- Let's Go And Unzip Them!
    Call VBUnZip32
    
    '-- Tell The User What Happened
    If Len(uZipMessage) > 0 Then
        MsgTmp = uZipMessage
    End If
    
    '-- Display Zip File Information.
    If Len(uZipInfo) > 0 Then
        MsgTmp = MsgTmp & vbNewLine & "uZipInfo is:" & vbNewLine & uZipInfo
    End If
    
    '-- Display The Number Of Extracted Files!
    If uZipNumber > 0 Then
        MsgTmp = MsgTmp & vbNewLine & "Number Of Files: " & Str(uZipNumber)
    End If
    
    MsgOut.Text = MsgOut.Text & MsgTmp & vbNewLine
    
    
End Sub


Private Sub Form_Load()
    
    ZipFName.Text = vbNullString
    ExtractRoot.Text = vbNullString
    Me.Show
    
End Sub


Private Sub QuitVBUnz_Click()
    Unload Me
End Sub


Private Sub SearchZfile_Click()
    CommonDialog1.DialogTitle = "Open Zip-archive"
    CommonDialog1.DefaultExt = ".zip"
    CommonDialog1.ShowOpen
    ZipFName.Text = CommonDialog1.filename
End Sub

