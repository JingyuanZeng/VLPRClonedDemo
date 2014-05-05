
// VLPRClonedDemoDlg.cpp : ʵ���ļ�

/**
* Auth: Karl
* Date: 2014/2/20
* LastUpdate: 2014/2/24
*/

#include "stdafx.h"
#include "VLPRClonedDemo.h"
#include "VLPRClonedDemoDlg.h"

#include "FileUtil.h"
#include "VideoUtil.h"
#include "LogM.h"
#include "LPRDB.h"


#pragma comment(lib, "TH_PLATEID.lib")

#pragma execution_character_set("utf-8")


//====================================================================
#define WIDTHSTEP(pixels_width)  (((pixels_width) * 24/8 +3) / 4 *4)

HANDLE handleExit = 0;
HANDLE handleVideoThread=0;
HANDLE handleVideoThreadStoped=0;
HANDLE handleLPRThreadStoped=0;
HANDLE hShowVideoFrame=0;
HANDLE hLoadedFilesName=0;

static bool debugMode = false;

void LoadFileThread(void *pParam);

//====================================================================

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CVLPRClonedDemoDlg �Ի���




CVLPRClonedDemoDlg::CVLPRClonedDemoDlg(CWnd* pParent /*=NULL*/)
: CDialog(CVLPRClonedDemoDlg::IDD, pParent)
, m_Threshold(60)
, m_dstDir(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	memset(pLocalChinese, 0, 3);

	loadFomatedFile = false;
	running = false;

	bLoadFileFinished = false;
	bLoadImageFinished = false;
	bProcessImageFinished = false;
	bResultFinished = false;

	handleExit = CreateEvent(NULL, FALSE, FALSE, NULL);
	handleVideoThread = CreateEvent(NULL, FALSE, FALSE, NULL);
	handleVideoThreadStoped = CreateEvent(NULL, FALSE, FALSE, NULL);
	hShowVideoFrame  = CreateEvent(NULL, FALSE, FALSE, NULL);
	handleLPRThreadStoped = CreateEvent(NULL, FALSE, FALSE, NULL);
	hLoadedFilesName  = CreateEvent(NULL, FALSE, FALSE, NULL);

}

void CVLPRClonedDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DIRS, m_listDirs);
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Text(pDX, IDC_EDIT_THREAD, m_Threshold);
	DDX_Text(pDX, IDC_EDIT_DST_DIR, m_dstDir);
	DDX_Control(pDX, IDC_LIST_LPR, m_listLpr);
}

