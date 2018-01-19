#include "txFaceDetect.h"

char *faceDetectionModel = "face.dat";

short txLoadFaceDetectionModel(faceDetector *myFaceDetector, const char *modelFolder)
{
	int i;
	int nTmp = 0;
	char strModelPath[255];

	FILE* file;
	FILE* pf;

	sprintf(strModelPath, "%s", modelFolder);
	strcat(strModelPath, faceDetectionModel);
	file = fopen(strModelPath, "rb");
	if (!file)
		return 0;

	fread(&myFaceDetector->m_tsr, sizeof(float), 1, file);
	fread(&myFaceDetector->m_tsc, sizeof(float), 1, file);
	fread(&myFaceDetector->m_tdepth, sizeof(int), 1, file);
	fread(&myFaceDetector->m_ntrees, sizeof(int), 1, file);
	myFaceDetector->m_dim = (1 << myFaceDetector->m_tdepth);

	myFaceDetector->m_luts = (float *)txAlloc(myFaceDetector->m_ntrees*myFaceDetector->m_dim*sizeof(float));
	myFaceDetector->m_thresholds = (float *)txAlloc(myFaceDetector->m_ntrees*sizeof(float));
	myFaceDetector->m_rtcodes = (short int *)txAlloc(myFaceDetector->m_ntrees*myFaceDetector->m_dim * 4 * sizeof(short int));

	if (myFaceDetector->m_luts == NULL || myFaceDetector->m_thresholds == NULL || myFaceDetector->m_rtcodes == NULL)
		return -1;

	fread(myFaceDetector->m_rtcodes, sizeof(short int), myFaceDetector->m_ntrees*(1 << myFaceDetector->m_tdepth) * 4, file);
	fread(myFaceDetector->m_luts, sizeof(float), myFaceDetector->m_ntrees*((1 << myFaceDetector->m_tdepth)), file);
	fread(myFaceDetector->m_thresholds, sizeof(float), myFaceDetector->m_ntrees, file);

	fclose(file);

	return 1;
}


void txReleaseFaceDetectionModel(faceDetector *myFaceDetector)
{
	if (myFaceDetector->m_rtcodes != NULL)
		txFree(myFaceDetector->m_rtcodes);
	if (myFaceDetector->m_luts != NULL)
		txFree(myFaceDetector->m_luts);
	if (myFaceDetector->m_thresholds != NULL)
		txFree(myFaceDetector->m_thresholds);
}


int run_facefinder(float* o, int r, int c, int s, void* vppixels, int nrows, int ncols, int ldim, faceDetector *myFaceDetector);


