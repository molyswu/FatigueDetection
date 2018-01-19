#include "txFatigueWarning.h"
#include "../basic/system.h"
#include "../basic/core.h"

TxFatigueWarning* initFatigueWarning(const char* modelFolder)
{
	int i;
	TxFatigueWarning *fw = (TxFatigueWarning*)txAlloc(sizeof(TxFatigueWarning));
	memset(fw, 0, sizeof(TxFatigueWarning));

	if (txInitGodEyeFace(modelFolder, &fw->m_faceEngine) < 0)
	{
		txFree(fw);
		return NULL;
	}
	return fw;
}

void releaseFatigueWarning(TxFatigueWarning** ppfw)
{
	TxFatigueWarning *fw = *ppfw;
	if (fw)
	{
		txReleaseGodEyeFace(&fw->m_faceEngine);
		txFree(fw);
	}

	*ppfw = NULL;
}


void controlFatigueWarning(TxFatigueWarning *fw)
{
	fw->m_faceDetectionPara.face_max_size_detect = 300;
	fw->m_faceDetectionPara.face_min_size_detect = 50;
	fw->m_faceDetectionPara.face_size_offset = 60;
	fw->m_faceDetectionPara.bSetFaceROI = TRUE;
}

BOOL processFatigueWarning(TxFatigueWarning *fw, TxImage* img, TxRect rectROIDetection)
{
	txGodEyeFaceMain(&fw->m_faceEngine, img, rectROIDetection, &fw->m_faceInfo, &fw->m_faceDetectionPara);
	return TRUE;
}