BEGIN_MESSAGE_MAP(CVLPRClonedDemoDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DROPFILES()
	ON_BN_CLICKED(BT_ADD_BROWSER, &CVLPRClonedDemoDlg::OnBnClickedAddBrowser)
	ON_BN_CLICKED(BT_DIRS_CLEAR, &CVLPRClonedDemoDlg::OnBnClickedDirsClear)
	ON_BN_CLICKED(BT_ANAY, &CVLPRClonedDemoDlg::OnBnClickedAnay)
	ON_LBN_DBLCLK(IDC_LIST_DIRS, &CVLPRClonedDemoDlg::OnLbnDblclkListDirs)
	ON_NOTIFY(NM_CLICK, IDC_LIST, &CVLPRClonedDemoDlg::OnNMClickList)
	ON_WM_MOUSEMOVE()
	ON_WM_CLOSE()
	ON_BN_CLICKED(BT_BROWSER_DEST_DIR, &CVLPRClonedDemoDlg::OnBnClickedBrowserDestDir)
	ON_BN_CLICKED(BT_RE_LOAD, &CVLPRClonedDemoDlg::OnBnClickedReLoad)
	ON_LBN_SELCHANGE(IDC_LIST_DIRS, &CVLPRClonedDemoDlg::OnLbnSelchangeListDirs)
	ON_NOTIFY(NM_CLICK, IDC_LIST_LPR, &CVLPRClonedDemoDlg::OnNMClickListLpr)
END_MESSAGE_MAP()


// CVLPRClonedDemoDlg ��Ϣ�������

BOOL CVLPRClonedDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	ReadConfig();

	DWORD dwStyle = m_list.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//ѡ��ĳ��ʹ���и�����ֻ������report����listctrl��
	dwStyle |= LVS_EX_GRIDLINES;//�����ߣ�ֻ������report����listctrl��
	//	dwStyle |= LVS_EX_CHECKBOXES;//itemǰ����checkbox�ؼ�
	m_list.SetExtendedStyle(dwStyle); //������չ���

	int cols = 0;
	m_list.InsertColumn( cols++, "����", LVCFMT_LEFT, 80 );
	m_list.InsertColumn( cols++, "��1����ʱ��", LVCFMT_LEFT, 150 );
	m_list.InsertColumn( cols++, "��1����", LVCFMT_LEFT, 80 );
	m_list.InsertColumn( cols++, "��1��ɫ", LVCFMT_LEFT, 80 );
	m_list.InsertColumn( cols++, "��2����ʱ��", LVCFMT_LEFT, 150 );
	m_list.InsertColumn( cols++, "��2����", LVCFMT_LEFT, 80 );
	m_list.InsertColumn( cols++, "��2��ɫ", LVCFMT_LEFT, 80 );
	m_list.InsertColumn( cols++, "ʱ���(����)", LVCFMT_LEFT, 120 );

	m_list.InsertColumn( cols++, "ͼƬ1", LVCFMT_LEFT, 0 );
	m_list.InsertColumn( cols++, "ͼƬ2", LVCFMT_LEFT, 0 );
	m_list.InsertColumn( cols++, "ID1", LVCFMT_LEFT, 0 );
	m_list.InsertColumn( cols++, "ID2", LVCFMT_LEFT, 0 );

	dwStyle = m_listLpr.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//ѡ��ĳ��ʹ���и�����ֻ������report����listctrl��
	dwStyle |= LVS_EX_GRIDLINES;//�����ߣ�ֻ������report����listctrl��
	m_listLpr.SetExtendedStyle(dwStyle); //������չ���

	cols = 0;
	m_listLpr.InsertColumn( cols++, "����", LVCFMT_LEFT, 80 );
	m_listLpr.InsertColumn( cols++, "����ʱ��", LVCFMT_LEFT, 150 );
	m_listLpr.InsertColumn( cols++, "��������", LVCFMT_LEFT, 100 );
	m_listLpr.InsertColumn( cols++, "����", LVCFMT_LEFT, 100 );
	m_listLpr.InsertColumn( cols++, "����ɫ", LVCFMT_LEFT, 100 );
	m_listLpr.InsertColumn( cols++, "����", LVCFMT_LEFT, 100 );


	m_listLpr.InsertColumn( cols++, "ͼƬ", LVCFMT_LEFT, 0 );
	m_listLpr.InsertColumn( cols++, "ID1", LVCFMT_LEFT, 0 );


	startThreads();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CVLPRClonedDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CVLPRClonedDemoDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CVLPRClonedDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CVLPRClonedDemoDlg::OnDropFiles(HDROP hDropInfo)
{
	int iFileCount = ::DragQueryFile(hDropInfo, 0xffffffff, NULL, 0);
	char file_name[MAX_PATH];
	CString strBuffer = "";
	bool bSame=false;
	char temp[256]={0};
	for(int i=0; i<iFileCount; i++){
		::DragQueryFile(hDropInfo, i, file_name, MAX_PATH);
		if(strlen(file_name)<2)
			continue;

		if(checkFolder(file_name)){
			sprintf(temp, "���ļ����Ѿ�������![%s]", file_name);
			MessageBox(temp);
			continue;
		}

		bSame=false;
		for(int j=0; j<m_listDirs.GetCount(); j++){
			m_listDirs.GetText(j, strBuffer);
			if(strBuffer.Compare(file_name)==0)
				bSame = true;
		}
		if( bSame==false && FileUtil::FindFirstFileExists(file_name, FILE_ATTRIBUTE_DIRECTORY)){
			m_listDirs.AddString(file_name);
		}
	}
	DragFinish(hDropInfo);
	
	_beginthread(LoadFileThread, 0 ,this);//�����ļ�

	CDialog::OnDropFiles(hDropInfo);
}

void CVLPRClonedDemoDlg::OnBnClickedAddBrowser()
{
	UpdateData(true);
	char *path = FileUtil::SelectFolder(this->m_hWnd, "ѡ���ļ���");
	if(path==0)
		return ;
	if(strlen(path)<2)
		return;

	char temp[256]={0};
	if(checkFolder(path)){
		if(MessageBox("���ļ����Ѿ�������,�������� ?", 0, MB_YESNO | MB_ICONQUESTION  )!=IDYES)
			return ;
	}

	bool bSame=false;
	CString strBuffer;
	for(int j=0; j<m_listDirs.GetCount(); j++){
		m_listDirs.GetText(j, strBuffer);
		if(strBuffer.Compare(path)==0)
			bSame = true;
	}
	if( bSame==false && FileUtil::FindFirstFileExists(path, FILE_ATTRIBUTE_DIRECTORY)){
		m_listDirs.AddString(path);
	}

	_beginthread(LoadFileThread, 0 ,this);//�����ļ�

	UpdateData(false);
}

void CVLPRClonedDemoDlg::OnBnClickedDirsClear()
{
	m_listDirs.ResetContent();
	m_list.DeleteAllItems();
	m_listLpr.DeleteAllItems();

	//���ͼƬ�ļ��б�
	while(WaitForSingleObject(handleExit,0)!=WAIT_OBJECT_0 )
	{
		if(mListPicturesPath.empty())
			break;
		else
		{
			LPR_File *f = mListPicturesPath.front();
			mListPicturesPath.pop_front();
			if(f)
				delete f;
		}
	}
}


//�����һ·ʶ�������mem1 �� mem2 �ڴ�����С������һ�ڸ����ο�ֵ�� 
static unsigned char mem1[0x4000]; 
static unsigned char mem2[100<<20];//60M 

bool CVLPRClonedDemoDlg::TH_InitDll(int bMovingImage)
{	
	int r = TH_UninitPlateIDSDK(&plateConfigTh);
	//plateConfigTh = c_defConfig;

	//���õ�һ·ʶ����� 
	plateConfigTh.nMinPlateWidth = 60; 
	plateConfigTh.nMaxPlateWidth = 400; 
	plateConfigTh.nMaxImageWidth = 3000; 
	plateConfigTh.nMaxImageHeight = 3000; 
	plateConfigTh.bVertCompress = 0; 
	plateConfigTh.bIsFieldImage = 0; 
	plateConfigTh.bOutputSingleFrame = 1; 
	plateConfigTh.bMovingImage = bMovingImage;   //��Ƶ��ʽ��Ϊ 1 
	plateConfigTh.pFastMemory=mem1; 
	plateConfigTh.nFastMemorySize=0x4000; 
	plateConfigTh.pMemory=mem2; 
	plateConfigTh.nMemorySize = 100<<20 ; 

	char error[512]={0};
	int n = TH_InitPlateIDSDK( &plateConfigTh ); 
	if(n!=0){
		sprintf(error, "%s,��������ʧ��",  pErrorInfo[n]);
		MessageBox(error);
	}
	int m =TH_SetEnabledPlateFormat(PARAM_INDIVIDUAL_ON, &plateConfigTh); //����ʶ��
	int k = TH_SetImageFormat(ImageFormatBGR, FALSE, FALSE, &plateConfigTh); 
	TH_SetEnableCarTypeClassify(true,  &plateConfigTh); //��������ʶ��
	char m_LocalProvince[10] = {0}; 
	sprintf(m_LocalProvince, "%s", pLocalChinese);
	int l = TH_SetProvinceOrder(m_LocalProvince, &plateConfigTh); //���س��ƺ���
	int logo =TH_SetEnableCarLogo(true, &plateConfigTh); //ʶ��logo����

	if (n!=0||m!=0||k!=0||l!=0 | logo!=0) 
		return false; 

	return true;
}

int filesCount = 0;//�ļ�����
int curFileIndex = 0;//��ǰ�����ļ����



BITMAPINFO *GetBitMapInfo(int width, int height, int bitCount=24){
	BITMAPINFO *m_bmphdr=0;  
	DWORD dwBmpHdr = sizeof(BITMAPINFO);  
	m_bmphdr = new BITMAPINFO[dwBmpHdr];  
	m_bmphdr->bmiHeader.biBitCount = 24;  
	m_bmphdr->bmiHeader.biClrImportant = 0;  
	m_bmphdr->bmiHeader.biSize = dwBmpHdr;  
	m_bmphdr->bmiHeader.biSizeImage = 0;  
	m_bmphdr->bmiHeader.biWidth = width;  
	m_bmphdr->bmiHeader.biHeight = height;  
	m_bmphdr->bmiHeader.biXPelsPerMeter = 0;  
	m_bmphdr->bmiHeader.biYPelsPerMeter = 0;  
	m_bmphdr->bmiHeader.biClrUsed = 0;  
	m_bmphdr->bmiHeader.biPlanes = 1;  
	m_bmphdr->bmiHeader.biCompression = BI_RGB;  
	return m_bmphdr;
}
//������Ƶ
int  PlayVideo(unsigned char *pix, int PICTURE_ID, CWnd *cWnd, int imageWidth, int imageHeight, bool revert=true)
{
	BITMAPINFO *m_bmphdr = GetBitMapInfo(imageWidth, imageHeight );

	CRect rc;
	cWnd->GetDlgItem(PICTURE_ID)->GetWindowRect(&rc);

	HDC hdc = cWnd->GetDlgItem(PICTURE_ID)->GetDC()->m_hDC;
	SetStretchBltMode(hdc, STRETCH_HALFTONE); //�ؼӣ�StretchBlt, StretchDIBits���Զ�ͼ�����ݽ�������, ѹ����ʾʧ��

	int nResult = 0;
	if(revert){
		nResult = ::StretchDIBits( hdc,  
			0, 0, rc.Width(), rc.Height(), 
			0, imageHeight,  imageWidth, -imageHeight,  
			pix, m_bmphdr,  DIB_RGB_COLORS,  SRCCOPY);  
	}else{
		nResult = ::StretchDIBits( hdc,  
			0, 0, rc.Width(), rc.Height(), 
			0, 0,  imageWidth, imageHeight,  
			pix, m_bmphdr,  DIB_RGB_COLORS,  SRCCOPY);  
	}
	return nResult;
}




//����ͼƬ���ƻ�ȡͼƬ��ʱ����Ϣ��FORMAT eg: 20140104150218_location.jpg
long getLastUpdateTime(char *filePath, char *lastUpdateTime)
{
	char fileName[256]={0};
	long lTime=0;
	sprintf( fileName, "%s", (strrchr(filePath,'\\')+1) );
	//fileName	0x0aa8f6f8 "20140224152924_��W56Y22_location.bmp"	char [256]
	char *p = strstr(fileName, "_");
	if( p == NULL){
		time_t timer = time(0);
		struct tm *tblock;
		tblock = localtime(&timer);	
		sprintf(lastUpdateTime , "%04d%02d%02d%02d%02d%02d", tblock->tm_year+1900, tblock->tm_mon+1, tblock->tm_mday, tblock->tm_hour, tblock->tm_min, tblock->tm_sec );
		return timer;
	}
	char szDateTime[64]={0};
	if((p - fileName) != 14)
	{
		debug(__FUNCTION__"(%s): %s get datetime error." , __LINE__, fileName);
		return -1;
	}
	int len = p-fileName;
	memcpy(szDateTime, fileName, len);
	struct tm t; 
	t.tm_sec	= atoi(szDateTime+12); szDateTime[12]='\0';
	t.tm_min	= atoi(szDateTime+10); szDateTime[10]='\0';
	t.tm_hour	= atoi(szDateTime+8); szDateTime[8]='\0';
	t.tm_mday		= atoi(szDateTime+6); szDateTime[6]='\0';
	t.tm_mon	= atoi(szDateTime+4)-1; szDateTime[4]='\0' ;
	t.tm_year	= atoi(szDateTime)- 1900; szDateTime[0]='\0';

	lTime = mktime(&t);
	if( lTime<1 || t.tm_year < 1 || t.tm_mon < 1 || t.tm_mday < 1)
	{
		debug(__FUNCTION__"(%s): %s comvert datetime error." , __LINE__, fileName);
		return -1;
	}
	memcpy(lastUpdateTime, fileName, len);

	return lTime;

}
#include "time.h"

//����ʶ�����߳�
void ProcessResultThread(void *pParam)
{
	try{

		char filename[256]={0};
		CVLPRClonedDemoDlg *dlg = (CVLPRClonedDemoDlg*)pParam;
		HANDLE handleCanExit = dlg->ReginsterMyThread("ProcessResultThread");

		char temp[256]={0};
		unsigned char *pBit = 0;
		debug("ProcessResultThread ����  handle=0x%x", handleCanExit);

		while(WaitForSingleObject(handleExit,0)!=WAIT_OBJECT_0){
		
			if(dlg->running==false){
				Sleep(100);			
				continue;
			}

			if(dlg->LPRQueueResult.empty()){
				Sleep(100);		
				dlg->bResultFinished = true;
				continue;
			}
			dlg->bResultFinished = false;
			LPR_Result *result = dlg->LPRQueueResult.front();
			dlg->LPRQueueResult.pop();
			if(result==0)
				continue;

			sprintf(temp, "���Ĳ�:������,��ʣ: %d", dlg->LPRQueueResult.size());
			dlg->GetDlgItem(BT_RESULT_STATUS)->SetWindowText(temp);

			log("ProcessResultThread Frame=%d  Plate=%s  Logo=%s (%d,%d)-(%d,%d)", dlg->nFrames, result->plate, result->carLogo, \
				result->plateRect.left, result->plateRect.top,
				result->plateRect.right, result->plateRect.bottom);

			if( strlen(result->resultPicture) > 5){
				result->time = getLastUpdateTime(result->resultPicture, result->lastUpdateTime);//����ͼƬ���ƻ�ȡͼƬ��ʱ����Ϣ��FORMAT eg: 2014-1-4_15.02.18_location.jpg

			}else{
				time_t timer;
				timer = time(NULL);
				struct tm *tblock;
				tblock = localtime(&timer);
				result->time = 	timer; //д��ʱ��
				sprintf(temp,"%d/%d/%d %d:%d:%d", tblock->tm_year+1900, tblock->tm_mon+1, tblock->tm_mday, tblock->tm_hour, tblock->tm_min, tblock->tm_sec );
				sprintf(result->lastUpdateTime, "%s", temp);//д��ʱ��

				//20140224152924_location.bmp
				sprintf(temp,"%04d%02d%02d%02d%02d%02d", tblock->tm_year+1900, tblock->tm_mon+1, tblock->tm_mday, tblock->tm_hour, tblock->tm_min, tblock->tm_sec );
				sprintf(filename, "%s\\%s_location_%s.bmp", dlg->m_imageDir, temp, result->plate);

				VideoUtil::write24BitBmpFile(filename, result->imageWidth, result->imageHeight,(unsigned char*)pBit,  WIDTHSTEP(result->imageWidth));//ץ����дͼ
				sprintf(result->resultPicture, filename);
			}

			if(strlen(result->plate)<3  || result->confidence==0)
				continue;

			if(result->time > 0 ){

				if(getClonedLpr(result, dlg->LPRClonedList, dlg->m_Threshold * 60)==1){
					log("getConedLpr @ 0x%x  %s", result, result->plate);
					insertLpr(result);

					//show at list
					int nSize = dlg->LPRClonedList.size();
					for(int i=0; i<nSize; i++)
					{
						LPR_ResultPair *lprPair = dlg->LPRClonedList.front();
						dlg->LPRClonedList.pop_front();
						if(lprPair==0)
							continue;
						if(lprPair->lpr_result[0].plate=="" || lprPair->lpr_result[1].plate=="")
							continue;

						int nRow = dlg->m_list.InsertItem(0, "");//
						
						int cols =0 ;
						dlg->m_list.SetItemText(nRow, cols++, lprPair->lpr_result[0].plate);//����
						dlg->m_list.SetItemText(nRow, cols++, lprPair->lpr_result[0].FormatTime());//ʱ��1
						dlg->m_list.SetItemText(nRow, cols++, lprPair->lpr_result[0].carLogo);//����1
						dlg->m_list.SetItemText(nRow, cols++, lprPair->lpr_result[0].carColor1);//��ɫ1
						dlg->m_list.SetItemText(nRow, cols++, lprPair->lpr_result[1].FormatTime());//ʱ��2
						dlg->m_list.SetItemText(nRow, cols++, lprPair->lpr_result[1].carLogo);//����2
						dlg->m_list.SetItemText(nRow, cols++, lprPair->lpr_result[1].carColor1);//��ɫ1
						sprintf(temp, "%d ", abs(lprPair->lpr_result[1].time - lprPair->lpr_result[0].time)/60);
						dlg->m_list.SetItemText(nRow, cols++,  temp);//ʱ���(����)
						dlg->m_list.SetItemText(nRow, cols++, lprPair->lpr_result[0].resultPicture);//ͼƬ1
						dlg->m_list.SetItemText(nRow, cols++, lprPair->lpr_result[1].resultPicture);//ͼƬ2
						
						sprintf(temp, "%d",  lprPair->lpr_result[0].id);
						dlg->m_list.SetItemText(nRow, cols++, temp);//ID FOR DEBUG
						sprintf(temp, "%d",  lprPair->lpr_result[1].id);
						dlg->m_list.SetItemText(nRow, cols++, temp);//ID FOR DEBUG

						if(lprPair->lpr_result[0].pResultBits)
							delete lprPair->lpr_result[0].pResultBits;
						if(lprPair->lpr_result[1].pResultBits)
							delete lprPair->lpr_result[1].pResultBits;
						delete lprPair;

					}
				}else{
					insertLpr(result);
					debug("ProcessResultThread LPRQueueResult.front 0x%x", result);
					if(result->pResultBits)
						delete result->pResultBits;
					delete result;
				}
			}

		}
end:
		debug("ProcessResultThread �����˳�");
		SetEvent(handleCanExit);//���ÿ����˳���
	}
	catch(...)
	{
		log("ProcessResultThread Error");
		//	MessageBox(0, "RecognitionThread Error", "", MB_OK);
	}


}

void PlayThread(void *pParam)
{
	try
	{
		CVLPRClonedDemoDlg *dlg = (CVLPRClonedDemoDlg*)pParam;
		HANDLE handleCanExit = dlg->ReginsterMyThread("PlayThread");
		log("PlayThread ����  handle=0x%x", handleCanExit);
		char temp[512]={0};

		while(WaitForSingleObject(handleExit,0)!=WAIT_OBJECT_0){
			if(dlg->running==false){
				Sleep(100);			
				continue;
			}
		//	if(WaitForSingleObject(hShowVideoFrame,0)==0)
			{
				if( !dlg->imagesQueuePlay.empty() ){
					LPR_Image* pLpr = dlg->imagesQueuePlay.front();
					dlg->imagesQueuePlay.pop();
					if(pLpr==NULL)
						continue;
					sprintf(temp, "PlaySize = %d ", dlg->imagesQueuePlay.size());
					dlg->GetDlgItem(ID_PLAY_STATUS)->SetWindowText(temp);

					debug("PlayThread imagesQueuePlay.front  0x%x  imagesQueuePlaySize = %d", pLpr->buffer,  dlg->imagesQueuePlay.size());
					PlayVideo( pLpr->buffer, ID_VIDEO_WALL,  dlg, pLpr->imageWidth, pLpr->imageHeight);
					
					try{
						if(pLpr->buffer)
							delete pLpr->buffer;
						if(pLpr)
							delete pLpr;
					}catch(...)
					{
						log("PlayThread Error @ delete pLpr->buffer");
						//	MessageBox(0, "RecognitionThread Error", "", MB_OK);
					}
				}
			}
		}
		log("PlayThread �����˳�");
		SetEvent(handleCanExit);//���ÿ����˳���
	}
	catch(...)
	{
		log("PlayThread Error");
	}
}


//ʶ���߳�
void RecognitionThread(void *pParam)
{
	try
	{
		CVLPRClonedDemoDlg *dlg = (CVLPRClonedDemoDlg*)pParam;
		HANDLE handleCanExit = dlg->ReginsterMyThread("RecognitionThread");

		log("RecognitionThread ����  handle=0x%x", handleCanExit);
		int imageSize = 0;
		clock_t t1,t2;
		LPR_Image *pLprImage=0;
		char temp[256]={0};
		char filename[256]={0};
		int ret = 0;
		SetEvent(handleLPRThreadStoped);
		while(WaitForSingleObject(handleExit,0)!=WAIT_OBJECT_0){

			if(dlg->running==false){
				Sleep(100);			
				continue;
			}

			if(dlg->imagesQueue.empty()){
				SetEvent(handleLPRThreadStoped);
				Sleep(100);		
				dlg->bProcessImageFinished = true;
				continue;
			}
			dlg->bProcessImageFinished = false;
	
			if(dlg->LPRQueueResult.size()>100){
				Sleep(2*1000);
				continue;
			}

			ResetEvent(handleLPRThreadStoped);

			if( !dlg->imagesQueue.empty())
			{
				pLprImage = 0;
				try{
					pLprImage = dlg->imagesQueue.front();
					dlg->imagesQueue.pop();
				}catch(...){
				
				}
				if(pLprImage==0)
					continue;

				dlg->nFrames ++;
				log("RecognitionThread imagesQueue.front 0x%x nFrames=%d  buffer @ 0x%x ( %d x %d )-0x%x  queue size=%d ",
					pLprImage, dlg->nFrames, pLprImage->buffer, pLprImage->imageWidth, pLprImage->imageHeight,
					pLprImage->buffer + WIDTHSTEP( pLprImage->imageWidth) * pLprImage->imageHeight, dlg->imagesQueue.size());

				imageSize = pLprImage->imageWidth * pLprImage->imageHeight *3;

				LPR_Image *playImage = new LPR_Image();
				memcpy(playImage, pLprImage, sizeof(LPR_Image));
				playImage->buffer = new unsigned char[imageSize];
				memcpy(playImage->buffer , pLprImage->buffer, imageSize);
				dlg->imagesQueuePlay.push( playImage );//For Play
		//		SetEvent(hShowVideoFrame);

				if(true) { //if(dlg->company == WENTONG && dlg->TH_LPR_canRun) 
					TH_PlateIDResult result[20];        //���붨�������� 
					memset(&result, 0, sizeof(result)); 
					int nResultNum = 1; 
					TH_RECT rcDetect={0, 0, pLprImage->imageWidth, pLprImage->imageHeight}; 

					try
					{
						ret = -1;
						t1 = clock();
						//sprintf(filename, "E:/vlpr-out/%d.bmp", dlg->nFrames);
						//VideoUtil::write24BitBmpFile(filename, pLprImage->imageWidth, pLprImage->imageHeight,
						//	(unsigned char*)pLprImage->buffer,  pLprImage->imageWidth);//ץ����дͼ
						if(debugMode){
							sprintf(result[0].license,"��A88888");
							result[0].nCarLogo = CarLogo_AUDI ;
							result[0].nConfidence = 50;
							ret = 0;
							nResultNum = 1;
						}else{
							ret  =  TH_RecogImage( pLprImage->buffer,  pLprImage->imageWidth, pLprImage->imageHeight,  result, &nResultNum, &rcDetect, &dlg->plateConfigTh); //ʶ����
							TH_EvaluateCarColor( pLprImage->buffer,  pLprImage->imageWidth, pLprImage->imageHeight,  result, &nResultNum, &rcDetect, &dlg->plateConfigTh); //ʶ������ɫ
						}
						t2 = clock();
					}
					catch(...)
					{
						sprintf(temp, "ʶ��[RecognitionThread]����(0x%x)@%d" ,GetLastError(), dlg->nFrames);
						dlg->GetDlgItem(ID_PLAY_STATUS)->SetWindowText(temp);
						log("RecognitionThread TH_RecogImage Error  GetLastError( 0x%x )", GetLastError() );
						dlg->loading.msg.Format("%s", temp);
						dlg->loading.waitTime = 2;//��
						//	dlg->loading.DoModal();

						if(pLprImage->buffer)
							delete pLprImage->buffer;
						if(pLprImage)
							delete pLprImage;

						Sleep(100);
						continue;
						//	MessageBox(0, "RecognitionThread Error", "", MB_OK);
					}
					
					if(ret!=0 || nResultNum<=0){
						debug("δʶ�� ret = %d", ret);
						ret = -1;
					}else{
						
						ret = 1;
						LPR_Result *r = new LPR_Result();
						r->takesTime = t2-t1;
						sprintf(r->plate, "%s", result[0].license);//����
						r->confidence = result[0].nConfidence*1.0/100;//���Ŷ�
						sprintf(r->plateType, "%s", plateType[result[0].nType]);//��������
						sprintf(r->plateColor, "%s", plateColor[result[0].nColor]);//������ɫ
						sprintf(r->carLogo, "%s", CarLogo[ result[0].nCarLogo] );//����
						sprintf(r->carType, "%s", CarType[ result[0].nCarType] );//����
						sprintf(r->carColor1, "%s", CarColor[ result[0].nCarColor] );//����ɫ
						sprintf(r->direct, "%s", TH_Dirction[result[0].nDirection]);//����

						r->plateRect.left = result[0].rcLocation.left;
						r->plateRect.top = result[0].rcLocation.top;
						r->plateRect.right = result[0].rcLocation.right;
						r->plateRect.bottom = result[0].rcLocation.bottom;

						r->pResultBits = new unsigned char[imageSize];
						memcpy(r->pResultBits , pLprImage->buffer, imageSize );

						memcpy(r->folder, pLprImage->folder, 512);//�ļ���

						sprintf(r->resultPicture, pLprImage->filePath);
						r->imageWidth = pLprImage->imageWidth;
						r->imageHeight = pLprImage->imageHeight;

						log("����: %s  ����: %s", result[0].license, CarLogo[ result[0].nCarLogo] );

						dlg->LPRQueueResult.push(r);
						debug("RecognitionThread LPRQueueResult.push  0x%x", r);
					}
				}
				if(pLprImage->buffer)
					delete pLprImage->buffer;
				if(pLprImage)
					delete pLprImage;

				dlg->timeNow = clock();
				sprintf(temp, "Rate:%d fps", dlg->nFrames*1000/(dlg->timeNow - dlg->timeStart));
				debug("RecognitionThread %s",temp );

				sprintf(temp, "������:����ͼƬʶ������,�ļ�����: %d,�Ѵ���: %0.3g %%(%d/%d) QSize=%d", 
								filesCount, dlg->nFrames*1.0/filesCount *100, dlg->nFrames, filesCount, dlg->imagesQueue.size());
				dlg->GetDlgItem(ID_PROCESS_STATUS)->SetWindowText(temp);

				//Sleep(100);
			}
		}

end:
		log("RecognitionThread �����˳�");
		SetEvent(handleCanExit);//���ÿ����˳���
	}
	catch(...)
	{
		log("RecognitionThread Error");
		//	MessageBox(0, "RecognitionThread Error", "", MB_OK);
	}

}

unsigned char* GetImageDataByPath(const char *pathIn, int *width, int *height, Rect *rectIn=0)
{
	Bitmap* image = 0;
	image = KLoadBitmap( pathIn );
	if(image==0)
		return  0 ;

	Rect rect(0,0, image->GetWidth(), image->GetHeight());
	if(rectIn!=0){
		memcpy(&rect, rectIn, sizeof(Rect));
	}
	*width = rect.Width ; 
	*height = rect.Height;

	int BufferLen =  rect.Width * rect.Height * 3;
	unsigned char* buffer = new unsigned char[BufferLen];

	BitmapData bitmapData={0};
	bitmapData.Stride = WIDTHSTEP( rect.Width );
	bitmapData.Scan0 = new BYTE[bitmapData.Stride * rect.Height];


	image->LockBits(&rect, ImageLockModeRead | ImageLockModeUserInputBuf, PixelFormat24bppRGB, &bitmapData); 

	memcpy(buffer, (BYTE*)bitmapData.Scan0, BufferLen); 

	delete bitmapData.Scan0;
	image->UnlockBits( &bitmapData);
	delete image;

	return buffer;

}

void CVLPRClonedDemoDlg::LoadImageFromPath(LPR_File * f)
{
	if(f==0)
		return ;
	try{
		Bitmap* image = 0;
		image = KLoadBitmap( f->filePath );
		//	DrawImg2Hdc(image, ID_VIDEO_WALL, this);
		if(image==0)
			return ;

		LPR_Image *pLpr = new LPR_Image();
		sprintf(pLpr->folder, "%s", f->folder);
		sprintf(pLpr->filePath,"%s", f->filePath );
		pLpr->imageWidth = image->GetWidth();
		pLpr->imageHeight = image->GetHeight();
		pLpr->imageSize = pLpr->imageWidth * pLpr->imageHeight * 3;
		pLpr->buffer = new unsigned char[pLpr->imageSize];

		long BufferLen;
		BufferLen= pLpr->imageSize;

		BitmapData bitmapData={0};
		bitmapData.Stride = WIDTHSTEP(pLpr->imageWidth);
		bitmapData.Scan0 = new BYTE[bitmapData.Stride * pLpr->imageHeight];

		Rect rect(0,0, pLpr->imageWidth, pLpr->imageHeight);
		image->LockBits(&rect, ImageLockModeRead | ImageLockModeUserInputBuf, PixelFormat24bppRGB, &bitmapData); 

		memcpy(pLpr->buffer, (BYTE*)bitmapData.Scan0, BufferLen); 

		image->UnlockBits( &bitmapData);
		try{
			delete bitmapData.Scan0;
			delete image;
		}catch(...){
			log(__FUNCTION__" : delete bitmapData.Scan0 or delete image ERROR");
		}
		
		imagesQueue.push(pLpr);//������н��д���
		log("LPRFromImage imagesQueue.push( 0x%x ) @buffer 0x%x ( %d x %d )  imagesQueue.size = %d", pLpr, pLpr->buffer, pLpr->imageWidth, pLpr->imageHeight, imagesQueue.size());

	}catch(...){
		log("dlg->LoadImageFromPath(%s) ERROR", f->filePath);
	}
}


int curLoadPictureIndex=0;
//����ͼƬ���ڴ�
void LoadPictureThread(void *pParam)
{
	try
	{
		CVLPRClonedDemoDlg *dlg = (CVLPRClonedDemoDlg*)pParam;
		HANDLE handleCanExit = dlg->ReginsterMyThread("LoadPictureThread");
		log("LoadPictureThread ����  handle=0x%x", handleCanExit);

		char temp[512]={0};
		curLoadPictureIndex = 0;

		while(WaitForSingleObject(handleExit,0)!=WAIT_OBJECT_0)
		{
			if(dlg->running==false){
				Sleep(100);			
				continue;
			}
			if(dlg->mListLPRPictures.empty()){
				Sleep(100);
				dlg->bLoadImageFinished = true;
				continue;
			}
			dlg->bLoadImageFinished = false;
			if(dlg->imagesQueue.size() > 10){
				Sleep(100);
				continue;
			}
		
			curLoadPictureIndex ++;
			LPR_File *f = dlg->mListLPRPictures.front();
			dlg->mListLPRPictures.pop_front();
			
			if(f!=0)
			{
				dlg->LoadImageFromPath(f);
				try{
					delete f;
				}catch(...){
					log("LoadPictureThread  delete path ERROR");
				}
				sprintf(temp, "�ڶ���:����ͼƬ���ڴ�, �ļ�����: %d, �Ѽ���: %0.3g %%@%d", filesCount, curLoadPictureIndex*1.0/filesCount *100,curLoadPictureIndex );
				debug(temp);
				dlg->GetDlgItem(ID_STATUS)->SetWindowText(temp);
			}
		}

	end:	
		log("LoadPictureThread �����˳�");
		SetEvent(handleCanExit);//���ÿ����˳���

	}
	catch(...)
	{
		MessageBox(0, "����ͼƬ���ڴ� Error", "", 0);
		log("LoadPictureThread Error");
		//	MessageBox(0, "RecognitionThread Error", "", MB_OK);
	}
}

//��ʽ���ļ�����
void FormatFileNameThread(void *pParam)
{
	try
	{
		CVLPRClonedDemoDlg *dlg = (CVLPRClonedDemoDlg*)pParam;
		HANDLE handleCanExit = dlg->ReginsterMyThread("FormatFileNameThread");
		log("FormatFileNameThread ����  handle=0x%x", handleCanExit);

		char temp[512]={0};

		int curFormatFileIndex = 0;
		filesCount = dlg->mListPicturesPath.size();
		int failedCount = 0;

		curLoadPictureIndex = 0;
		bool fileTime = true;// ((CButton *)dlg->GetDlgItem(IDC_RADIO1))->GetCheck();

		char *dstDir = 0; 
		if(dlg->m_dstDir.IsEmpty()==false)
		{
			dstDir = new char[512];
			sprintf(dstDir, "%s", dlg->m_dstDir.GetBuffer(dlg->m_dstDir.GetLength()));
		}
		char *destFilePath = 0;
		while(WaitForSingleObject(handleExit,0)!=WAIT_OBJECT_0  && dlg->mListPicturesPath.size() > 0)
		{
			if(dlg->running==false){
				break;
			}
			if(dlg->mListLPRPictures.size()>100){
				Sleep(100);
				dlg->bLoadFileFinished = true;
				continue;
			}
			dlg->bLoadFileFinished = false;
			curFormatFileIndex ++;
			LPR_File *f = dlg->mListPicturesPath.front();
			dlg->mListPicturesPath.pop_front();
			if(f!=0)
			{
				destFilePath = FileUtil::FormatFileName(f->filePath, curFormatFileIndex, fileTime, dstDir );
				if(destFilePath)
				{
					sprintf(f->filePath, "%s", destFilePath);
					dlg->mListLPRPictures.push_back(f);
					
				}else{
					failedCount ++;
					delete f;
				}

				sprintf(temp, "��һ��:��ʽ���ļ�, �ļ�����: %d, �Ѵ���: %0.3g %%@%d, ����ʧ��:%d ", filesCount, curFormatFileIndex*1.0/filesCount *100,  
					curFormatFileIndex, failedCount);
				dlg->GetDlgItem(ID_STATUS_LIST)->SetWindowText(temp);
				//Sleep(100);
			}
		}
		if(dstDir)
			delete dstDir;
end:
		log("FormatFileNameThread �����˳�");
		SetEvent(handleCanExit);//���ÿ����˳���
	}catch(...){
		log("FormatFileNameThread ERROR");
	}

}

void LoadFileThread(void *pParam)
{
	try
	{
		ResetEvent(hLoadedFilesName);

		CVLPRClonedDemoDlg *dlg = (CVLPRClonedDemoDlg*)pParam;
		HANDLE handleCanExit = dlg->ReginsterMyThread("LoadFileThread");
		log("LoadFileThread ����  handle=0x%x", handleCanExit);

		char temp[512]={0};

		//���ͼƬ�ļ��б�
		while(WaitForSingleObject(handleExit,0)!=WAIT_OBJECT_0 )
		{
			if(dlg->mListPicturesPath.empty())
				break;
			else
			{
				LPR_File *f = dlg->mListPicturesPath.front();
				dlg->mListPicturesPath.pop_front();
				if(f)
					delete f;
			}
		}
		debug("���֮ǰ��ͼƬ�ļ��б�  mListPicturesPath.size=%d ",dlg->mListPicturesPath.size());

		dlg->GetDlgItem(ID_STATUS)->SetWindowText("���ڼ����ļ��б�... ");
		if(dlg->loadFomatedFile){
			FileUtil::ListFiles(dlg->m_dstDir.GetBuffer(dlg->m_dstDir.GetLength()), dlg->mListPicturesTemp, "*.jpg", true);
			FileUtil::ListFiles(dlg->m_dstDir.GetBuffer(dlg->m_dstDir.GetLength()), dlg->mListPicturesTemp, "*.bmp", true);
			FileUtil::ListFiles(dlg->m_dstDir.GetBuffer(dlg->m_dstDir.GetLength()), dlg->mListPicturesTemp, "*.png", true);
		}else{
			CString path;
			for(int i=0; i<dlg->m_listDirs.GetCount(); i++){
				dlg->m_listDirs.GetText(i, path);
				FileUtil::ListFiles(path.GetBuffer(path.GetLength()), dlg->mListPicturesTemp, "*.jpg", true);
				FileUtil::ListFiles(path.GetBuffer(path.GetLength()), dlg->mListPicturesTemp, "*.bmp", true);
				FileUtil::ListFiles(path.GetBuffer(path.GetLength()), dlg->mListPicturesTemp, "*.png", true);

				sprintf(temp, "���ڼ����ļ��б�: %3g %% , �ļ�����: %d ", (i+1)*1.0/dlg->m_listDirs.GetCount() *100, dlg->mListPicturesTemp.size());
				debug("%s  %s", temp, path );
				dlg->GetDlgItem(ID_STATUS)->SetWindowText(temp);
			}
		}

		char *path=0;
		while(!dlg->mListPicturesTemp.empty()){
			path = 0;
			path = dlg->mListPicturesTemp.front();
			dlg->mListPicturesTemp.pop_front();
			if(path){
				LPR_File *f = new LPR_File();
				sprintf(f->filePath,"%s", path);
				memcpy(f->folder, path, strrchr(path, '\\')-path);
				dlg->mListPicturesPath.push_back(f);
				delete path;
			}
		}

		dlg->GetDlgItem(ID_STATUS)->SetWindowText("�����ļ��б����");
		if( dlg->mListPicturesPath.size()>0)
			dlg->GetDlgItem(BT_ANAY)->EnableWindow(true);
		else
			dlg->GetDlgItem(BT_ANAY)->EnableWindow(false);
end:
		log("LoadFileThread �����˳�");
		SetEvent(handleCanExit);//���ÿ����˳���

		SetEvent(hLoadedFilesName);

	}
	catch(...)
	{
		MessageBox(0, "���ڼ����ļ��б� Error", "", 0);
		log("LoadFileThread Error");
		//	MessageBox(0, "RecognitionThread Error", "", MB_OK);
	}

}

//�̼߳���
void MonitorThread(void *pParam)
{
	try
	{
		CVLPRClonedDemoDlg *dlg = (CVLPRClonedDemoDlg*)pParam;
		HANDLE handleCanExit = dlg->ReginsterMyThread("MonitorThread");
		log("MonitorThread ����  handle=0x%x", handleCanExit);

		int size[10]={0};
		int flag = 0;
		int i;

		while(WaitForSingleObject(handleExit,0)!=WAIT_OBJECT_0 )
		{
			if(dlg->running)
			{
				// ����ʽ���ļ��б� --����ʽ������--���������ڴ��ļ��б�--�������ڴ�--��������ͼƬ�б�--��ʶ����(ʶ����)--�����������б�--��������(�������ƺͱ��浽���ݿ�)  
				size[1] = dlg->mListPicturesPath.size();
				size[2] = dlg->mListLPRPictures.size();
				size[3] = dlg->imagesQueue.size();
				size[4] = dlg->LPRQueueResult.size();
				size[5] = dlg->imagesQueuePlay.size();
				log("����ʽ��  �ļ��б�: %d ", size[1]);
				log("�������ڴ��ļ��б�: %d ", size[2]);
				log("������ͼƬ    �б�: %d ", size[3]);
				log("��������    �б�: %d ", size[4]);
				log("����          �б�: %d ", size[5]);
				log("==============================================================");

				flag = 0;
				for(i=0; i<10; i++)
					flag += size[i];
				if(flag==0)
					dlg->running = false;

				if( dlg->bLoadFileFinished && dlg->bLoadImageFinished && dlg->bProcessImageFinished && dlg->bResultFinished )
					dlg->running = false;

				if( dlg->running){
					dlg->GetDlgItem(BT_ANAY)->SetWindowText("ֹͣ");
					dlg->GetDlgItem(BT_RE_LOAD)->EnableWindow(false);
				}else{
					dlg->GetDlgItem(BT_ANAY)->SetWindowText("����");
					dlg->GetDlgItem(BT_ANAY)->EnableWindow(false);
					dlg->GetDlgItem(BT_RE_LOAD)->EnableWindow(true);
					dlg->GetDlgItem(ID_PLAY_STATUS)->SetWindowText("�������");
						dlg->loading.msg.Format("�������");
						dlg->loading.waitTime = 3;//��
						dlg->loading.DoModal();
				}

			}

			Sleep(1000);

		}
		log("MonitorThread �����˳�");
		SetEvent(handleCanExit);//���ÿ����˳���
	}catch(...){
		log("MonitorThread ERROR");
	}
	

}



void CVLPRClonedDemoDlg::OnBnClickedAnay()
{
	UpdateData(true);

	if(running){
		running = false;
		GetDlgItem(BT_ANAY)->EnableWindow(false);
		
		Sleep(100);

		//clear
		LPR_File *f = 0;
		while(mListLPRPictures.empty()==false){
			f = mListLPRPictures.front();
			mListLPRPictures.pop_front();
			if(f){
				try{
					delete f;
				}catch(...){}
			}
		}

		LPR_Image * pLprImage =0;
		while(imagesQueue.empty()==false){
			pLprImage = imagesQueue.front();
			imagesQueue.pop();
			if(pLprImage){
				try{
					if(pLprImage->buffer)
						delete pLprImage->buffer;
					delete pLprImage;
				}catch(...){}
			}
		}
		while(imagesQueuePlay.empty()==false){
			pLprImage = imagesQueuePlay.front();
			imagesQueuePlay.pop();
			if(pLprImage){
				try{
					if(pLprImage->buffer)
						delete pLprImage->buffer;
					delete pLprImage;
				}catch(...){}
			}
		}
		LPR_Result *result = 0;
		while(LPRQueueResult.empty()==false){
			result = LPRQueueResult.front();
			LPRQueueResult.pop();
			if(result){
				try{
					if(result->pResultBits)
						delete result->pResultBits;
					delete result;
				}catch(...){}
			}
		}

		GetDlgItem(BT_ANAY)->SetWindowText("����");
		GetDlgItem(BT_ANAY)->EnableWindow(true);
		return ;

	}
	
	GetDlgItem(BT_RE_LOAD)->EnableWindow(false);

	if(m_dstDir.IsEmpty())
	{
		MessageBox("���������Ŀ¼!");
	//	OnBnClickedBrowserDestDir();
		return ;
	}

	//FileUtil::RemoveDir(m_dstDir.GetBuffer(m_dstDir.GetLength()));
	FileUtil::CreateFolders(m_dstDir.GetBuffer(m_dstDir.GetLength()));

	if( mListPicturesPath.size()<1)
	{
		GetDlgItem(BT_ANAY)->EnableWindow(false);
		MessageBox("�����Ŀ¼�����¼���","",0);
		return ;
	}

	memset(picture1Path, 0, 512);
	memset(picture2Path, 0, 512);
	nFrames = 0;

	if(debugMode){
		running = true;
		_beginthread(FormatFileNameThread, 0 ,this);//��ʽ��ͼƬ����
		GetDlgItem(BT_ANAY)->SetWindowText("ֹͣ");
	}
	else if(TH_InitDll(0))
	{
		running = true;
		_beginthread(FormatFileNameThread, 0 ,this);//��ʽ��ͼƬ����
		GetDlgItem(BT_ANAY)->SetWindowText("ֹͣ");
	//	GetDlgItem(BT_ANAY)->EnableWindow(false);
	}
}


//ÿ���̶߳�Ҫע�ᣬ�˳���ʱ������һһ���
//���أ���������̵߳�CEvent
HANDLE CVLPRClonedDemoDlg::ReginsterMyThread(char *name)
{
	// �����¼�
	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, name);
	SetEvent(hEvent);
	EventList.push_back(hEvent);
	debug("EventList.size = %d", EventList.size());
	return hEvent;

}

