
// VLPRClonedDemoDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


#include "LPR_info.h"
//=====  STL ==============
#include <queue>
#include <iostream>
#include <map>
#include <list>
using namespace std;
//=========================

#include "TH_PlateID.h"
#include "public.h"
#include "afxcmn.h"

#include "LoadingDlg.h"


// CVLPRClonedDemoDlg �Ի���
class CVLPRClonedDemoDlg : public CDialog
{
// ����
public:
	CVLPRClonedDemoDlg(CWnd* pParent = NULL);	// ��׼���캯��

	CLoadingDlg loading;
	int CloseThread();

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

	list<char*> mListPicturesTemp;
	list<LPR_File*> mListPicturesPath;//����ʽ�����ļ��б�
	list<LPR_File*> mListLPRPictures;//��ʽ������ļ�(�������ڴ�)�б�
	bool TH_InitDll(int bMovingImage);
	char pLocalChinese[3];    //���غ����ַ���������ַ�����Ϊ�ջ���31��ʡ��֮����֣���ʹ���׺���

	afx_msg void OnBnClickedAnay();

	list<HANDLE> EventList;
	HANDLE ReginsterMyThread(char *name);

	clock_t timeStart, timeNow;

	queue<LPR_Result*> LPRQueueResult;
	list<LPR_ResultPair*> LPRClonedList;
	queue<LPR_Image*> imagesQueue;
	queue<LPR_Image*> imagesQueuePlay;

	void LoadImageFromPath(LPR_File * path);
	void startThreads();
	afx_msg void OnLbnDblclkListDirs();

	CString m_imageDir;

	CListCtrl m_list;
	afx_msg void OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnClose();
	int m_Threshold;

	long  nFrames;
	CString m_dstDir;
	afx_msg void OnBnClickedBrowserDestDir();

	bool loadFomatedFile;

	bool ReadConfig();
	bool SaveConfig();
	afx_msg void OnBnClickedReLoad();

	bool	running ;
	bool	bLoadFileFinished;
	bool	bLoadImageFinished;
	bool	bProcessImageFinished;
	bool	bResultFinished;
	CListCtrl m_listLpr;
	afx_msg void OnLbnSelchangeListDirs();
	afx_msg void OnNMClickListLpr(NMHDR *pNMHDR, LRESULT *pResult);


	char picture1Path[512];
	char picture2Path[512];
};
