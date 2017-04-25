
// MIPSvirtualmachineDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MIPSvirtualmachine.h"
#include "MIPSvirtualmachineDlg.h"
#include "afxdialogex.h"

/**************************tmp*************/
#include <string>
/**************************tmp*************/

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CMIPSvirtualmachineDlg �Ի���



CMIPSvirtualmachineDlg::CMIPSvirtualmachineDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MIPSVIRTUALMACHINE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMIPSvirtualmachineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, DataWindows_IDC, dataWindows);
}
CPoint CMIPSvirtualmachineDlg::InterviewListCursorPosition() const
{
	DWORD pos = GetMessagePos();
	CPoint pt(LOWORD(pos), HIWORD(pos));
	ScreenToClient(&pt);

	CRect rect;
	CWnd *pWnd = GetDlgItem(DataWindows_IDC);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);

	pt.x -= rect.left;
	pt.y -= rect.top;

	return pt;
}
void CMIPSvirtualmachineDlg::OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	//m_fClickedList = true; //haven't been declared in CMIPSvirtualmachineDlg
	dataWindows.OnLButtonDown(MK_LBUTTON, InterviewListCursorPosition());

	*pResult = 0;
}
LRESULT CMIPSvirtualmachineDlg::OnNotifyDescriptionEdited(WPARAM wParam, LPARAM lParam)
{
	// Get the changed Description field text via the callback
	LV_DISPINFO* dispinfo = (LV_DISPINFO*)(lParam);
	/*******************************�Ƿ������޸Ķ�Ӧ���ڴ���Ϣ*************/
	int row = dispinfo->item.iItem;
	int col = dispinfo->item.iSubItem;
	CString tmp = dispinfo->item.pszText;
	if (dataMemoryEditable&&col != 0) {
		string tmpStr = CT2CA(tmp);
		if (MipsVirtual::check_number(tmpStr) && !tmpStr.empty()) {
			MipsVirtual::nk_data[row * 4 + col] = stoi(tmpStr);
		}
		else
			return 0;
	}
	else
		return 0;
	/**************************************************************/
	// Persist the selected attachment details upon updating its text
	dataWindows.SetItemText(dispinfo->item.iItem, dispinfo->item.iSubItem, dispinfo->item.pszText);
	return 0;
}
BEGIN_MESSAGE_MAP(CMIPSvirtualmachineDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMIPSvirtualmachineDlg::OnBnClickedOk)
	ON_BN_CLICKED(Running_Button, &CMIPSvirtualmachineDlg::LoadedProgramRunMain)
	ON_BN_CLICKED(IDCANCEL, &CMIPSvirtualmachineDlg::OnBnClickedCancel)
	ON_BN_CLICKED(TerminateButtone, &CMIPSvirtualmachineDlg::OnBnClickedTerminatebuttone)
	ON_BN_CLICKED(StepOver, &CMIPSvirtualmachineDlg::OnBnClickedStepover)
	ON_NOTIFY(NM_CLICK, DataWindows_IDC, OnNMClickList)
	ON_MESSAGE(WM_NOTIFY_DESCRIPTION_EDITED, OnNotifyDescriptionEdited)
	ON_BN_CLICKED(modifyData_modeIDC, &CMIPSvirtualmachineDlg::OnBnClickeddatamemoryeditableidc)
END_MESSAGE_MAP()


// CMIPSvirtualmachineDlg ��Ϣ�������

BOOL CMIPSvirtualmachineDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	vm = MipsVirtual::mipsVM("nkMips_machineCode.mc", "assemblycode.txt");
	//vm = MipsVirtual::mipsVM("test.mc", "testAssembly.txt");
	instructionString = NULL;
	registerStatusString = NULL;
	workHubLabelString = GetWC(getWorkHubLabelString());
	GetDlgItem(WorkHubLabel)->SetWindowText(workHubLabelString);
	for (int i = 0; i < 5; i++) {
		TimeSlide[i] = NULL;
		WorkHunIN[i] = 0;
	}
	EX_MEM_cond = false;
	/*****************����״̬��ʼ��*********/
	for (int i = 0; i < 5; i++)
		workFinished[i] = false;
	flyingReg.clear();
	for (int i = 0; i < 5; i++)
		pipLineStallStatus[i] = false;
	dataWindows.SetExtendedStyle(LVS_EX_DOUBLEBUFFER);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMIPSvirtualmachineDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMIPSvirtualmachineDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMIPSvirtualmachineDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMIPSvirtualmachineDlg::OnBnClickedOk() //load machine code
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//CDialogEx::OnOK();
	vm.InitialVM();
	if (instructionString != NULL)
		delete[] instructionString;
	if (registerStatusString != NULL)
		delete[] registerStatusString;
	if (pcStatusString != NULL)
		delete[] pcStatusString;
	instructionString = GetWC(getRecentInstructionString()); //����������ܹ��Զ��ͷ��ַ�������Ҫ�ⲿ����
	registerStatusString = GetWC(getRegisterStatus());
	pcStatusString = GetWC(getPcString());
	GetDlgItem(InstructionIDC)->SetWindowText(instructionString);
	GetDlgItem(RegisterStatusIDC)->SetWindowText(registerStatusString);
	GetDlgItem(pcStatusIDC)->SetWindowText(pcStatusString);
	/******************�߳̿�������ģ��--��ˮ��Ӳ��װ��*********************/
	for (int i = 0; i < timeSlideNum; i++) {
		timeBar[i][0] = i;
	}
	flushTimeBar();
	UpdateData(false);
	InitialPiplLineHardeWare();
	/**************Initial Data Window Box*********/
	InitializeDataWindows();
}
void CMIPSvirtualmachineDlg::LoadedProgramRunMain()
{
	VMRunning();
	/*VMRunningControlDlg *vmConDlg = new VMRunningControlDlg(NULL,terminal);

	BOOL ret = vmConDlg->Create(IDD_VMRUNNINGCONTROLDLG);
	if (ret == FALSE) {
		AfxMessageBox(_T("�Ի��򴴽�ʧ�ܣ�"));
	}

	vmConDlg->ShowWindow(SW_SHOW);*/
}
void CMIPSvirtualmachineDlg::flushTimeBar() {
	for (int i = 0; i < timeSlideNum; i++) {
		if (TimeSlide[i] != NULL)
			delete[] TimeSlide[i];
		TimeSlide[i] = GetWC(getTimebarString(i));
		//define TimeSlide1 1007 -> TimeSlide5 1011
		GetDlgItem(1007+i)->SetWindowText(TimeSlide[i]);
	}
}
void CMIPSvirtualmachineDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnCancel();
}
void CMIPSvirtualmachineDlg::OnBnClickedTerminatebuttone()
{
	/******************��ˮ����ֹ*********************/
	TerminalPiplLineHardWare();
	//boost::condition terminalCon;
	//boost::thread thread_setupVMcontrol(setupVMcontrolDlg, &terminalCon);
}
void CMIPSvirtualmachineDlg::VMRunning() {
	clockCon.notify_all();
	/******************��ˮ������*********************/
	while(pc<instructionCount){
		{
			boost::lock_guard<boost::mutex> lock_terminal(terminalVM_mutex);
			if (terminal)
				break;
		}
		OnePipeLineClock();
	}
	
}
void CMIPSvirtualmachineDlg::InitialPiplLineHardeWare() {
	/**********��ʼ���μ���ˮ�Ĵ���******/
	IF_ID_IR.IM_num = -1;
	ID_EX_IR.IM_num = -1;
	EX_MEM_IR.IM_num = -1;
	MEM_WB_IR.IM_num = -1;
	IF_ID_NPC = 0;
	ID_EX_NPC = 0;
	EX_MEM_opCode = -1;
	ID_EX_A = 0;
	ID_EX_B = 0;
	ID_EX_IMM = 0;
	EX_MEM_ALUOutput = 0;
	EX_MEM_B = 0;
	EX_MEM_cond = false;
	MEM_WB_ALUOutput = 0;
	MEM_WB_LMD = 0;
	/**********��ʼ����ˮ������ӳ���ϵ******/
	for (int i = 0; i < 5; i++)
		cpuPipeLine_map[cpuPipeLineName[i]] = i;
	/**********��ʼ����ˮ�߿����ź�******/
	terminal = false;
	time = 0;
	inerClock = 1;
	lock_inerClock = boost::unique_lock<boost::shared_mutex>(inerClock_mutex);
	lock_inerClock.unlock();
	/**********��ʼ����ˮ�߹�����******/
	IFworkHub iF(inerClock, WorkHunIN[0], inerClock_mutex,workFinished[0]);
	IDworkHub iD(inerClock, WorkHunIN[1], inerClock_mutex, workFinished[1]);
	EXworkHub eX(inerClock, WorkHunIN[2], inerClock_mutex, workFinished[2]);
	MEMworkHub Mem(inerClock, WorkHunIN[3], inerClock_mutex, workFinished[3]);
	WBworkHub Wb(inerClock, WorkHunIN[4], inerClock_mutex, workFinished[4]);
	/**********������ˮ�߹�����******/
	PipeLineHadware[0] = new boost::thread(iF);
	PipeLineHadware[1] = new boost::thread(iD);
	PipeLineHadware[2] = new boost::thread(eX);
	PipeLineHadware[3] = new boost::thread(Mem);
	PipeLineHadware[4] = new boost::thread(Wb);
}
void CMIPSvirtualmachineDlg::TerminalPiplLineHardWare() {
	TerminateThread(PipeLineHadware[0]->native_handle(), 0);
	TerminateThread(PipeLineHadware[1]->native_handle(), 0);
	TerminateThread(PipeLineHadware[2]->native_handle(), 0);
	TerminateThread(PipeLineHadware[3]->native_handle(), 0);
	TerminateThread(PipeLineHadware[4]->native_handle(), 0);
}
void CMIPSvirtualmachineDlg::hubWorkingTime() {
	clockCon.notify_all();
	bool notend = true;
	while (notend) {
		Sleep(pipeWorkTime); //work hub working time pre-half
		notend = false;
		for (int i = 0; i < 5; i++) {
			if (!workFinished[i]) {
				notend = true;
				break;
			}
		}
	}
	
}
void CMIPSvirtualmachineDlg::OnePipeLineClock() {
	for (int i = 0; i < 2; i++) {
		lock_inerClock.lock();
		inerClock = (inerClock + 1) % 2;
		lock_inerClock.unlock();
		if (inerClock == 0) {
			time++;
			//pre_fetch_change
			hubWorkingTime();
			//fetch_workTime
			hubWorkingTime();
		}
		else {
			//store_workTime
			hubWorkingTime();
			//post_store_workTime
			hubWorkingTime();
			lock_inerClock.lock();
			for (int i = timeSlideNum - 1; i >0; i--) {
				timeBar[i][0] = i + time;
				for (int j = 0; j < 5; j++) {
					timeBar[i][j + 1] = timeBar[i - 1][j + 1];
				}
			}
			for (int j = 0; j < 5; j++) {
				timeBar[0][0] = time;
				timeBar[0][j + 1] = WorkHunIN[j];
			}
			lock_inerClock.unlock();
		}
	}
	lock_inerClock.lock();
	flushTimeBar();
	if (instructionString != NULL)
		delete[] instructionString;
	if (registerStatusString != NULL)
		delete[] registerStatusString;
	if (pcStatusString != NULL)
		delete[] pcStatusString;
	instructionString = GetWC(getRecentInstructionString()); //����������ܹ��Զ��ͷ��ַ�������Ҫ�ⲿ����
	registerStatusString = GetWC(getRegisterStatus());
	pcStatusString = GetWC(getPcString());
	GetDlgItem(InstructionIDC)->SetWindowText(instructionString);
	GetDlgItem(RegisterStatusIDC)->SetWindowText(registerStatusString);
	GetDlgItem(pcStatusIDC)->SetWindowText(pcStatusString);
	flushDataWindows(0);
	UpdateData(false);
	Sleep(displayingTime);
	lock_inerClock.unlock();
}
void CMIPSvirtualmachineDlg::OnBnClickedStepover()
{
	OnePipeLineClock();
}
void CMIPSvirtualmachineDlg::InitializeDataWindows() {
	dataMemoryEditable = false; //�ڴ�������Ƿ�����ֱ���޸�
	HWND hWnd = ::GetDlgItem(m_hWnd, DataWindows_IDC);
	// Set the LVCOLUMN structure with the required 
	// column information
	LVCOLUMN list;
	list.mask = LVCF_TEXT | LVCF_WIDTH |
		LVCF_FMT | LVCF_SUBITEM;
	list.fmt = LVCFMT_LEFT;
	list.cx = 100;
	list.pszText = L"�ڴ��к�\\�к�";
	list.iSubItem = 0;
	//Inserts the column
	::SendMessage(hWnd, LVM_INSERTCOLUMN,
		(WPARAM)0, (WPARAM)&list);
	list.cx = 50;
	list.pszText = L"0";
	list.iSubItem = 1;
	::SendMessage(hWnd, LVM_INSERTCOLUMN,
		(WPARAM)1, (WPARAM)&list);
	list.cx = 50;
	list.pszText = L"1";
	list.iSubItem = 2;
	::SendMessage(hWnd, LVM_INSERTCOLUMN,
		(WPARAM)2, (WPARAM)&list);
	list.cx = 50;
	list.pszText = L"2";
	list.iSubItem = 3;
	::SendMessage(hWnd, LVM_INSERTCOLUMN,
		(WPARAM)3, (WPARAM)&list);
	list.cx = 50;
	list.pszText = L"3";
	list.iSubItem = 4;
	::SendMessage(hWnd, LVM_INSERTCOLUMN,
		(WPARAM)4, (WPARAM)&list);
	for (int r = 0; r < 5; r++) {
		SetDataWindowsCell(hWnd, to_string(r), r, 0);
	}
	for (int r = 0; r < 5; r++) {
		for (int c = 0; c < 4; c++) {
			SetDataWindowsCell(hWnd,to_string(nk_data[r*4+c]), r, c+1);
		}
	}
}
void CMIPSvirtualmachineDlg::SetDataWindowsCell(HWND hWnd1, string stringValue, int nRow, int nCol) {
	TCHAR     szString[256];
	CString value(stringValue.c_str());
	wsprintf(szString, value, 0);
	//Fill the LVITEM structure with the 
	//values given as parameters.
	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = nRow;
	lvItem.pszText = szString;
	lvItem.iSubItem = nCol;
	if (nCol >0)
		//set the value of listItem
		::SendMessage(hWnd1, LVM_SETITEM,
		(WPARAM)0, (WPARAM)&lvItem);
	else
		//Insert the value into List
		ListView_InsertItem(hWnd1, &lvItem);
}


void CMIPSvirtualmachineDlg::OnBnClickeddatamemoryeditableidc()
{
	if (dataMemoryEditable) {
		dataMemoryEditable = false;
		GetDlgItem(modifyData_modeIDC)->SetWindowText(L"�����ڴ��޸�");
	}
	else {
		dataMemoryEditable = true;
		GetDlgItem(modifyData_modeIDC)->SetWindowText(L"�����ڴ��޸�");
	}
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}
void CMIPSvirtualmachineDlg::flushDataWindows(int rows) {
	if (rows > (nk_data_memory_size / 4 - 5))//max row number can display
		return;
	
	for (int r = 0; r < 5; r++) {
		for (int c = 0; c < 4; c++) {
			string tmp = to_string(nk_data[r * 4 + c]);
			CString tmpC = CString(tmp.c_str());
			dataWindows.SetItemText(r, c+1, tmpC);
			//GetDlgItem(DataWindows_IDC)->
		}
	}
}
