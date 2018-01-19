#include "txGodEyeFaceClassifier.h"
#include "../basic/system.h"


//声明函数
float CalJaccard(TxRect *pRect1, TxRect *pRect2);
short bGotFace(godEyeFaceEngine *myGodEyeFaceEngine, TxRect *pRect, int rectNum, TxRect *pRectResult, int imageWidth, int imageHeight);
void restartFaceDetector(godEyeFaceEngine *myGodEyeFaceEngine, TxRect rectROIDetection, int imageW, int imageH);
void upDateFaceROI(godEyeFaceEngine *myGodEyeFaceEngine, TxVec2f *pPointOut, int imagew, int imageh);

char *RegressorModel = "Regressor_gong_reduced_newtrainset_rot.model";

short txInitGodEyeFace(const char* modelFolder, godEyeFaceEngine *myGodEyeFaceEngine)
{
	short bLoadSuc = -1;
	int nMemoryLength = 0;
	FILE* fpca;
	char strModelPath[255];

	sprintf(strModelPath, "%s", modelFolder);
	strcat(strModelPath, RegressorModel);

	// 人脸检测
	bLoadSuc = txLoadFaceDetectionModel(&myGodEyeFaceEngine->myFaceDetector, modelFolder);
	if (bLoadSuc < 1)
		return -1;

	// landmark
	// bLoadSuc = load_reduced_model(&myGodEyeFaceEngine->facedetectorgong, "..\\face\\faceModels\\Regressor_gong_reduced_newtrainset_rot.model", 0);
	bLoadSuc = load_reduced_model(&myGodEyeFaceEngine->facedetectorgong, strModelPath, 0);
	if (bLoadSuc < 1)
		return -1;

	//申请内存
	{
		myGodEyeFaceEngine->pPointOut = (TxVec2f *)txAlloc(KEYPOINTNUMGONG*sizeof(TxVec2f));
		myGodEyeFaceEngine->pPointOutLastFrame = (TxVec2f *)txAlloc(KEYPOINTNUMGONG*sizeof(TxVec2f));
		myGodEyeFaceEngine->pRect = (TxRect *)txAlloc(MAX_FACE_DETECTION_NUM*sizeof(TxRect));
		myGodEyeFaceEngine->pFaceRectUse = (TxRect *)txAlloc(FACE_NUM_USE*sizeof(TxRect));
		myGodEyeFaceEngine->pFaceRectSave = (TxRect *)txAlloc(FACE_NUM_SAVE*sizeof(TxRect));
	}

	//  初始化变量
	myGodEyeFaceEngine->bGotFaceSuc = -1;
	myGodEyeFaceEngine->bGotFaceFromROI = -1;
	myGodEyeFaceEngine->bFirstAlignment = -1;
	myGodEyeFaceEngine->bStartFaceDetector = -1;
	myGodEyeFaceEngine->bNoNeedResetDetector = -1;

	myGodEyeFaceEngine->regionROI.width = 0;
	myGodEyeFaceEngine->faceROI.width = 0;

	myGodEyeFaceEngine->countNoFaceFrame = 0;
	myGodEyeFaceEngine->FaceRectUseCount = 0;
	myGodEyeFaceEngine->FaceRectSaveCount = 0;
	myGodEyeFaceEngine->notFaceNumWithSunglass = 0;

	myGodEyeFaceEngine->faceWidthFullDetect = 0;

	myGodEyeFaceEngine->nProcessFrame = 0;   //帧数置0

	myGodEyeFaceEngine->rblack_state_left = 0;
	myGodEyeFaceEngine->rblack_state_right = 0;

	return 1;
}

