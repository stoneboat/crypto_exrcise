
// MIPSvirtualmachine.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once
#include "VMRunningControlDlg.h"
#include <boost\thread\mutex.hpp>
#include <boost\thread\condition.hpp>
#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMIPSvirtualmachineApp: 
// �йش����ʵ�֣������ MIPSvirtualmachine.cpp
//

class CMIPSvirtualmachineApp : public CWinApp
{
public:
	CMIPSvirtualmachineApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};
extern CMIPSvirtualmachineApp theApp;
void setupVMcontrolDlg(boost::condition* terminalCon);