int find_objects
(
float rs[], float cs[], float ss[], float qs[], int maxndetections,
int(*run_detection_cascade)(float*, int, int, int, void*, int, int, int, faceDetector *),
void* pixels, int nrows, int ncols, int ldim,
float scalefactor, float stridefactor, float minsize, float maxsize, TxRect rectROI, faceDetector *myFaceDetector, short bFullImage, int facewWidth
)
{
	float s1, s2;
	int ndetections;
	short bGotFaceSuccesssful = 0;
	short bLargeScaleDetected = 0;

	int countSmall = 0;
	int countLarge = 0;

	ndetections = 0;
	s1 = facewWidth;
	s2 = facewWidth*scalefactor;
	
	while ((s2 <= maxsize || s1 >= minsize))
	{
		if (s1 >= minsize)
		{
			float r, c, dr, dc, s;
			s = s1;
			
			dr = dc = MAX(stridefactor*s, 1.0f);
			
			for (r = rectROI.y; r <= nrows - s / 2 - 1 - (nrows - rectROI.y - rectROI.height); r += dr)
			{
				for (c = rectROI.x; c <= ncols - s / 2 - 1 - (ncols - rectROI.x - rectROI.width); c += dc)
				{
					float q;
					int t;

					if (run_detection_cascade(&q, r, c, s, pixels, nrows, ncols, ldim, myFaceDetector) == 1)
					{
						if (ndetections < maxndetections)
						{
							qs[ndetections] = q;
							rs[ndetections] = r;
							cs[ndetections] = c;
							ss[ndetections] = s;

							++ndetections;

						}
					}

				}

			}
			countSmall++;

			s1 = s1 / scalefactor;
		}
		
		if (ndetections >= FIND_FACES_NUM_BREAK || ((countSmall + countLarge) >= 4 && bFullImage == -1))
		{
				bGotFaceSuccesssful = 1;
				goto GET;
		}

		//----------------------------------------------------------------------------------
		if (s2 <= maxsize)
		{
			float r, c, dr, dc, s;
			s = s2;

			dr = dc = MAX(stridefactor*s, 1.0f);

			for (r = rectROI.y; r <= nrows - s / 2 - 1 - (nrows - rectROI.y - rectROI.height); r += dr)
			for (c = rectROI.x; c <= ncols - s / 2 - 1 - (ncols - rectROI.x - rectROI.width); c += dc)
			{
				float q;
				int t;

				if (run_detection_cascade(&q, r, c, s, pixels, nrows, ncols, ldim, myFaceDetector) == 1)
				{
					if (ndetections < maxndetections)
					{
						qs[ndetections] = q;
						rs[ndetections] = r;
						cs[ndetections] = c;
						ss[ndetections] = s;

						++ndetections;
					}
				}
			}
			bLargeScaleDetected = 1;

			s2 = s2*scalefactor;

			countLarge++;
		}

		if (ndetections >= FIND_FACES_NUM_BREAK || ((countSmall + countLarge) >= 4 && bFullImage == -1))
		{	
				bGotFaceSuccesssful = 1;
				goto GET;
		}

	}

GET:
	return ndetections;
}

float get_overlap(float r1, float c1, float s1, float r2, float c2, float s2)
{
	float overr, overc;

	overr = MAX(0, MIN(r1 + s1 / 2, r2 + s2 / 2) - MAX(r1 - s1 / 2, r2 - s2 / 2));
	overc = MAX(0, MIN(c1 + s1 / 2, c2 + s2 / 2) - MAX(c1 - s1 / 2, c2 - s2 / 2));

	return overr*overc / (s1*s1 + s2*s2 - overr*overc);
}

void ccdfs(int a[], int i, float rs[], float cs[], float ss[], int n)
{
	int j;

	for (j = 0; j<n; ++j)
	if (a[j] == 0 && get_overlap(rs[i], cs[i], ss[i], rs[j], cs[j], ss[j])>0.3f)
	{

		a[j] = a[i];

		ccdfs(a, j, rs, cs, ss, n);
	}
}

int find_connected_components(int a[], float rs[], float cs[], float ss[], int n)
{
	int i, ncc, cc;

	if (!n)
		return 0;

	for (i = 0; i<n; ++i)
		a[i] = 0;

	ncc = 0;
	cc = 1;

	for (i = 0; i<n; ++i)
	if (a[i] == 0)
	{
		a[i] = cc;

		ccdfs(a, i, rs, cs, ss, n);

		++ncc;
		++cc;
	}

	return ncc;
}

int cluster_detections(float rs[], float cs[], float ss[], float qs[], int n)
{
	int idx, ncc, cc;
	int a[4096];
	ncc = find_connected_components(a, rs, cs, ss, n);
	if (!ncc)
		return 0;
	idx = 0;

	for (cc = 1; cc <= ncc; ++cc)
	{
		int i, k;

		float sumqs = 0.0f, sumrs = 0.0f, sumcs = 0.0f, sumss = 0.0f;

		k = 0;

		for (i = 0; i<n; ++i)
			if (a[i] == cc)
			{
				sumqs += qs[i];
				sumrs += rs[i];
				sumcs += cs[i];
				sumss += ss[i];

				++k;
			}

		qs[idx] = sumqs; // accumulated confidence measure

		rs[idx] = sumrs / k;
		cs[idx] = sumcs / k;
		ss[idx] = sumss / k;

		++idx;
	}

	return idx;
}

