#include "image.h"
#include "core.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI  3.14159265358979323846
#endif
#define DBL_EPSILON     2.2204460492503131E-16

#define G7ABS(x)  ( (x)>0?(x):-(x) )


TxImage* txCreateImage( int rows, int cols, int channel)
{
	TxImage *im;

	if(rows <= 0 || cols <= 0 || channel <= 0)
	{
		return 0;
	}

	im = (TxImage *)txAlloc(sizeof(TxImage));
	im->cols = cols;
	im->rows = rows;
	im->channel = channel;
	im->data = (unsigned char*)txAlloc(sizeof(unsigned char)*cols*rows*channel);
	return im;
}
void  txReleaseImage( TxImage** _image )
{
	TxImage * image;
	if (!_image)
	{
		return;
	}
	image = *_image;
	if(image == NULL || image->data == NULL)
	{
		return;
	}

	txFree(image->data);	
	txFree(image);
	*_image = NULL;
}
void  txSetZeroImage( TxImage* image )
{
	memset(image->data, 0, sizeof(unsigned char) * image->cols * image->rows * image->channel);
}

void txCvtBGR2GRAY(TxImage* imBGR, TxImage* imGRAY)
{
	int i, j;
	int nr = imBGR->rows;
	int nc = imBGR->cols;
	unsigned char *dstData;
	unsigned char *srcData = imBGR->data;
	for(i = 0; i < nr; i++)
	{
		dstData = imGRAY->data + i*nc;
		for(j = 0; j < nc; j++)
		{
			*(dstData+j) = ((*srcData)*38 + *(srcData+1)*75 + *(srcData+2)*15) >> 7;
			srcData = srcData + 3;
		}
	}
	return;
}

void txCvtBGR2GR(TxImage* imBGR, TxImage* imGRAY)
{
	int i, j;
	int nr = imBGR->rows;
	int nc = imBGR->cols;
	unsigned char *dstData;
	unsigned char *srcData = imBGR->data;
	for(i = 0; i < nr; i++)
	{
		dstData = imGRAY->data + i*nc;
		for(j = 0; j < nc; j++)
		{
			*(dstData+j) = (*(srcData+1)*3 + *(srcData+2)*5) >> 3;
			srcData = srcData + 3;
		}
	}
	return;
}

short txResizeImage(TxImage* Dst,TxImage* Src,int width,int height)
{
    
    unsigned short dst_width = 0;
    unsigned short SrcX_Table[3000];
	unsigned short y = 0;
	unsigned short x = 0;
    unsigned char* pDstLine;
	unsigned short srcy;
	unsigned char* pSrcLine;

	Dst->cols = width;
	Dst->rows = height;

	if ((0==Dst->cols)||(0==Dst->rows) ||(0==Src->cols)||(0==Src->rows) || Dst->cols >= 2998)
		return -1;
	dst_width = Dst->cols;

    for (x=0;x<dst_width;++x)//生成表 SrcX_Table
    {
        SrcX_Table[x]=(x*Src->cols/Dst->cols);
    }
	pDstLine = Dst->data;
    for (y=0;y<Dst->rows;++y)
    {
        srcy=(y*Src->rows/Dst->rows);
        pSrcLine=(Src->data+Src->cols*srcy);
        for (x=0;x<dst_width;++x)
            pDstLine[x]=pSrcLine[SrcX_Table[x]];
           (pDstLine)+= Dst->cols;
    }
	return 1;
}

