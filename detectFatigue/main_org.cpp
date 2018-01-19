#include "stdafx.h"
extern "C"{
#include "../face/TxLocalDefine.h"

#include "../face/TxFaceDetect.h"
	
#include "../face/TxGodEyeFaceClassifier.h"
#include "../face/TxFatigueWarning.h"
}

#include "../include/config.h"
#include "../basic/system.h"
#include "TestVedio.h"

#include "opencv/cv.h"
#include "opencv/cxcore.h"
#include "opencv/highgui.h"
#include "GraphUtils.h"

#include <io.h>
#include "vector"
#include <atlstr.h>
#include <fstream>
#include <iostream>
#include <string>
#include <direct.h>
#include <stdlib.h>

using namespace std;
using namespace cv;

TxFatigueWarning *FWarning = NULL;

CvRect box;
bool bNextFrame = 0;
int g_frameCount = 0;

TxImage* pimg8;



short testGodEyeFace(IplImage *image, IplImage* image_copy, CvRect faceRegion)
{
	double t = 0;
	IplImage *pMyCopy = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
	cvCvtColor(image, pMyCopy, CV_BGR2GRAY);

	//初始化模型和参数
	if (FWarning == NULL)
		FWarning = initFatigueWarning("../face/faceModels/"); 

	TxImage* pimg8 = txCreateImage(pMyCopy->height, pMyCopy->width, 1);

	int i = 0;
	int j = 0;
	for (i = 0; i < pMyCopy->height; i++)
	{
		for (j = 0; j < pMyCopy->width; j++)
		{
			*(pimg8->data + i*pMyCopy->width + j) = (int)cvGetReal2D(pMyCopy, i, j);
		}
	}

	//控制疲劳检测参数
	controlFatigueWarning(FWarning);

	TxRect rectROI;
	if (!FWarning->m_faceDetectionPara.bSetFaceROI) //bSetFaceROI设置为false,在ROI检测人脸，否则，做全图检测
	{
		rectROI.x = REAR_IMG_WIDTH / 2 - REAR_IMG_WIDTH / 4;
		rectROI.y = 40;
		rectROI.width = 2 * REAR_IMG_WIDTH / 4; 
		rectROI.height = REAR_IMG_HEIGHT - 42;
	}
	else
	{
		rectROI.width = faceRegion.width; 
		rectROI.height = faceRegion.height;
		rectROI.x = faceRegion.x;
		rectROI.y = faceRegion.y;
	}

	FWarning->m_faceInfo.nframe = g_frameCount;

	//处理疲劳检测
	short faceResult = processFatigueWarning(FWarning, pimg8, rectROI);
	

	//输出并画出检测人脸坐标与人脸特征点
	DFW_Ouput output;
	Face_Info myFaceInfo_Output;
	txGetDFWOutput(FWarning, &output);
	myFaceInfo_Output = output.face;
	cvDrawRect(image_copy, cvPoint(myFaceInfo_Output.faceRect.x, myFaceInfo_Output.faceRect.y), cvPoint(myFaceInfo_Output.faceRect.x + myFaceInfo_Output.faceRect.width, myFaceInfo_Output.faceRect.y + myFaceInfo_Output.faceRect.height), cvScalar(0, 255, 255), 2);
	for (int i = 0; i<KEYPOINTNUM; i++)
	{
		cvDrawCircle(image_copy, cvPoint((myFaceInfo_Output.eyePoints + i)->data[0], (myFaceInfo_Output.eyePoints + i)->data[1]), 1, cvScalar(255, 0, 0), 1);
	}

	cvShowImage("Box Example", image_copy);
	cvReleaseImage(&pMyCopy);
	txReleaseImage(&pimg8);

	return 1;
}

/* G7 ADAS 初始化 */
void G7AdasInit(int nW, int nH, char* pPath)
{
	FWarning = initFatigueWarning(pPath);

	if (FWarning == NULL)
	{
		printf("error in initFatigueWarning \ n");
	}

	pimg8 = txCreateImage(nH, nW, 1);

	if (pimg8 == NULL)
	{
		printf("error in txCreateImage \ n");
	}

	return;
}


/* G7 ADAS 实际处理过程 */
void G7AdasProc(int nW, int nH, IplImage *in_image, IplImage *out_image, CvRect faceRegion)
{
	int i, j;
	TxRect rectROI;

	for (int i = 0; i < nH; i++)
	{
		for (int j = 0; j < nW; j++)
		{
			*(pimg8->data + i*nW + j) = (int)cvGetReal2D(in_image, i, j);
		}
	}

	if (!FWarning->m_faceDetectionPara.bSetFaceROI) //bSetFaceROI设置为false,在ROI检测人脸，否则，做全图检测
	{
		rectROI.x = REAR_IMG_WIDTH / 2 - REAR_IMG_WIDTH / 4;
		rectROI.y = 40;
		rectROI.width = 2 * REAR_IMG_WIDTH / 4;
		rectROI.height = REAR_IMG_HEIGHT - 42;
	}
	else
	{
		rectROI.width  = faceRegion.width;
		rectROI.height = faceRegion.height;
		rectROI.x      = faceRegion.x;
		rectROI.y      = faceRegion.y;
	}

	//处理疲劳检测
	short faceResult = processFatigueWarning(FWarning, pimg8, rectROI);


	//输出并画出检测人脸坐标与人脸特征点
	DFW_Ouput output;
	Face_Info myFaceInfo_Output;
	txGetDFWOutput(FWarning, &output);
	myFaceInfo_Output = output.face;
	cvDrawRect(out_image, cvPoint(myFaceInfo_Output.faceRect.x, myFaceInfo_Output.faceRect.y), cvPoint(myFaceInfo_Output.faceRect.x + myFaceInfo_Output.faceRect.width, myFaceInfo_Output.faceRect.y + myFaceInfo_Output.faceRect.height), cvScalar(0, 255, 255), 2);
	for (int i = 0; i<KEYPOINTNUM; i++)
	{
		cvDrawCircle(out_image, cvPoint((myFaceInfo_Output.eyePoints + i)->data[0], (myFaceInfo_Output.eyePoints + i)->data[1]), 1, cvScalar(255, 0, 0), 1);
	}

	return;
}

