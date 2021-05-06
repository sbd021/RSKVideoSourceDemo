
// RSKDemoDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "RSKDemo.h"
#include "RSKDemoDlg.h"
#include "afxdialogex.h"
#include "node_log.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CRSKDemoDlg dialog
void CRSKVideoSourceEventHandler::onStartStreamingSuccess()
{
	::PostMessage(m_hwnd, START_SUCCESS, 0, 0);
}
	 
void CRSKVideoSourceEventHandler::onStartStreamingFailure(START_STREAMING_ERROR err, const char* msg)
{
	::PostMessage(m_hwnd, START_FAILURE, err, 0);
}
	 
void CRSKVideoSourceEventHandler::onMediaStreamingError(MEDIA_STREAMING_ERROR err, const char* msg)
{
	::PostMessage(m_hwnd, START_MEDIA_ERR, err, 0);
}
	
void CRSKVideoSourceEventHandler::onStreamingConnectionStateChanged(STREAMING_CONNECTION_STATE state)
{
	::PostMessage(m_hwnd, START_STATE_CHANGED, state, 0);
}

void CRSKVideoSourceEventHandler::onVideoSourceExit()
{
	::PostMessage(m_hwnd, EXIT_VIDEO_SOURCE, 0, 0);


}


LRESULT CRSKDemoDlg::OnEIDStartStreamingSuccess(WPARAM wParam, LPARAM lParam)
{
	m_lstInfo.InsertString(m_lstInfo.GetCount(), L"onStartStreamingSuccess");
	SetTimer(video_id, 100, NULL);
	
	hAudioTherad = (HANDLE)_beginthreadex(NULL, 0, AudioThread, this, 0, NULL);
	//SetTimer(audio_id, 100, NULL);
	startSuccess = true;
	return 0;
}
LRESULT CRSKDemoDlg::OnEIDStartStreamingFailure(WPARAM wParam, LPARAM lParam)
{
	CString strInfo;
	strInfo.Format(_T("onStartStreamingFailure: err:%d, msg:%s\n"), wParam, lParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	return 0;
}
LRESULT CRSKDemoDlg::OnEIDMediaStreamingError(WPARAM wParam, LPARAM lParam)
{
	CString strInfo;
	strInfo.Format(_T("onMediaStreamingError: err:%d, msg:%s\n"),wParam);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);

	return 0;
}
LRESULT CRSKDemoDlg::OnEIDStreamingConnectionStateChanged(WPARAM wParam, LPARAM lParam)
{
	STREAMING_CONNECTION_STATE state = (STREAMING_CONNECTION_STATE)wParam;
	CString str;
	switch (state)
	{
	case agora::streaming::STREAMING_CONNECTION_STATE_DISCONNECTED:
		str = _T("STREAMING_CONNECTION_STATE_DISCONNECTED");
		break;
	case agora::streaming::STREAMING_CONNECTION_STATE_CONNECTED:
		str = _T("STREAMING_CONNECTION_STATE_CONNECTED");
		break;
	case agora::streaming::STREAMING_CONNECTION_STATE_RECONNECTING:
		str = _T("STREAMING_CONNECTION_STATE_RECONNECTING");
		break;
	case agora::streaming::STREAMING_CONNECTION_STATE_FAILED:
		str = _T("STREAMING_CONNECTION_STATE_FAILED");
		break;
	default:
		break;
	}
	CString strInfo;
	strInfo.Format(_T("onStreamingConnectionStateChanged: state:\n"));
	m_lstInfo.InsertString(m_lstInfo.GetCount(), strInfo);
	m_lstInfo.InsertString(m_lstInfo.GetCount(), str);
	return 0;
}
LRESULT CRSKDemoDlg::OnEIDonVideoSourceExit(WPARAM wParam, LPARAM lParam)
{
	m_lstInfo.InsertString(m_lstInfo.GetCount(), L"VideoSource.exe exit");
	return 0;
}
CRSKDemoDlg::CRSKDemoDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RSKDEMO_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_videoSourceSink.reset(new RSKVideoSourceSink);
}

CRSKDemoDlg::~CRSKDemoDlg()
{
	stopLogService();
}

void CRSKDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btnStart);
	DDX_Control(pDX, IDC_BUTTON_STIOP, m_btnStopStreaming);
	DDX_Control(pDX, IDC_EDIT_URL, m_edtUrl);
	DDX_Control(pDX, IDC_LIST_INFO, m_lstInfo);
}

BEGIN_MESSAGE_MAP(CRSKDemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CRSKDemoDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_STIOP, &CRSKDemoDlg::OnBnClickedButtonStiop)
	ON_MESSAGE(START_SUCCESS, &CRSKDemoDlg::OnEIDStartStreamingSuccess)
	ON_MESSAGE(START_FAILURE, &CRSKDemoDlg::OnEIDStartStreamingFailure)
	ON_MESSAGE(START_MEDIA_ERR, &CRSKDemoDlg::OnEIDMediaStreamingError)
	ON_MESSAGE(EXIT_VIDEO_SOURCE, &CRSKDemoDlg::OnEIDonVideoSourceExit)

	
	ON_MESSAGE(START_STATE_CHANGED, &CRSKDemoDlg::OnEIDStreamingConnectionStateChanged)