#include "ProcessState.h"
char* SizeFormat(uint64_t size, char *temp){
	if(temp==0)
		return 0;
	if(size<1024)
		sprintf(temp,"%d byte", size);
	else if(size<1024*1024)
		sprintf(temp,"%d KB",size/1024 );
	else if(size<1024*1024*1024)
		sprintf(temp,"%d MB",size/1024/1024 ); 
	else if(size<1024*1024*1024*1024)
		sprintf(temp,"%d GB",size/1024/1024/1024 ); 
	else // if(size<1024*1024*1024*1024*1024)
		sprintf(temp,"%d TB",size/1024/1024/1024/1024 );

	return temp;
}


void ProcessMonitorThread(void *pParam)
{
	try
	{
		CVLPRClonedDemoDlg *dlg = (CVLPRClonedDemoDlg*)pParam;

		HANDLE handleCanExit = dlg->ReginsterMyThread("ProcessMonitorThread");
		log("ProcessMonitorThread ����  handle=0x%x", handleCanExit);

		char temp[512]={0};
		uint64_t  mem=0;//�ڴ�ʹ��
		uint64_t  vmem=0;//�����ڴ�ʹ��
		int cpu =0;
		int ret=0;
		char tempsize[64]={0};
		while(WaitForSingleObject(handleExit,0)!=WAIT_OBJECT_0 )
		{
			cpu = get_cpu_usage();
			ret = get_memory_usage( &mem, &vmem);
			if( cpu>=0)
				sprintf(temp, "CPU: %d%% ", cpu );
			if( ret>=0){
				sprintf(temp, "%s  Memery: %s | %s ",temp, SizeFormat(mem, tempsize) , SizeFormat(vmem, tempsize) );
			}
			dlg->GetDlgItem(ID_PROCESS_STATE)->SetWindowText(temp);
			Sleep(1000);
		}
		log("ProcessMonitorThread �����˳� 0x%x ", handleCanExit);
		SetEvent(handleCanExit);//���ÿ����˳���
		

	}catch(...){
	
	}

}