/* G7 ADAS 控制处理过程 */
void G7AdasCtrl()
{
	controlFatigueWarning(FWarning);

	return;
}


void G7AdasRelease()
{
	if (pimg8 != NULL)
		txReleaseImage(&pimg8);

	
	if (FWarning != NULL)
		releaseFatigueWarning(&FWarning);

	return;
}


int _tmain(int argc, _TCHAR* argv[])
{
	IplImage* image_input_org  = NULL;
	IplImage* image_input_gray = NULL;
	IplImage* image_output = NULL;

	char* pPath = "../face/faceModels/";

	CvCapture* capture = cvCaptureFromCAM(0);

	IplImage* image_input_size = cvCreateImage(cvSize(REAR_IMG_WIDTH, REAR_IMG_HEIGHT), IPL_DEPTH_8U, 1);
	
	image_input_org = cvQueryFrame(capture);
	image_input_gray = cvCreateImage(cvGetSize(image_input_org), IPL_DEPTH_8U, 1);


	box = cvRect(-1, -1, 0, 0);


	G7AdasInit(REAR_IMG_WIDTH, REAR_IMG_HEIGHT, pPath);


	while (1)
	{
		int key = cvWaitKey(1);
		image_input_org = cvQueryFrame(capture);
		
		if (!image_input_org)
			break;

		cvCvtColor(image_input_org, image_input_gray, CV_BGR2GRAY);

		cvResize(image_input_gray, image_input_size);

		image_output = cvCloneImage(image_input_size);

		G7AdasCtrl();

		G7AdasProc(REAR_IMG_WIDTH, REAR_IMG_HEIGHT, image_input_size, image_output, box);

		cvShowImage("Box Example", image_output);

		g_frameCount++;

		if ((key == 'q' || key == 'Q'))
		{
			break;
		}
	}

	G7AdasRelease();
	cvDestroyWindow("Box Example");

	return 1;
}



int _tmain_old(int argc, _TCHAR* argv[])
{
	IplImage* image_input = NULL;
	IplImage* image_input_1 = NULL;

	cvNamedWindow("Box Example");



	while (1)
	{
		g_frameCount = 0;
		bool bStartPro = false;

		CvCapture* capture = cvCaptureFromCAM(0);
		//cvWaitKey(500);
		while (1)
		{
			int key = cvWaitKey(1);
			image_input = cvQueryFrame(capture);

			if (!image_input)
				break;
			if (image_input_1)
				cvReleaseImage(&image_input_1);

			box = cvRect(-1, -1, 0, 0);
// 竖屏
#if 0
			image_input_1 = cvCreateImage(cvSize(image_input->height, image_input->width), image_input->depth, image_input->nChannels);
			cvTranspose(image_input, image_input_1);
			IplImage* resizeImage = cvCreateImage(cvSize(REAR_IMG_HEIGHT, REAR_IMG_WIDTH), IPL_DEPTH_8U, image_input_1->nChannels);
			cvResize(image_input_1, resizeImage);
#else
			IplImage* resizeImage = cvCreateImage(cvSize(REAR_IMG_WIDTH, REAR_IMG_HEIGHT), IPL_DEPTH_8U, image_input->nChannels);
			cvResize(image_input, resizeImage);
#endif
			IplImage* image = cvCloneImage(resizeImage);
			IplImage* image_copy = cvCloneImage(image);
			IplImage* temp = cvCloneImage(image_copy);

			while (1)
			{
				if (bNextFrame && (g_frameCount % FACE_PROCESS_STEP != 0))
					break;
				cvCopy(image_copy, temp);
				cvShowImage("Box Example", temp);
				key = cvWaitKey(1);
				if (key == 27)
					break;
				if ((key == 'p' || key == 'P') || bStartPro)
				{
					bStartPro = true;
					short detectorState = testGodEyeFace(image, image_copy, box);
					bNextFrame = true;
					break;
				}
			}
			cvReleaseImage(&image_copy);
			cvReleaseImage(&temp);
			cvReleaseImage(&image);
			cvReleaseImage(&resizeImage);
			g_frameCount++;
		}
		cvReleaseCapture(&capture);
		
	}
	cvDestroyWindow("Box Example");
	//释放内存
	if (FWarning)
		releaseFatigueWarning(&FWarning);
	system("pause");
	return 0;
}