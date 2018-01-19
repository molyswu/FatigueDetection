#include "face3000.h"
#include "../basic/core.h"
#include "../basic/system.h"
#include "stdlib.h"
#include <stdio.h>


short load_reduced_model(FaceReg* reg, char* filename, short istrack)
{
	int i, j, k, m, q, temp;
	int mm = sizeof(short);
	float *v;
	int ind1, ind2, count;
	int noleafnum;
	int* p;
	//char* detect_model_name = "Regressor_gong_reduced_newtrainset_rot.model";
	//char* tracking_model_name = "Regressor_gong_tracking_reduced_rot3_1.model";
	char* filename_amba;
	float sq, tempx, tempy, thresh;

	FILE* pf = fopen(filename, "rb");
	if (pf == NULL)
		return -1;
	reg->istrack = istrack;
	fread(&(reg->global_params.featnumreduced), sizeof(int), 1, pf);
	fread(&(reg->global_params.max_numstage), sizeof(int), 1, pf);
	fread(&(reg->global_params.landmark_num), sizeof(int), 1, pf);
	fread(&(reg->global_params.max_numtrees), sizeof(int), 1, pf);
	fread(&(reg->global_params.max_depth), sizeof(int), 1, pf);
	reg->global_params.featnumori = reg->global_params.landmark_num*reg->global_params.max_numtrees*pow(2.0, reg->global_params.max_depth - 1);
	noleafnum = pow(2.0, reg->global_params.max_depth - 1) - 1;
	reg->feat_range = (float*)txAlloc(sizeof(float)*reg->global_params.max_numstage);
	memset(reg->feat_range, 0, sizeof(float)*reg->global_params.max_numstage);
	if (istrack == 1)
	{
		reg->meanshape1 = (float*)txAlloc(sizeof(float)*reg->global_params.landmark_num * 2);
		reg->meanshape2 = (float*)txAlloc(sizeof(float)*reg->global_params.landmark_num * 2);
		reg->meanshape3 = (float*)txAlloc(sizeof(float)*reg->global_params.landmark_num * 2);
		fread(reg->meanshape1, sizeof(float), reg->global_params.landmark_num * 2, pf);
		fread(reg->meanshape2, sizeof(float), reg->global_params.landmark_num * 2, pf);
		fread(reg->meanshape3, sizeof(float), reg->global_params.landmark_num * 2, pf);
	}
	else
	{
		reg->meanshape = (float*)txAlloc(sizeof(float)*reg->global_params.landmark_num * 2);
		fread(reg->meanshape, sizeof(float), reg->global_params.landmark_num * 2, pf);
	}
	reg->radius = (float*)txAlloc(sizeof(float)*reg->global_params.max_numstage);
	fread(reg->radius, sizeof(float), reg->global_params.max_numstage, pf);
	reg->linearW = (int*)txAlloc(sizeof(int)*reg->global_params.max_numstage*reg->global_params.landmark_num * 2 * reg->global_params.featnumreduced);
	reg->flags = (short*)txAlloc(sizeof(short)*reg->global_params.max_numstage*reg->global_params.featnumori);
	reg->wdim = (int*)txAlloc(sizeof(int)*reg->global_params.max_numstage);
	reg->feat = (float*)txAlloc(sizeof(float)*reg->global_params.max_numstage*reg->global_params.landmark_num*reg->global_params.max_numtrees*noleafnum * 4);


	reg->thresh = (int*)txAlloc(sizeof(int)*reg->global_params.max_numstage*reg->global_params.landmark_num*reg->global_params.max_numtrees*noleafnum);
	p = reg->thresh;
	v = reg->feat;
	for (i = 0; i < reg->global_params.max_numstage; i++)
	{
		fread(&temp, sizeof(int), 1, pf);
		fread(&reg->flags[reg->global_params.featnumori*i], sizeof(short), reg->global_params.featnumori, pf);
		fread(&reg->wdim[i], sizeof(int), 1, pf);
		ind1 = i*reg->global_params.landmark_num * 2 * reg->global_params.featnumreduced;
		for (j = 0; j < reg->global_params.landmark_num; j++)
		{
			//ind2 = ind1+j*reg->global_params.featnumreduced*2;
			//fread(&reg->linearW[ind2], sizeof(float),reg->wdim[i],pf);
			//ind2 += reg->global_params.featnumreduced;
			//fread(&reg->linearW[ind2], sizeof(float),reg->wdim[i],pf);

			for (count = 0; count<reg->global_params.featnumreduced; count++)
			{
				fread(&tempx, sizeof(float), 1, pf);
				reg->linearW[ind1 + count*reg->global_params.landmark_num * 2 + j * 2] = tempx * 100000;
				//	fread(&reg->linearW[ind1+count*reg->global_params.landmark_num*2+j*2],sizeof(float),1,pf);
			}
			for (count = 0; count<reg->global_params.featnumreduced; count++)
			{
				fread(&tempx, sizeof(float), 1, pf);
				reg->linearW[ind1 + count*reg->global_params.landmark_num * 2 + j * 2 + 1] = tempx * 100000;
				//	fread(&reg->linearW[ind1+count*reg->global_params.landmark_num*2+j*2+1],sizeof(float),1,pf);
			}

			//isLeaf[i][j] = new bool*[global_params.max_numtrees];
			//feat_thresh[i][j] = new float**[global_params.max_numtrees];
			for (k = 0; k < reg->global_params.max_numtrees; k++)
			{
				for (m = 0; m < noleafnum; m++)
				{
					fread(&thresh, sizeof(float), 1, pf);
					*p = thresh;
					fread(v, sizeof(float), 4, pf);
					for (q = 0; q < 4; q++)
					{
						v[q] *= reg->radius[i];
					}
					sq = v[0] * v[0] + v[1] * v[1];
					if (sq>reg->feat_range[i])
					{
						reg->feat_range[i] = sq;
					}
					sq = v[2] * v[2] + v[3] * v[3];
					if (sq>reg->feat_range[i])
					{
						reg->feat_range[i] = sq;
					}
					v = v + 4;
					p = p + 1;
					//cout << feat_thresh[i][j][k][m][0] << ";" ;
				}
			}
		}
		reg->feat_range[i] = sqrt(reg->feat_range[i]);
	}
	fclose(pf);
	if (istrack == 0)
	{
		sq = -atan2(reg->meanshape[28 * 2 + 1] - reg->meanshape[19 * 2 + 1], reg->meanshape[28 * 2] - reg->meanshape[19 * 2]);
		for (k = 0; k<reg->global_params.landmark_num; k++)
		{
			tempx = reg->meanshape[k * 2] * cos(sq) - reg->meanshape[k * 2 + 1] * sin(sq);
			tempy = reg->meanshape[k * 2] * sin(sq) + reg->meanshape[k * 2 + 1] * cos(sq);
			reg->meanshape[k * 2] = tempx;
			reg->meanshape[k * 2 + 1] = tempy;
		}
	}
	else
	{
		sq = -atan2(reg->meanshape1[28 * 2 + 1] - reg->meanshape1[19 * 2 + 1], reg->meanshape1[28 * 2] - reg->meanshape1[19 * 2]);
		for (k = 0; k<reg->global_params.landmark_num; k++)
		{
			tempx = reg->meanshape1[k * 2] * cos(sq) - reg->meanshape1[k * 2 + 1] * sin(sq);
			tempy = reg->meanshape1[k * 2] * sin(sq) + reg->meanshape1[k * 2 + 1] * cos(sq);
			reg->meanshape1[k * 2] = tempx;
			reg->meanshape1[k * 2 + 1] = tempy;
		}
		sq = -atan2(reg->meanshape2[28 * 2 + 1] - reg->meanshape2[19 * 2 + 1], reg->meanshape2[28 * 2] - reg->meanshape2[19 * 2]);
		for (k = 0; k<reg->global_params.landmark_num; k++)
		{
			tempx = reg->meanshape2[k * 2] * cos(sq) - reg->meanshape2[k * 2 + 1] * sin(sq);
			tempy = reg->meanshape2[k * 2] * sin(sq) + reg->meanshape2[k * 2 + 1] * cos(sq);
			reg->meanshape2[k * 2] = tempx;
			reg->meanshape2[k * 2 + 1] = tempy;
		}
		sq = -atan2(reg->meanshape3[28 * 2 + 1] - reg->meanshape3[19 * 2 + 1], reg->meanshape3[28 * 2] - reg->meanshape3[19 * 2]);
		for (k = 0; k<reg->global_params.landmark_num; k++)
		{
			tempx = reg->meanshape3[k * 2] * cos(sq) - reg->meanshape3[k * 2 + 1] * sin(sq);
			tempy = reg->meanshape3[k * 2] * sin(sq) + reg->meanshape3[k * 2 + 1] * cos(sq);
			reg->meanshape3[k * 2] = tempx;
			reg->meanshape3[k * 2 + 1] = tempy;
		}
	}
	return 1;
}

