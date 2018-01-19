#pragma once
#ifdef __cplusplus
extern "C" {
#endif

typedef float REAL;

typedef struct TxMat
{
	int rows;
	int cols;
	REAL* data;
} 
TxMat;

typedef struct TxMati
{
	int rows;
	int cols;
	int* data;
} 
TxMati;


TxMat* txCreateMat( int rows, int cols);
void  txReleaseMat( TxMat** _mat );
TxMati* txCreateMati( int rows, int cols);
void  txReleaseMati( TxMati** _mat );
void  txSetZero( TxMat* mat );
void  txSetIdentity( TxMat* mat, REAL val);
void  txCopyMat(const TxMat* src, TxMat* dst);
TxMat* txCloneMat( const TxMat* mat );

int txMatMul(const TxMat*src1,const TxMat*src2,TxMat*dst);
int txMatAdd(const TxMat*src1,const TxMat*src2,TxMat*dst);
int txMatMulAdd(const TxMat*src1,const TxMat*src2,const TxMat*srcAdd,TxMat*dst);
int txMatSub(const TxMat*src1,const TxMat*src2,TxMat*dst);
int txTranspose(const TxMat* src, TxMat*dst);

 //Inverts matrix 
double  txInvert( const TxMat* src, TxMat* dst);


#ifdef __cplusplus
}
#endif
