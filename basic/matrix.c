#include "matrix.h"
#include "string.h"
#include "core.h"

#define N 10

TxMat* txCreateMat( int rows, int cols)
{
	TxMat *m;
	int s;
	if(rows <= 0 || cols <= 0)
	{
		return 0;
	}
	s = sizeof(REAL)*cols*rows ;
	m = (TxMat *)txAlloc(sizeof(TxMat));
	m->cols = cols;
	m->rows = rows;
	m->data = (REAL*)txAlloc(s);
	memset(m->data,0,s);
	return m;
}
TxMati* txCreateMati( int rows, int cols)
{
	TxMati *m;
	int s;
	if(rows <= 0 || cols <= 0)
	{
		return 0;
	}
	s = sizeof(int)*cols*rows ;
	m = (TxMati *)txAlloc(sizeof(TxMati));
	m->cols = cols;
	m->rows = rows;
	m->data = (int*)txAlloc(s);
	memset(m->data,0,s);
	return m;
}
void  txReleaseMat( TxMat** _mat )
{
	TxMat * mat;
	if (!_mat)
	{
		return;
	}
	mat = *_mat;
	if(mat == NULL || mat->data == NULL)
	{
		return;
	}
	txFree(mat->data);
	txFree(mat);

	*_mat = NULL;
}
void  txReleaseMati( TxMati** _mat )
{
	TxMati * mat;
	if (!_mat)
	{
		return;
	}
	mat = *_mat;
	if(mat == NULL || mat->data == NULL)
	{
		return;
	}
	txFree(mat->data);
	txFree(mat);

	*_mat = NULL;
}
void  txSetZero( TxMat* mat )
{
	memset(mat->data, 0, sizeof(REAL) * mat->cols * mat->rows);
}
void  txSetIdentity( TxMat* mat, REAL val)
{
	int i,j;
	REAL* data = mat->data;
	for (i=0; i<mat->rows; i++,data += mat->cols)
	{
		for (j=0; j<mat->cols; j++)
		{
			data[j] = 0;
		}
		if(i < mat->cols) data[i] = val; 
	}
}
void  txCopyMat(const TxMat* src, TxMat* dst)
{
	if (dst->rows == src->rows && dst->cols == src->rows)
	{
		memcpy(dst->data,src->data,sizeof(REAL) * src->cols * src->rows);
	}
	
}
TxMat* txCloneMat( const TxMat* mat )
{
	TxMat * m = txCreateMat(mat->rows, mat->cols);

	memcpy(m->data,mat->data,sizeof(REAL) * mat->cols * mat->rows);
	return m;
}

