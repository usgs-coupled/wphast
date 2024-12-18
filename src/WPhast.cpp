// WPhast.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "WPhast.h"
#include "MainFrm.h"

#include "WPhastDoc.h"
#include "WPhastView.h"

#include "NewModelWizard.h"
#include "StartupDialog.h"

#include "NewModel.h"
#include "DelayRedraw.h"

#include "RunTypeDlg.h"

#include <atlpath.h> // ATL::ATLPath::FileExists

#include "Version.h"

#if defined(_DEBUG)
// see CWPhastApp::InitInstance()
#include <../atlmfc/src/atl/atls/Common.h>
#include <../atlmfc/src/atl/atls/Allocate.h>
#include <../atlmfc/src/atl/atls/AtlTraceModuleManager.h>
#endif

#if defined(__CPPUNIT__)
#include "TCPPUnit.h"
#endif

#include "afxcmn.h"
#include "afxwin.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CWPhastApp

BEGIN_MESSAGE_MAP(CWPhastApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// Standard file based document commands
	//ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_TOOLS_RUNOPTIONS, &CWPhastApp::OnToolsRunOptions)
END_MESSAGE_MAP()

// CWPhastApp construction

CWPhastApp::CWPhastApp()
: m_bShellFileNew(FALSE)
, pRecentDBFileList(0)
{
	// add construction code here,
	// Place all significant initialization in InitInstance
	size_t n;
	ASSERT(sizeof(n) == 8);

	n = H5Tget_size(H5T_NATIVE_UINT);
	ASSERT(n == 4);

	DWORD dword;
	n = sizeof(dword);
	ASSERT(n == 4);

	n = H5Tget_size(H5T_NATIVE_ULONG);
	ASSERT(n == 4);

	n = H5Tget_size(H5T_NATIVE_ULLONG);
	ASSERT(n == 8);

	ASSERT(sizeof(COLORREF) <= H5Tget_size(H5T_NATIVE_ULONG));
}

CWPhastApp::~CWPhastApp()
{
	ASSERT(this->pRecentDBFileList == NULL);
}

// The one and only CWPhastApp object

CWPhastApp theApp;

#if defined(WPHAST_AUTOMATION)
// This identifier was generated to be statistically unique for your app
// You may change it if you prefer to choose a specific identifier

// {36353903-2137-43FD-9AD6-40B65A96A839}
static const CLSID clsid =
{ 0x36353903, 0x2137, 0x43FD, { 0x9A, 0xD6, 0x40, 0xB6, 0x5A, 0x96, 0xA8, 0x39 } };
const GUID CDECL BASED_CODE _tlid =
		{ 0xD2FD46F3, 0xCD0, 0x4432, { 0xB8, 0x80, 0x72, 0xEC, 0xBE, 0x57, 0xD7, 0xD4 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;
#endif


// CWPhastApp initialization

BOOL CWPhastApp::InitInstance()
{
	// get path "C:\Program Files (x86)\USGS\Phast for Windows 1.0.1\bin\P4W.exe"
	TCHAR szPath[2*MAX_PATH+100];
	::GetModuleFileName(NULL, szPath, 2*MAX_PATH+100);

	// split path "C:\Program Files (x86)\USGS\Phast for Windows 1.0.1\bin\P4W.exe"
	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	TCHAR szFName[_MAX_FNAME];
	TCHAR szExt[_MAX_EXT];
	VERIFY(::_tsplitpath_s(szPath, szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szFName, _MAX_FNAME, szExt, _MAX_EXT) == 0);

	if (!ATL::ATLPath::IsUNC(szPath))
	{
		// capitalize drive
		if (szDrive[0]) szDrive[0] = toupper(szDrive[0]);
	}

	// trim off P4W.exe "C:\Program Files (x86)\USGS\Phast for Windows 1.0.1\bin"
	*(_tcsrchr(szDir, _TEXT('\\'))) = 0; // truncate EXE filename

	// trim off bin "C:\Program Files (x86)\USGS\Phast for Windows 1.0.1"
	*(_tcsrchr(szDir, _TEXT('\\'))) = 0; // truncate EXE filename

	// makepath "C:\Program Files (x86)\USGS\Phast for Windows 1.0.1"
	VERIFY(::_tmakepath_s(szPath, _MAX_DIR, szDrive, szDir, NULL, NULL) == 0);
	PathRemoveBackslash(szPath);

	VERIFY(CGlobal::SetEnvironmentVariableSafe(_T("APPLICATIONFOLDER"), szPath));
	VERIFY(CGlobal::SetEnvironmentVariableSafe(_T("INSTALLDIR"), szPath));
	VERIFY(CGlobal::SetEnvironmentVariableSafe(_T("P4W_TOPDIR"), szPath));
	VERIFY(CGlobal::SetEnvironmentVariableSafe(_T("P4W_INSTALLDIR"), szPath));

#if defined(__CPPUNIT__)
	{
// COMMENT: {12/1/2010 7:33:18 PM}		// turn off memory tracking
// COMMENT: {12/1/2010 7:33:18 PM}		BOOL prev = ::AfxEnableMemoryTracking(FALSE);
// COMMENT: {12/1/2010 7:33:18 PM}		int prevafxMemDF = ::afxMemDF;
// COMMENT: {12/1/2010 7:33:18 PM}		::afxMemDF = 0;

		TCPPUnit theTests;  // TCPPUnit must be constructed after/within theApp.InitInstance()
		return FALSE;
		// CWPhastApp anotherApp; // this will ASSERT
		/***
		CMapDialog dlg;
		dlg.SetFileName("ok1.bmp");
		dlg.SetWorldFileName("ok1.bpw");
		dlg.DoModal();
		return FALSE;
		***/
// COMMENT: {11/26/2008 4:44:28 PM}		int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
// COMMENT: {11/26/2008 4:44:28 PM}		tmpDbgFlag |= _CRTDBG_CHECK_ALWAYS_DF;
// COMMENT: {11/26/2008 4:44:28 PM}		_CrtSetDbgFlag(tmpDbgFlag);

// COMMENT: {8/21/2009 4:29:28 PM}		// set this to avoid warning display window
// COMMENT: {8/21/2009 4:29:28 PM}		// 
// COMMENT: {8/21/2009 4:29:28 PM}		vtkObject::SetGlobalWarningDisplay(0);
	}
#endif
#if defined(_DEBUG)
	// this treats
	// TRACE("CWPhastDoc::DeleteContents()\n");
	// as
	// TRACE("%s(%d) : CWPhastDoc::DeleteContents()\n", THIS_FILE, __LINE__);
	//
	// CHECKOUT => TRACE(traceAppMsg, 0, _T("Warning: could not parse the path '%s'.\n"), "TESTING");
	///g_Allocator.GetProcess()->m_bFileNameAndLineNo = true;
#endif

#if !defined(_DEBUG)
		// set this to avoid warning display window
		// 
		vtkObject::SetGlobalWarningDisplay(0);
#endif


	// Check requirements
	// ie  Version 4.71 and later of Shlwapi.dll

	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	//{{
	// For richedit in dialogs
	::AfxInitRichEdit2();
	//}}

	// Initialize OLE libraries
// COMMENT: {4/27/2005 4:19:12 PM}	if (!AfxOleInit())
// COMMENT: {4/27/2005 4:19:12 PM}	{
// COMMENT: {4/27/2005 4:19:12 PM}		AfxMessageBox(IDP_OLE_INIT_FAILED);
// COMMENT: {4/27/2005 4:19:12 PM}		return FALSE;
// COMMENT: {4/27/2005 4:19:12 PM}	}
	//{{
	// HACK
    _AFX_THREAD_STATE* pState = AfxGetThreadState();
    if (!pState->m_bNeedTerm && !AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	//}}


	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
#if defined(_DEBUG)
	SetRegistryKey(_T("USGS_DEBUG"));

	// must set after calling SetRegistryKey
	BOOL bEnable = AfxEnableMemoryTracking(FALSE);
	free((void*)m_pszProfileName);
	m_pszProfileName = _tcsdup(_T("Phast for Windows\\1.0"));
	AfxEnableMemoryTracking(bEnable);
#else
	SetRegistryKey(_T("USGS"));

	// must set after calling SetRegistryKey
#pragma push_macro("free")
#pragma warning(disable:4005)
#define free free
	BOOL bEnable = AfxEnableMemoryTracking(FALSE);
	free((void*)m_pszProfileName);
#ifdef _WIN64
	m_pszProfileName = _tcsdup(_T("Phast for Windows x64\\")
		_T(APR_STRINGIFY(VER_MAJOR))
		_T(".") _T(APR_STRINGIFY(VER_MINOR)));
#else
	m_pszProfileName = _tcsdup(_T("Phast for Windows\\")
		_T(APR_STRINGIFY(VER_MAJOR))
		_T(".") _T(APR_STRINGIFY(VER_MINOR)));
#endif
	AfxEnableMemoryTracking(bEnable);
#pragma warning(default:4005)
#pragma pop_macro("free")
#endif
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	LoadMoreProfileSettings();  // Load run options

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME_SDI,
		RUNTIME_CLASS(CWPhastDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CWPhastView));
	AddDocTemplate(pDocTemplate);
#if defined(WPHAST_AUTOMATION)
	// Connect the COleTemplateServer to the document template
	//  The COleTemplateServer creates new documents on behalf
	//  of requesting OLE containers by using information
	//  specified in the document template
	m_server.ConnectTemplate(clsid, pDocTemplate, FALSE);
	// Register all OLE server factories as running.  This enables the
	//  OLE libraries to create objects from other applications
	COleTemplateServer::RegisterAll();
		// Note: MDI applications register all server objects without regard
		//  to the /Embedding or /Automation on the command line
#endif
#ifdef RUN_AS_MDI
	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;
	// call DragAcceptFiles only if there's a suffix
	//  In an MDI app, this should occur immediately after setting m_pMainWnd
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();
#endif
	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
#if defined(WPHAST_AUTOMATION)
	// App was launched with /Embedding or /Automation switch.
	// Run app as automation server.
	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
	{
		// Register all OLE server factories as running.  This enables the
		//  OLE libraries to create objects from other applications
		COleTemplateServer::RegisterAll();

		// Don't show the main window
		return TRUE;
	}
	// App was launched with /Unregserver or /Unregister switch.  Unregister
	// typelibrary.  Other unregistration occurs in ProcessShellCommand().
	else if (cmdInfo.m_nShellCommand == CCommandLineInfo::AppUnregister)
	{
		m_server.UpdateRegistry(OAT_DISPATCH_OBJECT, NULL, NULL, FALSE);
		AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor);
	}
	// App was launched standalone or with other switches (e.g. /Register
	// or /Regserver).  Update registry entries, including typelibrary.
	else
	{
		m_server.UpdateRegistry(OAT_DISPATCH_OBJECT);
		COleObjectFactory::UpdateRegistryAll();
		AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid);
	}
#endif
	////{{SRC
	//DebugBreak();
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew)
	{
		this->m_bShellFileNew = TRUE;
        if (!AfxGetApp()->OnCmdMsg(ID_FILE_NEW, 0, NULL, NULL))
			OnFileNew();
		if (m_pMainWnd == NULL)
			return FALSE;
	}
	else
	{
	//}}SRC
		// Dispatch commands specified on the command line.  Will return FALSE if
		// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
		if (!ProcessShellCommand(cmdInfo))
			return FALSE;
	//{{SRC
	}

#if !defined(_USE_DEFAULT_MENUS_)
	CMenu* pMenu = m_pMainWnd->GetMenu();
	if (pMenu) pMenu->DestroyMenu();
	HMENU hMenu = ((CMainFrame*)m_pMainWnd)->NewMenu();
	pMenu = CMenu::FromHandle(hMenu);
	m_pMainWnd->SetMenu(pMenu);
	((CMainFrame*)m_pMainWnd)->m_hMenuDefault = hMenu;
#endif

	//}}SRC
	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	// force the view to redraw
	CFrameWnd *pFrame = (CFrameWnd*)::AfxGetApp()->m_pMainWnd;
	pFrame->GetActiveView()->Invalidate();

	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	return TRUE;
}



// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	CString VTKLicense;
	CStatic VTKStatic;
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
, VTKLicense(_T("VTK is an open-source toolkit licensed under the BSD license.\n\nCopyright (c) 1993-2008 Ken Martin, Will Schroeder, Bill Lorensen\nAll rights reserved.\n\nRedistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:\n\n  �  Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.\n  �  Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.\n  �  Neither name of Ken Martin, Will Schroeder, or Bill Lorensen nor the names of any contributors may be used to endorse or promote products derived from this software without specific prior written permission.\n\nTHIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."))
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VTK, VTKStatic);
	if (!pDX->m_bSaveAndValidate)
	{
		this->VTKStatic.SetWindowTextA(this->VTKLicense);
	}
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void CWPhastApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CWPhastApp message handlers


void CWPhastApp::OnFileNew()
{
	if (this->m_pMainWnd && ::IsWindow(this->m_pMainWnd->m_hWnd))
	{
		this->m_pMainWnd->Invalidate(TRUE);
	}
	if (m_pDocManager != NULL)
	{
		POSITION pos = m_pDocManager->GetFirstDocTemplatePosition();
		if (pos == NULL)
		{
			TRACE(traceAppMsg, 0, "Error: no document templates registered with CWinApp.\n");
			AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
			return;
		}
		CDocTemplate* pTemplate = m_pDocManager->GetNextDocTemplate(pos);
		ASSERT(pTemplate != NULL);
		ASSERT_KINDOF(CDocTemplate, pTemplate);

		CWPhastDoc* pDoc = static_cast<CWPhastDoc*> (pTemplate->OpenDocumentFile(NULL));

		// The main window has been initialized, so show and update it
		m_pMainWnd->ShowWindow(SW_SHOW);
		m_pMainWnd->UpdateWindow();

		if (pDoc)
		{
			CWnd *pWndTreeCtrl = 0;
			if (CPropertyTreeControlBar *pPropertyTreeControlBar = pDoc->GetPropertyTreeControlBar())
			{
				pWndTreeCtrl = pPropertyTreeControlBar->GetTreeCtrl();
			}

			//
			// 2/14/2011
			// It'd be nice if all the CDelayRedraws were here, but for some reason
			// Serialize needs to have them when loading
			//

			if (this->m_bShellFileNew)
			{
				this->m_bShellFileNew = FALSE;
				CStartupDialog dlg;
				dlg.DoModal();
				if (dlg.GetAction() == CStartupDialog::SDA_OPEN_FILE)
				{
					pDoc->SetModifiedFlag(FALSE);
					pDoc = static_cast<CWPhastDoc*> (pTemplate->OpenDocumentFile(dlg.GetPathName()));
					return;
				}
				else if (dlg.GetAction() == CStartupDialog::SDA_CREATE_DEFAULT)
				{
					CDelayRedraw tree(pWndTreeCtrl);
					CDelayRedraw bar(pWndTreeCtrl ? pWndTreeCtrl->GetParent() : 0);
					CDelayRedraw box(pDoc->GetBoxPropertiesDialogBar());
					CDelayRedraw mainwnd(0, pDoc);

					pDoc->New(CNewModel::Default());
					pDoc->SetModifiedFlag(FALSE);
					pDoc->ExecutePipeline();
					return;
				}
				else if (dlg.GetAction() == CStartupDialog::SDA_IMPORT_FILE)
				{
					CDelayRedraw tree(pWndTreeCtrl);
					CDelayRedraw bar(pWndTreeCtrl ? pWndTreeCtrl->GetParent() : 0);
					CDelayRedraw box(pDoc->GetBoxPropertiesDialogBar());
					CDelayRedraw mainwnd(0, pDoc);

					pDoc->DoImport(dlg.GetPathName());
					pDoc->ExecutePipeline();
					return;
				}
			}

			CDelayRedraw tree(pWndTreeCtrl);
			CDelayRedraw bar(pWndTreeCtrl ? pWndTreeCtrl->GetParent() : 0);
			CDelayRedraw box(pDoc->GetBoxPropertiesDialogBar());
			CDelayRedraw mainwnd(0, pDoc);

			CBitmap bmpWatermark;
			CBitmap bmpHeader;
			VERIFY(bmpWatermark.LoadBitmap(IDB_WATERMARK));
			VERIFY(bmpHeader.LoadBitmap(IDB_BANNER));

			CNewModelWizard wizard(_T("New Model Wizard"), this->m_pMainWnd, 0, bmpWatermark, 0, bmpHeader);

			wizard.SetGridElt(CGridElt::NewDefaults(false));
			wizard.SetHeadIC(CHeadIC::NewDefaults());
			wizard.SetChemIC(CChemIC::NewDefaults());

			if (wizard.DoModal() == ID_WIZFINISH)
			{
				CNewModel model = wizard.GetModel();
				pDoc->New(model);
				pDoc->ExecutePipeline();
			}
			else
			{
				pDoc->New(CNewModel::Default());
				pDoc->SetModifiedFlag(FALSE);
				pDoc->ExecutePipeline();
			}
		}
	}
}