void CVLPRClonedDemoDlg::startThreads()
{
	_beginthread(ProcessMonitorThread, 0, this);//���ϵͳ��Դ�߳�
	_beginthread(MonitorThread, 0, this);//�̼߳���
	_beginthread(RecognitionThread, 0, this);//ʶ���߳�
	_beginthread(PlayThread, 0, this);//�����߳�
	_beginthread(ProcessResultThread, 0, this);//�������߳�
	_beginthread(LoadPictureThread, 0, this);//����ͼƬ���ڴ��߳�

	

}
void CVLPRClonedDemoDlg::OnLbnDblclkListDirs()
{
	//˫�� �Ƴ�
	int index = m_listDirs.GetCurSel();
	m_listDirs.DeleteString(index);
	
	_beginthread(LoadFileThread, 0 ,this);//�����ļ�
}

void CVLPRClonedDemoDlg::OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(running){
		MessageBox("���ڴ������Ժ�����");
		return;
	}

	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	int nItem = m_list.GetNextSelectedItem(pos);
	CString temp;
	if(nItem>=0){
		temp = m_list.GetItemText(nItem, 8 );
		Bitmap* imagePlate = KLoadBitmap(temp.GetBuffer(temp.GetLength()));
		if(imagePlate)
		{
			sprintf(picture1Path, "%s", temp);
			DrawImg2Hdc(imagePlate, ID_VIDEO_WALL, this);
			delete imagePlate;
		}

		temp = m_list.GetItemText(nItem, 9 );
		Bitmap* imageScreen = KLoadBitmap(temp.GetBuffer(temp.GetLength()));
		if(imageScreen)
		{
			sprintf(picture2Path, "%s", temp);
			DrawImg2Hdc(imageScreen, ID_PICTURE, this);
			delete imageScreen;
		}
	}
	*pResult = 0;
}


