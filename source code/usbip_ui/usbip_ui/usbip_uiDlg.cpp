
// usbip_uiDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "usbip_ui.h"
#include "usbip_uiDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Cusbip_uiDlg ��ȭ ����




Cusbip_uiDlg::Cusbip_uiDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Cusbip_uiDlg::IDD, pParent)
	, m_strListData(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cusbip_uiDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS, m_edIpServerAddress);
	DDX_Text(pDX, IDC_EDIT3, m_strListData);
	DDX_Control(pDX, IDC_EDIT3, m_edListData);
	DDX_Control(pDX, IDC_BUSID, selBusid);
}

BEGIN_MESSAGE_MAP(Cusbip_uiDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(UM_ACCEPT_MESSAGE, &Cusbip_uiDlg::OnAcceptClient)
	ON_MESSAGE(UM_RECEIVE_MESSAGE, &Cusbip_uiDlg::OnReceiveData)
	ON_MESSAGE(UM_CLOSE_MESSAGE, &Cusbip_uiDlg::OnCloseSocket)
	ON_BN_CLICKED(IDC_LIST, &Cusbip_uiDlg::OnBnClickedList)
	ON_BN_CLICKED(IDC_CONNECT, &Cusbip_uiDlg::OnBnClickedConnect)
	ON_CBN_SELCHANGE(IDC_BUSID, &Cusbip_uiDlg::OnCbnSelchangeBusid)
END_MESSAGE_MAP()


// Cusbip_uiDlg �޽��� ó����

BOOL Cusbip_uiDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ���� ������ �־�� �մϴ�.
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

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

//	WinExec(_T("C:/WinDDK/7600.16385.1/src/usb/usbip/Debug/usbip.exe"), SW_SHOW);
/*	
	STARTUPINFO StartupInfo = {0};
	PROCESS_INFORMATION ProcessInfo;
	StartupInfo.cb = sizeof(STARTUPINFO);
	::CreateProcess(NULL, "C:/WinDDK/7600.16385.1/src/usb/usbip/Debug/usbip.exe",
		NULL, NULL, FALSE, 0, NULL, NULL, &StartupInfo, &ProcessInfo);
*/
	if(listenSocket.m_hSocket != INVALID_SOCKET)
	{
		AddMessage("�̹� ��� �����Դϴ�.");
	}

	if(!listenSocket.Create(PORT))
	{
		AddMessage("���� ���� ����");
	}

	if(!listenSocket.Listen())
	{
		AddMessage("��� ����");

		listenSocket.Close();
	}
	else
	{
		AddMessage("������ ���� �Ǿ����ϴ�.");
	}

	ShellExecute(NULL, _T("Open"), 
		_T("C:/Users/windows7/Android-USB/source code/usbip/Debug/usbip.exe"),
		NULL, NULL, SW_SHOW);

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void Cusbip_uiDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void Cusbip_uiDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR Cusbip_uiDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT Cusbip_uiDlg::OnAcceptClient(WPARAM wParam, LPARAM lParam)
{
	listenSocket.Accept(dataSocket);
	AddMessage("Ŭ���̾�Ʈ�� �����߽��ϴ�.");
	return 0;
}

LRESULT Cusbip_uiDlg::OnReceiveData(WPARAM wParam, LPARAM lParam)
{
	char Rcvdata[MAXLINE];

	CDataSocket* pDataSocket = (CDataSocket*)wParam;

	pDataSocket->Receive(Rcvdata, sizeof(Rcvdata));

	CString strMsg = Rcvdata;

	AddMessage(strMsg);

	return 0;
}

// �������� ���� ���� ó��
LRESULT Cusbip_uiDlg::OnCloseSocket(WPARAM wParam, LPARAM lParam)
{
	dataSocket.Close();
	/*
	AddMessage("������ ������ ���� �߽��ϴ�.");
	// ���Ӳ��� ��ư ��Ȱ��ȭ, �������� ��ư Ȱ��ȭ, ������ ��ư ��Ȱ��ȭ
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
	*/

	return 0;
}

void Cusbip_uiDlg::AddMessage(CString strMsg)
{

	UpdateData();
	
	m_strListData += strMsg + "\r\n";

	UpdateData(FALSE);

	m_edListData.LineScroll(m_edListData.GetLineCount());
}

BOOL Cusbip_uiDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	return CDialogEx::PreTranslateMessage(pMsg);
}


void Cusbip_uiDlg::OnBnClickedList()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int ret = 0;
	CString strMsg;
	CString strAddress;
	m_edIpServerAddress.GetWindowText(strAddress);
	strMsg.Format("l");
	
	ret = dataSocket.Send(strMsg, strMsg.GetLength()+1);
	if(ret < 1){
		AddMessage("�޼��� ���� ����");
		return;
	}
	AddMessage("���� : "+strMsg);
	ret = dataSocket.Send(strAddress, strAddress.GetLength()+1);
	if(ret < 1){
		AddMessage("�޼��� ���� ����");
		return;
	}
	AddMessage("���� : "+strAddress);
	
	
}


void Cusbip_uiDlg::OnBnClickedConnect()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int ret = 0;
	CString strMsg;
	CString strAddress;
	m_edIpServerAddress.GetWindowText(strAddress);
	strMsg.Format("a");
	
	ret = dataSocket.Send(strMsg, strMsg.GetLength()+1);
	if(ret < 1){
		AddMessage("�޼��� ���� ����");
		return;
	}
	AddMessage("���� : "+strMsg);
	ret = dataSocket.Send(strAddress, strAddress.GetLength()+1);
	if(ret < 1){
		AddMessage("�޼��� ���� ����");
		return;
	}
	AddMessage("���� : "+strAddress);
	
}


void Cusbip_uiDlg::OnCbnSelchangeBusid()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nIndex = selBusid.GetCurSel();
	if(nIndex == -1){
		return;
	}

	CString busid;
	selBusid.GetLBText(nIndex, busid);

	sendMsg(busid);
	//GetDlgItem(IDC_EDIT_SEND_DATA)->SetFocus();
	UpdateData(FALSE);
}

void Cusbip_uiDlg::sendMsg(CString strMsg)
{
	int ret = 0;
	ret = dataSocket.Send(strMsg, strMsg.GetLength()+1);

	if(ret < 1){
		AddMessage("�޼��� ���� ����");
		return;
	}
}