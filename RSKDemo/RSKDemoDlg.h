
// RSKDemoDlg.h : header file
//

#pragma once


// CRSKDemoDlg dialog
#include "RSK_video_source.h"

using namespace agora::streaming;

#define START_SUCCESS WM_USER + 0x00000001
#define START_FAILURE WM_USER + 0x00000002
#define START_MEDIA_ERR WM_USER + 0x00000003
#define START_STATE_CHANGED WM_USER + 0x00000004
#define EXIT_VIDEO_SOURCE WM_USER + 0x00000005
class CRSKVideoSourceEventHandler : public IRSKVideoSourceEventHandler {
public:
	virtual ~CRSKVideoSourceEventHandler() {}

	virtual void onStartStreamingSuccess() override;
	virtual void onStartStreamingFailure(START_STREAMING_ERROR err, const char* msg) override;
	virtual void onMediaStreamingError(MEDIA_STREAMING_ERROR err, const char* msg) override;
	virtual void onStreamingConnectionStateChanged(STREAMING_CONNECTION_STATE state) override;
	virtual void onVideoSourceExit() override;

	HWND m_hwnd = NULL;
};

class CRSKDemoDlg : public CDialogEx
{
// Construction
public:
	CRSKDemoDlg(CWnd* pParent = nullptr);	// standard constructor
	~CRSKDemoDlg();
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RSKDEMO_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	std::unique_ptr<RSKVideoSourceSink> m_videoSourceSink;
	CRSKVideoSourceEventHandler m_eventHandler;
	int video_id = 10086;
	int audio_id = 10087;
	FILE* fpPcm = NULL;
	FILE* fpYuv = NULL;
	int width = 640;
	int height = 360;
	int video_file_size = 0;
	int video_size = 640 * 360 * 3 / 2;
	uint8_t* video_buffer = NULL;
	int read_video_size = 0;

	int audio_file_size = 0;
	int audio_size = 480 * 2 * 2;
	uint8_t* audio_buffer = NULL;
	
	int samplRate = 48000;
	int audio_channel = 2;
	int samplesPerChannel = 480;

	std::string publishurl = "";
	bool startSuccess = true;

	HANDLE hAudioTherad = NULL;

	static  unsigned __stdcall AudioThread(void*);
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	LRESULT OnEIDStartStreamingSuccess(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDStartStreamingFailure(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDMediaStreamingError(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDStreamingConnectionStateChanged(WPARAM wParam, LPARAM lParam);
	LRESULT OnEIDonVideoSourceExit(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedOk();
	CButton m_btnStart;
	afx_msg void OnBnClickedButtonStiop();
	CButton m_btnStopStreaming;
	CEdit m_edtUrl;
	CListBox m_lstInfo;
};
