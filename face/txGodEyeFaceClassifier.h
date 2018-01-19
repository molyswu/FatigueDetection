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

	TxRect regionROI;           // �ھֲ�����������
	TxRect regionROISave;       //�ֲ���������������
	TxRect faceROI;             //������ŵ�rect��С
	TxRect rectForSDM;       // face alignment��������������

	int faceWidthFullDetect;  // ȫͼ���ʱ����ɨ���face�Ŀ��

	TxVec2f *pPointOut;      //����Ĺؼ���
	TxVec2f *pPointOutLastFrame;      //��һ֡����Ĺؼ���
	TxRect *pRect;      // ��������
	float *pSIFTBuffer;     //�����sift����

#if COLOR_FEATURE
	float *pSIFTCol;		// ������(128+9)*2����
	float *pSIFTCol1;
#endif

	TxRect *pFaceRectUse;
	TxRect *pFaceRectSave;

	char *p_MemoryEyeService_Model;
	char *p_MemoryFaceService_Model;
	char *p_MemoryMouthService_Model;
	char *p_MemorySunGlassesService_Model;

	faceDetector myFaceDetector;                  // ���������

	FaceReg facedetectorgong;

	long nProcessFrame;                    // �㷨�����֡��������

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