INT_PTR CWPhastApp::RunTypeDlgDoModal()
{
	CRunTypeDlg dlg;
	dlg.pRecentDBFileList = this->pRecentDBFileList;
	dlg.strDatabase       = this->settings.strDatabase;
	dlg.bRunMPI           = this->settings.bRunMPI;
	dlg.strMPICommand     = this->settings.strMPICommand;
	dlg.strMTCommand      = this->settings.strMTCommand;
	dlg.strCommandArgs    = this->settings.strCommandArgs;
	dlg.nThreads          = this->settings.nThreads;

	INT_PTR rval = dlg.DoModal();
	if (rval == IDOK)
	{
		this->settings.strDatabase  = dlg.strDatabase;
		if (this->pRecentDBFileList)
		{
			this->pRecentDBFileList->Add(dlg.strDatabase);
			this->pRecentDBFileList->WriteList();
		}

		this->settings.bRunMPI = dlg.bRunMPI;

		WriteProfileString(_T("Settings"), _T("Database"), this->settings.strDatabase);
		WriteProfileInt(_T("Settings"),    _T("RunMPI"),   this->settings.bRunMPI);

		if (dlg.bRunMPI)
		{
			this->settings.strMPICommand  = dlg.strMPICommand;
			this->settings.strCommandArgs = dlg.strCommandArgs;

			WriteProfileString(_T("Settings"), _T("RunMPICommand"),  this->settings.strMPICommand);
			WriteProfileString(_T("Settings"), _T("RunCommandArgs"), this->settings.strCommandArgs);
		}
		else
		{
			this->settings.strMTCommand = dlg.strMTCommand;
			this->settings.nThreads     = dlg.nThreads;

			WriteProfileString(_T("Settings"), _T("RunMTCommand"), this->settings.strMTCommand);
			WriteProfileInt(_T("Settings"),    _T("Threads"),      this->settings.nThreads);
		}
	}
	return rval;
}