int txMatMul(const TxMat*src1,const TxMat*src2,TxMat*dst)
{
	int i, j,k;
	REAL s;
	int min = src1->cols < src2->rows ? src1->cols : src2->rows;
	if(dst->rows != src1->rows ||dst->cols!= src2->cols)
	{
		return 0;
	}
	for (i = 0; i < src1->rows; i++)
	{
		for (j = 0; j < src2->cols; j++)
		{
			s = 0;
			for (k = 0; k < min; k++)
			{
				s += src1->data[src1->cols * i + k] * src2->data[src2->cols * k + j];
			}
			dst->data[src2->cols*i + j] = s;
		}
	}
	return 1;
}
int txMatMulAdd(const TxMat*src1,const TxMat*src2,const TxMat*srcAdd,TxMat*dst)
{
	int i, j,k;
	REAL s;
	int min = src1->cols < src2->rows ? src1->cols : src2->rows;
	if(dst->rows != src1->rows ||dst->cols!= src2->cols ||dst->rows!=srcAdd->rows || dst->cols!=srcAdd->cols)
	{
		return 0;
	}
	for (i = 0; i < src1->rows; i++)
	{
		for (j = 0; j < src2->cols; j++)
		{
			s = 0;
			for (k = 0; k < min; k++)
			{
				s += src1->data[src1->cols * i + k] * src2->data[src2->cols * k + j];
			}
			dst->data[src2->cols*i + j] = s + srcAdd->data[src2->cols*i + j];
		}
	}
	return 1;
}
int txMatAdd(const TxMat*src1,const TxMat*src2,TxMat*dst)
{
	int i,j,k;
	if (dst->rows!=src1->rows || src2->rows!=src1->rows
		||dst->cols!=src1->cols || src2->cols!=src1->cols)
	{
		return 0;
	}
	k = 0;
	for (i=0; i<src1->rows; i++)
	{
		for (j=0; j< src1->cols; j++)
		{
			dst->data[k]=src1->data[k]+src2->data[k];
			k++;
		}
	}

	return 1;
}
int txMatSub(const TxMat*src1,const TxMat*src2,TxMat*dst)
{
	int i,j,k;
	if (dst->rows!=src1->rows || src2->rows!=src1->rows
		||dst->cols!=src1->cols || src2->cols!=src1->cols)
	{
		return 0;
	}
	k = 0;
	for (i=0; i<src1->rows; i++)
	{
		for (j=0; j< src1->cols; j++)
		{
			dst->data[k]=src1->data[k]-src2->data[k];
			k++;
		}
	}

	return 1;
}
int txTranspose(const TxMat* src, TxMat*dst)
{
	int i,j;
	if (dst->rows!=src->cols ||dst->cols!=src->rows)
	{
		return 0;
	}
	for (i=0; i<src->rows; i++)
	{
		for (j=0; j< src->cols; j++)
		{
			dst->data[dst->cols*j +i]=src->data[src->cols*i + j];
		}
	}

	return 1;
}


REAL getA(REAL arcs[N][N],int n)//按第一行展开计算|A|
{

	REAL ans = 0,t = 0;
	REAL temp[N][N];
	int i,j,k;

	if(n==1)
	{
		return arcs[0][0];
	}

	for(i=0;i<n;i++)
	{
		for(j=0;j<n-1;j++)
		{
			for(k=0;k<n-1;k++)
			{
				temp[j][k] = arcs[j+1][(k>=i)?k+1:k];

			}
		}
		t = getA(temp,n-1);
		if(i%2==0)
		{
			ans += arcs[0][i]*t;
		}
		else
		{
			ans -=  arcs[0][i]*t;
		}
	}
	return ans;
}

void getAStart(REAL arcs[N][N],int n,REAL ans[N][N])//计算每一行每一列的每个元素所对应的余子式，组成A*
{

	int i,j,k,t;
	REAL temp[N][N];
	if(n==1)
	{
		ans[0][0] = 1;
		return;
	}

	for(i=0;i<n;i++)
	{
		for(j=0;j<n;j++)
		{
			for(k=0;k<n-1;k++)
			{
				for(t=0;t<n-1;t++)
				{
					temp[k][t] = arcs[k>=i?k+1:k][t>=j?t+1:t];
				}
			}


			ans[j][i]  =  getA(temp,n-1);
			if((i+j)%2 == 1)
			{
				ans[j][i] = - ans[j][i];
			}
		}
	}
}

//Inverts matrix 
double  txInvert( const TxMat* src, TxMat* dst)
{
	int i,j;
	int n = src->rows;
	REAL a=0;
	REAL input[N][N];
	REAL outputTmp[N][N];
	if(src->rows != src->cols)
	{
		return 0.0;
	}
	if(src->rows == 1)
	{
		*dst->data = *src->data;
		return *dst->data;
	}

	for(i = 0; i < src->rows; i++)
	{
		for(j = 0; j < src->cols; j++)
		{
			input[i][j] = src->data[i*src->cols + j];
		}
	}
	
	a = getA(input,n);
	if(a==0)
	{
		return a;
	}
	else
	{
		getAStart(input,n,outputTmp);
		for(i=0;i<n;i++)
		{
			for(j=0;j<n;j++)
			{
				dst->data[i*n + j] = outputTmp[i][j]/a;
			}
		}
	}
	dst->rows = src->rows; dst->cols = dst->cols;
	return a;
}