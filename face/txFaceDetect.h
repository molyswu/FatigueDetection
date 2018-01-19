#ifndef TXFACEDETECT_H
#define TXFACEDETECT_H

#include "txLocalDefine.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _faceDetector
{
	float m_tsr, m_tsc;
	int m_tdepth;
	int m_ntrees;
	int m_dim;
	float *m_luts;
	float *m_thresholds;
	short int *m_rtcodes;

}faceDetector;

short txFaceDetector_fullImage(faceDetector *myFaceDetector, TxImage* imgInput, TxRect rectROI, TxRect faceROI_reference, TxRect *pRectFaceOut, int *numberFace, fatigueDetectionPara *myFatigueDetectionPara, int);

short txLoadFaceDetectionModel(faceDetector *myFaceDetector,const char *modelFolder);
void txReleaseFaceDetectionModel(faceDetector *myFaceDetector);

#ifdef __cplusplus
}
#endif

#endif