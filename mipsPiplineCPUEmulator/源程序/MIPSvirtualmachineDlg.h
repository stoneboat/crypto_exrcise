
// MIPSvirtualmachineDlg.h : ͷ�ļ�
//

#pragma once
#include "EditableListCtrl.h"
#include "MIPSSetup.h"
#include <boost\thread\shared_mutex.hpp>
#include <boost\thread\mutex.hpp>
#include <boost\thread.hpp>
#include <boost\thread\condition.hpp>
#include <map>
#include <string>
#include "afxcmn.h"
using namespace MipsVirtual;
// CMIPSvirtualmachineDlg �Ի���
class CMIPSvirtualmachineDlg : public CDialogEx
{
// ����
public:
	CMIPSvirtualmachineDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MIPSVIRTUALMACHINE_DIALOG };
#endif
public:
	CEditableListCtrl dataWindows;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	void OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult);
	LRESULT OnNotifyDescriptionEdited(WPARAM wParam, LPARAM lParam);
	CPoint InterviewListCursorPosition() const;
private:
	MipsVirtual::mipsVM vm;
	const wchar_t* instructionString;
	const wchar_t* registerStatusString;
	const wchar_t* workHubLabelString;
	const wchar_t* pcStatusString;
	const wchar_t* TimeSlide[timeSlideNum];
	const int pipeWorkTime = 1;
	const int displayingTime = 0;// 350;
	boost::mutex terminalVM_mutex;
	void VMRunning();
	void InitialPiplLineHardeWare();
	void TerminalPiplLineHardWare();
	void OnePipeLineClock();
	/****************��ˮ�߿����ź�*************************/
	std::map<std::string, int> cpuPipeLine_map;
	std::string cpuPipeLineName[5] = { "IF","ID","EX","MEM","WB" };
	boost::thread* PipeLineHadware[5];
	boost::shared_mutex inerClock_mutex;
	boost::unique_lock<boost::shared_mutex> lock_inerClock;
	void InitializeDataWindows();
	void SetDataWindowsCell(HWND hWnd1, string value, int nRow, int nCol);
	void flushDataWindows(int rows);
	void hubWorkingTime();
	bool dataMemoryEditable = false;
public:
	afx_msg void LoadedProgramRunMain();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedTerminatebuttone();
	int WorkHunIN[5];
	void flushTimeBar();
	unsigned int time;
	int inerClock;
	afx_msg void OnBnClickedStepover();
	
	afx_msg void OnBnClickeddatamemoryeditableidc();
};
