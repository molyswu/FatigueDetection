#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include "core.h"
typedef struct TxImage
{
	int rows;
	int cols;
	int channel;
	unsigned char* data;
} TxImage;
struct TxRect;
struct TxRectf;

TxImage* txCreateImage( int rows, int cols, int channel);
void  txReleaseImage( TxImage** _image );
void  txSetZeroImage( TxImage* image );
void txCvtBGR2GRAY(TxImage* imBGR, TxImage* imGRAY);
void txCvtBGR2GR(TxImage* imBGR, TxImage* imGRAY);
void txImageResize(TxImage *src, TxImage *dst);	// ˫���Բ�ֵ
short txResizeImage(TxImage *src, TxImage *dst,int,int);	// ����ڲ�ֵ
short txResizeImage_1(TxImage* Dst,TxImage* Src,int width,int height,int x_b,int x_e,int y_b,int y_e);
void txImageResizeRoi(TxImage *src, TxImage *dst, TxRectf roi);
void txImageResizeArea(TxImage *src, TxImage *dst);
void txPryDown(TxImage *pry1, TxImage *pry2, TxImage *buf);
void txPryDownFull(TxImage *pry1, TxImage *pry2, TxImage *buf);
float txGetHashCode(TxImage *input, TxImage *buf, TxRectf roi, char *hashcode, TxImage *buf64, TxImage *buf32, TxImage *buf16);
void txGetpHashCode(TxImage *input, TxImage *buf,  TxRectf roi, char *hashcode, TxImage *buf64);
int txHammingDistance(char *model, char *test, int len);

#define DHASH 1								// �����ϣ�㷨
#define SIZECONSTRAIN	1				// ǰ����δ�СԼ��

#define SIZESTEP 5							// ���ƹ�ϣ��������

int OtsuTh(TxImage *img);				// �����ֵ�㷨

#ifdef __cplusplus
}
#endif
