#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include "../include/config.h"
#include "../basic/image.h"
#include "../basic/adas.h"
#include "txLocalDefine.h"

typedef struct Params{
	int max_numtrees;
	int max_depth;
	int landmark_num;
	int max_numstage;
	int featnumori;
	int featnumreduced;
}Params;

typedef struct BoundingBox{
	float start_x;
	float start_y;
	float width;
	float height;
	float centroid_x;
	float centroid_y;
}BoundingBox;

typedef struct FaceReg{
	Params global_params;
	int* linearW;//STAGENUM*(LANDMARKNUM*2)*FEATDIMREDUCE
	float* feat;
	int* thresh;//STAGENUM*LANDMARKNUM*TREENUM*NOLEAFNUM*5
	float* radius;
	float* meanshape;
	float* meanshape1;
	float* meanshape2;
	float* meanshape3;
	short* flags;//STAGENUM*FEATDIM;
	int* wdim;
	float* feat_range;
	short bincode[300];
	short terminate_stage;
	float scale;
	float rotation[4];
	short istrack;
}FaceReg;

short landmark_pred_reduced_model(FaceReg* reg, TxImage* image, TxRect* rectForSDM, TxVec2f *pPointOut, TxVec2f *pPointOutLastFrame, float fLastYaw);
short load_reduced_model(FaceReg* reg, char* filename, short istrack);
void release_reduced_model(FaceReg* reg);

#ifdef __cplusplus
}
#endif