short txResizeImage_1(TxImage* Dst,TxImage* Src,int width,int height,int x_b,int x_e,int y_b,int y_e)
{

	unsigned short dst_width = 0;
	unsigned short SrcX_Table[3000];
	unsigned short y = 0;
	unsigned short x = 0;
	unsigned char* pDstLine;
	unsigned short srcy;
	unsigned char* pSrcLine;

	Dst->cols = width;
	Dst->rows = height;

	if ((0==Dst->cols)||(0==Dst->rows) ||(0==Src->cols)||(0==Src->rows) || Dst->cols >= 2998)
		return -1;
	dst_width = Dst->cols;

	for (x=0;x<dst_width;++x)//生成表 SrcX_Table
	{
		if(x>=x_b && x<=x_e)
			SrcX_Table[x]=(x*Src->cols/Dst->cols);
	}
	pDstLine = Dst->data;
	for (y=0;y<Dst->rows;++y)
	{
		srcy=(y*Src->rows/Dst->rows);
		pSrcLine=(Src->data+Src->cols*srcy);
		if(y>=y_b && y<=y_e)
		{
			for (x=x_b;x<x_e;++x)
			{
			//	if(x>=x_b && x<=x_e)
					pDstLine[x]=pSrcLine[SrcX_Table[x]];
			}
		}
	
		
		(pDstLine)+= Dst->cols;
	}
	return 1;

}

void txImageResize(TxImage *src, TxImage *dst)
{
	const int dstW = dst->cols, dstH = dst->rows, srcW = src->cols, srcH = src->rows;
	int channel = src->channel;
	float tauX = (float)(srcW)/(float)(dstW);
	float tauY = (float)(srcH)/(float)(dstH);
	int i,j;
	float isrc;
	int itop;
	int ui, vi;
	unsigned char * srcColor1;
	unsigned char * srcColor2;
	unsigned char * dstColor;
	int leftInd;
	float jsrc;
	int jleftArray[640],ujArray[640], vjArray[640];

	for(j = 0; j < dstW; j++)
	{
		jsrc = (j+0.5)*tauX-0.5;
		jleftArray[j] = (int)jsrc;
		ujArray[j] = (int)((jsrc - jleftArray[j])*2048);
		vjArray[j] = 2048 - ujArray[j];
	}

	if(channel == 1)
	{
		for(i = 0; i < dstH; i++)
		{
			isrc = (i+0.5)*tauY - 0.5;
			itop = (int)isrc;
			ui = (int)((isrc - itop)*2048);
			vi = 2048 - ui;
			dstColor = dst->data + i*dstW*channel;
			srcColor1 = src->data + itop*srcW*channel;
			srcColor2 = src->data + (itop+1)*srcW*channel; 
			for(j = 0; j < dstW; j++)
			{
				/**(dstColor+j) = ((srcColor1[jleftArray[j]]*vjArray[j] + srcColor1[jleftArray[j] + 1]*ujArray[j])*vi 
				+ (srcColor2[jleftArray[j]]*vjArray[j] + srcColor2[jleftArray[j] + 1]*ujArray[j])*ui)>>22;*/
				*(dstColor+j) = ((srcColor1[jleftArray[j]]*vi + srcColor2[jleftArray[j]]*ui)*vjArray[j] 
				+ (srcColor1[jleftArray[j] + 1]*vi+ srcColor2[jleftArray[j] + 1]*ui)*ujArray[j])>>22;
			}
		}
	}
	else
	{
		for(i = 0; i < dstH; i++)
		{
			isrc = (i+0.5)*tauY - 0.5;
			itop = (int)isrc;
			ui = (int)((isrc - itop)*2048);
			vi = 2048 - ui;
			dstColor = dst->data + i*dstW*channel;
			srcColor1 = src->data + itop*srcW*channel;
			srcColor2 = src->data + (itop+1)*srcW*channel; 
			for(j = 0; j < dstW; j++)
			{
				leftInd = jleftArray[j]*3;

				*(dstColor) = ((srcColor1[leftInd]*vi + srcColor2[leftInd]*ui)*vjArray[j] 
				+ (srcColor1[leftInd+3]*vi+ srcColor2[leftInd+3]*ui)*ujArray[j])>>22;

				*(dstColor+1) = ((srcColor1[leftInd+1]*vi + srcColor2[leftInd+1]*ui)*vjArray[j] 
				+ (srcColor1[leftInd+4]*vi+ srcColor2[leftInd+4]*ui)*ujArray[j])>>22;

				*(dstColor+2) = ((srcColor1[leftInd+2]*vi + srcColor2[leftInd+2]*ui)*vjArray[j] 
				+ (srcColor1[leftInd+5]*vi+ srcColor2[leftInd+5]*ui)*ujArray[j])>>22;

				dstColor = dstColor + 3;
			}
		}
	}
}

