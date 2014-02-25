//LPR_info.h

#ifndef _LPR_INFO_HEADER_
#define _LPR_INFO_HEADER_

#include "windows.h"
#include "time.h"

typedef struct TF_Rect{
	int left;
	int top;
	int right;
	int bottom;
	TF_Rect(){
		memset(this, 0, sizeof(TF_Rect));
	}
}TF_Rect;

static char temp[64]={0};

typedef struct LPR_Result
{
	int id;
	char plate[32];//����
	float confidence;//���Ŷ�
	TF_Rect plateRect;//��������
	char  plateType[32];//��������
//	char  plateColor[32];//������ɫ
	
	char  carLogo[32];//����
	char  carColor1[32];//������ɫ
	char  carColor2[32];//��������ɫ
	char  direct[32];//�˶�����

	unsigned char * pResultBits;//����ͼƬ����
	char resultPicture[256];//����ͼƬ��ַ
	int	imageWidth;
	int imageHeight;

	long  takesTime;//��ʱ
	char  lastUpdateTime[64];//ʱ���ַ���
	long  time;//ʱ��long
	
	const char *FormatTime(long timeIn=0)
	{
		if(timeIn==0)
			timeIn = time;
		time_t timer = timeIn;
		struct tm *tblock;
		tblock = localtime(&timer);
		sprintf(temp,"%04d-%02d-%02d %02d:%02d:%02d", tblock->tm_year+1900, tblock->tm_mon+1, tblock->tm_mday, tblock->tm_hour, tblock->tm_min, tblock->tm_sec );

		return temp;
	}
	LPR_Result(){
		memset(this, 0, sizeof(LPR_Result));
	}

}LPR_Result;

typedef struct LPR_ResultPair
{
	LPR_Result lpr_result[2];
	bool set(LPR_Result *r1, LPR_Result *r2)
	{
		if(r1==0 || r2==0)
			return false;
		memcpy( &lpr_result[0], r1, sizeof(LPR_Result) );
		memcpy( &lpr_result[1], r2, sizeof(LPR_Result) );
	}
}LPR_ResultPair;



typedef struct LPR_Image
{
	unsigned char* buffer;
	char filePath[256];
	int  imageWidth;
	int  imageHeight;
	int  imageSize;
	LPR_Image(){
		memset(this, 0, sizeof(LPR_Image));
	}
}LPR_Image;




#endif