void CVLPRClonedDemoDlg::OnMouseMove(UINT nFlags, CPoint pointIn)
{
	CRect r1, r2, r3;
	GetDlgItem(ID_VIDEO_WALL)->GetWindowRect(&r1);
	GetDlgItem(ID_PICTURE)->GetWindowRect(&r2);
	GetDlgItem(ID_LPR_PICTURE)->GetWindowRect(&r3);
	ScreenToClient(&r1);
	ScreenToClient(&r2);

	//debug("OnMouseMove:  point(%d, %d) ", point.x, point.y );

	CString tempPath;

	if(r1.PtInRect(pointIn) || r2.PtInRect(pointIn) )
	{
		//	debug("PlayVideo:  r1.PtInRect(%d)  r2.PtInRect(%d) ", r1.PtInRect(point),  r2.PtInRect(point));
		//if(nItem>=0)
		{
			int width = 0;
			int height =0;

			CPoint point;
			CPoint pt;
			float bix=0.0, biy=0.0;
			if(r1.PtInRect(pointIn) && strlen(picture1Path)>2 ){
				Image *image = KLoadBitmap( picture1Path );
				if( image == 0)
					goto end;
				width = image->GetWidth();
				height = image->GetHeight();
				delete image;
				point.x = pointIn.x - r1.left;
				point.y = pointIn.y - r1.top;

				bix = width*1.0/r1.Width();
				biy = height*1.0/r1.Height();

				tempPath = picture1Path;
			}
			else if(r2.PtInRect(pointIn) && strlen(picture2Path)>2 ){
				Image *image = KLoadBitmap( picture2Path );
				if( image == 0)
					goto end;
				width = image->GetWidth();
				height = image->GetHeight();
				delete image;
				point.x = pointIn.x - r2.left;
				point.y = pointIn.y - r2.top;

				bix = width*1.0/r2.Width();
				biy = height*1.0/r2.Height();

				tempPath = picture2Path;
			}else
				goto end;

			pt.x = bix * point.x;
			pt.y = biy * point.y;

			//	debug("pt before (%d,%d)", pt.x, pt.y);
			//set pt.x
			pt.x -= r3.Width()/2;
			if(pt.x<0)
				pt.x = 0;
			else if(pt.x> (width - r3.Width()) )
				pt.x = width - r3.Width();

			//set pt.y
			pt.y -= r3.Height()/2;
			if(pt.y<0)
				pt.y = 0;
			else if(pt.y> (height - r3.Height()) )
				pt.y = height - r3.Height();

			//	debug("pt after (%d,%d)", pt.x, pt.y);

			int w=0, h=0;
			Rect rectIn( pt.x, pt.y, r3.Width(), r3.Height());
			unsigned char* imageData = GetImageDataByPath( tempPath.GetBuffer(tempPath.GetLength()), &w, &h, &rectIn);

			if(imageData)
			{
				//	debug("PlayVideo: %d x %d ",  width, height);
				PlayVideo(imageData, ID_LPR_PICTURE, this, w, h);
				delete imageData;
			}
		}
	}
end:
	CDialog::OnMouseMove(nFlags, pointIn);
}