void txImageResizeRoi(TxImage *src, TxImage *dst, TxRectf roi)
{
	const int dstW = dst->cols, dstH = dst->rows, srcW = src->cols, srcH = src->rows;
	int channel = src->channel;
	float tauX = roi.width/(float)(dstW);
	float tauY = roi.height/(float)(dstH);
	int i,j;
	float isrc;
	int itop;
	int ui, vi;
	unsigned char * srcColor1;
	unsigned char * srcColor2;
	unsigned char * dstColor;
	int leftInd;
	float jsrc;
	int jleftArray[180],ujArray[180], vjArray[180];

	for(j = 0; j < dstW; j++)
	{
		jsrc = (j+0.5)*tauX-0.5 + roi.x;
		jleftArray[j] = (int)jsrc;
		ujArray[j] = (int)((jsrc - jleftArray[j])*2048);
		vjArray[j] = 2048 - ujArray[j];
	}

	if(channel == 1)
	{
		for(i = 0; i < dstH; i++)
		{
			isrc = (i+0.5)*tauY - 0.5 + roi.y;
			itop = (int)isrc;
			ui = (int)((isrc - itop)*2048);
			vi = 2048 - ui;
			dstColor = dst->data + i*dstW*channel;
			srcColor1 = src->data + itop*srcW*channel;
			srcColor2 = src->data + (itop+1)*srcW*channel; 
			for(j = 0; j < dstW; j++)
			{
				*(dstColor+j) = ((srcColor1[jleftArray[j]]*vi + srcColor2[jleftArray[j]]*ui)*vjArray[j] 
				+ (srcColor1[jleftArray[j] + 1]*vi+ srcColor2[jleftArray[j] + 1]*ui)*ujArray[j])>>22;
			}
		}
	}
}

void txImageResizeArea(TxImage *src, TxImage *dst)
{
	const int dstW = dst->cols, dstH = dst->rows, srcW = src->cols, srcH = src->rows;
	int channel = src->channel;
	int i,j;
	//float scale_x = roi.width/(float)(dstW);
	//float scale_y = roi.height/(float)(dstH);
	float scale_x = (float)(srcW)/(float)(dstW);
	float scale_y = (float)(srcH)/(float)(dstH);
	double inv_scale_x = 1. / scale_x;
	double inv_scale_y = 1. / scale_y;
	int iscale_x = (int)(scale_x);
	int iscale_y = (int)(scale_y);
	//bool is_area_fast = abs(scale_x - iscale_x) < DBL_EPSILON && abs(scale_y - iscale_y) < DBL_EPSILON;
	unsigned char * srcColor1;
	unsigned char * srcColor2;
	unsigned char * dstColor;
	int sy, sx, sy1, sy2, sx1, sx2;
	double fsy1, fsy2, fsx1, fsx2;
	double cellHeight, cellWidth;
	float cbufy[2];
	float cbufx[2];


	if (G7ABS(scale_x - iscale_x) < DBL_EPSILON && G7ABS(scale_y - iscale_y) < DBL_EPSILON) //integer multiples
	{
		for (j = 0; j < dstH; ++j)
		{
			sy = j * scale_y;
			dstColor = dst->data + j*dstW*channel;
			srcColor1 = src->data + sy*srcW*channel;
			for (i = 0; i < dstW; ++i)
			{
				sx = i * scale_x;
				*(dstColor + i) = *(srcColor1 + sx);
			}
		}
		return;
	}

	for (j = 0; j < dstH; ++j)
	{
		fsy1 = j * scale_y;
		fsy2 = fsy1 + scale_y;
		cellHeight = MIN(scale_y, srcH - fsy1);

		sy1 = (int)(fsy1+1); sy2 = (int)(fsy2);

		sy2 = MIN(sy2, srcH - 1);
		sy1 = MIN(sy1, sy2);


		cbufy[0] = (float)((sy1 - fsy1) / cellHeight);
		cbufy[1] = (float)(MIN(MIN(fsy2 - sy2, 1.), cellHeight) / cellHeight);

		dstColor = dst->data + j*dstW*channel;
		srcColor1 = src->data + sy1*srcW*channel;
		srcColor2 = src->data + (sy1+1)*srcW*channel; 
		for (i = 0; i < dstW; ++i)
		{
			fsx1 = i * scale_x;
			fsx2 = fsx1 + scale_x;
			cellWidth = MIN(scale_x, srcW - fsx1);

			sx1 = (int)(fsx1+1); sx2 = (int)(fsx2);

			sx2 = MIN(sx2, srcW - 1);
			sx1 = MIN(sx1, sx2);


			cbufx[0] = (float)((sx1 - fsx1) / cellWidth);
			cbufx[1] = (float)(MIN(MIN(fsx2 - sx2, 1.), cellWidth) / cellWidth);

			*(dstColor+i) =  (unsigned char)(srcColor1[sx1]* cbufx[0] * cbufy[0] + 
				srcColor2[sx1] * cbufx[0] * cbufy[1] + 
				srcColor1[sx1+1] * cbufx[1] * cbufy[0] + 
				srcColor2[sx1+1] * cbufx[1] * cbufy[1]);
		}
	}
	return;
}