void txReleaseGodEyeFace(godEyeFaceEngine *myGodEyeFaceEngine)
{
	txFree(myGodEyeFaceEngine->pPointOut);

	txFree(myGodEyeFaceEngine->pRect);
	txFree(myGodEyeFaceEngine->pPointOutLastFrame);
	txFree(myGodEyeFaceEngine->pFaceRectUse);
	txFree(myGodEyeFaceEngine->pFaceRectSave);

	txReleaseFaceDetectionModel(&myGodEyeFaceEngine->myFaceDetector);

	release_reduced_model(&myGodEyeFaceEngine->facedetectorgong);
}


short txGodEyeFaceMain(godEyeFaceEngine *myGodEyeFaceEngine, TxImage* pimg8, TxRect rectROIDetection, Face_Info *myFaceInfo_Output, fatigueDetectionPara *myFatigueDetectionPara)
{
	int m = 0;
	int nPoint = 0;
	int numberFace = 0;
	short rDetector = 0;
	short rState = 0;
	float fLastYaw;

	//记录上一帧的角度
	fLastYaw = myFaceInfo_Output->yaw;
	
	//输出结果初始化
	myFaceInfo_Output->faceRect.x = 0;
	myFaceInfo_Output->faceRect.y = 0;
	myFaceInfo_Output->faceRect.width = 0;
	myFaceInfo_Output->faceRect.height = 0;

	//流程初始化
	if (myGodEyeFaceEngine->bGotFaceSuc < 1 && myGodEyeFaceEngine->bNoNeedResetDetector < 1)
	{
		restartFaceDetector(myGodEyeFaceEngine, rectROIDetection, pimg8->cols, pimg8->rows);
		myGodEyeFaceEngine->bNoNeedResetDetector = 1;
	}

	myGodEyeFaceEngine->bGotFaceSuc = -1;


	printf("+++++++++++++++++++++++++++++++++++ %d\n");

	printf("countNoFaceFrame = %d\n", myGodEyeFaceEngine->countNoFaceFrame);

	
	//人脸检测
	if (myGodEyeFaceEngine->bGotFaceSuc < 1)
	{
		if (myGodEyeFaceEngine->countNoFaceFrame % 2 == 1)
		{
			myGodEyeFaceEngine->countNoFaceFrame++;
		}
		else
		{
			rDetector = txFaceDetector_fullImage(&myGodEyeFaceEngine->myFaceDetector, pimg8, myGodEyeFaceEngine->regionROI, myGodEyeFaceEngine->faceROI, myGodEyeFaceEngine->pRect, &numberFace, myFatigueDetectionPara,myGodEyeFaceEngine->faceWidthFullDetect);
			
			if (rDetector == 1)
			{
				if (numberFace == 0 || numberFace > MAX_FACE_DETECTION_NUM)
				{
					myGodEyeFaceEngine->countNoFaceFrame++;
					if (myGodEyeFaceEngine->countNoFaceFrame >= MAX_NO_FACE_FRAME_NUM)
						myGodEyeFaceEngine->bNoNeedResetDetector = -1;
				}
				else
					myGodEyeFaceEngine->bGotFaceSuc = bGotFace(myGodEyeFaceEngine, myGodEyeFaceEngine->pRect, numberFace, &myGodEyeFaceEngine->rectForSDM, pimg8->cols, pimg8->rows);
				
				if (myGodEyeFaceEngine->bGotFaceSuc < 1)
				{
					myGodEyeFaceEngine->countNoFaceFrame++;
					if (myGodEyeFaceEngine->countNoFaceFrame >= MAX_NO_FACE_FRAME_NUM)
						myGodEyeFaceEngine->bNoNeedResetDetector = -1;
				}
				else
					myGodEyeFaceEngine->countNoFaceFrame = 0;
			}
		}
	}

	printf("bGotFaceSuc = %d\n", myGodEyeFaceEngine->bGotFaceSuc);

	/*
	myFaceInfo_Output->faceRect.x = myGodEyeFaceEngine->rectForSDM.x;
	myFaceInfo_Output->faceRect.y = myGodEyeFaceEngine->rectForSDM.y;
	myFaceInfo_Output->faceRect.width = myGodEyeFaceEngine->rectForSDM.width;
	myFaceInfo_Output->faceRect.height = myGodEyeFaceEngine->rectForSDM.height;
	*/
	//landmark点检测
	if (myGodEyeFaceEngine->bGotFaceSuc > 0)
	{
		myFaceInfo_Output->faceRect.x = myGodEyeFaceEngine->rectForSDM.x;
		myFaceInfo_Output->faceRect.y = myGodEyeFaceEngine->rectForSDM.y;
		myFaceInfo_Output->faceRect.width = myGodEyeFaceEngine->rectForSDM.width;
		myFaceInfo_Output->faceRect.height = myGodEyeFaceEngine->rectForSDM.height;

		rState = landmark_pred_reduced_model(&myGodEyeFaceEngine->facedetectorgong, pimg8, &myGodEyeFaceEngine->rectForSDM, myGodEyeFaceEngine->pPointOut, myGodEyeFaceEngine->pPointOutLastFrame, fLastYaw);
		
		printf("landmark_pred_reduced_model  rState = %d\n", rState);

		for (nPoint = 0; nPoint<KEYPOINTNUMGONG; nPoint++)
		{
			(myGodEyeFaceEngine->pPointOutLastFrame + nPoint)->data[0] = (myGodEyeFaceEngine->pPointOut + nPoint)->data[0];
			(myGodEyeFaceEngine->pPointOutLastFrame + nPoint)->data[1] = (myGodEyeFaceEngine->pPointOut + nPoint)->data[1];
		}

		myFaceInfo_Output->eyePoints[0].data[0] = (myGodEyeFaceEngine->pPointOut + 2)->data[0];
		myFaceInfo_Output->eyePoints[0].data[1] = (myGodEyeFaceEngine->pPointOut + 2)->data[1];
		myFaceInfo_Output->eyePoints[1].data[0] = (myGodEyeFaceEngine->pPointOut + 7)->data[0];
		myFaceInfo_Output->eyePoints[1].data[1] = (myGodEyeFaceEngine->pPointOut + 7)->data[1];
		myFaceInfo_Output->eyePoints[2].data[0] = (myGodEyeFaceEngine->pPointOut + 13)->data[0];
		myFaceInfo_Output->eyePoints[2].data[1] = (myGodEyeFaceEngine->pPointOut + 13)->data[1];
		
		for (nPoint = 3; nPoint<15; nPoint++)
		{
			myFaceInfo_Output->eyePoints[nPoint].data[0] = (myGodEyeFaceEngine->pPointOut + nPoint + 16)->data[0];
			myFaceInfo_Output->eyePoints[nPoint].data[1] = (myGodEyeFaceEngine->pPointOut + nPoint + 16)->data[1];
		}
		
		myFaceInfo_Output->eyePoints[15].data[0] = (myGodEyeFaceEngine->pPointOut + 31)->data[0];
		myFaceInfo_Output->eyePoints[15].data[1] = (myGodEyeFaceEngine->pPointOut + 31)->data[1];
		myFaceInfo_Output->eyePoints[16].data[0] = (myGodEyeFaceEngine->pPointOut + 37)->data[0];
		myFaceInfo_Output->eyePoints[16].data[1] = (myGodEyeFaceEngine->pPointOut + 37)->data[1];
	}

	/*  人脸检测失败或者特征点检测定位失败 */
	if (rState == 0 || myGodEyeFaceEngine->bGotFaceSuc < 0)
	{
		for (int i = 0; i < KEYPOINTNUM; i++)
		{
			myFaceInfo_Output->eyePoints[i].data[0] = 0;
			myFaceInfo_Output->eyePoints[i].data[1] = 0;
		}
	}

	return 1;
}

