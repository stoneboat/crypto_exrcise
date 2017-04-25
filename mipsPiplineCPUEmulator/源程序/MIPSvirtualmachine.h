
// MIPSvirtualmachine.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once
#include "VMRunningControlDlg.h"
#include <boost\thread\mutex.hpp>
#include <boost\thread\condition.hpp>
#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CMIPSvirtualmachineApp: 
// 有关此类的实现，请参阅 MIPSvirtualmachine.cpp
//

class CMIPSvirtualmachineApp : public CWinApp
{
public:
	CMIPSvirtualmachineApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};
extern CMIPSvirtualmachineApp theApp;
void setupVMcontrolDlg(boost::condition* terminalCon);