void LoadingThread(void* pParam)
{
	CVLPRClonedDemoDlg *dlg = (CVLPRClonedDemoDlg*)pParam;
	dlg->loading.DoModal();
	//	MessageBox(0, "�����˳�...", "", 0);
}


int CVLPRClonedDemoDlg::CloseThread()
{
	try
	{
		int times=0;
		while(EventList.size()>0 && times<10)
		{
			times ++;
			for(list<HANDLE>::iterator it = EventList.begin(); it != EventList.end();)
			{
				SetEvent(handleExit);
				Sleep(10);
				HANDLE h = (HANDLE)*it;
				int ret =WaitForSingleObject(h, 10);
				bool bSigned=false;
				switch(ret)
				{
				case WAIT_FAILED:
					debug(" WAIT_FAILED");
					break;
				case WAIT_TIMEOUT:
					debug("times=%d WAIT_TIMEOUT", times);
					break;
				case WAIT_OBJECT_0:
					debug(" WAIT_OBJECT_0 handle=0x%x  singed , so can exit", h);
					it = EventList.erase(it);
					bSigned = true;
					break;
				default :
					debug("ret=%d, 0x%x  !=WAIT_OBJECT_0  LastError=",ret, h, GetLastError());
					break;
				}
				if(bSigned==false)
					it++;
			}
		}
	}catch(...){
		return 0;
	}
	return EventList.size();
}

