#include "kalman.h"
#include "core.h"
#include "string.h"
#include "matrix.h"

/* Creates Kalman filter and sets A, B, Q, R and state to some initial values */
TxKalman* txCreateKalman( int DP, int MP, int CP)
{
	TxKalman *kalman = 0;

	if( DP <= 0 || MP <= 0 )
	{
		return NULL;
	};

	if( CP < 0 )
		CP = DP;

	/* allocating memory for the structure */
	kalman = (TxKalman *)txAlloc( sizeof( TxKalman ));
	memset( kalman, 0, sizeof(*kalman));

	kalman->DP = DP;
	kalman->MP = MP;
	kalman->CP = CP;

	kalman->statePre = txCreateMat( DP, 1);
	txSetZero( kalman->statePre );

	kalman->statePost = txCreateMat( DP, 1);
	txSetZero( kalman->statePost );

	kalman->transitionMatrix = txCreateMat( DP, DP);
	txSetIdentity( kalman->transitionMatrix , 1);

	kalman->processNoiseCov = txCreateMat( DP, DP);
	txSetIdentity( kalman->processNoiseCov , 1);

	kalman->measurementMatrix = txCreateMat( MP, DP);
	txSetZero( kalman->measurementMatrix );

	kalman->measurementNoiseCov = txCreateMat( MP, MP);
	txSetIdentity( kalman->measurementNoiseCov , 1);

	kalman->errorCovPre = txCreateMat( DP, DP);

	kalman->errorCovPost = txCreateMat( DP, DP);
	txSetZero( kalman->errorCovPost );

	kalman->gain = txCreateMat( DP, MP);

	if( CP > 0 )
	{
		kalman->controlMatrix = txCreateMat( DP, CP);
		txSetZero( kalman->controlMatrix );
	}

	kalman->temp1 = txCreateMat( DP, DP);
	kalman->temp2 = txCreateMat( MP, DP);
	kalman->temp3 = txCreateMat( MP, MP);
	kalman->temp4 = txCreateMat( MP, DP);
	kalman->temp5 = txCreateMat( MP, 1);

	//used for tranpose mat
	kalman->temp6 = txCreateMat( DP, DP);
	kalman->temp7 = txCreateMat( DP, MP);
	kalman->temp8 = txCreateMat( MP, MP);
	kalman->temp9 = txCreateMat( DP, 1);

	return kalman;
}

/* Releases Kalman filter state */
void  txReleaseKalman( TxKalman** _kalman)
{
	TxKalman *kalman;

	if( !_kalman )
	{
		return;
	};

	kalman = *_kalman;
	if( !kalman )
		return;

	/* freeing the memory */
	txReleaseMat( &kalman->statePre );
	txReleaseMat( &kalman->statePost );
	txReleaseMat( &kalman->transitionMatrix );
	txReleaseMat( &kalman->controlMatrix );
	txReleaseMat( &kalman->measurementMatrix );
	txReleaseMat( &kalman->processNoiseCov );
	txReleaseMat( &kalman->measurementNoiseCov );
	txReleaseMat( &kalman->errorCovPre );
	txReleaseMat( &kalman->gain );
	txReleaseMat( &kalman->errorCovPost );
	txReleaseMat( &kalman->temp1 );
	txReleaseMat( &kalman->temp2 );
	txReleaseMat( &kalman->temp3 );
	txReleaseMat( &kalman->temp4 );
	txReleaseMat( &kalman->temp5 );
	txReleaseMat( &kalman->temp6 );
	txReleaseMat( &kalman->temp7 );
	txReleaseMat( &kalman->temp8 );
	txReleaseMat( &kalman->temp9 );
	/* deallocating the structure */
	txFree(kalman);
	*_kalman = NULL;
}

/* Updates Kalman filter by time (predicts future state of the system) */
const TxMat*  txKalmanPredict( TxKalman* kalman,
                                      const TxMat* control)
{
	if( !kalman )
	{
		return NULL;
	}

	/* update the state */
	/* x'(k) = A*x(k) */
	txMatMul(kalman->transitionMatrix, kalman->statePost, kalman->statePre);

	if( control && kalman->CP > 0 )
	{
		/* x'(k) = x'(k) + B*u(k) */
		txMatMul(kalman->controlMatrix, control, kalman->temp9);
		txMatAdd(kalman->statePre,kalman->temp9,kalman->statePre);
	}

	/* update error covariance matrices */
	/* temp1 = A*P(k) */
	txMatMul( kalman->transitionMatrix, kalman->errorCovPost, kalman->temp1 );

	/* P'(k) = temp1*At + Q */
	txTranspose(kalman->transitionMatrix,kalman->temp6);
	txMatMul( kalman->temp1, kalman->temp6, kalman->errorCovPre);
	txMatAdd(kalman->errorCovPre,kalman->processNoiseCov,kalman->errorCovPre);

	/* handle the case when there will be measurement before the next predict */
	txCopyMat(kalman->statePre, kalman->statePost);

	return kalman->statePre;
}

/* Updates Kalman filter by measurement
   (corrects state of the system and internal matrices) */
const TxMat*  txKalmanCorrect( TxKalman* kalman, const TxMat* measurement )
{
	if( !kalman || !measurement )
	{
		return NULL;
	}

	/* temp2 = H*P'(k) */
	txMatMul(kalman->measurementMatrix, kalman->errorCovPre, kalman->temp2 );

	/* temp3 = temp2*Ht + R */
	txTranspose(kalman->measurementMatrix,kalman->temp7);
	txMatMul(kalman->temp2, kalman->temp7,kalman->temp3);
	txMatAdd(kalman->temp3, kalman->measurementNoiseCov,kalman->temp3);

	/* temp4 = inv(temp3)*temp2 = Kt(k) */
	txInvert(kalman->temp3, kalman->temp8);
	txMatMul(kalman->temp8, kalman->temp2,kalman->temp4);

	/* K(k) */
	txTranspose(kalman->temp4, kalman->gain);

	/* temp5 = z(k) - H*x'(k) */
	txMatMul(kalman->measurementMatrix, kalman->statePre,kalman->temp5);
	txMatSub(measurement,kalman->temp5,kalman->temp5);

	/* x(k) = x'(k) + K(k)*temp5 */
	txMatMul(kalman->gain, kalman->temp5,kalman->statePost);
	txMatAdd(kalman->statePre,kalman->statePost,kalman->statePost);

	/* P(k) = P'(k) - K(k)*temp2 */
	txMatMul(kalman->gain, kalman->temp2,kalman->errorCovPost);
	txMatSub(kalman->errorCovPre,kalman->errorCovPost,kalman->errorCovPost);

	return kalman->statePost;
}