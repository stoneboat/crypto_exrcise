// VMRunningControlDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MIPSvirtualmachine.h"
#include "VMRunningControlDlg.h"
#include "afxdialogex.h"


// VMRunningControlDlg 对话框

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


// VMRunningControlDlg 消息处理程序


void VMRunningControlDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	//boost::lock_guard<boost::mutex> lock_terminal(terminalVM_mutex);
	terminal = true;
	CDialogEx::OnCancel();
}