END_MESSAGE_MAP()


// CRSKDemoDlg message handlers

BOOL CRSKDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	std::string currentPath;
	INodeProcess::getCurrentModuleFileName(currentPath);
	std::string logFile = currentPath + ".txt";
	startLogService(logFile.c_str());

	m_btnStopStreaming.EnableWindow(FALSE);
	m_edtUrl.SetWindowText(_T("rtmp://ks-push.broadcastapp.agoraio.cn/live/agoratool"));
	// Add "About..." menu item to system menu.
	CString strUrl;
	m_edtUrl.GetWindowText(strUrl);
	CHAR szUrl[MAX_PATH];
	::WideCharToMultiByte(CP_ACP, 0, strUrl.GetBuffer(0), -1, szUrl, MAX_PATH, NULL, NULL);
	publishurl = szUrl;

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	
	TCHAR	szPath[MAX_PATH];
	::GetModuleFileName(NULL, szPath, MAX_PATH);
	LPTSTR lpLastSlash = (LPTSTR)_tcsrchr(szPath, _T('\\')) + 1;
	_tcscpy_s(lpLastSlash, 16, _T("test.yuv"));
	_tfopen_s(&fpYuv, szPath, L"rb");
	_tcscpy_s(lpLastSlash, 16, _T("test.pcm"));
	_tfopen_s(&fpPcm, szPath, L"rb");
	

	fseek(fpYuv, 0, SEEK_END);
	video_file_size = ftell(fpYuv);
	fseek(fpYuv, 0, SEEK_SET);


	fseek(fpPcm, 0, SEEK_END);
	audio_file_size = ftell(fpPcm);
	fseek(fpPcm, 0, SEEK_SET);

	video_size = width * height * 3 / 2;
	video_buffer = new uint8_t[video_size];
	memset(video_buffer, 0, video_size);

	audio_size = (samplRate / 100) * audio_channel * 2;
	audio_buffer = new uint8_t[audio_size];
	memset(audio_buffer, 0, audio_size);
	samplesPerChannel = samplRate / 100;

	m_eventHandler.m_hwnd = m_hWnd;
	m_videoSourceSink->initialize(&m_eventHandler, "aab8b8f5a8cd4469a63042fcfafe7063");

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRSKDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRSKDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRSKDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CRSKDemoDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == video_id) {
		int bytes = fread(video_buffer, 1, video_size, fpYuv);
		read_video_size += bytes;
		if (read_video_size  == video_file_size) {
			fseek(fpYuv, 0, SEEK_SET);
			read_video_size = 0;
		}
		m_videoSourceSink->pushExternalVideoFrame(video_buffer, width, height);
	}
	else if (nIDEvent == audio_id) {
	
	}
	CDialogEx::OnTimer(nIDEvent);
}
unsigned __stdcall CRSKDemoDlg::AudioThread(void* param)
{
	int read_audio_size = 0;
	CRSKDemoDlg* pThis = (CRSKDemoDlg*)param;
	while (true) {
		int audio_bytes = fread(pThis->audio_buffer, 1, pThis->audio_size, pThis->fpPcm);
		read_audio_size += audio_bytes;
		if (read_audio_size == pThis->audio_file_size) {
			fseek(pThis->fpPcm, 0, SEEK_SET);
			read_audio_size = 0;
		}
		AudioFrameIpcHeader audioHeader = { pThis->audio_channel, pThis->samplRate, pThis->samplesPerChannel, pThis->audio_size };
		pThis->m_videoSourceSink->pushExternalAudioFrame(pThis->audio_buffer, audioHeader);
		Sleep(10);
	}
}

void CRSKDemoDlg::OnBnClickedOk()
{

	// default maximum parameter = {1920, 1080}, if not setMaxExternalVideoFrameParameter
	// If maximum width > 1920, height > 1080, setMaxExternalVideoFrameParameter before startStreaming
    VideoFrameIpcHeader videoParam = { 1920, 1080 };
	m_videoSourceSink->setMaxExternalVideoFrameParameter(videoParam);

	// default parameter = {1920, 1080}, if not setMaxExternalVideoFrameParameter
	// If maximum samples > 480, sampleRate > 48000, setMaxExternalAudioFrameParameter before startStreaming
	AudioFrameIpcHeader audioParam = { 2, 48000, 480, 480 * 2 * 2 }; //channel  samplerate  samples size
	m_videoSourceSink->setMaxExternalAudioFrameParameter(audioParam);

	m_videoSourceSink->startStreaming(publishurl.c_str());
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("startStreaming"));
	m_btnStart.EnableWindow(FALSE);
	m_btnStopStreaming.EnableWindow(TRUE);
	// TODO: Add your control notification handler code here
	//CDialogEx::OnOK();
}


void CRSKDemoDlg::OnBnClickedButtonStiop()
{
	if (startSuccess) {
		KillTimer(audio_id);
		KillTimer(video_id);
		startSuccess = false;
	}
	m_videoSourceSink->stopStreaming();
	m_lstInfo.InsertString(m_lstInfo.GetCount(), _T("stopStreaming"));
	m_btnStopStreaming.EnableWindow(FALSE);
	m_btnStart.EnableWindow(TRUE);
}
