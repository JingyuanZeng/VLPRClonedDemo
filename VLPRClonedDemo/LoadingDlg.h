#pragma once


// CLoadingDlg �Ի���

class CLoadingDlg : public CDialog
{
	DECLARE_DYNAMIC(CLoadingDlg)

public:
	CLoadingDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLoadingDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_LOADING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