void release_reduced_model(FaceReg* reg)
{
	txFree(reg->flags);
	txFree(reg->linearW);
#ifndef MULTIVIEW
	txFree(reg->meanshape);
#else
	if (reg->istrack == 1)
	{
		txFree(reg->meanshape1);
		txFree(reg->meanshape2);
		txFree(reg->meanshape3);
	}
	else
	{
		txFree(reg->meanshape);
	}
#endif
	txFree(reg->radius);
	txFree(reg->wdim);
	txFree(reg->feat);
	txFree(reg->thresh);
}

void ProjectShape(float* shape, float* res, const BoundingBox* bounding_box, int rows)
{
	int j, indx, indy;
	float centerx = bounding_box->centroid_x;
	float centery = bounding_box->centroid_y;
	float wid = 2.0f / bounding_box->width;
	float hei = 2.0f / bounding_box->height;
	for (j = 0; j < rows; j++){
		indx = j + j;
		indy = indx + 1;
		res[indx] = (shape[indx] - centerx) * wid;
		res[indy] = (shape[indy] - centery) * hei;
	}
}

void ReProjectShape(float* shape, float* res, const BoundingBox* bounding_box, int rows)
{
	int j, indx, indy;
	float centerx = bounding_box->centroid_x;
	float centery = bounding_box->centroid_y;
	float wid = bounding_box->width / 2.0f;
	float hei = bounding_box->height / 2.0f;
	for (j = 0; j < rows; j++)
	{
		indx = j + j;
		indy = indx + 1;
		res[indx] = (shape[indx] * wid + centerx);
		res[indy] = (shape[indy] * hei + centery);
	}
}