void txImageResize2(TxImage *src, TxImage *dst, TxRectf roi, TxImage *buf64, TxImage *buf32, TxImage *buf16)
{
	const int dstW = dst->cols, dstH = dst->rows, srcW = src->cols, srcH = src->rows;
	int channel = src->channel;
	float tauX = roi.width/(float)(dstW);
	float tauY = roi.height/(float)(dstH);

	if(tauX > 8 || tauY > 8)
	{
		txImageResizeRoi(src, buf64, roi);
		txImageResize(buf64, buf32);
		txImageResize(buf32, buf16);
		txImageResize(buf16, dst);
	}
	else if(tauX > 4 || tauY > 4)
	{
		txImageResizeRoi(src, buf32, roi);
		txImageResize(buf32, buf16);
		txImageResize(buf16, dst);
	}
	else if(tauX > 2 || tauY > 2)
	{
		txImageResizeRoi(src, buf16, roi);
		txImageResize(buf16, dst);
	}
	else
	{
		txImageResizeRoi(src, dst, roi);
	}
}

void txImageResize3(TxImage *src, TxImage *dst, TxRectf roi, TxImage *buf64)
{
	const int dstW = dst->cols, dstH = dst->rows, srcW = src->cols, srcH = src->rows;
	int channel = src->channel;
	float tauX = roi.width/(float)(dstW);
	float tauY = roi.height/(float)(dstH);

	if(tauX > 2 || tauY > 2)
	{
		txImageResizeRoi(src, buf64, roi);
		txImageResize(buf64, dst);
	}
	else
	{
		txImageResizeRoi(src, dst, roi);
	}
}

