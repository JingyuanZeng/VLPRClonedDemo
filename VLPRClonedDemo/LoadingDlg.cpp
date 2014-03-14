// LoadingDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "VLPRClonedDemo.h"
#include "LoadingDlg.h"


// CLoadingDlg �Ի���

IMPLEMENT_DYNAMIC(CLoadingDlg, CDialog)

CLoadingDlg::CLoadingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoadingDlg::IDD, pParent)
	, msg(_T(""))
{

	waitTime = 0;
}

CLoadingDlg::~CLoadingDlg()
{
}

void CLoadingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, ID_MSG, msg);
}


BEGIN_MESSAGE_MAP(CLoadingDlg, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CLoadingDlg ��Ϣ�������

void CLoadingDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == 1){
		this->OnOK();
	}

	CDialog::OnTimer(nIDEvent);
}

BOOL CLoadingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(waitTime>0)
		SetTimer(1, waitTime*1000, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
