#pragma once


// VMRunningControlDlg 对话框

class VMRunningControlDlg : public CDialogEx
{
	DECLARE_DYNAMIC(VMRunningControlDlg)

public:
	VMRunningControlDlg(CWnd* pParent, bool& _terminal);
	virtual ~VMRunningControlDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VMRUNNINGCONTROLDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	bool& terminal;
public:
	afx_msg void OnBnClickedCancel();
};
