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
	double t;

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

	t = cvGetTickCount();
	//处理疲劳检测
	short faceResult = processFatigueWarning(FWarning, pimg8, rectROI);

	t = (double)cvGetTickCount() - t;

	printf("processFatigueWarning Time ********** : %fms\n", t / (cvGetTickFrequency() * 1000));

	//输出并画出检测人脸坐标与人脸特征点
	Face_Info myFaceInfo_Output;
	myFaceInfo_Output = FWarning->m_faceInfo;

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

		// image_output = cvCloneImage(image_input_size);
		image_output = image_input_size;

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



