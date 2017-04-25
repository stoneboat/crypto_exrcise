// VMRunningControlDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MIPSvirtualmachine.h"
#include "VMRunningControlDlg.h"
#include "afxdialogex.h"


// VMRunningControlDlg �Ի���

IMPLEMENT_DYNAMIC(VMRunningControlDlg, CDialogEx)

VMRunningControlDlg::VMRunningControlDlg(CWnd* pParent, bool& _terminal) : CDialogEx(IDD_VMRUNNINGCONTROLDLG, pParent),terminal(_terminal) {
}
VMRunningControlDlg::~VMRunningControlDlg()
{
}

void VMRunningControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(VMRunningControlDlg, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &VMRunningControlDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// VMRunningControlDlg ��Ϣ�������


void VMRunningControlDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//boost::lock_guard<boost::mutex> lock_terminal(terminalVM_mutex);
	terminal = true;
	CDialogEx::OnCancel();
}