float txGetHashCode(TxImage *input, TxImage *buf,  TxRectf roi, char *hashcode, TxImage *buf64, TxImage *buf32, TxImage *buf16)
{
	int i, j;
	int tmp, leftVal = 0, rightVal = 0;
	float meanVal = 0;
	txImageResize2(input, buf, roi, buf64, buf32, buf16);
#if DHASH
	{
		for (i = 0; i < buf->rows; i++)
		{
			tmp = i*(buf->cols - 1);

			for (j = 0; j < buf->cols - 1; j++)
			{
				leftVal = buf->data[tmp + j];
				leftVal = leftVal >> 2;

				rightVal = buf->data[tmp + j + 1];
				rightVal = rightVal >> 2;

				if (leftVal > rightVal)
				{
					hashcode[tmp + j] = 1;
				}
				else
				{
					hashcode[tmp + j] = 0;
				}
			}
		}
	}
#else
	for (i = 0; i < buf->rows; i++)
	{
		tmp = i*buf->cols;
		for (j = 0; j < buf->cols; j++)
		{
			//buf->data[tmp+j] = buf->data[tmp+j]/4;
			meanVal = meanVal + buf->data[tmp + j];
		}
	}
	meanVal = meanVal / (buf->rows*buf->cols);

	for (i = 0; i < buf->rows; i++)
	{
		tmp = i*buf->cols;
		for (j = 0; j < buf->cols; j++)
		{
			if (buf->data[tmp + j] >= meanVal)
			{
				hashcode[tmp + j] = 1;
			}
			else
			{
				hashcode[tmp + j] = 0;
			}
		}
	}
#endif
	return meanVal;
}

void txGetpHashCode(TxImage *input, TxImage *buf,  TxRectf roi, char *hashcode, TxImage *buf64)
{
	int i,j;
	int tmp;
	float meanVal = 0;
	double acc;
	int u, v, x, y, seq_i;
	float seq[64];
	float coef[8];
	coef[0] = 0.7071;
	for(i = 1; i < 8; i++)
	{
		coef[i] = 1;
	}
	seq_i = 0;
	txImageResize3(input,buf,roi,buf64);
	for (u = 0; u < 8; ++u) {
		for (v = 0; v < 8; ++v) {
			acc = 0.0;
			for (x = 0; x < buf->rows; ++x) {
				tmp = x*buf->cols;
				for (y = 0; y < buf->cols; ++y) {
					acc += buf->data[tmp+y]
					* cos(M_PI / 32.0 * (x + .5) * u)
						* cos(M_PI / 32.0 * (y + .5) * v);
				}
			}
			seq[seq_i++] = acc*coef[u]*coef[v]/4.0;
		}
	}

	for(i = 0; i < 64; i++)
	{
		meanVal = meanVal + seq[i];
	}
	//meanVal = meanVal - seq[0];
	meanVal = meanVal/64;
	//for(i = 0; i < 8; i++)
	//{
	//	for(j = 0; j < 8; j++)
	//	{
	//		printf("%f,",seq[i*8+j]);
	//	}
	//	printf("\n");
	//}
	for(i = 0; i < 64; i++)
	{

		if(seq[i] >= meanVal)
		{
			hashcode[i] = 1;
		}
		else
		{
			hashcode[i] = 0;
		}
	}
	return;
}

int txHammingDistance(char *model, char *test, int len)
{
	int i;
	int dis = 0;
	for(i = 0; i < len; i++)
	{
		dis = dis + (model[i] != test[i]);
	}
	return dis;
}

void txPryDown11(TxImage *pry1, TxImage *pry2, TxImage *buf)
{
	int step = pry1->cols;
	int h_cut = pry2->rows*0.1;
	int w_cut = pry2->cols*0.1;
	int hhstart = pry2->rows*0.8/3 + pry2->rows*0.1;				// 对图像上面的1/3不进行处理
	int hhend = pry2->rows - h_cut;
	int wwstart = w_cut;
	int wwend = pry2->cols - w_cut;
	int hh = pry2->rows, ww = pry2->cols;
	int i,j,k;
	unsigned char *src;
	unsigned char *row;
	unsigned char *dst;
	unsigned char *row0, *row1, *row2, *row3, *row4;

	//printf("%d,%d,%d,%d,%d\n",hhstart,hhend,wwstart,wwend,w_cut);
	for(i = hhstart; i < hhend; i++)
	{
		dst = pry2->data + i * ww;
		for(k = -2; k < 3; k++)
		{
			src = pry1->data + (i*2+k) * step;
			row = buf->data + (2+k) * ww;
			//row[0] = src[0]; 
			for(j = wwstart; j < wwend; j++)
			{
				row[j] = (src[j*2]*6 + (src[j*2 - 1] + src[j*2 + 1])*4 +
					src[j*2 - 2] + src[j*2 + 2])>>4;
			}
			//row[ww-1] = src[step-1];
		}
		row0 = buf->data;
		row1 = buf->data + ww;
		row2 = buf->data + 2*ww;
		row3 = buf->data + 3*ww;
		row4 = buf->data + 4*ww;
		for(j = wwstart; j < wwend; j++)
			dst[j] = (row2[j]*6 + (row1[j] + row3[j])*4 + row0[j] + row4[j])>>4;
	} 
}