void restartFaceDetector(godEyeFaceEngine *myGodEyeFaceEngine, TxRect rectROIDetection, int imageW, int imageH)
{
	if (rectROIDetection.width > 0) //ROI有效
	{
		if (rectROIDetection.x >= 0 && rectROIDetection.y >= 0 && (rectROIDetection.x + rectROIDetection.width) <imageW
			&& (rectROIDetection.y + rectROIDetection.height) <imageH)
		{
			myGodEyeFaceEngine->regionROI.x = rectROIDetection.x;
			myGodEyeFaceEngine->regionROI.y = rectROIDetection.y;
			myGodEyeFaceEngine->regionROI.width = rectROIDetection.width;
			myGodEyeFaceEngine->regionROI.height = rectROIDetection.height;
		}
		else
		{
			myGodEyeFaceEngine->regionROI.x = 0;
			myGodEyeFaceEngine->regionROI.y = 0;
			myGodEyeFaceEngine->regionROI.width = imageW - 1;
			myGodEyeFaceEngine->regionROI.height = imageH - 1;
		}
	}
	else if (myGodEyeFaceEngine->bGotFaceSuc != 1)
	{
		myGodEyeFaceEngine->regionROI.x = 0;
		myGodEyeFaceEngine->regionROI.y = 0;
		myGodEyeFaceEngine->regionROI.width = imageW - 1;
		myGodEyeFaceEngine->regionROI.height = imageH - 1;
	}
	myGodEyeFaceEngine->bFirstAlignment = -1;
	myGodEyeFaceEngine->bGotFaceSuc = -1;
	myGodEyeFaceEngine->bNoNeedResetDetector = -1;
	myGodEyeFaceEngine->bStartFaceDetector = -1;

	myGodEyeFaceEngine->faceWidthFullDetect = 0;
	myGodEyeFaceEngine->faceROI.width = 0;
	myGodEyeFaceEngine->countNoFaceFrame = 0;
	myGodEyeFaceEngine->FaceRectUseCount = 0;
	myGodEyeFaceEngine->FaceRectSaveCount = 0;

	return;
}

