#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "../basic/adas.h"
#include "txGodEyeFaceClassifier.h"
#include "txLocalDefine.h"

typedef struct _TxFatigueWarning
{
	//��ǰ������Ϣ
	Face_Info m_faceInfo;

	godEyeFaceEngine m_faceEngine;

	fatigueDetectionPara m_faceDetectionPara;
}TxFatigueWarning;

TxFatigueWarning* initFatigueWarning(const char* modelFolder); //��ʼ��ģ�ͺͲ���

void releaseFatigueWarning(TxFatigueWarning** fw); //�ͷ��ڴ�

BOOL processFatigueWarning(TxFatigueWarning *fw, TxImage* img, TxRect rectROIDetection);


void controlFatigueWarning(TxFatigueWarning *fw);

#ifdef __cplusplus
}
#endif