int run_facefinder(float* o, int r, int c, int s, void* vppixels, int nrows, int ncols, int ldim, faceDetector *myFaceDetector)
{
	int i, idx, sr, sc;
	unsigned char* pixels;
	short int *tcodes;
	int nDim;
	//	float *p_m_luts;
	sr = (int)(1.000000f*s);
	sc = (int)(1.000000f*s);

	r = r * 256;
	c = c * 256;

	if ((r + 128 * sr) / 256 >= nrows || (r - 128 * sr) / 256<0 || (c + 128 * sc) / 256 >= ncols || (c - 128 * sc) / 256<0)
		return -1;

	pixels = (unsigned char*)vppixels;

	*o = 0.0f;
	tcodes = myFaceDetector->m_rtcodes;
	nDim = myFaceDetector->m_dim;

	for (i = 0; i<myFaceDetector->m_ntrees; ++i)
	{
		idx = 1;
		idx = 2 * idx + (pixels[(r + (*(tcodes + i*nDim * 4 + idx * 4 + 0))*sr) / 256 * ldim + (c + (*(tcodes + i*nDim * 4 + idx * 4 + 1))*sc) / 256] <= pixels[(r + (*(tcodes + i*nDim * 4 + idx * 4 + 2))*sr) / 256 * ldim + (c + (*(tcodes + i*nDim * 4 + idx * 4 + 3))*sc) / 256]);
		idx = 2 * idx + (pixels[(r + (*(tcodes + i*nDim * 4 + idx * 4 + 0))*sr) / 256 * ldim + (c + (*(tcodes + i*nDim * 4 + idx * 4 + 1))*sc) / 256] <= pixels[(r + (*(tcodes + i*nDim * 4 + idx * 4 + 2))*sr) / 256 * ldim + (c + (*(tcodes + i*nDim * 4 + idx * 4 + 3))*sc) / 256]);
		idx = 2 * idx + (pixels[(r + (*(tcodes + i*nDim * 4 + idx * 4 + 0))*sr) / 256 * ldim + (c + (*(tcodes + i*nDim * 4 + idx * 4 + 1))*sc) / 256] <= pixels[(r + (*(tcodes + i*nDim * 4 + idx * 4 + 2))*sr) / 256 * ldim + (c + (*(tcodes + i*nDim * 4 + idx * 4 + 3))*sc) / 256]);
		idx = 2 * idx + (pixels[(r + (*(tcodes + i*nDim * 4 + idx * 4 + 0))*sr) / 256 * ldim + (c + (*(tcodes + i*nDim * 4 + idx * 4 + 1))*sc) / 256] <= pixels[(r + (*(tcodes + i*nDim * 4 + idx * 4 + 2))*sr) / 256 * ldim + (c + (*(tcodes + i*nDim * 4 + idx * 4 + 3))*sc) / 256]);
		idx = 2 * idx + (pixels[(r + (*(tcodes + i*nDim * 4 + idx * 4 + 0))*sr) / 256 * ldim + (c + (*(tcodes + i*nDim * 4 + idx * 4 + 1))*sc) / 256] <= pixels[(r + (*(tcodes + i*nDim * 4 + idx * 4 + 2))*sr) / 256 * ldim + (c + (*(tcodes + i*nDim * 4 + idx * 4 + 3))*sc) / 256]);

		*o = *o + *(myFaceDetector->m_luts + i*nDim + idx - 32);

		if (*o <= *(myFaceDetector->m_thresholds + i))
			return -1;
	}

	*o = *o - *(myFaceDetector->m_thresholds + myFaceDetector->m_ntrees - 1);

	return +1;
}

