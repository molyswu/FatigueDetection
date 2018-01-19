#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "../basic/adas.h"
#include "txGodEyeFaceClassifier.h"
#include "txLocalDefine.h"

typedef struct _TxFatigueWarning
{
	//当前人脸信息
	Face_Info m_faceInfo;

	godEyeFaceEngine m_faceEngine;

	fatigueDetectionPara m_faceDetectionPara;
}TxFatigueWarning;

TxFatigueWarning* initFatigueWarning(const char* modelFolder); //初始化模型和参数

void releaseFatigueWarning(TxFatigueWarning** fw); //释放内存

BOOL processFatigueWarning(TxFatigueWarning *fw, TxImage* img, TxRect rectROIDetection);


void controlFatigueWarning(TxFatigueWarning *fw);

#ifdef __cplusplus
}
#endif