float CalJaccard(TxRect *pRect1, TxRect *pRect2)
{
	int u = 0;
	int v = 0;
	int xBegin = 0;
	int yBegin = 0;
	int xEnd = 0;
	int yEnd = 0;
	long pixIn = 0;

	xBegin = pRect1->x <= pRect2->x ? pRect1->x : pRect2->x;
	yBegin = pRect1->y <= pRect2->y ? pRect1->y : pRect2->y;
	xEnd = pRect1->x + pRect1->width >= pRect2->x + pRect2->width ? pRect1->x + pRect1->width : pRect2->x + pRect2->width;
	yEnd = pRect1->y + pRect1->height >= pRect2->y + pRect2->height ? pRect1->y + pRect1->height : pRect2->y + pRect2->height;

	for (u = yBegin; u<yEnd; u++)
	{
		for (v = xBegin; v<xEnd; v++)
		{
			if ((u >= pRect1->y && u<(pRect1->y + pRect1->height) && v >= pRect1->x && v<(pRect1->x + pRect1->width)) &&
				(u >= pRect2->y && u<(pRect2->y + pRect2->height) && v >= pRect2->x && v<(pRect2->x + pRect2->width)))
				pixIn++;

		}
	}

	return ((float)pixIn / (float)((xEnd - xBegin)*(yEnd - yBegin)));
}

