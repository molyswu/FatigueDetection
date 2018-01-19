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
void txImageResize(TxImage *src, TxImage *dst);	// 双线性插值
short txResizeImage(TxImage *src, TxImage *dst,int,int);	// 最近邻插值
short txResizeImage_1(TxImage* Dst,TxImage* Src,int width,int height,int x_b,int x_e,int y_b,int y_e);
void txImageResizeRoi(TxImage *src, TxImage *dst, TxRectf roi);
void txImageResizeArea(TxImage *src, TxImage *dst);
void txPryDown(TxImage *pry1, TxImage *pry2, TxImage *buf);
void txPryDownFull(TxImage *pry1, TxImage *pry2, TxImage *buf);
float txGetHashCode(TxImage *input, TxImage *buf, TxRectf roi, char *hashcode, TxImage *buf64, TxImage *buf32, TxImage *buf16);
void txGetpHashCode(TxImage *input, TxImage *buf,  TxRectf roi, char *hashcode, TxImage *buf64);
int txHammingDistance(char *model, char *test, int len);

#define DHASH 1								// 差异哈希算法
#define SIZECONSTRAIN	1				// 前后矩形大小约束

#define SIZESTEP 5							// 控制哈希重启步长

int OtsuTh(TxImage *img);				// 大津阈值算法

#ifdef __cplusplus
}
#endif