void SimilarityTransform(float* shape1, float* shape2, float* rotation, float* scale, int rows)
{
	// center the data
	float center_x_1 = 0;
	float center_y_1 = 0;
	float center_x_2 = 0;
	float center_y_2 = 0;
	int i, indx, indy;
	float rowsinv;

	float temp1[136], temp2[136], covariance1, covariance2, s1, s2, num, den, norm, sin_theta, cos_theta;
	for (i = 0; i < rows; i++){
		indx = i + i;
		indy = indx + 1;
		center_x_1 += shape1[indx];
		center_y_1 += shape1[indy];
		center_x_2 += shape2[indx];
		center_y_2 += shape2[indy];
	}
	rowsinv = 1.0f / rows;
	center_x_1 *= rowsinv;
	center_y_1 *= rowsinv;
	center_x_2 *= rowsinv;
	center_y_2 *= rowsinv;

	covariance1 = 0.0f;
	covariance2 = 0.0f;

	for (i = 0; i < rows; i++){
		indx = i + i;
		indy = indx + 1;
		temp1[indx] = shape1[indx] - center_x_1;
		temp1[indy] = shape1[indy] - center_y_1;
		temp2[indx] = shape2[indx] - center_x_2;
		temp2[indy] = shape2[indy] - center_y_2;
		covariance1 += temp1[indx] * temp1[indx] + temp1[indy] * temp1[indy];
		covariance2 += temp2[indx] * temp2[indx] + temp2[indy] * temp2[indy];
	}

	s1 = sqrt(covariance1);
	s2 = sqrt(covariance2);
	*scale = s1 / s2;
	s1 = 1 / s1;
	s2 = 1 / s2;
	for (i = 0; i < rows; i++)
	{
		indx = i + i;
		indy = indx + 1;
		temp1[indx] *= s1;
		temp1[indy] *= s1;
		temp2[indx] *= s2;
		temp2[indy] *= s2;
	}
	num = 0;
	den = 0;
	for (i = 0; i < rows; i++){
		indx = i + i;
		indy = indx + 1;
		num = num + temp1[indy] * temp2[indx] - temp1[indx] * temp2[indy];
		den = den + temp1[indx] * temp2[indx] + temp1[indy] * temp2[indy];
	}

	norm = sqrt(num*num + den*den);
	sin_theta = num / norm;
	cos_theta = den / norm;
	rotation[0] = cos_theta;
	rotation[1] = -sin_theta;
	rotation[2] = sin_theta;
	rotation[3] = cos_theta;
}