void CWPhastApp::OnToolsRunOptions()
{
	this->RunTypeDlgDoModal();
}

BOOL CWPhastApp::LoadMoreProfileSettings(void)
{
	ASSERT_VALID(this);

	// build phast-msmpi cmd
	TCHAR szCmd[2*MAX_PATH+100];
	::GetModuleFileName(NULL, szCmd, MAX_PATH);
	*(_tcsrchr(szCmd, _TEXT('\\')) + 1) = 0;
	_stprintf(_tcschr(szCmd, 0), _T("phast-msmpi.exe"));

	// build mpiexec args
	TCHAR szCmdArgs[2*MAX_PATH+100];
	//_stprintf(szCmdArgs, _T("%s %s"), _T("-localonly %NUMBER_OF_PROCESSORS%"), szCmd);


	if (this->pRecentDBFileList == 0)
	{
		// create file MRU since nMaxMRU not zero
		this->pRecentDBFileList = new CRecentFileList2(0,
			_T("MRU DB"), _T("DB%d"),
			10);
		this->pRecentDBFileList->ReadList();

		if ((*this->pRecentDBFileList)[0].IsEmpty())
		{
			TCHAR szPath[2*MAX_PATH+100];
			::ExpandEnvironmentStrings(_T("%INSTALLDIR%\\database\\phast.dat"), szPath, 2*MAX_PATH+100);
			if (ATL::ATLPath::FileExists(szPath))
			{
				this->pRecentDBFileList->Add(_T("%INSTALLDIR%\\database\\phast.dat"));
			}
		}
	}

	SYSTEM_INFO sysinfo;
	::GetSystemInfo(&sysinfo);
	int threads = (int) sysinfo.dwNumberOfProcessors;

#ifdef _WIN64
	_stprintf(szCmdArgs, _T("-n %d %s"), threads, _T("\"%INSTALLDIR%\\bin\\phast-msmpi.exe\""));
#else
	_stprintf(szCmdArgs, _T("-n %d %s"), threads, _T("\"%INSTALLDIR%\\bin64\\phast-msmpi.exe\""));
#endif

	// Get default database from registry
	this->settings.strDatabase    = (this->pRecentDBFileList->GetSize()) ? (*this->pRecentDBFileList)[0] : _T("%INSTALLDIR%\\database\\phast.dat");
	if (this->settings.strDatabase.IsEmpty())
	{
		this->settings.strDatabase = _T("%INSTALLDIR%\\database\\phast.dat");
	}
	
	this->settings.bRunMPI        = GetProfileInt   (_T("Settings"), _T("RunMPI"),          0                                             );
	this->settings.nThreads       = GetProfileInt   (_T("Settings"), _T("Threads"),         threads                                       );
	this->settings.strMPICommand  = GetProfileString(_T("Settings"), _T("RunMPICommand"),   _T("\"%MSMPI_BIN%\\mpiexec.exe\"")            );
#ifdef _WIN64
	this->settings.strMTCommand   = GetProfileString(_T("Settings"), _T("RunMTCommand"),    _T("\"%INSTALLDIR%\\bin\\phast-mt.exe\"")   );
#else
	this->settings.strMTCommand   = GetProfileString(_T("Settings"), _T("RunMTCommand"),    _T("\"%INSTALLDIR%\\bin64\\phast-mt.exe\"")   );
#endif
	this->settings.strCommandArgs = GetProfileString(_T("Settings"), _T("RunCommandArgs"),  szCmdArgs                                     );

	return TRUE;
}

int CWPhastApp::ExitInstance()
{
	// Add your specialized code here and/or call the base class

	// Save database MRU list
	if (this->pRecentDBFileList != NULL)
	{
		// written on each run (see CWPhastApp::RunTypeDlgDoModal)
		delete this->pRecentDBFileList;
		this->pRecentDBFileList = NULL;
	}

	::OutputDebugString("CWPhastApp: About to call CWinApp::ExitInstance\n");

	return CWinApp::ExitInstance();
}
