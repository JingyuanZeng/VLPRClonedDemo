
// VLPRClonedDemoDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"

//=====  STL ==============
#include <queue>
#include <iostream>
#include <list>
using namespace std;
//=========================

#include "TH_PlateID.h"
#include "public.h"


typedef struct LPR_Image
{
	unsigned char* buffer;
	int  imageWidth;
	int  imageHeight;
	int  imageSize;
	LPR_Image(){
		memset(this, 0, sizeof(LPR_Image));
	}
}LPR_Image;


// CVLPRClonedDemoDlg �Ի���
class CVLPRClonedDemoDlg : public CDialog
{
// ����
public:
	CVLPRClonedDemoDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_VLPRCLONEDDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);

	CListBox m_listDirs;
	afx_msg void OnBnClickedAddBrowser();
	afx_msg void OnBnClickedDirsClear();

public:
	TH_PlateIDCfg plateConfigTh;

	list<char *> mListPicturesPath;
	bool TH_InitDll(int bMovingImage);
	char pLocalChinese[3];    //���غ����ַ���������ַ�����Ϊ�ջ���31��ʡ��֮����֣���ʹ���׺���

	afx_msg void OnBnClickedAnay();

	list<HANDLE> EventList;
	HANDLE ReginsterMyThread(char *name);

	clock_t timeStart, timeNow;

	queue<LPR_Result*> LPRQueueResult;
	queue<LPR_Image*> imagesQueue;
	queue<LPR_Image*> imagesQueuePlay;
	unsigned char*  imageDataForShow;
	int				imageDataForShowSize;

	void LoadImageFromPath(char * path);
	void startThreads();
};