void CVLPRClonedDemoDlg::OnClose()
{
	SaveConfig();
	_beginthread(LoadingThread, 0, this);

	//ֹͣ

	if( CloseThread()>0)
		debug("�������˳� @ CDialog::OnClose()  �߳�δȫ���ر�");
	else
		debug("���������˳� @ CDialog::OnClose()");
	Sleep(10);

	CDialog::OnClose();
}

void CVLPRClonedDemoDlg::OnBnClickedBrowserDestDir()
{
	UpdateData(true);

	char *tempdir = 0;
	
	if(m_dstDir.IsEmpty()==false)
	{
		tempdir = new char [256];
		sprintf(tempdir, m_dstDir.GetBuffer(m_dstDir.GetLength()));
	}
	char *path = FileUtil::SelectFolder(this->m_hWnd, "ѡ������ļ���", tempdir);

	if(path!=NULL)
		m_dstDir.Format( path );

	UpdateData(false);
}


bool CVLPRClonedDemoDlg::ReadConfig()
{
	bool ret = 0;
	char lpFileName[512]={0};
	char temp[512]={0};
	sprintf(lpFileName,"%s\\VLPRClonedDemo.ini", CVLPRClonedDemoApp::m_appPath);

	ret = GetPrivateProfileString("config","dstdir", "", temp, 512, lpFileName);
	m_dstDir.Format( temp );
	m_Threshold = GetPrivateProfileInt("config","timeThreshold", 60, lpFileName);

	UpdateData(false);
	return ret;

}

