
// VLPRClonedDemo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CVLPRClonedDemoApp:
// �йش����ʵ�֣������ VLPRClonedDemo.cpp
//

class CVLPRClonedDemoApp : public CWinAppEx
{
public:
	static CString m_appPath;
	static CString m_ProfileName;

	CVLPRClonedDemoApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CVLPRClonedDemoApp theApp;