short txFaceDetector(faceDetector *myFaceDetector, TxImage* imgInput, TxRect *pRect, int *numberFace, int minsize, int maxsize, float qthreshold /*= 5.0f;*/, float scalefactor /*= 1.1f;*/,
	float stridefactor, TxRect rectROI, short bFullImage, int faceWidth)
{
	unsigned char* pixels;
	int i, j;
	int nrows, ncols, ldim;
	int ndetections;
	float t;
	float qs[MAX_FACE_DETECTION_RESULT] = { 0 }, rs[MAX_FACE_DETECTION_RESULT] = { 0 }, cs[MAX_FACE_DETECTION_RESULT] = { 0 }, ss[MAX_FACE_DETECTION_RESULT] = { 0 };

	int(*run_detection_cascade)(float*, int, int, int, void*, int, int, int, faceDetector *)
		= run_facefinder;

	if (imgInput == NULL)
		return -1;

	{
		pixels = (unsigned char*)imgInput->data;
		nrows = imgInput->rows;
		ncols = imgInput->cols;
		ldim = imgInput->cols;

		ndetections = find_objects(rs, cs, ss, qs, MAX_FACE_DETECTION_RESULT, run_detection_cascade, pixels, nrows, ncols, ldim, scalefactor, stridefactor, minsize, maxsize, rectROI, myFaceDetector, bFullImage, faceWidth);
	}

	ndetections = cluster_detections(rs, cs, ss, qs, ndetections);


	if (ndetections > MAX_FACE_DETECTION_NUM)
		return -1;
	else
	{
		for (i = 0; i<ndetections; ++i)
		if (qs[i] >= qthreshold) // check the confidence threshold
		{

			(pRect + (*numberFace))->x = (int)cs[i] - ss[i] / 2;
			(pRect + (*numberFace))->y = (int)rs[i] - ss[i] / 2;
			(pRect + (*numberFace))->width = (int)ss[i];
			(pRect + (*numberFace))->height = (int)ss[i];

			(*numberFace)++;
		}
	}

	return 1;
}

short txFaceDetector_fullImage(faceDetector *myFaceDetector, TxImage* imgInput, TxRect rectROI, TxRect faceROI_reference, TxRect *pRectFaceOut, int *numberFace, fatigueDetectionPara *myFatigueDetectionPara,int faceWidthFullDetect)
{
	short rDetector;
	
	if (rectROI.width == imgInput->cols - 1)
	{
		rDetector = txFaceDetector(myFaceDetector, imgInput, pRectFaceOut, numberFace, myFatigueDetectionPara->face_min_size_detect, myFatigueDetectionPara->face_max_size_detect,6.5f, 1.25f, 0.07f, 
									rectROI, 1, (myFatigueDetectionPara->face_min_size_detect + myFatigueDetectionPara->face_max_size_detect) / 2);
	}
	else
	{
		if (faceROI_reference.width != 0)
		{
			int faceWidthMax = faceROI_reference.width + myFatigueDetectionPara->face_size_offset >= myFatigueDetectionPara->face_max_size_detect ? 
								myFatigueDetectionPara->face_max_size_detect : faceROI_reference.width + myFatigueDetectionPara->face_size_offset;
			
			int faceWidthMin = faceROI_reference.width - myFatigueDetectionPara->face_size_offset <= myFatigueDetectionPara->face_min_size_detect ? 
								myFatigueDetectionPara->face_min_size_detect : faceROI_reference.width - myFatigueDetectionPara->face_size_offset;

			rDetector = txFaceDetector(myFaceDetector, imgInput, pRectFaceOut, numberFace, faceWidthMin, faceWidthMax, 5.0f, 1.10f, 0.07f, rectROI, -1, faceROI_reference.width);
		}
		else
		{
			if (faceWidthFullDetect == 0)
				rDetector = txFaceDetector(myFaceDetector, imgInput, pRectFaceOut, numberFace, myFatigueDetectionPara->face_min_size_detect, myFatigueDetectionPara->face_max_size_detect, 
											5.0f, 1.2f, 0.09f, rectROI, 1, (myFatigueDetectionPara->face_min_size_detect + myFatigueDetectionPara->face_max_size_detect) / 2);
			else
				rDetector = txFaceDetector(myFaceDetector, imgInput, pRectFaceOut, numberFace, myFatigueDetectionPara->face_min_size_detect, myFatigueDetectionPara->face_max_size_detect, 
											5.0f, 1.1f, 0.07f, rectROI, 1, faceWidthFullDetect);
		}
	}
	return rDetector;
}