int  getBinaryCodeReduce(const TxImage* image,
	const BoundingBox* bounding_box,
	const float* rotation,
	const float wid,
	const float hei,
	float* const feat,
	int* const thresh,
	const float posx,
	const float posy,
	const int rows,
	const int leafnodesum)
{
	int currnode = 0, real_x1, real_x2, real_y1, real_y2;
	int bincode = 0;
	float x1, x2, y1, y2, project_x1, project_x2, project_y1, project_y2;
	int pdf;
	float *pfeat = feat;
	int *pthresh = thresh;
	int temp, i, imagecols = image->cols, imagerows = image->rows;
	unsigned char* pdata = image->data;
	for (i = 0; i<4; i++){
		x1 = pfeat[0];// *radio_radius;
		y1 = pfeat[1];// *radio_radius;
		x2 = pfeat[2];// *radio_radius;
		y2 = pfeat[3];// *radio_radius;


		project_x1 = rotation[0] * x1 + rotation[1] * y1;
		project_y1 = rotation[2] * x1 + rotation[3] * y1;
		project_x1 = project_x1 * wid;
		project_y1 = project_y1 * hei;

		real_x1 = MIN(MAX(0, project_x1 + posx), imagecols - 1);
		real_y1 = MIN(MAX(0, project_y1 + posy), imagerows - 1);

		project_x2 = rotation[0] * x2 + rotation[1] * y2;
		project_y2 = rotation[2] * x2 + rotation[3] * y2;
		project_x2 = project_x2 * wid;
		project_y2 = project_y2 * hei;

		real_x2 = MIN(MAX(0, project_x2 + posx), imagecols - 1);
		real_y2 = MIN(MAX(0, project_y2 + posy), imagerows - 1);

		pdf = (int)(pdata[real_y1*imagecols + real_x1]) - (int)(pdata[real_y2*imagecols + real_x2]);

		if (pdf < *pthresh){
			temp = currnode + 1;
			currnode = currnode + temp;
			pthresh += temp;
			pfeat += temp << 2;
		}
		else{
			temp = currnode + 2;
			currnode = currnode + temp;
			pthresh += temp;
			pfeat += temp << 2;
		}
	}
	bincode = currnode - leafnodesum + 1;
	return bincode;
}