short bGotFace(godEyeFaceEngine *myGodEyeFaceEngine, TxRect *pRect, int rectNum, TxRect *pRectResult, int imageWidth, int imageHeight)
{
	short bGot = 0;
	short index = -1;
	int i, j = 0;
	int maxWidth = 0;
	int xEnd = 0;
	int YEnd = 0;
	int offsetX = 0;
	int offsetY = 0;
	float rJaccard = 0;
	TxRect *pRectTmp;
	TxRect RectTmp;
	short resultState = -1;
	//找到最大长度的矩形框并记下位置
	for (i = 0; i<rectNum; i++)
	{
		if ((pRect + i)->width > maxWidth)
		{
			maxWidth = (pRect + i)->width;
			index = i;
		}
	}
	//printf("myGodEyeFaceEngine->FaceRectUseCount:%d\n", myGodEyeFaceEngine->FaceRectUseCount);
	if (myGodEyeFaceEngine->FaceRectUseCount == 0)
	{
		(myGodEyeFaceEngine->pFaceRectUse + myGodEyeFaceEngine->FaceRectUseCount)->x = (pRect + index)->x;
		(myGodEyeFaceEngine->pFaceRectUse + myGodEyeFaceEngine->FaceRectUseCount)->y = (pRect + index)->y;
		(myGodEyeFaceEngine->pFaceRectUse + myGodEyeFaceEngine->FaceRectUseCount)->width = (pRect + index)->width;
		(myGodEyeFaceEngine->pFaceRectUse + myGodEyeFaceEngine->FaceRectUseCount)->height = (pRect + index)->height;
		myGodEyeFaceEngine->FaceRectUseCount++;

		return resultState;
	}
	if (myGodEyeFaceEngine->FaceRectUseCount < FACE_NUM_USE && myGodEyeFaceEngine->FaceRectUseCount > 0)
	{
		for (i = 0; i<myGodEyeFaceEngine->FaceRectUseCount; i++)
		{
			pRectTmp = (myGodEyeFaceEngine->pFaceRectUse + i);
			//	printf("Dst : %d %d %d %d\n",pRectTmp->x,pRectTmp->y,pRectTmp->width,pRectTmp->height);
			//	printf("src : %d %d %d %d\n",(pRect+index)->x,(pRect+index)->y,(pRect+index)->width,(pRect+index)->height);
			rJaccard = CalJaccard(pRectTmp, (pRect + index));
			//	printf("jaccard = %f\n\n",rJaccard);
			//	printf("jaccard = %f\n",rJaccard);
			if (rJaccard >= JACCARD_THRESHOLD)
			{
				(myGodEyeFaceEngine->pFaceRectUse + myGodEyeFaceEngine->FaceRectUseCount)->x = (pRect + index)->x;
				(myGodEyeFaceEngine->pFaceRectUse + myGodEyeFaceEngine->FaceRectUseCount)->y = (pRect + index)->y;
				(myGodEyeFaceEngine->pFaceRectUse + myGodEyeFaceEngine->FaceRectUseCount)->width = (pRect + index)->width;
				(myGodEyeFaceEngine->pFaceRectUse + myGodEyeFaceEngine->FaceRectUseCount)->height = (pRect + index)->height;
				myGodEyeFaceEngine->FaceRectUseCount++;
				bGot = 1;
			}
			if (bGot == 1)
				break;
		}

		if (bGot < 1)
			myGodEyeFaceEngine->FaceRectUseCount = 0;
	}

	if (myGodEyeFaceEngine->FaceRectUseCount >= FACE_NUM_USE)
	{
		//------------------------------  //
		float rJaccard = 0;
		RectTmp.x = 0;
		RectTmp.y = 0;
		RectTmp.width = 0;
		RectTmp.height = 0;

		for (i = FACE_NUM_USE - 1; i >= 0; i--)
		{
			//	printf("%d rect : %d %d %d %d \n",i,
			//		(myGodEyeFaceEngine->pFaceRectUse+i)->x,(myGodEyeFaceEngine->pFaceRectUse+i)->y,(myGodEyeFaceEngine->pFaceRectUse+i)->width,(myGodEyeFaceEngine->pFaceRectUse+i)->height);
			RectTmp.x = (myGodEyeFaceEngine->pFaceRectUse + i)->x;
			RectTmp.y = (myGodEyeFaceEngine->pFaceRectUse + i)->y;
			RectTmp.width = (myGodEyeFaceEngine->pFaceRectUse + i)->width;
			RectTmp.height = (myGodEyeFaceEngine->pFaceRectUse + i)->height;

			rJaccard = CalJaccard(&RectTmp, (pRect + index));
			if (rJaccard >= JACCARD_THRESHOLD)
			{
				bGot = 1;

				RectTmp.x = (pRect + index)->x;
				RectTmp.y = (pRect + index)->y;
				RectTmp.width = (pRect + index)->width;
				RectTmp.height = (pRect + index)->height;

				//-----------------------------------------------

				for (j = 1; j <= FACE_NUM_USE - 1; j++)
				{

					(myGodEyeFaceEngine->pFaceRectUse + j - 1)->x = (myGodEyeFaceEngine->pFaceRectUse + j)->x;
					(myGodEyeFaceEngine->pFaceRectUse + j - 1)->y = (myGodEyeFaceEngine->pFaceRectUse + j)->y;
					(myGodEyeFaceEngine->pFaceRectUse + j - 1)->width = (myGodEyeFaceEngine->pFaceRectUse + j)->width;
					(myGodEyeFaceEngine->pFaceRectUse + j - 1)->height = (myGodEyeFaceEngine->pFaceRectUse + j)->height;
				}


				//------------------------------------------------

				(myGodEyeFaceEngine->pFaceRectUse + FACE_NUM_USE - 1)->x = (pRect + index)->x;
				(myGodEyeFaceEngine->pFaceRectUse + FACE_NUM_USE - 1)->y = (pRect + index)->y;
				(myGodEyeFaceEngine->pFaceRectUse + FACE_NUM_USE - 1)->width = (pRect + index)->width;
				(myGodEyeFaceEngine->pFaceRectUse + FACE_NUM_USE - 1)->height = (pRect + index)->height;

				offsetX = RectTmp.width *0.55;
				offsetY = RectTmp.height *0.55;

				myGodEyeFaceEngine->regionROI.x = RectTmp.x - offsetX >= 0 ? RectTmp.x - offsetX : 0;
				myGodEyeFaceEngine->regionROI.y = RectTmp.y - offsetY >= 0 ? RectTmp.y - offsetY : 0;

				xEnd = RectTmp.x + RectTmp.width + offsetX< imageWidth ? RectTmp.x + RectTmp.width + offsetX : imageWidth - 1;
				YEnd = RectTmp.y + RectTmp.height + offsetY< imageHeight ? RectTmp.y + RectTmp.height + offsetY : imageHeight - 1;

				myGodEyeFaceEngine->regionROI.width = xEnd - myGodEyeFaceEngine->regionROI.x;
				myGodEyeFaceEngine->regionROI.height = YEnd - myGodEyeFaceEngine->regionROI.y;

				myGodEyeFaceEngine->faceROI.x = RectTmp.x;
				myGodEyeFaceEngine->faceROI.y = RectTmp.y;
				myGodEyeFaceEngine->faceROI.width = RectTmp.width;
				myGodEyeFaceEngine->faceROI.height = RectTmp.height;

				myGodEyeFaceEngine->regionROISave.x = myGodEyeFaceEngine->regionROI.x;
				myGodEyeFaceEngine->regionROISave.y = myGodEyeFaceEngine->regionROI.y;
				myGodEyeFaceEngine->regionROISave.width = myGodEyeFaceEngine->regionROI.width;
				myGodEyeFaceEngine->regionROISave.height = myGodEyeFaceEngine->regionROI.height;

			
				RectTmp.height = 1.0*RectTmp.height;
				RectTmp.width = 1.0*RectTmp.width;
	
				pRectResult->x = RectTmp.x;
				pRectResult->y = RectTmp.y;
				pRectResult->width = RectTmp.width;
				pRectResult->height = RectTmp.height;
				

				resultState = 1;
			}

			if (bGot == 1)
				break;

		}
	}

	return resultState;
}
