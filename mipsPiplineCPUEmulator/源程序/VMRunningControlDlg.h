#pragma once


// VMRunningControlDlg �Ի���

class VMRunningControlDlg : public CDialogEx
{
	DECLARE_DYNAMIC(VMRunningControlDlg)

public:
	VMRunningControlDlg(CWnd* pParent, bool& _terminal);
	virtual ~VMRunningControlDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VMRUNNINGCONTROLDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:
	bool& terminal;
public:
	afx_msg void OnBnClickedCancel();
};