bool CVLPRClonedDemoDlg::SaveConfig()
{
	UpdateData(true);

	bool ret=0;
	char lpFileName[512]={0};
	char retString[512]={0};
	char temp[256]={0};
	sprintf(lpFileName,"%s\\VLPRClonedDemo.ini", CVLPRClonedDemoApp::m_appPath);
	
	if(m_dstDir.GetLength()>3)
		ret =WritePrivateProfileString("config","dstdir", m_dstDir, lpFileName);	
	sprintf(temp, "%d", m_Threshold);
	ret =WritePrivateProfileString("config","timeThreshold", temp, lpFileName);	

	return ret;
}
void CVLPRClonedDemoDlg::OnBnClickedReLoad()
{
	_beginthread(LoadFileThread, 0 ,this);//�����ļ�

}

void CVLPRClonedDemoDlg::OnLbnSelchangeListDirs()
{
	//
	UpdateData(true);
	int index = m_listDirs.GetCurSel();
	if(index<0)
		return ;
	char folder[512]={0}, temp[256]={0};
	m_listDirs.GetText(index, folder);
	list< LPR_Result*> list;
	LPR_Result *p=0;
	if( getLPRList(folder, list) >0){
		int count = list.size();
		m_listLpr.DeleteAllItems();//
		for(int i=0; i<count; i++){
			p = list.front();
			list.pop_front();
			if(p){
				int nRow = m_listLpr.InsertItem(0, "");//
				int cols =0 ;
				m_listLpr.SetItemText(nRow, cols++, p->plate);//����
				m_listLpr.SetItemText(nRow, cols++, p->FormatTime());//ʱ��
				m_listLpr.SetItemText(nRow, cols++, p->plateType);//��������
				m_listLpr.SetItemText(nRow, cols++, p->carLogo);//����
				m_listLpr.SetItemText(nRow, cols++, p->carColor1);//����ɫ
				m_listLpr.SetItemText(nRow, cols++, p->carType);//����
				m_listLpr.SetItemText(nRow, cols++, p->resultPicture);//ͼƬ·��
				sprintf(temp, "%d", p->id);
				m_listLpr.SetItemText(nRow, cols++, temp);//id
			}
		}
	}else{
		GetDlgItem(ID_STATUS_LIST)->SetWindowText("���ļ����޼�¼");
		//MessageBox("���ļ����޼�¼");
	}
}

void CVLPRClonedDemoDlg::OnNMClickListLpr(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	POSITION pos = m_listLpr.GetFirstSelectedItemPosition();
	int nItem = m_listLpr.GetNextSelectedItem(pos);
	CString temp;
	if(nItem>=0){

		temp = m_listLpr.GetItemText(nItem, 6);
		Bitmap* imagePlate = KLoadBitmap(temp.GetBuffer(temp.GetLength()));
		if(imagePlate)
		{
			sprintf(picture1Path, "%s", temp);
			DrawImg2Hdc(imagePlate, ID_VIDEO_WALL, this);
			delete imagePlate;
		}
	}
	
	*pResult = 0;
}
