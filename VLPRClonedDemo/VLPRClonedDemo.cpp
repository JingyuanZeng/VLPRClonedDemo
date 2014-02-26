
// VLPRClonedDemo.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "VLPRClonedDemo.h"
#include "VLPRClonedDemoDlg.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVLPRClonedDemoApp

BEGIN_MESSAGE_MAP(CVLPRClonedDemoApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CVLPRClonedDemoApp ����

CVLPRClonedDemoApp::CVLPRClonedDemoApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CVLPRClonedDemoApp ����

CVLPRClonedDemoApp theApp;


// CVLPRClonedDemoApp ��ʼ��

CString CVLPRClonedDemoApp::m_appPath="";
BOOL CVLPRClonedDemoApp::InitInstance()
{
	try
	{

	CString g_szOcxPath = this->m_pszHelpFilePath;
	CString g_szOcxName = this->m_pszExeName;

	g_szOcxName += ".HLP";
	int nTmp = g_szOcxName.GetLength();
	nTmp = g_szOcxPath.GetLength() - nTmp;
	g_szOcxPath = g_szOcxPath.Left(nTmp-1);

	m_appPath = g_szOcxPath;


	GdiplusStartup(&m_pGdiToken, &m_gdiplusStartupInput, NULL); //��ʼ��GDI+.


	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CVLPRClonedDemoDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	}
	catch(...){
		MessageBox(0, "�����쳣�������˳�", "",MB_OK);
	}

	return FALSE;
}
