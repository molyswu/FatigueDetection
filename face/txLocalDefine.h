#pragma once
#ifndef LOCALDEFINE_H
#define LOCALDEFINE_H

#include "../basic/core.h"
#include "../basic/image.h"
#include "../basic/matrix.h"
#include "../basic/cache.h"
#include "../basic/adas.h"
#include "../basic/system.h"
#include "../include/config.h"

#include "stdio.h"
#include "math.h"
#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MAX
#define MAX(a, b) ((a)>(b)?(a):(b))
#endif
#ifndef MIN
#define MIN(a, b) ((a)<(b)?(a):(b))
#endif

#define FRAME_USE_NUMBER 3
#define PCA_NUM 25
	// key point detector 
#define SIFT_HIST_WIDTH 3
#define FEATRUE_DIM 72
#define KEYPOINTNUMGONG 49

#define KEYPOINTNUM 17
#define MULTIVIEW

#define GLCM_HIST_LEN 0		
#define SIFT_NMS 0					// SIFT特征极大抑制，消除光照影响

//#define TWO_EYE_CLASSIFIER 1		// 两只眼睛的综合打分分类器
#define  TWO_EYE_SCORE_FEAT_LEN 8

#define COLOR_FEATURE 1

#define  HIST_COLOR_LEN 512
#define COLOR_RECT_LEN 4
#define COLOR_FEATURE_LEN 9

//eye sift
#define SIFT_HIST_WIDTH_EYE_SIFT 4
#define FEATRUE_DIM_EYE_SIFT 128
#define EYE_SIFT_NUM 2
// Face sift
#define SIFT_HIST_WIDTH_FACE_SIFT 5
#define FEATRUE_DIM_FACE_SIFT 72
#define FACE_SIFT_NUM 4

//Mouth sift
#define MOUTH_SIFT_NUM 5

// 重启全图人脸检测的无结果的帧数
#define MAX_NO_FACE_FRAME_NUM 14

// 最多检测的人脸个数
#define MAX_FACE_DETECTION_NUM 10

/*
// 区域人脸检测的最小人脸
#define FACE_MIN_SIZE_DETECT  50//85

//区域人脸检测的最大人脸
#define FACE_MAX_SIZE_DETECT 300//145

//相邻帧人脸size的偏差
#define FACE_SIZE_OFFSET 60
*/
typedef struct _fatigueDetectionPara
{
	int face_min_size_detect; // 区域人脸检测的最小人脸
	int face_max_size_detect; //区域人脸检测的最大人脸
	int face_size_offset; //相邻帧人脸size的偏差
	BOOL bSetFaceROI;//选择ROI检测还是全图检测人脸
}fatigueDetectionPara;

//人脸检测时最多检测框个数
#define  MAX_FACE_DETECTION_RESULT 256

// 可以保证检测到人脸的最少的检测框个数
#define FIND_FACES_NUM_BREAK 16

// 检测不到人脸后 间隔n帧再重新检测
#define NO_FACE_FRAME_NUM_STEP  1

#define JACCARD_THRESHOLD 0.4

#define SUBTRACTION_ITERATION 0

#define FACE_NUM_USE 4

#define FACE_NUM_SAVE 3

#define MAXIMAGERESIZE  1200

#define NORMALIZEIMAGEWIDTH 100

// 记录当前20帧的face rect
#define FACE_VALID_RECT_NUM 5

#ifdef __cplusplus
}
#endif

#endif
