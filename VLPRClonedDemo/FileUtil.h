// FileUtil.h

/**
* Auth: Karl
* Date: 2014/2/20
* LastUpdate: 2014/2/24
*/

#pragma once

#ifndef HEADER_FILEUTIL
#define HEADER_FILEUTIL

//#include <list>
//#include <string>
//using namespace std;

#include <list>
using namespace std;

#ifndef MAX_PATH
	#define MAX_PATH 256
#endif

class FileUtil
{
public:
	FileUtil(void);
	~FileUtil(void);

	//�г��ļ�������������ļ�
	static list<char*>* ListFiles(char *czPath, list<char*> &listResult, char *fileExt="*.*",bool fullPathName=false, bool bListDirectory=false, bool bListSub=false);
	
	//����ѡ���ļ��жԻ���
	static char* SelectFolder(HWND hwnd, char* title="Select Folder", char* dir=0);

	//�ж��ļ�/�ļ����Ƿ����
	//�ж��ļ���	FindFirstFileExists(lpPath, FALSE);  
	//�ж��ļ��У�	FindFirstFileExists(lpPath, FILE_ATTRIBUTE_DIRECTORY);  
	static BOOL FindFirstFileExists(LPCTSTR lpPath, DWORD dwFilter);

	//�����༶Ŀ¼
	static bool CreateFolders(const char* folderPath);

	//ɾ���༶Ŀ¼�������ļ���
	static bool RemoveDir(const char* szFileDir);

	static char *FormatFileName(const char *path, int indexIn=0, bool fileTime=true, const char *destDir=0);

private:
	


};

char *GetDateTime(char *timeString=0);
void __cdecl debug(const char *format, ...);
void __cdecl release(const char *format, ...);

long __cdecl getCurrentTime();

#endif