#pragma once
#ifndef TXGODEYECLASSIFIER_H
#define TXGODEYECLASSIFIER_H

#include "txLocalDefine.h"
#include "txFaceDetect.h"
#include "txGodEyeFaceClassifier.h"
#include "face3000.h"


#ifdef __cplusplus
extern "C"{
#endif

typedef struct _godEyeFaceEngine
{
	
	short bGotFaceSuc;
	short bGotFaceFromROI;
	short bFirstAlignment;
	short bStartFaceDetector;
	short bNoNeedResetDetector;
	short notFaceNumWithSunglass;

	int countNoFaceFrame;
	int FaceRectUseCount;
	int FaceRectSaveCount;

	float offsetOfFrames[2];

	TxRect regionROI;           // 在局部区域检测人脸
	TxRect regionROISave;       //局部区域检测人脸拷贝
	TxRect faceROI;             //人脸大概的rect大小
	TxRect rectForSDM;       // face alignment的输入人脸检测框

	int faceWidthFullDetect;  // 全图检测时首先扫描的face的宽度

	TxVec2f *pPointOut;      //输出的关键点
	TxVec2f *pPointOutLastFrame;      //上一帧输出的关键点
	TxRect *pRect;      // 人脸检测框
	float *pSIFTBuffer;     //输出的sift特征

#if COLOR_FEATURE
	float *pSIFTCol;		// 左右眼(128+9)*2特征
	float *pSIFTCol1;
#endif

	TxRect *pFaceRectUse;
	TxRect *pFaceRectSave;

	char *p_MemoryEyeService_Model;
	char *p_MemoryFaceService_Model;
	char *p_MemoryMouthService_Model;
	char *p_MemorySunGlassesService_Model;

	faceDetector myFaceDetector;                  // 人脸检测器

	FaceReg facedetectorgong;

	long nProcessFrame;                    // 算法处理的帧数计数器

	short rblack_state_left;
	short rblack_state_right;

}godEyeFaceEngine;

short txGodEyeFaceMain(godEyeFaceEngine *myGodEyeFaceEngine, TxImage* pimg8, TxRect rectROIDetection, Face_Info *myFaceInfo_Output, fatigueDetectionPara *myFatigueDetectionPara);
short txInitGodEyeFace(const char* modelFolder, godEyeFaceEngine *myGodEyeFaceEngine);
void txReleaseGodEyeFace(godEyeFaceEngine *myGodEyeFaceEngine);

#ifdef __cplusplus
}
#endif 

#endif