void txPryDown(TxImage *pry1, TxImage *pry2, TxImage *buf)
{
	int step = pry1->cols;
	int h_cut = pry2->rows*0.1;
	int w_cut = pry2->cols*0.1;
	int hhstart = pry2->rows*0.8/3 + pry2->rows*0.1;				// 对图像上面的1/3不进行处理
	int hhend = pry2->rows - h_cut;
	int wwstart = w_cut;
	int wwend = pry2->cols - w_cut;
	int hh = pry2->rows, ww = pry2->cols;
	int i,j,k;
	unsigned char *src;
	unsigned char *row;
	unsigned char *dst;
	unsigned char *row0, *row1, *row2, *row3, *row4;

	for(i = hhstart*2-2; i < hhend*2+2; i++)
	{
		row = buf->data + i * ww;
		src = pry1->data + i * step;
		for(j = wwstart; j < wwend; j++)
		{
			row[j] = (src[j*2]*6 + (src[j*2 - 1] + src[j*2 + 1])*4 +
				src[j*2 - 2] + src[j*2 + 2])>>4;
		}
	} 
	for(i = hhstart; i < hhend; i++)
	{
		dst = pry2->data + i * ww;
		row0 = buf->data + (i*2 - 2) * ww;
		row1 = buf->data + (i*2 - 1) * ww;
		row2 = buf->data + (i*2) * ww;
		row3 = buf->data + (i*2 + 1) * ww;
		row4 = buf->data + (i*2 + 2) *ww;
		for(j = wwstart; j < wwend; j++)
			dst[j] = (row2[j]*6 + (row1[j] + row3[j])*4 + row0[j] + row4[j])>>4;
	}
}

void txPryDownFull(TxImage *pry1, TxImage *pry2, TxImage *buf)
{
	int step = pry1->cols;
	int hhstart = pry2->rows/3;				// 对图像上面的1/3不进行处理
	int hh = pry2->rows, ww = pry2->cols;
	int i,j,k;
	unsigned char *src;
	unsigned char *row;
	unsigned char *dst;
	unsigned char *row0, *row1, *row2, *row3, *row4;

	for(i = hhstart; i < hh-1; i++)
	{
		dst = pry2->data + i * ww;
		for(k = -2; k < 3; k++)
		{
			src = pry1->data + (i*2+k) * step;
			row = buf->data + (2+k) * ww;
			row[0] = src[0]; 
			for(j = 1; j < ww-1; j++)
			{
				row[j] = (src[j*2]*6 + (src[j*2 - 1] + src[j*2 + 1])*4 +
					src[j*2 - 2] + src[j*2 + 2])>>4;
			}
			row[ww-1] = src[step-1];
		}
		row0 = buf->data;
		row1 = buf->data + ww;
		row2 = buf->data + 2*ww;
		row3 = buf->data + 3*ww;
		row4 = buf->data + 4*ww;
		for(j = 0; j < ww; j++)
			dst[j] = (row2[j]*6 + (row1[j] + row3[j])*4 + row0[j] + row4[j])>>4;
	} 

	dst = pry2->data + (hh-1)*ww;
	src = pry1->data + (pry1->rows-1)*step;
	for(j = 0; j < ww; j++)
	{
		dst[j] = src[j*2];
	}
}