short landmark_pred_reduced_model(FaceReg* reg, TxImage* image, TxRect* rectForSDM, TxVec2f *pPointOut, TxVec2f *pPointOutLastFrame, float fLastYaw)
{
	int rows = reg->global_params.landmark_num;
	int rows2 = rows * 2;
	float deltashape_bar[136];// = (float*)txAlloc(sizeof(float)*rows * 2);
	float temp[136];
	float* ptemp;// = (float*)txAlloc(sizeof(float)*rows * 2);
	float current_shape[136];
	float rot_shape[136], theta;
	int delta[136];// = (float*)txAlloc(sizeof(float)*rows * 2);
	int leafnodesnum = (int)pow(2.0f, reg->global_params.max_depth - 1);
	int noleafnum = leafnodesnum - 1;
	int noleafnum5 = noleafnum * 4;
	int maxtree = reg->global_params.max_numtrees;
	int maxforest = maxtree*leafnodesnum;
	int stage, j, k, bincode, mm, i, jind, kind, index;// index_pre;

	int wdim, pwstep, wdimperstage = rows2*reg->wdim[0];
	float *pfeat;
	int *pw, *pw1;
	short *pflag;
	float rotation[4], scale, scalewid, scalehei;
	short *pbincode = reg->bincode;
	int count;
	float* featrange = reg->feat_range;
	BoundingBox bbx;
	int left_x = 10000;
	int right_x = 0;
	int top_y = 10000;
	int bottom_y = 0;
	int nPoint;
	int *pthresh;
	float sint, cost, temp1;
	float left_x1 = 100, right_x1 = 0, top_y1 = 10000, bottom_y1 = 0;
	int maxstage = reg->global_params.max_numstage;
	float rottemp;
	TxVec2f PointOutLastFrame[49];
	float tempdiff = 0;
	float tempdiff1 = 0;
	float factor;
	if (reg->istrack == 0)
	{
		bbx.start_x = rectForSDM->x;
		bbx.start_y = rectForSDM->y;
		bbx.width = rectForSDM->width;
		bbx.height = rectForSDM->height;
		bbx.centroid_x = bbx.start_x + bbx.width*0.5;
		bbx.centroid_y = bbx.start_y + bbx.height*0.5;
		ReProjectShape(reg->meanshape, current_shape, &bbx, rows);
		ptemp = reg->meanshape;
	}
	else
	{
#ifdef MULTIVIEW
		if (fLastYaw>35)
		{
			reg->meanshape = reg->meanshape3;
		}
		else if (fLastYaw<-35)
		{
			reg->meanshape = reg->meanshape2;
		}
		else
		{
			reg->meanshape = reg->meanshape1;
		}
#endif
		theta = atan2(pPointOutLastFrame[28].data[1] - pPointOutLastFrame[19].data[1], pPointOutLastFrame[28].data[0] - pPointOutLastFrame[19].data[0]);

		if (theta<0.3&&theta>-0.3)
		{
			for (nPoint = 0; nPoint < reg->global_params.landmark_num; nPoint++)
			{
				if ((pPointOutLastFrame + nPoint)->data[0] < left_x)
					left_x = (pPointOutLastFrame + nPoint)->data[0];
				if ((pPointOutLastFrame + nPoint)->data[0] > right_x)
					right_x = (pPointOutLastFrame + nPoint)->data[0];
				if ((pPointOutLastFrame + nPoint)->data[1] > bottom_y)
					bottom_y = (pPointOutLastFrame + nPoint)->data[1];
				if ((pPointOutLastFrame + nPoint)->data[1] < top_y)
					top_y = (pPointOutLastFrame + nPoint)->data[1];
			}

			bbx.start_x = left_x;
			bbx.start_y = top_y;
			bbx.height = bottom_y - top_y + 1;
			bbx.width = right_x - left_x + 1;

			if (bbx.width<bbx.height)
			{
				tempdiff1 = bbx.height - bbx.width;
				tempdiff = tempdiff1 / 2;
				//bbx.width = bbx.height;
			}
			if (fLastYaw >= -35 && fLastYaw <= 35)
			{
				factor = 1.0;
			}
			else
			{
				factor = 0.0;
			}
			bbx.start_x = MAX(1.0, left_x - (factor*tempdiff));
			bbx.start_y = MAX(1.0, top_y - factor*(0.05)*bbx.height);
			bbx.width = MIN(image->cols - 2.0, left_x + bbx.width + factor*tempdiff) - bbx.start_x;
			bbx.height = MIN(image->rows - 2.0, top_y + bbx.height*1.0) - bbx.start_y;
			bbx.centroid_x = bbx.start_x + bbx.width / 2.0;
			bbx.centroid_y = bbx.start_y + bbx.height / 2.0;
			rectForSDM->x = bbx.start_x;
			rectForSDM->y = bbx.start_y;
			rectForSDM->width = bbx.width;
			rectForSDM->height = bbx.height;
			ReProjectShape(reg->meanshape, current_shape, &bbx, rows);
			//memcpy(temp,reg->meanshape,sizeof(float)*rows2);
			ptemp = reg->meanshape;
		}
		else
		{
			for (nPoint = 0; nPoint < reg->global_params.landmark_num; nPoint++)
			{
				if ((pPointOutLastFrame + nPoint)->data[0] < left_x)
					left_x = (pPointOutLastFrame + nPoint)->data[0];
				if ((pPointOutLastFrame + nPoint)->data[0] > right_x)
					right_x = (pPointOutLastFrame + nPoint)->data[0];
				if ((pPointOutLastFrame + nPoint)->data[1] > bottom_y)
					bottom_y = (pPointOutLastFrame + nPoint)->data[1];
				if ((pPointOutLastFrame + nPoint)->data[1] < top_y)
					top_y = (pPointOutLastFrame + nPoint)->data[1];
			}

			bbx.start_x = left_x;
			bbx.start_y = top_y;
			bbx.height = bottom_y - top_y + 1;
			bbx.width = right_x - left_x + 1;
			bbx.centroid_x = bbx.start_x + bbx.width / 2.0;
			bbx.centroid_y = bbx.start_y + bbx.height / 2.0;
			sint = sin(-theta);
			cost = cos(-theta);
			left_x = 10000;
			right_x = 0;
			top_y = 10000;
			bottom_y = 0;
			for (nPoint = 0; nPoint < reg->global_params.landmark_num; nPoint++)
			{
				PointOutLastFrame[nPoint].data[0] = (pPointOutLastFrame + nPoint)->data[0] - bbx.centroid_x;
				PointOutLastFrame[nPoint].data[1] = (pPointOutLastFrame + nPoint)->data[1] - bbx.centroid_y;
				rottemp = PointOutLastFrame[nPoint].data[0] * cost - PointOutLastFrame[nPoint].data[1] * sint;
				PointOutLastFrame[nPoint].data[1] = PointOutLastFrame[nPoint].data[0] * sint + PointOutLastFrame[nPoint].data[1] * cost;
				PointOutLastFrame[nPoint].data[0] = rottemp;
				PointOutLastFrame[nPoint].data[0] += bbx.centroid_x;
				PointOutLastFrame[nPoint].data[1] += bbx.centroid_y;
				if (PointOutLastFrame[nPoint].data[0] < left_x)
					left_x = PointOutLastFrame[nPoint].data[0];
				if (PointOutLastFrame[nPoint].data[0] > right_x)
					right_x = PointOutLastFrame[nPoint].data[0];
				if (PointOutLastFrame[nPoint].data[1] > bottom_y)
					bottom_y = PointOutLastFrame[nPoint].data[1];
				if (PointOutLastFrame[nPoint].data[1] < top_y)
					top_y = PointOutLastFrame[nPoint].data[1];
			}
			bbx.start_x = left_x;
			bbx.start_y = top_y;
			bbx.height = bottom_y - top_y + 1;
			bbx.width = right_x - left_x + 1;
			bbx.centroid_x = bbx.start_x + bbx.width / 2.0;
			bbx.centroid_y = bbx.start_y + bbx.height / 2.0;
			rectForSDM->x = bbx.start_x;
			rectForSDM->y = bbx.start_y;
			rectForSDM->width = bbx.width;
			rectForSDM->height = bbx.height;
			if (theta>0)
			{
				theta = theta - 0.09;
			}
			else
			{
				theta = theta + 0.09;
			}
			ReProjectShape(reg->meanshape, current_shape, &bbx, rows);

			sint = sin(theta);
			cost = cos(theta);

			for (k = 0; k<reg->global_params.landmark_num; k++)
			{
				i = k + k;
				j = i + 1;
				current_shape[i] -= bbx.centroid_x;
				current_shape[j] -= bbx.centroid_y;
				rottemp = current_shape[i] * cost - current_shape[j] * sint;
				current_shape[j] = current_shape[i] * sint + current_shape[j] * cost;
				current_shape[i] = rottemp;
				current_shape[i] += bbx.centroid_x;
				current_shape[j] += bbx.centroid_y;
			}

			ProjectShape(current_shape, rot_shape, &bbx, rows);
			//memcpy(temp,rot_shape,sizeof(float)*rows2);
			ptemp = rot_shape;
		}

	}

	pw = reg->linearW;
	pfeat = reg->feat;
	pthresh = reg->thresh;
	//t=0;

	scalewid = bbx.width*0.5;
	scalehei = bbx.height*0.5;
	SimilarityTransform(ptemp, reg->meanshape, rotation, &scale, rows);

	if (bbx.start_x - featrange[0] * scalewid<0)
		return 0;
	if (bbx.start_x + bbx.width + featrange[0] * scalewid>image->cols - 1)
		return 0;
	if (bbx.start_y - featrange[0] * scalehei<0)
		return 0;
	if (bbx.start_y + bbx.height + featrange[0] * scalehei>image->rows - 1)
		return 0;

	for (stage = 0; stage < maxstage; stage++, pw += wdimperstage){//
		count = 0;
		wdim = reg->wdim[stage];
		pflag = reg->flags + stage*reg->global_params.featnumori;
		memset(delta, 0, sizeof(int)*rows2);

		for (j = 0; j < rows; j++){
			jind = j + j;
			kind = jind + 1;
			for (k = 0; k < maxtree; k++, pfeat += noleafnum5, pthresh += noleafnum, pflag += leafnodesnum){
				bincode = getBinaryCodeReduce(image, &bbx, rotation, scalewid, scalehei, pfeat, pthresh, current_shape[jind], current_shape[kind], rows, leafnodesnum);
				index = pflag[bincode];
				pw1 = pw + index*rows2;
				//pbincode[count++] = bincode;
				for (mm = 0; mm < rows2; mm++)
				{
					delta[mm] += pw1[mm];
				}
			}
		}

		scale = scale / 100000.0f;
		for (mm = 0; mm < rows; mm++)
		{
			deltashape_bar[mm * 2] = (delta[mm] * rotation[0] + delta[mm + rows] * rotation[1])*scale;
			deltashape_bar[mm * 2 + 1] = (delta[mm] * rotation[2] + delta[mm + rows] * rotation[3])*scale;
		}
	
		for (mm = 0; mm < rows2; mm++)
		{
			temp[mm] = ptemp[mm] + deltashape_bar[mm];
		}
		ptemp = temp;
		ReProjectShape(temp, current_shape, &bbx, rows);

		SimilarityTransform(temp, reg->meanshape, rotation, &scale, rows);
		scalewid = scale*bbx.width / 2.0;
		scalehei = scale*bbx.height / 2.0;
		if (bbx.start_x - featrange[stage + 1] * scalewid<0)
		{
			break;
		}
		if (bbx.start_x + bbx.width + featrange[stage + 1] * scalewid>image->cols - 1)
		{
			break;
		}
		if (bbx.start_y - featrange[stage + 1] * scalehei<0)
		{
			break;
		}
		if (bbx.start_y + bbx.height + featrange[stage + 1] * scalehei>image->rows - 1)
		{
			break;
		}
	}
	for (nPoint = 0; nPoint<reg->global_params.landmark_num; nPoint++)
	{
		(pPointOut + nPoint)->data[0] = current_shape[nPoint * 2];
		(pPointOut + nPoint)->data[1] = current_shape[nPoint * 2 + 1];
	}

	left_x = 10000;
	right_x = 0;
	top_y = 10000;
	bottom_y = 0;
	for (nPoint = 0; nPoint < reg->global_params.landmark_num; nPoint++)
	{
		if ((pPointOut + nPoint)->data[0] < left_x)
			left_x = (pPointOut + nPoint)->data[0];
		if ((pPointOut + nPoint)->data[0] > right_x)
			right_x = (pPointOut + nPoint)->data[0];
		if ((pPointOut + nPoint)->data[1] > bottom_y)
			bottom_y = (pPointOut + nPoint)->data[1];
		if ((pPointOut + nPoint)->data[1] < top_y)
			top_y = (pPointOut + nPoint)->data[1];
	}

	rectForSDM->x = left_x;
	rectForSDM->y = top_y;
	rectForSDM->width = right_x - left_x + 1;
	rectForSDM->height = bottom_y - top_y + 1;
	return 1;
}